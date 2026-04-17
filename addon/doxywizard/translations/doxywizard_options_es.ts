<?xml version='1.0' encoding='utf-8'?>
<TS version="2.1" language="es">
<context>
    <name>Expert</name>
<message>
        <source>Possible values are:</source>
        <translation>Los valores posibles son:</translation>
    </message>
<message>
        <source>and</source>
        <translation>y</translation>
    </message>
<message>
        <source>The default value is: &lt;code&gt;%1&lt;/code&gt;.</source>
        <translation>El valor predeterminado es: &lt;code&gt;%1&lt;/code&gt;.</translation>
    </message>
<message>
        <source>Minimum value: %1, maximum value: %2, default value: %3.</source>
        <translation>Valor mínimo: %1, valor máximo: %2, valor predeterminado: %3.</translation>
    </message>
<message>
        <source>The default value is: system dependent.</source>
        <translation>El valor predeterminado es: dependiente del sistema.</translation>
    </message>
<message>
        <source>The default directory is: &lt;code&gt;%1&lt;/code&gt;.</source>
        <translation>El directorio predeterminado es: &lt;code&gt;%1&lt;/code&gt;.</translation>
    </message>
<message>
        <source>The default file is: &lt;code&gt;%1&lt;/code&gt;.</source>
        <translation>El archivo predeterminado es: &lt;code&gt;%1&lt;/code&gt;.</translation>
    </message>
<message>
        <source>The default file (with absolute path) is: &lt;code&gt;%1&lt;/code&gt;.</source>
        <translation>El archivo predeterminado (con ruta absoluta) es: &lt;code&gt;%1&lt;/code&gt;.</translation>
    </message>
<message>
        <source>The file has to be specified with full path.</source>
        <translation>El archivo debe especificarse con la ruta completa.</translation>
    </message>
<message>
        <source>The default image is: &lt;code&gt;%1&lt;/code&gt;.</source>
        <translation>La imagen predeterminada es: &lt;code&gt;%1&lt;/code&gt;.</translation>
    </message>
<message>
        <source>The default image (with absolute path) is: &lt;code&gt;%1&lt;/code&gt;.</source>
        <translation>La imagen predeterminada (con ruta absoluta) es: &lt;code&gt;%1&lt;/code&gt;.</translation>
    </message>
<message>
        <source>The image has to be specified with full path.</source>
        <translation>La imagen debe especificarse con la ruta completa.</translation>
    </message>
<message>
        <source>This tag requires that the tag %1 is set to &lt;code&gt;YES&lt;/code&gt;.</source>
        <translation>Esta etiqueta requiere que la etiqueta %1 esté establecida en &lt;code&gt;YES&lt;/code&gt;.</translation>
    </message>
</context>
<context>
    <name>OptionDocs</name>
<message>
        <source>DOXYFILE_ENCODING</source>
            <translation>Esta etiqueta especifica la codificacion de todos los caracteres en el archivo de configuracion que sigue. El valor predeterminado es UTF-8, que tambien es la codificacion para todo el texto antes de la primera aparicion de esta etiqueta. Doxygen usa \c libiconv (o iconv incorporado en \c libc) para la transcodificacion. Ver https://www.gnu.org/software/libiconv/ para la lista de posibles codificaciones.</translation>
    </message>
<message>
        <source>ABBREVIATE_BRIEF</source>
            <translation>Esta etiqueta implementa un abreviador de descripcion breve casi inteligente que se usa para formar el texto en varias listas. Cada cadena en esta lista, si se encuentra como el texto inicial de una descripcion breve, se eliminara del texto, y el resultado despues de procesar toda la lista se usara como el texto anotado. De lo contrario, se usara la descripcion breve como se especifico. Si se deja vacio, se usaran los siguientes valores (`$name` se reemplaza automaticamente por el nombre de la entidad):</translation>
    </message>
<message>
        <source>ALIASES</source>
            <translation>Esta etiqueta se puede usar para especificar varios alias que actuen como comandos en la documentacion. Un alias tiene la forma: \verbatim nombre=valor\endverbatim Por ejemplo, agregar \verbatim "sideeffect=@par Efectos secundarios:^^"\endverbatim le permitira colocar el comando \c \sideeffect (o \c \@sideeffect) en la documentacion, lo que resultara en un parrafo definido por el usuario con el encabezado "Efectos secundarios:". Tenga en cuenta que no puede poner \ref cmdn "\n" en el valor de un alias para insertar un salto de linea (en la salida resultante). Puede poner `^^` en el valor de un alias para insertar un salto de linea como si hubiera un salto de linea fisico en el archivo original. Si necesita un `{` o `}` o `,` literal en el valor de un alias, debe escaparlos con una barra invertida (\c \\).</translation>
    </message>
<message>
        <source>ALLEXTERNALS</source>
            <translation>Si la etiqueta \c ALLEXTERNALS esta configurada en \c YES, todas las clases externas y espacios de nombres se listaran en los indices de clases y espacios de nombres. Si se configura en \c NO, solo se listaran las clases externas heredadas.</translation>
    </message>
<message>
        <source>ALLOW_UNICODE_NAMES</source>
            <translation>Si la etiqueta \c ALLOW_UNICODE_NAMES esta configurada en \c YES, Doxygen permitira caracteres no ASCII en los nombres de los archivos generados. Si se configura en \c NO, los caracteres no ASCII seran escapados, por ejemplo _xE3_x81_x84 para Unicode U+3044.</translation>
    </message>
<message>
        <source>ALPHABETICAL_INDEX</source>
            <translation>Si la etiqueta \c ALPHABETICAL_INDEX esta configurada en \c YES, se generara un indice alfabetico de todas las entidades. Si configura las etiquetas \ref cfg_extract_all "EXTRACT_ALL" y \ref cfg_extract_private "EXTRACT_PRIVATE" en \c YES, tambien se generara el indice de todos los elementos internos. Si no desea esto, puede configurar \ref cfg_ignore_prefix "IGNORE_PREFIX" en un valor no vacio.</translation>
    </message>
<message>
        <source>ALWAYS_DETAILED_SEC</source>
            <translation>Si las etiquetas \c ALWAYS_DETAILED_SEC y \ref cfg_repeat_brief "REPEAT_BRIEF" estan configuradas en \c YES, Doxygen generara una seccion detallada incluso si solo hay una descripcion breve.</translation>
    </message>
<message>
        <source>AUTOLINK_IGNORE_WORDS</source>
            <translation>Esta etiqueta especifica una lista de palabras que, cuando coinciden con el comienzo de una palabra en la documentacion, suprimiran la generacion automatica de enlaces cuando esta esta habilitada por AUTOLINK_SUPPORT. Esta lista no afecta los enlaces creados explícitamente con \# o los comandos \ref cmdlink "\\link" o \ref cmdref "\\ref".</translation>
    </message>
<message>
        <source>AUTOLINK_SUPPORT</source>
            <translation>Si esta habilitado, Doxygen intentara vincular palabras que correspondan a una clase o espacio de nombres documentado con la documentacion correspondiente. Esto se puede evitar en casos individuales colocando un simbolo \c % antes de la palabra, o globalmente configurando \c AUTOLINK_SUPPORT en \c NO. Las palabras listadas en la etiqueta \c AUTOLINK_IGNORE_WORDS seran excluidas de la vinculacion automatica.</translation>
    </message>
<message>
        <source>BINARY_TOC</source>
            <translation>La bandera \c BINARY_TOC controla si se genera un tabla de contenidos binaria. Si se configura en \c YES, se creara una tabla de contenidos binaria, lo que resultara en una busqueda mas rapida y archivos mas pequenos. La tabla de contenidos predeterminada se almacena en un archivo de texto.</translation>
    </message>
<message>
        <source>BRIEF_MEMBER_DESC</source>
            <translation>Si la etiqueta \c BRIEF_MEMBER_DESC esta configurada en \c YES, Doxygen incluira descripciones breves de los miembros despues de los miembros que se enumeran en la documentacion de archivos y clases (similar a \c Javadoc). Configure en \c NO para desactivar esto.</translation>
    </message>
<message>
        <source>BUILTIN_STL_SUPPORT</source>
            <translation>Si usa clases STL (es decir, `std::string`, `std::vector`, etc.), pero no incluye el codigo fuente (archivo de etiquetas) de la STL como entrada, debe configurar esta etiqueta en \c YES para que Doxygen pueda hacer coincidir declaraciones y definiciones de funciones con argumentos que contienen clases STL (por ejemplo, `func(std::string`); versus `func(std::string) {}`). Esto tambien hace que los diagramas de herencia y colaboracion que involucran clases STL sean mas completos y precisos.</translation>
    </message>
<message>
        <source>CALLER_GRAPH</source>
            <translation>Si la etiqueta \c CALLER_GRAPH esta configurada en \c YES, Doxygen generara un grafo de dependencias de llamadores para cada funcion global o metodo de clase. Tenga en cuenta que habilitar esta opcion requiere que la configuracion \ref cfg_have_dot "HAVE_DOT" este en \c YES. La activacion explicita de un grafo de llamadores cuando \c CALLER_GRAPH esta configurado en \c NO se puede lograr con el comando \ref cmdcallergraph "\\callergraph". La desactivacion de un grafo de llamadores se puede lograr con el comando \ref cmdhidecallergraph "\\hidecallergraph".</translation>
    </message>
<message>
        <source>CALL_GRAPH</source>
            <translation>Si la etiqueta \c CALL_GRAPH esta configurada en \c YES, Doxygen generara un grafo de dependencias de llamadas para cada funcion global o metodo de clase. Tenga en cuenta que habilitar esta opcion requiere que la configuracion \ref cfg_have_dot "HAVE_DOT" este en \c YES. La activacion explicita de un grafo de llamadas cuando \c CALL_GRAPH esta configurado en \c NO se puede lograr con el comando \ref cmdcallgraph "\\callgraph". La desactivacion de un grafo de llamadas se puede lograr con el comando \ref cmdhidecallgraph "\\hidecallgraph".</translation>
    </message>
<message>
        <source>CASE_SENSE_NAMES</source>
            <translation>Con la configuracion correcta de la opcion \c CASE_SENSE_NAMES, Doxygen podra utilizar mejor las capacidades del sistema de archivos subyacente. En el caso de un sistema de archivos que distingue entre mayusculas y minusculas (es decir, admite archivos en el mismo directorio cuyos nombres difieren solo en mayusculas/minusculas), la opcion debe configurarse en \c YES para manejar correctamente dichos archivos, si aparecen en la entrada. Para sistemas de archivos que no distinguen entre mayusculas y minusculas, la opcion debe configurarse en \c NO para manejar archivos de salida para simbolos que difieren solo en mayusculas/minusculas. En Windows (incluyendo Cygwin) y macOS, los usuarios normalmente deben configurar esta opcion en \c NO, mientras que en Linux u otras variantes de Unix, normalmente debe configurarse en \c YES.</translation>
    </message>
<message>
        <source>CHM_FILE</source>
            <translation>La etiqueta \c CHM_FILE se puede usar para especificar el nombre del archivo `.chm` resultante. Una ruta relativa se interpreta relativa al directorio \ref cfg_html_output "HTML_OUTPUT". Si se deja vacio, se usa el valor predeterminado `index.chm`.</translation>
    </message>
<message>
        <source>CHM_INDEX_ENCODING</source>
            <translation>La etiqueta \c CHM_INDEX_ENCODING se usa para codificar el contenido del indice, contenido y archivo de proyecto de HtmlHelp. Por defecto se usa la codificacion de la salida HTML.</translation>
    </message>
<message>
        <source>CITE_BIB_FILES</source>
            <translation>La etiqueta \c CITE_BIB_FILES se puede usar para especificar uno o mas archivos \c bib que contienen las definiciones de referencias. Esto debe ser una lista de archivos &lt;code&gt;.bib&lt;/code&gt;. La extension &lt;code&gt;.bib&lt;/code se anadira automaticamente si se omite. Esto requiere que la herramienta \c bibtex este instalada. Ver tambien https://en.wikipedia.org/wiki/BibTeX para mas informacion. Para \f$\mbox{\LaTeX}\f$ el estilo de la bibliografia se puede controlar con \ref cfg_latex_bib_style "LATEX_BIB_STYLE". Para usar esta funcion, necesita \c bibtex y \c perl en la ruta de busqueda. Ver tambien \ref cmdcite "\\cite" para informacion sobre como crear referencias.</translation>
    </message>
<message>
        <source>CLANG_ADD_INC_PATHS</source>
            <translation>Si la etiqueta \c CLANG_ASSISTED_PARSING esta configurada en \c YES y la etiqueta \c CLANG_ADD_INC_PATHS esta configurada en \c YES, Doxygen agregara el directorio de cada entrada a la ruta de inclusion.</translation>
    </message>
<message>
        <source>CLANG_ASSISTED_PARSING</source>
            <translation>Si la etiqueta \c CLANG_ASSISTED_PARSING esta configurada en \c YES, Doxygen usara el analizador clang para un analisis mas preciso a costa de un rendimiento reducido.</translation>
    </message>
<message>
        <source>CLANG_DATABASE_PATH</source>
            <translation>Si el analisis asistido por clang esta habilitado, puede proporcionar al analizador clang la ruta al directorio que contiene un archivo llamado `compile_commands.json`. Este archivo puede ser generado por CMake con la opcion CMAKE_EXPORT_COMPILE_COMMANDS.</translation>
    </message>
<message>
        <source>CLANG_OPTIONS</source>
            <translation>Si el analisis asistido por clang esta habilitado, puede proporcionar al compilador opciones de linea de comandos que normalmente usaria al invocar el compilador. Tenga en cuenta que las rutas de inclusion ya estan configuradas por Doxygen para los archivos y directorios especificados con \ref cfg_input "INPUT" y \ref cfg_include_path "INCLUDE_PATH".</translation>
    </message>
<message>
        <source>CLASS_GRAPH</source>
            <translation>Si la etiqueta \c CLASS_GRAPH esta configurada en \c YES o \c GRAPH o \c BUILTIN, Doxygen generara un grafo para cada clase documentada que muestre las relaciones de herencia directas e indirectas. Si la etiqueta \c CLASS_GRAPH esta configurada en \c YES o \c GRAPH y \ref cfg_have_dot "HAVE_DOT" tambien esta habilitado, se usara `dot` para dibujar el grafo. Si la etiqueta \c CLASS_GRAPH esta configurada en \c YES y \ref cfg_have_dot "HAVE_DOT" esta deshabilitado o si la etiqueta \c CLASS_GRAPH esta configurada en \c BUILTIN, se usara el generador incorporado. Si la etiqueta \c CLASS_GRAPH esta configurada en \c TEXT, las relaciones de herencia directas e indirectas se mostraran como texto/enlaces. La activacion explicita de un grafo de herencia o la eleccion de una representacion diferente para un grafo de herencia de una clase especifica se puede lograr con el comando \ref cmdinheritancegraph "\\inheritancegraph". La desactivacion de un grafo de herencia se puede lograr con el comando \ref cmdhideinheritancegraph "\\hideinheritancegraph".</translation>
    </message>
<message>
        <source>COLLABORATION_GRAPH</source>
            <translation>Si la etiqueta \c COLLABORATION_GRAPH esta configurada en \c YES, Doxygen generara un grafo para cada clase documentada que muestre las dependencias de implementacion directas e indirectas (herencia, contencion y variables de referencia de clase) de la clase con otras clases documentadas. La activacion explicita de un grafo de colaboracion cuando \c COLLABORATION_GRAPH esta configurado en \c NO se puede lograr con el comando \ref cmdcollaborationgraph "\\collaborationgraph". La desactivacion de un grafo de colaboracion se puede lograr con el comando \ref cmdhidecollaborationgraph "\\hidecollaborationgraph".</translation>
    </message>
<message>
        <source>COMPACT_LATEX</source>
            <translation>Si la etiqueta \c COMPACT_LATEX esta configurada en \c YES, Doxygen generara documentos \f$\mbox{\LaTeX}\f$ mas compactos. Esto puede ser util para proyectos pequenos y puede ayudar a ahorrar algunos arboles en general.</translation>
    </message>
<message>
        <source>COMPACT_RTF</source>
            <translation>Si la etiqueta \c COMPACT_RTF esta configurada en \c YES, Doxygen generara documentos RTF mas compactos. Esto puede ser util para proyectos pequenos y puede ayudar a ahorrar algunos arboles en general.</translation>
    </message>
<message>
        <source>CPP_CLI_SUPPORT</source>
            <translation>Si usa el lenguaje C++/CLI de Microsoft, debe configurar esta opcion en \c YES para habilitar el soporte de analisis.</translation>
    </message>
<message>
        <source>CREATE_SUBDIRS</source>
            <translation>Si la etiqueta \c CREATE_SUBDIRS esta configurada en \c YES, Doxygen creara hasta 4096 subdirectorios (en 2 niveles) bajo el directorio de salida de cada formato de salida y distribuirá los archivos generados en estos directorios. Habilitar esta opcion puede ser util cuando se alimenta a Doxygen con una gran cantidad de archivos fuente, donde colocar todos los archivos generados en el mismo directorio causaria problemas de rendimiento para el sistema de archivos. Ajuste \c CREATE_SUBDIRS_LEVEL para controlar el numero de subdirectorios.</translation>
    </message>
<message>
        <source>CREATE_SUBDIRS_LEVEL</source>
            <translation>Controla el numero de subdirectorios que se crean cuando la etiqueta \c CREATE_SUBDIRS esta configurada en \c YES. El nivel 0 representa 16 directorios, y cada aumento de nivel duplica el numero de directorios, resultando en 4096 directorios en el nivel 8, que es el valor predeterminado y tambien el valor maximo. Los subdirectorios estan organizados en 2 niveles, donde el primer nivel siempre tiene un numero fijo de 16 directorios.</translation>
    </message>
<message>
        <source>DIAFILE_DIRS</source>
            <translation>La etiqueta \c DIAFILE_DIRS se puede usar para especificar uno o mas directorios que contienen archivos dia que se incluyen en la documentacion (vea el comando \ref cmddiafile "\\diafile").</translation>
    </message>
<message>
        <source>DIA_PATH</source>
            <translation>Puede incluir diagramas creados con dia en la documentacion de Doxygen. Doxygen ejecutara dia para crear el diagrama e insertarlo en la documentacion. La etiqueta DIA_PATH le permite especificar el directorio donde se encuentra el binario de dia. Si se deja vacio, se asume que dia se encuentra en la ruta de busqueda predeterminada.</translation>
    </message>
<message>
        <source>DIRECTORY_GRAPH</source>
            <translation>Si la etiqueta \c DIRECTORY_GRAPH esta configurada en \c YES, Doxygen mostrara las dependencias de un directorio de otros directorios de manera grafica. Las relaciones de dependencia se determinan por las relaciones \c \#include entre los archivos en los directorios. La activacion explicita de un grafo de directorio cuando \c DIRECTORY_GRAPH esta configurado en \c NO se puede lograr con el comando \ref cmddirectorygraph "\\directorygraph". La desactivacion de un grafo de directorio se puede lograr con el comando \ref cmdhidedirectorygraph "\\hidedirectorygraph".</translation>
    </message>
<message>
        <source>DIR_GRAPH_MAX_DEPTH</source>
            <translation>La etiqueta \c DIR_GRAPH_MAX_DEPTH se puede usar para limitar el numero maximo de niveles de subdirectorios que se generan en los grafos de dependencia de directorios generados por \c dot.</translation>
    </message>
<message>
        <source>DISABLE_INDEX</source>
            <translation>Si desea tener control total sobre el diseño de las paginas HTML generadas, puede ser necesario deshabilitar el indice y reemplazarlo por el suyo propio. La etiqueta \c DISABLE_INDEX se puede usar para activar o desactivar el indice condensado (pestanas) en la parte superior de cada pagina HTML. Un valor de \c NO habilita el indice y el valor \c YES lo deshabilita. Dado que las pestanas en el indice contienen la misma informacion que el arbol de navegacion, puede configurar esta opcion en \c YES si tambien configura \ref cfg_generate_treeview "GENERATE_TREEVIEW" en \c YES.</translation>
    </message>
<message>
        <source>DISTRIBUTE_GROUP_DOC</source>
            <translation>Si se usa la agrupacion de miembros en la documentacion y la etiqueta \c DISTRIBUTE_GROUP_DOC esta configurada en \c YES, Doxygen reutilizara la documentacion del primer miembro del grupo para los otros miembros del grupo (si los hay). Por defecto, todos los miembros de un grupo deben estar documentados explícitamente.</translation>
    </message>
