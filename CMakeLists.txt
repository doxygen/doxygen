cmake_minimum_required(VERSION 3.0)
project(doxygen)

option(build_wizard    "Build the GUI frontend for doxygen." OFF)
option(build_app       "Example showing how to embed doxygen in an application." OFF)
option(build_xmlparser "Example showing how to parse doxygen's XML output." OFF)
option(build_search    "Build external search tools (doxysearch and doxyindexer)" OFF)
option(use_sqlite3     "Add support for sqlite3 output [experimental]." OFF)
option(use_libclang    "Add support for libclang parsing." OFF)

set(doxygen_version_major    "1" CACHE STRING "Major")
set(doxygen_version_minor    "8" CACHE STRING "Minor")
set(doxygen_version_revision "8" CACHE STRING "Revision")
#set(doxygen_version_mmn      "5" CACHE STRING "Mmn")

set(sqlite3  "0" CACHE INTERNAL "used in settings.h")
set(clang    "0" CACHE INTERNAL "used in settings.h")

if (use_sqlite3)
	set(sqlite3  "1" CACHE INTERNAL "used in settings.h")
endif()
if (use_libclang)
	set(clang    "1" CACHE INTERNAL "used in settings.h")
endif()


list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# TODO: Dot
find_package(PythonInterp REQUIRED)
find_package(Perl REQUIRED)
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)
find_package(Threads)
if (sqlite3)
	find_package(SQLite3 REQUIRED)
endif(sqlite3)


set(GENERATED_SRC "${CMAKE_BINARY_DIR}/generated_src" CACHE INTERNAL "Stores generated files")
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib/)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/)
list(APPEND CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")

add_subdirectory(libmd5)
add_subdirectory(qtools)
add_subdirectory(vhdlparser)
add_subdirectory(src)

add_subdirectory(addon/doxmlparser)
add_subdirectory(addon/doxyapp)
add_subdirectory(addon/doxysearch)
add_subdirectory(addon/doxywizard)

enable_testing()
add_subdirectory(testing)
