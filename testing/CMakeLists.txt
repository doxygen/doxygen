find_program(XMLLINT NAMES xmllint)
find_program(DIFF NAMES diff)

file(GLOB test_files "[0-9][0-9][0-9]_*.*")
foreach(test_file ${test_files})
	if ( ${test_file} MATCHES "([0-9][0-9][0-9])_")
		set(test_id ${CMAKE_MATCH_1})
		set(test_out ${CMAKE_BINARY_DIR}/testing/${test_id})
		get_filename_component(test_dirname ${test_file} DIRECTORY)
		get_filename_component(test_basename ${test_file} NAME)
		set(doxyfile ${test_out}/Doxyfile)

		# setup the test directory
		execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory ${test_out})
		execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${test_out})
		execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/testing/Doxyfile ${test_out})
		execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${test_file} ${test_out})
		file(APPEND ${doxyfile} "STRIP_FROM_PATH = ${test_out}\n")
		file(APPEND ${doxyfile} "INPUT = ${test_basename}\n")

		# extract config lines from test files
		file(STRINGS "${test_basename}" test_file_config REGEX "// [a-z]+: .*")

		# clear the lists
		set(test_objective)
		set(test_config)
		set(test_check)

		# turn the config lines into lists
		foreach(line IN LISTS test_file_config)
			if ("${line}" MATCHES "//[ ]*([a-zA-Z]+)[ ]*:[ ]*(.*)")
				list(APPEND test_${CMAKE_MATCH_1} ${CMAKE_MATCH_2})
			endif()
		endforeach()
		foreach(line IN LISTS test_config)
			# append the config line to Doxyfile
			file(APPEND ${doxyfile} "${line}\n")
			# in case this is an INPUT line, copy the files to test directory
			if ("${line}" MATCHES "INPUT[ ]*=(.*)")
				separate_arguments(items UNIX_COMMAND "${CMAKE_MATCH_1}")
				foreach(i IN LISTS items)
					execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${test_dirname}/${i} ${test_out})
				endforeach()
			endif()
		endforeach()
		# replace spaces with semicolons so it can be passed as argument
		separate_arguments(test_check UNIX_COMMAND "${test_check}")
		add_test(NAME test_${test_id}
			WORKING_DIRECTORY ${test_out}
			COMMAND cmake
			-D check=${test_check}
			-D dirname=${test_dirname}/${test_id}
			-D top=${CMAKE_SOURCE_DIR}
			-D doxygen=${EXECUTABLE_OUTPUT_PATH}/doxygen
			-D diff=${DIFF}
			-D xmllint=${XMLLINT}
			-P ${CMAKE_SOURCE_DIR}/testing/test_driver.cmake
		)
	endif()
endforeach()
