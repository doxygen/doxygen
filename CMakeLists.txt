# vim:ts=4:sw=4:expandtab:autoindent:
#
# Copyright (C) 1997-2015 by Dimitri van Heesch.
#
# Permission to use, copy, modify, and distribute this software and its
# documentation under the terms of the GNU General Public License is hereby
# granted. No representations are made about the suitability of this software
# for any purpose. It is provided "as is" without express or implied warranty.
# See the GNU General Public License for more details.
#
# Documents produced by Doxygen are derivative works derived from the
# input used in their production; they are not affected by this license.

cmake_minimum_required(VERSION 3.14)
project(doxygen)

option("ENABLE_CLANG_TIDY" "Enable static analysis with clang-tidy" OFF)

option(build_wizard    "Build the GUI frontend for doxygen." OFF)
option(build_app       "Example showing how to embed doxygen in an application." OFF)
option(build_parse     "Parses source code and dumps the dependencies between the code elements." OFF)
option(build_search    "Build external search tools (doxysearch and doxyindexer)" OFF)
option(build_doc       "Build user manual (HTML and PDF)" OFF)
option(build_doc_chm   "Build user manual (CHM)" OFF)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  option(use_libc++  "Use libc++ as C++ standard library." ON)
endif()
option(use_libclang    "Add support for libclang parsing." OFF)
option(use_sys_spdlog  "Use system spdlog library instead of the one bundled." OFF)
option(use_sys_fmt     "Use system fmt library instead of the one bundled." OFF)
option(use_sys_sqlite3 "Use system sqlite3 library instead of the one bundled." OFF)
option(static_libclang "Link to a statically compiled version of LLVM/libclang." OFF)
option(static_libxapian "Link to a statically compiled version of libxapian." OFF)
option(win_static      "Link with /MT in stead of /MD on windows" OFF)
option(enable_console  "Enable that executables on Windows get the CONSOLE bit set for the doxywizard executable [development]"  OFF)
option(enable_coverage "Enable coverage reporting for gcc/clang [development]" OFF)
option(enable_tracing  "Enable tracing option in release builds [development]" OFF)
option(enable_lex_debug "Enable debugging info for lexical scanners in release builds [development]" OFF)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  if (CYGWIN OR MINGW OR NOT "${CMAKE_TOOLCHAIN_FILE}" STREQUAL "")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION FALSE)
  else()
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
  endif()
endif()

include(CheckCXXCompilerFlag)

set(force_qt CACHE INTERNAL "Forces doxywizard to build using the specified major version, this can be Qt5 or Qt6")
set_property(CACHE force_qt PROPERTY STRINGS OFF Qt6 Qt5)

set(enlarge_lex_buffers "262144" CACHE INTERNAL "Sets the lex input and read buffers to the specified size")

if(enable_coverage)
  if("${PROJECT_BINARY_DIR}" STREQUAL "${PROJECT_SOURCE_DIR}")
    message(FATAL_ERROR "Doxygen cannot be generated in-place, the build directory (${PROJECT_BINARY_DIR}) has to differ from the doxygen main directory (${PROJECT_SOURCE_DIR})\nPlease don't forget to remove the already created file 'CMakeCache.txt' and the directory 'CMakeFiles'!")
  endif()
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/Sanitizers")
include(version)

message(STATUS "Using Cmake version ${CMAKE_VERSION}")
if(${CMAKE_VERSION} VERSION_LESS "3.21.0")
  set(depfile_supported  "0" CACHE INTERNAL "DEPFILE is not supported")
else()
  set(depfile_supported  "1" CACHE INTERNAL "DEPFILE is supported")
endif()

set(clang    "0" CACHE INTERNAL "used in settings.h")

if(use_libclang)
  set(clang    "1" CACHE INTERNAL "used in settings.h")
  find_package(LLVM CONFIG REQUIRED)
  find_package(Clang CONFIG REQUIRED)
endif()
if(use_sys_fmt)
  find_package(fmt CONFIG REQUIRED)
endif()
if(use_sys_spdlog)
  find_package(spdlog CONFIG REQUIRED)
endif()
if(use_sys_sqlite3)
  find_package(SQLite3 REQUIRED)
endif()

if(NOT DEFINED MACOS_VERSION_MIN) # to allow overruling with -DMACOS_VERSION_MIN
  set(MACOS_VERSION_MIN 10.14)
  if(build_search AND CMAKE_SYSTEM MATCHES "Darwin")
    set(MACOS_VERSION_MIN 10.15) # needs std::filesystem
  endif()
  if(build_wizard AND CMAKE_SYSTEM MATCHES "Darwin")
    set(MACOS_VERSION_MIN 13.0) # needs Qt6
  endif()
