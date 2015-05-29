include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/qtools)
add_library(vhdlparser
CharStream.cc
ParseException.cc
Token.cc
TokenMgrError.cc
VhdlParser.cc
VhdlParserTokenManager.cc
VhdlParserIF.cpp
)
