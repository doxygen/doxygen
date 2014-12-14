if (build_wizard)

include_directories(
	.
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_SOURCE_DIR}/qtools
	${GENERATED_SRC}
)

add_definitions(-DQT_ARCH_X86_64 -DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII)
set(QT_USE_QTXML TRUE)
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

add_custom_command(
COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/src/configgen.py -wiz ${CMAKE_SOURCE_DIR}/src/config.xml > ${GENERATED_SRC}/configdoc.cpp
OUTPUT ${GENERATED_SRC}/configdoc.cpp
)
set_source_files_properties(${GENERATED_SRC}/configdoc.cpp PROPERTIES GENERATED 1)

FLEX_TARGET(config_doxyw config_doxyw.l ${GENERATED_SRC}/config_doxyw.cpp COMPILE_FLAGS "-Pconfig_doxywYY")

QT4_WRAP_CPP(doxywizard_MOC
doxywizard.h
expert.h
helplabel.h
inputint.h
inputbool.h
inputstring.h
inputstrlist.h
wizard.h
)

QT4_ADD_RESOURCES(doxywizard_RESOURCES_RCC doxywizard.qrc)

add_executable(doxywizard
doxywizard.cpp
expert.cpp
wizard.cpp
inputbool.cpp
inputstring.cpp
inputint.cpp
inputstrlist.cpp
${GENERATED_SRC}/settings.h
${GENERATED_SRC}/version.cpp
${GENERATED_SRC}/config_doxyw.cpp
${GENERATED_SRC}/configdoc.cpp
${doxywizard_MOC}
${doxywizard_RESOURCES_RCC}
)
target_link_libraries(doxywizard
qtools md5 vhdlparser ${QT_LIBRARIES}
)

endif()
