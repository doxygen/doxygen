#
# Generate parser (default target)
#
# when generating the parser with debug options it will look like:
#   make JAVACC_FLAGS=-debug_parser
#  or
#   make JAVACC_FLAGS="-debug_parser -debug_lookahead"
#
# Available debug options:
#  -debug_parser
#  -debug_token_manager
#  -debug_lookahead
#
# For other javacc settings / options consult the documentation of javacc.

find_package(Javacc)
if (JAVACC_FOUND)
    if (JAVACC_VERSION VERSION_LESS 7.0.6)
        message(STATUS "  Doxygen requires at least JavaCC version 7.0.6 (installed: ${JAVACC_VERSION})")
	message(STATUS "  Fall back to JavaCC not installed, using existing files.")
    else()
        add_custom_command(
	    COMMAND ${JAVACC_EXECUTABLE} ${JAVACC_FLAGS} -OUTPUT_DIRECTORY=${PROJECT_SOURCE_DIR}/vhdlparser ${PROJECT_SOURCE_DIR}/vhdlparser/vhdlparser.jj
            DEPENDS ${PROJECT_SOURCE_DIR}/vhdlparser/vhdlparser.jj
            OUTPUT ${PROJECT_SOURCE_DIR}/vhdlparser/CharStream.cc ${PROJECT_SOURCE_DIR}/vhdlparser/CharStream.h ${PROJECT_SOURCE_DIR}/vhdlparser/ErrorHandler.h ${PROJECT_SOURCE_DIR}/vhdlparser/ParseException.cc ${PROJECT_SOURCE_DIR}/vhdlparser/ParseException.h ${PROJECT_SOURCE_DIR}/vhdlparser/Token.cc ${PROJECT_SOURCE_DIR}/vhdlparser/Token.h ${PROJECT_SOURCE_DIR}/vhdlparser/TokenManager.h ${PROJECT_SOURCE_DIR}/vhdlparser/TokenMgrError.cc ${PROJECT_SOURCE_DIR}/vhdlparser/TokenMgrError.h ${PROJECT_SOURCE_DIR}/vhdlparser/VhdlParser.cc ${PROJECT_SOURCE_DIR}/vhdlparser/VhdlParser.h ${PROJECT_SOURCE_DIR}/vhdlparser/VhdlParserConstants.h ${PROJECT_SOURCE_DIR}/vhdlparser/VhdlParserTokenManager.cc ${PROJECT_SOURCE_DIR}/vhdlparser/VhdlParserTokenManager.h
        )
    endif()
endif()

add_custom_command(
    COMMAND ${Python_EXECUTABLE} ${PROJECT_SOURCE_DIR}/vhdlparser/vhdl_adj.py ${PROJECT_SOURCE_DIR}/vhdlparser/VhdlParser.cc ${GENERATED_SRC}/VhdlParser_adj.cc
    DEPENDS ${PROJECT_SOURCE_DIR}/vhdlparser/VhdlParser.cc ${PROJECT_SOURCE_DIR}/vhdlparser/vhdl_adj.py
    OUTPUT  ${GENERATED_SRC}/VhdlParser_adj.cc
)
set_source_files_properties(${GENERATED_SRC}/VhdlParser_adj.cc PROPERTIES GENERATED 1)

add_custom_command(
    COMMAND ${Python_EXECUTABLE} ${PROJECT_SOURCE_DIR}/vhdlparser/vhdl_adj.py ${PROJECT_SOURCE_DIR}/vhdlparser/VhdlParserTokenManager.cc ${GENERATED_SRC}/VhdlParserTokenManager_adj.cc
    DEPENDS ${PROJECT_SOURCE_DIR}/vhdlparser/VhdlParserTokenManager.cc ${PROJECT_SOURCE_DIR}/vhdlparser/vhdl_adj.py
    OUTPUT  ${GENERATED_SRC}/VhdlParserTokenManager_adj.cc
)
set_source_files_properties(${GENERATED_SRC}/VhdlParserTokenManager_adj.cc PROPERTIES GENERATED 1)

include_directories(
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/vhdlparser
	${GENERATED_SRC})
add_library(vhdlparser STATIC
CharStream.cc
ParseException.cc
Token.cc
TokenMgrError.cc
${GENERATED_SRC}/VhdlParser_adj.cc
${GENERATED_SRC}/VhdlParserTokenManager_adj.cc
)
add_dependencies(vhdlparser
    generate_configvalues_header
)
