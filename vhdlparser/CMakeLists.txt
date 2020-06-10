find_package(Javacc)
if (JAVACC_FOUND)
    if (JAVACC_VERSION VERSION_LESS 7.0.5)
        message(STATUS "  Doxygen requires at least JavaCC version 7.0.5 (installed: ${JAVACC_VERSION})")
	message(STATUS "  Fall back to JavaCC not installed, using existing files.")
    else()

    add_custom_command(
        COMMAND ${JAVACC_EXECUTABLE} ${JAVACC_FLAGS} -OUTPUT_DIRECTORY=${CMAKE_SOURCE_DIR}/vhdlparser ${CMAKE_SOURCE_DIR}/vhdlparser/vhdlparser.jj
        DEPENDS ${CMAKE_SOURCE_DIR}/vhdlparser/vhdlparser.jj
        OUTPUT ${CMAKE_SOURCE_DIR}/vhdlparser/CharStream.cc ${CMAKE_SOURCE_DIR}/vhdlparser/CharStream.h ${CMAKE_SOURCE_DIR}/vhdlparser/ErrorHandler.h ${CMAKE_SOURCE_DIR}/vhdlparser/ParseException.cc ${CMAKE_SOURCE_DIR}/vhdlparser/ParseException.h ${CMAKE_SOURCE_DIR}/vhdlparser/Token.cc ${CMAKE_SOURCE_DIR}/vhdlparser/Token.h ${CMAKE_SOURCE_DIR}/vhdlparser/TokenManager.h ${CMAKE_SOURCE_DIR}/vhdlparser/TokenMgrError.cc ${CMAKE_SOURCE_DIR}/vhdlparser/TokenMgrError.h ${CMAKE_SOURCE_DIR}/vhdlparser/VhdlParser.cc ${CMAKE_SOURCE_DIR}/vhdlparser/VhdlParser.h ${CMAKE_SOURCE_DIR}/vhdlparser/VhdlParserConstants.h ${CMAKE_SOURCE_DIR}/vhdlparser/VhdlParserTokenManager.cc ${CMAKE_SOURCE_DIR}/vhdlparser/VhdlParserTokenManager.h
    )

    endif()
endif()

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/qtools ${GENERATED_SRC})
add_library(vhdlparser STATIC
CharStream.cc
ParseException.cc
Token.cc
TokenMgrError.cc
VhdlParser.cc
VhdlParserTokenManager.cc
)
add_dependencies(vhdlparser
    generate_configvalues_header
)
