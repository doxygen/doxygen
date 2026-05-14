# Try finding Qt6
if (force_qt STREQUAL "Qt6" OR NOT force_qt)
  find_package(Qt6Core QUIET CONFIG)
  if (Qt6Core_FOUND)
    message(STATUS "Using Qt6")
    find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Xml Svg)
    set(QT_VERSION_MAJOR "6")
    macro(qt_wrap_cpp)
      qt6_wrap_cpp(${ARGN})
    endmacro()
    macro(qt_add_resources)
      qt6_add_resources(${ARGN})
    endmacro()
    macro(qt_add_translation)
      qt6_add_translation(${ARGN})
    endmacro()
  elseif (force_qt STREQUAL "Qt6")
    # no fallback to Qt5
    message(FATAL_ERROR "Qt6 not found")
  endif()
endif()

# Try finding Qt5
if (force_qt STREQUAL "Qt5" OR NOT Qt6_FOUND)
  find_package(Qt5Core QUIET CONFIG)
  if (Qt5Core_FOUND)
    message(STATUS "Using Qt5")
    find_package(Qt5 REQUIRED COMPONENTS Widgets Gui Xml Svg)
    set(QT_VERSION_MAJOR "5")
    macro(qt_wrap_cpp)
      qt5_wrap_cpp(${ARGN})
    endmacro()
    macro(qt_add_resources)
      qt5_add_resources(${ARGN})
    endmacro()
    macro(qt_add_translation)
      qt5_add_translation(${ARGN})
    endmacro()
  elseif (force_qt STREQUAL "Qt5")
    message(FATAL_ERROR "Qt5 not found")
  else()
    message(FATAL_ERROR "Qt5 nor Qt6 found")
  endif()
endif()

include_directories(
  .
  ${PROJECT_SOURCE_DIR}/libversion
  ${GENERATED_SRC}
)

set(GENERATED_SRC_WIZARD ${GENERATED_SRC}/doxywizard)
file(MAKE_DIRECTORY ${GENERATED_SRC_WIZARD})

add_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII -DUNICODE)
if (NOT Qt6Core_FOUND AND NOT Qt5Core_FOUND)
  include(${QT_USE_FILE})
endif()