<message>
        <source>DOCBOOK_OUTPUT</source>
            <translation>La etiqueta \c DOCBOOK_OUTPUT se usa para especificar donde se colocaran las paginas Docbook. Si se ingresa una ruta relativa, el valor de \ref cfg_output_directory "OUTPUT_DIRECTORY" se antepondra.</translation>
    </message>
<message>
        <source>DOCSET_BUNDLE_ID</source>
            <translation>Esta etiqueta especifica una cadena que debe identificar de manera unica el paquete del conjunto de documentos. Esta debe ser una cadena en estilo de nombre de dominio inverso, por ejemplo, &lt;code&gt;com.mycompany.MyDocSet&lt;/code&gt;. Doxygen anadira &lt;code&gt;.docset&lt;/code&gt; al nombre.</translation>
    </message>
<message>
        <source>DOCSET_FEEDNAME</source>
            <translation>Esta etiqueta determina el nombre del feed del conjunto de documentos. Un feed de documentacion proporciona un paraguas bajo el cual se pueden agrupar multiples conjuntos de documentos de un solo proveedor (por ejemplo, una empresa o una suite de productos).</translation>
    </message>
<message>
        <source>DOCSET_FEEDURL</source>
            <translation>Esta etiqueta determina la URL del feed del conjunto de documentos. Un feed de documentacion proporciona un paraguas bajo el cual se pueden agrupar multiples conjuntos de documentos de un solo proveedor (por ejemplo, una empresa o una suite de productos).</translation>
    </message>
<message>
        <source>DOCSET_PUBLISHER_ID</source>
            <translation>La etiqueta \c DOCSET_PUBLISHER_ID especifica una cadena que debe identificar de manera unica al editor de documentacion. Esta debe ser una cadena en estilo de nombre de dominio inverso, por ejemplo, &lt;code&gt;com.mycompany.MyDocSet.documentation&lt;/code&gt;.</translation>
    </message>
<message>
        <source>DOCSET_PUBLISHER_NAME</source>
            <translation>La etiqueta \c DOCSET_PUBLISHER_NAME identifica al editor de documentacion.</translation>
    </message>
<message>
        <source>DOTFILE_DIRS</source>
            <translation>La etiqueta \c DOTFILE_DIRS se puede usar para especificar uno o mas directorios que contienen archivos dot que se incluyen en la documentacion (vea el comando \ref cmddotfile "\\dotfile").</translation>
    </message>
<message>
        <source>DOT_CLEANUP</source>
            <translation>Si la etiqueta \c DOT_CLEANUP esta configurada en \c YES, Doxygen eliminara los archivos intermedios que se usaron para generar los diferentes grafos. &lt;br&gt;Nota: Esta configuracion no solo se usa para archivos dot, sino tambien para archivos temporales msc.</translation>
    </message>
<message>
        <source>DOT_COMMON_ATTR</source>
            <translation>\c DOT_COMMON_ATTR son atributos comunes para nodos, aristas y etiquetas de subgrafos. Si desea una fuente diferente en los archivos dot generados por Doxygen, puede especificar los atributos fontname, fontcolor y fontsize. Para detalles, vea &lt;a href=https://graphviz.org/doc/info/attrs.html&gt;Especificacion de atributos de nodos, aristas y grafos&lt;/a&gt;. Debe asegurarse de que dot pueda encontrar la fuente, lo que se puede lograr colocandola en una ubicacion estandar o configurando la variable de entorno \c DOTFONTPATH o configurando \ref cfg_dot_fontpath "DOT_FONTPATH" al directorio que contiene la fuente. El tamano de fuente predeterminado de graphviz es 14.</translation>
    </message>
<message>
        <source>DOT_EDGE_ATTR</source>
            <translation>\c DOT_EDGE_ATTR se concatena con \ref cfg_dot_common_attr "DOT_COMMON_ATTR". Para un estilo elegante, puede agregar 'arrowhead=open, arrowtail=open, arrowsize=0.5'. &lt;a href=https://graphviz.org/doc/info/arrows.html&gt;Documentacion completa sobre formas de flechas.&lt;/a&gt;</translation>
    </message>
<message>
        <source>DOT_FONTPATH</source>
            <translation>Puede configurar la ruta donde \c dot puede encontrar la fuente especificada con fontname en \ref cfg_dot_common_attr "DOT_COMMON_ATTR" y otros atributos dot.</translation>
    </message>
<message>
        <source>DOT_GRAPH_MAX_NODES</source>
            <translation>La etiqueta \c DOT_GRAPH_MAX_NODES se puede usar para establecer el numero maximo de nodos que se mostraran en un grafo. Si el numero de nodos en un grafo se vuelve mayor que este valor, Doxygen truncara el grafo, lo cual se visualiza representando un nodo como una caja roja. Tenga en cuenta que el grafo no se mostrara en absoluto si el numero de hijos directos del nodo raiz en un grafo ya es mayor que \c DOT_GRAPH_MAX_NODES. Tenga en cuenta tambien que el tamano de un grafo puede estar mas limitado por \ref cfg_max_dot_graph_depth "MAX_DOT_GRAPH_DEPTH".</translation>
    </message>
<message>
        <source>DOT_IMAGE_FORMAT</source>
            <translation>La etiqueta \c DOT_IMAGE_FORMAT se puede usar para establecer el formato de imagen de las imagenes generadas por \c dot. Para una explicacion de los formatos de imagen, vea la seccion de formatos de salida en la documentacion de la herramienta \c dot (&lt;a href="https://www.graphviz.org/"&gt;Graphviz&lt;/a&gt;). &lt;br&gt;Tenga en cuenta que los formatos `svg:cairo` y `svg:cairo:cairo` no se pueden usar en combinacion con \ref cfg_interactive_svg "INTERACTIVE_SVG" (\ref cfg_interactive_svg "INTERACTIVE_SVG" se configurara en `NO`).</translation>
    </message>
<message>
        <source>DOT_MULTI_TARGETS</source>
            <translation>Configure la etiqueta \c DOT_MULTI_TARGETS en \c YES para permitir que \c dot genere multiples archivos de salida en una sola ejecucion (es decir, multiples opciones -o y -T en la linea de comandos). Esto hace que \c dot sea mas rapido, pero dado que solo las versiones mas nuevas de \c dot (&gt;1.8.10) admiten esto, esta funcion esta deshabilitada por defecto.</translation>
    </message>
<message>
        <source>DOT_NODE_ATTR</source>
            <translation>\c DOT_NODE_ATTR se concatena con \ref cfg_dot_common_attr "DOT_COMMON_ATTR". Para una vista sin caja alrededor de los nodos, configure 'shape=plain' o 'shape=plaintext'. &lt;a href=https://www.graphviz.org/doc/info/shapes.html&gt;Especificacion de formas&lt;/a&gt;</translation>
    </message>
<message>
        <source>DOT_NUM_THREADS</source>
            <translation>La etiqueta \c DOT_NUM_THREADS especifica el numero de invocaciones de \c dot que Doxygen puede ejecutar en paralelo. Cuando se configura en \c 0, Doxygen lo basara en el numero de procesadores disponibles en el sistema. Puede configurarlo explicitamente en un valor mayor que 0 para obtener control sobre el equilibrio entre la carga de la CPU y la velocidad de procesamiento.</translation>
    </message>
<message>
        <source>DOT_PATH</source>
            <translation>La etiqueta \c DOT_PATH se puede usar para especificar la ruta donde se puede encontrar la herramienta \c dot. Si se deja vacio, se asume que dot se encuentra en la ruta de busqueda predeterminada.</translation>
    </message>
<message>
        <source>DOT_UML_DETAILS</source>
            <translation>Si la etiqueta \c DOT_UML_DETAILS esta configurada en \c NO, Doxygen mostrara atributos y metodos sin tipos y argumentos en los grafos UML. Si la etiqueta \c DOT_UML_DETAILS esta configurada en \c YES, Doxygen agregara tipo y argumentos para atributos y metodos en los grafos UML. Si la etiqueta \c DOT_UML_DETAILS esta configurada en \c NONE, Doxygen no generara campos con informacion de miembros de clase en los grafos UML. Los diagramas de clase se veran similares a los diagramas de clase predeterminados pero usando notacion UML para las relaciones.</translation>
    </message>
<message>
        <source>DOT_WRAP_THRESHOLD</source>
            <translation>La etiqueta \c DOT_WRAP_THRESHOLD se puede usar para establecer el numero maximo de caracteres que se mostraran en una sola linea. Si la longitud de la linea excede este valor, la linea se ajustara.</translation>
    </message>
<message>
        <source>ECLIPSE_DOC_ID</source>
            <translation>Un identificador unico para el plugin de ayuda de Eclipse. Esta debe ser una cadena en estilo de nombre de dominio inverso, por ejemplo, &lt;code&gt;org.doxygen.Project&lt;/code&gt;.</translation>
    </message>
<message>
        <source>ENABLED_SECTIONS</source>
            <translation>La etiqueta \c ENABLED_SECTIONS se puede usar para habilitar secciones de documentacion condicional marcadas con bloques \ref cmdif "\\if" \&lt;section_label\&gt; ... \ref cmdendif "\\endif" y \ref cmdcond "\\cond" \&lt;section_label\&gt; ... \ref cmdendcond "\\endcond".</translation>
    </message>
<message>
        <source>ENABLE_PREPROCESSING</source>
            <translation>Si la etiqueta \c ENABLE_PREPROCESSING esta configurada en \c YES, Doxygen evaluara todas las directivas del preprocesador C.</translation>
    </message>
<message>
        <source>ENUM_VALUES_PER_LINE</source>
            <translation>La etiqueta \c ENUM_VALUES_PER_LINE se puede usar para establecer el numero de valores de enumeracion que Doxygen agrupara en una linea.</translation>
    </message>
<message>
        <source>EXAMPLE_PATH</source>
            <translation>La etiqueta \c EXAMPLE_PATH se puede usar para especificar uno o mas archivos o directorios que contienen fragmentos de codigo de ejemplo.</translation>
    </message>
<message>
        <source>EXAMPLE_PATTERNS</source>
            <translation>Si el valor de la etiqueta \ref cfg_example_path "EXAMPLE_PATH" contiene directorios, puede usar la etiqueta \c EXAMPLE_PATTERNS para especificar uno o mas patrones de comodin para filtrar los archivos fuente en los directorios.</translation>
    </message>
<message>
        <source>EXAMPLE_RECURSIVE</source>
            <translation>Si la etiqueta \c EXAMPLE_RECURSIVE esta configurada en \c YES, se buscaran subdirectorios en busca de archivos de entrada. Si se configura en \c NO, solo se buscaran los directorios especificados en la etiqueta \ref cfg_example_path "EXAMPLE_PATH".</translation>
    </message>
<message>
        <source>EXCLUDE</source>
            <translation>La etiqueta \c EXCLUDE se puede usar para especificar archivos y/o directorios que deben excluirse de los archivos fuente \ref cfg_input "INPUT". De esta manera puede excluir facilmente un subdirectorio de un arbol de directorios cuya raiz se especifica con la etiqueta \ref cfg_input "INPUT". &lt;br&gt;Tenga en cuenta que las rutas relativas son relativas al directorio desde donde se ejecuta Doxygen.</translation>
    </message>
<message>
        <source>EXCLUDE_PATTERNS</source>
            <translation>Si el valor de la etiqueta \ref cfg_input "INPUT" contiene directorios, puede usar la etiqueta \c EXCLUDE_PATTERNS para especificar uno o mas patrones de comodin para excluir ciertos archivos de esos directorios.</translation>
    </message>
<message>
        <source>EXCLUDE_SYMBOLS</source>
            <translation>La etiqueta \c EXCLUDE_SYMBOLS se puede usar para especificar uno o mas nombres de simbolos que deben excluirse de la salida. El simbolo puede ser un nombre de funcion, variable, estructura, typedef, etc.</translation>
    </message>
<message>
        <source>EXCLUDE_SYMLINKS</source>
            <translation>La etiqueta \c EXCLUDE_SYMLINKS se puede usar para seleccionar si los archivos o directorios que son enlaces simbolicos deben excluirse de la entrada.</translation>
    </message>
<message>
        <source>EXPAND_AS_DEFINED</source>
            <translation>Si las etiquetas \ref cfg_macro_expansion "MACRO_EXPANSION" y \ref cfg_expand_only_predef "EXPAND_ONLY_PREDEF" estan configuradas en \c YES, esta etiqueta se puede usar para especificar una lista de nombres de macros que deben expandirse.</translation>
    </message>
<message>
        <source>EXPAND_ONLY_PREDEF</source>
            <translation>Si las etiquetas \c EXPAND_ONLY_PREDEF y \ref cfg_macro_expansion "MACRO_EXPANSION" estan configuradas en \c YES, la expansion de macros se limita a las macros especificadas con \ref cfg_expand_as_defined "EXPAND_AS_DEFINED".</translation>
    </message>