endif()

# use C++17 standard for compiling (unless very new Clang is present)
if(
  (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" AND
  CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 17) OR
  (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND
  CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19)
)
  set(CMAKE_CXX_STANDARD 20)
else()
  set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

if(ENABLE_CLANG_TIDY)
  find_program(CLANG_TIDY clang-tidy)
  if(CLANG_TIDY)
    set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY})
  else()
    message(SEND_ERROR "clang-tidy requested but executable not found")
  endif()
endif()

# produce compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_SYSTEM MATCHES "Darwin")
  set(CMAKE_OSX_DEPLOYMENT_TARGET "${MACOS_VERSION_MIN}" CACHE STRING "Minimum OS X deployment version")
  set(CMAKE_CXX_FLAGS "-Wno-deprecated-register ${CMAKE_CXX_FLAGS}")
  set(CMAKE_C_FLAGS "-Wno-deprecated-register ${CMAKE_C_FLAGS}")
  find_library(CORESERVICES_LIB CoreServices)
  set(EXTRA_LIBS ${CORESERVICES_LIB})
endif()

check_cxx_source_compiles(
  "
  #include <algorithm>

  #if !defined(__clang__) || !defined(_LIBCPP_VERSION)
  # error \"This is not clang with libcxx by llvm\"
  #endif

  int main() {
    return 0;
  }
  "
  IS_CLANG_LIBCPP
  FAIL_REGEX "This is not clang with libcxx by llvm"
)

add_compile_definitions(
  # LLVM's clang in combination with libc++
  $<$<AND:$<CONFIG:Debug>,$<BOOL:${IS_CLANG_LIBCPP}>>:_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_DEBUG>
  # LLVM's clang or gcc in combination with libstdc++ (GNU)
  $<$<AND:$<CONFIG:Debug>,$<NOT:$<BOOL:${IS_CLANG_LIBCPP}>>>:_GLIBCXX_ASSERTIONS>
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSQLITE_OMIT_LOAD_EXTENSION=1")

# Use 64-bit off_t on 32-bit Linux
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_SIZEOF_VOID_P EQUAL 4)
  # ensure 64bit offsets are used for filesystem accesses for 32bit compilation
  add_compile_definitions(_FILE_OFFSET_BITS=64)
endif()