# generate settings.h
file(GENERATE OUTPUT ${GENERATED_SRC_WIZARD}/settings.h
CONTENT "#ifndef SETTINGS_H
#define SETTINGS_H
#define USE_LIBCLANG ${clang}
#define IS_SUPPORTED(x) \\
  ( \\
   (USE_LIBCLANG && strcmp(\"USE_LIBCLANG\",(x))==0) || \\
  0)
#endif" )
set_source_files_properties(${GENERATED_SRC_WIZARD}/settings.h PROPERTIES GENERATED 1)

# generate configdoc.cpp
add_custom_command(
    COMMAND ${Python_EXECUTABLE} ${PROJECT_SOURCE_DIR}/src/configgen.py -wiz ${PROJECT_SOURCE_DIR}/src/config.xml > ${GENERATED_SRC_WIZARD}/configdoc.cpp
    DEPENDS  ${PROJECT_SOURCE_DIR}/src/configgen.py ${PROJECT_SOURCE_DIR}/src/config.xml
    OUTPUT ${GENERATED_SRC_WIZARD}/configdoc.cpp
)
set_source_files_properties(${GENERATED_SRC_WIZARD}/configdoc.cpp PROPERTIES GENERATED 1)

set(LEX_FILES config_doxyw)

if (NOT depfile_supported)
    # In case the DEPFILE possibility is not supported the complete list of lex include files for the dependency has to be used
    set(LEX_INC_FILES)
endif()

foreach(lex_file ${LEX_FILES})
    if (depfile_supported)
        add_custom_command(
            COMMAND ${Python_EXECUTABLE} ${PROJECT_SOURCE_DIR}/src/pre_lex.py ${PROJECT_SOURCE_DIR}/addon/doxywizard/${lex_file}.l  ${GENERATED_SRC_WIZARD}/${lex_file}.l ${GENERATED_SRC_WIZARD}/${lex_file}.corr ${GENERATED_SRC_WIZARD}/${lex_file}.d ${PROJECT_SOURCE_DIR}/src
            DEPENDS ${PROJECT_SOURCE_DIR}/src/pre_lex.py ${PROJECT_SOURCE_DIR}/addon/doxywizard/${lex_file}.l
            DEPFILE ${GENERATED_SRC_WIZARD}/${lex_file}.d
            OUTPUT  ${GENERATED_SRC_WIZARD}/${lex_file}.l ${GENERATED_SRC_WIZARD}/${lex_file}.corr ${GENERATED_SRC_WIZARD}/${lex_file}.d
        )
    else()
        add_custom_command(
            COMMAND ${Python_EXECUTABLE} ${PROJECT_SOURCE_DIR}/src/pre_lex.py ${PROJECT_SOURCE_DIR}/addon/doxywizard/${lex_file}.l  ${GENERATED_SRC_WIZARD}/${lex_file}.l ${GENERATED_SRC_WIZARD}/${lex_file}.corr ${GENERATED_SRC_WIZARD}/${lex_file}.d ${PROJECT_SOURCE_DIR}/src
            DEPENDS ${PROJECT_SOURCE_DIR}/src/pre_lex.py ${PROJECT_SOURCE_DIR}/addon/doxywizard/${lex_file}.l ${LEX_INC_FILES}
            OUTPUT  ${GENERATED_SRC_WIZARD}/${lex_file}.l ${GENERATED_SRC_WIZARD}/${lex_file}.corr ${GENERATED_SRC_WIZARD}/${lex_file}.d
        )
    endif()
    set_source_files_properties(${GENERATED_SRC_WIZARD}/${lex_file}.l PROPERTIES GENERATED 1)
    set_source_files_properties(${GENERATED_SRC_WIZARD}/${lex_file}.corr PROPERTIES GENERATED 1)
    set_source_files_properties(${GENERATED_SRC_WIZARD}/${lex_file}.d PROPERTIES GENERATED 1)

    if (depfile_supported)
        add_custom_command(
            COMMAND ${Python_EXECUTABLE} ${PROJECT_SOURCE_DIR}/src/pre_lex.py ${PROJECT_SOURCE_DIR}/addon/doxywizard/${lex_file}.l  ${GENERATED_SRC_WIZARD}/${lex_file}.l ${GENERATED_SRC_WIZARD}/${lex_file}.corr ${GENERATED_SRC_WIZARD}/${lex_file}.d ${PROJECT_SOURCE_DIR}/src
            DEPENDS ${PROJECT_SOURCE_DIR}/src/pre_lex.py ${PROJECT_SOURCE_DIR}/addon/doxywizard/${lex_file}.l
            DEPFILE ${GENERATED_SRC_WIZARD}/${lex_file}.d
            OUTPUT  ${GENERATED_SRC_WIZARD}/${lex_file}.l ${GENERATED_SRC_WIZARD}/${lex_file}.corr ${GENERATED_SRC_WIZARD}/${lex_file}.d
        )
    else()
        add_custom_command(
            COMMAND ${Python_EXECUTABLE} ${PROJECT_SOURCE_DIR}/src/pre_lex.py ${PROJECT_SOURCE_DIR}/addon/doxywizard/${lex_file}.l  ${GENERATED_SRC_WIZARD}/${lex_file}.l ${GENERATED_SRC_WIZARD}/${lex_file}.corr ${GENERATED_SRC_WIZARD}/${lex_file}.d ${PROJECT_SOURCE_DIR}/src
	    DEPENDS ${PROJECT_SOURCE_DIR}/src/pre_lex.py ${PROJECT_SOURCE_DIR}/addon/doxywizard/${lex_file}.l ${LEX_INC_FILES}
            OUTPUT  ${GENERATED_SRC_WIZARD}/${lex_file}.l ${GENERATED_SRC_WIZARD}/${lex_file}.corr ${GENERATED_SRC_WIZARD}/${lex_file}.d
	)
    endif()
    set_source_files_properties(${GENERATED_SRC_WIZARD}/${lex_file}.l PROPERTIES GENERATED 1)
    set_source_files_properties(${GENERATED_SRC_WIZARD}/${lex_file}.corr PROPERTIES GENERATED 1)
    set_source_files_properties(${GENERATED_SRC_WIZARD}/${lex_file}.d PROPERTIES GENERATED 1)

    add_custom_command(
        COMMAND ${Python_EXECUTABLE} ${PROJECT_SOURCE_DIR}/src/scan_states.py ${GENERATED_SRC_WIZARD}/${lex_file}.l > ${GENERATED_SRC_WIZARD}/${lex_file}.l.h
        DEPENDS ${PROJECT_SOURCE_DIR}/src/scan_states.py ${GENERATED_SRC_WIZARD}/${lex_file}.l
        OUTPUT  ${GENERATED_SRC_WIZARD}/${lex_file}.l.h
    )
    set_source_files_properties(${GENERATED_SRC_WIZARD}/${lex_file}.l.h PROPERTIES GENERATED 1)

    FLEX_TARGET(${lex_file}
                ${GENERATED_SRC_WIZARD}/${lex_file}.l
                ${GENERATED_SRC_WIZARD}/${lex_file}_intermediate.cpp
                COMPILE_FLAGS "${LEX_FLAGS}")
    add_custom_command(
        COMMAND ${Python_EXECUTABLE} ${PROJECT_SOURCE_DIR}/src/post_lex.py ${GENERATED_SRC_WIZARD}/${lex_file}_intermediate.cpp ${GENERATED_SRC_WIZARD}/${lex_file}.cpp  ${GENERATED_SRC_WIZARD}/${lex_file}.corr ${PROJECT_SOURCE_DIR}/addon/doxywizard/${lex_file}.l  ${GENERATED_SRC_WIZARD}/${lex_file}.l
        DEPENDS ${PROJECT_SOURCE_DIR}/src/post_lex.py ${GENERATED_SRC_WIZARD}/${lex_file}_intermediate.cpp ${GENERATED_SRC_WIZARD}/${lex_file}.corr
        OUTPUT  ${GENERATED_SRC_WIZARD}/${lex_file}.cpp
    )
endforeach()

qt_wrap_cpp(doxywizard_MOC
doxywizard.h
expert.h
helplabel.h
inputint.h
inputbool.h
inputstring.h
inputstrlist.h
wizard.h
languagedialog.h
)

set(TRANSLATIONS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/i18n)
file(GLOB DOXYWIZARD_TRANSLATION_FILES CONFIGURE_DEPENDS "${TRANSLATIONS_DIR}/doxywizard_*.ts")

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS LinguistTools QUIET)
if(Qt${QT_VERSION_MAJOR}LinguistTools_FOUND)
  set(I18N_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/i18n)
  file(MAKE_DIRECTORY ${I18N_BINARY_DIR})
  set_source_files_properties(${DOXYWIZARD_TRANSLATION_FILES} PROPERTIES OUTPUT_LOCATION ${I18N_BINARY_DIR})
  qt_add_translation(doxywizard_QM_FILES ${DOXYWIZARD_TRANSLATION_FILES})
  get_target_property(QT_QMAKE_EXECUTABLE Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)
  execute_process(
      COMMAND "${QT_QMAKE_EXECUTABLE}" -query QT_INSTALL_TRANSLATIONS
      OUTPUT_VARIABLE QT_TRANSLATIONS_DIR
      OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  message(STATUS "Qt translations dir: ${QT_TRANSLATIONS_DIR}")

  set(TRANSLATIONS_QRC_CONTENT "<!DOCTYPE RCC>\n<RCC version=\"1.0\">\n    <qresource>\n")
  set(doxywizard_QM_FILES_PATHS)
  foreach(ts_file ${DOXYWIZARD_TRANSLATION_FILES})
    get_filename_component(ts_name ${ts_file} NAME_WE)
    string(REGEX REPLACE "doxywizard_(.+)" "\\1" LOCALE "${ts_name}")
    set(qm_file "${I18N_BINARY_DIR}/${ts_name}.qm")
    list(APPEND doxywizard_QM_FILES_PATHS ${qm_file})

    if (QT_TRANSLATIONS_DIR)
      if (EXISTS "${QT_TRANSLATIONS_DIR}/qtbase_${LOCALE}.qm")
        configure_file("${QT_TRANSLATIONS_DIR}/qtbase_${LOCALE}.qm" "${I18N_BINARY_DIR}/qtbase_${LOCALE}.qm" COPYONLY)
        string(APPEND TRANSLATIONS_QRC_CONTENT "        <file>i18n/qtbase_${LOCALE}.qm</file>\n")
      else()
        message(WARNING "Qt translation file ${QT_TRANSLATIONS_DIR}/qtbase_${LOCALE}.qm not found")
      endif()
    endif()
    configure_file("${PROJECT_SOURCE_DIR}/src/i18n/config_${LOCALE}.xml" "${I18N_BINARY_DIR}/config_${LOCALE}.xml" COPYONLY)
    string(APPEND TRANSLATIONS_QRC_CONTENT "        <file>i18n/config_${LOCALE}.xml</file>\n")
    string(APPEND TRANSLATIONS_QRC_CONTENT "        <file>i18n/${ts_name}.qm</file>\n")
  endforeach()
  string(APPEND TRANSLATIONS_QRC_CONTENT "    </qresource>\n</RCC>\n")

  message(STATUS "i18n.qrc:\n${TRANSLATIONS_QRC_CONTENT}")
  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/i18n.qrc "${TRANSLATIONS_QRC_CONTENT}")
  set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/i18n.qrc PROPERTIES OBJECT_DEPENDS "${doxywizard_QM_FILES_PATHS}")
  qt_add_resources(doxywizard_TRANSLATION_RESOURCES ${CMAKE_CURRENT_BINARY_DIR}/i18n.qrc)

else()
  message(STATUS "Qt Linguist tools not found, translation files will not be compiled")
  set(doxywizard_TRANSLATION_RESOURCES)
endif()

qt_add_resources(doxywizard_RESOURCES_RCC doxywizard.qrc)

add_executable(doxywizard WIN32
config_msg.cpp
doxywizard.cpp
expert.cpp
wizard.cpp
inputbool.cpp
inputstring.cpp
inputint.cpp
inputstrlist.cpp
languagedialog.cpp
${GENERATED_SRC_WIZARD}/settings.h
${GENERATED_SRC_WIZARD}/config_doxyw.cpp
${GENERATED_SRC_WIZARD}/config_doxyw.l.h
${GENERATED_SRC_WIZARD}/configdoc.cpp
${doxywizard_MOC}
${doxywizard_TRANSLATION_RESOURCES}
${doxywizard_RESOURCES_RCC}
${PROJECT_SOURCE_DIR}/templates/icon/doxygen.rc
)

set_property(TARGET doxywizard PROPERTY WIN32_EXECUTABLE true)

include(ApplyEditbin)
if (enable_console)
apply_editbin(doxywizard console)
else()
apply_editbin(doxywizard windows)
endif()

if(Qt5Core_FOUND)
  target_link_libraries(doxywizard Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Xml doxygen_version)
elseif(Qt6Core_FOUND)
  target_link_libraries(doxywizard Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Xml doxygen_version)
else()
  target_link_libraries(doxywizard ${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY} doxygen_version)
endif()

install(TARGETS doxywizard DESTINATION bin)
