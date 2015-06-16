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

cmake_minimum_required(VERSION 2.8.12)
project(doxygen_doc)

if (build_doc)

set(TOP ${PROJECT_SOURCE_DIR}/..)
include (${TOP}/cmake/version.cmake)
string(TIMESTAMP DATE "%d-%m-%Y")

find_package(PythonInterp REQUIRED)
find_program(EPSTOPDF NAMES epstopdf )
find_program(PDFLATEX NAMES pdflatex )
find_program(MAKEINDEX NAMES makeindex )

if (doxygen_BINARY_DIR)
    set(DOXYGEN_EXECUTABLE ${doxygen_BINARY_DIR}/bin/doxygen)
else()
    # when building only the doxygen_doc, from the doc/ directory, the
    # doxygen project variables are unknown so look for doxygen in PATH
    find_package(Doxygen)
endif()

set(DOC_INSTALL_DIR "share/doc/packages/doxygen" CACHE STRING "Relative path where to install the documentation")
set(EXAMPLE_DIR ${TOP}/examples)
set(DOC_FILES
        arch.doc
        archoverview.eps
        archoverview.gif
        autolink.doc
        changelog.doc
        commands.doc
        custcmd.doc
        customize.doc
        diagrams.doc
        docblocks.doc
        Doxyfile
        doxygen_manual.css
        doxygen_usage.doc
        doxywizard_expert.png
        doxywizard.gif
        doxywizard_main.png
        doxywizard_menu.png
        doxywizard_page1.png
        doxywizard_page2.png
        doxywizard_page3.png
        doxywizard_page4.png
        doxywizard_usage.doc
        external.doc
        extsearch.doc
        extsearch_flow.dot
        extsearch_flow.eps
        extsearch_flow.png
        faq.doc
        features.doc
        formulas.doc
        grouping.doc
        htmlcmds.doc
        index.doc
        index.hhp.txt
        infoflow.eps
        infoflow.fig
        infoflow.png
        install.doc
        install_prefix
        language.tpl
        lists.doc
        maintainers.txt
        markdown.doc
        output.doc
        perlmod.doc
        perlmod_tree.doc
        preprocessing.doc
        searching.doc
        starting.doc
        translator.py
        trouble.doc
        xmlcmds.doc
)
file(GLOB LANG_FILES "${TOP}/src/translator_??.h")

file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/man)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/src)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/doc)

file(COPY ${TOP}/VERSION                  DESTINATION ${PROJECT_BINARY_DIR}/)
file(COPY ${TOP}/src/translator.h         DESTINATION ${PROJECT_BINARY_DIR}/src/)
file(COPY ${TOP}/src/translator_adapter.h DESTINATION ${PROJECT_BINARY_DIR}/src/)
file(COPY ${LANG_FILES}                   DESTINATION ${PROJECT_BINARY_DIR}/src/)
file(COPY ${DOC_FILES}                    DESTINATION ${PROJECT_BINARY_DIR}/doc/)
file(COPY ${EXAMPLE_DIR}                  DESTINATION ${PROJECT_BINARY_DIR}/)

configure_file(${PROJECT_SOURCE_DIR}/doxygen.sty        doc/doxygen.sty)
configure_file(${PROJECT_SOURCE_DIR}/doxygen_manual.tex doc/doxygen_manual.tex)
configure_file(${PROJECT_SOURCE_DIR}/doxygen.1          man/doxygen.1)
configure_file(${PROJECT_SOURCE_DIR}/doxywizard.1       man/doxywizard.1)
configure_file(${PROJECT_SOURCE_DIR}/doxysearch.1       man/doxysearch.1)
configure_file(${PROJECT_SOURCE_DIR}/doxyindexer.1      man/doxyindexer.1)

# doc/language.doc (see tag Doxyfile:INPUT)
add_custom_command(
        COMMAND ${PYTHON_EXECUTABLE} translator.py
        DEPENDS maintainers.txt language.tpl translator.py
        OUTPUT language.doc
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/doc
)
set_source_files_properties(language.doc PROPERTIES GENERATED 1)

# doc/config.doc (see tag Doxyfile:INPUT)
add_custom_command(
        COMMAND ${PYTHON_EXECUTABLE}  ${TOP}/src/configgen.py -doc ${TOP}/src/config.xml > config.doc
        DEPENDS ${TOP}/src/config.xml ${TOP}/src/configgen.py
        OUTPUT config.doc
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/doc/
)
set_source_files_properties(config.doc PROPERTIES GENERATED 1)
################################################################################
add_custom_target(run_doxygen
        COMMENT "Generating Latex and HTML documentation."
        COMMAND ${DOXYGEN_EXECUTABLE}
        DEPENDS language.doc config.doc
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/doc/
)
if (doxygen_BINARY_DIR)
    # building from project `doxygen'
    add_dependencies(run_doxygen
       doxygen
    )
endif()

add_custom_target(doxygen_pdf
        COMMENT "Generating Doxygen Manual PDF."
        COMMAND ${CMAKE_COMMAND} -E remove_directory refman.tex
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/doc/doxygen_manual.tex  .
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/doc/doxygen.sty  .
        COMMAND ${EPSTOPDF} ${PROJECT_SOURCE_DIR}/doxygen_logo.eps --outfile=doxygen_logo.pdf
        COMMAND ${PDFLATEX}  doxygen_manual.tex
        COMMAND ${MAKEINDEX} doxygen_manual.idx
        COMMAND ${PDFLATEX}  doxygen_manual.tex
        DEPENDS run_doxygen
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/latex
)
add_custom_target(docs
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/doxygen_logo.gif      html/
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/doxygen_logo_low.gif  html/
        DEPENDS ${PROJECT_BINARY_DIR}/man/doxygen.1
                ${PROJECT_BINARY_DIR}/man/doxywizard.1
                ${PROJECT_BINARY_DIR}/man/doxysearch.1
                ${PROJECT_BINARY_DIR}/man/doxyindexer.1
                doxygen_pdf
        VERBATIM
        )

################################################################################
install(FILES
        "${PROJECT_BINARY_DIR}/man/doxygen.1"
        "${PROJECT_BINARY_DIR}/man/doxywizard.1"
        "${PROJECT_BINARY_DIR}/man/doxysearch.1"
        "${PROJECT_BINARY_DIR}/man/doxyindexer.1"
        DESTINATION man/man1
)

install(FILES
        "${PROJECT_BINARY_DIR}/latex/doxygen_manual.pdf"
        DESTINATION "${CMAKE_INSTALL_PREFIX}/${DOC_INSTALL_DIR}"
)

install(DIRECTORY
        "${PROJECT_BINARY_DIR}/html"
        DESTINATION "${CMAKE_INSTALL_PREFIX}/${DOC_INSTALL_DIR}"
)

endif(build_doc)
