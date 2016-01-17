# configvalues.h
add_custom_command(
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/src/configgen.py -maph ${CMAKE_SOURCE_DIR}/src/config.xml > ${GENERATED_SRC}/configvalues.h
    DEPENDS ${CMAKE_SOURCE_DIR}/src/config.xml ${CMAKE_SOURCE_DIR}/src/configgen.py
    OUTPUT ${GENERATED_SRC}/configvalues.h
)
set_source_files_properties(${GENERATED_SRC}/configvalues.h PROPERTIES GENERATED 1)

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/qtools ${GENERATED_SRC})
add_library(vhdlparser STATIC
${GENERATED_SRC}/configvalues.h
CharStream.cc
ParseException.cc
Token.cc
TokenMgrError.cc
VhdlParser.cc
VhdlParserTokenManager.cc
VhdlParserIF.cpp
)
