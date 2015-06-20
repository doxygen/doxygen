if (build_doc)

find_program(EPSTOPDF NAMES epstopdf )
find_program(SED NAMES sed )
find_program(MAKE NAMES make gmake nmake )

file(GLOB DOC_FILES "*")
file(GLOB LANG_FILES "${CMAKE_SOURCE_DIR}/src/translator_??.h")

file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/man)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/src)
file(COPY ${CMAKE_SOURCE_DIR}/src/translator.h DESTINATION ${PROJECT_BINARY_DIR}/src)
file(COPY ${CMAKE_SOURCE_DIR}/src/translator_adapter.h DESTINATION ${PROJECT_BINARY_DIR}/src)
file(COPY ${LANG_FILES} DESTINATION ${PROJECT_BINARY_DIR}/src)
file(COPY ${CMAKE_SOURCE_DIR}/VERSION DESTINATION ${PROJECT_BINARY_DIR})

# copy all doc files
add_custom_target(copy_docs)
foreach(doc_file ${DOC_FILES})
  add_custom_command(TARGET copy_docs PRE_BUILD
                     COMMAND ${CMAKE_COMMAND} -E
                         copy ${doc_file} ${DOXYDOCS})
endforeach()

add_custom_target(docs
	COMMENT "Generating documentation in ${DOXYDOCS}"
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen
	COMMAND ${CMAKE_COMMAND} -E remove_directory ../latex/refman.tex
	COMMAND ${CMAKE_COMMAND} -E copy doxygen_logo.gif ../html
	COMMAND ${CMAKE_COMMAND} -E copy doxygen_logo_low.gif ../html
	COMMAND ${CMAKE_COMMAND} -E copy Makefile.latex ../latex/Makefile
	COMMAND ${SED} -e "s/\$VERSION/${VERSION}/g" doxygen_manual.tex > ../latex/doxygen_manual.tex
	COMMAND ${SED} -e "s/\$VERSION/${VERSION}/g" doxygen.sty > ../latex/doxygen.sty
	COMMAND ${EPSTOPDF} doxygen_logo.eps --outfile=../latex/doxygen_logo.pdf
        COMMAND ${MAKE} -C ../latex > latex_out.txt
	DEPENDS doxygen copy_docs ${PROJECT_BINARY_DIR}/doc/language.doc config.doc examples
                "${PROJECT_BINARY_DIR}/man/doxygen.1"
                "${PROJECT_BINARY_DIR}/man/doxywizard.1"
                "${PROJECT_BINARY_DIR}/man/doxysearch.1"
                "${PROJECT_BINARY_DIR}/man/doxyindexer.1"
	WORKING_DIRECTORY ${DOXYDOCS}
	VERBATIM
	)

# language.doc
add_custom_command(
        COMMAND ${CMAKE_COMMAND} "-DSOURCE=${CMAKE_SOURCE_DIR}" -P ${CMAKE_SOURCE_DIR}/cmake/run_translator.cmake
	DEPENDS ${DOXYDOCS}/translator.py
	DEPENDS maintainers.txt language.tpl translator.py
	OUTPUT ${PROJECT_BINARY_DIR}/doc/language.doc
	WORKING_DIRECTORY ${DOXYDOCS}
)
set_source_files_properties(${DOXYDOCS}/language.doc PROPERTIES GENERATED 1)

# config.doc
add_custom_command(
	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/src/configgen.py -doc ${CMAKE_SOURCE_DIR}/src/config.xml > config.doc
	DEPENDS ${CMAKE_SOURCE_DIR}/src/config.xml ${CMAKE_SOURCE_DIR}/src/configgen.py
	OUTPUT config.doc
	WORKING_DIRECTORY ${DOXYDOCS}
)
set_source_files_properties(${DOXYDOCS}/config.doc PROPERTIES GENERATED 1)

string(TIMESTAMP TODAY "%d-%m-%Y")

add_custom_command(
	COMMAND ${SED} -e "s/DATE/${TODAY}/g" -e "s/VERSION/${VERSION}/g" doxygen.1 > "${PROJECT_BINARY_DIR}/man/doxygen.1"
        OUTPUT "${PROJECT_BINARY_DIR}/man/doxygen.1"
)

add_custom_command(
	COMMAND ${SED} -e "s/DATE/${TODAY}/g" -e "s/VERSION/${VERSION}/g" doxywizard.1 > "${PROJECT_BINARY_DIR}/man/doxywizard.1"
        OUTPUT "${PROJECT_BINARY_DIR}/man/doxywizard.1"
)

add_custom_command(
	COMMAND ${SED} -e "s/DATE/${TODAY}/g" -e "s/VERSION/${VERSION}/g" doxysearch.1 > "${PROJECT_BINARY_DIR}/man/doxysearch.1"
        OUTPUT "${PROJECT_BINARY_DIR}/man/doxysearch.1"
)

add_custom_command(
	COMMAND ${SED} -e "s/DATE/${TODAY}/g" -e "s/VERSION/${VERSION}/g" doxyindexer.1 > "${PROJECT_BINARY_DIR}/man/doxyindexer.1"
        OUTPUT "${PROJECT_BINARY_DIR}/man/doxyindexer.1"
)

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

endif()
