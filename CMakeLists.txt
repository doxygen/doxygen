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

cmake_minimum_required(VERSION 3.2)
project(doxygen)

option(build_wizard    "Build the GUI frontend for doxygen." OFF)
option(build_app       "Example showing how to embed doxygen in an application." OFF)
option(build_parse     "Parses source code and dumps the dependencies between the code elements." OFF)
option(build_xmlparser "Example showing how to parse doxygen's XML output." OFF)
option(build_search    "Build external search tools (doxysearch and doxyindexer)" OFF)
option(build_doc       "Build user manual" OFF)
option(use_sqlite3     "Add support for sqlite3 output [experimental]." OFF)
option(use_libclang    "Add support for libclang parsing." OFF)
option(win_static      "Link with /MT in stead of /MD on windows" OFF)
option(english_only    "Only compile in support for the English language" OFF)
option(force_qt4       "Forces doxywizard to build using Qt4 even if Qt5 is installed" OFF)

SET(enlarge_lex_buffers "262144" CACHE INTERNAL "Sets the lex input and read buffere to the specified size")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(TOP "${CMAKE_SOURCE_DIR}")
include(version)

set(sqlite3  "0" CACHE INTERNAL "used in settings.h")
set(clang    "0" CACHE INTERNAL "used in settings.h")
if (use_sqlite3)
	set(sqlite3  "1" CACHE INTERNAL "used in settings.h")
endif()

set(MACOS_VERSION_MIN 10.9)
if (use_libclang)
	set(clang    "1" CACHE INTERNAL "used in settings.h")
        find_package(LLVM CONFIG REQUIRED)
        find_package(Clang CONFIG REQUIRED)
    if (CMAKE_SYSTEM MATCHES "Darwin")
        set(MACOS_VERSION_MIN 10.11)
    endif()
endif()

# use C++11 standard for compiling (libclang option requires it)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

if (CMAKE_SYSTEM MATCHES "Darwin")
    set(CMAKE_CXX_FLAGS "-Wno-deprecated-register -mmacosx-version-min=${MACOS_VERSION_MIN} ${CMAKE_CXX_FLAGS}")
    set(CMAKE_C_FLAGS "-Wno-deprecated-register -mmacosx-version-min=${MACOS_VERSION_MIN} ${CMAKE_C_FLAGS}")
    find_library(CORESERVICES_LIB CoreServices)
    set(EXTRA_LIBS ${CORESERVICES_LIB})
endif()

if (WIN32)
    if (NOT CMAKE_GENERATOR MATCHES "MinGW Makefiles")
        if (NOT ICONV_DIR)
          set(ICONV_DIR "${CMAKE_SOURCE_DIR}/winbuild")
        endif()
        set(CMAKE_REQUIRED_DEFINITIONS "-DLIBICONV_STATIC")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj") # needed for language.cpp on 64bit
        add_definitions(-DLIBICONV_STATIC -D_CRT_SECURE_NO_WARNINGS)
    endif()
endif()

if(POLICY CMP0063)
  cmake_policy(SET CMP0063 NEW)
endif()
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

if (CMAKE_GENERATOR MATCHES "Ninja")
  set(LEX_FLAGS )
  set(YACC_FLAGS )
else ()
  set(LEX_FLAGS $(LEX_FLAGS))
  set(YACC_FLAGS $(YACC_FLAGS))
endif ()

find_program(DOT NAMES dot)
find_package(PythonInterp REQUIRED)
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)
if (BISON_VERSION VERSION_LESS 2.7)
  message(SEND_ERROR "Doxygen requires at least bison version 2.7 (installed: ${BISON_VERSION})")
endif()
find_package(Threads)

if (sqlite3)
  find_package(SQLite3 REQUIRED)
  if (SQLITE3_VERSION VERSION_LESS 3.9.0)
    message(SEND_ERROR "Doxygen requires at least sqlite3 version 3.9.0 (installed: ${SQLITE3_VERSION})")
  endif()
endif()

find_package(Iconv REQUIRED)
include_directories(${ICONV_INCLUDE_DIR})


#set(DOXYDOCS ${CMAKE_SOURCE_DIR}/doc CACHE INTERNAL "Path to doxygen docs")
set(DOXYDOCS ${PROJECT_BINARY_DIR}/doc)
set(ENV{DOXYGEN_DOCDIR} ${DOXYDOCS})
set(GENERATED_SRC "${CMAKE_BINARY_DIR}/generated_src" CACHE INTERNAL "Stores generated files")
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# place binaries for all build types in the same directory, so we know where to find it
# when running tests or generating docs
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${EXECUTABLE_OUTPUT_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${EXECUTABLE_OUTPUT_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${EXECUTABLE_OUTPUT_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${EXECUTABLE_OUTPUT_PATH})

# gather lang codes for translation
file(GLOB lang_files RELATIVE "${CMAKE_SOURCE_DIR}/src" "${CMAKE_SOURCE_DIR}/src/translator_??.h")
if (english_only) # user only wants English
  set(lcodes "ENONLY")
else ()
  set(lcodes "")
  foreach (_lang ${lang_files})
    string(REGEX REPLACE "translator_(.*).h" "\\1" _lang_code ${_lang})
    string(TOUPPER ${_lang_code} lang_code)
    list(APPEND lcodes "${lang_code}")
  endforeach()
endif()
set(LANG_CODES ${lcodes} CACHE STRING "List of language codes for which translations should be compiled in")

if (win_static)
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


add_subdirectory(libmd5)
add_subdirectory(liblodepng)
add_subdirectory(libmscgen)
add_subdirectory(libversion)
add_subdirectory(qtools)
add_subdirectory(vhdlparser)
add_subdirectory(src)

if (build_doc)
    add_subdirectory(examples)
    add_subdirectory(doc)
endif ()

add_subdirectory(addon)

enable_testing()
add_subdirectory(testing)

include(cmake/packaging.cmake) # set CPACK_xxxx properties
include(CPack)
