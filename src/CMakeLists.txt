# vim:ts=4:sw=4:expandtab:autoindent:

include_directories(
    ${CMAKE_SOURCE_DIR}/qtools
    ${CMAKE_SOURCE_DIR}/libmd5
    ${CMAKE_SOURCE_DIR}/liblodepng
    ${CMAKE_SOURCE_DIR}/libmscgen
    ${CMAKE_SOURCE_DIR}/libversion
    ${CMAKE_SOURCE_DIR}/vhdlparser
    ${CMAKE_SOURCE_DIR}/src
    ${CLANG_INCLUDEDIR}
    ${GENERATED_SRC}
)


file(MAKE_DIRECTORY ${GENERATED_SRC})
file(GLOB LANGUAGE_FILES "${CMAKE_CURRENT_LIST_DIR}/translator_??.h")

# instead of increasebuffer.py
add_definitions(-DYY_BUF_SIZE=${enlarge_lex_buffers} -DYY_READ_BUF_SIZE=${enlarge_lex_buffers})

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


# configvalues.h
add_custom_command(
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/configgen.py -maph ${CMAKE_CURRENT_LIST_DIR}/config.xml > ${GENERATED_SRC}/configvalues.h
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/config.xml ${CMAKE_CURRENT_LIST_DIR}/configgen.py
    OUTPUT ${GENERATED_SRC}/configvalues.h
)
set_source_files_properties(${GENERATED_SRC}/configvalues.h PROPERTIES GENERATED 1)
add_custom_target(
    generate_configvalues_header
    DEPENDS ${GENERATED_SRC}/configvalues.h
)

# configvalues.cpp
add_custom_command(
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/configgen.py -maps ${CMAKE_CURRENT_LIST_DIR}/config.xml > ${GENERATED_SRC}/configvalues.cpp
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/config.xml ${CMAKE_CURRENT_LIST_DIR}/configgen.py
    OUTPUT ${GENERATED_SRC}/configvalues.cpp
)
set_source_files_properties(${GENERATED_SRC}/configvalues.cpp PROPERTIES GENERATED 1)

# configoptions.cpp
add_custom_command(
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/configgen.py -cpp ${CMAKE_CURRENT_LIST_DIR}/config.xml > ${GENERATED_SRC}/configoptions.cpp
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/config.xml ${CMAKE_CURRENT_LIST_DIR}/configgen.py
    OUTPUT ${GENERATED_SRC}/configoptions.cpp
)
set_source_files_properties(${GENERATED_SRC}/configoptions.cpp PROPERTIES GENERATED 1)


# ce_parse.h
add_custom_command(
    COMMAND ${BISON_EXECUTABLE} -l -d -p ce_parsexpYY ${CMAKE_CURRENT_LIST_DIR}/constexp.y -o ce_parse.c
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/constexp.y
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
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/res2cc_cmd.py ${CMAKE_SOURCE_DIR}/templates ${GENERATED_SRC}/resources.cpp
    DEPENDS ${RESOURCES}
    OUTPUT ${GENERATED_SRC}/resources.cpp
)
set_source_files_properties(${GENERATED_SRC}/resources.cpp PROPERTIES GENERATED 1)

# layout_default.xml
add_custom_command(
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/to_c_cmd.py < ${CMAKE_CURRENT_LIST_DIR}/layout_default.xml > ${GENERATED_SRC}/layout_default.xml.h
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/layout_default.xml
    OUTPUT  ${GENERATED_SRC}/layout_default.xml.h
)
set_source_files_properties(${GENERATED_SRC}/layout_default.xml.h PROPERTIES GENERATED 1)

set(LEX_FILES scanner 
    code
    pyscanner
    pycode
    fortranscanner
    fortrancode
    vhdlcode
    pre
    declinfo
    defargs
    doctokenizer
    commentcnv
    commentscan
    constexp
    xmlcode
    sqlcode
    configimpl)

