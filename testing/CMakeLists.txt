# run all tests sequentially (keep for backward compatibility)
add_custom_target(tests
  COMMENT "Running doxygen tests..."
  COMMAND ${Python_EXECUTABLE} ${PROJECT_SOURCE_DIR}/testing/runtests.py --doxygen ${PROJECT_BINARY_DIR}/bin/doxygen --inputdir ${PROJECT_SOURCE_DIR}/testing --outputdir ${PROJECT_BINARY_DIR}/testing
  DEPENDS doxygen ${update_doxmlparser_dependency}
)

# get the files in the testing directory starting with 3 digits and an underscore
file(GLOB TEST_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/[0-9][0-9][0-9]_*.*")

foreach(TEST_FILE ${TEST_FILES})
  # extract the test name from the file name
  string(REGEX REPLACE "^.*/([0-9][0-9][0-9]*.*)\\.[^.]*$" "\\1" TEST_NAME "${TEST_FILE}")
  # extract the test id from the file name
  string(REGEX REPLACE "^.*/([0-9][0-9][0-9]*).*$" "\\1" TEST_ID "${TEST_FILE}")
  # add a test target for each test
  add_test(NAME ${TEST_NAME}
    COMMAND ${Python_EXECUTABLE} ${PROJECT_SOURCE_DIR}/testing/runtests.py --id ${TEST_ID} --doxygen $<TARGET_FILE:doxygen> --inputdir ${PROJECT_SOURCE_DIR}/testing --outputdir ${PROJECT_BINARY_DIR}/testing
  )
endforeach()
