find_package(xapian REQUIRED)
find_package(ZLIB REQUIRED)

if (WIN32)
  set(WIN_EXTRA_LIBS uuid.lib rpcrt4.lib ws2_32.lib)
endif()

include_directories(
        ${PROJECT_SOURCE_DIR}/libversion
        ${PROJECT_SOURCE_DIR}/qtools
        ${XAPIAN_INCLUDE_DIR}
        ${ZLIB_INCLUDE_DIRS}
)
add_executable(doxyindexer
               doxyindexer.cpp
)
target_link_libraries(doxyindexer
                      ${XAPIAN_LIBRARIES}
                      ${ZLIB_LIBRARIES}
                      ${WIN_EXTRA_LIBS}
                      ${COVERAGE_LINKER_FLAGS}
                      doxygen_version
                      qtools
)

add_executable(doxysearch.cgi
               doxysearch.cpp
)
target_link_libraries(doxysearch.cgi
                      doxygen_version
                      ${XAPIAN_LIBRARIES}
                      ${ZLIB_LIBRARIES}
                      ${WIN_EXTRA_LIBS}
)

install(TARGETS doxyindexer doxysearch.cgi DESTINATION bin)
