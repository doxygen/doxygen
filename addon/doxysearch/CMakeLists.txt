if (build_search)

find_package(Xapian REQUIRED)
find_package(ZLIB REQUIRED)

include_directories(
	${CMAKE_SOURCE_DIR}/qtools
	${XAPIAN_INCLUDE_DIR}
        ${ZLIB_INCLUDE_DIRS}
)
add_executable(doxyindexer
               doxyindexer.cpp
)
target_link_libraries(doxyindexer
                      ${XAPIAN_LIBRARIES}
                      ${ZLIB_LIBRARIES}
                      qtools
)

add_executable(doxysearch.cgi
               doxysearch.cpp
)
target_link_libraries(doxysearch.cgi
                      ${XAPIAN_LIBRARIES}
                      ${ZLIB_LIBRARIES}
)

install(TARGETS doxyindexer doxysearch.cgi DESTINATION bin)

endif()
