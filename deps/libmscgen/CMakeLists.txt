include_directories(
    ${PROJECT_SOURCE_DIR}/deps/liblodepng
    ${PROJECT_SOURCE_DIR}/deps/libmscgen
    ${GENERATED_SRC}
)

set(LEX_FILES mscgen_lexer) 
foreach(lex_file ${LEX_FILES})
    add_custom_command(
        COMMAND ${Python_EXECUTABLE} ${PROJECT_SOURCE_DIR}/src/scan_states.py ${PROJECT_SOURCE_DIR}/deps/libmscgen/${lex_file}.l > ${GENERATED_SRC}/${lex_file}.l.h
        DEPENDS ${PROJECT_SOURCE_DIR}/src/scan_states.py ${PROJECT_SOURCE_DIR}/deps/libmscgen/${lex_file}.l
        OUTPUT  ${GENERATED_SRC}/${lex_file}.l.h
    )
    set_source_files_properties(${GENERATED_SRC}/${lex_file}.l.h PROPERTIES GENERATED 1)

    FLEX_TARGET(${lex_file}        ${lex_file}.l        ${GENERATED_SRC}/${lex_file}.cpp        COMPILE_FLAGS "${LEX_FLAGS}")
endforeach()

add_library(mscgen STATIC
gd.c
gd_security.c
gdfontt.c
gdtables.c
gd_color.c
gdfonts.c
gdhelpers.c
gd_lodepng.c
mscgen_adraw.c
mscgen_gd_out.c
mscgen_ps_out.c
mscgen_null_out.c
${GENERATED_SRC}/mscgen_language.cpp
${GENERATED_SRC}/mscgen_lexer.cpp
${GENERATED_SRC}/mscgen_lexer.l.h
mscgen_api.c
mscgen_msc.c
mscgen_safe.c
mscgen_svg_out.c
mscgen_usage.c
mscgen_utf8.c
)


BISON_TARGET(mscgen_language
             mscgen_language.y
             ${GENERATED_SRC}/mscgen_language.cpp
             COMPILE_FLAGS "${YACC_FLAGS}")
