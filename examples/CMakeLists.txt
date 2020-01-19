file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/examples
                    ${PROJECT_BINARY_DIR}/html/examples)
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

set(BASIC_EXAMPLES
                class:h
                define:h
                enum:h
                file:h
                func:h
                javadoc-banner:h
                structcmd:h
                afterdoc:h
                relates:cpp
                author:cpp
                par:cpp
                overload:cpp
                example:cpp
                include:cpp
                qtstyle:cpp
                jdstyle:cpp
                autolink:cpp
                restypedef:cpp
                tag:cpp
                group:cpp
                memgrp:cpp
                templ:cpp
                page:doc
                docstring:py
                pyexample:py
                tclexample:tcl
                manual:c
                mux:vhdl
)

foreach (f_inp  ${BASIC_EXAMPLES})
        string(REGEX REPLACE ".*:" "" f_ext ${f_inp})
        string(REGEX REPLACE ":.*" "" f ${f_inp})
    add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen ${f}.cfg
        COMMAND ${PYTHON_EXECUTABLE}  ${TOP}/examples/strip_example.py  < ${PROJECT_BINARY_DIR}/html/examples/${f}/latex/refman.tex > ${PROJECT_BINARY_DIR}/html/examples/${f}/latex/refman_doc.tex
	DEPENDS doxygen ${f}.${f_ext} ${f}.cfg ${TOP}/examples/strip_example.py
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/${f}/html/index.html ${PROJECT_BINARY_DIR}/html/examples/${f}/latex/refman_doc.tex
    )
    set(EXAMPLES_RES ${EXAMPLES_RES} "" ${PROJECT_BINARY_DIR}/html/examples/${f}/html/index.html)
endforeach()

add_custom_target(examples
  DEPENDS ${OUT_EXAMPLE_FILES}
                ${EXAMPLES_RES}
                ${DIAGRAM_EXAMPLE}
	WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/examples
)

if (DOT)
  add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen diagrams.cfg
        COMMAND ${PYTHON_EXECUTABLE}  ${TOP}/examples/strip_example.py  < ${PROJECT_BINARY_DIR}/html/examples/diagrams/latex/refman.tex > ${PROJECT_BINARY_DIR}/html/examples/diagrams/latex/refman_doc.tex
	DEPENDS doxygen diagrams_a.h diagrams_b.h diagrams_c.h diagrams_d.h diagrams_e.h diagrams.cfg ${TOP}/examples/strip_example.py
	OUTPUT ${PROJECT_BINARY_DIR}/html/examples/diagrams/html/index.html ${PROJECT_BINARY_DIR}/html/examples/diagrams/latex/refman_doc.tex
  )
endif(DOT)
