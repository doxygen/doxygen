find_program(EPSTOPDF NAMES epstopdf )
find_program(SED NAMES sed )

add_custom_target(doc
	ALL
	COMMENT "Generating documentation in ${DOXYDOCS}"
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen
	COMMAND ${CMAKE_COMMAND} -E remove_directory ../latex/refman.tex
	COMMAND ${CMAKE_COMMAND} -E copy doxygen_logo.gif ../html
	COMMAND ${CMAKE_COMMAND} -E copy doxygen_logo_low.gif ../html
	COMMAND ${CMAKE_COMMAND} -E copy Makefile.latex ../latex/Makefile
	COMMAND ${SED} -e "s/\$$VERSION/$(VERSION)/g" doxygen_manual.tex > ../latex/doxygen_manual.tex
	COMMAND ${SED} -e "s/\$$VERSION/$(VERSION)/g" doxygen.sty > ../latex/doxygen.sty
	COMMAND ${EPSTOPDF} doxygen_logo.eps --outfile=../latex/doxygen_logo.pdf
	DEPENDS doxygen language.doc config.doc
	WORKING_DIRECTORY ${DOXYDOCS}
	VERBATIM
	)

# language.doc
add_custom_command(
	COMMAND ${PYTHON_EXECUTABLE} ${DOXYDOCS}/translator.py
	DEPENDS ${DOXYDOCS}/translator.py
	DEPENDS maintainers.txt language.tpl translator.py
	OUTPUT language.doc
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
