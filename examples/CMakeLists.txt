if (build_doc)

file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/examples)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/html/examples)
file(GLOB EXAMPLE_FILES "*")

if (DOT)
  set(DIAGRAM_EXAMPLE ${PROJECT_BINARY_DIR}/html/examples/diagrams/html/index.html)
endif()

# copy all example files
add_custom_target(copy_examples)
foreach(example_file ${EXAMPLE_FILES})
  add_custom_command(TARGET copy_examples PRE_BUILD
                     COMMAND ${CMAKE_COMMAND} -E
                         copy ${example_file} ${PROJECT_BINARY_DIR}/examples)
endforeach()

add_custom_target(examples
	DEPENDS doxygen copy_examples
                ${PROJECT_BINARY_DIR}/html/examples/class/html/index.html
                ${PROJECT_BINARY_DIR}/html/examples/define/html/index.html
                ${PROJECT_BINARY_DIR}/html/examples/enum/html/index.html
                ${PROJECT_BINARY_DIR}/html/examples/file/html/index.html
                ${PROJECT_BINARY_DIR}/html/examples/func/html/index.html
                ${PROJECT_BINARY_DIR}/html/examples/page/html/index.html
                ${PROJECT_BINARY_DIR}/html/examples/relates/html/index.html
                ${PROJECT_BINARY_DIR}/html/examples/author/html/index.html
                ${PROJECT_BINARY_DIR}/html/examples/par/html/index.html
                ${PROJECT_BINARY_DIR}/html/examples/overload/html/index.html
                ${PROJECT_BINARY_DIR}/html/examples/example/html/index.html
                ${PROJECT_BINARY_DIR}/html/examples/include/html/index.html
                ${PROJECT_BINARY_DIR}/html/examples/qtstyle/html/index.html
                ${PROJECT_BINARY_DIR}/html/examples/jdstyle/html/index.html
                ${PROJECT_BINARY_DIR}/html/examples/structcmd/html/index.html
                ${PROJECT_BINARY_DIR}/html/examples/autolink/html/index.html
                ${PROJECT_BINARY_DIR}/html/examples/restypedef/html/index.html
                ${PROJECT_BINARY_DIR}/html/examples/afterdoc/html/index.html
                ${PROJECT_BINARY_DIR}/html/examples/template/html/index.html
                ${PROJECT_BINARY_DIR}/html/examples/tag/html/index.html
                ${PROJECT_BINARY_DIR}/html/examples/group/html/index.html
                ${PROJECT_BINARY_DIR}/html/examples/memgrp/html/index.html
                ${PROJECT_BINARY_DIR}/html/examples/docstring/html/index.html
                ${PROJECT_BINARY_DIR}/html/examples/pyexample/html/index.html
                ${PROJECT_BINARY_DIR}/html/examples/tclexample/html/index.html
                ${PROJECT_BINARY_DIR}/html/examples/mux/html/index.html
                ${PROJECT_BINARY_DIR}/html/examples/manual/html/index.html
                ${DIAGRAM_EXAMPLE}
	WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/examples
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen class.cfg
	DEPENDS doxygen class.h class.cfg
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/class/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen define.cfg
	DEPENDS doxygen define.h define.cfg
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/define/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen enum.cfg
	DEPENDS doxygen enum.h enum.cfg
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/enum/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen file.cfg
	DEPENDS doxygen file.h file.cfg
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/file/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen func.cfg
	DEPENDS doxygen func.h func.cfg
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/func/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen page.cfg
	DEPENDS doxygen page.doc page.cfg
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/page/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen relates.cfg
	DEPENDS doxygen relates.cpp relates.cfg
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/relates/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen author.cfg
	DEPENDS doxygen author.cpp author.cfg
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/author/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen par.cfg
	DEPENDS doxygen par.cpp par.cfg
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/par/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen overload.cfg
	DEPENDS doxygen overload.cpp overload.cfg
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/overload/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen example.cfg
	DEPENDS doxygen example.cpp example_test.cpp example.cfg
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/example/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen include.cfg
	DEPENDS doxygen include.cpp example_test.cpp include.cfg
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/include/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen qtstyle.cfg
	DEPENDS doxygen qtstyle.cpp qtstyle.cfg
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/qtstyle/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen jdstyle.cfg
	DEPENDS doxygen jdstyle.cpp jdstyle.cfg
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/jdstyle/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen structcmd.cfg
	DEPENDS doxygen structcmd.h structcmd.cfg
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/structcmd/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen autolink.cfg
	DEPENDS doxygen autolink.cpp autolink.cfg
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/autolink/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen tag.cfg
	DEPENDS doxygen tag.cpp tag.cfg ${PROJECT_BINARY_DIR}/html/examples/example/html/index.html
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/tag/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen restypedef.cfg
	DEPENDS doxygen restypedef.cpp restypedef.cfg
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/restypedef/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen afterdoc.cfg
	DEPENDS doxygen afterdoc.h afterdoc.cfg
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/afterdoc/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen templ.cfg
	DEPENDS doxygen templ.cpp templ.cfg
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/template/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen group.cfg
	DEPENDS doxygen group.cpp group.cfg
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/group/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen memgrp.cfg
	DEPENDS doxygen memgrp.cpp memgrp.cfg
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/memgrp/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen pyexample.cfg
	DEPENDS doxygen pyexample.py pyexample.cfg
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/pyexample/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen tclexample.cfg
	DEPENDS doxygen tclexample.tcl tclexample.cfg
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/tclexample/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen mux.cfg
	DEPENDS doxygen mux.vhdl mux.cfg
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/mux/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen manual.cfg
	DEPENDS doxygen manual.c manual.cfg
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/manual/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen docstring.cfg
	DEPENDS doxygen docstring.py docstring.cfg
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/docstring/html/index.html
)

if (DOT)
  add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen diagrams.cfg
	DEPENDS doxygen diagrams_a.h diagrams_b.h diagrams_c.h diagrams_d.h diagrams_e.h diagrams.cfg
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/diagrams/html/index.html
  )
endif(DOT)

endif()
