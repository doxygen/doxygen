if (build_search)

find_package(Xapian REQUIRED)

include_directories(
	${CMAKE_SOURCE_DIR}/qtools
	${XAPIAN_INCLUDE_DIR}
)
add_executable(doxyindexer
doxyindexer.cpp
)
target_link_libraries(doxyindexer
${XAPIAN_LIBRARIES}
qtools
)

add_executable(doxysearch.cgi
doxysearch.cpp
)
target_link_libraries(doxysearch.cgi
${XAPIAN_LIBRARIES}
)

endif()
