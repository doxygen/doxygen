# vim:ts=4:sw=4:expandtab:autoindent:

include_directories(
    ${CMAKE_SOURCE_DIR}/qtools
    ${CMAKE_SOURCE_DIR}/libmd5
    ${CMAKE_SOURCE_DIR}/vhdlparser/
    ${CMAKE_SOURCE_DIR}/src
    ${CLANG_INCLUDEDIR}
    ${GENERATED_SRC}
)


file(MAKE_DIRECTORY ${GENERATED_SRC})
file(GLOB LANGUAGE_FILES "${CMAKE_SOURCE_DIR}/src/translator_??.h")

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

# configvalues.h
add_custom_command(
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/src/configgen.py -maph ${CMAKE_SOURCE_DIR}/src/config.xml > ${GENERATED_SRC}/configvalues.h
    DEPENDS ${CMAKE_SOURCE_DIR}/src/config.xml ${CMAKE_SOURCE_DIR}/src/configgen.py
    OUTPUT ${GENERATED_SRC}/configvalues.h
)
set_source_files_properties(${GENERATED_SRC}/configvalues.h PROPERTIES GENERATED 1)

# configvalues.cpp
add_custom_command(
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/src/configgen.py -maps ${CMAKE_SOURCE_DIR}/src/config.xml > ${GENERATED_SRC}/configvalues.cpp
    DEPENDS ${CMAKE_SOURCE_DIR}/src/config.xml ${CMAKE_SOURCE_DIR}/src/configgen.py
    OUTPUT ${GENERATED_SRC}/configvalues.cpp
)
set_source_files_properties(${GENERATED_SRC}/configvalues.cpp PROPERTIES GENERATED 1)

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
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/cmake/lang_cfg.cmake ${GENERATED_SRC}/lang_cfg.h ${LANG_CODES}
    DEPENDS ${LANGUAGE_FILES}
    OUTPUT ${GENERATED_SRC}/lang_cfg.h
)
set_source_files_properties(${GENERATED_SRC}/lang_cfg.h PROPERTIES GENERATED 1)

