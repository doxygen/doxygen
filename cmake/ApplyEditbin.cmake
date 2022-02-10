# helper script for Windows to run editbin.exe on a generated executable
function(apply_editbin target_name target_type)
if (WIN32)
  find_program(EDITBIN editbin)
  if(EDITBIN)
    set(EDITBIN_FLAGS /nologo /OSVERSION:5.1)
    if (${target_type} STREQUAL "console")
      set(EDITBIN_FLAGS ${EDITBIN_FLAGS} /SUBSYSTEM:CONSOLE,6.00)
    elseif (${target_type} STREQUAL "windows")
      set(EDITBIN_FLAGS ${EDITBIN_FLAGS} /SUBSYSTEM:WINDOWS,6.00)
    endif()
    add_custom_command(
      TARGET ${target_name}
      POST_BUILD
      COMMAND "${EDITBIN}" ${EDITBIN_FLAGS} "$<TARGET_FILE:${target_name}>"
      VERBATIM)
    endif()
  endif()
endfunction()
