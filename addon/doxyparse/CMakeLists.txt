find_package(Iconv)

include_directories(
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/libversion
	${GENERATED_SRC}
	${Iconv_INCLUDE_DIRS}
	${CLANG_INCLUDEDIR}
)

add_executable(doxyparse
doxyparse.cpp
${PROJECT_SOURCE_DIR}/templates/icon/doxygen.rc
)

include(ApplyEditbin)
apply_editbin(doxyparse console)
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
md5
sqlite3
xml
lodepng
mscgen
doxygen_version
doxycfg
vhdlparser
${Iconv_LIBRARIES}
${CMAKE_THREAD_LIBS_INIT}
${SQLITE3_LIBRARIES}
${EXTRA_LIBS}
${CLANG_LIBS}
${COVERAGE_LINKER_FLAGS}
)

install(TARGETS doxyparse DESTINATION bin)