<message>
        <source>EXTENSION_MAPPING</source>
            <translation>Doxygen selecciona el analizador a usar basandose en la extension del archivo a analizar. Con esta etiqueta puede asignar un analizador para usar con una extension dada. Doxygen tiene un mapeo incorporado, pero puede anularlo o extenderlo con esta etiqueta. El formato es &lt;code&gt;ext=lenguaje&lt;/code&gt;, donde \c ext es una extension de archivo y lenguaje es uno de los analizadores soportados por Doxygen: IDL, Java, JavaScript, Csharp (C#), C, C++, Lex, D, PHP, md (Markdown), Objective-C, Python, Slice, VHDL, Fortran (Fortran de formato fijo: FortranFixed, Fortran de formato libre: FortranFree, Fortran de formato desconocido: Fortran. En este ultimo caso, el analizador intentara adivinar si el codigo es de formato fijo o libre, que es el valor predeterminado para archivos de tipo Fortran). Por ejemplo, para hacer que Doxygen trate los archivos &lt;code&gt;.inc&lt;/code&gt; como archivos Fortran (el valor predeterminado es PHP) y los archivos &lt;code&gt;.f&lt;/code&gt; como C (el valor predeterminado es Fortran), use: `inc=Fortran f=C`. &lt;br&gt;Nota: Para archivos sin extension, puede usar `no_extension` como comodin. &lt;br&gt;Nota: Para extensiones personalizadas, tambien debe configurar \ref cfg_file_patterns "FILE_PATTERNS", de lo contrario los archivos no seran leidos por Doxygen. &lt;br&gt;Nota: Vea tambien la lista de \ref default_file_extension_mapping "mapeos de extensiones de archivo predeterminados".</translation>
    </message>
<message>
        <source>EXTERNAL_GROUPS</source>
            <translation>Si la etiqueta \c EXTERNAL_GROUPS esta configurada en \c YES, todos los grupos externos se listaran en el indice de temas. Si se configura en \c NO, solo se listaran los grupos del proyecto actual.</translation>
    </message>
<message>
        <source>EXTERNAL_PAGES</source>
            <translation>Si la etiqueta \c EXTERNAL_PAGES esta configurada en \c YES, todas las paginas externas se listaran en el indice de paginas relacionadas. Si se configura en \c NO, solo se listaran las paginas del proyecto actual.</translation>
    </message>
<message>
        <source>EXTERNAL_SEARCH</source>
            <translation>Si la etiqueta \c EXTERNAL_SEARCH esta habilitada, Doxygen ya no generara el script PHP para buscar. En su lugar, los resultados de busqueda se escriben en un archivo XML que debe ser procesado por un indexador externo. Doxygen invocara un motor de busqueda externo especificado por la opcion \ref cfg_searchengine_url "SEARCHENGINE_URL" para obtener los resultados de busqueda. &lt;br&gt;Doxygen proporciona un indexador de ejemplo (\c doxyindexer) y un motor de busqueda (&lt;code&gt;doxysearch.cgi&lt;/code&gt;) basado en la biblioteca del motor de busqueda de codigo abierto &lt;a href="https://xapian.org/"&gt;Xapian&lt;/a&gt;. &lt;br&gt;Vea la seccion \ref extsearch para mas detalles.</translation>
    </message>
<message>
        <source>EXTERNAL_SEARCH_ID</source>
            <translation>Si \ref cfg_server_based_search "SERVER_BASED_SEARCH" y \ref cfg_external_search "EXTERNAL_SEARCH" estan habilitados, la etiqueta \c EXTERNAL_SEARCH_ID se puede usar como identificador para el proyecto. Esto es util en combinacion con \ref cfg_extra_search_mappings "EXTRA_SEARCH_MAPPINGS" para buscar en multiples proyectos y dirigir los resultados de vuelta al proyecto correcto.</translation>
    </message>
<message>
        <source>EXTERNAL_TOOL_PATH</source>
            <translation>La etiqueta \c EXTERNAL_TOOL_PATH se puede usar para extender la ruta de busqueda (variable de entorno PATH) para que las herramientas externas como \c latex y \c gs puedan ser encontradas. \nota Los directorios especificados con EXTERNAL_TOOL_PATH se agregan antes de las rutas ya especificadas por la variable PATH y se agregan en el orden especificado. \nota Esta opcion es particularmente util para macOS version 14 (Sonoma) y superior cuando Doxygen se ejecuta desde Doxywizard, ya que en ese caso se ignoran todos los cambios personalizados en PATH. Un ejemplo tipico en macOS es configurar \verbatim EXTERNAL_TOOL_PATH = /Library/TeX/texbin /usr/local/bin \endverbatim Junto con la ruta predeterminada, la ruta de busqueda completa que Doxygen usara al iniciar herramientas externas se convierte en \verbatim PATH=/Library/TeX/texbin:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin \endverbatim</translation>
    </message>
<message>
        <source>EXTRACT_ALL</source>
            <translation>Si la etiqueta \c EXTRACT_ALL esta configurada en \c YES, Doxygen asumira que todas las entidades en la documentacion estan documentadas, incluso si no habia documentacion disponible. Los miembros privados de clase y los miembros estaticos de archivo se ocultaran a menos que las etiquetas \ref cfg_extract_private "EXTRACT_PRIVATE" y \ref cfg_extract_static "EXTRACT_STATIC" esten configuradas en \c YES. \nota Esto tambien deshabilita las advertencias sobre miembros no documentados que normalmente se generan cuando \ref cfg_warnings "WARNINGS" esta configurado en \c YES.</translation>
    </message>
<message>
        <source>EXTRACT_ANON_NSPACES</source>
            <translation>Si esta bandera esta configurada en \c YES, los miembros de espacios de nombres anonimos seran extraidos y apareceran en la documentacion como un espacio de nombres llamado 'anonymous_namespace{file}', donde file se reemplaza por el nombre base del archivo que contiene el espacio de nombres anonimo. Por defecto, los espacios de nombres anonimos estan ocultos.</translation>
    </message>
<message>
        <source>EXTRACT_LOCAL_CLASSES</source>
            <translation>Si la etiqueta \c EXTRACT_LOCAL_CLASSES esta configurada en \c YES, las clases definidas localmente en archivos fuente (y estructuras) se incluiran en la documentacion. Si se configura en \c NO, solo se incluiran las clases definidas en archivos de encabezado. No tiene efecto para las fuentes Java.</translation>
    </message>
<message>
        <source>EXTRACT_LOCAL_METHODS</source>
            <translation>Esta bandera solo es util para codigo Objective-C. Si se configura en \c YES, los metodos locales definidos en la seccion de implementacion pero no en la interfaz se incluiran en la documentacion. Si se configura en \c NO, solo se incluiran los metodos en la interfaz.</translation>
    </message>
<message>
        <source>EXTRACT_PACKAGE</source>
            <translation>Si la etiqueta \c EXTRACT_PACKAGE esta configurada en \c YES, todos los miembros con alcance de visibilidad de paquete o interno se incluiran en la documentacion.</translation>
    </message>
<message>
        <source>EXTRACT_PRIVATE</source>
            <translation>Si la etiqueta \c EXTRACT_PRIVATE esta configurada en \c YES, todos los miembros privados de una clase se incluiran en la documentacion.</translation>
    </message>
<message>
        <source>EXTRACT_PRIV_VIRTUAL</source>
            <translation>Si la etiqueta \c EXTRACT_PRIV_VIRTUAL esta configurada en \c YES, los metodos virtuales privados documentados de una clase se incluiran en la documentacion.</translation>
    </message>
<message>
        <source>EXTRACT_STATIC</source>
            <translation>Si la etiqueta \c EXTRACT_STATIC esta configurada en \c YES, todos los miembros estaticos de un archivo se incluiran en la documentacion.</translation>
    </message>
<message>
        <source>EXTRA_PACKAGES</source>
            <translation>La etiqueta \c EXTRA_PACKAGES se puede usar para especificar uno o mas nombres de paquetes \f$\mbox{\LaTeX}\f$ que se incluiran en la salida \f$\mbox{\LaTeX}\f$. El paquete se puede especificar solo por su nombre o con la sintaxis correcta para usar con el comando `\usepackage` de \f$\mbox{\LaTeX}\f$. Por ejemplo, para obtener la fuente `times`, puede especificar: \verbatim EXTRA_PACKAGES=times o EXTRA_PACKAGES={times} \endverbatim Para usar la opcion `intlimits` con el paquete `amsmath`, puede especificar: \verbatim EXTRA_PACKAGES=[intlimits]{amsmath} \endverbatim Si se deja vacio, no se incluiran paquetes adicionales.</translation>
    </message>
<message>
        <source>EXTRA_SEARCH_MAPPINGS</source>
            <translation>La etiqueta \c EXTRA_SEARCH_MAPPINGS se puede usar para habilitar la busqueda en proyectos Doxygen que no estan definidos por este archivo de configuracion pero que se agregan todos al mismo indice de busqueda externo. Cada proyecto debe tener una ID unica que se establece a traves de \ref cfg_external_search_id "EXTERNAL_SEARCH_ID". El mapeo de busqueda luego asigna la ID a una ubicacion relativa donde se puede encontrar la documentacion. El formato es: \verbatim EXTRA_SEARCH_MAPPINGS = tagname1=loc1 tagname2=loc2 ... \endverbatim</translation>
    </message>
<message>
        <source>EXT_LINKS_IN_WINDOW</source>
            <translation>Si la opcion \c EXT_LINKS_IN_WINDOW esta configurada en \c YES, Doxygen abrira enlaces a simbolos externos importados a traves de archivos de etiquetas en una ventana separada.</translation>
    </message>
<message>
        <source>FILE_PATTERNS</source>
            <translation>Si el valor de la etiqueta \ref cfg_input "INPUT" contiene directorios, puede usar la etiqueta \c FILE_PATTERNS para especificar uno o mas patrones de comodin (como `*.cpp` y `*.h`) para filtrar los archivos fuente en los directorios.&lt;br&gt; Tenga en cuenta que para extensiones personalizadas o extensiones no soportadas directamente, tambien debe configurar \ref cfg_extension_mapping "EXTENSION_MAPPING" para la extension, de lo contrario los archivos no seran leidos por Doxygen.&lt;br&gt; Tenga en cuenta que la lista de patrones de archivo verificados por defecto puede diferir de la lista de \ref default_file_extension_mapping "mapeos de extensiones de archivo predeterminados".&lt;br&gt; Si se deja vacio, se verificaran los siguientes patrones:</translation>
    </message>
<message>
        <source>FILE_VERSION_FILTER</source>
            <translation>La etiqueta \c FILE_VERSION_FILTER se puede usar para especificar un programa o script que Doxygen debe invocar para obtener la version actual de cada archivo (tipicamente del sistema de control de versiones). Doxygen invocara el programa ejecutando (via &lt;code&gt;popen()&lt;/code&gt;) el comando &lt;code&gt;command input-file&lt;/code&gt;, donde \c command es el valor de la etiqueta \c FILE_VERSION_FILTER y \c input-file es el nombre de un archivo de entrada proporcionado por Doxygen. Lo que sea que el programa escriba en la salida estandar se usara como la version del archivo. Para un ejemplo, vea la documentacion.</translation>
    </message>
<message>
        <source>FILTER_PATTERNS</source>
            <translation>La etiqueta \c FILTER_PATTERNS se puede usar para especificar filtros basados en patrones de archivo. Doxygen comparara el nombre del archivo con cada patron y aplicara el filtro si hay una coincidencia. Los filtros son una lista de la forma: patron=filtro (como `*.cpp=my_cpp_filter`). Vea \ref cfg_input_filter "INPUT_FILTER" para mas informacion sobre el uso de filtros. Si la etiqueta \c FILTER_PATTERNS esta vacia o ninguno de los patrones coincide con el nombre del archivo, se aplicara \ref cfg_input_filter "INPUT_FILTER". &lt;br&gt;Tenga en cuenta que para extensiones personalizadas o extensiones no soportadas directamente, tambien debe configurar \ref cfg_extension_mapping "EXTENSION_MAPPING" para la extension, de lo contrario los archivos no seran procesados correctamente por Doxygen.&lt;br&gt;</translation>
    </message>
<message>
        <source>FILTER_SOURCE_FILES</source>
            <translation>Si la etiqueta \c FILTER_SOURCE_FILES esta configurada en \c YES, el filtro de entrada (si se configura con \ref cfg_input_filter "INPUT_FILTER") tambien se usara para filtrar los archivos de entrada utilizados para crear los archivos fuente para navegar (es decir, cuando \ref cfg_source_browser "SOURCE_BROWSER" esta configurado en \c YES).</translation>
    </message>
<message>
        <source>FILTER_SOURCE_PATTERNS</source>
            <translation>La etiqueta \c FILTER_SOURCE_PATTERNS se puede usar para especificar filtros de fuente por patron de archivo. Un patron anula la configuracion de \ref cfg_filter_patterns "FILTER_PATTERN" (si existe) y tambien es posible deshabilitar el filtrado de fuente para un patron particular con `*.ext=` (es decir, sin nombrar un filtro).</translation>
    </message>
<message>
        <source>FORCE_LOCAL_INCLUDES</source>
            <translation>Si la etiqueta \c FORCE_LOCAL_INCLUDES esta configurada en \c YES, Doxygen listara los archivos de inclusion con comillas dobles en la documentacion en lugar de con corchetes angulares.</translation>
    </message>
<message>
        <source>FORMULA_FONTSIZE</source>
            <translation>Use esta etiqueta para cambiar el tamano de fuente de las formulas \f$\mbox{\LaTeX}\f$ que se incluyen como imagenes en la documentacion HTML. Si cambia el tamano de fuente despues de una ejecucion exitosa de Doxygen, debe eliminar manualmente todas las imagenes `form_*.png` del directorio de salida HTML para forzar su regeneracion.</translation>
    </message>
<message>
        <source>FORMULA_MACROFILE</source>
            <translation>El archivo \c FORMULA_MACROFILE puede contener comandos \f$\mbox{\LaTeX}\f$ `\newcommand` y `\renewcommand` para crear nuevos comandos \f$\mbox{\LaTeX}\f$ que sirvan como bloques de construccion en las formulas. Vea la seccion \ref formulas para mas detalles.</translation>
    </message>
<message>
        <source>FORTRAN_COMMENT_AFTER</source>
            <translation>El estandar Fortran especifica que para el codigo Fortran de formato fijo, todos los caracteres desde la posicion 72 se consideran comentarios. Una extension comun es permitir lineas mas largas antes de que comience el comentario automatico. La configuracion \c FORTRAN_COMMENT_AFTER tambien hace posible que se procesen lineas mas largas antes de que comience el comentario automatico.</translation>
    </message>
<message>
        <source>FULL_PATH_NAMES</source>
            <translation>Si la etiqueta \c FULL_PATH_NAMES esta configurada en \c YES, Doxygen antepondra la ruta completa antes de los nombres de archivo en la lista de archivos y en los archivos de encabezado. Si se configura en \c NO, se usara la ruta mas corta que haga que los nombres de archivo sean unicos.</translation>
    </message>
<message>
        <source>FULL_SIDEBAR</source>
            <translation>Si \ref cfg_generate_treeview "GENERATE_TREEVIEW" esta configurado en \c YES, la opcion \c FULL_SIDEBAR determina si la barra lateral esta limitada al area del arbol de navegacion (valor \c NO) o si debe extenderse a toda la altura de la ventana (valor \c YES). Configurar esto en \c YES dara como resultado un diseño similar a, por ejemplo, https://docs.readthedocs.io con mas espacio para contenido pero menos espacio para el logotipo del proyecto, titulo y descripcion. Si \ref cfg_generate_treeview "GENERATE_TREEVIEW" esta configurado en \c NO, esta opcion no tiene efecto.</translation>
    </message>
<message>
        <source>GENERATE_AUTOGEN_DEF</source>
            <translation>Si la etiqueta \c GENERATE_AUTOGEN_DEF esta configurada en \c YES, Doxygen generara un archivo de definicion AutoGen (vea https://autogen.sourceforge.net/) que captura la estructura del codigo incluyendo toda la documentacion. Tenga en cuenta que esta funcion es actualmente experimental e incompleta.</translation>
    </message>
<message>
        <source>GENERATE_BUGLIST</source>
            <translation>La etiqueta \c GENERATE_BUGLIST se puede usar para habilitar (\c YES) o deshabilitar (\c NO) la lista de errores. Esta lista se crea colocando comandos \ref cmdbug "\\bug" en la documentacion.</translation>
    </message>
<message>
        <source>GENERATE_CHI</source>
            <translation>La bandera \c GENERATE_CHI controla si se genera un archivo de indice `.chi` separado (\c YES) o si debe incluirse en el archivo principal `.chm` (\c NO).</translation>
    </message>
<message>
        <source>GENERATE_DEPRECATEDLIST</source>
            <translation>La etiqueta \c GENERATE_DEPRECATEDLIST se puede usar para habilitar (\c YES) o deshabilitar (\c NO) la lista de obsoletos. Esta lista se crea colocando comandos \ref cmddeprecated "\\deprecated" en la documentacion.</translation>
    </message>
<message>
        <source>GENERATE_DOCBOOK</source>
            <translation>Si la etiqueta \c GENERATE_DOCBOOK esta configurada en \c YES, Doxygen generara archivos Docbook que se pueden usar para crear PDF.</translation>
    </message>
<message>
        <source>GENERATE_DOCSET</source>
            <translation>Si la etiqueta \c GENERATE_DOCSET esta configurada en \c YES, se generaran archivos de indice adicionales que se pueden usar como entrada para el &lt;a href="https://developer.apple.com/xcode/"&gt;entorno de desarrollo integrado Xcode 3 de Apple&lt;/a&gt; que se introdujo con OSX 10.5 (Leopard). Para crear un conjunto de documentos, Doxygen generara un Makefile en el directorio de salida HTML. Ejecutar \c make creara el conjunto de documentos en ese directorio y ejecutar &lt;code&gt;make install&lt;/code&gt; instalara el conjunto de documentos en &lt;code&gt;~/Library/Developer/Shared/Documentation/DocSets&lt;/code&gt; para que Xcode lo encuentre al inicio. Vea https://developer.apple.com/library/archive/featuredarticles/DoxygenXcode/_index.html para mas informacion.</translation>
    </message>
<message>
        <source>GENERATE_ECLIPSEHELP</source>
            <translation>Si la etiqueta \c GENERATE_ECLIPSEHELP esta configurada en \c YES, se generaran archivos de indice adicionales que junto con los archivos HTML forman un plugin de ayuda de Eclipse. Para instalar el plugin, coloque el directorio que contiene los archivos HTML y de indice en su directorio de instalacion de Eclipse en `plugins`.</translation>
    </message>
<message>
        <source>GENERATE_HTML</source>
            <translation>Si la etiqueta \c GENERATE_HTML esta configurada en \c YES, Doxygen generara salida HTML.</translation>
    </message>
<message>
        <source>GENERATE_HTMLHELP</source>
            <translation>Si la etiqueta \c GENERATE_HTMLHELP esta configurada en \c YES, Doxygen generara tres archivos de indice HTML adicionales: \c index.hhp, \c index.hhc y \c index.hhk. El \c index.hhp es un archivo de proyecto que puede ser leido por el Taller de Ayuda HTML de Microsoft en Windows. A principios de 2021, Microsoft desconecto la pagina original con los enlaces de descarga (el Taller de Ayuda HTML ya estaba en modo de mantenimiento durante muchos anos). Puede descargar el Taller de Ayuda HTML desde los archivos web en &lt;a href="http://web.archive.org/web/20160201063255/https://download.microsoft.com/download/0/A/9/0A939EF6-E31C-430F-A3DF-DFAE7960D564/htmlhelp.exe"&gt;ejecutable de instalacion&lt;/a&gt;. &lt;br&gt;El Taller de Ayuda HTML contiene un compilador que convertira toda la salida HTML generada por Doxygen en un unico archivo HTML compilado (`.chm`). Los archivos HTML compilados ahora se usan como el formato de ayuda de Windows 98 y reemplazaran el antiguo formato de ayuda de Windows (`.hlp`) en todas las plataformas Windows en el futuro. Los archivos HTML comprimidos tambien contienen un indice, una tabla de contenidos y puede buscar palabras en la documentacion. El taller HTML tambien contiene un visor para archivos HTML comprimidos.</translation>
    </message>
<message>
        <source>GENERATE_LATEX</source>
            <translation>Si la etiqueta \c GENERATE_LATEX esta configurada en \c YES, Doxygen generara salida \f$\mbox{\LaTeX}\f$.</translation>
    </message>
<message>
        <source>GENERATE_LEGEND</source>
            <translation>Si la etiqueta \c GENERATE_LEGEND esta configurada en \c YES, Doxygen generara una pagina de leyenda que explica el significado de los diferentes cuadros en los grafos producidos por dot.</translation>
    </message>
<message>
        <source>GENERATE_MAN</source>
            <translation>Si la etiqueta \c GENERATE_MAN esta configurada en \c YES, Doxygen generara paginas de manual para clases y archivos.</translation>
    </message>
<message>
        <source>GENERATE_PERLMOD</source>
            <translation>Si la etiqueta \c GENERATE_PERLMOD esta configurada en \c YES, Doxygen generara un archivo de modulo Perl que captura cierta informacion sobre el codigo en una forma adecuada para modulos Perl.</translation>
    </message>
<message>
        <source>GENERATE_QHP</source>
            <translation>Si la etiqueta \c GENERATE_QHP esta configurada en \c YES y tanto \ref cfg_qhp_namespace "QHP_NAMESPACE" como \ref cfg_qhp_virtual_folder "QHP_VIRTUAL_FOLDER" estan configurados, se generara un archivo de indice adicional que se puede usar como entrada para el qhelpgenerator de Qt para crear una Ayuda Comprimida de Qt (`.qch`) de la documentacion HTML generada.</translation>
    </message>
<message>
        <source>GENERATE_REQUIREMENTS</source>
            <translation>La etiqueta \c GENERATE_REQUIREMENTS se puede usar para habilitar (\c YES) o deshabilitar (\c NO) la pagina de requisitos. Cuando esta habilitada, esta pagina se crea automaticamente cuando al menos un bloque de comentarios con un comando \ref cmdrequirement "\\requirement" aparece en la entrada.</translation>
    </message>
<message>
        <source>GENERATE_RTF</source>
            <translation>Si la etiqueta \c GENERATE_RTF esta configurada en \c YES, Doxygen generara salida RTF. La salida RTF esta optimizada para Word 97 y puede no verse muy bien con otros lectores/editores RTF.</translation>
    </message>
<message>
        <source>GENERATE_SQLITE3</source>
            <translation>Si la etiqueta \c GENERATE_SQLITE3 esta configurada en \c YES, Doxygen generara una base de datos \c Sqlite3 con simbolos que puede ser usada por el motor de busqueda.</translation>
    </message>
<message>
        <source>GENERATE_TAGFILE</source>
            <translation>Si se especifica un nombre de archivo despues de \c GENERATE_TAGFILE, Doxygen creara un archivo de etiquetas que puede ser usado por otro proyecto para crear referencias a este proyecto.</translation>
    </message>
<message>
        <source>GENERATE_TESTLIST</source>
            <translation>La etiqueta \c GENERATE_TESTLIST se puede usar para habilitar (\c YES) o deshabilitar (\c NO) la lista de pruebas. Esta lista se crea colocando comandos \ref cmdtest "\\test" en la documentacion.</translation>
    </message>
<message>
        <source>GENERATE_TODOLIST</source>
            <translation>La etiqueta \c GENERATE_TODOLIST se puede usar para habilitar (\c YES) o deshabilitar (\c NO) la lista de tareas. Esta lista se crea colocando comandos \ref cmdtodo "\\todo" en la documentacion.</translation>
    </message>
<message>
        <source>GENERATE_TREEVIEW</source>
            <translation>La etiqueta \c GENERATE_TREEVIEW se usa para especificar si se debe generar una estructura de indice en forma de arbol para mostrar informacion jerarquica. Si el valor de la etiqueta esta configurado en \c YES, se generara un panel lateral que contiene una estructura de indice en forma de arbol (igual que la que se genera para la Ayuda HTML). Para que esto funcione se requiere un navegador que soporte JavaScript, DHTML, CSS y marcos (es decir, cualquier navegador moderno). Los usuarios de Windows probablemente esten mejor usando la funcion de ayuda HTML. A traves de hojas de estilo personalizadas (vea \ref cfg_html_extra_stylesheet "HTML_EXTRA_STYLESHEET") se puede ajustar aun mas la apariencia del indice (vea \ref doxygen_finetune). Como ejemplo, la hoja de estilo predeterminada generada por Doxygen tiene un ejemplo que muestra como colocar una imagen en la raiz del arbol en lugar del \ref cfg_project_name "PROJECT_NAME".</translation>
    </message>
<message>
        <source>GENERATE_XML</source>
            <translation>Si la etiqueta \c GENERATE_XML esta configurada en \c YES, Doxygen generara un archivo XML que captura el codigo y la documentacion.</translation>
    </message>
<message>
        <source>GRAPHICAL_HIERARCHY</source>
            <translation>Si la etiqueta \c GRAPHICAL_HIERARCHY esta configurada en \c YES, Doxygen mostrara una jerarquia grafica de todas las clases. Tenga en cuenta que habilitar esta opcion requiere que la configuracion \ref cfg_have_dot "HAVE_DOT" este en \c YES.</translation>
    </message>
<message>
        <source>GROUP_GRAPHS</source>
            <translation>Si la etiqueta \c GROUP_GRAPHS esta configurada en \c YES, Doxygen generara un grafo para grupos que muestre las dependencias directas de grupos. La activacion explicita de un grafo de dependencia de grupos cuando \c GROUP_GRAPHS esta configurado en \c NO se puede lograr con el comando \ref cmdgroupgraph "\\groupgraph". La desactivacion de un grafo de dependencia de grupos se puede lograr con el comando \ref cmdhidegroupgraph "\\hidegroupgraph". Vea tambien el capitulo \ref grouping "Agrupacion" en el manual.</translation>
    </message>
<message>
        <source>GROUP_NESTED_COMPOUNDS</source>
            <translation>Cuando una estructura o clase se agrega a un grupo y esta opcion esta habilitada, todas las clases o estructuras anidadas tambien se agregaran al mismo grupo. Por defecto, esta opcion esta deshabilitada y uno debe agregar entidades anidadas explicitamente a traves de \ref cmdingroup "\\ingroup".</translation>
    </message>
<message>
        <source>HAVE_DOT</source>
            <translation>Si configura la etiqueta \c HAVE_DOT en \c YES, Doxygen asumira que la herramienta \c dot esta disponible en el \c path. Esta herramienta es parte de &lt;a href="https://www.graphviz.org/"&gt;Graphviz&lt;/a&gt;, un kit de herramientas de visualizacion de grafos de AT&amp;T y Lucent Bell Labs. Las otras opciones en esta seccion no tendran efecto si esta opcion esta configurada en \c NO.</translation>
    </message>
<message>
        <source>HHC_LOCATION</source>
            <translation>La etiqueta \c HHC_LOCATION se puede usar para especificar la ubicacion (ruta absoluta incluyendo nombre de archivo) del compilador de ayuda HTML (\c hhc.exe). Si no esta vacio, Doxygen intentara ejecutar el compilador de ayuda HTML en el archivo \c index.hhp generado.</translation>
    </message>
<message>
        <source>HIDE_COMPOUND_REFERENCE</source>
            <translation>Si la etiqueta \c HIDE_COMPOUND_REFERENCE esta configurada en \c NO (predeterminado), Doxygen agregara texto adicional al titulo de una pagina, como Referencia de Clase. Si se configura en \c YES, la referencia de compuesto se ocultara.</translation>
    </message>
<message>
        <source>HIDE_FRIEND_COMPOUNDS</source>
            <translation>Si la etiqueta \c HIDE_FRIEND_COMPOUNDS esta configurada en \c YES, todas las declaraciones de amigo se ocultaran. Si se configura en \c NO, estas declaraciones se incluiran en la documentacion.</translation>
    </message>
<message>
        <source>HIDE_IN_BODY_DOCS</source>
            <translation>Si la etiqueta \c HIDE_IN_BODY_DOCS esta configurada en \c YES, Doxygen ocultara los bloques de documentacion encontrados dentro del cuerpo de una funcion. Si se configura en \c NO, estos bloques se agregaran al bloque de documentacion detallado de la funcion.</translation>
    </message>
<message>
        <source>HIDE_SCOPE_NAMES</source>
            <translation>Si la etiqueta \c HIDE_SCOPE_NAMES esta configurada en \c NO, Doxygen mostrara miembros con sus ambitos de clase y espacio de nombres completos en la documentacion. Si se configura en \c YES, el ambito se ocultara.</translation>
    </message>
<message>
        <source>HIDE_UNDOC_CLASSES</source>
            <translation>Si la etiqueta \c HIDE_UNDOC_CLASSES esta configurada en \c YES, Doxygen ocultara todas las clases no documentadas que normalmente serian visibles en la jerarquia de clases. Si se configura en \c NO, estas clases se incluiran en las varias descripciones generales. Esta opcion tambien oculta los conceptos de C++ no documentados si esta habilitada. Esta opcion no tiene efecto si \ref cfg_extract_all "EXTRACT_ALL" esta habilitada.</translation>
    </message>
<message>
        <source>HIDE_UNDOC_MEMBERS</source>
            <translation>Si la etiqueta \c HIDE_UNDOC_MEMBERS esta configurada en \c YES, Doxygen ocultara todos los miembros no documentados dentro de clases o archivos documentados. Si se configura en \c NO, estos miembros se incluiran en las varias descripciones generales, pero no se generara ninguna seccion de documentacion. Esta opcion no tiene efecto si \ref cfg_extract_all "EXTRACT_ALL" esta habilitada.</translation>
    </message>
<message>
        <source>HIDE_UNDOC_NAMESPACES</source>
            <translation>Si la etiqueta \c HIDE_UNDOC_NAMESPACES esta configurada en \c YES, Doxygen ocultara todos los espacios de nombres no documentados que normalmente serian visibles en la jerarquia de espacios de nombres. Si se configura en \c NO, estos espacios de nombres se incluiran en las varias descripciones generales. Esta opcion no tiene efecto si \ref cfg_extract_all "EXTRACT_ALL" esta habilitada.</translation>
    </message>
<message>
        <source>HIDE_UNDOC_RELATIONS</source>
            <translation>Si se configura en \c YES, los grafos de herencia y colaboracion ocultaran relaciones de herencia y uso cuando el objetivo no este documentado o no sea una clase.</translation>
    </message>
<message>
        <source>HTML_CODE_FOLDING</source>
            <translation>Si la etiqueta \c HTML_CODE_FOLDING esta configurada en \c YES, las clases y funciones en el codigo fuente HTML generado se pueden colapsar y expandir dinamicamente.</translation>
    </message>
<message>
        <source>HTML_COLORSTYLE</source>
            <translation>La etiqueta \c HTML_COLORSTYLE se puede usar para especificar si la salida HTML generada se renderiza con un tema oscuro o claro.</translation>
    </message>
<message>
        <source>HTML_COLORSTYLE_GAMMA</source>
            <translation>La etiqueta \c HTML_COLORSTYLE_GAMMA controla la correccion gamma aplicada al componente de luminancia de los colores en la salida HTML. Valores por debajo de 100 hacen que la salida sea gradualmente mas clara, mientras que valores por encima de 100 hacen que la salida sea mas oscura. El valor dividido por 100 es la gamma realmente aplicada, por lo que 80 representa una gamma de 0.8, el valor 220 representa una gamma de 2.2, y 100 no cambia la gamma.</translation>
    </message>
<message>
        <source>HTML_COLORSTYLE_HUE</source>
            <translation>La etiqueta \c HTML_COLORSTYLE_HUE controla el color de la salida HTML. Doxygen ajustara los colores en la hoja de estilo y las imagenes de fondo de acuerdo con este color. El tono se especifica como un angulo en una rueda de color, vea https://en.wikipedia.org/wiki/Hue para mas informacion. Por ejemplo, el valor 0 representa rojo, 60 es amarillo, 120 es verde, 180 es cian, 240 es azul, 300 es purpura y 360 es rojo de nuevo.</translation>
    </message>
<message>
        <source>HTML_COLORSTYLE_SAT</source>
            <translation>La etiqueta \c HTML_COLORSTYLE_SAT controla la pureza (o saturacion) de los colores en la salida HTML. Para un valor de 0, la salida usa solo escalas de grises. Un valor de 255 producira los colores mas vivos.</translation>
    </message>
<message>
        <source>HTML_COPY_CLIPBOARD</source>
            <translation>Si la etiqueta \c HTML_COPY_CLIPBOARD esta configurada en \c YES, Doxygen mostrara un icono en la esquina superior derecha de los fragmentos de codigo y texto que permite al usuario copiar el contenido al portapapeles. Tenga en cuenta que esto solo funciona si el navegador lo soporta y la pagina web se sirve a traves de un &lt;a href="https://www.w3.org/TR/secure-contexts/"&gt;contexto seguro&lt;/a&gt;, es decir, usando el protocolo https: o file:.</translation>
    </message>
<message>
        <source>HTML_DYNAMIC_MENUS</source>
            <translation>Si la etiqueta \c HTML_DYNAMIC_MENUS esta configurada en \c YES, la documentacion HTML generada incluira un indice principal con menus de navegacion verticales creados dinamicamente a traves de JavaScript. Si esta deshabilitado, el indice de navegacion consiste en multiples niveles de pestanas que se incrustan estaticamente en cada pagina HTML. Deshabilite esta opcion para soportar navegadores que no tienen JavaScript, como el navegador de ayuda Qt.</translation>
    </message>
<message>
        <source>HTML_DYNAMIC_SECTIONS</source>
            <translation>Si la etiqueta \c HTML_DYNAMIC_SECTIONS esta configurada en \c YES, la documentacion HTML generada contendra secciones que se pueden mostrar y ocultar despues de que se cargue la pagina.</translation>
    </message>
<message>
        <source>HTML_EXTRA_FILES</source>
            <translation>La etiqueta \c HTML_EXTRA_FILES se puede usar para especificar una o mas imagenes adicionales u otros archivos de origen que deben copiarse en el directorio de salida HTML. Tenga en cuenta que estos archivos se copiaran en el directorio de salida HTML base. Use el marcador &lt;code&gt;$relpath^&lt;/code&gt; en los archivos \ref cfg_html_header "HTML_HEADER" y/o \ref cfg_html_footer "HTML_FOOTER" para cargar estos archivos. En el archivo \ref cfg_html_stylesheet "HTML_STYLESHEET" use solo el nombre del archivo. Tenga en cuenta tambien que los archivos se copian tal como estan; no hay comandos o marcadores disponibles.</translation>
    </message>
<message>
        <source>HTML_EXTRA_STYLESHEET</source>
            <translation>La etiqueta \c HTML_EXTRA_STYLESHEET se puede usar para especificar hojas de estilo en cascada personalizadas adicionales que se incluiran despues de las hojas de estilo estandar creadas por Doxygen. Con esta opcion puede anular ciertos aspectos de estilo. Esto es preferible al uso de \ref cfg_html_stylesheet "HTML_STYLESHEET" ya que no reemplaza la hoja de estilo estandar y por lo tanto es mas robusto frente a actualizaciones futuras. Doxygen copiara los archivos de hoja de estilo en el directorio de salida. \nota El orden de los archivos de hoja de estilo adicionales es importante (por ejemplo, la ultima hoja de estilo en la lista anula la configuracion de las anteriores en la lista). \nota Dado que el estilo de las barras de desplazamiento en webkit/Chromium actualmente no se puede anular, el estilo del archivo estandar doxygen.css se omite cuando se especifican una o mas hojas de estilo adicionales. Entonces, si se desea personalizar las barras de desplazamiento, debe agregarse explícitamente. Para un ejemplo, vea la documentacion.</translation>
    </message>
<message>
        <source>HTML_FILE_EXTENSION</source>
            <translation>La etiqueta \c HTML_FILE_EXTENSION se puede usar para especificar la extension de archivo para cada pagina HTML generada (por ejemplo: &lt;code&gt;.htm, .php, .asp&lt;/code&gt;).</translation>
    </message>
<message>
        <source>HTML_FOOTER</source>
            <translation>La etiqueta \c HTML_FOOTER se puede usar para especificar un pie de pagina HTML personalizado para cada pagina HTML generada. Si la etiqueta se deja vacia, Doxygen generara un pie de pagina estandar. Vea \ref cfg_html_header "HTML_HEADER" para mas informacion sobre como generar un pie de pagina estandar y que comandos especiales se pueden usar dentro del pie de pagina. Vea tambien la seccion \ref doxygen_usage para informacion sobre como generar el pie de pagina estandar que Doxygen normalmente usa.</translation>
    </message>
<message>
        <source>HTML_FORMULA_FORMAT</source>
            <translation>Si la opcion \c HTML_FORMULA_FORMAT esta configurada en \c svg, Doxygen usara la herramienta pdf2svg (vea https://github.com/dawbarton/pdf2svg) o Inkscape (vea https://inkscape.org) para generar formulas como imagenes SVG en lugar de PNGs para la salida HTML. Estas imagenes generalmente se ven mejor en resoluciones escaladas.</translation>
    </message>
<message>
        <source>HTML_HEADER</source>
            <translation>La etiqueta \c HTML_HEADER se puede usar para especificar un archivo de encabezado HTML personalizado para cada pagina HTML generada. Si la etiqueta se deja vacia, Doxygen generara un encabezado estandar. &lt;br&gt;Para obtener HTML valido, el archivo de encabezado debe contener todos los scripts y hojas de estilo que Doxygen necesita, lo cual depende de las opciones de configuracion usadas (por ejemplo, la configuracion \ref cfg_generate_treeview "GENERATE_TREEVIEW"). Se recomienda encarecidamente comenzar con un encabezado estandar usando \verbatim doxygen -w html new_header.html new_footer.html new_stylesheet.css YourConfigFile \endverbatim y luego modificar el archivo \c new_header.html. Vea tambien la seccion \ref doxygen_usage para informacion sobre como generar el encabezado estandar que Doxygen normalmente usa. @note El encabezado puede cambiar, por lo que generalmente necesita regenerar el encabezado estandar cuando actualiza a una version mas nueva de Doxygen. Para una descripcion de los posibles marcadores y nombres de bloques, vea la documentacion.</translation>
    </message>
<message>
        <source>HTML_INDEX_NUM_ENTRIES</source>
            <translation>Con \c HTML_INDEX_NUM_ENTRIES se puede controlar el numero preferido de entradas que se muestran inicialmente en los varios indices en forma de arbol; el usuario puede expandir y colapsar entradas dinamicamente mas tarde. Doxygen expandira el arbol a un nivel tal que a lo sumo el numero especificado de entradas sea visible (a menos que un arbol completamente colapsado ya exceda esta cantidad). Configurar el numero de entradas en 1 produce un arbol completamente colapsado por defecto. 0 es un valor especial que representa un numero infinito de entradas y resultara en un arbol completamente expandido por defecto.</translation>
    </message>
<message>
        <source>HTML_OUTPUT</source>
            <translation>La etiqueta \c HTML_OUTPUT se usa para especificar donde se colocaran los documentos HTML. Si se ingresa una ruta relativa, el valor de \ref cfg_output_directory "OUTPUT_DIRECTORY" se antepondra.</translation>
    </message>
<message>
        <source>HTML_PROJECT_COOKIE</source>
            <translation>Doxygen almacena algunas configuraciones permanentemente en el navegador (por ejemplo, a traves de cookies). Por defecto, estas configuraciones se aplican a todas las paginas HTML generadas por Doxygen en todos los proyectos. La etiqueta \c HTML_PROJECT_COOKIE se puede usar para almacenar las configuraciones bajo una clave especifica del proyecto, de modo que la configuracion del usuario se guarde por separado.</translation>
    </message>
<message>
        <source>HTML_STYLESHEET</source>
            <translation>La etiqueta \c HTML_STYLESHEET se puede usar para especificar una hoja de estilo en cascada personalizada que es usada por cada pagina HTML. Se puede usar para ajustar la apariencia de la salida HTML. Si se deja vacio, Doxygen generara una hoja de estilo estandar. Vea tambien la seccion \ref doxygen_usage para informacion sobre como generar la hoja de estilo que Doxygen normalmente usa. \nota Se recomienda usar \ref cfg_html_extra_stylesheet "HTML_EXTRA_STYLESHEET" en lugar de esta etiqueta, ya que es mas robusto y esta etiqueta (&lt;code&gt;HTML_STYLESHEET&lt;/code&gt;) quedara obsoleta en el futuro.</translation>
    </message>
<message>
        <source>IDL_PROPERTY_SUPPORT</source>
            <translation>Para el IDL de Microsoft, existen los atributos \c propget y \c propput para indicar metodos getter y setter para una propiedad. Configurar esta opcion en \c YES hara que Doxygen reemplace los metodos get y set en la documentacion con la propiedad. Esto solo funciona si los metodos realmente obtienen o establecen un tipo simple. Si este no es el caso o si desea mostrar los metodos de todos modos, debe configurar esta opcion en \c NO.</translation>
    </message>
<message>
        <source>IGNORE_PREFIX</source>
            <translation>La etiqueta \c IGNORE_PREFIX se puede usar para especificar uno o mas prefijos que deben ignorarse al generar los encabezados de indice. La etiqueta \c IGNORE_PREFIX funciona para nombres de clases, funciones y miembros. La entidad se colocara en la lista alfabetica bajo la primera letra del nombre de la entidad que queda despues de eliminar el prefijo.</translation>
    </message>
<message>
        <source>IMAGE_PATH</source>
            <translation>La etiqueta \c IMAGE_PATH se puede usar para especificar uno o mas directorios que contienen imagenes que se incluyen en la documentacion (vea el comando \ref cmdimage "\\image").</translation>
    </message>
<message>
        <source>IMPLICIT_DIR_DOCS</source>
            <translation>Si la etiqueta \c IMPLICIT_DIR_DOCS esta configurada en \c YES, cualquier archivo `README.md` encontrado en los subdirectorios de la raiz del proyecto se usara como documentacion para ese subdirectorio, a menos que el `README.md` comience con un comando \ref cmddir "\\dir", \ref cmdpage "\\page" o \ref cmdmainpage "\\mainpage". Si se configura en \c NO, el archivo `README.md` debe comenzar con un comando \ref cmddir "\\dir" explicito para ser usado como documentacion del directorio.</translation>
    </message>
<message>
        <source>INCLUDED_BY_GRAPH</source>
            <translation>Si las etiquetas \c INCLUDED_BY_GRAPH, \ref cfg_enable_preprocessing "ENABLE_PREPROCESSING" y \ref cfg_search_includes "SEARCH_INCLUDES" estan configuradas en \c YES, Doxygen generara un grafo para cada archivo documentado que muestre las dependencias de inclusion directas e indirectas del archivo con otros archivos documentados. La activacion explicita de un grafo de included-by cuando \c INCLUDED_BY_GRAPH esta configurado en \c NO se puede lograr con el comando \ref cmdincludedbygraph "\\includedbygraph". La desactivacion de un grafo de included-by se puede lograr con el comando \ref cmdhideincludedbygraph "\\hideincludedbygraph".</translation>
    </message>
<message>
        <source>INCLUDE_FILE_PATTERNS</source>
            <translation>Puede usar la etiqueta \c INCLUDE_FILE_PATTERNS para especificar uno o mas patrones de comodin (como `*.h` y `*.hpp`) para filtrar los archivos de encabezado en los directorios. Si se deja vacio, se usaran los patrones especificados con \ref cfg_file_patterns "FILE_PATTERNS".</translation>
    </message>
<message>
        <source>INCLUDE_GRAPH</source>
            <translation>Si la etiqueta \c INCLUDE_GRAPH esta configurada en \c YES, Doxygen generara un grafo para cada archivo documentado que muestre las dependencias de inclusion directas e indirectas del archivo con otros archivos documentados. La activacion explicita de un grafo de inclusion cuando \c INCLUDE_GRAPH esta configurado en \c NO se puede lograr con el comando \ref cmdincludegraph "\\includegraph". La desactivacion de un grafo de inclusion se puede lograr con el comando \ref cmdhideincludegraph "\\hideincludegraph".</translation>
    </message>
<message>
        <source>INCLUDE_PATH</source>
            <translation>La etiqueta \c INCLUDE_PATH se puede usar para especificar uno o mas directorios que contienen archivos de inclusion que no son archivos de entrada pero que deben ser procesados por el preprocesador. Tenga en cuenta que \c INCLUDE_PATH no es recursivo, por lo que configurar \ref cfg_recursive "RECURSIVE" aqui no tendra efecto.</translation>
    </message>
<message>
        <source>INHERIT_DOCS</source>
            <translation>Si la etiqueta \c INHERIT_DOCS esta configurada en \c YES, un miembro no documentado heredara la documentacion de cualquier miembro documentado que reimplemente.</translation>
    </message>
<message>
        <source>INLINE_GROUPED_CLASSES</source>
            <translation>Si la etiqueta \c INLINE_GROUPED_CLASSES esta configurada en \c YES, las clases, estructuras y uniones se mostraran dentro del grupo en el que estan contenidas (por ejemplo, con \ref cmdingroup "\\ingroup") en lugar de en una pagina separada (para HTML y paginas de manual) o en una seccion (para \f$\mbox{\LaTeX}\f$ y RTF). &lt;br&gt;Tenga en cuenta que esta funcion no funciona en combinacion con \ref cfg_separate_member_pages "SEPARATE_MEMBER_PAGES".</translation>
    </message>
<message>
        <source>INLINE_INFO</source>
            <translation>Si la etiqueta \c INLINE_INFO esta configurada en \c YES, se insertara una etiqueta [inline] en la documentacion para miembros inline.</translation>
    </message>
<message>
        <source>INLINE_INHERITED_MEMB</source>
            <translation>Si la etiqueta \c INLINE_INHERITED_MEMB esta configurada en \c YES, Doxygen mostrara todos los miembros heredados de una clase en la documentacion de esa clase como si esos miembros fueran miembros normales de la clase. Los constructores, destructores y operadores de asignacion de las clases base no se mostraran.</translation>
    </message>
<message>
        <source>INLINE_SIMPLE_STRUCTS</source>
            <translation>Si la etiqueta \c INLINE_SIMPLE_STRUCTS esta configurada en \c YES, las estructuras, clases y uniones con solo campos de datos publicos o campos typedef simples se mostraran en linea en la documentacion del ambito en el que estan definidas (es decir, documentacion de archivo, espacio de nombres o grupo), siempre que dicho ambito este documentado. Si se configura en \c NO, las estructuras, clases y uniones se mostraran en una pagina separada (para HTML y paginas de manual) o en una seccion (para \f$\mbox{\LaTeX}\f$ y RTF).</translation>
    </message>
<message>
        <source>INLINE_SOURCES</source>
            <translation>Si la etiqueta \c INLINE_SOURCES esta configurada en \c YES, el cuerpo de funciones, clases y espacios de nombres se insertara directamente en la documentacion.</translation>
    </message>
<message>
        <source>INPUT</source>
            <translation>La etiqueta \c INPUT se usa para especificar los archivos y/o directorios que contienen los archivos de codigo fuente y otros archivos de documentacion. Puede ingresar nombres de archivo como \c myfile.cpp o directorios como \c /usr/src/myproject. Separe los archivos o directorios con espacios. Vea tambien \ref cfg_file_patterns "FILE_PATTERNS" y \ref cfg_extension_mapping "EXTENSION_MAPPING". \nota Si esta etiqueta esta vacia, se buscara en el directorio actual.</translation>
    </message>
<message>
        <source>INPUT_ENCODING</source>
            <translation>Esta etiqueta se puede usar para especificar la codificacion de caracteres de los archivos fuente que Doxygen analiza. Internamente, Doxygen usa la codificacion UTF-8. Doxygen usa `libiconv` (o el `iconv` incorporado en `libc`) para la transcodificacion. Vea &lt;a href="https://www.gnu.org/software/libiconv/"&gt;la documentacion de libiconv&lt;/a&gt; para la lista de codificaciones posibles. \sa \ref cfg_input_file_encoding "INPUT_FILE_ENCODING"</translation>
    </message>
<message>
        <source>INPUT_FILE_ENCODING</source>
            <translation>Esta etiqueta se puede usar para especificar la codificacion de caracteres de los archivos fuente que Doxygen analiza. La etiqueta \c INPUT_FILE_ENCODING se puede usar para especificar la codificacion de caracteres por patron de archivo. Doxygen comparara el nombre del archivo con cada patron y aplicara la codificacion en lugar de la \ref cfg_input_encoding "INPUT_ENCODING" predeterminada cuando haya una coincidencia. Las codificaciones de caracteres son una lista de la forma: patron=codificacion (como `*.php=ISO-8859-1`). \sa \ref cfg_input_encoding "INPUT_ENCODING" para mas informacion sobre codificaciones soportadas.</translation>
    </message>
<message>
        <source>INPUT_FILTER</source>
            <translation>La etiqueta \c INPUT_FILTER se puede usar para especificar un programa que Doxygen debe invocar para cada archivo de entrada. Doxygen invocara el programa de filtro ejecutando (via &lt;code&gt;popen()&lt;/code&gt;) el comando: &lt;br&gt; &lt;code&gt;\&lt;filtro\&gt; \&lt;archivo-entrada\&gt;&lt;/code&gt; &lt;br&gt; donde &lt;code&gt;\&lt;filtro\&gt;&lt;/code&gt; es el valor de la etiqueta \c INPUT_FILTER y &lt;code&gt;\&lt;archivo-entrada\&gt;&lt;/code&gt; es el nombre de un archivo de entrada. Doxygen usara entonces la salida que el programa de filtro escriba en la salida estandar. Si \ref cfg_filter_patterns "FILTER_PATTERNS" esta especificado, esta etiqueta se ignorara. &lt;br&gt;Tenga en cuenta que el filtro no debe agregar ni eliminar lineas; se aplica antes de que se escanee el codigo, pero no cuando se genera el codigo de salida. Si se agregan o eliminan lineas, los anclajes no se colocaran correctamente. &lt;br&gt;Tenga en cuenta que Doxygen usara los datos procesados y escritos en la salida estandar para su posterior procesamiento, por lo que no se debe escribir nada mas, como declaraciones de depuracion o comandos usados (así que en el caso de un archivo por lotes de Windows, siempre use `@echo OFF`). &lt;br&gt;Tenga en cuenta que para extensiones personalizadas o extensiones no soportadas directamente, tambien debe configurar \ref cfg_extension_mapping "EXTENSION_MAPPING" para la extension, de lo contrario los archivos no seran procesados correctamente por Doxygen.&lt;br&gt;</translation>
    </message>
<message>
        <source>INTERACTIVE_SVG</source>
            <translation>Si \ref cfg_dot_image_format "DOT_IMAGE_FORMAT" esta configurado en `svg` o `svg:svg` o `svg:svg:core`, esta opcion se puede configurar en \c YES para habilitar la generacion de imagenes SVG interactivas que permiten zoom y desplazamiento. &lt;br&gt;Tenga en cuenta que esto requiere un navegador moderno que no sea Internet Explorer. Los navegadores probados y funcionales son Firefox, Chrome, Safari y Opera. &lt;br&gt;Tenga en cuenta que esta opcion se deshabilita automaticamente cuando \ref cfg_dot_image_format "DOT_IMAGE_FORMAT" esta configurado en `svg:cairo` o `svg:cairo:cairo`.</translation>
    </message>
<message>
        <source>INTERNAL_DOCS</source>
            <translation>La etiqueta \c INTERNAL_DOCS determina si la documentacion escrita despues de un comando \ref cmdinternal "\\internal" se incluye. Si la etiqueta esta configurada en \c NO, la documentacion se excluye. Configúrela en \c YES para incluir la documentacion interna.</translation>
    </message>
<message>
        <source>JAVADOC_AUTOBRIEF</source>
            <translation>Si la etiqueta \c JAVADOC_AUTOBRIEF esta configurada en \c YES, Doxygen interpretara la primera linea (hasta el primer punto, signo de interrogacion o signo de exclamacion) de un comentario estilo Javadoc como la descripcion breve. Si se configura en \c NO, el estilo Javadoc se comportara como comentarios de estilo Qt normales (por lo que se requiere un comando \ref cmdbrief "\@brief" explícito para una descripcion breve).</translation>
    </message>
<message>
        <source>JAVADOC_BANNER</source>
            <translation>Si la etiqueta \c JAVADOC_BANNER esta configurada en \c YES, Doxygen interpretara una linea como \verbatim /***************\endverbatim como el comienzo de un banner de comentario estilo Javadoc. Si se configura en \c NO, el estilo Javadoc se comportara como comentarios normales y no sera interpretado por Doxygen.</translation>
    </message>
<message>
        <source>LATEX_BATCHMODE</source>
            <translation>Si la etiqueta \c LATEX_BATCHMODE esta configurada en \c YES, Doxygen ejecutara el compilador \f$\mbox{\LaTeX}\f$ en modo por lotes, lo que significa que no se detendra en errores.</translation>
    </message>
<message>
        <source>LATEX_BIB_STYLE</source>
            <translation>La etiqueta \c LATEX_BIB_STYLE se puede usar para especificar el estilo de bibliografia que usa la herramienta \c bibtex. Por defecto, esto esta configurado en `plainnat`.</translation>
    </message>
<message>
        <source>LATEX_CMD_NAME</source>
            <translation>La etiqueta \c LATEX_CMD_NAME se puede usar para especificar el nombre del comando \f$\mbox{\LaTeX}\f$ a invocar. &lt;br&gt;Tenga en cuenta que cuando no se habilita \ref cfg_use_pdflatex "USE_PDFLATEX" el valor predeterminado es \c latex, cuando se habilita \ref cfg_use_pdflatex "USE_PDFLATEX" el valor predeterminado es \c pdflatex y cuando en este ultimo caso se elige \c latex, esto se sobrescribe por \c pdflatex. Para idiomas de salida especificos, el valor predeterminado puede haberse configurado de manera diferente, esto depende de la implementacion del idioma de salida.</translation>
    </message>
<message>
        <source>LATEX_EMOJI_DIRECTORY</source>
            <translation>La etiqueta \c LATEX_EMOJI_DIRECTORY se usa para especificar la ruta desde donde se leen las imagenes de emoji. Si se deja vacio, se usara el directorio de emoji predeterminado.</translation>
    </message>
<message>
        <source>LATEX_EXTRA_FILES</source>
            <translation>La etiqueta \c LATEX_EXTRA_FILES se puede usar para especificar una o mas imagenes adicionales u otros archivos de origen que deben copiarse en el directorio de salida \ref cfg_latex_output "LATEX_OUTPUT". Tenga en cuenta que los archivos se copian tal como estan; no hay comandos o marcadores disponibles.</translation>
    </message>
<message>
        <source>LATEX_EXTRA_STYLESHEET</source>
            <translation>La etiqueta \c LATEX_EXTRA_STYLESHEET se puede usar para especificar hojas de estilo \f$\mbox{\LaTeX}\f$ personalizadas adicionales que se incluiran despues de las hojas de estilo estandar creadas por Doxygen. Con esta opcion puede anular ciertos aspectos de estilo. Doxygen copiara los archivos de hoja de estilo en el directorio de salida. \nota El orden de los archivos de hoja de estilo adicionales es importante (por ejemplo, la ultima hoja de estilo en la lista anula la configuracion de las anteriores en la lista).</translation>
    </message>
<message>
        <source>LATEX_FOOTER</source>
            <translation>La etiqueta \c LATEX_FOOTER se puede usar para especificar un pie de pagina \f$\mbox{\LaTeX}\f$ personalizado para el documento \f$\mbox{\LaTeX}\f$ generado. El pie de pagina debe contener todo despues del ultimo capitulo. Si se deja vacio, Doxygen generara un pie de pagina estandar. Vea \ref cfg_latex_header "LATEX_HEADER" para mas informacion sobre como generar un pie de pagina estandar y que comandos especiales se pueden usar en el pie de pagina. Vea tambien la seccion \ref doxygen_usage para informacion sobre como generar el pie de pagina estandar que Doxygen normalmente usa. Nota: ¡Solo use un pie de pagina personalizado si sabe lo que esta haciendo!</translation>
    </message>
<message>
        <source>LATEX_HEADER</source>
            <translation>La etiqueta \c LATEX_HEADER se puede usar para especificar un encabezado \f$\mbox{\LaTeX}\f$ personalizado para el documento \f$\mbox{\LaTeX}\f$ generado. El encabezado debe contener todo hasta el primer capitulo. Si se deja vacio, Doxygen generara un encabezado estandar. Se recomienda encarecidamente comenzar con un encabezado estandar usando \verbatim doxygen -w latex new_header.tex new_footer.tex new_stylesheet.sty \endverbatim y luego modificar el archivo \c new_header.tex. Vea tambien la seccion \ref doxygen_usage para informacion sobre como generar el encabezado estandar que Doxygen normalmente usa. &lt;br&gt;Nota: ¡Solo use un encabezado personalizado si sabe lo que esta haciendo! @note El encabezado puede cambiar, por lo que generalmente necesita regenerar el encabezado estandar cuando actualiza a una version mas nueva de Doxygen. Para una descripcion de los posibles marcadores y nombres de bloques, vea la documentacion.</translation>
    </message>
<message>
        <source>LATEX_HIDE_INDICES</source>
            <translation>Si la etiqueta \c LATEX_HIDE_INDICES esta configurada en \c YES, Doxygen no incluira los capitulos de indice (como Indice de Archivos, Indice de Compuestos, etc.) en la salida.</translation>
    </message>
<message>
        <source>LATEX_MAKEINDEX_CMD</source>
            <translation>La etiqueta \c LATEX_MAKEINDEX_CMD se puede usar para especificar el nombre del comando para generar el indice para \f$\mbox{\LaTeX}\f$. Si no hay una barra invertida (`\`) como primer caracter, se agregara automaticamente en el codigo \f$\mbox{\LaTeX}\f$. @note Esta etiqueta se usa en el archivo de salida generado (`.tex`). \sa \ref cfg_makeindex_cmd_name "MAKEINDEX_CMD_NAME" para la parte en el `Makefile` / `make.bat`.</translation>
    </message>
<message>
        <source>LATEX_OUTPUT</source>
            <translation>La etiqueta \c LATEX_OUTPUT se usa para especificar donde se colocaran los documentos \f$\mbox{\LaTeX}\f$. Si se ingresa una ruta relativa, el valor de \ref cfg_output_directory "OUTPUT_DIRECTORY" se antepondra.</translation>
    </message>
<message>
        <source>LAYOUT_FILE</source>
            <translation>La etiqueta \c LAYOUT_FILE se puede usar para especificar un archivo de diseño que Doxygen analizara. El archivo de diseño controla la estructura global de los archivos de salida generados de una manera independiente del formato de salida. Para crear el archivo de diseño que representa los valores predeterminados de Doxygen, ejecute Doxygen con la opcion `-l`. Puede especificar opcionalmente un nombre de archivo despues de la opcion, si se omite, se usara \c DoxygenLayout.xml como nombre del archivo de diseño. Vea tambien la seccion \ref layout para informacion. &lt;br&gt;Tenga en cuenta que Doxygen analizara automaticamente un archivo llamado \c DoxygenLayout.xml si ejecuta Doxygen desde un directorio que contiene ese archivo, incluso si la etiqueta \c LAYOUT_FILE se deja vacia.</translation>
    </message>
<message>
        <source>LOOKUP_CACHE_SIZE</source>
            <translation>El tamano del cache de busqueda de simbolos se puede configurar con \c LOOKUP_CACHE_SIZE. Este cache se usa para resolver simbolos por su nombre y alcance. Dado que este puede ser un proceso costoso y el mismo simbolo aparece a menudo multiples veces en el codigo, Doxygen mantiene un cache de simbolos pre-resueltos. Si el cache es demasiado pequeno, Doxygen sera mas lento. Si el cache es demasiado grande, se desperdicia memoria. El tamano del cache viene dado por esta formula: \f$2^{(16+\mbox{LOOKUP\_CACHE\_SIZE})}\f$. El rango valido es 0..9, el predeterminado es 0, que corresponde a un tamano de cache de \f$2^{16} = 65536\f$ simbolos. Al final de una ejecucion, Doxygen informara el uso del cache y sugerira el tamano de cache optimo desde el punto de vista de la velocidad.</translation>
    </message>
<message>
        <source>MACRO_EXPANSION</source>
            <translation>Si la etiqueta \c MACRO_EXPANSION esta configurada en \c YES, Doxygen expandira todos los nombres de macros en el codigo fuente. Si se configura en \c NO, solo se realizara la compilacion condicional. La expansion de macros se puede controlar configurando \ref cfg_expand_only_predef "EXPAND_ONLY_PREDEF" en \c YES.</translation>
    </message>
<message>
        <source>MAKEINDEX_CMD_NAME</source>
            <translation>La etiqueta \c MAKEINDEX_CMD_NAME se puede usar para especificar el nombre del comando para generar el indice para \f$\mbox{\LaTeX}\f$. @note Esta etiqueta se usa en el `Makefile` / `make.bat`. \sa \ref cfg_latex_makeindex_cmd "LATEX_MAKEINDEX_CMD" para la parte en el archivo de salida generado (`.tex`).</translation>
    </message>
<message>
        <source>MAN_EXTENSION</source>
            <translation>La etiqueta \c MAN_EXTENSION determina la extension que se agrega a las paginas de manual generadas. Si la seccion del manual no comienza con un numero, se antepondra el numero 3. El punto (.) al comienzo de la etiqueta \c MAN_EXTENSION es opcional.</translation>
    </message>
<message>
        <source>MAN_LINKS</source>
            <translation>Si la etiqueta \c MAN_LINKS esta configurada en \c YES y Doxygen genera salida de manual, generara un archivo de manual adicional para cada entidad documentada en las paginas de manual reales. Estos archivos adicionales solo apuntan a la pagina de manual real, pero sin ellos, el comando \c man no podria encontrar la pagina correcta.</translation>
    </message>
<message>
        <source>MAN_OUTPUT</source>
            <translation>La etiqueta \c MAN_OUTPUT se usa para especificar donde se colocaran las paginas de manual. Si se ingresa una ruta relativa, el valor de \ref cfg_output_directory "OUTPUT_DIRECTORY" se antepondra. Se creara un directorio \c man3 dentro del directorio especificado por \c MAN_OUTPUT.</translation>
    </message>
<message>
        <source>MAN_SUBDIR</source>
            <translation>La etiqueta \c MAN_SUBDIR determina el nombre del directorio creado dentro de \c MAN_OUTPUT donde se colocan las paginas de manual. Por defecto, es man seguido de \c MAN_EXTENSION sin el `.` inicial.</translation>
    </message>
<message>
        <source>MARKDOWN_ID_STYLE</source>
            <translation>La etiqueta \c MARKDOWN_ID_STYLE se puede usar para especificar el algoritmo utilizado para generar identificadores para los encabezados de Markdown. Nota: Cada identificador es unico.</translation>
    </message>
<message>
        <source>MARKDOWN_STRICT</source>
            <translation>Si la etiqueta \c MARKDOWN_STRICT esta habilitada, Doxygen tratara el texto en los comentarios como Markdown formateado tambien en casos donde el formato de marcado nativo de Doxygen entra en conflicto con el de Markdown. Esto solo es relevante en casos donde se usan acentos graves. El estilo de marcado nativo de Doxygen permite que una comilla simple termine un fragmento de texto que comenzo con un acento grave y luego tratarlo como un fragmento de texto citado, mientras que en Markdown tal fragmento de texto se trata como literal y solo termina cuando se encuentra un segundo acento grave coincidente. Ademas, el formato de marcado nativo de Doxygen requiere que las comillas dobles se escapen cuando aparecen en una seccion de acentos graves, mientras que esto no es necesario para Markdown.</translation>
    </message>
<message>
        <source>MARKDOWN_SUPPORT</source>
            <translation>Si la etiqueta \c MARKDOWN_SUPPORT esta habilitada, Doxygen preprocesara todos los comentarios de acuerdo con el formato Markdown, lo que permite una documentacion mas legible. Vea https://daringfireball.net/projects/markdown/ para detalles. La salida del procesamiento de Markdown es procesada posteriormente por Doxygen, por lo que puede mezclar comandos de Doxygen, HTML y XML con formato Markdown. Deshabilite esto solo si tiene problemas con la compatibilidad hacia atras.</translation>
    </message>
<message>
        <source>MATHJAX_CODEFILE</source>
            <translation>La etiqueta \c MATHJAX_CODEFILE se puede usar para especificar un archivo con fragmentos de codigo JavaScript que se usaran cuando se inicie el codigo MathJax. Vea el sitio web de MathJax para mas detalles: - &lt;a href="https://docs.mathjax.org/en/v2.7/"&gt;MathJax version 2&lt;/a&gt; - &lt;a href="https://docs.mathjax.org/en/v3.2/"&gt;MathJax version 3&lt;/a&gt; - &lt;a href="https://docs.mathjax.org/en/v4.0/"&gt;MathJax version 4&lt;/a&gt; Para un ejemplo, vea la documentacion.</translation>
    </message>
<message>
        <source>MATHJAX_EXTENSIONS</source>
            <translation>La etiqueta \c MATHJAX_EXTENSIONS se puede usar para especificar uno o mas nombres de extensiones MathJax que deben activarse durante el renderizado de MathJax. Por ejemplo para MathJax Version 2 (vea https://docs.mathjax.org/en/v2.7/tex.html): \verbatim MATHJAX_EXTENSIONS = TeX/AMSmath TeX/AMSsymbols \endverbatim Por ejemplo para MathJax Version 3 (vea https://docs.mathjax.org/en/v3.2/input/tex/extensions/): \verbatim MATHJAX_EXTENSIONS = ams \endverbatim Por ejemplo para MathJax Version 4 (vea https://docs.mathjax.org/en/v4.0/input/tex/extensions/): \verbatim MATHJAX_EXTENSIONS = units \endverbatim Tenga en cuenta que para MathJax Version 4 ya se cargan automaticamente algunas extensiones. Para deshabilitar un paquete en MathJax Version 4, puede usar el nombre del paquete con un signo menos delante (`-` como `MATHJAX_EXTENSIONS += -textmacros`)</translation>
    </message>
<message>
        <source>MATHJAX_FORMAT</source>
            <translation>Si MathJax esta habilitado, puede establecer el formato de salida predeterminado que se usara para la salida de MathJax. Para mas detalles sobre el formato de salida, vea &lt;a href="https://docs.mathjax.org/en/v2.7/output.html"&gt;MathJax version 2&lt;/a&gt;, &lt;a href="https://docs.mathjax.org/en/v3.2/output/index.html"&gt;MathJax version 3&lt;/a&gt; y &lt;a href="https://docs.mathjax.org/en/v4.0/output/index.htm"&gt;MathJax version 4&lt;/a&gt;.</translation>
    </message>
<message>
        <source>MATHJAX_RELPATH</source>
            <translation>Si MathJax esta habilitado, debe especificar la ubicacion relativa al directorio de salida HTML con la opcion \c MATHJAX_RELPATH. Para MathJax Version 2, el directorio de destino debe contener el script `MathJax.js`. Si, por ejemplo, el directorio \c mathjax esta en el mismo nivel que el directorio de salida HTML, entonces \c MATHJAX_RELPATH debe ser &lt;code&gt;../mathjax&lt;/code&gt;. Para MathJax Versiones 3 y 4, el directorio de destino debe contener el script `tex-&lt;format&gt;.js` (donde `&lt;format&gt;` es `chtml` o `svg`). El valor predeterminado apunta a la Red de Distribucion de Contenido de MathJax, por lo que puede ver el resultado rapidamente sin instalar MathJax. Sin embargo, se recomienda encarecidamente instalar una copia local de MathJax desde https://www.mathjax.org antes de implementar. El valor predeterminado es: - en el caso de MathJax Version 2: https://cdn.jsdelivr.net/npm/mathjax@2 - en el caso de MathJax Version 3: https://cdn.jsdelivr.net/npm/mathjax@3 - en el caso de MathJax Version 4: https://cdn.jsdelivr.net/npm/mathjax@4</translation>
    </message>
<message>
        <source>MATHJAX_VERSION</source>
            <translation>Con \c MATHJAX_VERSION es posible especificar la version de MathJax que se usara. Tenga en cuenta que las diferentes versiones de MathJax tienen diferentes requisitos para las diferentes configuraciones, por lo que es posible que tambien sea necesario cambiar otras configuraciones de MathJax al cambiar entre las diferentes versiones de MathJax.</translation>
    </message>
<message>
        <source>MAX_DOT_GRAPH_DEPTH</source>
            <translation>La etiqueta \c MAX_DOT_GRAPH_DEPTH se puede usar para establecer la profundidad maxima de los grafos generados por \c dot. Un valor de profundidad de 3 significa que solo se mostraran los nodos que son accesibles desde el nodo raiz a traves de una ruta de como maximo 3 aristas. Los nodos que se encuentran mas lejos del nodo raiz se omitiran. Tenga en cuenta que configurar esta opcion en 1 o 2 puede reducir en gran medida el tiempo de computacion necesario para bases de codigo grandes. Tenga en cuenta tambien que el tamano de un grafo puede estar mas limitado por \ref cfg_dot_graph_max_nodes "DOT_GRAPH_MAX_NODES". Usar una profundidad de 0 significa sin limite de profundidad.</translation>
    </message>
<message>
        <source>MAX_INITIALIZER_LINES</source>
            <translation>La etiqueta \c MAX_INITIALIZER_LINES determina el numero maximo de lineas que puede tener el valor inicial de una variable o macro / define para aparecer en la documentacion. Si el inicializador consiste en mas lineas de las especificadas aqui, se ocultara. Use el valor 0 para ocultar los inicializadores por completo. La aparicion del valor de variables individuales y macros / defines se puede controlar con el comando \ref cmdshowinitializer "\\showinitializer" o \ref cmdhideinitializer "\\hideinitializer" en la documentacion, independientemente de esta configuracion.</translation>
    </message>
<message>
        <source>MERMAIDFILE_DIRS</source>
            <translation>La etiqueta \c MERMAIDFILE_DIRS se puede usar para especificar uno o mas directorios que contienen archivos Mermaid que se incluyen en la documentacion (vea el comando \ref cmdmermaidfile "\\mermaidfile").</translation>
    </message>
<message>
        <source>MERMAID_CONFIG_FILE</source>
            <translation>Cuando se usan diagramas Mermaid, la etiqueta \c MERMAID_CONFIG_FILE se puede usar para especificar un archivo de configuracion JSON para la herramienta CLI de Mermaid (\c mmdc). Este archivo puede contener configuraciones de tema y otras opciones de configuracion de Mermaid.</translation>
    </message>
<message>
        <source>MERMAID_JS_URL</source>
            <translation>La etiqueta \c MERMAID_JS_URL especifica la URL desde donde cargar \c mermaid.js cuando se usa renderizado del lado del cliente (\c MERMAID_RENDER_MODE es \c CLIENT_SIDE o \c AUTO). El valor predeterminado apunta a la ultima version de Mermaid v11 en el CDN de jsDelivr. \par Ejemplos: - Ultima v11 (predeterminado): \c 'https://cdn.jsdelivr.net/npm/mermaid@11/dist/mermaid.esm.min.mjs' - Version fijada: \c 'https://cdn.jsdelivr.net/npm/mermaid@11.3.0/dist/mermaid.esm.min.mjs' - Copia local: \c './mermaid.esm.min.mjs' (el usuario debe colocar el archivo en el directorio de salida HTML) \par La URL del CDN predeterminada requiere acceso a Internet al ver la documentacion generada. Para uso sin conexion, descargue \c mermaid.esm.min.mjs y configure esto en una ruta relativa, o use \c MERMAID_RENDER_MODE=CLI para pre-generar imagenes en su lugar.</translation>
    </message>
<message>
        <source>MERMAID_PATH</source>
            <translation>Cuando se usan diagramas Mermaid con renderizado CLI, la etiqueta \c MERMAID_PATH debe usarse para especificar el directorio donde se puede encontrar el ejecutable \c mmdc (Mermaid CLI). Si se deja vacio, el renderizado basado en CLI esta deshabilitado. Para salida HTML, el renderizado del lado del cliente via JavaScript se usa por defecto y no requiere \c mmdc. Para salida LaTeX/PDF, se requiere \c mmdc para pre-generar imagenes. Doxygen generara una advertencia cuando se necesite renderizado CLI pero \c mmdc no este disponible.</translation>
    </message>
<message>
        <source>MERMAID_RENDER_MODE</source>
            <translation>La etiqueta \c MERMAID_RENDER_MODE controla como se renderizan los diagramas Mermaid. Los valores posibles son: \c CLI (usa la herramienta `mmdc` desde la linea de comandos), \c CLIENT (usa mermaid.js en el navegador) o \c SERVER (usa mermaid.ink como servicio externo). Por defecto, esto esta configurado en \c CLIENT.</translation>
    </message>
<message>
        <source>MSCFILE_DIRS</source>
            <translation>La etiqueta \c MSCFILE_DIRS se puede usar para especificar uno o mas directorios que contienen archivos MSC que se incluyen en la documentacion (vea el comando \ref cmdmscfile "\\mscfile").</translation>
    </message>
<message>
        <source>MSCGEN_TOOL</source>
            <translation>Puede definir graficos de secuencia de mensajes dentro de los comentarios de Doxygen usando el comando \ref cmdmsc "\\msc". Si la etiqueta \c MSCGEN_TOOL se deja vacia (el valor predeterminado), Doxygen usara una version incorporada de la herramienta mscgen para producir los graficos. Alternativamente, la etiqueta \c MSCGEN_TOOL tambien puede especificar el nombre de una herramienta externa. Por ejemplo, especificando `prog` como valor, Doxygen llamara a la herramienta como `prog -T &lt;formato_archivo_salida&gt; -o &lt;archivo_salida&gt; &lt;archivo_entrada&gt;`. La herramienta externa debe soportar formatos de archivo de salida "png", "eps", "svg" e "ismap".</translation>
    </message>
<message>
        <source>MULTILINE_CPP_IS_BRIEF</source>
            <translation>La etiqueta \c MULTILINE_CPP_IS_BRIEF se puede configurar en \c YES para hacer que Doxygen trate un bloque de comentario especial de C++ multilinea (es decir, un bloque de comentarios \c //! o \c ///) como una descripcion breve. Este era el comportamiento predeterminado anteriormente. El nuevo valor predeterminado es tratar un bloque de comentarios de C++ multilinea como una descripcion detallada. Configure esta etiqueta en \c YES si prefiere el comportamiento anterior. &lt;br&gt;Tenga en cuenta que configurar esta etiqueta en \c YES tambien significa que los comentarios de Rational Rose ya no seran reconocidos.</translation>
    </message>
<message>
        <source>NUM_PROC_THREADS</source>
            <translation>La etiqueta \c NUM_PROC_THREADS especifica el numero de subprocesos que Doxygen puede usar durante el procesamiento. Cuando se configura en \c 0, Doxygen lo basara en el numero de nucleos disponibles en el sistema. Puede configurarlo explicitamente en un valor mayor que 0 para obtener mas control sobre el equilibrio entre la carga de la CPU y la velocidad de procesamiento. Por el momento, solo el procesamiento de entrada se puede realizar con multiples subprocesos. Dado que esta es todavia una caracteristica experimental, el valor predeterminado se establece en 1, lo que deshabilita efectivamente el procesamiento paralelo. Por favor, informe cualquier problema que encuentre. La generacion paralela de grafos dot se controla mediante la configuracion \c DOT_NUM_THREADS.</translation>
    </message>
<message>
        <source>OBFUSCATE_EMAILS</source>
            <translation>Si la etiqueta \c OBFUSCATE_EMAILS esta configurada en \c YES, las direcciones de correo electronico en la salida HTML generada seran ofuscadas para evitar bots de spam.</translation>
    </message>
<message>
        <source>OPTIMIZE_FOR_FORTRAN</source>
            <translation>Configure la etiqueta \c OPTIMIZE_FOR_FORTRAN en \c YES si su proyecto consiste en fuentes Fortran. Doxygen generara una salida adaptada a Fortran.</translation>
    </message>
<message>
        <source>OPTIMIZE_OUTPUT_FOR_C</source>
            <translation>Configure la etiqueta \c OPTIMIZE_OUTPUT_FOR_C en \c YES si su proyecto consiste solo en fuentes C. Doxygen generara una salida mas adaptada a C. Por ejemplo, algunos de los nombres usados seran diferentes. La lista de todos los miembros se omitira, etc.</translation>
    </message>
<message>
        <source>OPTIMIZE_OUTPUT_JAVA</source>
            <translation>Configure la etiqueta \c OPTIMIZE_OUTPUT_JAVA en \c YES si su proyecto consiste solo en fuentes Java o Python. Doxygen generara una salida mas adaptada a ese lenguaje. Por ejemplo, los espacios de nombres se presentaran como paquetes, los ambitos calificados se veran diferentes, etc.</translation>
    </message>
<message>
        <source>OPTIMIZE_OUTPUT_SLICE</source>
            <translation>Configure la etiqueta \c OPTIMIZE_OUTPUT_SLICE en \c YES si su proyecto consiste solo en fuentes Slice. Doxygen generara una salida mas adaptada a ese lenguaje. Por ejemplo, los espacios de nombres se presentaran como modulos, los tipos se agruparan en mas grupos, etc.</translation>
    </message>
<message>
        <source>OPTIMIZE_OUTPUT_VHDL</source>
            <translation>Configure la etiqueta \c OPTIMIZE_OUTPUT_VHDL en \c YES si su proyecto consiste en fuentes VHDL. Doxygen generara una salida adaptada a VHDL.</translation>
    </message>
<message>
        <source>OUTPUT_DIRECTORY</source>
            <translation>La etiqueta \c OUTPUT_DIRECTORY se usa para especificar la ruta (relativa o absoluta) donde se escribira la documentacion generada. Si se ingresa una ruta relativa, es relativa a la ubicacion desde donde se inicio Doxygen. Si se deja vacio, se usara el directorio actual.</translation>
    </message>
<message>
        <source>OUTPUT_LANGUAGE</source>
            <translation>La etiqueta \c OUTPUT_LANGUAGE se usa para especificar el idioma en el que se escribe toda la documentacion generada por Doxygen. Doxygen usara esta informacion para generar toda la salida constante en el idioma apropiado.</translation>
    </message>
<message>
        <source>PAGE_OUTLINE_PANEL</source>
            <translation>Si \ref cfg_generate_treeview "GENERATE_TREEVIEW" esta configurado en \c YES, la opcion \c PAGE_OUTLINE_PANEL determina si se muestra un panel de navegacion adicional en el lado derecho de la pantalla, mostrando un esquema del contenido de la pagina principal, similar a, por ejemplo, https://developer.android.com/reference. Si \ref cfg_generate_treeview "GENERATE_TREEVIEW" esta configurado en \c NO, esta opcion no tiene efecto.</translation>
    </message>
<message>
        <source>PAPER_TYPE</source>
            <translation>La etiqueta \c PAPER_TYPE se puede usar para establecer el tipo de papel que usa la impresora. Los valores posibles son: \c a4, \c letter, \c legal y \c executive.</translation>
    </message>
<message>
        <source>PDF_HYPERLINKS</source>
            <translation>Si la etiqueta \c PDF_HYPERLINKS esta configurada en \c YES, el \f$\mbox{\LaTeX}\f$ generado esta preparado para la conversion a PDF (usando \c ps2pdf o \c pdflatex). El archivo PDF contendra enlaces (igual que la salida HTML) en lugar de referencias de pagina. Esto hace que la salida sea adecuada para navegacion en linea usando un visor PDF.</translation>
    </message>
<message>
        <source>PERLMOD_LATEX</source>
            <translation>Si la etiqueta \c PERLMOD_LATEX esta configurada en \c YES, Doxygen generara las reglas necesarias de \c Makefile, scripts \c Perl y codigo \f$\mbox{\LaTeX}\f$ para poder generar salida PDF y DVI de la salida del modulo Perl.</translation>
    </message>
<message>
        <source>PERLMOD_MAKEVAR_PREFIX</source>
            <translation>Los nombres de las variables make en el archivo `doxyrules.make` generado tienen como prefijo la cadena contenida en \c PERLMOD_MAKEVAR_PREFIX. Esto es util para que diferentes archivos `doxyrules.make` incluidos por el mismo `Makefile` no sobrescriban las variables de los demas.</translation>
    </message>
<message>
        <source>PERLMOD_PRETTY</source>
            <translation>Si la etiqueta \c PERLMOD_PRETTY esta configurada en \c YES, la salida del modulo Perl estara formateada de manera agradable para que pueda ser analizada por un lector humano. Esto es util si desea entender lo que esta sucediendo. Por otro lado, si esta etiqueta esta configurada en \c NO, el tamano de la salida del modulo Perl sera mucho mas pequeno y Perl lo analizara de la misma manera.</translation>
    </message>
<message>
        <source>PLANTUMLFILE_DIRS</source>
            <translation>La etiqueta \c PLANTUMLFILE_DIRS se puede usar para especificar uno o mas directorios que contienen archivos PlantUml que se incluyen en la documentacion (vea el comando \ref cmdplantumlfile "\\plantumlfile").</translation>
    </message>
<message>
        <source>PLANTUML_CFG_FILE</source>
            <translation>Cuando se usa PlantUML, la etiqueta \c PLANTUML_CFG_FILE se puede usar para especificar un archivo de configuracion para PlantUML.</translation>
    </message>
<message>
        <source>PLANTUML_INCLUDE_PATH</source>
            <translation>Cuando se usa PlantUML, las rutas especificadas se buscan para archivos especificados por la declaracion \c !include en un bloque PlantUML.</translation>
    </message>
<message>
        <source>PLANTUML_JAR_PATH</source>
            <translation>Cuando se usa PlantUML, la etiqueta \c PLANTUML_JAR_PATH debe usarse para especificar la ruta donde Java puede encontrar el archivo \c plantuml.jar o el nombre de archivo del archivo \c jar a usar. Si se deja vacio, se asume que PlantUML no se usa o se llama durante un paso de preprocesamiento. Doxygen generara una advertencia cuando encuentre un comando \ref cmdstartuml "\\startuml" en este caso y no generara salida para el diagrama.</translation>
    </message>
<message>
        <source>PREDEFINED</source>
            <translation>La etiqueta \c PREDEFINED se puede usar para especificar uno o mas nombres de macros que se definen antes de que se inicie el preprocesador (similar a la opcion `-D` de, por ejemplo, \c gcc). El argumento de la etiqueta es una lista de macros de la forma: &lt;code&gt;nombre&lt;/code&gt; o &lt;code&gt;nombre=definicion&lt;/code&gt; (sin espacios). Si se omite la definicion y el \c "=", se asume \c "=1". Para evitar que una definicion de macro quede indefinida via \c \#undef o se expanda recursivamente, use el operador &lt;code&gt;:=&lt;/code&gt; en lugar del operador \c =.</translation>
    </message>
<message>
        <source>PROJECT_BRIEF</source>
            <translation>Usando la etiqueta \c PROJECT_BRIEF se puede proporcionar una descripcion opcional de una linea para un proyecto que aparece en la parte superior de cada pagina y debe dar a los espectadores una idea rapida sobre el proposito del proyecto. Mantenga la descripcion corta.</translation>
    </message>
<message>
        <source>PROJECT_ICON</source>
            <translation>Con la etiqueta \c PROJECT_ICON se puede especificar un icono que se incluye en las pestanas cuando se muestra el documento HTML. Doxygen copiara el logotipo en el directorio de salida.</translation>
    </message>
<message>
        <source>PROJECT_LOGO</source>
            <translation>Con la etiqueta \c PROJECT_LOGO se puede especificar un logotipo o icono que se incluye en la documentacion. La altura maxima del logotipo no debe exceder los 55 pixeles y el ancho maximo no debe exceder los 200 pixeles. Doxygen copiara el logotipo en el directorio de salida.</translation>
    </message>
<message>
        <source>PROJECT_NAME</source>
            <translation>La etiqueta \c PROJECT_NAME es una sola palabra (o una secuencia de palabras rodeadas de comillas dobles, a menos que este usando Doxywizard) que debe identificar el proyecto para el cual se genera la documentacion. Este nombre se usa en el titulo de la mayoria de las paginas generadas y en algunos otros lugares.</translation>
    </message>
<message>
        <source>PROJECT_NUMBER</source>
            <translation>La etiqueta \c PROJECT_NUMBER se puede usar para ingresar un numero de proyecto o revision. Esto puede ser util para archivar la documentacion generada o si se usa algun sistema de control de versiones.</translation>
    </message>
<message>
        <source>PYTHON_DOCSTRING</source>
            <translation>Por defecto, los docstrings de Python se muestran como texto preformateado y no se pueden usar los comandos especiales de Doxygen. Al configurar \c PYTHON_DOCSTRING en \c NO, se pueden usar los comandos especiales de Doxygen y el contenido de los bloques de documentacion de docstrings se muestra como documentacion de Doxygen.</translation>
    </message>
<message>
        <source>QCH_FILE</source>
            <translation>Si la etiqueta \ref cfg_qhg_location "QHG_LOCATION" esta especificada, la etiqueta \c QCH_FILE se puede usar para especificar el nombre del archivo `.qch` resultante. La ruta especificada es relativa a la carpeta de salida HTML.</translation>
    </message>
<message>
        <source>QHG_LOCATION</source>
            <translation>La etiqueta \c QHG_LOCATION se puede usar para especificar la ubicacion (ruta absoluta incluyendo nombre de archivo) del qhelpgenerator de Qt. Si no esta vacio, Doxygen intentara ejecutar qhelpgenerator en el archivo `.qhp` generado.</translation>
    </message>
<message>
        <source>QHP_CUST_FILTER_ATTRS</source>
            <translation>La etiqueta \c QHP_CUST_FILTER_ATTRS especifica la lista de atributos del filtro personalizado a agregar. Para mas informacion, vea &lt;a href="https://doc.qt.io/archives/qt-4.8/qthelpproject.html#custom-filters"&gt;Qt Help Project / Custom Filters&lt;/a&gt;.</translation>
    </message>
<message>
        <source>QHP_CUST_FILTER_NAME</source>
            <translation>Si la etiqueta \c QHP_CUST_FILTER_NAME esta configurada, especifica el nombre de un filtro personalizado a agregar. Para mas informacion, vea &lt;a href="https://doc.qt.io/archives/qt-4.8/qthelpproject.html#custom-filters"&gt;Qt Help Project / Custom Filters&lt;/a&gt;.</translation>
    </message>
<message>
        <source>QHP_NAMESPACE</source>
            <translation>La etiqueta \c QHP_NAMESPACE especifica el espacio de nombres a usar al generar la salida del Proyecto de Ayuda Qt. Para mas informacion, vea &lt;a href="https://doc.qt.io/archives/qt-4.8/qthelpproject.html#namespace"&gt;Qt Help Project / Namespace&lt;/a&gt;.</translation>
    </message>
<message>
        <source>QHP_SECT_FILTER_ATTRS</source>
            <translation>La etiqueta \c QHP_SECT_FILTER_ATTRS especifica la lista de atributos con los que coincide la seccion de filtro de este proyecto. &lt;a href="https://doc.qt.io/archives/qt-4.8/qthelpproject.html#filter-attributes"&gt;Qt Help Project / Filter Attributes&lt;/a&gt;.</translation>
    </message>
<message>
        <source>QHP_VIRTUAL_FOLDER</source>
            <translation>La etiqueta \c QHP_VIRTUAL_FOLDER especifica el espacio de nombres a usar al generar la salida del Proyecto de Ayuda Qt. Para mas informacion, vea &lt;a href="https://doc.qt.io/archives/qt-4.8/qthelpproject.html#virtual-folders"&gt;Qt Help Project / Virtual Folders&lt;/a&gt;.</translation>
    </message>
<message>
        <source>QT_AUTOBRIEF</source>
            <translation>Si la etiqueta \c QT_AUTOBRIEF esta configurada en \c YES, Doxygen interpretara la primera linea (hasta el primer punto, signo de interrogacion o signo de exclamacion) de un comentario estilo Qt como la descripcion breve. Si se configura en \c NO, el estilo Qt se comportara como comentarios estilo Qt normales (por lo que se requiere un comando \ref cmdbrief "\\brief" explícito para una descripcion breve).</translation>
    </message>
<message>
        <source>QUIET</source>
            <translation>La etiqueta \c QUIET se puede usar para activar/desactivar los mensajes generados a la salida estandar por Doxygen. Si \c QUIET esta configurado en \c YES, esto implica que los mensajes estan desactivados.</translation>
    </message>
<message>
        <source>RECURSIVE</source>
            <translation>La etiqueta \c RECURSIVE se puede usar para especificar si tambien se deben buscar subdirectorios en busca de archivos de entrada.</translation>
    </message>
<message>
        <source>REFERENCED_BY_RELATION</source>
            <translation>Si la etiqueta \c REFERENCED_BY_RELATION esta configurada en \c YES, para cada entidad documentada se listaran todas las funciones documentadas que la referencian.</translation>
    </message>
<message>
        <source>REFERENCES_LINK_SOURCE</source>
            <translation>Si la etiqueta \c REFERENCES_LINK_SOURCE esta configurada en \c YES y la etiqueta \ref cfg_source_browser "SOURCE_BROWSER" esta configurada en \c YES, entonces los hipervinculos de las funciones en las listas \ref cfg_references_relation "REFERENCES_RELATION" y \ref cfg_referenced_by_relation "REFERENCED_BY_RELATION" enlazaran al codigo fuente. De lo contrario, enlazaran a la documentacion.</translation>
    </message>
<message>
        <source>REFERENCES_RELATION</source>
            <translation>Si la etiqueta \c REFERENCES_RELATION esta configurada en \c YES, para cada funcion documentada se listaran todas las entidades documentadas llamadas/usadas por esa funcion.</translation>
    </message>
<message>
        <source>REPEAT_BRIEF</source>
            <translation>Si la etiqueta \c REPEAT_BRIEF esta configurada en \c YES, Doxygen antepondra la descripcion breve de un miembro o funcion antes de la descripcion detallada. &lt;br&gt;Nota: Si tanto \ref cfg_hide_undoc_members "HIDE_UNDOC_MEMBERS" como \ref cfg_brief_member_desc "BRIEF_MEMBER_DESC" estan configurados en \c NO, las descripciones breves se suprimiran por completo.</translation>
    </message>
<message>
        <source>REQ_TRACEABILITY_INFO</source>
            <translation>La etiqueta \c REQ_TRACEABILITY_INFO controla si se muestra informacion de trazabilidad en la pagina de requisitos (solo relevante cuando se usan bloques de comentarios \ref cmdrequirement "\\requirement"). La configuracion \c NO deshabilitara la informacion de trazabilidad por completo. La configuracion \c UNSATISFIED_ONLY mostrara una lista de requisitos a los que les falta una relacion satisfies (a traves del comando: \ref cmdsatisfies "\\satisfies"). De manera similar, la configuracion \c UNVERIFIED_ONLY mostrara una lista de requisitos a los que les falta una relacion verifies (a traves del comando: \ref cmdverifies "\\verifies"). Configurar la etiqueta en \c YES (el valor predeterminado) mostrara ambas listas si corresponde.</translation>
    </message>
<message>
        <source>RESOLVE_UNNAMED_PARAMS</source>
            <translation>Si esta bandera esta configurada en \c YES, el nombre de un parametro sin nombre en una declaracion sera determinado por la definicion correspondiente. Por defecto, los parametros sin nombre permanecen sin nombre en la salida.</translation>
    </message>
<message>
        <source>RTF_EXTENSIONS_FILE</source>
            <translation>Configure variables opcionales usadas en la generacion de un documento RTF. La sintaxis es similar al archivo de configuracion de Doxygen. Se puede generar un archivo de extensiones de plantilla usando &lt;code&gt;doxygen -e rtf extensionFile&lt;/code&gt;.</translation>
    </message>
<message>
        <source>RTF_EXTRA_FILES</source>
            <translation>La etiqueta \c RTF_EXTRA_FILES se puede usar para especificar una o mas imagenes adicionales u otros archivos de origen que deben copiarse en el directorio de salida RTF. Tenga en cuenta que los archivos se copian tal como estan; no hay comandos o marcadores disponibles.</translation>
    </message>
<message>
        <source>RTF_HYPERLINKS</source>
            <translation>Si la etiqueta \c RTF_HYPERLINKS esta configurada en \c YES, la salida RTF generada puede contener enlaces. Esto requiere que el archivo de salida sea leido por aplicaciones como Word o WordPad que soportan enlaces RTF. Para otras aplicaciones, los enlaces aparecen como texto normal.</translation>
    </message>
<message>
        <source>RTF_OUTPUT</source>
            <translation>La etiqueta \c RTF_OUTPUT se usa para especificar donde se colocaran los documentos RTF. Si se ingresa una ruta relativa, el valor de \ref cfg_output_directory "OUTPUT_DIRECTORY" se antepondra.</translation>
    </message>
<message>
        <source>RTF_STYLESHEET_FILE</source>
            <translation>Cargue definiciones de hoja de estilo desde un archivo. La sintaxis es similar al archivo de configuracion de Doxygen, es decir, una serie de asignaciones. Solo tiene que proporcionar reemplazos, las definiciones faltantes se establecen en su valor predeterminado. &lt;br&gt;Vea tambien la seccion \ref doxygen_usage para informacion sobre como generar la hoja de estilo predeterminada que Doxygen normalmente usa.</translation>
    </message>
<message>
        <source>SEARCHDATA_FILE</source>
            <translation>Si \ref cfg_server_based_search "SERVER_BASED_SEARCH" y \ref cfg_external_search "EXTERNAL_SEARCH" estan habilitados, la etiqueta \c SEARCHDATA_FILE se puede usar para especificar un archivo donde escribir los datos de busqueda. Si se deja vacio, se usara `searchdata.xml`.</translation>
    </message>
<message>
        <source>SEARCHENGINE</source>
            <translation>Cuando la etiqueta \c SEARCHENGINE esta habilitada, Doxygen generara un cuadro de busqueda para la salida HTML. El motor de busqueda subyacente usa JavaScript y DHTML y deberia funcionar en cualquier navegador moderno. Tenga en cuenta que cuando se usa ayuda HTML (\ref cfg_generate_htmlhelp "GENERATE_HTMLHELP"), ayuda Qt (\ref cfg_generate_qhp "GENERATE_QHP") o docsets (\ref cfg_generate_docset "GENERATE_DOCSET") ya hay una funcion de busqueda, por lo que esta deberia estar tipicamente deshabilitada. Para proyectos grandes, el motor de busqueda basado en JavaScript puede ser lento, entonces habilitar \ref cfg_server_based_search "SERVER_BASED_SEARCH" puede proporcionar una mejor solucion. Es posible buscar usando el teclado; para saltar al cuadro de busqueda use &lt;code&gt;\&lt;tecla de acceso\&gt; + S&lt;/code&gt; (cual es la &lt;code&gt;\&lt;tecla de acceso\&gt;&lt;/code&gt; depende del sistema).</translation>
    </message>
<message>
        <source>SEARCHENGINE_URL</source>
            <translation>Si \ref cfg_server_based_search "SERVER_BASED_SEARCH" y \ref cfg_external_search "EXTERNAL_SEARCH" estan habilitados, la etiqueta \c SEARCHENGINE_URL se puede usar para especificar una URL a la que enviar una consulta de busqueda para obtener los resultados de busqueda. La consulta de busqueda se anexa a la URL. Para el motor de busqueda externo, la consulta de busqueda se envia como un objeto JSON.</translation>
    </message>
<message>
        <source>SEARCH_INCLUDES</source>
            <translation>Si la etiqueta \c SEARCH_INCLUDES esta configurada en \c YES, las dependencias de inclusion se incluyen en la lista de archivos que se buscan.</translation>
    </message>
<message>
        <source>SEPARATE_MEMBER_PAGES</source>
            <translation>Si la etiqueta \c SEPARATE_MEMBER_PAGES esta configurada en \c YES, Doxygen generara una pagina separada para cada miembro. Si se configura en \c NO, la documentacion de todos los miembros de un compuesto (clase, archivo, espacio de nombres) se combinara en un solo documento.</translation>
    </message>
<message>
        <source>SERVER_BASED_SEARCH</source>
            <translation>Si la etiqueta \c SERVER_BASED_SEARCH esta configurada en \c YES, el motor de busqueda se implementa usando un servidor web en lugar de un cliente web. Hay dos formas de configurar esto: usando un script PHP o usando un indexador externo. Vea la seccion \ref extsearch para mas detalles.</translation>
    </message>
<message>
        <source>SHORT_NAMES</source>
            <translation>Si la etiqueta \c SHORT_NAMES esta configurada en \c YES, Doxygen generara nombres de archivo cortos (pero no unicos). Esto puede ser util si su sistema de archivos no soporta nombres largos, como DOS, Mac o CD-ROM.</translation>
    </message>
<message>
        <source>SHOW_ENUM_VALUES</source>
            <translation>Cuando la etiqueta \c SHOW_ENUM_VALUES esta configurada, Doxygen mostrara los valores de enumeracion especificados junto a los mnemonicos de enumeracion.</translation>
    </message>
<message>
        <source>SHOW_FILES</source>
            <translation>Si la etiqueta \c SHOW_FILES esta configurada en \c NO, la lista de archivos no se generara. Esto ahorra tiempo si tiene un numero muy grande de archivos y no necesita la lista de archivos.</translation>
    </message>
<message>
        <source>SHOW_GROUPED_MEMB_INC</source>
            <translation>Si la etiqueta \c SHOW_GROUPED_MEMB_INC esta configurada en \c YES, las dependencias de inclusion para miembros de grupos se mostraran en la documentacion.</translation>
    </message>
<message>
        <source>SHOW_HEADERFILE</source>
            <translation>Si la etiqueta \c SHOW_HEADERFILE esta configurada en \c YES, la documentacion de una clase mostrara que archivo necesita ser incluido para usar la clase.</translation>
    </message>
<message>
        <source>SHOW_INCLUDE_FILES</source>
            <translation>Si la etiqueta \c SHOW_INCLUDE_FILES esta configurada en \c YES, Doxygen mostrara una lista de los archivos que son incluidos por un archivo en la documentacion de ese archivo.</translation>
    </message>
<message>
        <source>SHOW_NAMESPACES</source>
            <translation>Si la etiqueta \c SHOW_NAMESPACES esta configurada en \c NO, la lista de espacios de nombres no se generara. Esto ahorra tiempo si tiene un numero muy grande de espacios de nombres y no necesita la lista de espacios de nombres.</translation>
    </message>
<message>
        <source>SHOW_USED_FILES</source>
            <translation>Configure la etiqueta \c SHOW_USED_FILES en \c NO para deshabilitar la lista de archivos generados al final de la documentacion de una clase.</translation>
    </message>
<message>
        <source>SIP_SUPPORT</source>
            <translation>Si usa SIP (vea https://www.riverbankcomputing.com/software/sip/) para crear enlaces Python, configure esta opcion en \c YES para que Doxygen reconozca los comentarios especiales para soporte SIP.</translation>
    </message>
<message>
        <source>SITEMAP_URL</source>
            <translation>La etiqueta \c SITEMAP_URL se usa para especificar la URL completa del lugar donde el usuario colocara la documentacion generada en el servidor durante el despliegue de la documentacion. El mapa del sitio generado se llama `sitemap.xml` y se coloca en el directorio especificado por \ref cfg_html_output "HTML_OUTPUT". En caso de que no se especifique \c SITEMAP_URL, no se genera mapa del sitio. Para informacion sobre el protocolo del mapa del sitio vea https://www.sitemaps.org</translation>
    </message>
<message>
        <source>SKIP_FUNCTION_MACROS</source>
            <translation>Esta etiqueta se puede usar para especificar la lista de macros de funcion que deben omitirse en la documentacion. Las macros de funcion son macros que parecen funciones y tienen parametros.</translation>
    </message>
<message>
        <source>SORT_BRIEF_DOCS</source>
            <translation>Si la etiqueta \c SORT_BRIEF_DOCS esta configurada en \c YES, Doxygen ordenara las descripciones breves de los miembros de archivos, espacios de nombres y clases alfabeticamente por nombre de miembro. Si se configura en \c NO, los miembros apareceran en orden de declaracion. Tenga en cuenta que esto tambien influira en el orden de las clases en la lista de clases.</translation>
    </message>
<message>
        <source>SORT_BY_SCOPE_NAME</source>
            <translation>Si la etiqueta \c SORT_BY_SCOPE_NAME esta configurada en \c YES, la lista de clases se ordenara por nombres completamente calificados, incluyendo espacios de nombres. Si se configura en \c NO, la lista de clases se ordenara solo por nombre de clase, sin incluir la parte del espacio de nombres. \note Esta opcion no es muy util si \ref cfg_hide_scope_names "HIDE_SCOPE_NAMES" esta configurado en \c YES. \note Esta opcion se aplica solo a la lista de clases, no a la lista alfabetica.</translation>
    </message>
<message>
        <source>SORT_GROUP_NAMES</source>
            <translation>Si la etiqueta \c SORT_GROUP_NAMES esta configurada en \c YES, Doxygen ordenara la jerarquia de nombres de grupos en orden alfabetico. Si se configura en \c NO, los nombres de grupos apareceran en su orden definido.</translation>
    </message>
<message>
        <source>SORT_MEMBERS_CTORS_1ST</source>
            <translation>Si la etiqueta \c SORT_MEMBERS_CTORS_1ST esta configurada en \c YES, Doxygen ordenara la documentacion (breve y detallada) de los miembros de clase para que los constructores y destructores se listen primero. Si se configura en \c NO, los constructores apareceran en los ordenes respectivos definidos por \ref cfg_sort_brief_docs "SORT_BRIEF_DOCS" y \ref cfg_sort_member_docs "SORT_MEMBER_DOCS". \note Si \ref cfg_sort_brief_docs "SORT_BRIEF_DOCS" esta configurado en \c NO, esta opcion se ignora para ordenar la documentacion breve de miembros. \note Si \ref cfg_sort_member_docs "SORT_MEMBER_DOCS" esta configurado en \c NO, esta opcion se ignora para ordenar la documentacion detallada de miembros.</translation>
    </message>
<message>
        <source>SORT_MEMBER_DOCS</source>
            <translation>Si la etiqueta \c SORT_MEMBER_DOCS esta configurada en \c YES, Doxygen ordenara la documentacion (detallada) de los miembros de archivos y clases alfabeticamente por nombre de miembro. Si se configura en \c NO, los miembros apareceran en orden de declaracion.</translation>
    </message>
<message>
        <source>SOURCE_BROWSER</source>
            <translation>Si la etiqueta \c SOURCE_BROWSER esta configurada en \c YES, se generara una lista de archivos fuente. Las entidades documentadas seran referenciadas cruzadas con estas fuentes. &lt;br&gt;Nota: Para deshacerse de todo el codigo fuente en la salida generada, asegurese de que tambien \ref cfg_verbatim_headers "VERBATIM_HEADERS" este configurado en \c NO.</translation>
    </message>
<message>
        <source>SOURCE_TOOLTIPS</source>
            <translation>Si \c SOURCE_TOOLTIPS esta habilitado (el valor predeterminado), al pasar el cursor sobre un hipervinculo en el codigo fuente se mostrara una descripcion emergente con informacion adicional como prototipo, descripcion breve y enlaces a la definicion y documentacion. Dado que esto hara que el archivo HTML sea mas grande y la carga de archivos grandes sea un poco mas lenta, puede optar por deshabilitar esta funcion.</translation>
    </message>
<message>
        <source>SQLITE3_OUTPUT</source>
            <translation>La etiqueta \c SQLITE3_OUTPUT se usa para especificar donde se colocara la base de datos \c Sqlite3. Si se ingresa una ruta relativa, el valor de \ref cfg_output_directory "OUTPUT_DIRECTORY" se antepondra.</translation>
    </message>
<message>
        <source>SQLITE3_RECREATE_DB</source>
            <translation>Si la etiqueta \c SQLITE3_RECREATE_DB esta configurada en \c YES, el archivo de base de datos doxygen_sqlite3.db existente se recreara con cada ejecucion de Doxygen. Si se configura en \c NO, Doxygen advertira si ya se encuentra un archivo de base de datos y no lo modificara.</translation>
    </message>
<message>
        <source>STRICT_PROTO_MATCHING</source>
            <translation>Si la opcion \c STRICT_PROTO_MATCHING esta habilitada y Doxygen no puede hacer la resolucion de tipo adecuada de todos los parametros de una funcion, rechazara una coincidencia entre el prototipo y la implementacion de una funcion miembro incluso si solo hay un candidato o es obvio que candidato elegir haciendo una simple coincidencia de cadena. Al deshabilitar \c STRICT_PROTO_MATCHING, Doxygen seguira aceptando una coincidencia entre prototipo e implementacion en tales casos.</translation>
    </message>
<message>
        <source>STRIP_CODE_COMMENTS</source>
            <translation>Configurar la etiqueta \c STRIP_CODE_COMMENTS en \c YES indicara a Doxygen que oculte cualquier bloque de comentario especial de los fragmentos de codigo fuente generados. Los comentarios normales de C, C++ y Fortran siempre permaneceran visibles.</translation>
    </message>
<message>
        <source>STRIP_FROM_INC_PATH</source>
            <translation>La etiqueta \c STRIP_FROM_INC_PATH se puede usar para eliminar una parte definida por el usuario de la ruta mencionada en la documentacion de una clase, que le dice al lector que archivo de encabezado incluir para usar una clase. Si se deja vacio, solo se usara el nombre del archivo de encabezado que contiene la definicion de la clase. De lo contrario, se debe especificar la lista de rutas de inclusion que normalmente se pasan al compilador usando la bandera `-I`.</translation>
    </message>
<message>
        <source>STRIP_FROM_PATH</source>
            <translation>La etiqueta \c STRIP_FROM_PATH se puede usar para eliminar una parte definida por el usuario de la ruta. La eliminacion solo se hace si una de las cadenas especificadas coincide con la parte izquierda de la ruta. La etiqueta se puede usar para mostrar rutas relativas en la lista de archivos. Si se deja vacio, el directorio desde donde se ejecuta Doxygen se usa como la ruta a eliminar. &lt;br&gt;Tenga en cuenta que puede especificar rutas absolutas aqui, pero tambien rutas relativas, que seran relativas al directorio donde se inicio Doxygen.</translation>
    </message>
<message>
        <source>SUBGROUPING</source>
            <translation>Configure la etiqueta \c SUBGROUPING en \c YES para permitir que los grupos de miembros de clase del mismo tipo (por ejemplo, un grupo de funciones publicas) se coloquen como un subgrupo de ese tipo (por ejemplo, bajo la seccion Funciones Publicas). Configúrelo en \c NO para evitar la subagrupacion. Alternativamente, esto se puede hacer por clase usando el comando \ref cmdnosubgrouping "\\nosubgrouping".</translation>
    </message>
<message>
        <source>TAB_SIZE</source>
            <translation>La etiqueta \c TAB_SIZE se puede usar para establecer el numero de espacios en una tabulacion. Doxygen usa este valor para reemplazar las tabulaciones por espacios en los fragmentos de codigo.</translation>
    </message>
<message>
        <source>TAGFILES</source>
            <translation>La etiqueta \c TAGFILES se puede usar para especificar uno o mas archivos de etiquetas. Para cada archivo de etiquetas se debe agregar la ubicacion de la documentacion externa. El formato de un archivo de etiquetas sin esta ubicacion es el siguiente: \verbatim TAGFILES = file1 file2 ... \endverbatim Agregar ubicacion para los archivos de etiquetas se hace de la siguiente manera: \verbatim TAGFILES = file1=loc1 "file2 = loc2" ... \endverbatim donde `loc1` y `loc2` pueden ser rutas relativas o absolutas o URLs. Vea la seccion \ref external para mas informacion sobre el uso de archivos de etiquetas. \note Cada archivo de etiquetas debe tener un nombre unico (donde el nombre NO incluye la ruta). Si un archivo de etiquetas no esta ubicado en el directorio en el que se ejecuta Doxygen, tambien debe especificar la ruta al archivo de etiquetas aqui.</translation>
    </message>
<message>
        <source>TEMPLATE_RELATIONS</source>
            <translation>Si la etiqueta \c TEMPLATE_RELATIONS esta configurada en \c YES, los grafos de herencia y colaboracion mostraran las relaciones entre plantillas y sus instancias.</translation>
    </message>
<message>
        <source>TIMESTAMP</source>
            <translation>Si la etiqueta \c TIMESTAMP esta configurada diferente de \c NO, cada pagina generada contendra la fecha o fecha y hora cuando se genero la pagina. Configurar esto en \c NO puede ayudar al comparar la salida de multiples ejecuciones.</translation>
    </message>
<message>
        <source>TOC_EXPAND</source>
            <translation>La bandera \c TOC_EXPAND se puede configurar en \c YES para agregar elementos adicionales para miembros de grupos a la tabla de contenidos de la documentacion de ayuda HTML y a la vista de arbol.</translation>
    </message>
<message>
        <source>TOC_INCLUDE_HEADINGS</source>
            <translation>Cuando la etiqueta \c TOC_INCLUDE_HEADINGS esta configurada en un valor no nulo, todos los encabezados hasta ese nivel se incluyen automaticamente en la tabla de contenidos, incluso si no tienen un atributo id. \note Esta funcion actualmente se aplica solo a los encabezados de Markdown.</translation>
    </message>
<message>
        <source>TREEVIEW_WIDTH</source>
            <translation>Si la vista de arbol esta habilitada (vea \ref cfg_generate_treeview "GENERATE_TREEVIEW"), entonces esta etiqueta se puede usar para establecer el ancho inicial (en pixeles) del marco en el que se muestra el arbol.</translation>
    </message>
<message>
        <source>TYPEDEF_HIDES_STRUCT</source>
            <translation>Cuando la etiqueta \c TYPEDEF_HIDES_STRUCT esta habilitada, una definicion de tipo de una estructura, union o enumeracion se documenta como estructura, union o enumeracion con el nombre de la definicion de tipo. Asi, &lt;code&gt;typedef struct TypeS {} TypeT&lt;/code&gt; aparecera en la documentacion como una estructura con nombre \c TypeT. Cuando esta deshabilitado, la definicion de tipo aparecera como un miembro de un archivo, espacio de nombres o clase. Y la estructura se llamara \c TypeS. Esto puede ser tipicamente util para codigo C en caso de que la convencion de codificacion dicte que todos los tipos compuestos se definen con typedef y solo se hace referencia a la definicion de tipo, nunca al nombre de la etiqueta.</translation>
    </message>
<message>
        <source>UML_LIMIT_NUM_FIELDS</source>
            <translation>Si la etiqueta \ref cfg_uml_look "UML_LOOK" esta habilitada, los campos y metodos se muestran dentro del nodo de clase. Si hay muchos campos o metodos y muchos nodos, el grafo puede volverse demasiado grande para ser util. El umbral \c UML_LIMIT_NUM_FIELDS limita el numero de elementos para cada tipo para hacer el tamano mas manejable. Configure esto en 0 para sin limite. Tenga en cuenta que el umbral puede excederse en un 50% antes de que se aplique el limite. Asi que cuando configura el umbral en 10, pueden aparecer hasta 15 campos, pero si el numero excede 15, la cantidad total de campos mostrados se limita a 10.</translation>
    </message>
<message>
        <source>UML_LOOK</source>
            <translation>Si la etiqueta \c UML_LOOK esta configurada en \c YES, Doxygen generara diagramas de herencia y colaboracion en un estilo similar al Lenguaje de Modelado Unificado de OMG.</translation>
    </message>
<message>
        <source>UML_MAX_EDGE_LABELS</source>
            <translation>Si la etiqueta \ref cfg_uml_look "UML_LOOK" esta habilitada, las etiquetas de campo se muestran a lo largo de la arista entre dos nodos de clase. Si hay muchos campos y muchos nodos, el grafo puede volverse demasiado desordenado. El umbral \c UML_MAX_EDGE_LABELS limita el numero de elementos para hacer el tamano mas manejable. Configure esto en 0 para sin limite.</translation>
    </message>
<message>
        <source>USE_HTAGS</source>
            <translation>Si la etiqueta \c USE_HTAGS esta configurada en \c YES, las referencias al codigo fuente apuntaran al HTML generado por la herramienta \c htags(1) en lugar del navegador de codigo fuente incorporado de Doxygen. La herramienta \c htags es parte del sistema de etiquetado de codigo fuente global de GNU (vea https://www.gnu.org/software/global/global.html). Necesitara la version 4.8.6 o superior. &lt;br&gt;Para usarlo haga lo siguiente: -# Instale la ultima version de \c global -# Habilite \ref cfg_source_browser "SOURCE_BROWSER" y \c USE_HTAGS en el archivo de configuracion -# Asegurese de que \ref cfg_input "INPUT" apunte a la raiz del arbol de fuentes -# Ejecute \c doxygen como de costumbre &lt;br&gt;Doxygen invocara \c htags (y eso a su vez invocara \c gtags), por lo que estas herramientas deben estar disponibles desde la linea de comandos (es decir, en la ruta de busqueda). &lt;br&gt;El resultado: en lugar del navegador de codigo fuente generado por Doxygen, los enlaces al codigo fuente ahora apuntaran a la salida de \c htags.</translation>
    </message>
<message>
        <source>USE_MATHJAX</source>
            <translation>Habilite la opcion \c USE_MATHJAX para renderizar formulas \f$\mbox{\LaTeX}\f$ usando MathJax (vea https://www.mathjax.org) que usa JavaScript del lado del cliente para el renderizado en lugar de usar bitmaps pre-renderizados. Use esto si no tiene \f$\mbox{\LaTeX}\f$ instalado o si desea que las formulas se vean mas bonitas en la salida HTML. Cuando esta habilitado, tambien puede necesitar instalar MathJax por separado y configurar la ruta usando la opcion \ref cfg_mathjax_relpath "MATHJAX_RELPATH".</translation>
    </message>
<message>
        <source>USE_MDFILE_AS_MAINPAGE</source>
            <translation>Si la etiqueta \c USE_MDFILE_AS_MAINPAGE se refiere al nombre de un archivo markdown que es parte de la entrada, su contenido se colocara en la pagina principal (`index.html`). Esto puede ser util si tiene un proyecto en, por ejemplo, GitHub y desea reutilizar el archivo README.md como pagina principal.</translation>
    </message>
<message>
        <source>USE_PDFLATEX</source>
            <translation>Si la etiqueta \c USE_PDFLATEX esta configurada en \c YES, Doxygen usara el motor especificado con \ref cfg_latex_cmd_name "LATEX_CMD_NAME" para generar el archivo PDF directamente de los archivos \f$\mbox{\LaTeX}\f$. Configure esta opcion en \c YES para obtener documentacion PDF de mayor calidad. &lt;br&gt;Vea tambien la seccion \ref cfg_latex_cmd_name "LATEX_CMD_NAME" para seleccionar el motor.</translation>
    </message>
<message>
        <source>VERBATIM_HEADERS</source>
            <translation>Si la etiqueta \c VERBATIM_HEADERS esta configurada en \c YES, Doxygen generara una copia literal del archivo de encabezado para cada clase para la cual se especifica una inclusion. Configure en \c NO para deshabilitar esto. \sa Seccion \ref cmdclass "\\class".</translation>
    </message>
<message>
        <source>WARNINGS</source>
            <translation>La etiqueta \c WARNINGS se puede usar para activar/desactivar los mensajes de advertencia generados a la salida de error estandar (\c stderr) por Doxygen. Si \c WARNINGS esta configurado en \c YES, esto implica que las advertencias estan activadas. &lt;br&gt; \b Consejo: Active las advertencias mientras escribe la documentacion.</translation>
    </message>
<message>
        <source>WARN_AS_ERROR</source>
            <translation>Si la etiqueta \c WARN_AS_ERROR esta configurada en \c YES, Doxygen se detendra inmediatamente cuando encuentre una advertencia. Si la etiqueta \c WARN_AS_ERROR esta configurada en \c FAIL_ON_WARNINGS, Doxygen continuara ejecutandose como si la etiqueta \c WARN_AS_ERROR estuviera configurada en \c NO, pero al final del proceso Doxygen regresara con un estado distinto de cero. Si la etiqueta \c WARN_AS_ERROR esta configurada en \c FAIL_ON_WARNINGS_PRINT, Doxygen se comportara como \c FAIL_ON_WARNINGS pero en caso de que no se defina \ref cfg_warn_logfile "WARN_LOGFILE", Doxygen no escribira los mensajes de advertencia entre otros mensajes sino que los escribira al final de una ejecucion. En caso de que se defina \ref cfg_warn_logfile "WARN_LOGFILE", los mensajes de advertencia ademas de estar en el archivo definido tambien se mostraran al final de una ejecucion, a menos que \ref cfg_warn_logfile "WARN_LOGFILE" se defina como `-`, es decir, salida estandar (`stdout`), en ese caso el comportamiento seguira siendo como con la configuracion \c FAIL_ON_WARNINGS.</translation>
    </message>
<message>
        <source>WARN_FORMAT</source>
            <translation>La etiqueta \c WARN_FORMAT determina el formato de los mensajes de advertencia que Doxygen puede producir. La cadena debe contener las etiquetas &lt;code&gt;\$file&lt;/code&gt;, &lt;code&gt;\$line&lt;/code&gt; y &lt;code&gt;\$text&lt;/code&gt;, que seran reemplazadas por el archivo y numero de linea desde donde se origino la advertencia y el texto de la advertencia. Opcionalmente, el formato puede contener &lt;code&gt;$version&lt;/code&gt;, que sera reemplazado por la version del archivo (si se pudo obtener a traves de \ref cfg_file_version_filter "FILE_VERSION_FILTER"). \sa \ref cfg_warn_line_format "WARN_LINE_FORMAT"</translation>
    </message>
<message>
        <source>WARN_IF_DOC_ERROR</source>
            <translation>Si la etiqueta \c WARN_IF_DOC_ERROR esta configurada en \c YES, Doxygen generara advertencias para errores potenciales en la documentacion, como documentar algunos parametros en una funcion documentada dos veces, o documentar parametros que no existen o usar comandos de marcado incorrectamente.</translation>
    </message>
<message>
        <source>WARN_IF_INCOMPLETE_DOC</source>
            <translation>Si \c WARN_IF_INCOMPLETE_DOC esta configurado en \c YES, Doxygen advertira sobre documentacion incompleta de parametros de funcion. Si se configura en \c NO, Doxygen aceptara que algunos parametros no tengan documentacion sin advertir.</translation>
    </message>
<message>
        <source>WARN_IF_UNDOCUMENTED</source>
            <translation>Si la etiqueta \c WARN_IF_UNDOCUMENTED esta configurada en \c YES, Doxygen generara advertencias para miembros no documentados. Si \ref cfg_extract_all "EXTRACT_ALL" esta configurado en \c YES, esta bandera se deshabilitara automaticamente.</translation>
    </message>
<message>
        <source>WARN_IF_UNDOC_ENUM_VAL</source>
            <translation>Si la opcion \c WARN_IF_UNDOC_ENUM_VAL esta configurada en \c YES, Doxygen advertira sobre valores de enumeracion no documentados. Si se configura en \c NO, Doxygen aceptara valores de enumeracion no documentados. Si \ref cfg_extract_all "EXTRACT_ALL" esta configurado en \c YES, esta bandera se deshabilitara automaticamente.</translation>
    </message>
<message>
        <source>WARN_LAYOUT_FILE</source>
            <translation>Si la opcion \c WARN_LAYOUT_FILE esta configurada en \c YES, Doxygen advertira sobre problemas encontrados al analizar el archivo de diseño definido por el usuario, como elementos faltantes o incorrectos. Vea tambien \ref cfg_layout_file "LAYOUT_FILE" para detalles. Si se configura en \c NO, los problemas con el archivo de diseño se suprimiran.</translation>
    </message>
<message>
        <source>WARN_LINE_FORMAT</source>
            <translation>En la parte `$text` del comando \ref cfg_warn_format "WARN_FORMAT", es posible que se proporcione una referencia a un lugar mas especifico. Para facilitar el salto a este lugar (fuera de Doxygen), el usuario puede definir una cadena personalizada de "cortar"/"pegar". Ejemplo: \verbatim WARN_LINE_FORMAT = "'vi $file +$line'" \endverbatim \sa \ref cfg_warn_format "WARN_FORMAT"</translation>
    </message>
<message>
        <source>WARN_LOGFILE</source>
            <translation>La etiqueta \c WARN_LOGFILE se puede usar para especificar un archivo en el que se deben escribir los mensajes de advertencia y error. Si se deja vacio, la salida se escribe en el error estandar (`stderr`). En caso de que el archivo especificado no se pueda abrir para escritura, los mensajes de advertencia y error se escriben en el error estandar. Cuando se especifica el archivo `-`, los mensajes de advertencia y error se escriben en la salida estandar (`stdout`).</translation>
    </message>
<message>
        <source>WARN_NO_PARAMDOC</source>
            <translation>Esta opcion \c WARN_NO_PARAMDOC se puede habilitar para obtener advertencias para funciones que estan documentadas, pero no tienen documentacion para sus parametros o valor de retorno. Si se configura en \c NO, Doxygen solo advertira sobre documentacion de parametros incorrecta, pero no sobre la ausencia de documentacion. Si \ref cfg_extract_all "EXTRACT_ALL" esta configurado en \c YES, esta bandera se deshabilitara automaticamente. Vea tambien \ref cfg_warn_if_incomplete_doc "WARN_IF_INCOMPLETE_DOC"</translation>
    </message>
<message>
        <source>XML_NS_MEMB_FILE_SCOPE</source>
            <translation>Si la etiqueta \c XML_NS_MEMB_FILE_SCOPE esta configurada en \c YES, Doxygen incluira miembros de espacios de nombres en el ambito del archivo tambien, coincidiendo con la salida HTML.</translation>
    </message>
<message>
        <source>XML_OUTPUT</source>
            <translation>La etiqueta \c XML_OUTPUT se usa para especificar donde se colocaran las paginas XML. Si se ingresa una ruta relativa, el valor de \ref cfg_output_directory "OUTPUT_DIRECTORY" se antepondra.</translation>
    </message>
<message>
        <source>XML_PROGRAMLISTING</source>
            <translation>Si la etiqueta \c XML_PROGRAMLISTING esta configurada en \c YES, Doxygen volcara los listados de programas (incluyendo resaltado de sintaxis e informacion de referencias cruzadas) a la salida XML. Tenga en cuenta que habilitar esto aumentara significativamente el tamano de la salida XML.</translation>
    </message>
</context>
<context>
    <name>OptionValue</name>
<message>
        <source>(United States)</source>
        <translation>(Estados Unidos)</translation>
    </message>
<message>
        <source>(Persian)</source>
        <translation>(Persa)</translation>
    </message>
<message>
        <source>(Japanese with English messages)</source>
        <translation>(Japones con mensajes en ingles)</translation>
    </message>
<message>
        <source>(Korean with English messages)</source>
        <translation>(Coreano con mensajes en ingles)</translation>
    </message>
<message>
        <source>(Farsi)</source>
        <translation>(Farsi)</translation>
    </message>
<message>
        <source>use a fixed 'autotoc_md' string followed by a sequence number starting at 0</source>
        <translation>usar una cadena 'autotoc_md' fija seguida de un numero de secuencia comenzando en 0</translation>
    </message>
<message>
        <source>use the lower case version of title with any whitespace replaced by '-' and punctuation characters removed</source>
        <translation>usar la version en minusculas del titulo con cualquier espacio en blanco reemplazado por '-' y caracteres de puntuacion eliminados</translation>
    </message>
<message>
        <source>(to be provided as Doxygen C comment)</source>
        <translation>(proporcionar como comentario C de Doxygen)</translation>
    </message>
<message>
        <source>always generates light mode output</source>
        <translation>siempre genera salida en modo claro</translation>
    </message>
<message>
        <source>always generates dark mode output</source>
        <translation>siempre genera salida en modo oscuro</translation>
    </message>
<message>
        <source>automatically sets the mode according to the user preference, uses light mode if no preference is set (the default)</source>
        <translation>establece automaticamente el modo segun la preferencia del usuario, usa modo claro si no hay preferencia establecida (predeterminado)</translation>
    </message>
<message>
        <source>automatically sets the mode according to the user preference, uses dark mode if no preference is set</source>
        <translation>establece automaticamente el modo segun la preferencia del usuario, usa modo oscuro si no hay preferencia establecida</translation>
    </message>
<message>
        <source>allows a user to switch between light and dark mode via a button</source>
        <translation>permite al usuario cambiar entre modo claro y oscuro mediante un boton</translation>
    </message>
<message>
        <source>(the default)</source>
        <translation>(predeterminado)</translation>
    </message>
<message>
        <source>(looks nicer but requires the pdf2svg or inkscape tool)</source>
        <translation>(se ve mejor pero requiere la herramienta pdf2svg o inkscape)</translation>
    </message>
<message>
        <source>(which is slower, but has the best compatibility. This is the name for Mathjax version 2, for MathJax version 3 this will be translated into \c chtml)</source>
        <translation>(mas lento, pero con mejor compatibilidad. Este es el nombre para MathJax version 2, para MathJax version 3 se traducira a \c chtml)</translation>
    </message>
<message>
        <source>(i.e. MathML. Only supported for MathJax 2. For MathJax version 3 \c chtml will be used instead.)</source>
        <translation>(es decir, MathML. Solo compatible con MathJax 2. Para MathJax version 3 se usara \c chtml en su lugar.)</translation>
    </message>
<message>
        <source>(This is the name for Mathjax version 3, for MathJax version 2 this will be translated into \c HTML-CSS)</source>
        <translation>(Este es el nombre para MathJax version 3, para MathJax version 2 se traducira a \c HTML-CSS)</translation>
    </message>
<message>
        <source>(210 x 297 mm)</source>
        <translation>(210 x 297 mm)</translation>
    </message>
<message>
        <source>(8.5 x 11 inches)</source>
        <translation>(8.5 x 11 pulgadas)</translation>
    </message>
<message>
        <source>(8.5 x 14 inches)</source>
        <translation>(8.5 x 14 pulgadas)</translation>
    </message>
<message>
        <source>(7.25 x 10.5 inches)</source>
        <translation>(7.25 x 10.5 pulgadas)</translation>
    </message>
<message>
        <source>same as ERROR_STOP</source>
        <translation>igual que ERROR_STOP</translation>
    </message>
<message>
        <source>same as BATCH</source>
        <translation>igual que BATCH</translation>
    </message>
<message>
        <source>In batch mode nothing is printed on the terminal, errors are scrolled as if \&lt;return\&gt; is hit at every error; missing files that TeX tries to input or request from keyboard input (\\read on a not open input stream) cause the job to abort</source>
        <translation>En modo batch no se imprime nada en la terminal, los errores se desplazan como si se presionara \&lt;return\&gt; en cada error; los archivos faltantes que TeX intenta leer o solicitar desde la entrada del teclado (\\read en un flujo de entrada no abierto) causan que el trabajo se aborte</translation>
    </message>
<message>
        <source>In nonstop mode the diagnostic message will appear on the terminal, but there is no possibility of user interaction just like in batch mode</source>
        <translation>En modo nonstop el mensaje de diagnostico aparecera en la terminal, pero no hay posibilidad de interaccion del usuario igual que en modo batch</translation>
    </message>
<message>
        <source>In scroll mode, TeX will stop only for missing files to input or if keyboard input is necessary</source>
        <translation>En modo scroll, TeX solo se detendra para archivos faltantes o si se necesita entrada del teclado</translation>
    </message>
<message>
        <source>In errorstop mode, TeX will stop at each error, asking for user intervention</source>
        <translation>En modo errorstop, TeX se detendra en cada error, solicitando intervencion del usuario</translation>
    </message>
<message>
        <source>(use client-side rendering for HTML and \c mmdc for LaTeX/PDF and other formats. If \c MERMAID_PATH is not set, non-HTML diagrams will produce a warning)</source>
        <translation>(usar renderizado del lado del cliente para HTML y \c mmdc para LaTeX/PDF y otros formatos. Si \c MERMAID_PATH no esta configurado, los diagramas no HTML produciran una advertencia)</translation>
    </message>
<message>
        <source>(use the \c mmdc tool to pre-generate images (requires \c Node.js and \c mermaid-js/mermaid-cli). Works for all output formats)</source>
        <translation>(usar la herramienta \c mmdc para pregenerar imagenes (requiere \c Node.js y \c mermaid-js/mermaid-cli). Funciona para todos los formatos de salida)</translation>
    </message>
<message>
        <source>(embed \c mermaid.js in HTML output for client-side rendering. Does not require \c mmdc but only works for HTML output)</source>
        <translation>(incrustar \c mermaid.js en la salida HTML para renderizado del lado del cliente. No requiere \c mmdc pero solo funciona para salida HTML)</translation>
    </message>
</context>
</TS>