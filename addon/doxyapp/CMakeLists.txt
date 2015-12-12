if (build_app)

find_package(Iconv)

include_directories(
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_SOURCE_DIR}/qtools
	${ICONV_INCLUDE_DIR}
	${CLANG_INCLUDEDIR}
)

add_executable(doxyapp
doxyapp.cpp
)
target_link_libraries(doxyapp
_doxygen
qtools
md5
doxycfg
vhdlparser
${ICONV_LIBRARIES}
${CMAKE_THREAD_LIBS_INIT}
${SQLITE3_LIBRARIES}
${EXTRA_LIBS}
${CLANG_LIBS}
)

install(TARGETS doxyapp DESTINATION bin)

endif()
