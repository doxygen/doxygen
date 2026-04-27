# helper script for Windows to run mt.exe on a generated executable to update its manifest
# Usage: apply_manifest(<target> [<manifest_file>])
# If manifest_file is omitted, defaults to ${PROJECT_SOURCE_DIR}/cmake/doxygen.manifest
function(apply_manifest target_name)
  if (WIN32 AND MSVC)
    if (ARGC GREATER 1)
      set(MANIFEST_FILE "${ARGV1}")
    else()
      set(MANIFEST_FILE "${PROJECT_SOURCE_DIR}/cmake/doxygen.manifest")
    endif()
    if (CMAKE_MT)
      set(MT_EXECUTABLE "${CMAKE_MT}")
    else()
      find_program(MT_EXECUTABLE mt)
    endif()
    if(MT_EXECUTABLE)
      add_custom_command(
        TARGET ${target_name}
        POST_BUILD
        COMMAND "${MT_EXECUTABLE}" -nologo -manifest "${MANIFEST_FILE}" "-updateresource:$<TARGET_FILE:${target_name}>$<SEMICOLON>1"
        VERBATIM)
    else()
      message(WARNING "mt.exe not found, manifest will not be applied to ${target_name}")
    endif()
  endif()
endfunction()
