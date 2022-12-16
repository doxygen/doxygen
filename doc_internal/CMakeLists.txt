# vim:ts=4:sw=4:expandtab:autoindent:
#
# Copyright (C) 1997-2022 by Dimitri van Heesch.
#
# Permission to use, copy, modify, and distribute this software and its
# documentation under the terms of the GNU General Public License is hereby
# granted. No representations are made about the suitability of this software
# for any purpose. It is provided "as is" without express or implied warranty.
# See the GNU General Public License for more details.
#
# Documents produced by Doxygen are derivative works derived from the
# input used in their production; they are not affected by this license.

if (doxygen_BINARY_DIR)
    set(DOXYGEN_EXECUTABLE ${doxygen_BINARY_DIR}/bin/doxygen)
else()
    # when building only the doxygen_doc, from the doc/ directory, the
    # doxygen project variables are unknown so look for doxygen in PATH
    find_package(Doxygen)
endif()

configure_file(${CMAKE_SOURCE_DIR}/doc_internal/Doxyfile.in "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" @ONLY)

add_custom_target(docs_internal
        COMMENT "Generating HTML internal documentation."
        COMMAND ${CMAKE_COMMAND} -E env VERSION=${VERSION} ${DOXYGEN_EXECUTABLE}
        DEPENDS doxygen
        )
