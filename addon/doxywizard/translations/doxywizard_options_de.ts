<?xml version='1.0' encoding='utf-8'?>
<TS version="2.1" language="de">
<context>
    <name>Expert</name>
<message>
        <source>Possible values are:</source>
        <translation>Mögliche Werte sind:</translation>
    </message>
<message>
        <source>and</source>
        <translation>und</translation>
    </message>
<message>
        <source>The default value is: &lt;code&gt;%1&lt;/code&gt;.</source>
        <translation>Der Standardwert ist: &lt;code&gt;%1&lt;/code&gt;.</translation>
    </message>
<message>
        <source>Minimum value: %1, maximum value: %2, default value: %3.</source>
        <translation>Mindestwert: %1, Höchstwert: %2, Standardwert: %3.</translation>
    </message>
<message>
        <source>The default value is: system dependent.</source>
        <translation>Der Standardwert ist: systemabhängig.</translation>
    </message>
<message>
        <source>The default directory is: &lt;code&gt;%1&lt;/code&gt;.</source>
        <translation>Das Standardverzeichnis ist: &lt;code&gt;%1&lt;/code&gt;.</translation>
    </message>
<message>
        <source>The default file is: &lt;code&gt;%1&lt;/code&gt;.</source>
        <translation>Die Standarddatei ist: &lt;code&gt;%1&lt;/code&gt;.</translation>
    </message>
<message>
        <source>The default file (with absolute path) is: &lt;code&gt;%1&lt;/code&gt;.</source>
        <translation>Die Standarddatei (mit absolutem Pfad) ist: &lt;code&gt;%1&lt;/code&gt;.</translation>
    </message>
<message>
        <source>The file has to be specified with full path.</source>
        <translation>Die Datei muss mit vollem Pfad angegeben werden.</translation>
    </message>
<message>
        <source>The default image is: &lt;code&gt;%1&lt;/code&gt;.</source>
        <translation>Das Standardbild ist: &lt;code&gt;%1&lt;/code&gt;.</translation>
    </message>
<message>
        <source>The default image (with absolute path) is: &lt;code&gt;%1&lt;/code&gt;.</source>
        <translation>Das Standardbild (mit absolutem Pfad) ist: &lt;code&gt;%1&lt;/code&gt;.</translation>
    </message>
<message>
        <source>The image has to be specified with full path.</source>
        <translation>Das Bild muss mit vollem Pfad angegeben werden.</translation>
    </message>
<message>
        <source>This tag requires that the tag %1 is set to &lt;code&gt;YES&lt;/code&gt;.</source>
        <translation>Dieses Tag erfordert, dass das Tag %1 auf &lt;code&gt;YES&lt;/code&gt; gesetzt ist.</translation>
    </message>
</context>
<context>
    <name>OptionDocs</name>
<message>
        <source>DOXYFILE_ENCODING</source>
            <translation>Dieses Tag gibt die Codierung fur alle Zeichen in der Konfigurationsdatei an, die folgen. Der Standardwert ist UTF-8, was auch die Codierung fur den gesamten Text vor dem ersten Auftreten dieses Tags ist. Doxygen verwendet \c libiconv (oder das in \c libc eingebaute iconv) fur die Transkodierung. Siehe https://www.gnu.org/software/libiconv/ fur die Liste moglicher Codierungen.</translation>
    </message>
<message>
        <source>ABBREVIATE_BRIEF</source>
            <translation>Dieses Tag implementiert einen quasi-intelligenten Kurzbeschreibungs-Abkurzer, der verwendet wird, um den Text in verschiedenen Auflistungen zu bilden. Jede Zeichenfolge in dieser Liste, wenn sie als fuhrender Text der Kurzbeschreibung gefunden wird, wird vom Text entfernt, und das Ergebnis nach der Verarbeitung der gesamten Liste wird als kommentierter Text verwendet. Andernfalls wird die Kurzbeschreibung wie angegeben verwendet. Wenn leer gelassen, werden die folgenden Werte verwendet (`$name` wird automatisch durch den Namen der Entitat ersetzt):</translation>
    </message>
<message>
        <source>ALIASES</source>
            <translation>Dieses Tag kann verwendet werden, um eine Reihe von Aliasnamen anzugeben, die als Befehle in der Dokumentation fungieren. Ein Alias hat die Form: \verbatim name=wert\endverbatim Zum Beispiel wurde das Hinzufugen von \verbatim "sideeffect=@par Nebeneffekte:^^"\endverbatim es Ihnen ermoglichen, den Befehl \c \sideeffect (oder \c \@sideeffect) in der Dokumentation zu platzieren, was einen benutzerdefinierten Absatz mit der Uberschrift "Nebeneffekte:" erzeugt. Beachten Sie, dass Sie im Werteteil eines Alias kein \ref cmdn "\n" einfugen konnen, um einen Zeilenumbruch einzufugen (in der resultierenden Ausgabe). Sie konnen `^^` im Werteteil eines Alias einfugen, um einen Zeilenumbruch einzufugen, als ob ein physikalischer Zeilenumbruch in der Originaldatei vorhanden ware. Wenn Sie ein literales `{` oder `}` oder `,` im Werteteil eines Alias benotigen, mussen Sie diese mit einem Backslash maskieren (\c \\).</translation>
    </message>
<message>
        <source>ALLEXTERNALS</source>
            <translation>Wenn das \c ALLEXTERNALS-Tag auf \c YES gesetzt ist, werden alle externen Klassen und Namensraume im Klassen- und Namensraumindex aufgelistet. Wenn auf \c NO gesetzt, werden nur die geerbten externen Klassen aufgelistet.</translation>
    </message>
<message>
        <source>ALLOW_UNICODE_NAMES</source>
            <translation>Wenn das \c ALLOW_UNICODE_NAMES-Tag auf \c YES gesetzt ist, erlaubt Doxygen Nicht-ASCII-Zeichen in den Namen der generierten Dateien. Wenn auf \c NO gesetzt, werden Nicht-ASCII-Zeichen maskiert, zum Beispiel wird _xE3_x81_x84 fur Unicode U+3044 verwendet.</translation>
    </message>
<message>
        <source>ALPHABETICAL_INDEX</source>
            <translation>Wenn das \c ALPHABETICAL_INDEX-Tag auf \c YES gesetzt ist, wird ein alphabetischer Index aller Verbindungen generiert. Wenn Sie die \ref cfg_extract_all "EXTRACT_ALL" und \ref cfg_extract_private "EXTRACT_PRIVATE" Tags auf \c YES setzen, wird auch der Index aller internen Elemente generiert. Wenn Sie dies nicht mochten, konnen Sie \ref cfg_ignore_prefix "IGNORE_PREFIX" auf einen nicht-leeren Wert setzen.</translation>
    </message>
<message>
        <source>ALWAYS_DETAILED_SEC</source>
            <translation>Wenn sowohl die Tags \c ALWAYS_DETAILED_SEC als auch \ref cfg_repeat_brief "REPEAT_BRIEF" auf \c YES gesetzt sind, generiert Doxygen einen detaillierten Abschnitt, auch wenn es nur eine Kurzbeschreibung gibt.</translation>
    </message>
<message>
        <source>AUTOLINK_IGNORE_WORDS</source>
            <translation>Dieses Tag gibt eine Liste von Wörtern an, die, wenn sie mit dem Anfang eines Wortes in der Dokumentation ubereinstimmen, die automatische Verknupfungsgenerierung unterdrucken, wenn diese durch AUTOLINK_SUPPORT aktiviert ist. Diese Liste beeinflusst keine explizit mit \# oder den Befehlen \ref cmdlink "\\link" oder \ref cmdref "\\ref" erstellten Links.</translation>
    </message>
<message>
        <source>AUTOLINK_SUPPORT</source>
            <translation>Wenn aktiviert, versucht Doxygen, Wörter, die einer dokumentierten Klasse oder einem Namespace entsprechen, mit der entsprechenden Dokumentation zu verknupfen. Dies kann in Einzelfallen verhindert werden, indem ein \c %-Symbol vor das Wort gesetzt wird, oder global, indem \c AUTOLINK_SUPPORT auf \c NO gesetzt wird. Wörter, die im \c AUTOLINK_IGNORE_WORDS-Tag aufgelistet sind, werden von der automatischen Verknupfung ausgeschlossen.</translation>
    </message>
<message>
        <source>BINARY_TOC</source>
            <translation>Das \c BINARY_TOC-Flag steuert, ob ein binaires Inhaltsverzeichnis generiert wird. Wenn es auf \c YES gesetzt ist, wird ein binaires Inhaltsverzeichnis erstellt, was zu schnellerer Suche und kleineren Dateien fuhrt. Das Standard-Inhaltsverzeichnis wird in einer Textdatei gespeichert.</translation>
    </message>
<message>
        <source>BRIEF_MEMBER_DESC</source>
            <translation>Wenn das \c BRIEF_MEMBER_DESC-Tag auf \c YES gesetzt ist, wird Doxygen kurze Mitgliederbeschreibungen nach den Mitgliedern einfugen, die in der Datei- und Klassendokumentation aufgefuhrt sind (ahnlich wie \c Javadoc). Auf \c NO setzen, um dies zu deaktivieren.</translation>
    </message>
<message>
        <source>BUILTIN_STL_SUPPORT</source>
            <translation>Wenn Sie STL-Klassen verwenden (d.h. `std::string`, `std::vector` usw.), aber nicht den (Tag-Datei-)Quellcode der STL als Eingabe einbinden mochten, sollten Sie dieses Tag auf \c YES setzen, damit Doxygen Funktionsdeklarationen und -definitionen mit Argumenten, die STL-Klassen enthalten, abgleichen kann (z.B. `func(std::string`); gegenuber `func(std::string) {}`). Dies macht auch die Vererbungs- und Kollaborationsdiagramme, die STL-Klassen beinhalten, vollstandiger und genauer.</translation>
    </message>
<message>
        <source>CALLER_GRAPH</source>
            <translation>Wenn das \c CALLER_GRAPH-Tag auf \c YES gesetzt ist, generiert Doxygen einen Aufruferabhangigkeitsgraphen fur jede globale Funktion oder Klassenmethode. Beachten Sie, dass die Aktivierung dieses Tags die Einstellung \ref cfg_have_dot "HAVE_DOT" auf \c YES voraussetzt. Das explizite Aktivieren eines Aufrufergraphen, wenn \c CALLER_GRAPH auf \c NO gesetzt ist, kann mit dem Befehl \ref cmdcallergraph "\\callergraph" erreicht werden. Das Deaktivieren eines Aufrufergraphen kann mit dem Befehl \ref cmdhidecallergraph "\\hidecallergraph" erreicht werden.</translation>
    </message>
<message>
        <source>CALL_GRAPH</source>
            <translation>Wenn das \c CALL_GRAPH-Tag auf \c YES gesetzt ist, generiert Doxygen einen Aufrufabhangigkeitsgraphen fur jede globale Funktion oder Klassenmethode. Beachten Sie, dass die Aktivierung dieses Tags die Einstellung \ref cfg_have_dot "HAVE_DOT" auf \c YES voraussetzt. Das explizite Aktivieren eines Aufrufgraphen, wenn \c CALL_GRAPH auf \c NO gesetzt ist, kann mit dem Befehl \ref cmdcallgraph "\\callgraph" erreicht werden. Das Deaktivieren eines Aufrufgraphen kann mit dem Befehl \ref cmdhidecallgraph "\\hidecallgraph" erreicht werden.</translation>
    </message>
<message>
        <source>CASE_SENSE_NAMES</source>
            <translation>Mit der korrekten Einstellung der Option \c CASE_SENSE_NAMES wird Doxygen die Fahigkeiten des zugrunde liegenden Dateisystems besser nutzen konnen. Im Fall eines Dateisystems, das zwischen Grob- und Kleinschreibung unterscheidet (d.h. es unterstutzt Dateien im selben Verzeichnis, deren Namen sich nur in der Grob-/Kleinschreibung unterscheiden), muss die Option auf \c YES gesetzt werden, um solche Dateien ordnungsgema zu behandeln, falls sie in der Eingabe erscheinen. Fur Dateisysteme, die nicht zwischen Grob- und Kleinschreibung unterscheiden, sollte die Option auf \c NO gesetzt werden, um Ausgabedateien fur Symbole zu behandeln, die sich nur in der Grob-/Kleinschreibung unterscheiden. Auf Windows (einschließlich Cygwin) und macOS sollten Benutzer diese Option typischerweise auf \c NO setzen, wahrend auf Linux oder anderen Unix-Varianten sie typischerweise auf \c YES gesetzt werden sollte.</translation>
    </message>
<message>
        <source>CHM_FILE</source>
            <translation>Das \c CHM_FILE-Tag kann verwendet werden, um den Dateinamen der resultierenden `.chm`-Datei anzugeben. Ein relativer Pfad wird relativ zum \ref cfg_html_output "HTML_OUTPUT"-Verzeichnis interpretiert. Wenn leer gelassen, wird der Standardwert `index.chm` verwendet.</translation>
    </message>
<message>
        <source>CHM_INDEX_ENCODING</source>
            <translation>Das \c CHM_INDEX_ENCODING wird verwendet, um HtmlHelp-Index, Inhalt und Projektdatei-Inhalt zu kodieren. Standardmäßig wird die Codierung des HTML-Outputs verwendet.</translation>
    </message>
<message>
        <source>CITE_BIB_FILES</source>
            <translation>Das \c CITE_BIB_FILES-Tag kann verwendet werden, um eine oder mehrere \c bib-Dateien anzugeben, die die Referenzdefinitionen enthalten. Dies muss eine Liste von &lt;code&gt;.bib&lt;/code&gt;-Dateien sein. Die &lt;code&gt;.bib&lt;/code&gt;-Erweiterung wird automatisch angehangt, wenn weggelassen. Dies erfordert, dass das \c bibtex-Tool installiert ist. Siehe auch https://en.wikipedia.org/wiki/BibTeX fur weitere Informationen. Fur \f$\mbox{\LaTeX}\f$ kann der Stil der Bibliographie mit \ref cfg_latex_bib_style "LATEX_BIB_STYLE" gesteuert werden. Um diese Funktion zu nutzen, benotigen Sie \c bibtex und \c perl im Suchpfad. Siehe auch \ref cmdcite "\\cite" fur Informationen zum Erstellen von Referenzen.</translation>
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
            <translation>Wenn clang-gestutztes Parsing aktiviert ist, konnen Sie dem clang-Parser den Pfad zum Verzeichnis mit einer Datei namens `compile_commands.json` bereitstellen. Diese Datei kann von CMake mit der Option CMAKE_EXPORT_COMPILE_COMMANDS generiert werden.</translation>
    </message>
<message>
        <source>CLANG_OPTIONS</source>
            <translation>Wenn clang-gestutztes Parsing aktiviert ist, konnen Sie dem Compiler Befehlszeilenoptionen bereitstellen, die Sie normalerweise beim Aufruf des Compilers verwenden wurden. Beachten Sie, dass die Include-Pfade bereits von Doxygen fur die mit \ref cfg_input "INPUT" und \ref cfg_include_path "INCLUDE_PATH" angegebenen Dateien und Verzeichnisse gesetzt werden.</translation>
    </message>
<message>
        <source>CLASS_GRAPH</source>
            <translation>Wenn das \c CLASS_GRAPH-Tag auf \c YES oder \c GRAPH oder \c BUILTIN gesetzt ist, generiert Doxygen einen Graphen fur jede dokumentierte Klasse, der die direkten und indirekten Vererbungsbeziehungen zeigt. Falls das \c CLASS_GRAPH-Tag auf \c YES oder \c GRAPH gesetzt ist und \ref cfg_have_dot "HAVE_DOT" ebenfalls aktiviert ist, wird `dot` verwendet, um den Graphen zu zeichnen. Falls das \c CLASS_GRAPH-Tag auf \c YES gesetzt ist und \ref cfg_have_dot "HAVE_DOT" deaktiviert ist oder wenn das \c CLASS_GRAPH-Tag auf \c BUILTIN gesetzt ist, wird der integrierte Generator verwendet. Wenn das \c CLASS_GRAPH-Tag auf \c TEXT gesetzt ist, werden die direkten und indirekten Vererbungsbeziehungen als Texte / Links angezeigt. Das explizite Aktivieren eines Vererbungsgraphen oder die Wahl einer anderen Darstellung fur einen Vererbungsgraphen einer bestimmten Klasse kann mit dem Befehl \ref cmdinheritancegraph "\\inheritancegraph" erreicht werden. Das Deaktivieren eines Vererbungsgraphen kann mit dem Befehl \ref cmdhideinheritancegraph "\\hideinheritancegraph" erreicht werden.</translation>
    </message>
<message>
        <source>COLLABORATION_GRAPH</source>
            <translation>Wenn das \c COLLABORATION_GRAPH-Tag auf \c YES gesetzt ist, generiert Doxygen einen Graphen fur jede dokumentierte Klasse, der die direkten und indirekten Implementierungsabhangigkeiten (Vererbung, Enthaltensein und Klassenreferenzvariablen) der Klasse mit anderen dokumentierten Klassen zeigt. Das explizite Aktivieren eines Kollaborationsgraphen, wenn \c COLLABORATION_GRAPH auf \c NO gesetzt ist, kann mit dem Befehl \ref cmdcollaborationgraph "\\collaborationgraph" erreicht werden. Das Deaktivieren eines Kollaborationsgraphen kann mit dem Befehl \ref cmdhidecollaborationgraph "\\hidecollaborationgraph" erreicht werden.</translation>
    </message>
<message>
        <source>COMPACT_LATEX</source>
            <translation>Wenn das \c COMPACT_LATEX-Tag auf \c YES gesetzt ist, generiert Doxygen kompaktere \f$\mbox{\LaTeX}\f$-Dokumente. Dies kann fur kleine Projekte nutzlich sein und kann dazu beitragen, im Allgemeinen einige Baume zu sparen.</translation>
    </message>
<message>
        <source>COMPACT_RTF</source>
            <translation>Wenn das \c COMPACT_RTF-Tag auf \c YES gesetzt ist, generiert Doxygen kompaktere RTF-Dokumente. Dies kann fur kleine Projekte nutzlich sein und kann dazu beitragen, im Allgemeinen einige Baume zu sparen.</translation>
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
            <translation>Das \c DIAFILE_DIRS-Tag kann verwendet werden, um ein oder mehrere Verzeichnisse anzugeben, die dia-Dateien enthalten, die in die Dokumentation eingebunden werden (siehe den Befehl \ref cmddiafile "\\diafile").</translation>
    </message>
<message>
        <source>DIA_PATH</source>
            <translation>Sie konnen mit dia erstellte Diagramme in die Doxygen-Dokumentation einbinden. Doxygen wird dann dia ausfuhren, um das Diagramm zu erstellen und in die Dokumentation einzufugen. Das DIA_PATH-Tag ermoglicht es Ihnen, das Verzeichnis anzugeben, in dem sich die dia-Binardatei befindet. Wenn leer gelassen, wird angenommen, dass dia im Standard-Suchpfad gefunden wird.</translation>
    </message>
<message>
        <source>DIRECTORY_GRAPH</source>
            <translation>Wenn das \c DIRECTORY_GRAPH-Tag auf \c YES gesetzt ist, zeigt Doxygen die Abhangigkeiten eines Verzeichnisses von anderen Verzeichnissen in grafischer Weise an. Die Abhangigkeitsbeziehungen werden durch die \c \#include-Beziehungen zwischen den Dateien in den Verzeichnissen bestimmt. Das explizite Aktivieren eines Verzeichnisgraphen, wenn \c DIRECTORY_GRAPH auf \c NO gesetzt ist, kann mit dem Befehl \ref cmddirectorygraph "\\directorygraph" erreicht werden. Das Deaktivieren eines Verzeichnisgraphen kann mit dem Befehl \ref cmdhidedirectorygraph "\\hidedirectorygraph" erreicht werden.</translation>
    </message>
<message>
        <source>DIR_GRAPH_MAX_DEPTH</source>
            <translation>Das \c DIR_GRAPH_MAX_DEPTH-Tag kann verwendet werden, um die maximale Anzahl von Ebenen untergeordneter Verzeichnisse zu begrenzen, die in Verzeichnisabhangigkeitsgraphen von \c dot generiert werden.</translation>
    </message>
<message>
        <source>DISABLE_INDEX</source>
            <translation>Wenn Sie volle Kontrolle uber das Layout der generierten HTML-Seiten mochten, kann es notwendig sein, den Index zu deaktivieren und ihn durch Ihren eigenen zu ersetzen. Das \c DISABLE_INDEX-Tag kann verwendet werden, um den kondensierten Index (Tabs) oben auf jeder HTML-Seite ein- oder auszuschalten. Ein Wert von \c NO aktiviert den Index und der Wert \c YES deaktiviert ihn. Da die Tabs im Index dieselben Informationen wie der Navigationsbaum enthalten, konnen Sie diese Option auf \c YES setzen, wenn Sie auch \ref cfg_generate_treeview "GENERATE_TREEVIEW" auf \c YES setzen.</translation>
    </message>
<message>
        <source>DISTRIBUTE_GROUP_DOC</source>
            <translation>Wenn in der Dokumentation eine Mitgliedergruppierung verwendet wird und das \c DISTRIBUTE_GROUP_DOC-Tag auf \c YES gesetzt ist, wird Doxygen die Dokumentation des ersten Mitglieds der Gruppe fur die anderen Mitglieder der Gruppe wiederverwenden (falls vorhanden). Standardmäßig mussen alle Mitglieder einer Gruppe explizit dokumentiert werden.</translation>
    </message>
<message>
        <source>DOCBOOK_OUTPUT</source>
            <translation>Das \c DOCBOOK_OUTPUT-Tag wird verwendet, um anzugeben, wo die Docbook-Seiten platziert werden. Wenn ein relativer Pfad eingegeben wird, wird der Wert von \ref cfg_output_directory "OUTPUT_DIRECTORY" davor gesetzt.</translation>
    </message>
<message>
        <source>DOCSET_BUNDLE_ID</source>
            <translation>Dieses Tag gibt eine Zeichenfolge an, die das Dokumentensatz-Bundle eindeutig identifizieren sollte. Dies sollte eine Zeichenfolge im umgekehrten Domainnamen-Stil sein, z.B. &lt;code&gt;com.mycompany.MyDocSet&lt;/code&gt;. Doxygen wird &lt;code&gt;.docset&lt;/code&gt; an den Namen anhangen.</translation>
    </message>
<message>
        <source>DOCSET_FEEDNAME</source>
            <translation>Dieses Tag bestimmt den Namen des Docset-Feeds. Ein Dokumentations-Feed bietet einen Dach, unter dem mehrere Dokumentensatze eines einzelnen Anbieters (z.B. eines Unternehmens oder einer Produktsuite) gruppiert werden konnen.</translation>
    </message>
