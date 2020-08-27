
include_directories(
	../../include
)

add_executable(doxmlparser_metrics
main.cpp
)


target_link_libraries(doxmlparser_metrics
	doxmlparser
	qtools
        ${COVERAGE_LINKER_FLAGS}
)

