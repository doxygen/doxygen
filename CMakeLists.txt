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

cmake_minimum_required(VERSION 3.0)
project(doxygen)

# `Configure' phase
set(DOXYDOCS ${CMAKE_SOURCE_DIR}/doc CACHE INTERNAL "Path to doxygen docs")
set(ENV{DOXYGEN_DOCDIR} ${DOXYDOCS})
set(GENERATED_SRC "${CMAKE_BINARY_DIR}/generated_src" CACHE INTERNAL "Stores generated files")
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)


option(build_wizard    "Build the GUI frontend for doxygen." OFF)
option(build_app       "Example showing how to embed doxygen in an application." OFF)
option(build_xmlparser "Example showing how to parse doxygen's XML output." OFF)
option(build_search    "Build external search tools (doxysearch and doxyindexer)" OFF)
option(use_sqlite3     "Add support for sqlite3 output [experimental]." OFF)
option(use_libclang    "Add support for libclang parsing." OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(version)

set(sqlite3  "0" CACHE INTERNAL "used in settings.h")
set(clang    "0" CACHE INTERNAL "used in settings.h")
if (${use_sqlite3})
	set(sqlite3  "1" CACHE INTERNAL "used in settings.h")
endif()
if (${use_libclang})
	set(clang    "1" CACHE INTERNAL "used in settings.h")
endif()

if (${CMAKE_SYSTEM} MATCHES "Darwin")
    set(CMAKE_CXX_FLAGS "-Wno-deprecated-register ${CMAKE_CXX_FLAGS}")
    find_library(CORESERVICES_LIB CoreServices)
    set(EXTRA_LIBS ${CORESERVICES_LIB})
endif()

if (WIN32)
    set(ICONV_DIR "${CMAKE_SOURCE_DIR}/winbuild")
    set(CMAKE_REQUIRED_DEFINITIONS "-DLIBICONV_STATIC")
    add_definitions(-DLIBICONV_STATIC -D_CRT_SECURE_NO_WARNINGS)
endif()

find_program(DOT NAMES dot)
find_package(PythonInterp REQUIRED)
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)
find_package(Threads)

if (sqlite3)
    find_package(SQLite3 REQUIRED)
endif()

find_package(Iconv REQUIRED)
include_directories(${ICONV_INCLUDE_DIR})


add_subdirectory(libmd5)
add_subdirectory(qtools)
add_subdirectory(vhdlparser)
add_subdirectory(src)
add_subdirectory(doc)

add_subdirectory(addon/doxmlparser)
add_subdirectory(addon/doxyapp)
add_subdirectory(addon/doxysearch)
add_subdirectory(addon/doxywizard)

enable_testing()
add_subdirectory(testing)


# `Generate' phase
#  If a target uses a generated file, set the GENERATED file property in the CMakeLists.txt
#  defining that target.
#  see src/CMakeLists.txt

file(MAKE_DIRECTORY ${GENERATED_SRC})

# Generate settings.h
file(GENERATE OUTPUT ${GENERATED_SRC}/settings.h
CONTENT "#ifndef SETTINGS_H
#define SETTINGS_H
#define USE_SQLITE3 ${sqlite3}
#define USE_LIBCLANG ${clang}
#define IS_SUPPORTED(x) \\
  ((USE_SQLITE3  && strcmp(\"USE_SQLITE3\",(x))==0)  || \\
   (USE_LIBCLANG && strcmp(\"USE_LIBCLANG\",(x))==0) || \\
  0)
#endif" )

# Generate version.cpp
file(GENERATE OUTPUT ${GENERATED_SRC}/version.cpp
    CONTENT "char versionString[]=\"${VERSION}\";"
)
