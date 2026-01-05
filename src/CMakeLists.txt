# vim:ts=4:sw=4:expandtab:autoindent:

include_directories(
    ${PROJECT_SOURCE_DIR}/deps/TinyDeflate
    ${PROJECT_SOURCE_DIR}/deps/filesystem
    ${PROJECT_SOURCE_DIR}/deps/libmd5
    ${PROJECT_SOURCE_DIR}/deps/liblodepng
    ${PROJECT_SOURCE_DIR}/deps/libmscgen
    ${PROJECT_SOURCE_DIR}/libversion
    ${PROJECT_SOURCE_DIR}/libxml
    ${PROJECT_SOURCE_DIR}/vhdlparser
    ${PROJECT_SOURCE_DIR}/src
    ${CLANG_INCLUDEDIR}
    ${GENERATED_SRC}
)

if (NOT use_sys_spdlog)
    include_directories(${PROJECT_SOURCE_DIR}/deps/spdlog/include)
endif()
if (NOT use_sys_fmt)
    include_directories(${PROJECT_SOURCE_DIR}/deps/fmt/include)
endif()
if (NOT use_sys_sqlite3)
    include_directories(${PROJECT_SOURCE_DIR}/deps/sqlite3)
endif()


file(MAKE_DIRECTORY ${GENERATED_SRC})
file(GLOB LANGUAGE_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/translator_??.h")

# instead of increasebuffer.py
add_definitions(-DYY_BUF_SIZE=${enlarge_lex_buffers} -DYY_READ_BUF_SIZE=${enlarge_lex_buffers})

# generate settings.h
file(GENERATE OUTPUT ${GENERATED_SRC}/settings.h
CONTENT "#ifndef SETTINGS_H
#define SETTINGS_H
#define USE_LIBCLANG ${clang}
#define IS_SUPPORTED(x) \\
  ( \\
   (USE_LIBCLANG && strcmp(\"USE_LIBCLANG\",(x))==0) || \\
  0)
#endif" )
set_source_files_properties(${GENERATED_SRC}/settings.h PROPERTIES GENERATED 1)


# configvalues.h
add_custom_command(
    COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/configgen.py -maph ${CMAKE_CURRENT_LIST_DIR}/config.xml > ${GENERATED_SRC}/configvalues.h
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
    COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/configgen.py -maps ${CMAKE_CURRENT_LIST_DIR}/config.xml > ${GENERATED_SRC}/configvalues.cpp
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/config.xml ${CMAKE_CURRENT_LIST_DIR}/configgen.py
    OUTPUT ${GENERATED_SRC}/configvalues.cpp
)
set_source_files_properties(${GENERATED_SRC}/configvalues.cpp PROPERTIES GENERATED 1)

# configoptions.cpp
add_custom_command(
    COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/configgen.py -cpp ${CMAKE_CURRENT_LIST_DIR}/config.xml > ${GENERATED_SRC}/configoptions.cpp
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/config.xml ${CMAKE_CURRENT_LIST_DIR}/configgen.py
    OUTPUT ${GENERATED_SRC}/configoptions.cpp
)
set_source_files_properties(${GENERATED_SRC}/configoptions.cpp PROPERTIES GENERATED 1)


# all resource files
file(GLOB RESOURCES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/templates/*/*)

# resources.cpp
add_custom_command(
    COMMENT  "Generating ${GENERATED_SRC}/resources.cpp"
    COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/res2cc_cmd.py ${PROJECT_SOURCE_DIR}/templates ${GENERATED_SRC}/resources.cpp
    DEPENDS ${RESOURCES} ${CMAKE_CURRENT_LIST_DIR}/res2cc_cmd.py
    OUTPUT ${GENERATED_SRC}/resources.cpp
)
set_source_files_properties(${GENERATED_SRC}/resources.cpp PROPERTIES GENERATED 1)

set(LEX_FILES
    code
    commentcnv
    commentscan
    configimpl
    constexp
    declinfo
    defargs
    doctokenizer
    fortrancode
    fortranscanner
    lexcode
    lexscanner
    pre
    pycode
    pyscanner
    scanner
    sqlcode
    vhdlcode
    xmlcode
)

if (NOT depfile_supported)
    # In case the DEPFILE possibility is not supported the complete list of lex include files for the dependency has to be used
    set(LEX_INC_FILES)
endif()

set(LEX_FILES_H)
set(LEX_FILES_CPP)
set(_depfile_args)

foreach(lex_file ${LEX_FILES})
    # configimpl is handled specially (it's in the static lib)
    if (NOT lex_file STREQUAL "configimpl")
        list(APPEND LEX_FILES_H ${GENERATED_SRC}/${lex_file}.l.h)
        list(APPEND LEX_FILES_CPP ${GENERATED_SRC}/${lex_file}.cpp)
    endif()

    set(_generated_files
        ${GENERATED_SRC}/${lex_file}.l
        ${GENERATED_SRC}/${lex_file}.corr
        ${GENERATED_SRC}/${lex_file}.d)

    if (depfile_supported)
        set(_depfile_args DEPFILE ${GENERATED_SRC}/${lex_file}.d)
    endif()
    add_custom_command(
        OUTPUT ${_generated_files}
        COMMAND ${Python_EXECUTABLE}
            ${CMAKE_CURRENT_LIST_DIR}/pre_lex.py
            ${CMAKE_CURRENT_LIST_DIR}/${lex_file}.l
            ${_generated_files}
            ${CMAKE_CURRENT_LIST_DIR}
        DEPENDS
            ${CMAKE_CURRENT_LIST_DIR}/pre_lex.py
            ${CMAKE_CURRENT_LIST_DIR}/${lex_file}.l
        ${_depfile_args}
    )
    set_source_files_properties(${_generated_files} PROPERTIES GENERATED 1)

    add_custom_command(
        COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/scan_states.py ${GENERATED_SRC}/${lex_file}.l > ${GENERATED_SRC}/${lex_file}.l.h
        DEPENDS ${CMAKE_CURRENT_LIST_DIR}/scan_states.py ${GENERATED_SRC}/${lex_file}.l
        OUTPUT  ${GENERATED_SRC}/${lex_file}.l.h
    )
    set_source_files_properties(${GENERATED_SRC}/${lex_file}.l.h PROPERTIES GENERATED 1)

    if(enable_coverage)
        # for code coverage we need the flex sources in the build src directory
        add_custom_command(
            COMMAND ${CMAKE_COMMAND} -E copy ${GENERATED_SRC}/${lex_file}.l ${PROJECT_BINARY_DIR}/src/${lex_file}.l
            DEPENDS ${GENERATED_SRC}/${lex_file}.l
            OUTPUT  ${PROJECT_BINARY_DIR}/src/${lex_file}.l
        )
    endif()

    FLEX_TARGET(${lex_file}
                ${GENERATED_SRC}/${lex_file}.l
                ${GENERATED_SRC}/${lex_file}_intermediate.cpp
                COMPILE_FLAGS "${LEX_FLAGS}")
    add_custom_command(
        COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/post_lex.py ${GENERATED_SRC}/${lex_file}_intermediate.cpp ${GENERATED_SRC}/${lex_file}.cpp  ${GENERATED_SRC}/${lex_file}.corr ${CMAKE_CURRENT_LIST_DIR}/${lex_file}.l  ${GENERATED_SRC}/${lex_file}.l
        DEPENDS ${CMAKE_CURRENT_LIST_DIR}/post_lex.py ${GENERATED_SRC}/${lex_file}_intermediate.cpp ${GENERATED_SRC}/${lex_file}.corr
        OUTPUT  ${GENERATED_SRC}/${lex_file}.cpp
    )
endforeach()
unset(_depfile_args)
unset(_generated_files)

# Cache generated sources lists
set(LEX_FILES_H ${LEX_FILES_H} CACHE INTERNAL "Stores generated files")
set(LEX_FILES_CPP ${LEX_FILES_CPP} CACHE INTERNAL "Stores generated files")


BISON_TARGET(constexp
             ${CMAKE_CURRENT_LIST_DIR}/constexp.y
             ${GENERATED_SRC}/ce_parse.cpp
             COMPILE_FLAGS "${YACC_FLAGS}")

if(enable_coverage)
    add_custom_command(
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/src/constexp.y ${PROJECT_BINARY_DIR}/src
        DEPENDS ${PROJECT_SOURCE_DIR}/src/constexp.y
        OUTPUT  ${PROJECT_BINARY_DIR}/src/constexp.y
    )
endif()

add_library(doxycfg STATIC
    ${GENERATED_SRC}/configimpl.cpp
    ${GENERATED_SRC}/configimpl.l.h
    ${GENERATED_SRC}/configoptions.cpp
    ${GENERATED_SRC}/configvalues.cpp
    ${GENERATED_SRC}/settings.h
    portable.cpp
    portable_c.c
    message.cpp
    debug.cpp
    trace.cpp
)
add_dependencies(doxycfg generate_configvalues_header)

target_link_libraries(doxycfg PRIVATE
    spdlog::spdlog
)
add_sanitizers(doxycfg)

add_library(doxymain STATIC
    # generated for/by flex/bison
    ${LEX_FILES_H}
    ${LEX_FILES_CPP}
    #
    ${GENERATED_SRC}/ce_parse.cpp
    # custom generated files
    ${GENERATED_SRC}/ce_parse.hpp
    ${GENERATED_SRC}/resources.cpp
    #
    aliases.cpp
    anchor.cpp
    arguments.cpp
    cite.cpp
    clangparser.cpp
    classdef.cpp
    classlist.cpp
    cmdmapper.cpp
    codefragment.cpp
    conceptdef.cpp
    condparser.cpp
    cppvalue.cpp
    datetime.cpp
    defgen.cpp
    definition.cpp
    dia.cpp
    diagram.cpp
    dir.cpp
    dirdef.cpp
    docbookgen.cpp
    docbookvisitor.cpp
    docgroup.cpp
    docnode.cpp
    docparser.cpp
    docsets.cpp
    docvisitor.cpp
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
    fileinfo.cpp
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
    moduledef.cpp
    msc.cpp
    namespacedef.cpp
    outputgen.cpp
    outputlist.cpp
    pagedef.cpp
    perlmodgen.cpp
    plantuml.cpp
    qcstring.cpp
    qhp.cpp
    reflist.cpp
    regex.cpp
    requirement.cpp
    resourcemgr.cpp
    rtfdocvisitor.cpp
    rtfgen.cpp
    rtfstyle.cpp
    searchindex.cpp
    searchindex_js.cpp
    singlecomment.cpp
    sitemap.cpp
    sqlite3gen.cpp
    stlsupport.cpp
    symbolresolver.cpp
    tagreader.cpp
    textdocvisitor.cpp
    tooltip.cpp
    utf8.cpp
    util.cpp
    vhdldocgen.cpp
    vhdljjparser.cpp
    xmldocvisitor.cpp
    xmlgen.cpp
)
target_link_libraries(doxymain PRIVATE
    spdlog::spdlog
)
add_sanitizers(doxymain)
add_dependencies(doxymain generate_configvalues_header)

# LLVM/clang headers give a lot of warnings with -Wshadow and -Wcast-align so we disable them for
# the one file that includes them.
# for the -Wbitfield-enum-conversion warning issue https://github.com/llvm/llvm-project/issues/59840 was submitted

if (NOT MSVC)
set_source_files_properties(clangparser.cpp PROPERTIES COMPILE_FLAGS "-Wno-shadow -Wno-cast-align -Wno-bitfield-enum-conversion")
endif()

##foreach(lex_file ${LEX_FILES})
##add_library(doxymain STATIC ${GENERATED_SRC}/${lex_file}.l.h)
##endforeach()

add_executable(doxygen
    main.cpp
    ${PROJECT_SOURCE_DIR}/templates/icon/doxygen.rc
)

# enable to monitor compilation times
#set_property(TARGET doxymain PROPERTY RULE_LAUNCH_COMPILE "${CMAKE_COMMAND} -E time")

include(ApplyEditbin)
apply_editbin(doxygen console)

add_sanitizers(doxygen)

if (use_libclang)
    find_package(LLVM REQUIRED CONFIG)
    find_package(Clang REQUIRED CONFIG)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_features(doxymain PRIVATE cxx_alignof)
        target_compile_features(doxygen PRIVATE cxx_alignof)
        if (use_libc++)
            target_compile_options(doxymain PRIVATE -stdlib=libc++)
            target_compile_options(doxygen PRIVATE -stdlib=libc++)
        endif()
    endif()
    include_directories(${LLVM_INCLUDE_DIRS})
    add_definitions(${LLVM_DEFINITIONS})
    if (static_libclang)
        if (MSVC OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang"))
          set(CLANG_LIBS libclang clangTooling)
        else()
          set(CLANG_LIBS libclang_static clangTooling)
        endif()
        add_definitions(-DCINDEX_NO_EXPORTS)
    else() # dynamically linked version of clang
        llvm_config(doxymain USE_SHARED support)
        set(CLANG_LIBS libclang clang-cpp)
    endif()
    target_compile_definitions(doxygen PRIVATE ${LLVM_DEFINITIONS})
endif()

if((CMAKE_BUILD_TYPE STREQUAL "Debug") OR enable_tracing)
    target_compile_definitions(doxycfg  PRIVATE -DENABLE_TRACING=1)
    target_compile_definitions(doxymain PRIVATE -DENABLE_TRACING=1)
    target_compile_definitions(doxygen  PRIVATE -DENABLE_TRACING=1)
endif()

target_link_libraries(doxygen PRIVATE
    doxymain
    doxycfg
    md5
    sqlite3
    lodepng
    mscgen
    xml
    doxygen_version
    vhdlparser
    ${Iconv_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${EXTRA_LIBS}
    ${CLANG_LIBS}
    ${COVERAGE_LINKER_FLAGS}
    ${DOXYGEN_EXTRA_LINK_OPTIONS}
)

set_project_warnings(doxycfg)
set_project_warnings(doxymain)
set_project_warnings(doxygen)

set_project_coverage(doxycfg)
set_project_coverage(doxymain)
set_project_coverage(doxygen)

install(TARGETS doxygen DESTINATION bin)
