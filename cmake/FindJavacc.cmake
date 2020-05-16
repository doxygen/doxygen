
find_program(JAVACC_EXECUTABLE NAMES javacc javaCC Javacc JavaCC javacc.bat DOC "path to the javacc executable")
mark_as_advanced(JAVACC_EXECUTABLE)
if(JAVACC_EXECUTABLE)
  set(JAVACC_FOUND 1)
  message(STATUS "The javacc executable: ${JAVACC_EXECUTABLE}")
  if(JAVACC_EXECUTABLE)
    execute_process(
      COMMAND "${JAVACC_EXECUTABLE}" --version
      OUTPUT_VARIABLE JAVACC_TEMP_VERSION
      OUTPUT_STRIP_TRAILING_WHITESPACE
      RESULT_VARIABLE _JavaCC_version_result
    )
    if(_JavaCC_version_result)
      message(WARNING "Unable to determine JavaCC version: ${_JavaCC_version_result}")
      set(JAVACC_FOUND 0)
    else()
      string(REGEX REPLACE ".*([0-9]+\.[0-9]\.[0-9]+).*" "\\1" JAVACC_VERSION ${JAVACC_TEMP_VERSION})
    endif()
  endif()
else()
  set(JAVACC_FOUND 0)
  message(STATUS "The javacc executable not found, using existing files")
endif()
