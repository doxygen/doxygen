if (GENERATEDS_FOUND)
  add_custom_command(
	  COMMENT "Updating index.py from index.xsd..."
	  COMMAND ${GENERATEDS_EXECUTABLE} --no-dates --no-versions -f -o ${PROJECT_SOURCE_DIR}/addon/doxmlparser/doxmlparser/index.py ${PROJECT_SOURCE_DIR}/templates/xml/index.xsd
	  COMMAND ${Python_EXECUTABLE} ${PROJECT_SOURCE_DIR}/addon/doxmlparser/generateDS_post.py ${PROJECT_SOURCE_DIR}/addon/doxmlparser/doxmlparser/index.py ${PROJECT_SOURCE_DIR}/addon/doxmlparser/doxmlparser/index_new.py
          COMMAND ${CMAKE_COMMAND} -E remove ${PROJECT_SOURCE_DIR}/addon/doxmlparser/doxmlparser/index.py
          COMMAND ${CMAKE_COMMAND} -E rename ${PROJECT_SOURCE_DIR}/addon/doxmlparser/doxmlparser/index_new.py ${PROJECT_SOURCE_DIR}/addon/doxmlparser/doxmlparser/index.py
	  DEPENDS ${PROJECT_SOURCE_DIR}/templates/xml/index.xsd ${PROJECT_SOURCE_DIR}/addon/doxmlparser/generateDS_post.py
	  OUTPUT  ${PROJECT_SOURCE_DIR}/addon/doxmlparser/doxmlparser/index.py
  )
  add_custom_command(
	  COMMENT "Updating compound.py from compound.xsd..."
	  COMMAND ${GENERATEDS_EXECUTABLE} --no-dates --no-versions -f -o ${PROJECT_SOURCE_DIR}/addon/doxmlparser/doxmlparser/compound.py ${PROJECT_SOURCE_DIR}/templates/xml/compound.xsd
	  COMMAND ${Python_EXECUTABLE} ${PROJECT_SOURCE_DIR}/addon/doxmlparser/generateDS_post.py ${PROJECT_SOURCE_DIR}/addon/doxmlparser/doxmlparser/compound.py ${PROJECT_SOURCE_DIR}/addon/doxmlparser/doxmlparser/compound_new.py
          COMMAND ${CMAKE_COMMAND} -E remove ${PROJECT_SOURCE_DIR}/addon/doxmlparser/doxmlparser/compound.py
          COMMAND ${CMAKE_COMMAND} -E rename ${PROJECT_SOURCE_DIR}/addon/doxmlparser/doxmlparser/compound_new.py ${PROJECT_SOURCE_DIR}/addon/doxmlparser/doxmlparser/compound.py
	  DEPENDS ${PROJECT_SOURCE_DIR}/templates/xml/compound.xsd ${PROJECT_SOURCE_DIR}/addon/doxmlparser/generateDS_post.py
	  OUTPUT  ${PROJECT_SOURCE_DIR}/addon/doxmlparser/doxmlparser/compound.py
  )
  add_custom_target(
	  ${update_doxmlparser_dependency} ALL
	  DEPENDS ${PROJECT_SOURCE_DIR}/addon/doxmlparser/doxmlparser/index.py
	  DEPENDS ${PROJECT_SOURCE_DIR}/addon/doxmlparser/doxmlparser/compound.py
	  COMMENT "Updating doxmlparser module..."
  )
endif()