<message>
        <source>DOCSET_FEEDURL</source>
            <translation>Dieses Tag bestimmt die URL des Docset-Feeds. Ein Dokumentations-Feed bietet einen Dach, unter dem mehrere Dokumentensatze eines einzelnen Anbieters (z.B. eines Unternehmens oder einer Produktsuite) gruppiert werden konnen.</translation>
    </message>
<message>
        <source>DOCSET_PUBLISHER_ID</source>
            <translation>Das \c DOCSET_PUBLISHER_ID-Tag gibt eine Zeichenfolge an, die den Dokumentationsherausgeber eindeutig identifizieren sollte. Dies sollte eine Zeichenfolge im umgekehrten Domainnamen-Stil sein, z.B. &lt;code&gt;com.mycompany.MyDocSet.documentation&lt;/code&gt;.</translation>
    </message>
<message>
        <source>DOCSET_PUBLISHER_NAME</source>
            <translation>Das \c DOCSET_PUBLISHER_NAME-Tag identifiziert den Dokumentationsherausgeber.</translation>
    </message>
<message>
        <source>DOTFILE_DIRS</source>
            <translation>Das \c DOTFILE_DIRS-Tag kann verwendet werden, um ein oder mehrere Verzeichnisse anzugeben, die dot-Dateien enthalten, die in die Dokumentation eingebunden werden (siehe den Befehl \ref cmddotfile "\\dotfile").</translation>
    </message>
<message>
        <source>DOT_CLEANUP</source>
            <translation>Wenn das \c DOT_CLEANUP-Tag auf \c YES gesetzt ist, loscht Doxygen die Zwischendateien, die zum Generieren der verschiedenen Diagramme verwendet wurden. &lt;br&gt;Hinweis: Diese Einstellung wird nicht nur fur dot-Dateien verwendet, sondern auch fur msc-Temporardateien.</translation>
    </message>
<message>
        <source>DOT_COMMON_ATTR</source>
            <translation>\c DOT_COMMON_ATTR sind gemeinsame Attribute fur Knoten, Kanten und Labels von Untergraphen. Wenn Sie eine anders aussehende Schrift in den von Doxygen generierten dot-Dateien mochten, konnen Sie die Attribute fontname, fontcolor und fontsize angeben. Fur Details siehe &lt;a href=https://graphviz.org/doc/info/attrs.html&gt;Node, Edge and Graph Attributes specification&lt;/a&gt;. Sie mussen sicherstellen, dass dot die Schrift finden kann, was erreicht werden kann, indem Sie sie an einem Standardort platzieren oder die Umgebungsvariable \c DOTFONTPATH setzen oder \ref cfg_dot_fontpath "DOT_FONTPATH" auf das Verzeichnis setzen, das die Schrift enthalt. Die Standard-Schriftgroße von graphviz ist 14.</translation>
    </message>
<message>
        <source>DOT_EDGE_ATTR</source>
            <translation>\c DOT_EDGE_ATTR wird mit \ref cfg_dot_common_attr "DOT_COMMON_ATTR" verkettet. Fur einen eleganten Stil konnen Sie 'arrowhead=open, arrowtail=open, arrowsize=0.5' hinzufugen. &lt;a href=https://graphviz.org/doc/info/arrows.html&gt;Vollstandige Dokumentation uber Pfeilformen.&lt;/a&gt;</translation>
    </message>
<message>
        <source>DOT_FONTPATH</source>
            <translation>Sie konnen den Pfad festlegen, in dem \c dot die mit fontname in \ref cfg_dot_common_attr "DOT_COMMON_ATTR" und anderen dot-Attributen angegebene Schrift finden kann.</translation>
    </message>
<message>
        <source>DOT_GRAPH_MAX_NODES</source>
            <translation>Das \c DOT_GRAPH_MAX_NODES-Tag kann verwendet werden, um die maximale Anzahl von Knoten festzulegen, die im Graphen angezeigt werden. Wenn die Anzahl der Knoten in einem Graphen groser wird als dieser Wert, wird Doxygen den Graphen abschneiden, was visualisiert wird, indem ein Knoten als rotes Feld dargestellt wird. Beachten Sie, dass der Graph uberhaupt nicht angezeigt wird, wenn die Anzahl der direkten Kinder des Wurzelknotens in einem Graphen bereits groser als \c DOT_GRAPH_MAX_NODES ist. Beachten Sie auch, dass die Große eines Graphen durch \ref cfg_max_dot_graph_depth "MAX_DOT_GRAPH_DEPTH" weiter eingeschrankt werden kann.</translation>
    </message>
<message>
        <source>DOT_IMAGE_FORMAT</source>
            <translation>Das \c DOT_IMAGE_FORMAT-Tag kann verwendet werden, um das Bildformat der von \c dot generierten Bilder festzulegen. Fur eine Erklarung der Bildformate siehe den Abschnitt Ausgabeformate in der Dokumentation des \c dot-Tools (&lt;a href="https://www.graphviz.org/"&gt;Graphviz&lt;/a&gt;). &lt;br&gt;Beachten Sie, dass die Formate `svg:cairo` und `svg:cairo:cairo` nicht in Kombination mit \ref cfg_interactive_svg "INTERACTIVE_SVG" verwendet werden konnen (\ref cfg_interactive_svg "INTERACTIVE_SVG" wird auf `NO` gesetzt).</translation>
    </message>
<message>
        <source>DOT_MULTI_TARGETS</source>
            <translation>Setzen Sie das \c DOT_MULTI_TARGETS-Tag auf \c YES, um \c dot zu erlauben, mehrere Ausgabedateien in einem Lauf zu generieren (d.h. mehrere -o und -T Optionen in der Befehlszeile). Dies macht \c dot schneller, aber da nur neuere Versionen von \c dot (&gt;1.8.10) dies unterstutzen, ist diese Funktion standardmäßig deaktiviert.</translation>
    </message>
<message>
        <source>DOT_NODE_ATTR</source>
            <translation>\c DOT_NODE_ATTR wird mit \ref cfg_dot_common_attr "DOT_COMMON_ATTR" verkettet. Fur eine Ansicht ohne Kasten um Knoten setzen Sie 'shape=plain' oder 'shape=plaintext'. &lt;a href=https://www.graphviz.org/doc/info/shapes.html&gt;Shapes specification&lt;/a&gt;</translation>
    </message>
<message>
        <source>DOT_NUM_THREADS</source>
            <translation>Das \c DOT_NUM_THREADS gibt die Anzahl der \c dot-Aufrufe an, die Doxygen parallel ausfuhren darf. Wenn auf \c 0 gesetzt, wird Doxygen dies auf der Anzahl der im System verfugbaren Prozessoren basieren. Sie konnen es explizit auf einen Wert groser als 0 setzen, um die Kontrolle uber das Gleichgewicht zwischen CPU-Last und Verarbeitungsgeschwindigkeit zu erhalten.</translation>
    </message>
<message>
        <source>DOT_PATH</source>
            <translation>Das \c DOT_PATH-Tag kann verwendet werden, um den Pfad anzugeben, in dem das \c dot-Tool gefunden werden kann. Wenn leer gelassen, wird angenommen, dass dot im Standard-Suchpfad gefunden wird.</translation>
    </message>
<message>
        <source>DOT_UML_DETAILS</source>
            <translation>Wenn das \c DOT_UML_DETAILS-Tag auf \c NO gesetzt ist, zeigt Doxygen Attribute und Methoden ohne Typen und Argumente in den UML-Graphen. Wenn das \c DOT_UML_DETAILS-Tag auf \c YES gesetzt ist, fugt Doxygen Typen und Argumente fur Attribute und Methoden in den UML-Graphen hinzu. Wenn das \c DOT_UML_DETAILS-Tag auf \c NONE gesetzt ist, generiert Doxygen keine Felder mit Klassenmitgliedsinformationen in den UML-Graphen. Die Klassendiagramme ahneln den Standard-Klassendiagrammen, verwenden aber UML-Notation fur die Beziehungen.</translation>
    </message>
<message>
        <source>DOT_WRAP_THRESHOLD</source>
            <translation>Das \c DOT_WRAP_THRESHOLD-Tag kann verwendet werden, um die maximale Anzahl von Zeichen festzulegen, die in einer einzelnen Zeile angezeigt werden. Wenn die Zeilenlange diesen Wert uberschreitet, wird die Zeile umbrochen.</translation>
    </message>
<message>
        <source>ECLIPSE_DOC_ID</source>
            <translation>Eine eindeutige Kennung fur das Eclipse-Hilfe-Plugin. Dies sollte eine Zeichenfolge im umgekehrten Domainnamen-Stil sein, z.B. &lt;code&gt;org.doxygen.Project&lt;/code&gt;.</translation>
    </message>
<message>
        <source>ENABLED_SECTIONS</source>
            <translation>Das \c ENABLED_SECTIONS-Tag kann verwendet werden, um bedingte Dokumentationsabschnitte zu aktivieren, die mit \ref cmdif "\\if" \&lt;section_label\&gt; ... \ref cmdendif "\\endif" und \ref cmdcond "\\cond" \&lt;section_label\&gt; ... \ref cmdendcond "\\endcond" Blöcken markiert sind.</translation>
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
            <translation>Wenn der Wert des \ref cfg_example_path "EXAMPLE_PATH"-Tags Verzeichnisse enthalt, konnen Sie das \c EXAMPLE_PATTERNS-Tag verwenden, um ein oder mehrere Wildcard-Muster anzugeben, um die Quelldateien in den Verzeichnissen zu filtern.</translation>
    </message>
<message>
        <source>EXAMPLE_RECURSIVE</source>
            <translation>Wenn das \c EXAMPLE_RECURSIVE-Tag auf \c YES gesetzt ist, werden Unterverzeichnisse nach Eingabedateien durchsucht. Wenn auf \c NO gesetzt, werden nur die im \ref cfg_example_path "EXAMPLE_PATH"-Tag angegebenen Verzeichnisse durchsucht.</translation>
    </message>
<message>
        <source>EXCLUDE</source>
            <translation>Das \c EXCLUDE-Tag kann verwendet werden, um Dateien und/oder Verzeichnisse anzugeben, die von den \ref cfg_input "INPUT"-Quelldateien ausgeschlossen werden sollen. Auf diese Weise konnen Sie einfach ein Unterverzeichnis aus einem Verzeichnisbaum ausschließen, dessen Wurzel mit dem \ref cfg_input "INPUT"-Tag angegeben ist. &lt;br&gt;Beachten Sie, dass relative Pfade relativ zu dem Verzeichnis sind, aus dem Doxygen ausgefuhrt wird.</translation>
    </message>
<message>
        <source>EXCLUDE_PATTERNS</source>
            <translation>Wenn der Wert des \ref cfg_input "INPUT"-Tags Verzeichnisse enthalt, konnen Sie das \c EXCLUDE_PATTERNS-Tag verwenden, um ein oder mehrere Wildcard-Muster anzugeben, um bestimmte Dateien aus diesen Verzeichnissen auszuschlieben.</translation>
    </message>
<message>
        <source>EXCLUDE_SYMBOLS</source>
            <translation>Das \c EXCLUDE_SYMBOLS-Tag kann verwendet werden, um einen oder mehrere Symbolnamen anzugeben, die von der Ausgabe ausgeschlossen werden sollen. Das Symbol kann ein Name einer Funktion, einer Variable, einer Struktur, eines Typedefs usw. sein.</translation>
    </message>
<message>
        <source>EXCLUDE_SYMLINKS</source>
            <translation>Das \c EXCLUDE_SYMLINKS-Tag kann verwendet werden, um auszuwahlen, ob Dateien oder Verzeichnisse, die symbolische Links sind, von der Eingabe ausgeschlossen werden.</translation>
    </message>
<message>
        <source>EXPAND_AS_DEFINED</source>
            <translation>Wenn die Tags \ref cfg_macro_expansion "MACRO_EXPANSION" und \ref cfg_expand_only_predef "EXPAND_ONLY_PREDEF" auf \c YES gesetzt sind, kann dieses Tag verwendet werden, um eine Liste von Makronamen anzugeben, die expandiert werden sollen.</translation>
    </message>
<message>
        <source>EXPAND_ONLY_PREDEF</source>
            <translation>Wenn die Tags \c EXPAND_ONLY_PREDEF und \ref cfg_macro_expansion "MACRO_EXPANSION" beide auf \c YES gesetzt sind, ist die Makroexpansion beschrankt auf die mit \ref cfg_expand_as_defined "EXPAND_AS_DEFINED" angegebenen Makros.</translation>
    </message>
