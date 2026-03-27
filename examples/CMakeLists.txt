file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/examples
                    ${PROJECT_BINARY_DIR}/html/examples
                    ${PROJECT_BINARY_DIR}/latex/examples)
file(GLOB EXAMPLE_FILES RELATIVE ${PROJECT_SOURCE_DIR}/examples "*")

if (build_doc_chm)
  file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/chm/html/examples)
endif()

if (DOT)
  set(DIAGRAM_EXAMPLE ${PROJECT_BINARY_DIR}/html/examples/diagrams/html/index.html)
  if (build_doc_chm)
    set(DIAGRAM_CHM_EXAMPLE ${PROJECT_BINARY_DIR}/chm/html/examples/diagrams/html/index.html)
  endif()
endif()

foreach (f  ${EXAMPLE_FILES})
  configure_file(${PROJECT_SOURCE_DIR}/examples/${f} ${PROJECT_BINARY_DIR}/examples/${f} COPYONLY)
  set_source_files_properties("${PROJECT_BINARY_DIR}/examples/${f}" PROPERTIES GENERATED 1)
endforeach()

foreach (f  ${EXAMPLE_FILES})
  list(APPEND OUT_EXAMPLE_FILES "${PROJECT_BINARY_DIR}/examples/${f}")
endforeach()

# name                 file           extension   dependency (optional)
set(EXAMPLE_class      class          h)
set(EXAMPLE_define     define         h)
set(EXAMPLE_enum       enum           h)
set(EXAMPLE_file       file           h)
set(EXAMPLE_func       func           h)
set(EXAMPLE_javadoc    javadoc-banner h)
set(EXAMPLE_structcmd  structcmd      h)
set(EXAMPLE_afterdoc   afterdoc       h)
set(EXAMPLE_relates    relates        cpp)
set(EXAMPLE_author     author         cpp)
set(EXAMPLE_par        par            cpp)
set(EXAMPLE_overload   overload       cpp)
set(EXAMPLE_example    example        cpp)
set(EXAMPLE_include    include        cpp)
set(EXAMPLE_qtstyle    qtstyle        cpp)
set(EXAMPLE_jdstyle    jdstyle        cpp)
set(EXAMPLE_autolink   autolink       cpp)
set(EXAMPLE_restypedef restypedef     cpp)
set(EXAMPLE_tag        tag            cpp        example)
set(EXAMPLE_group      group          cpp)
set(EXAMPLE_memgrp     memgrp         cpp)
set(EXAMPLE_templ      templ          cpp)
set(EXAMPLE_page       page           doc)
set(EXAMPLE_docstring  docstring      py)
set(EXAMPLE_pyexample  pyexample      py)
set(EXAMPLE_manual     manual         c)
set(EXAMPLE_mux        mux            vhdl)

set(BASIC_EXAMPLES
  EXAMPLE_class   EXAMPLE_define    EXAMPLE_enum       EXAMPLE_file      EXAMPLE_func
  EXAMPLE_javadoc EXAMPLE_structcmd EXAMPLE_afterdoc   EXAMPLE_relates   EXAMPLE_author
  EXAMPLE_par     EXAMPLE_overload  EXAMPLE_example    EXAMPLE_include   EXAMPLE_qtstyle
  EXAMPLE_jdstyle EXAMPLE_autolink  EXAMPLE_restypedef EXAMPLE_tag       EXAMPLE_group
  EXAMPLE_memgrp  EXAMPLE_templ     EXAMPLE_page       EXAMPLE_docstring EXAMPLE_pyexample
  EXAMPLE_manual  EXAMPLE_mux
)

configure_file(${PROJECT_SOURCE_DIR}/templates/icon/doxygen.ico ${PROJECT_BINARY_DIR}/examples/doxygen.ico COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/doc/doxygen_manual.css ${PROJECT_BINARY_DIR}/examples/doxygen_manual.css COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/doc/doxygen_manual_chm.css ${PROJECT_BINARY_DIR}/examples/doxygen_manual_chm.css COPYONLY)