# unfortunately ${LEX_FILES_H} and ${LEX_FILES_CPP} don't work in older versions of CMake (like 3.6.2) for add_library
foreach(lex_file ${LEX_FILES})
    set(LEX_FILES_H ${LEX_FILES_H} " " ${GENERATED_SRC}/${lex_file}.l.h CACHE INTERNAL "Stores generated files")
    set(LEX_FILES_CPP ${LEX_FILES_CPP} " " ${GENERATED_SRC}/${lex_file}.cpp CACHE INTERNAL "Stores generated files")
    add_custom_command(
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/scan_states.py ${CMAKE_CURRENT_LIST_DIR}/${lex_file}.l > ${GENERATED_SRC}/${lex_file}.l.h
        DEPENDS ${CMAKE_CURRENT_LIST_DIR}/scan_states.py ${CMAKE_CURRENT_LIST_DIR}/${lex_file}.l
        OUTPUT  ${GENERATED_SRC}/${lex_file}.l.h
    )
    set_source_files_properties(${GENERATED_SRC}/${lex_file}.l.h PROPERTIES GENERATED 1)
    # for code coverage we need the flex sources in the build src directory
    add_custom_command(
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/${lex_file}.l ${CMAKE_BINARY_DIR}/src/${lex_file}.l
        DEPENDS ${CMAKE_SOURCE_DIR}/src/${lex_file}.l
        OUTPUT  ${CMAKE_BINARY_DIR}/src/${lex_file}.l
    )

    FLEX_TARGET(${lex_file}
                ${CMAKE_CURRENT_LIST_DIR}/${lex_file}.l
                ${GENERATED_SRC}/${lex_file}.cpp
                COMPILE_FLAGS "${LEX_FLAGS}")
endforeach()


BISON_TARGET(constexp
             ${CMAKE_CURRENT_LIST_DIR}/constexp.y
             ${GENERATED_SRC}/ce_parse.cpp
             COMPILE_FLAGS "${YACC_FLAGS}")

add_custom_command(
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/constexp.y ${CMAKE_BINARY_DIR}/src
    DEPENDS ${CMAKE_SOURCE_DIR}/src/constexp.y
    OUTPUT  ${CMAKE_BINARY_DIR}/src/constexp.y
)

add_library(doxycfg STATIC
    ${GENERATED_SRC}/lang_cfg.h
    ${GENERATED_SRC}/configvalues.h
    ${GENERATED_SRC}/configimpl.cpp
    ${GENERATED_SRC}/configimpl.l.h
    ${GENERATED_SRC}/configoptions.cpp
    ${GENERATED_SRC}/configvalues.cpp
    ${GENERATED_SRC}/settings.h
    portable.cpp
    portable_c.c
    ftextstream.cpp
    message.cpp
    debug.cpp
)
add_sanitizers(doxycfg)

add_library(doxymain STATIC
    # generated for/by flex/bison
    #${LEX_FILES_H} #unfortunately doesn't work in older versions of CMake (like 3.6.2)
    #${LEX_FILES_CPP} #unfortunately doesn't work in older versions of CMake (like 3.6.2)
    ${GENERATED_SRC}/code.l.h
    ${GENERATED_SRC}/commentcnv.l.h
    ${GENERATED_SRC}/commentscan.l.h
    ${GENERATED_SRC}/constexp.cpp
    ${GENERATED_SRC}/constexp.l.h
    ${GENERATED_SRC}/declinfo.l.h
    ${GENERATED_SRC}/defargs.l.h
    ${GENERATED_SRC}/doctokenizer.l.h
    ${GENERATED_SRC}/fortrancode.l.h
    ${GENERATED_SRC}/fortranscanner.l.h
    ${GENERATED_SRC}/pre.l.h
    ${GENERATED_SRC}/pycode.l.h
    ${GENERATED_SRC}/pyscanner.l.h
    ${GENERATED_SRC}/scanner.l.h
    ${GENERATED_SRC}/sqlcode.l.h
    ${GENERATED_SRC}/vhdlcode.l.h
    ${GENERATED_SRC}/xmlcode.l.h
    ${GENERATED_SRC}/code.cpp
    ${GENERATED_SRC}/commentcnv.cpp
    ${GENERATED_SRC}/commentscan.cpp
    ${GENERATED_SRC}/declinfo.cpp
    ${GENERATED_SRC}/defargs.cpp
    ${GENERATED_SRC}/doctokenizer.cpp
    ${GENERATED_SRC}/fortrancode.cpp
    ${GENERATED_SRC}/fortranscanner.cpp
    ${GENERATED_SRC}/pre.cpp
    ${GENERATED_SRC}/pycode.cpp
    ${GENERATED_SRC}/pyscanner.cpp
    ${GENERATED_SRC}/scanner.cpp
    ${GENERATED_SRC}/sqlcode.cpp
    ${GENERATED_SRC}/vhdlcode.cpp
    ${GENERATED_SRC}/xmlcode.cpp
    #
    ${GENERATED_SRC}/ce_parse.cpp
    # custom generated files
    ${GENERATED_SRC}/lang_cfg.h
    ${GENERATED_SRC}/layout_default.xml.h
    ${GENERATED_SRC}/ce_parse.h
    ${GENERATED_SRC}/resources.cpp
    #
    arguments.cpp
    cite.cpp
    clangparser.cpp
    classdef.cpp
    classlist.cpp
    cmdmapper.cpp
    condparser.cpp
    context.cpp
    cppvalue.cpp
    defgen.cpp
    definition.cpp
    dia.cpp
    diagram.cpp
    dirdef.cpp
    docbookgen.cpp
    docbookvisitor.cpp
    docgroup.cpp
    docparser.cpp
    docsets.cpp
    dot.cpp
    dotcallgraph.cpp
    dotclassgraph.cpp
    dotdirdeps.cpp
    dotfilepatcher.cpp
    dotgfxhierarchytable.cpp
    dotgraph.cpp
    dotgroupcollaboration.cpp
    dotincldepgraph.cpp
    dotlegendgraph.cpp
    dotnode.cpp
    dotrunner.cpp
    doxygen.cpp
    eclipsehelp.cpp
    emoji.cpp
    entry.cpp
    filedef.cpp
    fileparser.cpp
    formula.cpp
    ftvhelp.cpp
    groupdef.cpp
    htags.cpp
    htmldocvisitor.cpp
    htmlentity.cpp
    htmlgen.cpp
    htmlhelp.cpp
    image.cpp
    index.cpp
    language.cpp
    latexdocvisitor.cpp
    latexgen.cpp
    layout.cpp
    mandocvisitor.cpp
    mangen.cpp
    markdown.cpp
    memberdef.cpp
    membergroup.cpp
    memberlist.cpp
    msc.cpp
    namespacedef.cpp
    outputgen.cpp
    outputlist.cpp
    pagedef.cpp
    perlmodgen.cpp
    plantuml.cpp
    qhp.cpp
    qhpxmlwriter.cpp
    reflist.cpp
    resourcemgr.cpp
    rtfdocvisitor.cpp
    rtfgen.cpp
    rtfstyle.cpp
    searchindex.cpp
    sqlite3gen.cpp
    stlsupport.cpp
    tagreader.cpp
    template.cpp
    textdocvisitor.cpp
    tooltip.cpp
    util.cpp
    vhdldocgen.cpp
    vhdljjparser.cpp
    xmldocvisitor.cpp
    xmlgen.cpp
)
add_sanitizers(doxymain)

