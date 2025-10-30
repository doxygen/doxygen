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
	    COMMAND ${JAVACC_EXECUTABLE} ${JAVACC_FLAGS} -OUTPUT_DIRECTORY=${TOP}/vhdlparser ${TOP}/vhdlparser/vhdlparser.jj
            DEPENDS ${TOP}/vhdlparser/vhdlparser.jj
            OUTPUT ${TOP}/vhdlparser/CharStream.cc ${TOP}/vhdlparser/CharStream.h ${TOP}/vhdlparser/ErrorHandler.h ${TOP}/vhdlparser/ParseException.cc ${TOP}/vhdlparser/ParseException.h ${TOP}/vhdlparser/Token.cc ${TOP}/vhdlparser/Token.h ${TOP}/vhdlparser/TokenManager.h ${TOP}/vhdlparser/TokenMgrError.cc ${TOP}/vhdlparser/TokenMgrError.h ${TOP}/vhdlparser/VhdlParser.cc ${TOP}/vhdlparser/VhdlParser.h ${TOP}/vhdlparser/VhdlParserConstants.h ${TOP}/vhdlparser/VhdlParserTokenManager.cc ${TOP}/vhdlparser/VhdlParserTokenManager.h
        )
    endif()
endif()

add_custom_command(
    COMMAND ${Python_EXECUTABLE} ${TOP}/vhdlparser/vhdl_adj.py ${TOP}/vhdlparser/VhdlParser.cc ${GENERATED_SRC}/VhdlParser_adj.cc
    DEPENDS ${TOP}/vhdlparser/VhdlParser.cc ${TOP}/vhdlparser/vhdl_adj.py
    OUTPUT  ${GENERATED_SRC}/VhdlParser_adj.cc
)
set_source_files_properties(${GENERATED_SRC}/VhdlParser_adj.cc PROPERTIES GENERATED 1)

add_custom_command(
    COMMAND ${Python_EXECUTABLE} ${TOP}/vhdlparser/vhdl_adj.py ${TOP}/vhdlparser/VhdlParserTokenManager.cc ${GENERATED_SRC}/VhdlParserTokenManager_adj.cc
    DEPENDS ${TOP}/vhdlparser/VhdlParserTokenManager.cc ${TOP}/vhdlparser/vhdl_adj.py
    OUTPUT  ${GENERATED_SRC}/VhdlParserTokenManager_adj.cc
)
set_source_files_properties(${GENERATED_SRC}/VhdlParserTokenManager_adj.cc PROPERTIES GENERATED 1)

include_directories(
	${TOP}/src
	${TOP}/vhdlparser
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
