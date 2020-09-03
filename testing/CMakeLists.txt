# run all tests sequentially (keep for backward compatibility)
add_custom_target(tests
	COMMENT "Running doxygen tests..."
	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/testing/runtests.py --doxygen ${PROJECT_BINARY_DIR}/bin/doxygen --inputdir ${CMAKE_SOURCE_DIR}/testing --outputdir ${PROJECT_BINARY_DIR}/testing
	DEPENDS doxygen
)

# get the files in the testing directory starting with 3 digits and an underscore
if (${CMAKE_VERSION} VERSION_EQUAL "3.11.0" OR ${CMAKE_VERSION} VERSION_GREATER "3.11.0")
	file(GLOB TEST_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/[0-9][0-9][0-9]_*.*")
else()
	file(GLOB TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/[0-9][0-9][0-9]_*.*")
endif()

foreach(TEST_FILE ${TEST_FILES})
	# extract the test name from the file name
	string(REGEX REPLACE "^.*/([0-9][0-9][0-9]*.*)\\.[^.]*$" "\\1" TEST_NAME "${TEST_FILE}")
	# extract the test id from the file name
	string(REGEX REPLACE "^.*/([0-9][0-9][0-9]*).*$" "\\1" TEST_ID "${TEST_FILE}")
	# add a test target for each test
	add_test(NAME ${TEST_NAME}
		 COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/testing/runtests.py --id ${TEST_ID} --doxygen $<TARGET_FILE:doxygen> --inputdir ${CMAKE_SOURCE_DIR}/testing --outputdir ${PROJECT_BINARY_DIR}/testing
	)
endforeach()
