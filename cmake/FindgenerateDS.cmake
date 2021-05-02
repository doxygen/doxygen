find_program(GENERATEDS_EXECUTABLE NAMES generateDS generateDS.py DOC "path to the generateDS executable")
mark_as_advanced(GENERATEDS_EXECUTABLE)
set(GENERATEDS_FOUND 0)
if(GENERATEDS_EXECUTABLE)
  execute_process(
      COMMAND "${GENERATEDS_EXECUTABLE}" --version
      OUTPUT_VARIABLE GENERATEDS_TEMP_VERSION
  )
  string(REGEX MATCH ".* ([0-9]+(\\.[0-9]+)+)" GENERATEDS_TEMP_VERSION_UNUSED "${GENERATEDS_TEMP_VERSION}")
  if(CMAKE_MATCH_1)
    set(GENERATEDS_FOUND 1)
    set(GENERATEDS_VERSION ${CMAKE_MATCH_1})
  endif()
endif()
if(GENERATEDS_FOUND)
  message(STATUS "The generateDS executable: ${GENERATEDS_EXECUTABLE} (found version \"${GENERATEDS_VERSION}\")")
else()
  message(STATUS "The generateDS executable not found, using existing files")
endif()
