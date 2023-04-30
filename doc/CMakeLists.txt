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

include (${TOP}/cmake/version.cmake)
string(TIMESTAMP DATE "%d-%m-%Y")
string(TIMESTAMP YEAR "%Y")

find_package(PythonInterp REQUIRED)
find_program(EPSTOPDF NAMES epstopdf )
find_program(PDFLATEX NAMES pdflatex )
find_program(MAKEINDEX NAMES makeindex )
include(GNUInstallDirs)

if (doxygen_BINARY_DIR)
    set(DOXYGEN_EXECUTABLE ${doxygen_BINARY_DIR}/bin/doxygen)
else()
    # when building only the doxygen_doc, from the doc/ directory, the
    # doxygen project variables are unknown so look for doxygen in PATH
    find_package(Doxygen)
endif()

set(DOC_INSTALL_DIR "share/doc/packages/doxygen" CACHE STRING "Relative path where to install the documentation")
set(DOC_FILES
        arch.doc
        archoverview.eps
        archoverview.svg
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
        doxywizard_main.png
        doxywizard_menu_file.png
        doxywizard_menu_settings.png
        doxywizard_menu_help.png
        doxywizard_page1.png
        doxywizard_page2.png
        doxywizard_page3.png
        doxywizard_page4.png
        doxywizard_run.png
        doxywizard_usage.doc
        expert_bool.png
        expert_enum.png
        expert_int.png
        expert_list_dir.png
        expert_list_file.png
        expert_list_filedir.png
        expert_list_string.png
        expert_string_dir.png
        expert_string_file.png
        expert_string_filedir.png
        expert_string_image.png
        expert_string_string.png
        external.doc
        extsearch.doc
        extsearch_flow.dot
        extsearch_flow.eps
        extsearch_flow.png
        faq.doc
        features.doc
        formulas.doc
        tables.doc
        grouping.doc
        htmlcmds.doc
        index.hhp.txt
        infoflow.eps
        infoflow.fig
        infoflow.png
        install.doc
        layout_index_and_notreeview.png
        layout_index_and_treeview.png
        layout_noindex_and_notreeview.png
        layout_noindex_and_treeview.png
        layout_noindex_and_sidebar.png
        lists.doc
        markdown.doc
        output.doc
        perlmod.doc
        preprocessing.doc
        searching.doc
        starting.doc
        trouble.doc
        xmlcmds.doc
        emojisup.doc
        language.tpl
        maintainers.txt
        translator.py
        smile.png
        doxygen_logo.svg
)

set(DOC_FILES_IMAGES
        add.png
        del.png
        file.png
        folder.png
        refresh.png
)

if (build_doc_chm)
    set(DOC_FILES_CHM
        Doxyfile_chm
)
endif ()

if (${CMAKE_VERSION} VERSION_EQUAL "3.11.0" OR ${CMAKE_VERSION} VERSION_GREATER "3.11.0")
    file(GLOB LANG_FILES CONFIGURE_DEPENDS "${TOP}/src//translator_??.h")
else()
    file(GLOB LANG_FILES "${TOP}/src//translator_??.h")
endif()


file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/man
                    ${PROJECT_BINARY_DIR}/src
                    ${PROJECT_BINARY_DIR}/doc
                    ${PROJECT_BINARY_DIR}/doc/images)

foreach (f  ${DOC_FILES})
add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/doc/${f}
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/doc/${f} ${PROJECT_BINARY_DIR}/doc/
    DEPENDS ${PROJECT_SOURCE_DIR}/doc/${f}
    )
set_source_files_properties(${PROJECT_BINARY_DIR}/doc/${f} PROPERTIES GENERATED 1)
list(APPEND OUT_DOC_FILES "${PROJECT_BINARY_DIR}/doc/${f}")
endforeach()

foreach (f  ${DOC_FILES_IMAGES})
add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/doc/images/${f}
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/addon/doxywizard/images/${f} ${PROJECT_BINARY_DIR}/doc/images/
    DEPENDS ${PROJECT_SOURCE_DIR}/addon/doxywizard/images/${f}
    )
set_source_files_properties(${PROJECT_BINARY_DIR}/doc/images/${f} PROPERTIES GENERATED 1)
list(APPEND OUT_DOC_FILES "${PROJECT_BINARY_DIR}/doc/images/${f}")
endforeach()

