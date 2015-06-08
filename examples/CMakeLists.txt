if (build_examples)

file(COPY ${EXAMPLE_DIR} DESTINATION ${PROJECT_BINARY_DIR})

# class
#add_custom_target(examples
#	COMMENT "Generating documentation for examples"
#	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen class.cfg
#	DEPENDS doxygen class.h class.cfg
#	OUTPUT class/html/index.html
#	WORKING_DIRECTORY examples
#	VERBATIM
#)

add_custom_target(examples
	DEPENDS doxygen
                class/html/index.html
                define/html/index.html
                enum/html/index.html
                file/html/index.html
                func/html/index.html
                page/html/index.html
                relates/html/index.html
                author/html/index.html
                par/html/index.html
                overload/html/index.html
                example/html/index.html
                include/html/index.html
                qtstyle/html/index.html
                jdstyle/html/index.html
                structcmd/html/index.html
                autolink/html/index.html
                restypedef/html/index.html
                afterdoc/html/index.html
                template/html/index.html
                tag/html/index.html
                group/html/index.html
                diagrams/html/index.html
                memgrp/html/index.html
                docstring/html/index.html
                pyexample/html/index.html
                tclexample/html/index.html
                mux/html/index.html
                manual/html/index.html
	WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/examples
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen class.cfg
	DEPENDS doxygen class.h class.cfg
	OUTPUT class/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen define.cfg
	DEPENDS doxygen define.h define.cfg
	OUTPUT define/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen enum.cfg
	DEPENDS doxygen enum.h enum.cfg
	OUTPUT enum/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen file.cfg
	DEPENDS doxygen file.h file.cfg
	OUTPUT file/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen func.cfg
	DEPENDS doxygen func.h func.cfg
	OUTPUT func/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen page.cfg
	DEPENDS doxygen page.doc page.cfg
	OUTPUT page/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen relates.cfg
	DEPENDS doxygen relates.cpp relates.cfg
	OUTPUT relates/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen author.cfg
	DEPENDS doxygen author.cpp author.cfg
	OUTPUT author/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen par.cfg
	DEPENDS doxygen par.cpp par.cfg
	OUTPUT par/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen overload.cfg
	DEPENDS doxygen overload.cpp overload.cfg
	OUTPUT overload/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen example.cfg
	DEPENDS doxygen example.cpp example_test.cpp example.cfg
	OUTPUT example/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen include.cfg
	DEPENDS doxygen include.cpp example_test.cpp include.cfg
	OUTPUT include/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen qtstyle.cfg
	DEPENDS doxygen qtstyle.cpp qtstyle.cfg
	OUTPUT qtstyle/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen jdstyle.cfg
	DEPENDS doxygen jdstyle.cpp jdstyle.cfg
	OUTPUT jdstyle/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen structcmd.cfg
	DEPENDS doxygen structcmd.h structcmd.cfg
	OUTPUT structcmd/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen autolink.cfg
	DEPENDS doxygen autolink.cpp autolink.cfg
	OUTPUT autolink/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen tag.cfg
	DEPENDS doxygen tag.cpp tag.cfg example/html/index.html
	OUTPUT tag/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen restypedef.cfg
	DEPENDS doxygen restypedef.cpp restypedef.cfg
	OUTPUT restypedef/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen afterdoc.cfg
	DEPENDS doxygen afterdoc.h afterdoc.cfg
	OUTPUT afterdoc/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen templ.cfg
	DEPENDS doxygen templ.cpp templ.cfg
	OUTPUT template/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen group.cfg
	DEPENDS doxygen group.cpp group.cfg
	OUTPUT group/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen memgrp.cfg
	DEPENDS doxygen memgrp.cpp memgrp.cfg
	OUTPUT memgrp/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen pyexample.cfg
	DEPENDS doxygen pyexample.py pyexample.cfg
	OUTPUT pyexample/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen tclexample.cfg
	DEPENDS doxygen tclexample.tcl tclexample.cfg
	OUTPUT tclexample/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen mux.cfg
	DEPENDS doxygen mux.vhdl mux.cfg
	OUTPUT mux/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen manual.cfg
	DEPENDS doxygen manual.c manual.cfg
	OUTPUT manual/html/index.html
)

add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen docstring.cfg
	DEPENDS doxygen docstring.py docstring.cfg
	OUTPUT docstring/html/index.html
)

if (DOT)
  add_custom_command(
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/doxygen diagrams.cfg
	DEPENDS doxygen diagrams_a.h diagrams_b.h diagrams_c.h diagrams_d.h diagrams_e.h diagrams.cfg
	OUTPUT diagrams/html/index.html
  )
endif(DOT)

endif()
