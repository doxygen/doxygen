find_package(XAPIAN REQUIRED)
find_package(ZLIB REQUIRED)

if(WIN32)
  set(EXTRA_LIBS uuid rpcrt4 ws2_32)
elseif(static_libxapian)
  set(EXTRA_LIBS uuid)
  if(UUID_LIB)
    add_library(uuid STATIC IMPORTED)
    set_target_properties(uuid PROPERTIES IMPORTED_LOCATION "${UUID_LIB}")
  endif()
endif()

include_directories(
        ${PROJECT_SOURCE_DIR}/libversion
        ${PROJECT_SOURCE_DIR}/libxml
        ${XAPIAN_INCLUDE_DIR}
        ${ZLIB_INCLUDE_DIRS}
)
add_executable(doxyindexer
               doxyindexer.cpp
               ${PROJECT_SOURCE_DIR}/templates/icon/doxygen.rc
)

target_link_libraries(doxyindexer
	              PRIVATE
                      ${XAPIAN_LIBRARIES}
                      ${ZLIB_LIBRARIES}
                      ${EXTRA_LIBS}
                      ${COVERAGE_LINKER_FLAGS}
                      doxygen_version
		      xml
)

add_executable(doxysearch.cgi
               doxysearch.cpp
               ${PROJECT_SOURCE_DIR}/templates/icon/doxygen.rc
)

target_link_libraries(doxysearch.cgi
	              PRIVATE
                      doxygen_version
                      ${XAPIAN_LIBRARIES}
                      ${ZLIB_LIBRARIES}
                      ${EXTRA_LIBS}
)

include(ApplyEditbin)
apply_editbin(doxyindexer console)
apply_editbin(doxysearch.cgi console)


install(TARGETS doxyindexer doxysearch.cgi DESTINATION bin)