<message>
        <source>EXTENSION_MAPPING</source>
            <translation>Doxygen wahlt den zu verwendenden Parser basierend auf der Erweiterung der zu parsenden Datei aus. Mit diesem Tag konnen Sie einen Parser zuweisen, der fur eine bestimmte Erweiterung verwendet werden soll. Doxygen hat eine eingebaute Zuordnung, aber Sie konnen diese mit diesem Tag uberschreiben oder erweitern. Das Format ist &lt;code&gt;ext=sprache&lt;/code&gt;, wobei \c ext eine Dateierweiterung ist und sprache einer der von Doxygen unterstutzten Parser: IDL, Java, JavaScript, Csharp (C#), C, C++, Lex, D, PHP, md (Markdown), Objective-C, Python, Slice, VHDL, Fortran (festes Format Fortran: FortranFixed, freies Format Fortran: FortranFree, unbekanntes Format Fortran: Fortran. Im letzteren Fall versucht der Parser zu erraten, ob der Code festes oder freies Format ist, was die Standardeinstellung fur Fortran-Typ-Dateien ist). Um beispielsweise Doxygen dazu zu bringen, &lt;code&gt;.inc&lt;/code&gt;-Dateien als Fortran-Dateien zu behandeln (Standard ist PHP) und &lt;code&gt;.f&lt;/code&gt;-Dateien als C (Standard ist Fortran), verwenden Sie: `inc=Fortran f=C`. &lt;br&gt;Hinweis: Fur Dateien ohne Erweiterung konnen Sie `no_extension` als Platzhalter verwenden. &lt;br&gt;Hinweis: Fur benutzerdefinierte Erweiterungen mussen Sie auch \ref cfg_file_patterns "FILE_PATTERNS" setzen, sonst werden die Dateien von Doxygen nicht gelesen. &lt;br&gt;Hinweis: Siehe auch die Liste der \ref default_file_extension_mapping "Standard-Dateierweiterungszuordnungen".</translation>
    </message>
<message>
        <source>EXTERNAL_GROUPS</source>
            <translation>Wenn das \c EXTERNAL_GROUPS-Tag auf \c YES gesetzt ist, werden alle externen Gruppen im Themenindex aufgefuhrt. Wenn auf \c NO gesetzt, werden nur die Gruppen des aktuellen Projekts aufgefuhrt.</translation>
    </message>
<message>
        <source>EXTERNAL_PAGES</source>
            <translation>Wenn das \c EXTERNAL_PAGES-Tag auf \c YES gesetzt ist, werden alle externen Seiten im Related-Pages-Index aufgefuhrt. Wenn auf \c NO gesetzt, werden nur die Seiten des aktuellen Projekts aufgefuhrt.</translation>
    </message>
<message>
        <source>EXTERNAL_SEARCH</source>
            <translation>Wenn das \c EXTERNAL_SEARCH-Tag aktiviert ist, generiert Doxygen das PHP-Skript zum Suchen nicht mehr. Stattdessen werden die Suchergebnisse in eine XML-Datei geschrieben, die von einem externen Indexer verarbeitet werden muss. Doxygen ruft eine externe Suchmaschine auf, die durch die Option \ref cfg_searchengine_url "SEARCHENGINE_URL" angegeben wird, um die Suchergebnisse zu erhalten. &lt;br&gt;Doxygen liefert einen Beispiel-Indexer (\c doxyindexer) und eine Suchmaschine (&lt;code&gt;doxysearch.cgi&lt;/code&gt;), die auf der Open-Source-Suchmaschinen-Bibliothek &lt;a href="https://xapian.org/"&gt;Xapian&lt;/a&gt; basieren. &lt;br&gt;Siehe den Abschnitt \ref extsearch fur Details.</translation>
    </message>
<message>
        <source>EXTERNAL_SEARCH_ID</source>
            <translation>Wenn \ref cfg_server_based_search "SERVER_BASED_SEARCH" und \ref cfg_external_search "EXTERNAL_SEARCH" beide aktiviert sind, kann das \c EXTERNAL_SEARCH_ID-Tag als Kennung fur das Projekt verwendet werden. Dies ist in Kombination mit \ref cfg_extra_search_mappings "EXTRA_SEARCH_MAPPINGS" nutzlich, um mehrere Projekte zu durchsuchen und die Ergebnisse zurück zum richtigen Projekt zu leiten.</translation>
    </message>
<message>
        <source>EXTERNAL_TOOL_PATH</source>
            <translation>Das \c EXTERNAL_TOOL_PATH-Tag kann verwendet werden, um den Suchpfad (PATH-Umgebungsvariable) zu erweitern, damit externe Tools wie \c latex und \c gs gefunden werden konnen. \note Mit EXTERNAL_TOOL_PATH angegebene Verzeichnisse werden vor den bereits durch die PATH-Variable angegebenen Pfad hinzugefugt und in der angegebenen Reihenfolge hinzugefugt. \note Diese Option ist besonders nutzlich fur macOS Version 14 (Sonoma) und hoher, wenn Doxygen von Doxywizard ausgefuhrt wird, da in diesem Fall alle benutzerdefinierten Anderungen an PATH ignoriert werden. Ein typisches Beispiel auf macOS ist die Einstellung \verbatim EXTERNAL_TOOL_PATH = /Library/TeX/texbin /usr/local/bin \endverbatim Zusammen mit dem Standardpfad wird der vollstandige Suchpfad, den Doxygen beim Starten externer Tools verwendet, zu \verbatim PATH=/Library/TeX/texbin:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin \endverbatim</translation>
    </message>
<message>
        <source>EXTRACT_ALL</source>
            <translation>Wenn das \c EXTRACT_ALL-Tag auf \c YES gesetzt ist, geht Doxygen davon aus, dass alle Entitaten in der Dokumentation dokumentiert sind, auch wenn keine Dokumentation verfugbar war. Private Klassenmitglieder und statische Dateimitglieder werden ausgeblendet, es sei denn, die Tags \ref cfg_extract_private "EXTRACT_PRIVATE" bzw. \ref cfg_extract_static "EXTRACT_STATIC" sind auf \c YES gesetzt. \note Dies deaktiviert auch die Warnungen uber undokumentierte Mitglieder, die normalerweise erzeugt werden, wenn \ref cfg_warnings "WARNINGS" auf \c YES gesetzt ist.</translation>
    </message>
<message>
        <source>EXTRACT_ANON_NSPACES</source>
            <translation>Wenn dieses Flag auf \c YES gesetzt ist, werden die Mitglieder anonymer Namensraume extrahiert und erscheinen in der Dokumentation als Namensraum namens 'anonymous_namespace{file}', wobei file durch den Basisnamen der Datei ersetzt wird, die den anonymen Namensraum enthalt. Standardmäßig sind anonyme Namensraume ausgeblendet.</translation>
    </message>
<message>
        <source>EXTRACT_LOCAL_CLASSES</source>
            <translation>Wenn das \c EXTRACT_LOCAL_CLASSES-Tag auf \c YES gesetzt ist, werden lokal in Quelldateien definierte Klassen (und Strukturen) in die Dokumentation aufgenommen. Wenn auf \c NO gesetzt, werden nur in Header-Dateien definierte Klassen aufgenommen. Hat keine Auswirkung auf Java-Quellen.</translation>
    </message>
<message>
        <source>EXTRACT_LOCAL_METHODS</source>
            <translation>Dieses Flag ist nur fur Objective-C-Code nutzlich. Wenn auf \c YES gesetzt, werden lokale Methoden, die im Implementierungsabschnitt definiert sind, aber nicht im Interface, in die Dokumentation aufgenommen. Wenn auf \c NO gesetzt, werden nur Methoden im Interface aufgenommen.</translation>
    </message>
<message>
        <source>EXTRACT_PACKAGE</source>
            <translation>Wenn das \c EXTRACT_PACKAGE-Tag auf \c YES gesetzt ist, werden alle Mitglieder mit package- oder internem Sichtbarkeitsbereich in die Dokumentation aufgenommen.</translation>
    </message>
<message>
        <source>EXTRACT_PRIVATE</source>
            <translation>Wenn das \c EXTRACT_PRIVATE-Tag auf \c YES gesetzt ist, werden alle privaten Mitglieder einer Klasse in die Dokumentation aufgenommen.</translation>
    </message>
<message>
        <source>EXTRACT_PRIV_VIRTUAL</source>
            <translation>Wenn das \c EXTRACT_PRIV_VIRTUAL-Tag auf \c YES gesetzt ist, werden dokumentierte private virtuelle Methoden einer Klasse in die Dokumentation aufgenommen.</translation>
    </message>
<message>
        <source>EXTRACT_STATIC</source>
            <translation>Wenn das \c EXTRACT_STATIC-Tag auf \c YES gesetzt ist, werden alle statischen Mitglieder einer Datei in die Dokumentation aufgenommen.</translation>
    </message>
<message>
        <source>EXTRA_PACKAGES</source>
            <translation>Das \c EXTRA_PACKAGES-Tag kann verwendet werden, um einen oder mehrere \f$\mbox{\LaTeX}\f$-Paketnamen anzugeben, die in die \f$\mbox{\LaTeX}\f$-Ausgabe aufgenommen werden sollen. Das Paket kann nur durch seinen Namen oder mit der korrekten Syntax fur die Verwendung mit dem \f$\mbox{\LaTeX}\f$-Befehl `\usepackage` angegeben werden. Um beispielsweise die `times`-Schrift zu erhalten, konnen Sie angeben: \verbatim EXTRA_PACKAGES=times oder EXTRA_PACKAGES={times} \endverbatim Um die Option `intlimits` mit dem `amsmath`-Paket zu verwenden, konnen Sie angeben: \verbatim EXTRA_PACKAGES=[intlimits]{amsmath} \endverbatim Wenn leer gelassen, werden keine zusatzlichen Pakete aufgenommen.</translation>
    </message>
<message>
        <source>EXTRA_SEARCH_MAPPINGS</source>
            <translation>Das \c EXTRA_SEARCH_MAPPINGS-Tag kann verwendet werden, um das Durchsuchen von Doxygen-Projekten zu ermoglichen, die nicht durch diese Konfigurationsdatei definiert sind, aber alle demselben externen Suchindex hinzugefugt werden. Jedes Projekt muss eine eindeutige ID haben, die uber \ref cfg_external_search_id "EXTERNAL_SEARCH_ID" gesetzt wird. Die Suchzuordnung ordnet dann die ID einer relativen Position zu, an der die Dokumentation gefunden werden kann. Das Format ist: \verbatim EXTRA_SEARCH_MAPPINGS = tagname1=loc1 tagname2=loc2 ... \endverbatim</translation>
    </message>
<message>
        <source>EXT_LINKS_IN_WINDOW</source>
            <translation>Wenn die \c EXT_LINKS_IN_WINDOW-Option auf \c YES gesetzt ist, offnet Doxygen Links zu externen Symbolen, die uber Tag-Dateien importiert wurden, in einem separaten Fenster.</translation>
    </message>
<message>
        <source>FILE_PATTERNS</source>
            <translation>Wenn der Wert des \ref cfg_input "INPUT"-Tags Verzeichnisse enthalt, konnen Sie das \c FILE_PATTERNS-Tag verwenden, um ein oder mehrere Wildcard-Muster (wie `*.cpp` und `*.h`) anzugeben, um die Quelldateien in den Verzeichnissen zu filtern.&lt;br&gt; Beachten Sie, dass Sie fur benutzerdefinierte Erweiterungen oder nicht direkt unterstutzte Erweiterungen auch \ref cfg_extension_mapping "EXTENSION_MAPPING" fur die Erweiterung setzen mussen, sonst werden die Dateien von Doxygen nicht gelesen.&lt;br&gt; Beachten Sie, dass die Liste der standardmäßig gepruften Dateimuster von der Liste der \ref default_file_extension_mapping "Standard-Dateierweiterungszuordnungen" abweichen kann.&lt;br&gt; Wenn leer gelassen, werden die folgenden Muster gepruft:</translation>
    </message>
<message>
        <source>FILE_VERSION_FILTER</source>
            <translation>Das \c FILE_VERSION_FILTER-Tag kann verwendet werden, um ein Programm oder Skript anzugeben, das Doxygen aufrufen soll, um die aktuelle Version fur jede Datei zu erhalten (typischerweise vom Versionskontrollsystem). Doxygen ruft das Programm auf, indem es (uber &lt;code&gt;popen()&lt;/code&gt;) den Befehl &lt;code&gt;command input-file&lt;/code&gt; ausfuhrt, wobei \c command der Wert des \c FILE_VERSION_FILTER-Tags ist und \c input-file der Name einer von Doxygen bereitgestellten Eingabedatei ist. Was auch immer das Programm auf die Standardausgabe schreibt, wird als Dateiversion verwendet. Fur ein Beispiel siehe die Dokumentation.</translation>
    </message>
<message>
        <source>FILTER_PATTERNS</source>
            <translation>Das \c FILTER_PATTERNS-Tag kann verwendet werden, um Filter auf Dateimusterbasis anzugeben. Doxygen vergleicht den Dateinamen mit jedem Muster und wendet den Filter an, wenn es eine Ubereinstimmung gibt. Die Filter sind eine Liste der Form: muster=filter (wie `*.cpp=my_cpp_filter`). Siehe \ref cfg_input_filter "INPUT_FILTER" fur weitere Informationen zur Verwendung von Filtern. Wenn das \c FILTER_PATTERNS-Tag leer ist oder keines der Muster mit dem Dateinamen ubereinstimmt, wird \ref cfg_input_filter "INPUT_FILTER" angewendet. &lt;br&gt;Beachten Sie, dass Sie fur benutzerdefinierte Erweiterungen oder nicht direkt unterstutzte Erweiterungen auch \ref cfg_extension_mapping "EXTENSION_MAPPING" fur die Erweiterung setzen mussen, sonst werden die Dateien von Doxygen nicht ordnungsgema verarbeitet.&lt;br&gt;</translation>
    </message>
<message>
        <source>FILTER_SOURCE_FILES</source>
            <translation>Wenn das \c FILTER_SOURCE_FILES-Tag auf \c YES gesetzt ist, wird der Eingabefilter (falls mit \ref cfg_input_filter "INPUT_FILTER" gesetzt) auch verwendet, um die Eingabedateien zu filtern, die zum Erstellen der Quelldateien zum Durchsuchen verwendet werden (d.h. wenn \ref cfg_source_browser "SOURCE_BROWSER" auf \c YES gesetzt ist).</translation>
    </message>
<message>
        <source>FILTER_SOURCE_PATTERNS</source>
            <translation>Das \c FILTER_SOURCE_PATTERNS-Tag kann verwendet werden, um Quellfilter pro Dateimuster anzugeben. Ein Muster uberschreibt die Einstellung fur \ref cfg_filter_patterns "FILTER_PATTERN" (falls vorhanden) und es ist auch moglich, die Quellfilterung fur ein bestimmtes Muster mit `*.ext=` zu deaktivieren (also ohne einen Filter zu nennen).</translation>
    </message>
<message>
        <source>FORCE_LOCAL_INCLUDES</source>
            <translation>Wenn das \c FORCE_LOCAL_INCLUDES-Tag auf \c YES gesetzt ist, wird Doxygen Include-Dateien mit doppelten Anfuhrungszeichen in der Dokumentation auflisten, anstatt mit spitzen Klammern.</translation>
    </message>
<message>
        <source>FORMULA_FONTSIZE</source>
            <translation>Verwenden Sie dieses Tag, um die Schriftgrobe von \f$\mbox{\LaTeX}\f$-Formeln zu andern, die als Bilder in die HTML-Dokumentation eingebunden werden. Wenn Sie die Schriftgrobe nach einem erfolgreichen Doxygen-Lauf andern, mussen Sie manuell alle `form_*.png`-Bilder aus dem HTML-Ausgabeverzeichnis entfernen, um deren Neugenerierung zu erzwingen.</translation>
    </message>
<message>
        <source>FORMULA_MACROFILE</source>
            <translation>Die \c FORMULA_MACROFILE kann \f$\mbox{\LaTeX}\f$-Befehle `\newcommand` und `\renewcommand` enthalten, um neue \f$\mbox{\LaTeX}\f$-Befehle zu erstellen, die als Bausteine in Formeln verwendet werden. Siehe den Abschnitt \ref formulas fur Details.</translation>
    </message>
<message>
        <source>FORTRAN_COMMENT_AFTER</source>
            <translation>Der Fortran-Standard gibt an, dass fur fest formatierten Fortran-Code alle Zeichen ab Position 72 als Kommentar betrachtet werden. Eine haufige Erweiterung ist es, langere Zeilen zu erlauben, bevor der automatische Kommentar beginnt. Die Einstellung \c FORTRAN_COMMENT_AFTER macht es auch moglich, dass langere Zeilen verarbeitet werden konnen, bevor der automatische Kommentar beginnt.</translation>
    </message>
<message>
        <source>FULL_PATH_NAMES</source>
            <translation>Wenn das \c FULL_PATH_NAMES-Tag auf \c YES gesetzt ist, fugt Doxygen den vollstandigen Pfad vor Dateinamen in der Dateiliste und in den Header-Dateien hinzu. Wenn auf \c NO gesetzt, wird der kurzeste Pfad verwendet, der die Dateinamen eindeutig macht.</translation>
    </message>
<message>
        <source>FULL_SIDEBAR</source>
            <translation>Wenn \ref cfg_generate_treeview "GENERATE_TREEVIEW" auf \c YES gesetzt ist, bestimmt die \c FULL_SIDEBAR-Option, ob die Seitenleiste auf den Treeview-Bereich beschrankt ist (Wert \c NO) oder ob sie sich uber die volle Hohe des Fensters erstrecken soll (Wert \c YES). Wenn dies auf \c YES gesetzt wird, ergibt sich ein Layout ahnlich wie z.B. https://docs.readthedocs.io mit mehr Platz fur Inhalte, aber weniger Platz fur Projektlogo, Titel und Beschreibung. Wenn \ref cfg_generate_treeview "GENERATE_TREEVIEW" auf \c NO gesetzt ist, hat diese Option keine Auswirkung.</translation>
    </message>
<message>
        <source>GENERATE_AUTOGEN_DEF</source>
            <translation>Wenn das \c GENERATE_AUTOGEN_DEF-Tag auf \c YES gesetzt ist, generiert Doxygen eine AutoGen Definitions-Datei (siehe https://autogen.sourceforge.net/), die die Struktur des Codes einschließlich aller Dokumentation erfasst. Beachten Sie, dass diese Funktion derzeit noch experimentell und unvollstandig ist.</translation>
    </message>
<message>
        <source>GENERATE_BUGLIST</source>
            <translation>Das \c GENERATE_BUGLIST-Tag kann verwendet werden, um die Bug-Liste zu aktivieren (\c YES) oder zu deaktivieren (\c NO). Diese Liste wird erstellt, indem \ref cmdbug "\\bug"-Befehle in der Dokumentation platziert werden.</translation>
    </message>
<message>
        <source>GENERATE_CHI</source>
            <translation>Das \c GENERATE_CHI-Flag steuert, ob eine separate `.chi`-Indexdatei generiert wird (\c YES) oder dass sie in die Haupt-`.chm`-Datei aufgenommen werden soll (\c NO).</translation>
    </message>
<message>
        <source>GENERATE_DEPRECATEDLIST</source>
            <translation>Das \c GENERATE_DEPRECATEDLIST-Tag kann verwendet werden, um die Veraltet-Liste zu aktivieren (\c YES) oder zu deaktivieren (\c NO). Diese Liste wird erstellt, indem \ref cmddeprecated "\\deprecated"-Befehle in der Dokumentation platziert werden.</translation>
    </message>
<message>
        <source>GENERATE_DOCBOOK</source>
            <translation>Wenn das \c GENERATE_DOCBOOK-Tag auf \c YES gesetzt ist, generiert Doxygen Docbook-Dateien, die zur Erstellung von PDF verwendet werden konnen.</translation>
    </message>
<message>
        <source>GENERATE_DOCSET</source>
            <translation>Wenn das \c GENERATE_DOCSET-Tag auf \c YES gesetzt ist, werden zusatzliche Indexdateien generiert, die als Eingabe fur &lt;a href="https://developer.apple.com/xcode/"&gt;Apples Xcode 3 integrierte Entwicklungsumgebung&lt;/a&gt; verwendet werden konnen, die mit OSX 10.5 (Leopard) eingefuhrt wurde. Um einen Dokumentensatz zu erstellen, generiert Doxygen eine Makefile im HTML-Ausgabeverzeichnis. Das Ausfuhren von \c make erzeugt den Dokumentensatz in diesem Verzeichnis und das Ausfuhren von &lt;code&gt;make install&lt;/code&gt; installiert den Dokumentensatz in &lt;code&gt;~/Library/Developer/Shared/Documentation/DocSets&lt;/code&gt;, sodass Xcode ihn beim Start findet. Siehe https://developer.apple.com/library/archive/featuredarticles/DoxygenXcode/_index.html fur weitere Informationen.</translation>
    </message>
<message>
        <source>GENERATE_ECLIPSEHELP</source>
            <translation>Wenn das \c GENERATE_ECLIPSEHELP-Tag auf \c YES gesetzt ist, werden zusatzliche Indexdateien generiert, die zusammen mit den HTML-Dateien ein Eclipse-Hilfe-Plugin bilden. Um das Plugin zu installieren, platzieren Sie das Verzeichnis, das die HTML- und Indexdateien enthalt, in Ihrem Eclipse-Installationsverzeichnis unter `plugins`.</translation>
    </message>
<message>
        <source>GENERATE_HTML</source>
            <translation>Wenn das \c GENERATE_HTML-Tag auf \c YES gesetzt ist, generiert Doxygen HTML-Ausgabe.</translation>
    </message>
<message>
        <source>GENERATE_HTMLHELP</source>
            <translation>Wenn das \c GENERATE_HTMLHELP-Tag auf \c YES gesetzt ist, generiert Doxygen drei zusatzliche HTML-Indexdateien: \c index.hhp, \c index.hhc und \c index.hhk. Die \c index.hhp ist eine Projektdatei, die von Microsofts HTML Help Workshop unter Windows gelesen werden kann. Anfang 2021 hat Microsoft die ursprungliche Seite mit den Download-Links offline genommen (der HTML Help Workshop war bereits viele Jahre im Wartungsmodus). Sie konnen den HTML Help Workshop aus den Web-Archiven unter &lt;a href="http://web.archive.org/web/20160201063255/https://download.microsoft.com/download/0/A/9/0A939EF6-E31C-430F-A3DF-DFAE7960D564/htmlhelp.exe"&gt;Installation executable&lt;/a&gt; herunterladen. &lt;br&gt;Der HTML Help Workshop enthalt einen Compiler, der alle von Doxygen generierten HTML-Ausgaben in eine einzelne kompilierte HTML-Datei (`.chm`) umwandeln kann. Kompilierte HTML-Dateien werden jetzt als Windows 98-Hilfeformat verwendet und werden in Zukunft das alte Windows-Hilfeformat (`.hlp`) auf allen Windows-Plattformen ersetzen. Komprimierte HTML-Dateien enthalten auch einen Index, ein Inhaltsverzeichnis und Sie konnen nach Wörtern in der Dokumentation suchen. Der HTML-Workshop enthalt auch einen Viewer fur komprimierte HTML-Dateien.</translation>
    </message>
<message>
        <source>GENERATE_LATEX</source>
            <translation>Wenn das \c GENERATE_LATEX-Tag auf \c YES gesetzt ist, generiert Doxygen \f$\mbox{\LaTeX}\f$-Ausgabe.</translation>
    </message>
<message>
        <source>GENERATE_LEGEND</source>
            <translation>Wenn das \c GENERATE_LEGEND-Tag auf \c YES gesetzt ist, generiert Doxygen eine Legenden-Seite, die die Bedeutung der verschiedenen Boxen in den von dot erzeugten Graphen erklart.</translation>
    </message>
<message>
        <source>GENERATE_MAN</source>
            <translation>Wenn das \c GENERATE_MAN-Tag auf \c YES gesetzt ist, generiert Doxygen Man-Pages fur Klassen und Dateien.</translation>
    </message>
<message>
        <source>GENERATE_PERLMOD</source>
            <translation>Wenn das \c GENERATE_PERLMOD-Tag auf \c YES gesetzt ist, generiert Doxygen eine Perl-Modul-Datei, die bestimmte Informationen uber den Code in einer fur Perl-Module geeigneten Form erfasst.</translation>
    </message>
<message>
        <source>GENERATE_QHP</source>
            <translation>Wenn das \c GENERATE_QHP-Tag auf \c YES gesetzt ist und sowohl \ref cfg_qhp_namespace "QHP_NAMESPACE" als auch \ref cfg_qhp_virtual_folder "QHP_VIRTUAL_FOLDER" gesetzt sind, wird eine zusatzliche Indexdatei generiert, die als Eingabe fur Qt's qhelpgenerator verwendet werden kann, um eine Qt Compressed Help (`.qch`) der generierten HTML-Dokumentation zu erstellen.</translation>
    </message>
<message>
        <source>GENERATE_REQUIREMENTS</source>
            <translation>Das \c GENERATE_REQUIREMENTS-Tag kann verwendet werden, um die Anforderungen-Seite zu aktivieren (\c YES) oder zu deaktivieren (\c NO). Wenn aktiviert, wird diese Seite automatisch erstellt, wenn mindestens ein Kommentarblock mit einem \ref cmdrequirement "\\requirement"-Befehl in der Eingabe erscheint.</translation>
    </message>
<message>
        <source>GENERATE_RTF</source>
            <translation>Wenn das \c GENERATE_RTF-Tag auf \c YES gesetzt ist, generiert Doxygen RTF-Ausgabe. Die RTF-Ausgabe ist fur Word 97 optimiert und kann mit anderen RTF-Lesern/Editoren nicht sehr schon aussehen.</translation>
    </message>
<message>
        <source>GENERATE_SQLITE3</source>
            <translation>Wenn das \c GENERATE_SQLITE3-Tag auf \c YES gesetzt ist, generiert Doxygen eine \c Sqlite3-Datenbank mit Symbolen, die von der Suchmaschine verwendet werden kann.</translation>
    </message>
<message>
        <source>GENERATE_TAGFILE</source>
            <translation>Wenn ein Dateiname nach \c GENERATE_TAGFILE angegeben wird, erstellt Doxygen eine Tag-Datei, die von einem anderen Projekt verwendet werden kann, um Verweise auf dieses Projekt zu erstellen.</translation>
    </message>
<message>
        <source>GENERATE_TESTLIST</source>
            <translation>Das \c GENERATE_TESTLIST-Tag kann verwendet werden, um die Testliste zu aktivieren (\c YES) oder zu deaktivieren (\c NO). Diese Liste wird erstellt, indem \ref cmdtest "\\test"-Befehle in der Dokumentation platziert werden.</translation>
    </message>
<message>
        <source>GENERATE_TODOLIST</source>
            <translation>Das \c GENERATE_TODOLIST-Tag kann verwendet werden, um die Todo-Liste zu aktivieren (\c YES) oder zu deaktivieren (\c NO). Diese Liste wird erstellt, indem \ref cmdtodo "\\todo"-Befehle in der Dokumentation platziert werden.</translation>
    </message>
<message>
        <source>GENERATE_TREEVIEW</source>
            <translation>Das \c GENERATE_TREEVIEW-Tag wird verwendet, um anzugeben, ob eine baumartige Indexstruktur generiert werden soll, um hierarchische Informationen anzuzeigen. Wenn der Tag-Wert auf \c YES gesetzt ist, wird ein Seitenpanel generiert, das eine baumartige Indexstruktur enthalt (genau wie die, die fur HTML-Hilfe generiert wird). Dafur ist ein Browser erforderlich, der JavaScript, DHTML, CSS und Frames unterstutzt (d.h. jeder moderne Browser). Windows-Benutzer sind wahrscheinlich besser mit der HTML-Hilfe-Funktion bedient. Uber benutzerdefinierte Stylesheets (siehe \ref cfg_html_extra_stylesheet "HTML_EXTRA_STYLESHEET") kann man das Aussehen des Index weiter verfeinern (siehe \ref doxygen_finetune). Als Beispiel hat das von Doxygen generierte Standard-Stylesheet ein Beispiel, das zeigt, wie man ein Bild an der Wurzel des Baums statt des \ref cfg_project_name "PROJECT_NAME" platziert.</translation>
    </message>
<message>
        <source>GENERATE_XML</source>
            <translation>Wenn das \c GENERATE_XML-Tag auf \c YES gesetzt ist, generiert Doxygen eine XML-Datei, die den Code und die Dokumentation erfasst.</translation>
    </message>
<message>
        <source>GRAPHICAL_HIERARCHY</source>
            <translation>Wenn das \c GRAPHICAL_HIERARCHY-Tag auf \c YES gesetzt ist, zeigt Doxygen eine grafische Hierarchie aller Klassen an. Beachten Sie, dass die Aktivierung dieses Tags die Einstellung \ref cfg_have_dot "HAVE_DOT" auf \c YES voraussetzt.</translation>
    </message>
<message>
        <source>GROUP_GRAPHS</source>
            <translation>Wenn das \c GROUP_GRAPHS-Tag auf \c YES gesetzt ist, generiert Doxygen einen Graphen fur Gruppen, der die direkten Gruppenabhangigkeiten zeigt. Das explizite Aktivieren eines Gruppenabhangigkeitsgraphen, wenn \c GROUP_GRAPHS auf \c NO gesetzt ist, kann mit dem Befehl \ref cmdgroupgraph "\\groupgraph" erreicht werden. Das Deaktivieren eines Gruppenabhangigkeitsgraphen kann mit dem Befehl \ref cmdhidegroupgraph "\\hidegroupgraph" erreicht werden. Siehe auch das Kapitel \ref grouping "Grouping" im Handbuch.</translation>
    </message>
<message>
        <source>GROUP_NESTED_COMPOUNDS</source>
            <translation>Wenn eine Struktur oder Klasse zu einer Gruppe hinzugefugt wird und diese Option aktiviert ist, werden alle verschachtelten Klassen oder Strukturen ebenfalls derselben Gruppe hinzugefugt. Standardmäßig ist diese Option deaktiviert und man muss verschachtelte Verbindungen explizit uber \ref cmdingroup "\\ingroup" hinzufugen.</translation>
    </message>
<message>
        <source>HAVE_DOT</source>
            <translation>Wenn Sie das \c HAVE_DOT-Tag auf \c YES setzen, geht Doxygen davon aus, dass das \c dot-Tool im \c path verfugbar ist. Dieses Tool ist Teil von &lt;a href="https://www.graphviz.org/"&gt;Graphviz&lt;/a&gt;, einem Graphvisualisierungstoolkit von AT&amp;T und Lucent Bell Labs. Die anderen Optionen in diesem Abschnitt haben keine Auswirkung, wenn diese Option auf \c NO gesetzt ist.</translation>
    </message>
<message>
        <source>HHC_LOCATION</source>
            <translation>Das \c HHC_LOCATION-Tag kann verwendet werden, um den Speicherort (absoluter Pfad einschließlich Dateiname) des HTML-Help-Compilers (\c hhc.exe) anzugeben. Wenn nicht leer, wird Doxygen versuchen, den HTML-Help-Compiler auf der generierten \c index.hhp-Datei auszufuhren.</translation>
    </message>
<message>
        <source>HIDE_COMPOUND_REFERENCE</source>
            <translation>Wenn das \c HIDE_COMPOUND_REFERENCE-Tag auf \c NO (Standard) gesetzt ist, wird Doxygen zusatzlichen Text an den Titel einer Seite anhangen, wie z.B. Klassenreferenz. Wenn auf \c YES gesetzt, wird die Verbundreferenz ausgeblendet.</translation>
    </message>
<message>
        <source>HIDE_FRIEND_COMPOUNDS</source>
            <translation>Wenn das \c HIDE_FRIEND_COMPOUNDS-Tag auf \c YES gesetzt ist, werden alle Friend-Deklarationen ausgeblendet. Wenn auf \c NO gesetzt, werden diese Deklarationen in die Dokumentation aufgenommen.</translation>
    </message>
<message>
        <source>HIDE_IN_BODY_DOCS</source>
            <translation>Wenn das \c HIDE_IN_BODY_DOCS-Tag auf \c YES gesetzt ist, wird Doxygen alle Dokumentationsblöcke ausblenden, die im Korper einer Funktion gefunden werden. Wenn auf \c NO gesetzt, werden diese Blöcke an den detaillierten Dokumentationsblock der Funktion angehangt.</translation>
    </message>
<message>
        <source>HIDE_SCOPE_NAMES</source>
            <translation>Wenn das \c HIDE_SCOPE_NAMES-Tag auf \c NO gesetzt ist, zeigt Doxygen Mitglieder mit ihren vollstandigen Klassen- und Namensraumbereichen in der Dokumentation an. Wenn auf \c YES gesetzt, wird der Bereich ausgeblendet.</translation>
    </message>
<message>
        <source>HIDE_UNDOC_CLASSES</source>
            <translation>Wenn das \c HIDE_UNDOC_CLASSES-Tag auf \c YES gesetzt ist, blendet Doxygen alle undokumentierten Klassen aus, die normalerweise in der Klassenhierarchie sichtbar sind. Wenn auf \c NO gesetzt, werden diese Klassen in den verschiedenen Ubersichten aufgenommen. Diese Option blendet auch undokumentierte C++-Konzepte aus, falls aktiviert. Diese Option hat keine Auswirkung, wenn \ref cfg_extract_all "EXTRACT_ALL" aktiviert ist.</translation>
    </message>
<message>
        <source>HIDE_UNDOC_MEMBERS</source>
            <translation>Wenn das \c HIDE_UNDOC_MEMBERS-Tag auf \c YES gesetzt ist, blendet Doxygen alle undokumentierten Mitglieder innerhalb dokumentierter Klassen oder Dateien aus. Wenn auf \c NO gesetzt, werden diese Mitglieder in die verschiedenen Ubersichten aufgenommen, aber es wird kein Dokumentationsabschnitt generiert. Diese Option hat keine Auswirkung, wenn \ref cfg_extract_all "EXTRACT_ALL" aktiviert ist.</translation>
    </message>
<message>
        <source>HIDE_UNDOC_NAMESPACES</source>
            <translation>Wenn das \c HIDE_UNDOC_NAMESPACES-Tag auf \c YES gesetzt ist, blendet Doxygen alle undokumentierten Namensraume aus, die normalerweise in der Namensraum-Hierarchie sichtbar sind. Wenn auf \c NO gesetzt, werden diese Namensraume in die verschiedenen Ubersichten aufgenommen. Diese Option hat keine Auswirkung, wenn \ref cfg_extract_all "EXTRACT_ALL" aktiviert ist.</translation>
    </message>
<message>
        <source>HIDE_UNDOC_RELATIONS</source>
            <translation>Wenn auf \c YES gesetzt, blenden die Vererbungs- und Kollaborationsgraphen Vererbungs- und Verwendungsbeziehungen aus, wenn das Ziel undokumentiert ist oder keine Klasse ist.</translation>
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
            <translation>Das \c HTML_COLORSTYLE_GAMMA-Tag steuert die Gammakorrektur, die auf die Luminanzkomponente der Farben in der HTML-Ausgabe angewendet wird. Werte unter 100 machen die Ausgabe allmahlich heller, wahrend Werte uber 100 die Ausgabe dunkler machen. Der Wert geteilt durch 100 ist das tatsachlich angewendete Gamma, also reprasentiert 80 ein Gamma von 0.8, der Wert 220 reprasentiert ein Gamma von 2.2, und 100 andert das Gamma nicht.</translation>
    </message>
<message>
        <source>HTML_COLORSTYLE_HUE</source>
            <translation>Das \c HTML_COLORSTYLE_HUE-Tag steuert die Farbe der HTML-Ausgabe. Doxygen passt die Farben im Stylesheet und in den Hintergrundbildern entsprechend dieser Farbe an. Farbton wird als Winkel auf einem Farbkreis angegeben, siehe https://en.wikipedia.org/wiki/Hue fur weitere Informationen. Zum Beispiel reprasentiert der Wert 0 Rot, 60 ist Gelb, 120 ist Grün, 180 ist Cyan, 240 ist Blau, 300 ist Lila und 360 ist wieder Rot.</translation>
    </message>
<message>
        <source>HTML_COLORSTYLE_SAT</source>
            <translation>Das \c HTML_COLORSTYLE_SAT-Tag steuert die Reinheit (oder Sattigung) der Farben in der HTML-Ausgabe. Fur einen Wert von 0 verwendet die Ausgabe nur Graustufen. Ein Wert von 255 erzeugt die lebhaftesten Farben.</translation>
    </message>
<message>
        <source>HTML_COPY_CLIPBOARD</source>
            <translation>Wenn das \c HTML_COPY_CLIPBOARD-Tag auf \c YES gesetzt ist, zeigt Doxygen ein Symbol in der oberen rechten Ecke von Code- und Textfragmenten an, das es dem Benutzer ermoglicht, den Inhalt in die Zwischenablage zu kopieren. Beachten Sie, dass dies nur funktioniert, wenn es vom Browser unterstutzt wird und die Webseite uber einen &lt;a href="https://www.w3.org/TR/secure-contexts/"&gt;sicheren Kontext&lt;/a&gt; bereitgestellt wird, d.h. uber das https: oder file: Protokoll.</translation>
    </message>
<message>
        <source>HTML_DYNAMIC_MENUS</source>
            <translation>Wenn das \c HTML_DYNAMIC_MENUS-Tag auf \c YES gesetzt ist, enthalt die generierte HTML-Dokumentation einen Hauptindex mit vertikalen Navigationsmenus, die dynamisch uber JavaScript erstellt werden. Wenn deaktiviert, besteht der Navigationsindex aus mehreren Ebenen von Tabs, die statisch in jede HTML-Seite eingebettet sind. Deaktivieren Sie diese Option, um Browser zu unterstutzen, die kein JavaScript haben, wie den Qt-Hilfe-Browser.</translation>
    </message>
<message>
        <source>HTML_DYNAMIC_SECTIONS</source>
            <translation>Wenn das \c HTML_DYNAMIC_SECTIONS-Tag auf \c YES gesetzt ist, enthalt die generierte HTML-Dokumentation Abschnitte, die nach dem Laden der Seite ein- und ausgeblendet werden konnen.</translation>
    </message>
<message>
        <source>HTML_EXTRA_FILES</source>
            <translation>Das \c HTML_EXTRA_FILES-Tag kann verwendet werden, um eine oder mehrere zusatzliche Bilder oder andere Quelldateien anzugeben, die in das HTML-Ausgabeverzeichnis kopiert werden sollen. Beachten Sie, dass diese Dateien in das Basis-HTML-Ausgabeverzeichnis kopiert werden. Verwenden Sie den Marker &lt;code&gt;$relpath^&lt;/code&gt; in den Dateien \ref cfg_html_header "HTML_HEADER" und/oder \ref cfg_html_footer "HTML_FOOTER", um diese Dateien zu laden. In der Datei \ref cfg_html_stylesheet "HTML_STYLESHEET" verwenden Sie nur den Dateinamen. Beachten Sie auch, dass die Dateien wie besehen kopiert werden; es sind keine Befehle oder Marker verfugbar.</translation>
    </message>
<message>
        <source>HTML_EXTRA_STYLESHEET</source>
            <translation>Das \c HTML_EXTRA_STYLESHEET-Tag kann verwendet werden, um zusatzliche benutzerdefinierte Cascading Stylesheets anzugeben, die nach den von Doxygen erstellten Standard-Stylesheets eingebunden werden. Mit dieser Option konnen bestimmte Stilaspekte uberschrieben werden. Dies ist der Verwendung von \ref cfg_html_stylesheet "HTML_STYLESHEET" vorzuziehen, da es das Standard-Stylesheet nicht ersetzt und daher robuster gegen zukunftige Updates ist. Doxygen kopiert die Stylesheet-Dateien in das Ausgabeverzeichnis. \note Die Reihenfolge der zusatzlichen Stylesheet-Dateien ist wichtig (z.B. uberschreibt das letzte Stylesheet in der Liste die Einstellungen der vorherigen in der Liste). \note Da die Gestaltung von Scrollleisten in Webkit/Chromium derzeit nicht uberschrieben werden kann, wird die Gestaltung aus der Standard-datei doxygen.css weggelassen, wenn ein oder mehrere zusatzliche Stylesheets angegeben wurden. Wenn also eine Anpassung der Scrollleisten gewunscht wird, muss diese explizit hinzugefugt werden. Fur ein Beispiel siehe die Dokumentation.</translation>
    </message>
<message>
        <source>HTML_FILE_EXTENSION</source>
            <translation>Das \c HTML_FILE_EXTENSION-Tag kann verwendet werden, um die Dateierweiterung fur jede generierte HTML-Seite anzugeben (zum Beispiel: &lt;code&gt;.htm, .php, .asp&lt;/code&gt;).</translation>
    </message>
<message>
        <source>HTML_FOOTER</source>
            <translation>Das \c HTML_FOOTER-Tag kann verwendet werden, um eine benutzerdefinierte HTML-Fuszzeile fur jede generierte HTML-Seite anzugeben. Wenn das Tag leer gelassen wird, generiert Doxygen eine Standard-Fuszzeile. Siehe \ref cfg_html_header "HTML_HEADER" fur weitere Informationen zum Generieren einer Standard-Fuszzeile und welche speziellen Befehle innerhalb der Fuszzeile verwendet werden konnen. Siehe auch Abschnitt \ref doxygen_usage fur Informationen zum Generieren der Standard-Fuszzeile, die Doxygen normalerweise verwendet.</translation>
    </message>
<message>
        <source>HTML_FORMULA_FORMAT</source>
            <translation>Wenn die \c HTML_FORMULA_FORMAT-Option auf \c svg gesetzt ist, verwendet Doxygen das Tool pdf2svg (siehe https://github.com/dawbarton/pdf2svg) oder Inkscape (siehe https://inkscape.org), um Formeln als SVG-Bilder anstelle von PNGs fur die HTML-Ausgabe zu generieren. Diese Bilder sehen bei skalierten Auflosungen im Allgemeinen besser aus.</translation>
    </message>
<message>
        <source>HTML_HEADER</source>
            <translation>Das \c HTML_HEADER-Tag kann verwendet werden, um eine benutzerdefinierte HTML-Header-Datei fur jede generierte HTML-Seite anzugeben. Wenn das Tag leer gelassen wird, generiert Doxygen einen Standard-Header. &lt;br&gt;Um gultiges HTML zu erhalten, muss die Header-Datei alle Skripte und Stylesheets enthalten, die Doxygen benotigt, was von den verwendeten Konfigurationsoptionen abhangt (z.B. die Einstellung \ref cfg_generate_treeview "GENERATE_TREEVIEW"). Es wird dringend empfohlen, mit einem Standard-Header zu beginnen, indem Sie \verbatim doxygen -w html new_header.html new_footer.html new_stylesheet.css YourConfigFile \endverbatim verwenden und dann die Datei \c new_header.html andern. Siehe auch Abschnitt \ref doxygen_usage fur Informationen zum Generieren des Standard-Headers, den Doxygen normalerweise verwendet. @note Der Header kann sich andern, daher mussen Sie normalerweise den Standard-Header regenerieren, wenn Sie auf eine neuere Version von Doxygen aktualisieren. Fur eine Beschreibung der moglichen Marker und Blocknamen siehe die Dokumentation.</translation>
    </message>
<message>
        <source>HTML_INDEX_NUM_ENTRIES</source>
            <translation>Mit \c HTML_INDEX_NUM_ENTRIES kann die bevorzugte Anzahl von Eintragen gesteuert werden, die anfanglich in den verschiedenen baumstrukturierten Indizes angezeigt werden; der Benutzer kann Eintrage spater dynamisch erweitern und reduzieren. Doxygen erweitert den Baum auf eine solche Ebene, dass hochstens die angegebene Anzahl von Eintragen sichtbar ist (es sei denn, ein vollstandig reduzierter Baum uberschreitet diesen Betrag bereits). Das Setzen der Anzahl der Eintrage auf 1 erzeugt standardmäßig einen vollstandig reduzierten Baum. 0 ist ein spezieller Wert, der eine unendliche Anzahl von Eintragen reprasentiert und standardmäßig zu einem vollstandig erweiterten Baum fuhrt.</translation>
    </message>
<message>
        <source>HTML_OUTPUT</source>
            <translation>Das \c HTML_OUTPUT-Tag wird verwendet, um anzugeben, wo die HTML-Dokumente platziert werden. Wenn ein relativer Pfad eingegeben wird, wird der Wert von \ref cfg_output_directory "OUTPUT_DIRECTORY" davor gesetzt.</translation>
    </message>
<message>
        <source>HTML_PROJECT_COOKIE</source>
            <translation>Doxygen speichert einige Einstellungen dauerhaft im Browser (z.B. uber Cookies). Standardmäßig gelten diese Einstellungen fur alle von Doxygen generierten HTML-Seiten uber alle Projekte hinweg. Das \c HTML_PROJECT_COOKIE-Tag kann verwendet werden, um die Einstellungen unter einem projektspezifischen Schlussel zu speichern, sodass die Benutzereinstellungen separat gespeichert werden.</translation>
    </message>
<message>
        <source>HTML_STYLESHEET</source>
            <translation>Das \c HTML_STYLESHEET-Tag kann verwendet werden, um ein benutzerdefiniertes Cascading Stylesheet anzugeben, das von jeder HTML-Seite verwendet wird. Es kann verwendet werden, um das Aussehen der HTML-Ausgabe zu optimieren. Wenn leer gelassen, generiert Doxygen ein Standard-Stylesheet. Siehe auch Abschnitt \ref doxygen_usage fur Informationen zum Generieren des Stylesheets, das Doxygen normalerweise verwendet. \note Es wird empfohlen, \ref cfg_html_extra_stylesheet "HTML_EXTRA_STYLESHEET" anstelle dieses Tags zu verwenden, da es robuster ist und dieses Tag (&lt;code&gt;HTML_STYLESHEET&lt;/code&gt;) wird in Zukunft veraltet sein.</translation>
    </message>
<message>
        <source>IDL_PROPERTY_SUPPORT</source>
            <translation>Fur Microsofts IDL gibt es die Attribute \c propget und \c propput, um Getter- und Setter-Methoden fur eine Eigenschaft anzuzeigen. Das Setzen dieser Option auf \c YES fuhrt dazu, dass Doxygen die get- und set-Methoden in der Dokumentation durch die Eigenschaft ersetzt. Dies funktioniert nur, wenn die Methoden tatsachlich einen einfachen Typ abrufen oder setzen. Wenn dies nicht der Fall ist oder Sie die Methoden trotzdem anzeigen mochten, sollten Sie diese Option auf \c NO setzen.</translation>
    </message>
<message>
        <source>IGNORE_PREFIX</source>
            <translation>Das \c IGNORE_PREFIX-Tag kann verwendet werden, um ein Prax (oder eine Liste von Praxen) anzugeben, das beim Generieren der Index-Header ignoriert werden soll. Das \c IGNORE_PREFIX-Tag funktioniert fur Klassen-, Funktions- und Mitgliedsnamen. Die Entitat wird in der alphabetischen Liste unter dem ersten Buchstaben des Entitatsnamens platziert, der nach dem Entfernen des Praxes ubrig bleibt.</translation>
    </message>
<message>
        <source>IMAGE_PATH</source>
            <translation>Das \c IMAGE_PATH-Tag kann verwendet werden, um ein oder mehrere Verzeichnisse anzugeben, die Bilder enthalten, die in die Dokumentation eingebunden werden (siehe den Befehl \ref cmdimage "\\image").</translation>
    </message>
<message>
        <source>IMPLICIT_DIR_DOCS</source>
            <translation>Wenn das \c IMPLICIT_DIR_DOCS-Tag auf \c YES gesetzt ist, wird jede `README.md`-Datei, die in Unterverzeichnissen des Projektstamms gefunden wird, als Dokumentation fur dieses Unterverzeichnis verwendet, außer wenn die `README.md` mit einem \ref cmddir "\\dir", \ref cmdpage "\\page" oder \ref cmdmainpage "\\mainpage"-Befehl beginnt. Wenn auf \c NO gesetzt, muss die `README.md`-Datei mit einem expliziten \ref cmddir "\\dir"-Befehl beginnen, um als Verzeichnisdokumentation verwendet zu werden.</translation>
    </message>
<message>
        <source>INCLUDED_BY_GRAPH</source>
            <translation>Wenn das \c INCLUDED_BY_GRAPH, \ref cfg_enable_preprocessing "ENABLE_PREPROCESSING" und \ref cfg_search_includes "SEARCH_INCLUDES"-Tag auf \c YES gesetzt sind, generiert Doxygen fur jede dokumentierte Datei einen Graphen, der die direkten und indirekten Include-Abhangigkeiten der Datei mit anderen dokumentierten Dateien zeigt. Das explizite Aktivieren eines Included-by-Graphen, wenn \c INCLUDED_BY_GRAPH auf \c NO gesetzt ist, kann mit dem Befehl \ref cmdincludedbygraph "\\includedbygraph" erreicht werden. Das Deaktivieren eines Included-by-Graphen kann mit dem Befehl \ref cmdhideincludedbygraph "\\hideincludedbygraph" erreicht werden.</translation>
    </message>
<message>
        <source>INCLUDE_FILE_PATTERNS</source>
            <translation>Sie konnen das \c INCLUDE_FILE_PATTERNS-Tag verwenden, um ein oder mehrere Platzhaltermuster (wie `*.h` und `*.hpp`) anzugeben, um die Header-Dateien in den Verzeichnissen zu filtern. Wenn leer gelassen, werden die mit \ref cfg_file_patterns "FILE_PATTERNS" angegebenen Muster verwendet.</translation>
    </message>
<message>
        <source>INCLUDE_GRAPH</source>
            <translation>Wenn das \c INCLUDE_GRAPH-Tag auf \c YES gesetzt ist, generiert Doxygen fur jede dokumentierte Datei einen Graphen, der die direkten und indirekten Include-Abhangigkeiten der Datei mit anderen dokumentierten Dateien zeigt. Das explizite Aktivieren eines Include-Graphen, wenn \c INCLUDE_GRAPH auf \c NO gesetzt ist, kann mit dem Befehl \ref cmdincludegraph "\\includegraph" erreicht werden. Das Deaktivieren eines Include-Graphen kann mit dem Befehl \ref cmdhideincludegraph "\\hideincludegraph" erreicht werden.</translation>
    </message>
<message>
        <source>INCLUDE_PATH</source>
            <translation>Das \c INCLUDE_PATH-Tag kann verwendet werden, um ein oder mehrere Verzeichnisse anzugeben, die Include-Dateien enthalten, die keine Eingabedateien sind, aber vom Praprozessor verarbeitet werden sollen. Beachten Sie, dass der \c INCLUDE_PATH nicht rekursiv ist, sodass die Einstellung von \ref cfg_recursive "RECURSIVE" hier keine Auswirkung hat.</translation>
    </message>
<message>
        <source>INHERIT_DOCS</source>
            <translation>Wenn das \c INHERIT_DOCS-Tag auf \c YES gesetzt ist, erbt ein undokumentiertes Mitglied die Dokumentation von jedem dokumentierten Mitglied, das es neu implementiert.</translation>
    </message>
<message>
        <source>INLINE_GROUPED_CLASSES</source>
            <translation>Wenn das \c INLINE_GROUPED_CLASSES-Tag auf \c YES gesetzt ist, werden Klassen, Strukturen und Unions innerhalb der Gruppe angezeigt, in der sie enthalten sind (z.B. mit \ref cmdingroup "\\ingroup") anstatt auf einer separaten Seite (fur HTML und Man-Pages) oder in einem Abschnitt (fur \f$\mbox{\LaTeX}\f$ und RTF). &lt;br&gt;Beachten Sie, dass diese Funktion nicht in Kombination mit \ref cfg_separate_member_pages "SEPARATE_MEMBER_PAGES" funktioniert.</translation>
    </message>
<message>
        <source>INLINE_INFO</source>
            <translation>Wenn das \c INLINE_INFO-Tag auf \c YES gesetzt ist, wird ein Tag [inline] in die Dokumentation fur Inline-Mitglieder eingefugt.</translation>
    </message>
<message>
        <source>INLINE_INHERITED_MEMB</source>
            <translation>Wenn das \c INLINE_INHERITED_MEMB-Tag auf \c YES gesetzt ist, zeigt Doxygen alle geerbten Mitglieder einer Klasse in der Dokumentation dieser Klasse an, als ob diese Mitglieder gewohnliche Klassenmitglieder waren. Konstruktoren, Destruktoren und Zuweisungsoperatoren der Basisklassen werden nicht angezeigt.</translation>
    </message>
<message>
        <source>INLINE_SIMPLE_STRUCTS</source>
            <translation>Wenn das \c INLINE_SIMPLE_STRUCTS-Tag auf \c YES gesetzt ist, werden Strukturen, Klassen und Unions mit nur offentlichen Datenfeldern oder einfachen Typedef-Feldern inline in der Dokumentation des Bereichs angezeigt, in dem sie definiert sind (d.h. Datei-, Namensraum- oder Gruppendokumentation), vorausgesetzt dieser Bereich ist dokumentiert. Wenn auf \c NO gesetzt, werden Strukturen, Klassen und Unions auf einer separaten Seite (fur HTML und Man-Pages) oder in einem Abschnitt (fur \f$\mbox{\LaTeX}\f$ und RTF) angezeigt.</translation>
    </message>
<message>
        <source>INLINE_SOURCES</source>
            <translation>Wenn das \c INLINE_SOURCES-Tag auf \c YES gesetzt ist, wird der Korper der Funktionen, Klassen und Namensraume direkt in die Dokumentation eingefugt.</translation>
    </message>
<message>
        <source>INPUT</source>
            <translation>Das \c INPUT-Tag wird verwendet, um die Dateien und/oder Verzeichnisse anzugeben, die die Quellcode- und anderen Dokumentationsdateien enthalten. Sie konnen Dateinamen wie \c myfile.cpp oder Verzeichnisse wie \c /usr/src/myproject eingeben. Trennen Sie die Dateien oder Verzeichnisse durch Leerzeichen. Siehe auch \ref cfg_file_patterns "FILE_PATTERNS" und \ref cfg_extension_mapping "EXTENSION_MAPPING". \note Wenn dieses Tag leer ist, wird das aktuelle Verzeichnis durchsucht.</translation>
    </message>
<message>
        <source>INPUT_ENCODING</source>
            <translation>Dieses Tag kann verwendet werden, um die Zeichencodierung der Quelldateien anzugeben, die Doxygen analysiert. Intern verwendet Doxygen die UTF-8-Codierung. Doxygen verwendet `libiconv` (oder das in `libc` eingebaute `iconv`) fur die Transkodierung. Siehe &lt;a href="https://www.gnu.org/software/libiconv/"&gt;die libiconv-Dokumentation&lt;/a&gt; fur die Liste der moglichen Codierungen. \sa \ref cfg_input_file_encoding "INPUT_FILE_ENCODING"</translation>
    </message>
<message>
        <source>INPUT_FILE_ENCODING</source>
            <translation>Dieses Tag kann verwendet werden, um die Zeichencodierung der Quelldateien anzugeben, die Doxygen analysiert. Das \c INPUT_FILE_ENCODING-Tag kann verwendet werden, um die Zeichencodierung pro Dateimuster anzugeben. Doxygen vergleicht den Dateinamen mit jedem Muster und wendet die Codierung anstelle der Standard-\ref cfg_input_encoding "INPUT_ENCODING" an, wenn eine Ubereinstimmung vorliegt. Die Zeichencodierungen sind eine Liste der Form: Muster=Codierung (wie `*.php=ISO-8859-1`). \sa \ref cfg_input_encoding "INPUT_ENCODING" fur weitere Informationen zu unterstutzten Codierungen.</translation>
    </message>
<message>
        <source>INPUT_FILTER</source>
            <translation>Das \c INPUT_FILTER-Tag kann verwendet werden, um ein Programm anzugeben, das Doxygen fur jede Eingabedatei aufrufen soll. Doxygen ruft das Filterprogramm auf, indem es (uber &lt;code&gt;popen()&lt;/code&gt;) den Befehl ausfuhrt: &lt;br&gt; &lt;code&gt;\&lt;filter\&gt; \&lt;input-file\&gt;&lt;/code&gt; &lt;br&gt; wobei &lt;code&gt;\&lt;filter\&gt;&lt;/code&gt; der Wert des \c INPUT_FILTER-Tags ist und &lt;code&gt;\&lt;input-file\&gt;&lt;/code&gt; der Name einer Eingabedatei ist. Doxygen verwendet dann die Ausgabe, die das Filterprogramm auf die Standardausgabe schreibt. Wenn \ref cfg_filter_patterns "FILTER_PATTERNS" angegeben ist, wird dieses Tag ignoriert. &lt;br&gt;Beachten Sie, dass der Filter keine Zeilen hinzufugen oder entfernen darf; er wird angewendet, bevor der Code gescannt wird, aber nicht wenn der Ausgabecode generiert wird. Wenn Zeilen hinzugefugt oder entfernt werden, werden die Anker nicht korrekt platziert. &lt;br&gt;Beachten Sie, dass Doxygen die verarbeiteten und auf die Standardausgabe geschriebenen Daten fur die weitere Verarbeitung verwendet, daher sollte nichts anderes, wie Debug-Anweisungen oder verwendete Befehle (also im Fall einer Windows-Batch-Datei immer `@echo OFF` verwenden), auf die Standardausgabe geschrieben werden. &lt;br&gt;Beachten Sie, dass fur benutzerdefinierte Erweiterungen oder nicht direkt unterstutzte Erweiterungen Sie auch \ref cfg_extension_mapping "EXTENSION_MAPPING" fur die Erweiterung setzen mussen, sonst werden die Dateien von Doxygen nicht korrekt verarbeitet.&lt;br&gt;</translation>
    </message>
<message>
        <source>INTERACTIVE_SVG</source>
            <translation>Wenn \ref cfg_dot_image_format "DOT_IMAGE_FORMAT" auf `svg` oder `svg:svg` oder `svg:svg:core` gesetzt ist, kann diese Option auf \c YES gesetzt werden, um die Generierung interaktiver SVG-Bilder zu aktivieren, die Zoomen und Schwenken ermoglichen. &lt;br&gt;Beachten Sie, dass dies einen modernen Browser außer Internet Explorer erfordert. Getestet und funktionierend sind Firefox, Chrome, Safari und Opera. &lt;br&gt;Beachten Sie, dass diese Option automatisch deaktiviert wird, wenn \ref cfg_dot_image_format "DOT_IMAGE_FORMAT" auf `svg:cairo` oder `svg:cairo:cairo` gesetzt ist.</translation>
    </message>
<message>
        <source>INTERNAL_DOCS</source>
            <translation>Das \c INTERNAL_DOCS-Tag bestimmt, ob Dokumentation, die nach einem \ref cmdinternal "\\internal"-Befehl geschrieben wird, einbezogen wird. Wenn das Tag auf \c NO gesetzt ist, wird die Dokumentation ausgeschlossen. Setzen Sie es auf \c YES, um die interne Dokumentation einzubeziehen.</translation>
    </message>
<message>
        <source>JAVADOC_AUTOBRIEF</source>
            <translation>Wenn das \c JAVADOC_AUTOBRIEF-Tag auf \c YES gesetzt ist, interpretiert Doxygen die erste Zeile (bis zum ersten Punkt, Fragezeichen oder Ausrufezeichen) eines Javadoc-Stil-Kommentars als Kurzbeschreibung. Wenn auf \c NO gesetzt, verhalt sich der Javadoc-Stil wie regulare Qt-Stil-Kommentare (wodurch ein expliziter \ref cmdbrief "\@brief"-Befehl fur eine Kurzbeschreibung erforderlich ist).</translation>
    </message>
<message>
        <source>JAVADOC_BANNER</source>
            <translation>Wenn das \c JAVADOC_BANNER-Tag auf \c YES gesetzt ist, interpretiert Doxygen eine Zeile wie \verbatim /***************\endverbatim als den Anfang eines Javadoc-Stil-Kommentar-Banners. Wenn auf \c NO gesetzt, verhalt sich der Javadoc-Stil wie regulare Kommentare und wird nicht von Doxygen interpretiert.</translation>
    </message>
<message>
        <source>LATEX_BATCHMODE</source>
            <translation>Wenn das \c LATEX_BATCHMODE-Tag auf \c YES gesetzt ist, fuhrt Doxygen den \f$\mbox{\LaTeX}\f$-Compiler im Batch-Modus aus, was bedeutet, dass er nicht bei Fehlern anhelt.</translation>
    </message>
<message>
        <source>LATEX_BIB_STYLE</source>
            <translation>Das \c LATEX_BIB_STYLE-Tag kann verwendet werden, um den Stil der Bibliographie anzugeben, der vom \c bibtex-Tool verwendet wird. Standardmäßig ist dies auf `plainnat` gesetzt.</translation>
    </message>
<message>
        <source>LATEX_CMD_NAME</source>
            <translation>Das \c LATEX_CMD_NAME-Tag kann verwendet werden, um den aufzurufenden \f$\mbox{\LaTeX}\f$-Befehlsnamen anzugeben. &lt;br&gt;Beachten Sie, dass bei Nicht-Aktivierung von \ref cfg_use_pdflatex "USE_PDFLATEX" der Standard \c latex ist, bei Aktivierung von \ref cfg_use_pdflatex "USE_PDFLATEX" der Standard \c pdflatex ist und wenn im letzteren Fall \c latex gewahlt wird, wird dies durch \c pdflatex uberschrieben. Fur bestimmte Ausgabesprachen kann der Standard unterschiedlich festgelegt sein, dies hangt von der Implementierung der Ausgabesprache ab.</translation>
    </message>
<message>
        <source>LATEX_EMOJI_DIRECTORY</source>
            <translation>Das \c LATEX_EMOJI_DIRECTORY-Tag wird verwendet, um den Pfad anzugeben, aus dem die Emoji-Bilder gelesen werden. Wenn leer gelassen, wird das Standard-Emoji-Verzeichnis verwendet.</translation>
    </message>
<message>
        <source>LATEX_EXTRA_FILES</source>
            <translation>Das \c LATEX_EXTRA_FILES-Tag kann verwendet werden, um ein oder mehrere zusatzliche Bilder oder andere Quelldateien anzugeben, die in das \ref cfg_latex_output "LATEX_OUTPUT"-Ausgabeverzeichnis kopiert werden sollen. Beachten Sie, dass die Dateien wie sie sind kopiert werden; es sind keine Befehle oder Marker verfugbar.</translation>
    </message>
<message>
        <source>LATEX_EXTRA_STYLESHEET</source>
            <translation>Das \c LATEX_EXTRA_STYLESHEET-Tag kann verwendet werden, um zusatzliche benutzerdefinierte \f$\mbox{\LaTeX}\f$-Stylesheets anzugeben, die nach den von Doxygen erstellten Standard-Stylesheets eingebunden werden. Mit dieser Option konnen bestimmte Stilaspekte uberschrieben werden. Doxygen kopiert die Stylesheet-Dateien in das Ausgabeverzeichnis. \note Die Reihenfolge der zusatzlichen Stylesheet-Dateien ist wichtig (z.B. das letzte Stylesheet in der Liste uberschreibt die Einstellungen der vorherigen in der Liste).</translation>
    </message>
<message>
        <source>LATEX_FOOTER</source>
            <translation>Das \c LATEX_FOOTER-Tag kann verwendet werden, um einen benutzerdefinierten \f$\mbox{\LaTeX}\f$-Footer fur das generierte \f$\mbox{\LaTeX}\f$-Dokument anzugeben. Der Footer sollte alles nach dem letzten Kapitel enthalten. Wenn er leer gelassen wird, generiert Doxygen einen Standard-Footer. Siehe \ref cfg_latex_header "LATEX_HEADER" fur weitere Informationen zum Generieren eines Standard-Footers und welche speziellen Befehle im Footer verwendet werden konnen. Siehe auch Abschnitt \ref doxygen_usage fur Informationen zum Generieren des Standard-Footers, den Doxygen normalerweise verwendet. Hinweis: Verwenden Sie nur einen benutzerdefinierten Footer, wenn Sie wissen, was Sie tun!</translation>
    </message>
<message>
        <source>LATEX_HEADER</source>
            <translation>Das \c LATEX_HEADER-Tag kann verwendet werden, um einen benutzerdefinierten \f$\mbox{\LaTeX}\f$-Header fur das generierte \f$\mbox{\LaTeX}\f$-Dokument anzugeben. Der Header sollte alles bis zum ersten Kapitel enthalten. Wenn er leer gelassen wird, generiert Doxygen einen Standard-Header. Es wird dringend empfohlen, mit einem Standard-Header zu beginnen, indem Sie \verbatim doxygen -w latex new_header.tex new_footer.tex new_stylesheet.sty \endverbatim verwenden und dann die Datei \c new_header.tex andern. Siehe auch Abschnitt \ref doxygen_usage fur Informationen zum Generieren des Standard-Headers, den Doxygen normalerweise verwendet. &lt;br&gt;Hinweis: Verwenden Sie nur einen benutzerdefinierten Header, wenn Sie wissen, was Sie tun! @note Der Header kann sich andern, daher mussen Sie normalerweise den Standard-Header regenerieren, wenn Sie auf eine neuere Version von Doxygen aktualisieren. Fur eine Beschreibung der moglichen Marker und Blocknamen siehe die Dokumentation.</translation>
    </message>
<message>
        <source>LATEX_HIDE_INDICES</source>
            <translation>Wenn das \c LATEX_HIDE_INDICES-Tag auf \c YES gesetzt ist, wird Doxygen die Indexkapitel (wie Datei-Index, Verbund-Index usw.) nicht in die Ausgabe aufnehmen.</translation>
    </message>
<message>
        <source>LATEX_MAKEINDEX_CMD</source>
            <translation>Das \c LATEX_MAKEINDEX_CMD-Tag kann verwendet werden, um den Befehlsnamen zum Generieren des Index fur \f$\mbox{\LaTeX}\f$ anzugeben. Falls kein Backslash (`\`) als erstes Zeichen vorhanden ist, wird er automatisch im \f$\mbox{\LaTeX}\f$-Code hinzugefugt. @note Dieses Tag wird in der generierten Ausgabedatei (`.tex`) verwendet. \sa \ref cfg_makeindex_cmd_name "MAKEINDEX_CMD_NAME" fur den Teil in der `Makefile` / `make.bat`.</translation>
    </message>
<message>
        <source>LATEX_OUTPUT</source>
            <translation>Das \c LATEX_OUTPUT-Tag wird verwendet, um anzugeben, wo die \f$\mbox{\LaTeX}\f$-Dokumente platziert werden. Wenn ein relativer Pfad eingegeben wird, wird der Wert von \ref cfg_output_directory "OUTPUT_DIRECTORY" davor gesetzt.</translation>
    </message>
<message>
        <source>LAYOUT_FILE</source>
            <translation>Das \c LAYOUT_FILE-Tag kann verwendet werden, um eine Layout-Datei anzugeben, die von Doxygen geparst wird. Die Layout-Datei steuert die globale Struktur der generierten Ausgabedateien in einer ausgabeformatunabhangigen Weise. Um die Layout-Datei zu erstellen, die Doxygens Standards reprasentiert, fuhren Sie Doxygen mit der Option `-l` aus. Sie konnen optional einen Dateinamen nach der Option angeben, wenn weggelassen wird \c DoxygenLayout.xml als Name der Layout-Datei verwendet. Siehe auch Abschnitt \ref layout fur Informationen. &lt;br&gt;Beachten Sie, dass Doxygen automatisch eine Datei namens \c DoxygenLayout.xml analysiert, wenn Sie Doxygen aus einem Verzeichnis ausfuhren, das diese Datei enthalt, auch wenn das \c LAYOUT_FILE-Tag leer gelassen wird.</translation>
    </message>
<message>
        <source>LOOKUP_CACHE_SIZE</source>
            <translation>Die Grose des Symbol-Lookup-Caches kann mit \c LOOKUP_CACHE_SIZE festgelegt werden. Dieser Cache wird verwendet, um Symbole anhand ihres Namens und Bereichs aufzulosen. Da dies ein aufwendiger Prozess sein kann und dasselbe Symbol oft mehrmals im Code erscheint, halt Doxygen einen Cache von voraufgelosten Symbolen. Wenn der Cache zu klein ist, wird Doxygen langsamer. Wenn der Cache zu groß ist, wird Speicher verschwendet. Die Cache-Grose wird durch diese Formel gegeben: \f$2^{(16+\mbox{LOOKUP\_CACHE\_SIZE})}\f$. Der gultige Bereich ist 0..9, der Standard ist 0, was einer Cache-Grose von \f$2^{16} = 65536\f$ Symbolen entspricht. Am Ende eines Laufs meldet Doxygen die Cache-Nutzung und schlagt die optimale Cache-Grose aus Geschwindigkeitssicht vor.</translation>
    </message>
<message>
        <source>MACRO_EXPANSION</source>
            <translation>Wenn das \c MACRO_EXPANSION-Tag auf \c YES gesetzt ist, expandiert Doxygen alle Makronamen im Quellcode. Wenn auf \c NO gesetzt, wird nur bedingte Kompilierung durchgefuhrt. Makro-Expansion kann kontrolliert werden, indem \ref cfg_expand_only_predef "EXPAND_ONLY_PREDEF" auf \c YES gesetzt wird.</translation>
    </message>
<message>
        <source>MAKEINDEX_CMD_NAME</source>
            <translation>Das \c MAKEINDEX_CMD_NAME-Tag kann verwendet werden, um den Befehlsnamen zum Generieren des Index fur \f$\mbox{\LaTeX}\f$ anzugeben. @note Dieses Tag wird in der `Makefile` / `make.bat` verwendet. \sa \ref cfg_latex_makeindex_cmd "LATEX_MAKEINDEX_CMD" fur den Teil in der generierten Ausgabedatei (`.tex`).</translation>
    </message>
<message>
        <source>MAN_EXTENSION</source>
            <translation>Das \c MAN_EXTENSION-Tag bestimmt die Erweiterung, die den generierten Man-Pages hinzugefugt wird. Falls der Handbuchabschnitt nicht mit einer Zahl beginnt, wird die Zahl 3 vorangestellt. Der Punkt (.) am Anfang des \c MAN_EXTENSION-Tags ist optional.</translation>
    </message>
<message>
        <source>MAN_LINKS</source>
            <translation>Wenn das \c MAN_LINKS-Tag auf \c YES gesetzt ist und Doxygen Man-Ausgabe generiert, wird es eine zusatzliche Man-Datei fur jede Entitat generieren, die in den echten Man-Pages dokumentiert ist. Diese zusatzlichen Dateien verweisen nur auf die echte Man-Page, aber ohne sie ware der \c man-Befehl nicht in der Lage, die richtige Seite zu finden.</translation>
    </message>
<message>
        <source>MAN_OUTPUT</source>
            <translation>Das \c MAN_OUTPUT-Tag wird verwendet, um anzugeben, wo die Man-Pages platziert werden. Wenn ein relativer Pfad eingegeben wird, wird der Wert von \ref cfg_output_directory "OUTPUT_DIRECTORY" davor gesetzt. Ein Verzeichnis \c man3 wird innerhalb des von \c MAN_OUTPUT angegebenen Verzeichnisses erstellt.</translation>
    </message>
<message>
        <source>MAN_SUBDIR</source>
            <translation>Das \c MAN_SUBDIR-Tag bestimmt den Namen des Verzeichnisses, das innerhalb von \c MAN_OUTPUT erstellt wird, in dem die Man-Pages platziert werden. Standardmäßig ist dies man gefolgt von \c MAN_EXTENSION ohne den anfanglichen `.`.</translation>
    </message>
<message>
        <source>MARKDOWN_ID_STYLE</source>
            <translation>Das \c MARKDOWN_ID_STYLE-Tag kann verwendet werden, um den Algorithmus anzugeben, der zum Generieren von Bezeichnern fur Markdown-Uberschriften verwendet wird. Hinweis: Jeder Bezeichner ist eindeutig.</translation>
    </message>
<message>
        <source>MARKDOWN_STRICT</source>
            <translation>Wenn das \c MARKDOWN_STRICT-Tag aktiviert ist, behandelt Doxygen den Text in Kommentaren auch dann als Markdown-Format, wenn das native Markup-Format von Doxygen mit Markdown in Konflikt steht. Dies ist nur relevant, wenn Backticks verwendet werden. Der native Markup-Stil von Doxygen erlaubt es einem einfachen Anfuhrungszeichen, ein mit einem Backtick begonnenes Textfragment zu beenden und es dann als zitierten Text zu behandeln, wahrend in Markdown ein solches Textfragment als wortlicher Text behandelt wird und nur endet, wenn ein zweiter ubereinstimmender Backtick gefunden wird. Zudem erfordert das native Markup-Format von Doxygen, dass doppelte Anfuhrungszeichen in einem Backtick-Abschnitt maskiert werden, wahrend dies fur Markdown nicht notig ist.</translation>
    </message>
<message>
        <source>MARKDOWN_SUPPORT</source>
            <translation>Wenn das \c MARKDOWN_SUPPORT-Tag aktiviert ist, verarbeitet Doxygen alle Kommentare vorab gemab dem Markdown-Format, was eine lesbarere Dokumentation ermoglicht. Siehe https://daringfireball.net/projects/markdown/ fur Details. Die Ausgabe der Markdown-Verarbeitung wird von Doxygen weiterverarbeitet, sodass Sie Doxygen-, HTML- und XML-Befehle mit Markdown-Formatierung mischen konnen. Deaktivieren Sie dies nur bei Problemen mit der Abwartskompatibilitat.</translation>
    </message>
<message>
        <source>MATHJAX_CODEFILE</source>
            <translation>Das \c MATHJAX_CODEFILE-Tag kann verwendet werden, um eine Datei mit JavaScript-Code-Stucken anzugeben, die beim Start des MathJax-Codes verwendet werden. Siehe die MathJax-Website fur weitere Details: - &lt;a href="https://docs.mathjax.org/en/v2.7/"&gt;MathJax version 2&lt;/a&gt; - &lt;a href="https://docs.mathjax.org/en/v3.2/"&gt;MathJax version 3&lt;/a&gt; - &lt;a href="https://docs.mathjax.org/en/v4.0/"&gt;MathJax version 4&lt;/a&gt; Fur ein Beispiel siehe die Dokumentation.</translation>
    </message>
<message>
        <source>MATHJAX_EXTENSIONS</source>
            <translation>Das \c MATHJAX_EXTENSIONS-Tag kann verwendet werden, um einen oder mehrere MathJax-Erweiterungsnamen anzugeben, die wahrend des MathJax-Renderings aktiviert werden sollen. Zum Beispiel fur MathJax Version 2 (siehe https://docs.mathjax.org/en/v2.7/tex.html): \verbatim MATHJAX_EXTENSIONS = TeX/AMSmath TeX/AMSsymbols \endverbatim Zum Beispiel fur MathJax Version 3 (siehe https://docs.mathjax.org/en/v3.2/input/tex/extensions/): \verbatim MATHJAX_EXTENSIONS = ams \endverbatim Zum Beispiel fur MathJax Version 4 (siehe https://docs.mathjax.org/en/v4.0/input/tex/extensions/): \verbatim MATHJAX_EXTENSIONS = units \endverbatim Beachten Sie, dass fur MathJax Version 4 bereits einige Erweiterungen automatisch geladen werden. Um ein Paket in MathJax Version 4 zu deaktivieren, konnen Sie den Paketnamen mit einem vorangestellten Minuszeichen verwenden (`-` wie `MATHJAX_EXTENSIONS += -textmacros`)</translation>
    </message>
<message>
        <source>MATHJAX_FORMAT</source>
            <translation>Wenn MathJax aktiviert ist, konnen Sie das Standardausgabeformat festlegen, das fur die MathJax-Ausgabe verwendet wird. Fur weitere Details zum Ausgabeformat siehe &lt;a href="https://docs.mathjax.org/en/v2.7/output.html"&gt;MathJax version 2&lt;/a&gt;, &lt;a href="https://docs.mathjax.org/en/v3.2/output/index.html"&gt;MathJax version 3&lt;/a&gt; und &lt;a href="https://docs.mathjax.org/en/v4.0/output/index.htm"&gt;MathJax version 4&lt;/a&gt;.</translation>
    </message>
<message>
        <source>MATHJAX_RELPATH</source>
            <translation>Wenn MathJax aktiviert ist, mussen Sie den Speicherort relativ zum HTML-Ausgabeverzeichnis mit der Option \c MATHJAX_RELPATH angeben. Fur MathJax Version 2 sollte das Zielverzeichnis das `MathJax.js`-Skript enthalten. Wenn sich beispielsweise das \c mathjax-Verzeichnis auf derselben Ebene wie das HTML-Ausgabeverzeichnis befindet, sollte \c MATHJAX_RELPATH &lt;code&gt;../mathjax&lt;/code&gt; sein. Fur MathJax Versionen 3 und 4 sollte das Zielverzeichnis das `tex-&lt;format&gt;.js`-Skript enthalten (wobei `&lt;format&gt;` entweder `chtml` oder `svg` ist). Der Standardwert verweist auf das MathJax Content Delivery Network, sodass Sie das Ergebnis schnell sehen konnen, ohne MathJax zu installieren. Es wird jedoch dringend empfohlen, eine lokale Kopie von MathJax von https://www.mathjax.org vor der Bereitstellung zu installieren. Der Standardwert ist: - im Fall von MathJax Version 2: https://cdn.jsdelivr.net/npm/mathjax@2 - im Fall von MathJax Version 3: https://cdn.jsdelivr.net/npm/mathjax@3 - im Fall von MathJax Version 4: https://cdn.jsdelivr.net/npm/mathjax@4</translation>
    </message>
<message>
        <source>MATHJAX_VERSION</source>
            <translation>Mit \c MATHJAX_VERSION ist es moglich, die zu verwendende MathJax-Version anzugeben. Beachten Sie, dass die verschiedenen Versionen von MathJax unterschiedliche Anforderungen an die verschiedenen Einstellungen haben, sodass es moglich ist, dass auch andere MathJax-Einstellungen geandert werden mussen, wenn zwischen den verschiedenen MathJax-Versionen gewechselt wird.</translation>
    </message>
<message>
        <source>MAX_DOT_GRAPH_DEPTH</source>
            <translation>Das \c MAX_DOT_GRAPH_DEPTH-Tag kann verwendet werden, um die maximale Tiefe der von \c dot generierten Graphen festzulegen. Ein Tiefenwert von 3 bedeutet, dass nur Knoten angezeigt werden, die vom Wurzelknoten aus durch einen Pfad uber hochstens 3 Kanten erreichbar sind. Knoten, die weiter vom Wurzelknoten entfernt liegen, werden weggelassen. Beachten Sie, dass das Setzen dieser Option auf 1 oder 2 die fur große Codebasen benotigte Rechenzeit erheblich reduzieren kann. Beachten Sie auch, dass die Große eines Graphen durch \ref cfg_dot_graph_max_nodes "DOT_GRAPH_MAX_NODES" weiter eingeschrankt werden kann. Die Verwendung einer Tiefe von 0 bedeutet keine Tiefenbeschrankung.</translation>
    </message>
<message>
        <source>MAX_INITIALIZER_LINES</source>
            <translation>Das \c MAX_INITIALIZER_LINES-Tag bestimmt die maximale Anzahl von Zeilen, die der Anfangswert einer Variable oder eines Makros / Defines haben darf, um in der Dokumentation zu erscheinen. Wenn der Initialisierer aus mehr Zeilen besteht als hier angegeben, wird er ausgeblendet. Verwenden Sie den Wert 0, um Initialisierer vollstandig auszublenden. Das Erscheinen des Werts einzelner Variablen und Makros / Defines kann mit dem Befehl \ref cmdshowinitializer "\\showinitializer" oder \ref cmdhideinitializer "\\hideinitializer" in der Dokumentation gesteuert werden, unabhangig von dieser Einstellung.</translation>
    </message>
<message>
        <source>MERMAIDFILE_DIRS</source>
            <translation>Das \c MERMAIDFILE_DIRS-Tag kann verwendet werden, um ein oder mehrere Verzeichnisse anzugeben, die Mermaid-Dateien enthalten, die in die Dokumentation eingebunden werden (siehe den Befehl \ref cmdmermaidfile "\\mermaidfile").</translation>
    </message>
<message>
        <source>MERMAID_CONFIG_FILE</source>
            <translation>Bei Verwendung von Mermaid-Diagrammen kann das \c MERMAID_CONFIG_FILE-Tag verwendet werden, um eine JSON-Konfigurationsdatei fur das Mermaid-CLI-Tool (\c mmdc) anzugeben. Diese Datei kann Themeneinstellungen und andere Mermaid-Konfigurationsoptionen enthalten.</translation>
    </message>
<message>
        <source>MERMAID_JS_URL</source>
            <translation>Das \c MERMAID_JS_URL-Tag gibt die URL an, von der \c mermaid.js geladen werden soll, wenn clientseitiges Rendering verwendet wird (\c MERMAID_RENDER_MODE ist \c CLIENT_SIDE oder \c AUTO). Der Standardwert verweist auf die neueste Mermaid v11-Version auf dem jsDelivr CDN. \par Beispiele: - Neueste v11 (Standard): \c 'https://cdn.jsdelivr.net/npm/mermaid@11/dist/mermaid.esm.min.mjs' - Angeheftete Version: \c 'https://cdn.jsdelivr.net/npm/mermaid@11.3.0/dist/mermaid.esm.min.mjs' - Lokale Kopie: \c './mermaid.esm.min.mjs' (Benutzer muss Datei im HTML-Ausgabeverzeichnis platzieren) \par Die Standard-CDN-URL erfordert Internetzugang beim Anzeigen der generierten Dokumentation. Fur die Offline-Verwendung laden Sie \c mermaid.esm.min.mjs herunter und setzen Sie dies auf einen relativen Pfad, oder verwenden Sie \c MERMAID_RENDER_MODE=CLI, um stattdessen Bilder vorab zu generieren.</translation>
    </message>
<message>
        <source>MERMAID_PATH</source>
            <translation>Bei Verwendung von Mermaid-Diagrammen mit CLI-Rendering sollte das \c MERMAID_PATH-Tag verwendet werden, um das Verzeichnis anzugeben, in dem die ausfuhrbare Datei \c mmdc (Mermaid CLI) gefunden werden kann. Wenn leer gelassen, ist CLI-basiertes Rendering deaktiviert. Fur HTML-Ausgabe wird standardmäßig clientseitiges Rendering uber JavaScript verwendet und erfordert kein \c mmdc. Fur LaTeX/PDF-Ausgabe ist \c mmdc erforderlich, um Bilder vorab zu generieren. Doxygen generiert eine Warnung, wenn CLI-Rendering benotigt wird, aber \c mmdc nicht verfugbar ist.</translation>
    </message>
<message>
        <source>MERMAID_RENDER_MODE</source>
            <translation>Das \c MERMAID_RENDER_MODE-Tag steuert, wie Mermaid-Diagramme gerendert werden. Mogliche Werte sind: \c CLI (verwendet das `mmdc`-Tool von der Befehlszeile), \c CLIENT (verwendet mermaid.js im Browser) oder \c SERVER (verwendet mermaid.ink als externen Dienst). Standardmäßig ist dies auf \c CLIENT gesetzt.</translation>
    </message>
<message>
        <source>MSCFILE_DIRS</source>
            <translation>Das \c MSCFILE_DIRS-Tag kann verwendet werden, um ein oder mehrere Verzeichnisse anzugeben, die MSC-Dateien enthalten, die in die Dokumentation eingebunden werden (siehe den Befehl \ref cmdmscfile "\\mscfile").</translation>
    </message>
<message>
        <source>MSCGEN_TOOL</source>
            <translation>Sie konnen Nachrichtensequenzdiagramme in Doxygen-Kommentaren mit dem Befehl \ref cmdmsc "\\msc" definieren. Wenn das \c MSCGEN_TOOL-Tag leer gelassen wird (der Standard), verwendet Doxygen eine eingebaute Version des mscgen-Tools, um die Diagramme zu erstellen. Alternativ kann das \c MSCGEN_TOOL-Tag auch den Namen eines externen Tools angeben. Wenn Sie beispielsweise `prog` als Wert angeben, ruft Doxygen das Tool als `prog -T &lt;ausgabedateiformat&gt; -o &lt;ausgabedatei&gt; &lt;eingabedatei&gt;` auf. Das externe Tool sollte die Ausgabedateiformate "png", "eps", "svg" und "ismap" unterstutzen.</translation>
    </message>
<message>
        <source>MULTILINE_CPP_IS_BRIEF</source>
            <translation>Das \c MULTILINE_CPP_IS_BRIEF-Tag kann auf \c YES gesetzt werden, damit Doxygen einen mehrzeiligen C++-Spezialkommentarblock (d.h. einen Block aus \c //! oder \c /// Kommentaren) als Kurzbeschreibung behandelt. Dies war frueher das Standardverhalten. Der neue Standard ist, einen mehrzeiligen C++-Kommentarblock als detaillierte Beschreibung zu behandeln. Setzen Sie dieses Tag auf \c YES, wenn Sie das alte Verhalten bevorzugen. &lt;br&gt;Beachten Sie, dass das Setzen dieses Tags auf \c YES auch bedeutet, dass Rational Rose-Kommentare nicht mehr erkannt werden.</translation>
    </message>
<message>
        <source>NUM_PROC_THREADS</source>
            <translation>Das \c NUM_PROC_THREADS gibt die Anzahl der Threads an, die Doxygen wahrend der Verarbeitung verwenden darf. Wenn auf \c 0 gesetzt, wird Doxygen dies auf der Anzahl der im System verfugbaren Kerne basieren. Sie konnen es explizit auf einen Wert groser als 0 setzen, um mehr Kontrolle uber das Gleichgewicht zwischen CPU-Last und Verarbeitungsgeschwindigkeit zu erhalten. Im Moment kann nur die Eingabeverarbeitung mit mehreren Threads durchgefuhrt werden. Da dies noch eine experimentelle Funktion ist, ist der Standard auf 1 gesetzt, was die parallele Verarbeitung effektiv deaktiviert. Bitte melden Sie alle Probleme, die Sie feststellen. Das parallele Generieren von dot-Graphen wird durch die Einstellung \c DOT_NUM_THREADS gesteuert.</translation>
    </message>
<message>
        <source>OBFUSCATE_EMAILS</source>
            <translation>Wenn das \c OBFUSCATE_EMAILS-Tag auf \c YES gesetzt ist, werden E-Mail-Adressen in der generierten HTML-Ausgabe verschleiert, um Spam-Bots zu verhindern.</translation>
    </message>
<message>
        <source>OPTIMIZE_FOR_FORTRAN</source>
            <translation>Setzen Sie das \c OPTIMIZE_FOR_FORTRAN-Tag auf \c YES, wenn Ihr Projekt aus Fortran-Quellen besteht. Doxygen wird dann eine Ausgabe generieren, die auf Fortran zugeschnitten ist.</translation>
    </message>
<message>
        <source>OPTIMIZE_OUTPUT_FOR_C</source>
            <translation>Setzen Sie das \c OPTIMIZE_OUTPUT_FOR_C-Tag auf \c YES, wenn Ihr Projekt nur aus C-Quellen besteht. Doxygen wird dann eine Ausgabe generieren, die mehr auf C zugeschnitten ist. Zum Beispiel werden einige der verwendeten Namen anders sein. Die Liste aller Mitglieder wird weggelassen usw.</translation>
    </message>
<message>
        <source>OPTIMIZE_OUTPUT_JAVA</source>
            <translation>Setzen Sie das \c OPTIMIZE_OUTPUT_JAVA-Tag auf \c YES, wenn Ihr Projekt nur aus Java- oder Python-Quellen besteht. Doxygen wird dann eine Ausgabe generieren, die mehr auf diese Sprache zugeschnitten ist. Zum Beispiel werden Namensraume als Pakete prasentiert, qualifizierte Bereiche werden anders aussehen usw.</translation>
    </message>
<message>
        <source>OPTIMIZE_OUTPUT_SLICE</source>
            <translation>Setzen Sie das \c OPTIMIZE_OUTPUT_SLICE-Tag auf \c YES, wenn Ihr Projekt nur aus Slice-Quellen besteht. Doxygen wird dann eine Ausgabe generieren, die mehr auf diese Sprache zugeschnitten ist. Zum Beispiel werden Namensraume als Module prasentiert, Typen werden in mehr Gruppen unterteilt usw.</translation>
    </message>
<message>
        <source>OPTIMIZE_OUTPUT_VHDL</source>
            <translation>Setzen Sie das \c OPTIMIZE_OUTPUT_VHDL-Tag auf \c YES, wenn Ihr Projekt aus VHDL-Quellen besteht. Doxygen wird dann eine Ausgabe generieren, die auf VHDL zugeschnitten ist.</translation>
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
            <translation>Wenn \ref cfg_generate_treeview "GENERATE_TREEVIEW" auf \c YES gesetzt ist, bestimmt die \c PAGE_OUTLINE_PANEL-Option, ob ein zusatzliches Navigationspanel auf der rechten Seite des Bildschirms angezeigt wird, das eine Gliederung des Inhalts der Hauptseite anzeigt, ahnlich wie z.B. https://developer.android.com/reference. Wenn \ref cfg_generate_treeview "GENERATE_TREEVIEW" auf \c NO gesetzt ist, hat diese Option keine Auswirkung.</translation>
    </message>
<message>
        <source>PAPER_TYPE</source>
            <translation>Das \c PAPER_TYPE-Tag kann verwendet werden, um das Papierformat festzulegen, das vom Drucker verwendet wird. Mogliche Werte sind: \c a4, \c letter, \c legal und \c executive.</translation>
    </message>
<message>
        <source>PDF_HYPERLINKS</source>
            <translation>Wenn das \c PDF_HYPERLINKS-Tag auf \c YES gesetzt ist, ist das generierte \f$\mbox{\LaTeX}\f$ fur die Konvertierung zu PDF vorbereitet (mit \c ps2pdf oder \c pdflatex). Die PDF-Datei wird Links (genau wie die HTML-Ausgabe) anstelle von Seitenreferenzen enthalten. Dies macht die Ausgabe fur die Online-Anzeige mit einem PDF-Viewer geeignet.</translation>
    </message>
<message>
        <source>PERLMOD_LATEX</source>
            <translation>Wenn das \c PERLMOD_LATEX-Tag auf \c YES gesetzt ist, generiert Doxygen die notwendigen \c Makefile-Regeln, \c Perl-Skripte und \f$\mbox{\LaTeX}\f$-Code, um PDF- und DVI-Ausgabe aus der Perl-Modul-Ausgabe generieren zu konnen.</translation>
    </message>
<message>
        <source>PERLMOD_MAKEVAR_PREFIX</source>
            <translation>Die Namen der Make-Variablen in der generierten `doxyrules.make`-Datei werden mit der Zeichenfolge vorangestellt, die in \c PERLMOD_MAKEVAR_PREFIX enthalten ist. Dies ist nutzlich, damit verschiedene `doxyrules.make`-Dateien, die von derselben `Makefile` eingebunden werden, die Variablen der anderen nicht uberschreiben.</translation>
    </message>
<message>
        <source>PERLMOD_PRETTY</source>
            <translation>Wenn das \c PERLMOD_PRETTY-Tag auf \c YES gesetzt ist, wird die Perl-Modul-Ausgabe schon formatiert, sodass sie von einem menschlichen Leser geparst werden kann. Dies ist nutzlich, wenn Sie verstehen mochten, was passiert. Auf der anderen Seite, wenn dieses Tag auf \c NO gesetzt ist, wird die Grose der Perl-Modul-Ausgabe viel kleiner sein und Perl wird sie genauso parsen.</translation>
    </message>
<message>
        <source>PLANTUMLFILE_DIRS</source>
            <translation>Das \c PLANTUMLFILE_DIRS-Tag kann verwendet werden, um ein oder mehrere Verzeichnisse anzugeben, die PlantUml-Dateien enthalten, die in die Dokumentation eingebunden werden (siehe den Befehl \ref cmdplantumlfile "\\plantumlfile").</translation>
    </message>
<message>
        <source>PLANTUML_CFG_FILE</source>
            <translation>Bei Verwendung von PlantUML kann das \c PLANTUML_CFG_FILE-Tag verwendet werden, um eine Konfigurationsdatei fur PlantUML anzugeben.</translation>
    </message>
<message>
        <source>PLANTUML_INCLUDE_PATH</source>
            <translation>Bei Verwendung von PlantUML werden die angegebenen Pfade nach Dateien durchsucht, die durch die \c !include-Anweisung in einem PlantUML-Block angegeben werden.</translation>
    </message>
<message>
        <source>PLANTUML_JAR_PATH</source>
            <translation>Bei Verwendung von PlantUML sollte das \c PLANTUML_JAR_PATH-Tag verwendet werden, um den Pfad anzugeben, in dem Java die \c plantuml.jar-Datei finden kann, oder den Dateinamen der zu verwendenden \c jar-Datei. Wenn leer gelassen, wird angenommen, dass PlantUML nicht verwendet oder wahrend eines Vorverarbeitungsschritts aufgerufen wird. Doxygen generiert eine Warnung, wenn es in diesem Fall auf einen \ref cmdstartuml "\\startuml"-Befehl stost, und generiert keine Ausgabe fur das Diagramm.</translation>
    </message>
<message>
        <source>PREDEFINED</source>
            <translation>Das \c PREDEFINED-Tag kann verwendet werden, um einen oder mehrere Makronamen anzugeben, die definiert sind, bevor der Praprozessor gestartet wird (ahnlich der `-D`-Option von z.B. \c gcc). Das Argument des Tags ist eine Liste von Makros der Form: &lt;code&gt;name&lt;/code&gt; oder &lt;code&gt;name=definition&lt;/code&gt; (keine Leerzeichen). Wenn die Definition und das \c "=" weggelassen werden, wird \c "=1" angenommen. Um zu verhindern, dass eine Makrodefinition uber \c \#undef aufgehoben oder rekursiv expandiert wird, verwenden Sie den &lt;code&gt;:=&lt;/code&gt;-Operator anstelle des \c =-Operators.</translation>
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
            <translation>Das \c PROJECT_NAME-Tag ist ein einzelnes Wort (oder eine Folge von Wörtern, die von Anfuhrungszeichen umgeben sind, es sei denn, Sie verwenden Doxywizard), das das Projekt identifizieren soll, fur das die Dokumentation erstellt wird. Dieser Name wird im Titel der meisten generierten Seiten und an einigen anderen Stellen verwendet.</translation>
    </message>
<message>
        <source>PROJECT_NUMBER</source>
            <translation>Das \c PROJECT_NUMBER-Tag kann verwendet werden, um eine Projekt- oder Revisionsnummer einzugeben. Dies kann zum Archivieren der generierten Dokumentation oder bei Verwendung eines Versionskontrollsystems nutzlich sein.</translation>
    </message>
<message>
        <source>PYTHON_DOCSTRING</source>
            <translation>Standardmäßig werden Python-Docstrings als vorformatierter Text angezeigt und konnen keine Doxygen-Spezialbefehle verwenden. Durch Setzen von \c PYTHON_DOCSTRING auf \c NO konnen Doxygen-Spezialbefehle verwendet werden, und der Inhalt des Docstring-Dokumentationsblocks wird als Doxygen-Dokumentation angezeigt.</translation>
    </message>
<message>
        <source>QCH_FILE</source>
            <translation>Wenn das \ref cfg_qhg_location "QHG_LOCATION"-Tag angegeben ist, kann das \c QCH_FILE-Tag verwendet werden, um den Dateinamen der resultierenden `.qch`-Datei anzugeben. Der angegebene Pfad ist relativ zum HTML-Ausgabeordner.</translation>
    </message>
<message>
        <source>QHG_LOCATION</source>
            <translation>Das \c QHG_LOCATION-Tag kann verwendet werden, um den Speicherort (absoluter Pfad einschließlich Dateiname) von Qt's qhelpgenerator anzugeben. Wenn nicht leer, wird Doxygen versuchen, qhelpgenerator auf der generierten `.qhp`-Datei auszufuhren.</translation>
    </message>
<message>
        <source>QHP_CUST_FILTER_ATTRS</source>
            <translation>Das \c QHP_CUST_FILTER_ATTRS-Tag gibt die Liste der Attribute des benutzerdefinierten Filters an, der hinzugefugt werden soll. Fur weitere Informationen siehe &lt;a href="https://doc.qt.io/archives/qt-4.8/qthelpproject.html#custom-filters"&gt;Qt Help Project / Custom Filters&lt;/a&gt;.</translation>
    </message>
<message>
        <source>QHP_CUST_FILTER_NAME</source>
            <translation>Wenn das \c QHP_CUST_FILTER_NAME-Tag gesetzt ist, gibt es den Namen eines benutzerdefinierten Filters an, der hinzugefugt werden soll. Fur weitere Informationen siehe &lt;a href="https://doc.qt.io/archives/qt-4.8/qthelpproject.html#custom-filters"&gt;Qt Help Project / Custom Filters&lt;/a&gt;.</translation>
    </message>
<message>
        <source>QHP_NAMESPACE</source>
            <translation>Das \c QHP_NAMESPACE-Tag gibt den Namespace an, der beim Generieren der Qt Help Project-Ausgabe verwendet werden soll. Fur weitere Informationen siehe &lt;a href="https://doc.qt.io/archives/qt-4.8/qthelpproject.html#namespace"&gt;Qt Help Project / Namespace&lt;/a&gt;.</translation>
    </message>
<message>
        <source>QHP_SECT_FILTER_ATTRS</source>
            <translation>Das \c QHP_SECT_FILTER_ATTRS-Tag gibt die Liste der Attribute an, mit denen der Filterabschnitt dieses Projekts ubereinstimmt. &lt;a href="https://doc.qt.io/archives/qt-4.8/qthelpproject.html#filter-attributes"&gt;Qt Help Project / Filter Attributes&lt;/a&gt;.</translation>
    </message>
<message>
        <source>QHP_VIRTUAL_FOLDER</source>
            <translation>Das \c QHP_VIRTUAL_FOLDER-Tag gibt den virtuellen Ordner an, der beim Generieren der Qt Help Project-Ausgabe verwendet werden soll. Fur weitere Informationen siehe &lt;a href="https://doc.qt.io/archives/qt-4.8/qthelpproject.html#virtual-folders"&gt;Qt Help Project / Virtual Folders&lt;/a&gt;.</translation>
    </message>
<message>
        <source>QT_AUTOBRIEF</source>
            <translation>Wenn das \c QT_AUTOBRIEF-Tag auf \c YES gesetzt ist, interpretiert Doxygen die erste Zeile (bis zum ersten Punkt, Fragezeichen oder Ausrufezeichen) eines Qt-Stil-Kommentars als Kurzbeschreibung. Wenn auf \c NO gesetzt, verhalt sich der Qt-Stil wie normale Qt-Stil-Kommentare (wodurch ein expliziter \ref cmdbrief "\\brief"-Befehl fur eine Kurzbeschreibung erforderlich ist).</translation>
    </message>
<message>
        <source>QUIET</source>
            <translation>Das \c QUIET-Tag kann verwendet werden, um die Nachrichten ein- oder auszuschalten, die von Doxygen auf die Standardausgabe generiert werden. Wenn \c QUIET auf \c YES gesetzt ist, bedeutet dies, dass die Nachrichten ausgeschaltet sind.</translation>
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
            <translation>Wenn das \c REFERENCES_LINK_SOURCE-Tag auf \c YES gesetzt ist und das \ref cfg_source_browser "SOURCE_BROWSER"-Tag auf \c YES gesetzt ist, dann verweisen die Hyperlinks von Funktionen in den \ref cfg_references_relation "REFERENCES_RELATION"- und \ref cfg_referenced_by_relation "REFERENCED_BY_RELATION"-Listen auf den Quellcode. Andernfalls verweisen sie auf die Dokumentation.</translation>
    </message>
<message>
        <source>REFERENCES_RELATION</source>
            <translation>Wenn das \c REFERENCES_RELATION-Tag auf \c YES gesetzt ist, wird fur jede dokumentierte Funktion alle dokumentierten Entitaten aufgelistet, die von dieser Funktion aufgerufen/verwendet werden.</translation>
    </message>
<message>
        <source>REPEAT_BRIEF</source>
            <translation>Wenn das \c REPEAT_BRIEF-Tag auf \c YES gesetzt ist, wird Doxygen die Kurzbeschreibung eines Mitglieds oder einer Funktion vor der detaillierten Beschreibung voranstellen. &lt;br&gt;Hinweis: Wenn sowohl \ref cfg_hide_undoc_members "HIDE_UNDOC_MEMBERS" als auch \ref cfg_brief_member_desc "BRIEF_MEMBER_DESC" auf \c NO gesetzt sind, werden die Kurzbeschreibungen vollstandig unterdruckt.</translation>
    </message>
<message>
        <source>REQ_TRACEABILITY_INFO</source>
            <translation>Das \c REQ_TRACEABILITY_INFO-Tag steuert, ob Ruckverfolgbarkeitsinformationen auf der Anforderungen-Seite angezeigt werden (nur relevant bei Verwendung von \ref cmdrequirement "\\requirement"-Kommentarblöcken). Die Einstellung \c NO deaktiviert die Ruckverfolgbarkeitsinformationen vollstandig. Die Einstellung \c UNSATISFIED_ONLY zeigt eine Liste von Anforderungen an, denen eine satisfies-Beziehung fehlt (uber den Befehl: \ref cmdsatisfies "\\satisfies"). Ahnlich zeigt die Einstellung \c UNVERIFIED_ONLY eine Liste von Anforderungen an, denen eine verifies-Beziehung fehlt (uber den Befehl: \ref cmdverifies "\\verifies"). Das Setzen des Tags auf \c YES (der Standard) zeigt beide Listen an, falls zutreffend.</translation>
    </message>
<message>
        <source>RESOLVE_UNNAMED_PARAMS</source>
            <translation>Wenn dieses Flag auf \c YES gesetzt ist, wird der Name eines unbenannten Parameters in einer Deklaration durch die entsprechende Definition bestimmt. Standardmäßig bleiben unbenannte Parameter in der Ausgabe unbenannt.</translation>
    </message>
<message>
        <source>RTF_EXTENSIONS_FILE</source>
            <translation>Setzen Sie optionale Variablen, die bei der Generierung eines RTF-Dokuments verwendet werden. Die Syntax ist ahnlich wie die Konfigurationsdatei von Doxygen. Eine Vorlage fur Erweiterungsdateien kann mit &lt;code&gt;doxygen -e rtf extensionFile&lt;/code&gt; generiert werden.</translation>
    </message>
<message>
        <source>RTF_EXTRA_FILES</source>
            <translation>Das \c RTF_EXTRA_FILES-Tag kann verwendet werden, um ein oder mehrere zusatzliche Bilder oder andere Quelldateien anzugeben, die in das RTF-Ausgabeverzeichnis kopiert werden sollen. Beachten Sie, dass die Dateien wie sie sind kopiert werden; es sind keine Befehle oder Marker verfugbar.</translation>
    </message>
<message>
        <source>RTF_HYPERLINKS</source>
            <translation>Wenn das \c RTF_HYPERLINKS-Tag auf \c YES gesetzt ist, kann die generierte RTF-Ausgabe Links enthalten. Dies erfordert, dass die Ausgabedatei von Anwendungen wie Word oder WordPad gelesen wird, die RTF-Links unterstutzen. Fur andere Anwendungen erscheinen die Links als normaler Text.</translation>
    </message>
<message>
        <source>RTF_OUTPUT</source>
            <translation>Das \c RTF_OUTPUT-Tag wird verwendet, um anzugeben, wo die RTF-Dokumente platziert werden. Wenn ein relativer Pfad eingegeben wird, wird der Wert von \ref cfg_output_directory "OUTPUT_DIRECTORY" davor gesetzt.</translation>
    </message>
<message>
        <source>RTF_STYLESHEET_FILE</source>
            <translation>Laden Sie Stylesheet-Definitionen aus einer Datei. Die Syntax ist ahnlich wie die Konfigurationsdatei von Doxygen, d.h. eine Reihe von Zuweisungen. Sie mussen nur Ersetzungen angeben, fehlende Definitionen werden auf ihren Standardwert gesetzt. &lt;br&gt;Siehe auch Abschnitt \ref doxygen_usage fur Informationen zum Generieren des Standard-Stylesheets, das Doxygen normalerweise verwendet.</translation>
    </message>
<message>
        <source>SEARCHDATA_FILE</source>
            <translation>Wenn \ref cfg_server_based_search "SERVER_BASED_SEARCH" und \ref cfg_external_search "EXTERNAL_SEARCH" beide aktiviert sind, kann das \c SEARCHDATA_FILE-Tag verwendet werden, um eine Datei anzugeben, in der Suchdaten geschrieben werden. Wenn leer gelassen, wird `searchdata.xml` verwendet.</translation>
    </message>
<message>
        <source>SEARCHENGINE</source>
            <translation>Wenn das \c SEARCHENGINE-Tag aktiviert ist, generiert Doxygen ein Suchfeld fur die HTML-Ausgabe. Die zugrunde liegende Suchmaschine verwendet JavaScript und DHTML und sollte in jedem modernen Browser funktionieren. Beachten Sie, dass bei Verwendung von HTML-Hilfe (\ref cfg_generate_htmlhelp "GENERATE_HTMLHELP"), Qt-Hilfe (\ref cfg_generate_qhp "GENERATE_QHP") oder Docsets (\ref cfg_generate_docset "GENERATE_DOCSET") bereits eine Suchfunktion vorhanden ist, sodass diese normalerweise deaktiviert werden sollte. Fur große Projekte kann die JavaScript-basierte Suchmaschine langsam sein, dann kann das Aktivieren von \ref cfg_server_based_search "SERVER_BASED_SEARCH" eine bessere Losung bieten. Es ist moglich, mit der Tastatur zu suchen; um zum Suchfeld zu springen, verwenden Sie &lt;code&gt;\&lt;Zugriffsschlussel\&gt; + S&lt;/code&gt; (was der &lt;code&gt;\&lt;Zugriffsschlussel\&gt;&lt;/code&gt; ist, hangt vom System ab).</translation>
    </message>
<message>
        <source>SEARCHENGINE_URL</source>
            <translation>Wenn \ref cfg_server_based_search "SERVER_BASED_SEARCH" und \ref cfg_external_search "EXTERNAL_SEARCH" beide aktiviert sind, kann das \c SEARCHENGINE_URL-Tag verwendet werden, um eine URL anzugeben, an die eine Suchanfrage gesendet wird, um die Suchergebnisse zu erhalten. Die Suchanfrage wird an die URL angehangt. Fur die externe Suchmaschine wird die Suchanfrage als JSON-Objekt gesendet.</translation>
    </message>
<message>
        <source>SEARCH_INCLUDES</source>
            <translation>Wenn das \c SEARCH_INCLUDES-Tag auf \c YES gesetzt ist, sind die Include-Abhangigkeiten in der Liste der Dateien, die durchsucht werden, enthalten.</translation>
    </message>
<message>
        <source>SEPARATE_MEMBER_PAGES</source>
            <translation>Wenn das \c SEPARATE_MEMBER_PAGES-Tag auf \c YES gesetzt ist, generiert Doxygen eine separate Seite fur jedes Mitglied. Wenn auf \c NO gesetzt, wird die Dokumentation aller Mitglieder eines Verbunds (Klasse, Datei, Namensraum) in einem einzigen Dokument zusammengefasst.</translation>
    </message>
<message>
        <source>SERVER_BASED_SEARCH</source>
            <translation>Wenn das \c SERVER_BASED_SEARCH-Tag auf \c YES gesetzt ist, wird die Suchmaschine mit einem Webserver anstelle eines Webclients implementiert. Es gibt zwei Moglichkeiten, dies einzurichten: mit einem PHP-Skript oder mit einem externen Indexer. Siehe Abschnitt \ref extsearch fur Details.</translation>
    </message>
<message>
        <source>SHORT_NAMES</source>
            <translation>Wenn das \c SHORT_NAMES-Tag auf \c YES gesetzt ist, generiert Doxygen kurze (aber nicht eindeutige) Dateinamen. Dies kann nützlich sein, wenn Ihr Dateisystem keine langen Namen unterstutzt, wie z.B. DOS, Mac oder CD-ROM.</translation>
    </message>
<message>
        <source>SHOW_ENUM_VALUES</source>
            <translation>Wenn das \c SHOW_ENUM_VALUES-Tag gesetzt ist, zeigt Doxygen die angegebenen Aufzahlungswerte neben den Aufzahlungsmnemoniken an.</translation>
    </message>
<message>
        <source>SHOW_FILES</source>
            <translation>Wenn das \c SHOW_FILES-Tag auf \c NO gesetzt ist, wird die Dateiliste nicht generiert. Dies spart Zeit, wenn Sie eine sehr große Anzahl von Dateien haben und die Dateiliste nicht benotigen.</translation>
    </message>
<message>
        <source>SHOW_GROUPED_MEMB_INC</source>
            <translation>Wenn das \c SHOW_GROUPED_MEMB_INC-Tag auf \c YES gesetzt ist, werden Include-Abhangigkeiten fur Gruppenmitglieder in der Dokumentation angezeigt.</translation>
    </message>
<message>
        <source>SHOW_HEADERFILE</source>
            <translation>Wenn das \c SHOW_HEADERFILE-Tag auf \c YES gesetzt ist, zeigt die Dokumentation fur eine Klasse an, welche Datei eingebunden werden muss, um die Klasse zu verwenden.</translation>
    </message>
<message>
        <source>SHOW_INCLUDE_FILES</source>
            <translation>Wenn das \c SHOW_INCLUDE_FILES-Tag auf \c YES gesetzt ist, wird Doxygen eine Liste der Dateien anzeigen, die von einer Datei eingebunden werden, in der Dokumentation dieser Datei.</translation>
    </message>
<message>
        <source>SHOW_NAMESPACES</source>
            <translation>Wenn das \c SHOW_NAMESPACES-Tag auf \c NO gesetzt ist, wird die Namensraum-Liste nicht generiert. Dies spart Zeit, wenn Sie eine sehr große Anzahl von Namensraumen haben und die Namensraum-Liste nicht benotigen.</translation>
    </message>
<message>
        <source>SHOW_USED_FILES</source>
            <translation>Setzen Sie das \c SHOW_USED_FILES-Tag auf \c NO, um die Liste der Dateien, die am Ende der Dokumentation einer Klasse angezeigt werden, zu deaktivieren.</translation>
    </message>
<message>
        <source>SIP_SUPPORT</source>
            <translation>Wenn Sie SIP (siehe https://www.riverbankcomputing.com/software/sip/) verwenden, um Python-Bindungen zu erstellen, setzen Sie diese Option auf \c YES, damit Doxygen die speziellen Kommentare fur SIP-Unterstützung erkennen kann.</translation>
    </message>
<message>
        <source>SITEMAP_URL</source>
            <translation>Das \c SITEMAP_URL-Tag wird verwendet, um die vollstandige URL des Ortes anzugeben, an dem die generierte Dokumentation vom Benutzer wahrend der Bereitstellung der Dokumentation auf dem Server platziert wird. Die generierte Sitemap heißt `sitemap.xml` und wird im durch \ref cfg_html_output "HTML_OUTPUT" angegebenen Verzeichnis platziert. Falls keine \c SITEMAP_URL angegeben ist, wird keine Sitemap generiert. Fur Informationen zum Sitemap-Protokoll siehe https://www.sitemaps.org</translation>
    </message>
<message>
        <source>SKIP_FUNCTION_MACROS</source>
            <translation>Dieses Tag kann verwendet werden, um die Liste der Funktionsmakros zu spezifizieren, die in der Dokumentation ubersprungen werden sollen. Funktionsmakros sind Makros, die wie Funktionen aussehen und Parameter haben.</translation>
    </message>
<message>
        <source>SORT_BRIEF_DOCS</source>
            <translation>Wenn das \c SORT_BRIEF_DOCS-Tag auf \c YES gesetzt ist, wird Doxygen die Kurzbeschreibungen von Datei-, Namensraum- und Klassenmitgliedern alphabetisch nach Mitgliedsnamen sortieren. Wenn auf \c NO gesetzt, erscheinen die Mitglieder in Deklarationsreihenfolge. Beachten Sie, dass dies auch die Reihenfolge der Klassen in der Klassenliste beeinflusst.</translation>
    </message>
<message>
        <source>SORT_BY_SCOPE_NAME</source>
            <translation>Wenn das \c SORT_BY_SCOPE_NAME-Tag auf \c YES gesetzt ist, wird die Klassenliste nach vollqualifizierten Namen sortiert, einschließlich Namensraume. Wenn auf \c NO gesetzt, wird die Klassenliste nur nach Klassennamen sortiert, ohne den Namensraumteil. \note Diese Option ist nicht sehr nützlich, wenn \ref cfg_hide_scope_names "HIDE_SCOPE_NAMES" auf \c YES gesetzt ist. \note Diese Option gilt nur fur die Klassenliste, nicht fur die alphabetische Liste.</translation>
    </message>
<message>
        <source>SORT_GROUP_NAMES</source>
            <translation>Wenn das \c SORT_GROUP_NAMES-Tag auf \c YES gesetzt ist, wird Doxygen die Hierarchie der Gruppennamen in alphabetische Reihenfolge sortieren. Wenn auf \c NO gesetzt, erscheinen die Gruppennamen in ihrer definierten Reihenfolge.</translation>
    </message>
<message>
        <source>SORT_MEMBERS_CTORS_1ST</source>
            <translation>Wenn das \c SORT_MEMBERS_CTORS_1ST-Tag auf \c YES gesetzt ist, wird Doxygen die (Kurz- und detaillierte) Dokumentation von Klassenmitgliedern so sortieren, dass Konstruktoren und Destruktoren zuerst aufgefuhrt werden. Wenn auf \c NO gesetzt, erscheinen die Konstruktoren in den jeweiligen Reihenfolgen, die durch \ref cfg_sort_brief_docs "SORT_BRIEF_DOCS" und \ref cfg_sort_member_docs "SORT_MEMBER_DOCS" definiert sind. \note Wenn \ref cfg_sort_brief_docs "SORT_BRIEF_DOCS" auf \c NO gesetzt ist, wird diese Option beim Sortieren der kurzen Mitgliedsdokumentation ignoriert. \note Wenn \ref cfg_sort_member_docs "SORT_MEMBER_DOCS" auf \c NO gesetzt ist, wird diese Option beim Sortieren der detaillierten Mitgliedsdokumentation ignoriert.</translation>
    </message>
<message>
        <source>SORT_MEMBER_DOCS</source>
            <translation>Wenn das \c SORT_MEMBER_DOCS-Tag auf \c YES gesetzt ist, wird Doxygen die (detaillierte) Dokumentation von Datei- und Klassenmitgliedern alphabetisch nach Mitgliedsnamen sortieren. Wenn auf \c NO gesetzt, erscheinen die Mitglieder in Deklarationsreihenfolge.</translation>
    </message>
<message>
        <source>SOURCE_BROWSER</source>
            <translation>Wenn das \c SOURCE_BROWSER-Tag auf \c YES gesetzt ist, wird eine Liste der Quelldateien generiert. Dokumentierte Entitaten werden mit diesen Quellen querverwiesen. &lt;br&gt;Hinweis: Um den gesamten Quellcode in der generierten Ausgabe zu entfernen, stellen Sie sicher, dass auch \ref cfg_verbatim_headers "VERBATIM_HEADERS" auf \c NO gesetzt ist.</translation>
    </message>
<message>
        <source>SOURCE_TOOLTIPS</source>
            <translation>Wenn \c SOURCE_TOOLTIPS aktiviert ist (Standard), zeigt das Schweben uber einem Hyperlink im Quellcode einen Tooltip mit zusatzlichen Informationen wie Prototyp, Kurzbeschreibung und Links zur Definition und Dokumentation an. Da dies die HTML-Datei großer macht und das Laden großer Dateien etwas langsamer, konnen Sie diese Funktion deaktivieren.</translation>
    </message>
<message>
        <source>SQLITE3_OUTPUT</source>
            <translation>Das \c SQLITE3_OUTPUT-Tag wird verwendet, um anzugeben, wo die \c Sqlite3-Datenbank platziert wird. Wenn ein relativer Pfad eingegeben wird, wird der Wert von \ref cfg_output_directory "OUTPUT_DIRECTORY" davor gesetzt.</translation>
    </message>
<message>
        <source>SQLITE3_RECREATE_DB</source>
            <translation>Wenn das \c SQLITE3_RECREATE_DB-Tag auf \c YES gesetzt ist, wird die vorhandene doxygen_sqlite3.db-Datenbankdatei bei jedem Doxygen-Lauf neu erstellt. Wenn auf \c NO gesetzt, warnt Doxygen, wenn eine Datenbankdatei bereits gefunden wird, und andert sie nicht.</translation>
    </message>
<message>
        <source>STRICT_PROTO_MATCHING</source>
            <translation>Wenn die \c STRICT_PROTO_MATCHING-Option aktiviert ist und Doxygen keine korrekte Typauflosung aller Parameter einer Funktion durchfuhren kann, wird eine Ubereinstimmung zwischen dem Prototyp und der Implementierung einer Mitgliedsfunktion abgelehnt, selbst wenn es nur einen Kandidaten gibt oder es offensichtlich ist, welcher Kandidat durch einen einfachen Zeichenfolgenabgleich zu wahlen ist. Durch Deaktivieren von \c STRICT_PROTO_MATCHING akzeptiert Doxygen in solchen Fallen weiterhin eine Ubereinstimmung zwischen Prototyp und Implementierung.</translation>
    </message>
<message>
        <source>STRIP_CODE_COMMENTS</source>
            <translation>Das Setzen des \c STRIP_CODE_COMMENTS-Tags auf \c YES weist Doxygen an, alle speziellen Kommentarblocke aus generierten Quellcode-Fragmenten auszublenden. Normale C-, C++- und Fortran-Kommentare bleiben immer sichtbar.</translation>
    </message>
<message>
        <source>STRIP_FROM_INC_PATH</source>
            <translation>Das \c STRIP_FROM_INC_PATH-Tag kann verwendet werden, um einen benutzerdefinierten Teil des Pfads zu entfernen, der in der Dokumentation einer Klasse erwahnt wird, der dem Leser sagt, welche Header-Datei er einbinden muss, um die Klasse zu verwenden. Wenn leer gelassen, wird nur der Name der Header-Datei verwendet, die die Klassendefinition enthalt. Andernfalls sollten Sie eine Liste von Include-Pfaden angeben, die normalerweise mit dem `-I`-Flag an den Compiler ubergeben werden.</translation>
    </message>
<message>
        <source>STRIP_FROM_PATH</source>
            <translation>Das \c STRIP_FROM_PATH-Tag kann verwendet werden, um einen benutzerdefinierten Teil des Pfads zu entfernen. Das Entfernen erfolgt nur, wenn eine der angegebenen Zeichenfolgen mit dem linken Teil des Pfads ubereinstimmt. Dieses Tag kann verwendet werden, um relative Pfade in der Dateiliste anzuzeigen. Wenn leer gelassen, wird das Verzeichnis, in dem Doxygen ausgefuhrt wird, als zu entfernender Pfad verwendet. &lt;br&gt;Beachten Sie, dass Sie hier absolute Pfade angeben konnen, aber auch relative Pfade, die relativ zu dem Verzeichnis sind, in dem Doxygen gestartet wurde.</translation>
    </message>
<message>
        <source>SUBGROUPING</source>
            <translation>Setzen Sie das \c SUBGROUPING-Tag auf \c YES, um Gruppen von Klassenmitgliedern desselben Typs (z.B. eine Gruppe offentlicher Funktionen) als Untergruppe dieses Typs zu platzieren (z.B. unter dem Abschnitt Offentliche Funktionen). Setzen Sie es auf \c NO, um Untergruppierung zu verhindern. Alternativ kann dies pro Klasse mit dem Befehl \ref cmdnosubgrouping "\\nosubgrouping" erfolgen.</translation>
    </message>
<message>
        <source>TAB_SIZE</source>
            <translation>Das \c TAB_SIZE-Tag kann verwendet werden, um die Anzahl der Leerzeichen in einem Tabulator festzulegen. Doxygen verwendet diesen Wert, um Tabulatoren in Codefragmenten durch Leerzeichen zu ersetzen.</translation>
    </message>
<message>
        <source>TAGFILES</source>
            <translation>Das \c TAGFILES-Tag kann verwendet werden, um eine oder mehrere Tag-Dateien anzugeben. Fur jede Tag-Datei sollte der Speicherort der externen Dokumentation hinzugefugt werden. Das Format einer Tag-Datei ohne diesen Speicherort ist wie folgt: \verbatim TAGFILES = file1 file2 ... \endverbatim Das Hinzufugen des Speicherorts fur die Tag-Dateien erfolgt wie folgt: \verbatim TAGFILES = file1=loc1 "file2 = loc2" ... \endverbatim wobei `loc1` und `loc2` relative oder absolute Pfade oder URLs sein konnen. Siehe den Abschnitt \ref external fur weitere Informationen zur Verwendung von Tag-Dateien. \note Jede Tag-Datei muss einen eindeutigen Namen haben (wobei der Name den Pfad NICHT enthalt). Wenn eine Tag-Datei nicht in dem Verzeichnis liegt, in dem Doxygen ausgefuhrt wird, mussen Sie auch den Pfad zur Tag-Datei hier angeben.</translation>
    </message>
<message>
        <source>TEMPLATE_RELATIONS</source>
            <translation>Wenn das \c TEMPLATE_RELATIONS-Tag auf \c YES gesetzt ist, zeigen die Vererbungs- und Kollaborationsgraphen die Beziehungen zwischen Templates und ihren Instanzen.</translation>
    </message>
<message>
        <source>TIMESTAMP</source>
            <translation>Wenn das \c TIMESTAMP-Tag auf einen anderen Wert als \c NO gesetzt ist, enthalt jede generierte Seite das Datum oder Datum und Uhrzeit, wann die Seite generiert wurde. Das Setzen auf \c NO kann beim Vergleichen der Ausgabe mehrerer Laufe helfen.</translation>
    </message>
<message>
        <source>TOC_EXPAND</source>
            <translation>Das \c TOC_EXPAND-Flag kann auf \c YES gesetzt werden, um zusatzliche Elemente fur Gruppenmitglieder zum Inhaltsverzeichnis der HTML-Hilfe-Dokumentation und zur Baumansicht hinzuzufugen.</translation>
    </message>
<message>
        <source>TOC_INCLUDE_HEADINGS</source>
            <translation>Wenn das \c TOC_INCLUDE_HEADINGS-Tag auf einen Wert ungleich null gesetzt ist, werden alle Uberschriften bis zu dieser Ebene automatisch in das Inhaltsverzeichnis aufgenommen, auch wenn sie kein id-Attribut haben. \note Diese Funktion gilt derzeit nur fur Markdown-Uberschriften.</translation>
    </message>
<message>
        <source>TREEVIEW_WIDTH</source>
            <translation>Wenn der Treeview aktiviert ist (siehe \ref cfg_generate_treeview "GENERATE_TREEVIEW"), kann dieses Tag verwendet werden, um die anfangliche Breite (in Pixeln) des Frames festzulegen, in dem der Baum angezeigt wird.</translation>
    </message>
<message>
        <source>TYPEDEF_HIDES_STRUCT</source>
            <translation>Wenn das \c TYPEDEF_HIDES_STRUCT-Tag aktiviert ist, wird eine Typedef einer Struktur, Union oder Aufzahlung als Struktur, Union oder Aufzahlung mit dem Namen der Typedef dokumentiert. So wird &lt;code&gt;typedef struct TypeS {} TypeT&lt;/code&gt; in der Dokumentation als Struktur mit dem Namen \c TypeT erscheinen. Wenn deaktiviert, erscheint die Typedef als Mitglied einer Datei, eines Namensraums oder einer Klasse. Und die Struktur wird \c TypeS genannt. Dies kann typischerweise fur C-Code nützlich sein, falls die Codierungskonvention vorschreibt, dass alle Verbundtypen typedefiniert sind und nur auf die Typedef verwiesen wird, niemals auf den Tag-Namen.</translation>
    </message>
<message>
        <source>UML_LIMIT_NUM_FIELDS</source>
            <translation>Wenn das \ref cfg_uml_look "UML_LOOK"-Tag aktiviert ist, werden die Felder und Methoden innerhalb des Klassenknotens angezeigt. Wenn es viele Felder oder Methoden und viele Knoten gibt, kann der Graph zu groß werden, um nützlich zu sein. Der \c UML_LIMIT_NUM_FIELDS-Schwellenwert begrenzt die Anzahl der Elemente fur jeden Typ, um die Größe handhabbarer zu machen. Setzen Sie dies auf 0 fur kein Limit. Beachten Sie, dass der Schwellenwert um 50% uberschritten werden kann, bevor das Limit durchgesetzt wird. Wenn Sie also den Schwellenwert auf 10 setzen, konnen bis zu 15 Felder erscheinen, aber wenn die Anzahl 15 uberschreitet, wird die Gesamtanzahl der angezeigten Felder auf 10 begrenzt.</translation>
    </message>
<message>
        <source>UML_LOOK</source>
            <translation>Wenn das \c UML_LOOK-Tag auf \c YES gesetzt ist, generiert Doxygen Vererbungs- und Kollaborationsdiagramme in einem Stil, der der Unified Modeling Language der OMG ahnelt.</translation>
    </message>
<message>
        <source>UML_MAX_EDGE_LABELS</source>
            <translation>Wenn das \ref cfg_uml_look "UML_LOOK"-Tag aktiviert ist, werden Feldbeschriftungen entlang der Kante zwischen zwei Klassenknoten angezeigt. Wenn es viele Felder und viele Knoten gibt, kann der Graph zu uberfullt werden. Der \c UML_MAX_EDGE_LABELS-Schwellenwert begrenzt die Anzahl der Elemente, um die Größe handhabbarer zu machen. Setzen Sie dies auf 0 fur kein Limit.</translation>
    </message>
<message>
        <source>USE_HTAGS</source>
            <translation>Wenn das \c USE_HTAGS-Tag auf \c YES gesetzt ist, verweisen die Referenzen auf den Quellcode auf das vom \c htags(1)-Tool generierte HTML anstelle des integrierten Quellcode-Browsers von Doxygen. Das \c htags-Tool ist Teil des globalen Quellcode-Tagging-Systems von GNU (siehe https://www.gnu.org/software/global/global.html). Sie benotigen Version 4.8.6 oder hoher. &lt;br&gt;Um es zu verwenden, tun Sie Folgendes: -# Installieren Sie die neueste Version von \c global -# Aktivieren Sie \ref cfg_source_browser "SOURCE_BROWSER" und \c USE_HTAGS in der Konfigurationsdatei -# Stellen Sie sicher, dass \ref cfg_input "INPUT" auf die Wurzel des Quellbaums zeigt -# Fuhren Sie \c doxygen wie gewohnt aus &lt;br&gt;Doxygen ruft \c htags auf (und das wiederum ruft \c gtags auf), daher mussen diese Tools uber die Befehlszeile verfugbar sein (d.h. im Suchpfad). &lt;br&gt;Das Ergebnis: Anstelle des von Doxygen generierten Quellcode-Browsers zeigen die Links zum Quellcode jetzt auf die Ausgabe von \c htags.</translation>
    </message>
<message>
        <source>USE_MATHJAX</source>
            <translation>Aktivieren Sie die \c USE_MATHJAX-Option, um \f$\mbox{\LaTeX}\f$-Formeln mit MathJax zu rendern (siehe https://www.mathjax.org), das clientseitiges JavaScript fur das Rendering verwendet, anstatt vorgerenderte Bitmaps zu verwenden. Verwenden Sie dies, wenn Sie \f$\mbox{\LaTeX}\f$ nicht installiert haben oder wenn Sie mochten, dass die Formeln in der HTML-Ausgabe schoner aussehen. Wenn aktiviert, mussen Sie moglicherweise auch MathJax separat installieren und den Pfad dazu mit der Option \ref cfg_mathjax_relpath "MATHJAX_RELPATH" konfigurieren.</translation>
    </message>
<message>
        <source>USE_MDFILE_AS_MAINPAGE</source>
            <translation>Wenn das \c USE_MDFILE_AS_MAINPAGE-Tag auf den Namen einer Markdown-Datei verweist, die Teil der Eingabe ist, wird ihr Inhalt auf der Hauptseite (`index.html`) platziert. Dies kann nützlich sein, wenn Sie ein Projekt beispielsweise auf GitHub haben und die README.md-Datei als Hauptseite wiederverwenden mochten.</translation>
    </message>
<message>
        <source>USE_PDFLATEX</source>
            <translation>Wenn das \c USE_PDFLATEX-Tag auf \c YES gesetzt ist, verwendet Doxygen die mit \ref cfg_latex_cmd_name "LATEX_CMD_NAME" angegebene Engine, um die PDF-Datei direkt aus den \f$\mbox{\LaTeX}\f$-Dateien zu generieren. Setzen Sie diese Option auf \c YES, um eine PDF-Dokumentation hoherer Qualität zu erhalten. &lt;br&gt;Siehe auch Abschnitt \ref cfg_latex_cmd_name "LATEX_CMD_NAME" fur die Auswahl der Engine.</translation>
    </message>
<message>
        <source>VERBATIM_HEADERS</source>
            <translation>Wenn das \c VERBATIM_HEADERS-Tag auf \c YES gesetzt ist, generiert Doxygen eine wortliche Kopie der Header-Datei fur jede Klasse, fur die ein Include angegeben ist. Setzen Sie auf \c NO, um dies zu deaktivieren. \sa Abschnitt \ref cmdclass "\\class".</translation>
    </message>
<message>
        <source>WARNINGS</source>
            <translation>Das \c WARNINGS-Tag kann verwendet werden, um die Warnmeldungen ein- oder auszuschalten, die von Doxygen auf die Standardfehlerausgabe (\c stderr) generiert werden. Wenn \c WARNINGS auf \c YES gesetzt ist, bedeutet dies, dass die Warnungen eingeschaltet sind. &lt;br&gt; \b Tipp: Schalten Sie Warnungen ein, wahrend Sie die Dokumentation schreiben.</translation>
    </message>
<message>
        <source>WARN_AS_ERROR</source>
            <translation>Wenn das \c WARN_AS_ERROR-Tag auf \c YES gesetzt ist, stoppt Doxygen sofort, wenn eine Warnung auftritt. Wenn das \c WARN_AS_ERROR-Tag auf \c FAIL_ON_WARNINGS gesetzt ist, fahrt Doxygen fort, als ob das \c WARN_AS_ERROR-Tag auf \c NO gesetzt ware, aber am Ende des Doxygen-Prozesses kehrt Doxygen mit einem Status ungleich Null zuruck. Wenn das \c WARN_AS_ERROR-Tag auf \c FAIL_ON_WARNINGS_PRINT gesetzt ist, verhalt sich Doxygen wie \c FAIL_ON_WARNINGS, aber falls keine \ref cfg_warn_logfile "WARN_LOGFILE" definiert ist, schreibt Doxygen die Warnmeldungen nicht zwischen andere Nachrichten, sondern schreibt sie am Ende eines Laufs. Falls eine \ref cfg_warn_logfile "WARN_LOGFILE" definiert ist, werden die Warnmeldungen zusatzlich zur definierten Datei auch am Ende eines Laufs angezeigt, es sei denn, die \ref cfg_warn_logfile "WARN_LOGFILE" ist als `-` definiert, d.h. Standardausgabe (`stdout`), in diesem Fall bleibt das Verhalten wie bei der Einstellung \c FAIL_ON_WARNINGS.</translation>
    </message>
<message>
        <source>WARN_FORMAT</source>
            <translation>Das \c WARN_FORMAT-Tag bestimmt das Format der Warnmeldungen, die Doxygen erzeugen kann. Die Zeichenfolge sollte die Tags &lt;code&gt;\$file&lt;/code&gt;, &lt;code&gt;\$line&lt;/code&gt; und &lt;code&gt;\$text&lt;/code&gt; enthalten, die durch die Datei und Zeilennummer, von der die Warnung stammt, und den Warnungstext ersetzt werden. Optional kann das Format &lt;code&gt;$version&lt;/code&gt; enthalten, das durch die Version der Datei ersetzt wird (falls sie uber \ref cfg_file_version_filter "FILE_VERSION_FILTER" ermittelt werden konnte). \sa \ref cfg_warn_line_format "WARN_LINE_FORMAT"</translation>
    </message>
<message>
        <source>WARN_IF_DOC_ERROR</source>
            <translation>Wenn das \c WARN_IF_DOC_ERROR-Tag auf \c YES gesetzt ist, generiert Doxygen Warnungen fur potenzielle Fehler in der Dokumentation, wie z.B. das zweimalige Dokumentieren einiger Parameter in einer dokumentierten Funktion, das Dokumentieren nicht vorhandener Parameter oder das falsche Verwenden von Markup-Befehlen.</translation>
    </message>
<message>
        <source>WARN_IF_INCOMPLETE_DOC</source>
            <translation>Wenn \c WARN_IF_INCOMPLETE_DOC auf \c YES gesetzt ist, warnt Doxygen vor unvollstandiger Funktionsparameter-Dokumentation. Wenn auf \c NO gesetzt, akzeptiert Doxygen, dass einige Parameter keine Dokumentation haben, ohne zu warnen.</translation>
    </message>
<message>
        <source>WARN_IF_UNDOCUMENTED</source>
            <translation>Wenn das \c WARN_IF_UNDOCUMENTED-Tag auf \c YES gesetzt ist, generiert Doxygen Warnungen fur undokumentierte Mitglieder. Wenn \ref cfg_extract_all "EXTRACT_ALL" auf \c YES gesetzt ist, wird dieses Flag automatisch deaktiviert.</translation>
    </message>
<message>
        <source>WARN_IF_UNDOC_ENUM_VAL</source>
            <translation>Wenn die \c WARN_IF_UNDOC_ENUM_VAL-Option auf \c YES gesetzt ist, warnt Doxygen vor undokumentierten Aufzahlungswerten. Wenn auf \c NO gesetzt, akzeptiert Doxygen undokumentierte Aufzahlungswerte. Wenn \ref cfg_extract_all "EXTRACT_ALL" auf \c YES gesetzt ist, wird dieses Flag automatisch deaktiviert.</translation>
    </message>
<message>
        <source>WARN_LAYOUT_FILE</source>
            <translation>Wenn die \c WARN_LAYOUT_FILE-Option auf \c YES gesetzt ist, warnt Doxygen vor Problemen, die beim Parsen der benutzerdefinierten Layout-Datei gefunden wurden, wie fehlende oder falsche Elemente. Siehe auch \ref cfg_layout_file "LAYOUT_FILE" fur Details. Wenn auf \c NO gesetzt, werden Probleme mit der Layout-Datei unterdruckt.</translation>
    </message>
<message>
        <source>WARN_LINE_FORMAT</source>
            <translation>Im `$text`-Teil des \ref cfg_warn_format "WARN_FORMAT"-Befehls ist es moglich, dass ein Verweis auf einen spezifischeren Ort angegeben wird. Um das Springen zu diesem Ort (außerhalb von Doxygen) zu erleichtern, kann der Benutzer eine benutzerdefinierte "Ausschneiden"/"Einfugen"-Zeichenfolge definieren. Beispiel: \verbatim WARN_LINE_FORMAT = "'vi $file +$line'" \endverbatim \sa \ref cfg_warn_format "WARN_FORMAT"</translation>
    </message>
<message>
        <source>WARN_LOGFILE</source>
            <translation>Das \c WARN_LOGFILE-Tag kann verwendet werden, um eine Datei anzugeben, in die Warn- und Fehlermeldungen geschrieben werden sollen. Wenn leer gelassen, wird die Ausgabe auf die Standardfehlerausgabe (`stderr`) geschrieben. Falls die angegebene Datei nicht zum Schreiben geoffnet werden kann, werden die Warn- und Fehlermeldungen auf die Standardfehlerausgabe geschrieben. Wenn die Datei `-` angegeben wird, werden die Warn- und Fehlermeldungen auf die Standardausgabe (`stdout`) geschrieben.</translation>
    </message>
<message>
        <source>WARN_NO_PARAMDOC</source>
            <translation>Diese \c WARN_NO_PARAMDOC-Option kann aktiviert werden, um Warnungen fur Funktionen zu erhalten, die dokumentiert sind, aber keine Dokumentation fur ihre Parameter oder Ruckgabewerte haben. Wenn auf \c NO gesetzt, warnt Doxygen nur uber falsche Parameterdokumentation, aber nicht uber das Fehlen von Dokumentation. Wenn \ref cfg_extract_all "EXTRACT_ALL" auf \c YES gesetzt ist, wird dieses Flag automatisch deaktiviert. Siehe auch \ref cfg_warn_if_incomplete_doc "WARN_IF_INCOMPLETE_DOC"</translation>
    </message>
<message>
        <source>XML_NS_MEMB_FILE_SCOPE</source>
            <translation>Wenn das \c XML_NS_MEMB_FILE_SCOPE-Tag auf \c YES gesetzt ist, wird Doxygen Namensraum-Mitglieder auch im Dateibereich aufnehmen, passend zur HTML-Ausgabe.</translation>
    </message>
<message>
        <source>XML_OUTPUT</source>
            <translation>Das \c XML_OUTPUT-Tag wird verwendet, um anzugeben, wo die XML-Seiten platziert werden. Wenn ein relativer Pfad eingegeben wird, wird der Wert von \ref cfg_output_directory "OUTPUT_DIRECTORY" davor gesetzt.</translation>
    </message>
<message>
        <source>XML_PROGRAMLISTING</source>
            <translation>Wenn das \c XML_PROGRAMLISTING-Tag auf \c YES gesetzt ist, gibt Doxygen die Programmauflistungen (einschließlich Syntaxhervorhebung und Querverweisinformationen) in die XML-Ausgabe. Beachten Sie, dass die Aktivierung dieser Option die Größe der XML-Ausgabe erheblich vergroßert.</translation>
    </message>
</context>
<context>
    <name>OptionValue</name>
<message>
        <source>(United States)</source>
        <translation>(Vereinigte Staaten)</translation>
    </message>
<message>
        <source>(Persian)</source>
        <translation>(Persisch)</translation>
    </message>
<message>
        <source>(Japanese with English messages)</source>
        <translation>(Japanisch mit englischen Meldungen)</translation>
    </message>
<message>
        <source>(Korean with English messages)</source>
        <translation>(Koreanisch mit englischen Meldungen)</translation>
    </message>
<message>
        <source>(Farsi)</source>
        <translation>(Farsi)</translation>
    </message>
<message>
        <source>use a fixed 'autotoc_md' string followed by a sequence number starting at 0</source>
        <translation>verwendet eine feste 'autotoc_md'-Zeichenkette gefolgt von einer Sequenznummer beginnend bei 0</translation>
    </message>
<message>
        <source>use the lower case version of title with any whitespace replaced by '-' and punctuation characters removed</source>
        <translation>verwendet die Kleinbuchstabenversion des Titels, wobei alle Leerzeichen durch '-' ersetzt und Satzzeichen entfernt werden</translation>
    </message>
<message>
        <source>(to be provided as Doxygen C comment)</source>
        <translation>(als Doxygen-C-Kommentar bereitzustellen)</translation>
    </message>
<message>
        <source>always generates light mode output</source>
        <translation>generiert immer Light-Mode-Ausgabe</translation>
    </message>
<message>
        <source>always generates dark mode output</source>
        <translation>generiert immer Dark-Mode-Ausgabe</translation>
    </message>
<message>
        <source>automatically sets the mode according to the user preference, uses light mode if no preference is set (the default)</source>
        <translation>setzt den Modus automatisch entsprechend der Benutzereinstellung, verwendet Light-Mode, wenn keine Einstellung festgelegt ist (Standard)</translation>
    </message>
<message>
        <source>automatically sets the mode according to the user preference, uses dark mode if no preference is set</source>
        <translation>setzt den Modus automatisch entsprechend der Benutzereinstellung, verwendet Dark-Mode, wenn keine Einstellung festgelegt ist</translation>
    </message>
<message>
        <source>allows a user to switch between light and dark mode via a button</source>
        <translation>ermöglicht dem Benutzer, über eine Schaltfläche zwischen Light- und Dark-Mode zu wechseln</translation>
    </message>
<message>
        <source>(the default)</source>
        <translation>(Standard)</translation>
    </message>
<message>
        <source>(looks nicer but requires the pdf2svg or inkscape tool)</source>
        <translation>(sieht schöner aus, erfordert aber das pdf2svg- oder inkscape-Tool)</translation>
    </message>
<message>
        <source>(which is slower, but has the best compatibility. This is the name for Mathjax version 2, for MathJax version 3 this will be translated into \c chtml)</source>
        <translation>(langsamer, aber mit bester Kompatibilität. Dies ist der Name für MathJax Version 2, für MathJax Version 3 wird dies in \c chtml übersetzt)</translation>
    </message>
<message>
        <source>(i.e. MathML. Only supported for MathJax 2. For MathJax version 3 \c chtml will be used instead.)</source>
        <translation>(d.h. MathML. Nur für MathJax 2 unterstützt. Für MathJax Version 3 wird stattdessen \c chtml verwendet.)</translation>
    </message>
<message>
        <source>(This is the name for Mathjax version 3, for MathJax version 2 this will be translated into \c HTML-CSS)</source>
        <translation>(Dies ist der Name für MathJax Version 3, für MathJax Version 2 wird dies in \c HTML-CSS übersetzt)</translation>
    </message>
<message>
        <source>(210 x 297 mm)</source>
        <translation>(210 x 297 mm)</translation>
    </message>
<message>
        <source>(8.5 x 11 inches)</source>
        <translation>(8,5 x 11 Zoll)</translation>
    </message>
<message>
        <source>(8.5 x 14 inches)</source>
        <translation>(8,5 x 14 Zoll)</translation>
    </message>
<message>
        <source>(7.25 x 10.5 inches)</source>
        <translation>(7,25 x 10,5 Zoll)</translation>
    </message>
<message>
        <source>same as ERROR_STOP</source>
        <translation>gleich wie ERROR_STOP</translation>
    </message>
<message>
        <source>same as BATCH</source>
        <translation>gleich wie BATCH</translation>
    </message>
<message>
        <source>In batch mode nothing is printed on the terminal, errors are scrolled as if \&lt;return\&gt; is hit at every error; missing files that TeX tries to input or request from keyboard input (\\read on a not open input stream) cause the job to abort</source>
        <translation>Im Batch-Modus wird nichts auf dem Terminal ausgegeben, Fehler werden so gescrollt, als ob bei jedem Fehler \&lt;Return\&gt; gedrückt würde; fehlende Dateien, die TeX versucht einzulesen oder über Tastatureingabe anzufordern (\\read auf einem nicht geöffneten Eingabestream), führen zum Abbruch des Auftrags</translation>
    </message>
<message>
        <source>In nonstop mode the diagnostic message will appear on the terminal, but there is no possibility of user interaction just like in batch mode</source>
        <translation>Im Nonstop-Modus erscheint die Diagnosemeldung auf dem Terminal, aber es gibt keine Möglichkeit der Benutzerinteraktion, genau wie im Batch-Modus</translation>
    </message>
<message>
        <source>In scroll mode, TeX will stop only for missing files to input or if keyboard input is necessary</source>
        <translation>Im Scroll-Modus hält TeX nur an, um fehlende Dateien einzulesen oder wenn Tastatureingabe erforderlich ist</translation>
    </message>
<message>
        <source>In errorstop mode, TeX will stop at each error, asking for user intervention</source>
        <translation>Im Errorstop-Modus hält TeX bei jedem Fehler an und fordert Benutzerintervention</translation>
    </message>
<message>
        <source>(use client-side rendering for HTML and \c mmdc for LaTeX/PDF and other formats. If \c MERMAID_PATH is not set, non-HTML diagrams will produce a warning)</source>
        <translation>(verwendet Client-Side-Rendering für HTML und \c mmdc für LaTeX/PDF und andere Formate. Wenn \c MERMAID_PATH nicht gesetzt ist, erzeugen Nicht-HTML-Diagramme eine Warnung)</translation>
    </message>
<message>
        <source>(use the \c mmdc tool to pre-generate images (requires \c Node.js and \c mermaid-js/mermaid-cli). Works for all output formats)</source>
        <translation>(verwendet das \c mmdc-Tool zum Vorgenerieren von Bildern (erfordert \c Node.js und \c mermaid-js/mermaid-cli). Funktioniert für alle Ausgabeformate)</translation>
    </message>
<message>
        <source>(embed \c mermaid.js in HTML output for client-side rendering. Does not require \c mmdc but only works for HTML output)</source>
        <translation>(bettet \c mermaid.js in die HTML-Ausgabe für Client-Side-Rendering ein. Erfordert kein \c mmdc, funktioniert aber nur für HTML-Ausgabe)</translation>
    </message>
</context>
</TS>