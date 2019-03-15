include_directories(
    ${CMAKE_SOURCE_DIR}/mscgen
)


add_library(mscgen STATIC
mscgen_adraw.c
mscgen_cmdparse.c
mscgen_gd_out.c
mscgen_language.cpp
${GENERATED_SRC}/mscgen_lexer.cpp
mscgen_main.c
mscgen_msc.c
mscgen_null_out.c
mscgen_ps_out.c
mscgen_safe.c
mscgen_svg_out.c
mscgen_usage.c
mscgen_utf8.c
)


FLEX_TARGET(mscgen_lexer        mscgen_lexer.l        ${GENERATED_SRC}/mscgen_lexer.cpp        COMPILE_FLAGS "${LEX_FLAGS}")


#${GENERATED_SRC}/mscgen_language.cpp
#BISON_TARGET(mscgen_language      mscgen_language.y       ${GENERATED_SRC}/mscgen_language.cpp       COMPILE_FLAGS "${YACC_FLAGS}")
