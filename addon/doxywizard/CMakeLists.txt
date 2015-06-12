if (build_wizard)

include_directories(
	.
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_SOURCE_DIR}/qtools
	${GENERATED_SRC}
)

set(GENERATED_SRC_WIZARD ${GENERATED_SRC}/doxywizard)
file(MAKE_DIRECTORY ${GENERATED_SRC_WIZARD})

add_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Xml REQUIRED)

# generate settings.h
file(GENERATE OUTPUT ${GENERATED_SRC_WIZARD}/settings.h
CONTENT "#ifndef SETTINGS_H
#define SETTINGS_H
#define USE_SQLITE3 ${sqlite3}
#define USE_LIBCLANG ${clang}
#define IS_SUPPORTED(x) \\
  ((USE_SQLITE3  && strcmp(\"USE_SQLITE3\",(x))==0)  || \\
   (USE_LIBCLANG && strcmp(\"USE_LIBCLANG\",(x))==0) || \\
  0)
#endif" )
set_source_files_properties(${GENERATED_SRC_WIZARD}/settings.h PROPERTIES GENERATED 1)

# generate version.cpp
file(GENERATE OUTPUT ${GENERATED_SRC_WIZARD}/version.cpp
    CONTENT "char versionString[]=\"${VERSION}\";"
)
set_source_files_properties(${GENERATED_SRC_WIZARD}/version.cpp PROPERTIES GENERATED 1)

# generate configdoc.cpp
add_custom_command(
COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/src/configgen.py -wiz ${CMAKE_SOURCE_DIR}/src/config.xml > ${GENERATED_SRC_WIZARD}/configdoc.cpp
OUTPUT ${GENERATED_SRC_WIZARD}/configdoc.cpp
)
set_source_files_properties(${GENERATED_SRC_WIZARD}/configdoc.cpp PROPERTIES GENERATED 1)

FLEX_TARGET(config_doxyw config_doxyw.l ${GENERATED_SRC_WIZARD}/config_doxyw.cpp COMPILE_FLAGS "-Pconfig_doxywYY")

set(CMAKE_AUTOMOC ON)

qt5_add_resources(doxywizard_RESOURCES_RCC doxywizard.qrc)

add_executable(doxywizard
doxywizard.cpp
expert.cpp
wizard.cpp
helplabel.cpp
inputbool.cpp
inputstring.cpp
inputint.cpp
inputstrlist.cpp
${GENERATED_SRC_WIZARD}/settings.h
${GENERATED_SRC_WIZARD}/version.cpp
${GENERATED_SRC_WIZARD}/config_doxyw.cpp
${GENERATED_SRC_WIZARD}/configdoc.cpp
${doxywizard_RESOURCES_RCC}
)
target_link_libraries(doxywizard)
qt5_use_modules(doxywizard LINK_PUBLIC Widgets Xml)

install(TARGETS doxywizard DESTINATION bin)

endif()