# LLVM/clang headers give a lot of warnings with -Wshadow and -Wcast-align so we disable them for
# the one file that includes them

if (NOT MSVC)
set_source_files_properties(clangparser.cpp PROPERTIES COMPILE_FLAGS "-Wno-shadow -Wno-cast-align")
endif()

##foreach(lex_file ${LEX_FILES})
##add_library(doxymain STATIC ${GENERATED_SRC}/${lex_file}.l.h)
##endforeach()

add_executable(doxygen
    main.cpp
)
add_sanitizers(doxygen)

if (use_libclang)
    find_package(LLVM REQUIRED CONFIG)
    find_package(Clang REQUIRED CONFIG)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        cmake_minimum_required(VERSION 3.1)
        target_compile_features(doxymain PRIVATE cxx_alignof)
        target_compile_features(doxygen PRIVATE cxx_alignof)
        target_compile_options(doxymain PRIVATE -stdlib=libc++)
        target_compile_options(doxygen PRIVATE -stdlib=libc++)
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(doxymain PRIVATE -std=c++11)
        target_compile_options(doxygen PRIVATE -std=c++11)
    endif()
    include_directories(${LLVM_INCLUDE_DIRS})
    add_definitions(${LLVM_DEFINITIONS})
    if (static_libclang)
        llvm_map_components_to_libnames(llvm_libs support core option)
        set(CLANG_LIBS libclang clangTooling ${llvm_libs})
    else() # dynamically linked version of clang
        llvm_map_components_to_libnames(llvm_libs support)
        set(CLANG_LIBS libclang clang-cpp ${llvm_libs})
    endif()
    target_compile_definitions(doxygen PRIVATE ${LLVM_DEFINITIONS})
endif()

target_link_libraries(doxygen
    doxymain
    doxycfg
    qtools
    md5
    lodepng
    mscgen
    doxygen_version
    vhdlparser
    ${SQLITE3_LIBRARIES}
    ${ICONV_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${EXTRA_LIBS}
    ${CLANG_LIBS}
    ${COVERAGE_LINKER_FLAGS}
    ${DOXYGEN_EXTRA_LINK_OPTIONS}
)

set_project_warnings(doxycfg)
set_project_warnings(doxymain)
set_project_warnings(doxygen)

set_project_coverage(qtools)
set_project_coverage(doxycfg)
set_project_coverage(doxymain)
set_project_coverage(doxygen)


install(TARGETS doxygen DESTINATION bin)

