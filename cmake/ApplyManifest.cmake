# helper script for Windows to run mt.exe on a generated executable to update its manifest
function(apply_manifest target_name)
  if (WIN32 AND MSVC)
    if (CMAKE_MT)
      set(MT_EXECUTABLE "${CMAKE_MT}")
    else()
      find_program(MT_EXECUTABLE mt)
    endif()
    if(MT_EXECUTABLE)
      set(MANIFEST_FILE "${PROJECT_SOURCE_DIR}/cmake/doxygen.manifest")
      add_custom_command(
        TARGET ${target_name}
        POST_BUILD
        COMMAND "${MT_EXECUTABLE}" -nologo -manifest "${MANIFEST_FILE}" "-updateresource:$<TARGET_FILE:${target_name}>$<SEMICOLON>1"
        VERBATIM)
    endif()
  endif()
endfunction()