# all resource files
file(GLOB RESOURCES ${CMAKE_SOURCE_DIR}/templates/*/*)

# resources.cpp
add_custom_command(
    COMMENT  "Generating ${GENERATED_SRC}/resources.cpp"
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/src/res2cc_cmd.py ${CMAKE_SOURCE_DIR}/templates ${GENERATED_SRC}/resources.cpp
    DEPENDS ${RESOURCES}
    OUTPUT ${GENERATED_SRC}/resources.cpp
)
set_source_files_properties(${GENERATED_SRC}/resources.cpp PROPERTIES GENERATED 1)

# layout_default.xml
add_custom_command(
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/src/to_c_cmd.py < ${CMAKE_SOURCE_DIR}/src/layout_default.xml > ${GENERATED_SRC}/layout_default.xml.h
    DEPENDS ${CMAKE_SOURCE_DIR}/src/layout_default.xml
    OUTPUT  ${GENERATED_SRC}/layout_default.xml.h
)
set_source_files_properties(${GENERATED_SRC}/layout_default.xml.h PROPERTIES GENERATED 1)

# Targets for flex/bison   generated files
FLEX_TARGET(scanner        scanner.l        ${GENERATED_SRC}/scanner.cpp        COMPILE_FLAGS "${LEX_FLAGS}")
FLEX_TARGET(code           code.l           ${GENERATED_SRC}/code.cpp           COMPILE_FLAGS "${LEX_FLAGS}")
FLEX_TARGET(pyscanner      pyscanner.l      ${GENERATED_SRC}/pyscanner.cpp      COMPILE_FLAGS "${LEX_FLAGS}")
FLEX_TARGET(pycode         pycode.l         ${GENERATED_SRC}/pycode.cpp         COMPILE_FLAGS "${LEX_FLAGS}")
FLEX_TARGET(fortranscanner fortranscanner.l ${GENERATED_SRC}/fortranscanner.cpp COMPILE_FLAGS "${LEX_FLAGS}")
FLEX_TARGET(fortrancode    fortrancode.l    ${GENERATED_SRC}/fortrancode.cpp    COMPILE_FLAGS "${LEX_FLAGS}")
FLEX_TARGET(vhdlcode       vhdlcode.l       ${GENERATED_SRC}/vhdlcode.cpp       COMPILE_FLAGS "${LEX_FLAGS}")
FLEX_TARGET(tclscanner     tclscanner.l     ${GENERATED_SRC}/tclscanner.cpp     COMPILE_FLAGS "${LEX_FLAGS}")
FLEX_TARGET(pre            pre.l            ${GENERATED_SRC}/pre.cpp            COMPILE_FLAGS "${LEX_FLAGS}")
FLEX_TARGET(declinfo       declinfo.l       ${GENERATED_SRC}/declinfo.cpp       COMPILE_FLAGS "${LEX_FLAGS}")
FLEX_TARGET(defargs        defargs.l        ${GENERATED_SRC}/defargs.cpp        COMPILE_FLAGS "${LEX_FLAGS}")
FLEX_TARGET(doctokenizer   doctokenizer.l   ${GENERATED_SRC}/doctokenizer.cpp   COMPILE_FLAGS "${LEX_FLAGS}")
FLEX_TARGET(commentcnv     commentcnv.l     ${GENERATED_SRC}/commentcnv.cpp     COMPILE_FLAGS "${LEX_FLAGS}")
FLEX_TARGET(commentscan    commentscan.l    ${GENERATED_SRC}/commentscan.cpp    COMPILE_FLAGS "${LEX_FLAGS}")
FLEX_TARGET(constexp       constexp.l       ${GENERATED_SRC}/constexp.cpp       COMPILE_FLAGS "${LEX_FLAGS}")
FLEX_TARGET(xmlcode        xmlcode.l        ${GENERATED_SRC}/xmlcode.cpp        COMPILE_FLAGS "${LEX_FLAGS}")
FLEX_TARGET(sqlcode        sqlcode.l        ${GENERATED_SRC}/sqlcode.cpp        COMPILE_FLAGS "${LEX_FLAGS}")
FLEX_TARGET(configimpl     configimpl.l     ${GENERATED_SRC}/configimpl.cpp     COMPILE_FLAGS "${LEX_FLAGS}")

BISON_TARGET(constexp      constexp.y       ${GENERATED_SRC}/ce_parse.cpp       COMPILE_FLAGS "${YACC_FLAGS}")

add_library(doxycfg STATIC
    ${GENERATED_SRC}/lang_cfg.h
    ${GENERATED_SRC}/configvalues.h
    ${GENERATED_SRC}/configimpl.cpp
    ${GENERATED_SRC}/configoptions.cpp
    ${GENERATED_SRC}/configvalues.cpp
    portable.cpp
    portable_c.c
)

add_library(_doxygen STATIC
    # custom generated files
    ${GENERATED_SRC}/lang_cfg.h
    ${GENERATED_SRC}/settings.h
    ${GENERATED_SRC}/layout_default.xml.h
    ${GENERATED_SRC}/version.cpp
    ${GENERATED_SRC}/ce_parse.h
    ${GENERATED_SRC}/configvalues.h
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
    ${GENERATED_SRC}/sqlcode.cpp
    #
    ${GENERATED_SRC}/ce_parse.cpp
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
    emoji.cpp
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
    mandocvisitor.cpp
    mangen.cpp
    sqlite3gen.cpp
    markdown.cpp
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

if (use_libclang)
    find_package(LLVM REQUIRED CONFIG)
    find_package(Clang REQUIRED CONFIG)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        cmake_minimum_required(VERSION 3.1)
        target_compile_features(_doxygen PRIVATE cxx_alignof)
        target_compile_features(doxygen PRIVATE cxx_alignof)
        target_compile_options(_doxygen PRIVATE -stdlib=libc++)
        target_compile_options(doxygen PRIVATE -stdlib=libc++)
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(_doxygen PRIVATE -std=c++11)
        target_compile_options(doxygen PRIVATE -std=c++11)
    endif()
    include_directories(${LLVM_INCLUDE_DIRS})
    add_definitions(${LLVM_DEFINITIONS})
    llvm_map_components_to_libnames(llvm_libs support core option)
    target_compile_definitions(doxygen PRIVATE ${LLVM_DEFINITIONS})
    set(CLANG_LIBS libclang clangTooling ${llvm_libs})
endif()

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
    ${CLANG_LIBS}
)

install(TARGETS doxygen DESTINATION bin)
