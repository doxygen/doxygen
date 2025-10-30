if (GENERATEDS_FOUND)
  add_custom_command(
	  COMMENT "Updating index.py from index.xsd..."
	  COMMAND ${GENERATEDS_EXECUTABLE} --no-dates --no-versions -f -o ${TOP}/addon/doxmlparser/doxmlparser/index.py ${TOP}/templates/xml/index.xsd
	  COMMAND ${Python_EXECUTABLE} ${TOP}/addon/doxmlparser/generateDS_post.py ${TOP}/addon/doxmlparser/doxmlparser/index.py ${TOP}/addon/doxmlparser/doxmlparser/index_new.py
	  COMMAND ${CMAKE_COMMAND} -E remove ${TOP}/addon/doxmlparser/doxmlparser/index.py
	  COMMAND ${CMAKE_COMMAND} -E rename ${TOP}/addon/doxmlparser/doxmlparser/index_new.py ${TOP}/addon/doxmlparser/doxmlparser/index.py
	  DEPENDS ${TOP}/templates/xml/index.xsd ${TOP}/addon/doxmlparser/generateDS_post.py
	  OUTPUT  ${TOP}/addon/doxmlparser/doxmlparser/index.py
  )
  add_custom_command(
	  COMMENT "Updating compound.py from compound.xsd..."
	  COMMAND ${GENERATEDS_EXECUTABLE} --no-dates --no-versions -f -o ${TOP}/addon/doxmlparser/doxmlparser/compound.py ${TOP}/templates/xml/compound.xsd
	  COMMAND ${Python_EXECUTABLE} ${TOP}/addon/doxmlparser/generateDS_post.py ${TOP}/addon/doxmlparser/doxmlparser/compound.py ${TOP}/addon/doxmlparser/doxmlparser/compound_new.py
	  COMMAND ${CMAKE_COMMAND} -E remove ${TOP}/addon/doxmlparser/doxmlparser/compound.py
	  COMMAND ${CMAKE_COMMAND} -E rename ${TOP}/addon/doxmlparser/doxmlparser/compound_new.py ${TOP}/addon/doxmlparser/doxmlparser/compound.py
	  DEPENDS ${TOP}/templates/xml/compound.xsd ${TOP}/addon/doxmlparser/generateDS_post.py
	  OUTPUT  ${TOP}/addon/doxmlparser/doxmlparser/compound.py
  )
  add_custom_target(
	  ${update_doxmlparser_dependency} ALL
	  DEPENDS ${TOP}/addon/doxmlparser/doxmlparser/index.py
	  DEPENDS ${TOP}/addon/doxmlparser/doxmlparser/compound.py
	  COMMENT "Updating doxmlparser module..."
  )
endif()

