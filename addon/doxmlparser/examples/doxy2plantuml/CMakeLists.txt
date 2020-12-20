set(CMAKE_CXX_STANDARD 11)

include_directories(../../include)

add_executable(doxmlparser_doxy2plantuml main.cpp)


target_link_libraries(doxmlparser_doxy2plantuml doxmlparser qtools ${COVERAGE_LINKER_FLAGS})

