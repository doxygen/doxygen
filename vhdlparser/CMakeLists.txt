include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/qtools ${GENERATED_SRC})
add_library(vhdlparser STATIC
CharStream.cc
ParseException.cc
Token.cc
TokenMgrError.cc
VhdlParser.cc
VhdlParserTokenManager.cc
VhdlParserIF.cpp
)
add_dependencies(vhdlparser
    generate_configvalues_header
)
