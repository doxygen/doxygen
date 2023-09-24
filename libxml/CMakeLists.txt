include_directories(
    ${PROJECT_SOURCE_DIR}/libxml
)

add_custom_command(
    COMMAND ${Python_EXECUTABLE} ${PROJECT_SOURCE_DIR}/src/scan_states.py ${PROJECT_SOURCE_DIR}/libxml/xml.l > ${GENERATED_SRC}/xml.l.h
    DEPENDS ${PROJECT_SOURCE_DIR}/src/scan_states.py ${PROJECT_SOURCE_DIR}/libxml/xml.l
    OUTPUT  ${GENERATED_SRC}/xml.l.h
)
set_source_files_properties(${GENERATED_SRC}/xml.l.h PROPERTIES GENERATED 1)

FLEX_TARGET(xml xml.l ${GENERATED_SRC}/xml.cpp COMPILE_FLAGS "${LEX_FLAGS}")

add_library(xml STATIC
${GENERATED_SRC}/xml.cpp
${GENERATED_SRC}/xml.l.h
)


