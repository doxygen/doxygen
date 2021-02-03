find_package(generateDS)
if (GENERATEDS_FOUND)
  add_custom_command(
	  COMMENT "Updating index.py from index.xsd..."
	  COMMAND ${GENERATEDS_EXECUTABLE} --no-dates --no-versions -f -o ${PROJECT_SOURCE_DIR}/addon/doxmlparser/doxmlparser/index.py ${PROJECT_SOURCE_DIR}/templates/xml/index.xsd
	  DEPENDS ${PROJECT_SOURCE_DIR}/templates/xml/index.xsd
	  OUTPUT  ${PROJECT_SOURCE_DIR}/addon/doxmlparser/doxmlparser/index.py
  )
  add_custom_command(
	  COMMENT "Updating compound.py from compound.xsd..."
	  COMMAND ${GENERATEDS_EXECUTABLE} --no-dates --no-versions -f -o ${PROJECT_SOURCE_DIR}/addon/doxmlparser/doxmlparser/compound.py ${PROJECT_SOURCE_DIR}/templates/xml/compound.xsd
	  DEPENDS ${PROJECT_SOURCE_DIR}/templates/xml/compound.xsd
	  OUTPUT  ${PROJECT_SOURCE_DIR}/addon/doxmlparser/doxmlparser/compound.py
  )
  add_custom_target(
	  update_doxmlparser_files ALL
	  DEPENDS ${PROJECT_SOURCE_DIR}/addon/doxmlparser/doxmlparser/index.py
	  DEPENDS ${PROJECT_SOURCE_DIR}/addon/doxmlparser/doxmlparser/compound.py
	  COMMENT "Updating doxmlparser module..."
  )
endif()