foreach (f  ${DOC_FILES_CHM})
add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/doc/${f}
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/doc/${f} ${PROJECT_BINARY_DIR}/doc/
    DEPENDS ${PROJECT_SOURCE_DIR}/doc/${f}
    )
set_source_files_properties(${PROJECT_BINARY_DIR}/doc/${f} PROPERTIES GENERATED 1)
list(APPEND OUT_DOC_CHM_FILES "${PROJECT_BINARY_DIR}/doc/${f}")
endforeach()

configure_file(${PROJECT_SOURCE_DIR}/doc/manual.sty         ${PROJECT_BINARY_DIR}/doc/manual.sty COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/doc/doxygen_manual.tex ${PROJECT_BINARY_DIR}/doc/doxygen_manual.tex COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/doc/doxygen.1          ${PROJECT_BINARY_DIR}/man/doxygen.1)
configure_file(${PROJECT_SOURCE_DIR}/doc/doxywizard.1       ${PROJECT_BINARY_DIR}/man/doxywizard.1)
configure_file(${PROJECT_SOURCE_DIR}/doc/doxysearch.1       ${PROJECT_BINARY_DIR}/man/doxysearch.1)
configure_file(${PROJECT_SOURCE_DIR}/doc/doxyindexer.1      ${PROJECT_BINARY_DIR}/man/doxyindexer.1)

# Call the main page "Introduction" in LaTeX, which is more appropriate for that format.
set(MAINPAGE_TITLE Doxygen)
configure_file(${PROJECT_SOURCE_DIR}/doc/index.doc          ${PROJECT_BINARY_DIR}/doc/index_html.doc)
set(MAINPAGE_TITLE Introduction)
configure_file(${PROJECT_SOURCE_DIR}/doc/index.doc          ${PROJECT_BINARY_DIR}/doc/index_latex.doc)

# doc/language.doc (see tag Doxyfile:INPUT)
add_custom_command(
        COMMAND ${PYTHON_EXECUTABLE} translator.py --doc ${PROJECT_SOURCE_DIR}
        DEPENDS ${PROJECT_SOURCE_DIR}/VERSION ${PROJECT_SOURCE_DIR}/doc/maintainers.txt ${PROJECT_SOURCE_DIR}/doc/language.tpl ${PROJECT_BINARY_DIR}/doc/translator.py ${LANG_FILES}
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
        COMMAND ${CMAKE_COMMAND} -E env VERSION=${VERSION} INDEX_DOC=index_html.doc GENERATE_HTML=YES GENERATE_LATEX=NO ${DOXYGEN_EXECUTABLE}
        COMMAND ${CMAKE_COMMAND} -E env VERSION=${VERSION} INDEX_DOC=index_latex.doc GENERATE_HTML=NO GENERATE_LATEX=YES ${DOXYGEN_EXECUTABLE}
        DEPENDS ${PROJECT_BINARY_DIR}/doc/language.doc ${PROJECT_BINARY_DIR}/doc/config.doc
        DEPENDS ${OUT_DOC_FILES}
        DEPENDS examples
        DEPENDS doxygen
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/doc/
)

add_custom_target(doxygen_pdf
        COMMENT "Generating Doxygen Manual PDF."
        COMMAND ${CMAKE_COMMAND} -E remove refman.tex
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/doc/doxygen_manual.tex  .
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/doc/manual.sty  .
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/doc/doxygen_logo.pdf  .
        COMMAND ${PDFLATEX}  -shell-escape doxygen_manual.tex
        COMMAND ${MAKEINDEX} doxygen_manual.idx
        COMMAND ${PDFLATEX}  -shell-escape doxygen_manual.tex
        COMMAND ${MAKEINDEX} doxygen_manual.idx
        COMMAND ${PDFLATEX}  -shell-escape doxygen_manual.tex
        DEPENDS run_doxygen
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/latex
)
add_custom_target(docs
        DEPENDS ${PROJECT_BINARY_DIR}/man/doxygen.1
                ${PROJECT_BINARY_DIR}/man/doxywizard.1
                ${PROJECT_BINARY_DIR}/man/doxysearch.1
                ${PROJECT_BINARY_DIR}/man/doxyindexer.1
                doxygen_pdf
        VERBATIM
        )

