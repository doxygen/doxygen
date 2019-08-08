# search for Qt5
if (NOT force_qt4)
  find_package(Qt5Core QUIET CONFIG)
  if (Qt5Core_FOUND)
    message(STATUS "Using Qt5")
    find_package(Qt5 COMPONENTS Widgets Gui Xml)
    macro(qt_wrap_cpp)
        qt5_wrap_cpp(${ARGN})
    endmacro()
    macro(qt_add_resources)
        qt5_add_resources(${ARGN})
    endmacro()
  endif()
endif()
# fallback to Qt4
if (NOT Qt5Core_FOUND)
  if (NOT force_qt4)
    message(STATUS "Qt5 not found, searching for Qt4 instead...")
  else()
    message(STATUS "Using Qt4")
  endif()
  find_package(Qt4 REQUIRED COMPONENTS QtCore QtXml QtGui)
  macro(qt_wrap_cpp)
      qt4_wrap_cpp(${ARGN})
  endmacro()
  macro(qt_add_resources)
      qt4_add_resources(${ARGN})
  endmacro()
endif()

include_directories(
	.
        ${CMAKE_SOURCE_DIR}/libversion
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_SOURCE_DIR}/qtools
	${GENERATED_SRC}
)

set(GENERATED_SRC_WIZARD ${GENERATED_SRC}/doxywizard)
file(MAKE_DIRECTORY ${GENERATED_SRC_WIZARD})

add_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII -DUNICODE)
if (NOT Qt5Core_FOUND)
  include(${QT_USE_FILE})
endif()

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
add_custom_command(
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/src/version.py ${VERSION} > ${GENERATED_SRC_WIZARD}/version.cpp
    DEPENDS ${CMAKE_SOURCE_DIR}/VERSION ${CMAKE_SOURCE_DIR}/src/version.py
    OUTPUT ${GENERATED_SRC_WIZARD}/version.cpp
)
set_source_files_properties(${GENERATED_SRC_WIZARD}/version.cpp PROPERTIES GENERATED 1)

# generate configdoc.cpp
add_custom_command(
COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/src/configgen.py -wiz ${CMAKE_SOURCE_DIR}/src/config.xml > ${GENERATED_SRC_WIZARD}/configdoc.cpp
OUTPUT ${GENERATED_SRC_WIZARD}/configdoc.cpp
)
set_source_files_properties(${GENERATED_SRC_WIZARD}/configdoc.cpp PROPERTIES GENERATED 1)

FLEX_TARGET(config_doxyw config_doxyw.l ${GENERATED_SRC_WIZARD}/config_doxyw.cpp COMPILE_FLAGS "${LEX_FLAGS}")

qt_wrap_cpp(doxywizard_MOC
doxywizard.h
expert.h
helplabel.h
inputint.h
inputbool.h
inputstring.h
inputstrlist.h
wizard.h
)

qt_add_resources(doxywizard_RESOURCES_RCC doxywizard.qrc)

add_executable(doxywizard WIN32
doxywizard.cpp
expert.cpp
wizard.cpp
inputbool.cpp
inputstring.cpp
inputint.cpp
inputstrlist.cpp
${GENERATED_SRC_WIZARD}/settings.h
${GENERATED_SRC_WIZARD}/config_doxyw.cpp
${GENERATED_SRC_WIZARD}/configdoc.cpp
${doxywizard_MOC}
${doxywizard_RESOURCES_RCC}
doxywizard.rc
)

if(Qt5Core_FOUND)
  target_link_libraries(doxywizard Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Xml doxygen_version)
else()
  target_link_libraries(doxywizard ${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY} doxygen_version)
endif()

install(TARGETS doxywizard DESTINATION bin)
