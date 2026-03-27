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

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" @ONLY)

set(DOC_FILES
        language.tpl
        maintainers.txt
        translator.py
)

file(GLOB LANG_FILES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src//translator_??.h")

foreach (f  ${DOC_FILES})
add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/doc_internal/${f}
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/doc/${f} ${PROJECT_BINARY_DIR}/doc_internal/
    DEPENDS ${PROJECT_SOURCE_DIR}/doc/${f}
    )
set_source_files_properties(${PROJECT_BINARY_DIR}/doc_internal/${f} PROPERTIES GENERATED 1)
endforeach()

add_custom_command(
    COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/doc_internal/cmds_tags.py -cmds ${CMAKE_SOURCE_DIR}/doc_internal ${CMAKE_SOURCE_DIR}/doc_internal/commands_history.md ${PROJECT_BINARY_DIR}/doc_internal/commands_history.md
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/cmds_tags.py ${CMAKE_SOURCE_DIR}/doc_internal/commands_history.md
    OUTPUT ${PROJECT_BINARY_DIR}/doc_internal/commands_history.md
)

add_custom_command(
    COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/doc_internal/cmds_tags.py -tags ${CMAKE_SOURCE_DIR}/doc_internal ${CMAKE_SOURCE_DIR}/doc_internal/tags_history.md ${PROJECT_BINARY_DIR}/doc_internal/tags_history.md
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/cmds_tags.py ${CMAKE_SOURCE_DIR}/doc_internal/tags_history.md
    OUTPUT ${PROJECT_BINARY_DIR}/doc_internal/tags_history.md
)

add_custom_command(
        COMMAND ${Python_EXECUTABLE} translator.py --doc_internal ${PROJECT_SOURCE_DIR}
        DEPENDS ${PROJECT_SOURCE_DIR}/VERSION ${PROJECT_SOURCE_DIR}/doc/maintainers.txt ${PROJECT_SOURCE_DIR}/doc/language.tpl ${PROJECT_BINARY_DIR}/doc_internal/translator.py ${LANG_FILES}
        OUTPUT translator_report.md
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/doc_internal
)

add_custom_target(docs_internal
        COMMENT "Generating HTML internal documentation."
        COMMAND ${CMAKE_COMMAND} -E env VERSION=${VERSION} ${DOXYGEN_EXECUTABLE}
        DEPENDS doxygen
        DEPENDS ${PROJECT_BINARY_DIR}/doc_internal/commands_history.md
        DEPENDS ${PROJECT_BINARY_DIR}/doc_internal/tags_history.md
        DEPENDS ${PROJECT_BINARY_DIR}/doc_internal/translator_report.md
        )
