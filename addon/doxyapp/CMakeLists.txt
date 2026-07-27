find_package(Iconv)

include_directories(
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/libversion
	${GENERATED_SRC}
	${Iconv_INCLUDE_DIRS}
	${CLANG_INCLUDEDIR}
)

add_executable(doxyapp
doxyapp.cpp
${PROJECT_SOURCE_DIR}/templates/icon/doxygen.rc
)
include(ApplyEditbin)
apply_editbin(doxyapp console)
add_sanitizers(doxyapp)

if (use_libclang)
    find_package(LLVM REQUIRED CONFIG)
    find_package(Clang REQUIRED CONFIG)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_features(doxyapp PRIVATE cxx_alignof)
        if (use_libc++)
            target_compile_options(doxyapp PRIVATE -stdlib=libc++)
        endif()
    endif()
    include_directories(${LLVM_INCLUDE_DIRS})
    add_definitions(${LLVM_DEFINITIONS})
    if (static_libclang)
        set(CLANG_LIBS libclang clangTooling)
    else() # dynamically linked version of clang
        llvm_config(doxymain USE_SHARED support)
        set(CLANG_LIBS libclang clang-cpp)
    endif()
    target_compile_definitions(doxyapp PRIVATE ${LLVM_DEFINITIONS})
endif()

target_link_libraries(doxyapp
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

install(TARGETS doxyapp DESTINATION bin)