if(WIN32)
  if(MSVC)
    if(NOT ICONV_DIR)
      if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        list(APPEND CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/deps/iconv_winbuild/include" "${PROJECT_SOURCE_DIR}/deps/iconv_winbuild/x64")
      elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        list(APPEND CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/deps/iconv_winbuild/include" "${PROJECT_SOURCE_DIR}/deps/iconv_winbuild/x86")
      endif()
    else()
      list(APPEND CMAKE_PREFIX_PATH ${ICONV_DIR})
    endif()
    set(CMAKE_REQUIRED_DEFINITIONS "-DLIBICONV_STATIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj") # needed for language.cpp on 64bit
    add_definitions(-DLIBICONV_STATIC -D_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LARGEADDRESSAWARE")
  endif()
  if(CMAKE_GENERATOR MATCHES "NMake Makefiles")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
  endif()
endif()
if(CYGWIN OR MINGW)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og -Wa,-mbig-obj")

  if(CMAKE_BUILD_TYPE STREQUAL  "")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1")
  endif()
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wa,-mbig-obj")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wa,-mbig-obj")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wa,-mbig-obj")
endif()

if(WIN32 AND MSVC)
  # workaround for GitHub runner, see https://github.com/actions/runner-images/issues/10004
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR")
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
  if((CMAKE_GENERATOR MATCHES "MinGW Makefiles") OR
    (CMAKE_GENERATOR MATCHES "MSYS Makefiles") OR
    (CMAKE_GENERATOR MATCHES "Unix Makefiles"))

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og")

    if(CMAKE_BUILD_TYPE STREQUAL  "")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1")
    endif()
  endif()
endif()

# needed for JavaCC
if(CMAKE_CXX_STANDARD EQUAL 20)
  set(JAVA_CC_EXTRA_FLAGS "-DJAVACC_CHAR_TYPE=\"char8_t\"")
else()
  set(JAVA_CC_EXTRA_FLAGS "-DJAVACC_CHAR_TYPE=\"unsigned char\"")
endif()
set(CMAKE_CXX_FLAGS       "${CMAKE_CXX_FLAGS}       ${JAVA_CC_EXTRA_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${JAVA_CC_EXTRA_FLAGS}")

if(POLICY CMP0063)
  cmake_policy(SET CMP0063 NEW)
endif()

if(POLICY CMP0116)
  cmake_policy(SET CMP0116 NEW)
endif()

# when using mutrace comment the next 3 lines and uncomment the last 2
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)
#set(CMAKE_CXX_FLAGS       "${CMAKE_CXX_FLAGS}       -rdynamic")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -rdynamic")

if(CMAKE_GENERATOR MATCHES "Ninja")
  set(LEX_FLAGS )
  set(YACC_FLAGS )
  set(JAVACC_FLAGS )
else()
  set(LEX_FLAGS $(LEX_FLAGS))
  set(YACC_FLAGS $(YACC_FLAGS))
  set(JAVACC_FLAGS $(JAVACC_FLAGS))
endif()

find_program(DOT NAMES dot)
find_package(Python REQUIRED)
find_package(FLEX REQUIRED)
if(FLEX_VERSION VERSION_LESS 2.5.37)
  message(SEND_ERROR "Doxygen requires at least flex version 2.5.37 (installed: ${FLEX_VERSION})")
endif()
if(FLEX_VERSION VERSION_LESS 2.6.0)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Dregister=")
  if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_ALLOW_KEYWORD_MACROS=")
  endif()
endif()
find_package(BISON REQUIRED)
if(BISON_VERSION VERSION_LESS 2.7)
  message(SEND_ERROR "Doxygen requires at least bison version 2.7 (installed: ${BISON_VERSION})")
endif()
find_package(Threads)
find_package(Sanitizers)

if((CMAKE_BUILD_TYPE STREQUAL "Debug") OR enable_lex_debug)
  set(LEX_FLAGS "${LEX_FLAGS} -d")
endif()

find_package(Iconv REQUIRED)
include_directories(${Iconv_INCLUDE_DIRS})


#set(DOXYDOCS ${PROJECT_SOURCE_DIR}/doc CACHE INTERNAL "Path to doxygen docs")
set(DOXYDOCS ${PROJECT_BINARY_DIR}/doc)
set(ENV{DOXYGEN_DOCDIR} ${DOXYDOCS})
set(GENERATED_SRC "${PROJECT_BINARY_DIR}/generated_src" CACHE INTERNAL "Stores generated files")
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# place binaries for all build types in the same directory, so we know where to find it
# when running tests or generating docs
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${EXECUTABLE_OUTPUT_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${EXECUTABLE_OUTPUT_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${EXECUTABLE_OUTPUT_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${EXECUTABLE_OUTPUT_PATH})

if(win_static)
  set(CompilerFlags
      CMAKE_CXX_FLAGS
      CMAKE_CXX_FLAGS_DEBUG
      CMAKE_CXX_FLAGS_RELEASE
      CMAKE_CXX_FLAGS_MINSIZEREL
      CMAKE_CXX_FLAGS_RELWITHDEBINFO
      CMAKE_C_FLAGS
      CMAKE_C_FLAGS_DEBUG
      CMAKE_C_FLAGS_RELEASE
      CMAKE_C_FLAGS_MINSIZEREL
      CMAKE_C_FLAGS_RELWITHDEBINFO)
  foreach(CompilerFlag ${CompilerFlags})
    string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
  endforeach()
endif()

include(cmake/CompilerWarnings.cmake)
include(cmake/Coverage.cmake)
include(cmake/WindowsEncoding.cmake)

add_subdirectory(deps)
add_subdirectory(libversion)
add_subdirectory(libxml)
add_subdirectory(vhdlparser)
add_subdirectory(src)

if(build_doc_chm)
  if(WIN32)
    find_package(HTMLHelp REQUIRED)
    set(build_doc ON)
  else()
    message(WARNING "CHM documentation generation not supported for this platform, ignoring setting.")
    set(build_doc_chm OFF)
  endif()
endif()

# always parse doc directory to at least install man pages
add_subdirectory(doc)
if(build_doc)
  add_subdirectory(examples)
endif()

add_subdirectory(doc_internal)

find_package(generateDS)
set(update_doxmlparser_dependency "")
if(GENERATEDS_FOUND)
  set(update_doxmlparser_dependency "update_doxmlparser_files")
endif()
add_subdirectory(addon)

enable_testing()
add_subdirectory(testing)

include(cmake/packaging.cmake) # set CPACK_xxxx properties
include(CPack)
