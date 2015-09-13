if (build_doc)

file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/examples)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/html/examples)
file(GLOB EXAMPLE_FILES RELATIVE ${CMAKE_SOURCE_DIR}/examples "*")

if (DOT)
  set(DIAGRAM_EXAMPLE ${PROJECT_BINARY_DIR}/html/examples/diagrams/html/index.html)
endif()

foreach (f  ${EXAMPLE_FILES})
    add_custom_command(
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/examples/${f}" "${PROJECT_BINARY_DIR}/examples/"
        DEPENDS "${CMAKE_SOURCE_DIR}/examples/${f}"
        OUTPUT "${PROJECT_BINARY_DIR}/examples/${f}"
    )
    set_source_files_properties("${PROJECT_BINARY_DIR}/examples/${f}" PROPERTIES GENERATED 1)
    list(APPEND OUT_EXAMPLE_FILES "${PROJECT_BINARY_DIR}/examples/${f}")
endforeach()

add_custom_target(examples
  DEPENDS ${OUT_EXAMPLE_FILES}
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
        COMMAND ${PYTHON_EXECUTABLE}  ${TOP}/examples/strip_example.py  < ${PROJECT_BINARY_DIR}/html/examples/class/latex/refman.tex > ${PROJECT_BINARY_DIR}/html/examples/class/latex/refman_doc.tex
	DEPENDS doxygen class.h class.cfg ${TOP}/examples/strip_example.py
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/class/html/index.html ${PROJECT_BINARY_DIR}/html/examples/class/latex/refman_doc.tex
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen define.cfg
        COMMAND ${PYTHON_EXECUTABLE}  ${TOP}/examples/strip_example.py  < ${PROJECT_BINARY_DIR}/html/examples/define/latex/refman.tex > ${PROJECT_BINARY_DIR}/html/examples/define/latex/refman_doc.tex
	DEPENDS doxygen define.h define.cfg ${TOP}/examples/strip_example.py
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/define/html/index.html ${PROJECT_BINARY_DIR}/html/examples/define/latex/refman_doc.tex
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen enum.cfg
        COMMAND ${PYTHON_EXECUTABLE}  ${TOP}/examples/strip_example.py  < ${PROJECT_BINARY_DIR}/html/examples/enum/latex/refman.tex > ${PROJECT_BINARY_DIR}/html/examples/enum/latex/refman_doc.tex
	DEPENDS doxygen enum.h enum.cfg ${TOP}/examples/strip_example.py
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/enum/html/index.html ${PROJECT_BINARY_DIR}/html/examples/enum/latex/refman_doc.tex
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen file.cfg
        COMMAND ${PYTHON_EXECUTABLE}  ${TOP}/examples/strip_example.py  < ${PROJECT_BINARY_DIR}/html/examples/file/latex/refman.tex > ${PROJECT_BINARY_DIR}/html/examples/file/latex/refman_doc.tex
	DEPENDS doxygen file.h file.cfg ${TOP}/examples/strip_example.py
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/file/html/index.html ${PROJECT_BINARY_DIR}/html/examples/file/latex/refman_doc.tex
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen func.cfg
        COMMAND ${PYTHON_EXECUTABLE}  ${TOP}/examples/strip_example.py  < ${PROJECT_BINARY_DIR}/html/examples/func/latex/refman.tex > ${PROJECT_BINARY_DIR}/html/examples/func/latex/refman_doc.tex
	DEPENDS doxygen func.h func.cfg ${TOP}/examples/strip_example.py
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/func/html/index.html ${PROJECT_BINARY_DIR}/html/examples/func/latex/refman_doc.tex
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen page.cfg
        COMMAND ${PYTHON_EXECUTABLE}  ${TOP}/examples/strip_example.py  < ${PROJECT_BINARY_DIR}/html/examples/page/latex/refman.tex > ${PROJECT_BINARY_DIR}/html/examples/page/latex/refman_doc.tex
	DEPENDS doxygen page.doc page.cfg ${TOP}/examples/strip_example.py
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/page/html/index.html ${PROJECT_BINARY_DIR}/html/examples/page/latex/refman_doc.tex
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen relates.cfg
        COMMAND ${PYTHON_EXECUTABLE}  ${TOP}/examples/strip_example.py  < ${PROJECT_BINARY_DIR}/html/examples/relates/latex/refman.tex > ${PROJECT_BINARY_DIR}/html/examples/relates/latex/refman_doc.tex
	DEPENDS doxygen relates.cpp relates.cfg ${TOP}/examples/strip_example.py
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/relates/html/index.html ${PROJECT_BINARY_DIR}/html/examples/relates/latex/refman_doc.tex
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen author.cfg
        COMMAND ${PYTHON_EXECUTABLE}  ${TOP}/examples/strip_example.py  < ${PROJECT_BINARY_DIR}/html/examples/author/latex/refman.tex > ${PROJECT_BINARY_DIR}/html/examples/author/latex/refman_doc.tex
	DEPENDS doxygen author.cpp author.cfg ${TOP}/examples/strip_example.py
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/author/html/index.html ${PROJECT_BINARY_DIR}/html/examples/author/latex/refman_doc.tex
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen par.cfg
        COMMAND ${PYTHON_EXECUTABLE}  ${TOP}/examples/strip_example.py  < ${PROJECT_BINARY_DIR}/html/examples/par/latex/refman.tex > ${PROJECT_BINARY_DIR}/html/examples/par/latex/refman_doc.tex
	DEPENDS doxygen par.cpp par.cfg ${TOP}/examples/strip_example.py
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/par/html/index.html ${PROJECT_BINARY_DIR}/html/examples/par/latex/refman_doc.tex
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen overload.cfg
        COMMAND ${PYTHON_EXECUTABLE}  ${TOP}/examples/strip_example.py  < ${PROJECT_BINARY_DIR}/html/examples/overload/latex/refman.tex > ${PROJECT_BINARY_DIR}/html/examples/overload/latex/refman_doc.tex
	DEPENDS doxygen overload.cpp overload.cfg ${TOP}/examples/strip_example.py
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/overload/html/index.html ${PROJECT_BINARY_DIR}/html/examples/overload/latex/refman_doc.tex
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen example.cfg
        COMMAND ${PYTHON_EXECUTABLE}  ${TOP}/examples/strip_example.py  < ${PROJECT_BINARY_DIR}/html/examples/example/latex/refman.tex > ${PROJECT_BINARY_DIR}/html/examples/example/latex/refman_doc.tex
	DEPENDS doxygen example.cpp example_test.cpp example.cfg ${TOP}/examples/strip_example.py
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/example/html/index.html ${PROJECT_BINARY_DIR}/html/examples/example/latex/refman_doc.tex
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen include.cfg
        COMMAND ${PYTHON_EXECUTABLE}  ${TOP}/examples/strip_example.py  < ${PROJECT_BINARY_DIR}/html/examples/include/latex/refman.tex > ${PROJECT_BINARY_DIR}/html/examples/include/latex/refman_doc.tex
	DEPENDS doxygen include.cpp include_test.cpp include.cfg ${TOP}/examples/strip_example.py
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/include/html/index.html ${PROJECT_BINARY_DIR}/html/examples/include/latex/refman_doc.tex
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen qtstyle.cfg
        COMMAND ${PYTHON_EXECUTABLE}  ${TOP}/examples/strip_example.py  < ${PROJECT_BINARY_DIR}/html/examples/qtstyle/latex/refman.tex > ${PROJECT_BINARY_DIR}/html/examples/qtstyle/latex/refman_doc.tex
	DEPENDS doxygen qtstyle.cpp qtstyle.cfg ${TOP}/examples/strip_example.py
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/qtstyle/html/index.html ${PROJECT_BINARY_DIR}/html/examples/qtstyle/latex/refman_doc.tex
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen jdstyle.cfg
        COMMAND ${PYTHON_EXECUTABLE}  ${TOP}/examples/strip_example.py  < ${PROJECT_BINARY_DIR}/html/examples/jdstyle/latex/refman.tex > ${PROJECT_BINARY_DIR}/html/examples/jdstyle/latex/refman_doc.tex
	DEPENDS doxygen jdstyle.cpp jdstyle.cfg ${TOP}/examples/strip_example.py
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/jdstyle/html/index.html ${PROJECT_BINARY_DIR}/html/examples/jdstyle/latex/refman_doc.tex
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen structcmd.cfg
        COMMAND ${PYTHON_EXECUTABLE}  ${TOP}/examples/strip_example.py  < ${PROJECT_BINARY_DIR}/html/examples/structcmd/latex/refman.tex > ${PROJECT_BINARY_DIR}/html/examples/structcmd/latex/refman_doc.tex
	DEPENDS doxygen structcmd.h structcmd.cfg ${TOP}/examples/strip_example.py
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/structcmd/html/index.html ${PROJECT_BINARY_DIR}/html/examples/structcmd/latex/refman_doc.tex
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen autolink.cfg
        COMMAND ${PYTHON_EXECUTABLE}  ${TOP}/examples/strip_example.py  < ${PROJECT_BINARY_DIR}/html/examples/autolink/latex/refman.tex > ${PROJECT_BINARY_DIR}/html/examples/autolink/latex/refman_doc.tex
	DEPENDS doxygen autolink.cpp autolink.cfg ${TOP}/examples/strip_example.py
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/autolink/html/index.html ${PROJECT_BINARY_DIR}/html/examples/autolink/latex/refman_doc.tex
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen tag.cfg
	DEPENDS doxygen tag.cpp tag.cfg ${PROJECT_BINARY_DIR}/html/examples/example/html/index.html ${TOP}/examples/strip_example.py
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/tag/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen restypedef.cfg
        COMMAND ${PYTHON_EXECUTABLE}  ${TOP}/examples/strip_example.py  < ${PROJECT_BINARY_DIR}/html/examples/restypedef/latex/refman.tex > ${PROJECT_BINARY_DIR}/html/examples/restypedef/latex/refman_doc.tex
	DEPENDS doxygen restypedef.cpp restypedef.cfg ${TOP}/examples/strip_example.py
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/restypedef/html/index.html ${PROJECT_BINARY_DIR}/html/examples/restypedef/latex/refman_doc.tex
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen afterdoc.cfg
        COMMAND ${PYTHON_EXECUTABLE}  ${TOP}/examples/strip_example.py  < ${PROJECT_BINARY_DIR}/html/examples/afterdoc/latex/refman.tex > ${PROJECT_BINARY_DIR}/html/examples/afterdoc/latex/refman_doc.tex
	DEPENDS doxygen afterdoc.h afterdoc.cfg ${TOP}/examples/strip_example.py
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/afterdoc/html/index.html ${PROJECT_BINARY_DIR}/html/examples/afterdoc/latex/refman_doc.tex
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen templ.cfg
	DEPENDS doxygen templ.cpp templ.cfg ${TOP}/examples/strip_example.py
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/template/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen group.cfg
        COMMAND ${PYTHON_EXECUTABLE}  ${TOP}/examples/strip_example.py  < ${PROJECT_BINARY_DIR}/html/examples/group/latex/refman.tex > ${PROJECT_BINARY_DIR}/html/examples/group/latex/refman_doc.tex
	DEPENDS doxygen group.cpp group.cfg ${TOP}/examples/strip_example.py
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/group/html/index.html ${PROJECT_BINARY_DIR}/html/examples/group/latex/refman_doc.tex
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen memgrp.cfg
        COMMAND ${PYTHON_EXECUTABLE}  ${TOP}/examples/strip_example.py  < ${PROJECT_BINARY_DIR}/html/examples/memgrp/latex/refman.tex > ${PROJECT_BINARY_DIR}/html/examples/memgrp/latex/refman_doc.tex
	DEPENDS doxygen memgrp.cpp memgrp.cfg ${TOP}/examples/strip_example.py
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/memgrp/html/index.html ${PROJECT_BINARY_DIR}/html/examples/memgrp/latex/refman_doc.tex
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen pyexample.cfg
        COMMAND ${PYTHON_EXECUTABLE}  ${TOP}/examples/strip_example.py  < ${PROJECT_BINARY_DIR}/html/examples/pyexample/latex/refman.tex > ${PROJECT_BINARY_DIR}/html/examples/pyexample/latex/refman_doc.tex
	DEPENDS doxygen pyexample.py pyexample.cfg ${TOP}/examples/strip_example.py
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/pyexample/html/index.html ${PROJECT_BINARY_DIR}/html/examples/pyexample/latex/refman_doc.tex
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen tclexample.cfg
        COMMAND ${PYTHON_EXECUTABLE}  ${TOP}/examples/strip_example.py  < ${PROJECT_BINARY_DIR}/html/examples/tclexample/latex/refman.tex > ${PROJECT_BINARY_DIR}/html/examples/tclexample/latex/refman_doc.tex
	DEPENDS doxygen tclexample.tcl tclexample.cfg ${TOP}/examples/strip_example.py
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/tclexample/html/index.html ${PROJECT_BINARY_DIR}/html/examples/tclexample/latex/refman_doc.tex
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen mux.cfg
        COMMAND ${PYTHON_EXECUTABLE}  ${TOP}/examples/strip_example.py  < ${PROJECT_BINARY_DIR}/html/examples/mux/latex/refman.tex > ${PROJECT_BINARY_DIR}/html/examples/mux/latex/refman_doc.tex
	DEPENDS doxygen mux.vhdl mux.cfg ${TOP}/examples/strip_example.py
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/mux/html/index.html ${PROJECT_BINARY_DIR}/html/examples/mux/latex/refman_doc.tex
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen manual.cfg
        COMMAND ${PYTHON_EXECUTABLE}  ${TOP}/examples/strip_example.py  < ${PROJECT_BINARY_DIR}/html/examples/manual/latex/refman.tex > ${PROJECT_BINARY_DIR}/html/examples/manual/latex/refman_doc.tex
	DEPENDS doxygen manual.c manual.cfg ${TOP}/examples/strip_example.py
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/manual/html/index.html ${PROJECT_BINARY_DIR}/html/examples/manual/latex/refman_doc.tex
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen docstring.cfg
        COMMAND ${PYTHON_EXECUTABLE}  ${TOP}/examples/strip_example.py  < ${PROJECT_BINARY_DIR}/html/examples/docstring/latex/refman.tex > ${PROJECT_BINARY_DIR}/html/examples/docstring/latex/refman_doc.tex
	DEPENDS doxygen docstring.py docstring.cfg ${TOP}/examples/strip_example.py
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/docstring/html/index.html ${PROJECT_BINARY_DIR}/html/examples/docstring/latex/refman_doc.tex
)

if (DOT)
  add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen diagrams.cfg
        COMMAND ${PYTHON_EXECUTABLE}  ${TOP}/examples/strip_example.py  < ${PROJECT_BINARY_DIR}/html/examples/diagrams/latex/refman.tex > ${PROJECT_BINARY_DIR}/html/examples/diagrams/latex/refman_doc.tex
	DEPENDS doxygen diagrams_a.h diagrams_b.h diagrams_c.h diagrams_d.h diagrams_e.h diagrams.cfg ${TOP}/examples/strip_example.py
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/diagrams/html/index.html ${PROJECT_BINARY_DIR}/html/examples/diagrams/latex/refman_doc.tex
  )
endif(DOT)

endif()
