# vim:ts=4:sw=4:expandtab:autoindent:

# setup information for doxygen version handling
set(PRE_CONFIGURE_DOXYGEN_VERSION_FILE "${CMAKE_SOURCE_DIR}/libversion/doxyversion.cpp.in")
set(POST_CONFIGURE_DOXYGEN_VERSION_FILE "${GENERATED_SRC}/doxyversion.cpp")

# setup information for git version handling
set(PRE_CONFIGURE_GIT_VERSION_FILE "${CMAKE_SOURCE_DIR}/libversion/gitversion.cpp.in")
set(POST_CONFIGURE_GIT_VERSION_FILE "${GENERATED_SRC}/gitversion.cpp")

include(${CMAKE_SOURCE_DIR}/cmake/git_watcher.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/doxygen_version.cmake)

include_directories(
    .
)

add_library(doxygen_version STATIC
    ${POST_CONFIGURE_DOXYGEN_VERSION_FILE}
    ${POST_CONFIGURE_GIT_VERSION_FILE}
)

add_dependencies( doxygen_version check_git_repository )
add_dependencies( doxygen_version check_doxygen_version )

set_source_files_properties(${POST_CONFIGURE_GIT_VERSION_FILE} PROPERTIES GENERATED 1)
set_source_files_properties(${POST_CONFIGURE_DOXYGEN_VERSION_FILE} PROPERTIES GENERATED 1)
