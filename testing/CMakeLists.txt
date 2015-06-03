find_program(XMLLINT NAMES xmllint)
find_program(DIFF NAMES diff)

add_custom_target(tests
	COMMENT "Running doxygen tests..."
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/testing/runtests.py --all --doxygen ${PROJECT_BINARY_DIR}/bin/doxygen --inputdir ${CMAKE_SOURCE_DIR}/testing --outputdir ${PROJECT_BINARY_DIR}/testing
	DEPENDS doxygen
)

