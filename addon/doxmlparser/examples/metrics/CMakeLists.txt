
include_directories(
	../../include
	${PROJECT_SOURCE_DIR}/libversion
)

add_executable(doxmlparser_metrics
main.cpp
)


target_link_libraries(doxmlparser_metrics
	doxmlparser
	doxygen_version
	qtools
        ${COVERAGE_LINKER_FLAGS}
)