# return status of ${HTML_HELP_COMPILER} is not correct, so we have to do a trick with an "or" statement
# output is redirected to "nul" as we are on windows no problem
if (build_doc_chm)
add_custom_target(docs_chm
    COMMENT "Generating CHM documentation."
    COMMAND ${CMAKE_COMMAND} -E env VERSION=${VERSION} HTML_HELP_COMPILER=${HTML_HELP_COMPILER} ${DOXYGEN_EXECUTABLE} Doxyfile_chm
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_BINARY_DIR}/html/examples ${PROJECT_BINARY_DIR}/chm/examples
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/chm/index.hhp ${PROJECT_BINARY_DIR}/chm/doxygen_manual.hhp
    COMMAND ${CMAKE_COMMAND} -E echo "import os" > ${PROJECT_BINARY_DIR}/chm/doxygen_manual_examples_chm.py
    COMMAND ${CMAKE_COMMAND} -E echo "for root, dirs, files in os.walk('examples'):" >> ${PROJECT_BINARY_DIR}/chm/doxygen_manual_examples_chm.py
    COMMAND ${CMAKE_COMMAND} -E echo "    for file in files:" >> ${PROJECT_BINARY_DIR}/chm/doxygen_manual_examples_chm.py
    COMMAND ${CMAKE_COMMAND} -E echo "        if file.endswith('.html') or file.endswith('.png') or file.endswith('.css') or file.endswith('.gif'):" >> ${PROJECT_BINARY_DIR}/chm/doxygen_manual_examples_chm.py
    COMMAND ${CMAKE_COMMAND} -E echo "            print(os.path.join(root, file))" >> ${PROJECT_BINARY_DIR}/chm/doxygen_manual_examples_chm.py
    COMMAND ${CMAKE_COMMAND} -E chdir  ${PROJECT_BINARY_DIR}/chm ${PYTHON_EXECUTABLE} ${PROJECT_BINARY_DIR}/chm/doxygen_manual_examples_chm.py >> ${PROJECT_BINARY_DIR}/chm/doxygen_manual.hhp
    COMMAND ${CMAKE_COMMAND} -E chdir ${PROJECT_BINARY_DIR}/chm "${HTML_HELP_COMPILER}" doxygen_manual.hhp || echo > nul
    COMMAND ${CMAKE_COMMAND} -E rename ${PROJECT_BINARY_DIR}/chm/index.chm ${PROJECT_BINARY_DIR}/chm/doxygen_manual.chm
        DEPENDS ${PROJECT_BINARY_DIR}/doc/language.doc ${PROJECT_BINARY_DIR}/doc/config.doc
        DEPENDS ${OUT_DOC_FILES}
        DEPENDS ${OUT_DOC_CHM_FILES}
        DEPENDS examples
        DEPENDS doxygen
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/doc/
)
endif ()
################################################################################
install(FILES
        "${PROJECT_BINARY_DIR}/man/doxygen.1"
        "${PROJECT_BINARY_DIR}/man/doxywizard.1"
        "${PROJECT_BINARY_DIR}/man/doxysearch.1"
        "${PROJECT_BINARY_DIR}/man/doxyindexer.1"
        DESTINATION ${CMAKE_INSTALL_MANDIR}/man1
)

install(CODE "if(NOT EXISTS \"${PROJECT_BINARY_DIR}/latex/doxygen_manual.pdf\")
    message(FATAL_ERROR \"\nTerminating:\n    documentation has not been generated, \n    create documentation by using the 'docs' target followed by an 'install'\n\")
endif()"
)

install(FILES
        "${PROJECT_BINARY_DIR}/latex/doxygen_manual.pdf"
        DESTINATION "${CMAKE_INSTALL_PREFIX}/${DOC_INSTALL_DIR}"
)

if (build_doc_chm)
install(CODE "if(NOT EXISTS \"${PROJECT_BINARY_DIR}/chm/doxygen_manual.chm\")
    message(FATAL_ERROR \"\nTerminating:\n    CHM documentation has not been generated, \n    create CHM documentation by using the 'docs_chm' target followed by an 'install'\n\")
endif()"
)

install(FILES
        "${PROJECT_BINARY_DIR}/chm/doxygen_manual.chm"
        DESTINATION "${CMAKE_INSTALL_PREFIX}/${DOC_INSTALL_DIR}"
)
endif ()

install(DIRECTORY
        "${PROJECT_BINARY_DIR}/html"
        DESTINATION "${CMAKE_INSTALL_PREFIX}/${DOC_INSTALL_DIR}"
)
