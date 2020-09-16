find_package(Iconv)

include_directories(
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/libversion
	${GENERATED_SRC}
	${PROJECT_SOURCE_DIR}/qtools
	${ICONV_INCLUDE_DIR}
	${CLANG_INCLUDEDIR}
)

add_executable(doxyparse
doxyparse.cpp
)
add_sanitizers(doxyparse)

if (use_libclang)
    if (static_libclang)
        set(CLANG_LIBS libclang clangTooling ${llvm_libs})
    else()
        set(CLANG_LIBS libclang clang-cpp ${llvm_libs})
    endif()
endif()

target_link_libraries(doxyparse
doxymain
qtools
md5
lodepng
mscgen
doxygen_version
doxycfg
vhdlparser
${ICONV_LIBRARIES}
${CMAKE_THREAD_LIBS_INIT}
${SQLITE3_LIBRARIES}
${EXTRA_LIBS}
${CLANG_LIBS}
${COVERAGE_LINKER_FLAGS}
)

install(TARGETS doxyparse DESTINATION bin)
