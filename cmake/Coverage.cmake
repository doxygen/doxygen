if(enable_coverage)
  FIND_PROGRAM( LCOV_PATH lcov )
  FIND_PROGRAM( GENHTML_PATH genhtml )
  set(COVERAGE_COMPILER_FLAGS -g --coverage -O0
      CACHE INTERNAL "")
  set(COVERAGE_LINKER_FLAGS --coverage
      CACHE INTERNAL "")
  add_custom_target(coverage-clean
	COMMAND ${LCOV_PATH} --rc lcov_branch_coverage=1 --directory . --zerocounters
	WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  )
  add_custom_target(coverage
	COMMAND ${LCOV_PATH} --rc lcov_branch_coverage=1 --directory . --capture --output-file cov.info
	COMMAND ${LCOV_PATH} --rc lcov_branch_coverage=1 --remove cov.info '*/c++/*' '*/_ctype.h' '*/generated_src/*' --output-file cov.info.cleaned
	COMMAND ${CMAKE_COMMAND} -Dsearch=${PROJECT_BINARY_DIR}
	                         -Dreplace=${PROJECT_SOURCE_DIR}
				 -Dsrc=cov.info.cleaned
				 -Ddst=cov.info.final
				 -P ${PROJECT_SOURCE_DIR}/cmake/SearchReplace.cmake
	COMMAND ${GENHTML_PATH} --rc genhtml_branch_coverage=1
	                        --function-coverage --branch-coverage
	                        --title "Doxygen Coverage Report" --num-spaces 2
				--legend --prefix ${PROJECT_SOURCE_DIR} --demangle-cpp
				--output-directory cov_output cov.info.final
        COMMAND ${CMAKE_COMMAND} -E remove cov.info cov.info.cleaned cov.info.final
	WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  )
  add_custom_command(TARGET coverage POST_BUILD
	COMMAND ;
	COMMENT "Open ./cov_output/index.html in your browser to view the coverage report"
  )
endif()

function(set_project_coverage project_name)
  if(enable_coverage)
    target_compile_options(${project_name} PRIVATE ${COVERAGE_COMPILER_FLAGS})
  endif()
endfunction()

