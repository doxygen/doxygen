if (build_parse)

find_package(Iconv)

include_directories(
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_SOURCE_DIR}/qtools
	${ICONV_INCLUDE_DIR}
)

add_executable(doxyparse
doxyparse.cpp
)
target_link_libraries(doxyparse
_doxygen
qtools
md5
doxycfg
vhdlparser
${ICONV_LIBRARIES}
${CMAKE_THREAD_LIBS_INIT}
${SQLITE3_LIBRARIES}
${EXTRA_LIBS}
)

install(TARGETS doxyparse DESTINATION bin)

endif()