foreach (f_inp  ${BASIC_EXAMPLES})
        list(LENGTH ${f_inp} f_len)
        list(GET    ${f_inp} 0 f)
        list(GET    ${f_inp} 1 f_ext)
        set(f_dep "")
        set(f_dep_chm "")
        if (${f_len} GREATER 2)
          list(GET  ${f_inp} 2 f_dep)
          list(GET  ${f_inp} 2 f_dep_chm)
          set(f_dep "${PROJECT_BINARY_DIR}/html/examples/${f_dep}/html/index.html")
          if (build_doc_chm)
            set(f_dep_chm "${PROJECT_BINARY_DIR}/chm/html/examples/${f_dep_chm}/html/index.html")
          endif()
        endif()
    add_custom_command(
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/html/examples/${f}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/latex/examples/${f}
        COMMAND ${CMAKE_COMMAND} -E env PROJECT_BINARY_DIR=${PROJECT_BINARY_DIR} EXTRA_SETTINGS=docuexample.cfg ${EXECUTABLE_OUTPUT_PATH}/doxygen ${f}.cfg
        COMMAND ${Python_EXECUTABLE}  ${PROJECT_SOURCE_DIR}/examples/strip_example.py  < ${PROJECT_BINARY_DIR}/latex/examples/${f}/latex/refman.tex > ${PROJECT_BINARY_DIR}/latex/examples/${f}/latex/refman_doc.tex
        DEPENDS doxygen ${f}.${f_ext} ${f}.cfg ${PROJECT_SOURCE_DIR}/examples/strip_example.py ${f_dep} baseexample.cfg docuexample.cfg
        OUTPUT ${PROJECT_BINARY_DIR}/html/examples/${f}/html/index.html ${PROJECT_BINARY_DIR}/latex/examples/${f}/latex/refman_doc.tex
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/examples
    )
    set(EXAMPLES_RES ${EXAMPLES_RES} "" ${PROJECT_BINARY_DIR}/html/examples/${f}/html/index.html)

    if (build_doc_chm)
      add_custom_command(
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/chm/html/examples/${f}
        COMMAND ${CMAKE_COMMAND} -E env PROJECT_BINARY_DIR=${PROJECT_BINARY_DIR} EXTRA_SETTINGS=chmexample.cfg ${EXECUTABLE_OUTPUT_PATH}/doxygen ${f}.cfg
        DEPENDS doxygen ${f}.${f_ext} ${f}.cfg ${f_dep_chm} baseexample.cfg chmexample.cfg
        OUTPUT ${PROJECT_BINARY_DIR}/chm/html/examples/${f}/html/index.html
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/examples
      )
      set(EXAMPLES_CHM_RES ${EXAMPLES_CHM_RES} "" ${PROJECT_BINARY_DIR}/chm/html/examples/${f}/html/index.html)
    endif()
endforeach()

add_custom_target(examples
  DEPENDS ${OUT_EXAMPLE_FILES}
                ${EXAMPLES_RES}
                ${DIAGRAM_EXAMPLE}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/examples
)

if (build_doc_chm)
  add_custom_target(examples_chm
    DEPENDS ${OUT_EXAMPLE_FILES}
                ${EXAMPLES_CHM_RES}
                ${DIAGRAM_CHM_EXAMPLE}
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/examples
  )
endif()

if (DOT)
  add_custom_command(
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/html/examples/diagrams
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/latex/examples/diagrams
        COMMAND ${CMAKE_COMMAND} -E env PROJECT_BINARY_DIR=${PROJECT_BINARY_DIR} EXTRA_SETTINGS=docuexample.cfg ${EXECUTABLE_OUTPUT_PATH}/doxygen diagrams.cfg
        COMMAND ${Python_EXECUTABLE}  ${PROJECT_SOURCE_DIR}/examples/strip_example.py  < ${PROJECT_BINARY_DIR}/latex/examples/diagrams/latex/refman.tex > ${PROJECT_BINARY_DIR}/latex/examples/diagrams/latex/refman_doc.tex
        DEPENDS doxygen diagrams_a.h diagrams_b.h diagrams_c.h diagrams_d.h diagrams_e.h diagrams.cfg ${PROJECT_SOURCE_DIR}/examples/strip_example.py baseexample.cfg docuexample.cfg
        OUTPUT ${PROJECT_BINARY_DIR}/html/examples/diagrams/html/index.html ${PROJECT_BINARY_DIR}/latex/examples/diagrams/latex/refman_doc.tex
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/examples
  )
  if (build_doc_chm)
    add_custom_command(
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/chm/html/examples/diagrams
        COMMAND ${CMAKE_COMMAND} -E env PROJECT_BINARY_DIR=${PROJECT_BINARY_DIR} EXTRA_SETTINGS=chmexample.cfg ${EXECUTABLE_OUTPUT_PATH}/doxygen diagrams.cfg
        DEPENDS doxygen diagrams_a.h diagrams_b.h diagrams_c.h diagrams_d.h diagrams_e.h diagrams.cfg baseexample.cfg chmexample.cfg
        OUTPUT ${PROJECT_BINARY_DIR}/chm/html/examples/diagrams/html/index.html
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/examples
    )
  endif()
endif(DOT)
