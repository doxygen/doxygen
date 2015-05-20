include(${top}/cmake/version.cmake)

macro(check_rv)
	if (${ARGV0})
		message(FATAL_ERROR "test failed")
	endif()
endmacro()

execute_process( COMMAND ${doxygen} Doxyfile RESULT_VARIABLE rv)
check_rv(${rv})

foreach (_check IN LISTS check)
	set(xml_file "out_${_check}")

	set(xml_cmd ${xmllint} --format --noblanks --nowarning xml/${_check} --output ${xml_file})
	message(STATUS ${xml_cmd})
	execute_process(COMMAND  ${xml_cmd} RESULT_VARIABLE rv)
	check_rv(${rv})

	message(STATUS "CHOMP VOLATILE")
	file(READ ${xml_file} OUT_XML)
	string(REGEX REPLACE "version=\"${VERSION}\"" "version=\"\"" OUT_WO_VERSION_XML "${OUT_XML}")
	file(WRITE ${xml_file} ${OUT_WO_VERSION_XML})

	set(diff_cmd ${diff} -u ${xml_file} ${dirname}/${_check})
	message(STATUS ${diff_cmd})
	execute_process(COMMAND ${diff_cmd} RESULT_VARIABLE rv)
	check_rv(${rv})
endforeach()
