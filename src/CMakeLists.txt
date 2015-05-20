# vim:ts=4:sw=4:expandtab:autoindent:

include_directories(
    ${CMAKE_SOURCE_DIR}/qtools
    ${CMAKE_SOURCE_DIR}/libmd5
    ${CMAKE_SOURCE_DIR}/vhdlparser/
    ${CMAKE_SOURCE_DIR}/src
    ${GENERATED_SRC}
)


file(MAKE_DIRECTORY ${GENERATED_SRC})

# instead of increasebuffer.py
add_definitions(-DYY_BUF_SIZE=262144 -DYY_READ_BUF_SIZE=262144)

# generate settings.h
file(GENERATE OUTPUT ${GENERATED_SRC}/settings.h
CONTENT "#ifndef SETTINGS_H
#define SETTINGS_H
#define USE_SQLITE3 ${sqlite3}
#define USE_LIBCLANG ${clang}
#define IS_SUPPORTED(x) \\
  ((USE_SQLITE3  && strcmp(\"USE_SQLITE3\",(x))==0)  || \\
   (USE_LIBCLANG && strcmp(\"USE_LIBCLANG\",(x))==0) || \\
  0)
#endif" )
set_source_files_properties(${GENERATED_SRC}/settings.h PROPERTIES GENERATED 1)


# generate version.cpp
file(GENERATE OUTPUT ${GENERATED_SRC}/version.cpp
    CONTENT "char versionString[]=\"${VERSION}\";"
)
set_source_files_properties(${GENERATED_SRC}/version.cpp PROPERTIES GENERATED 1)


# configoptions.cpp
add_custom_command(
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/src/configgen.py -cpp ${CMAKE_SOURCE_DIR}/src/config.xml > ${GENERATED_SRC}/configoptions.cpp
    DEPENDS ${CMAKE_SOURCE_DIR}/src/config.xml ${CMAKE_SOURCE_DIR}/src/configgen.py
    OUTPUT ${GENERATED_SRC}/configoptions.cpp
)
set_source_files_properties(${GENERATED_SRC}/configoptions.cpp PROPERTIES GENERATED 1)


# ce_parse.h
add_custom_command(
    COMMAND ${BISON_EXECUTABLE} -l -d -p ce_parsexpYY ${CMAKE_SOURCE_DIR}/src/constexp.y -o ce_parse.c
    DEPENDS ${CMAKE_SOURCE_DIR}/src/constexp.y
    OUTPUT ${GENERATED_SRC}/ce_parse.h
    WORKING_DIRECTORY ${GENERATED_SRC}
)
set_source_files_properties(${GENERATED_SRC}/ce_parse.h PROPERTIES GENERATED 1)


# lang_cfg.h
add_custom_command(
    COMMENT  "Generating ${GENERATED_SRC}/lang_cfg.h"
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/cmake/lang_cfg.cmake ENONLY 2> ${GENERATED_SRC}/lang_cfg.h
    OUTPUT ${GENERATED_SRC}/lang_cfg.h
)
set_source_files_properties(${GENERATED_SRC}/lang_cfg.h PROPERTIES GENERATED 1)


# resources.cpp
add_custom_command(
    COMMENT  "Generating ${GENERATED_SRC}/resources.cpp"
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/src/res2cc_cmd.py ${CMAKE_SOURCE_DIR}/templates ${GENERATED_SRC}/resources.cpp
    OUTPUT ${GENERATED_SRC}/resources.cpp
)
set_source_files_properties(${GENERATED_SRC}/resources.cpp PROPERTIES GENERATED 1)


#
function( to_c_cmd in_file out_file)
add_custom_command(
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/src/to_c_cmd.py < ${CMAKE_SOURCE_DIR}/templates/html/${in_file} > ${GENERATED_SRC}/${out_file}
    DEPENDS ${CMAKE_SOURCE_DIR}/templates/html/${in_file}
    OUTPUT ${GENERATED_SRC}/${out_file}
)
set_source_files_properties(${GENERATED_SRC}/${out_file} PROPERTIES GENERATED 1)
endfunction()


# Targets for flex/bison generated files
FLEX_TARGET(scanner     scanner.l     ${GENERATED_SRC}/scanner.cpp      COMPILE_FLAGS "-PscannerYY")
FLEX_TARGET(code        code.l        ${GENERATED_SRC}/code.cpp         COMPILE_FLAGS "-PcodeYY")
FLEX_TARGET(pyscanner   pyscanner.l   ${GENERATED_SRC}/pyscanner.cpp    COMPILE_FLAGS "-PpyscannerYY")
FLEX_TARGET(pycode      pycode.l      ${GENERATED_SRC}/pycode.cpp       COMPILE_FLAGS "-PpycodeYY")
FLEX_TARGET(fortranscanner fortranscanner.l ${GENERATED_SRC}/fortranscanner.cpp COMPILE_FLAGS "-i -PfortranscannerYY")
FLEX_TARGET(fortrancode fortrancode.l ${GENERATED_SRC}/fortrancode.cpp  COMPILE_FLAGS "-i -PfortrancodeYY")
FLEX_TARGET(vhdlcode    vhdlcode.l    ${GENERATED_SRC}/vhdlcode.cpp     COMPILE_FLAGS "-i -PvhdlcodeYY")
FLEX_TARGET(tclscanner  tclscanner.l  ${GENERATED_SRC}/tclscanner.cpp   COMPILE_FLAGS "-i -PtclscannerYY")
FLEX_TARGET(pre         pre.l         ${GENERATED_SRC}/pre.cpp          COMPILE_FLAGS "-PpreYY")
FLEX_TARGET(declinfo    declinfo.l    ${GENERATED_SRC}/declinfo.cpp     COMPILE_FLAGS "-PdeclinfoYY")
FLEX_TARGET(defargs     defargs.l     ${GENERATED_SRC}/defargs.cpp      COMPILE_FLAGS "-PdefargsYY")
FLEX_TARGET(doctokenizer doctokenizer.l     ${GENERATED_SRC}/doctokenizer.cpp   COMPILE_FLAGS "-PdoctokenizerYY")
FLEX_TARGET(commentcnv  commentcnv.l  ${GENERATED_SRC}/commentcnv.cpp   COMPILE_FLAGS "-PcommentcnvYY")
FLEX_TARGET(commentscan commentscan.l ${GENERATED_SRC}/commentscan.cpp  COMPILE_FLAGS "-PcommentscanYY")
FLEX_TARGET(constexp    constexp.l    ${GENERATED_SRC}/constexp.cpp     COMPILE_FLAGS "-PconstexpYY")
FLEX_TARGET(xmlcode     xmlcode.l     ${GENERATED_SRC}/xmlcode.cpp      COMPILE_FLAGS "-PxmlcodeYY")

FLEX_TARGET(config      config.l      ${GENERATED_SRC}/config.cpp       COMPILE_FLAGS "-PconfigYY")

BISON_TARGET(vhdlparser vhdlparser.y  ${GENERATED_SRC}/vhdlparser.cpp   COMPILE_FLAGS "-l -p vhdlscannerYY")
BISON_TARGET(constexp   constexp.y    ${GENERATED_SRC}/ce_parse.cpp     COMPILE_FLAGS "-l -p constexpYY")


to_c_cmd(navtree.js navtree.js.h)
to_c_cmd(navtree.css navtree.css.h)
to_c_cmd(resize.js resize.js.h)
to_c_cmd(header.html header.html.h)
to_c_cmd(footer.html footer.html.h)
to_c_cmd(doxygen.css doxygen.css.h)
to_c_cmd(layout_default.xml layout_default.xml.h)
to_c_cmd(search_functions.php search_functions.php.h)
to_c_cmd(search_opensearch.php search_opensearch.php.h)
to_c_cmd(search.css search.css.h)
to_c_cmd(jquery.js jquery.js.h)
to_c_cmd(svgpan.js svgpan.js.h)
to_c_cmd(dynsections.js dynsections.js.h)
to_c_cmd(extsearch.js extsearch.js.h)
to_c_cmd(search.js search.js.h)

add_library(doxycfg
    ${GENERATED_SRC}/lang_cfg.h
    ${GENERATED_SRC}/config.cpp
    ${GENERATED_SRC}/configoptions.cpp
    portable.cpp
    portable_c.c
)

add_library(_doxygen
    # custom generated files
    ${GENERATED_SRC}/lang_cfg.h
    ${GENERATED_SRC}/settings.h
    ${GENERATED_SRC}/version.cpp
    ${GENERATED_SRC}/ce_parse.h
    ${GENERATED_SRC}/resources.cpp
    # generated by flex/bison
    ${GENERATED_SRC}/scanner.cpp
    ${GENERATED_SRC}/code.cpp
    ${GENERATED_SRC}/pyscanner.cpp
    ${GENERATED_SRC}/pycode.cpp
    ${GENERATED_SRC}/fortranscanner.cpp
    ${GENERATED_SRC}/fortrancode.cpp
    ${GENERATED_SRC}/vhdlcode.cpp
    ${GENERATED_SRC}/tclscanner.cpp
    ${GENERATED_SRC}/pre.cpp
    ${GENERATED_SRC}/declinfo.cpp
    ${GENERATED_SRC}/defargs.cpp
    ${GENERATED_SRC}/doctokenizer.cpp
    ${GENERATED_SRC}/commentcnv.cpp
    ${GENERATED_SRC}/commentscan.cpp
    ${GENERATED_SRC}/constexp.cpp
    ${GENERATED_SRC}/xmlcode.cpp
    #
    ${GENERATED_SRC}/ce_parse.cpp
    # generated by to_c_cmd
    ${GENERATED_SRC}/navtree.js.h
    ${GENERATED_SRC}/navtree.css.h
    ${GENERATED_SRC}/resize.js.h
    ${GENERATED_SRC}/header.html.h
    ${GENERATED_SRC}/footer.html.h
    ${GENERATED_SRC}/doxygen.css.h
    ${GENERATED_SRC}/layout_default.xml.h
    ${GENERATED_SRC}/search_functions.php.h
    ${GENERATED_SRC}/search_opensearch.php.h
    ${GENERATED_SRC}/search.css.h
    ${GENERATED_SRC}/jquery.js.h
    ${GENERATED_SRC}/svgpan.js.h
    ${GENERATED_SRC}/dynsections.js.h
    ${GENERATED_SRC}/extsearch.js.h
    ${GENERATED_SRC}/search.js.h
    #
    plantuml.cpp
    arguments.cpp
    cite.cpp
    clangparser.cpp
    fileparser.cpp
    classdef.cpp
    classlist.cpp
    cmdmapper.cpp
    condparser.cpp
    context.cpp
    cppvalue.cpp
    debug.cpp
    defgen.cpp
    define.cpp
    definition.cpp
    diagram.cpp
    dirdef.cpp
    docparser.cpp
    docsets.cpp
    dot.cpp
    doxygen.cpp
    eclipsehelp.cpp
    entry.cpp
    filedef.cpp
    filename.cpp
    formula.cpp
    ftextstream.cpp
    ftvhelp.cpp
    groupdef.cpp
    htags.cpp
    htmldocvisitor.cpp
    htmlentity.cpp
    resourcemgr.cpp
    htmlgen.cpp
    htmlhelp.cpp
    image.cpp
    index.cpp
    language.cpp
    latexdocvisitor.cpp
    latexgen.cpp
    layout.cpp
    lodepng.cpp
    logos.cpp
    mandocvisitor.cpp
    mangen.cpp
    sqlite3gen.cpp
    markdown.cpp
    marshal.cpp
    memberdef.cpp
    membergroup.cpp
    memberlist.cpp
    membername.cpp
    message.cpp
    msc.cpp
    dia.cpp
    namespacedef.cpp
    objcache.cpp
    outputgen.cpp
    outputlist.cpp
    pagedef.cpp
    perlmodgen.cpp
    qhp.cpp
    qhpxmlwriter.cpp
    reflist.cpp
    rtfdocvisitor.cpp
    rtfgen.cpp
    rtfstyle.cpp
    searchindex.cpp
    store.cpp
    tagreader.cpp
    template.cpp
    textdocvisitor.cpp
    tooltip.cpp
    util.cpp
    vhdldocgen.cpp
    vhdljjparser.cpp
    xmldocvisitor.cpp
    xmlgen.cpp
    docbookvisitor.cpp
    docbookgen.cpp
)

add_executable(doxygen main.cpp)
target_link_libraries(doxygen
    _doxygen
    doxycfg
    qtools
    md5
    vhdlparser
    ${SQLITE3_LIBRARIES}
    ${ICONV_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${EXTRA_LIBS}
)
