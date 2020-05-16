
find_program(JAVACC_EXECUTABLE NAMES javacc javaCC Javacc JavaCC javacc.bat DOC "path to the javacc executable")
mark_as_advanced(JAVACC_EXECUTABLE)
if(JAVACC_EXECUTABLE)
  set(JAVACC_FOUND 1)
  message(STATUS "The javacc executable: ${JAVACC_EXECUTABLE}")
  execute_process(
      COMMAND "${JAVACC_EXECUTABLE}" --version
      OUTPUT_VARIABLE JAVACC_TEMP_VERSION
  )
  string(REGEX MATCH ".* ([0-9]+\.[0-9]+[\.0-9]*) .*" JAVACC_TEMP_VERSION2_UNUSED "${JAVACC_TEMP_VERSION}")
  if(CMAKE_MATCH_1)
    set(JAVACC_VERSION ${CMAKE_MATCH_1})
  else()
    message(STATUS "Unable to determine JavaCC version, using existing files")
    set(JAVACC_FOUND 0)
  endif()
else()
  set(JAVACC_FOUND 0)
  message(STATUS "The javacc executable not found, using existing files")
endif()
