# this custom rule depends on generateDS, install with 'pip install generateDS'
add_custom_target(update_doxmlparser
	          COMMENT "Updating doxmlparser module from XML schema files"
		  COMMAND generateDS --no-dates --no-versions -f -o ${PROJECT_SOURCE_DIR}/addon/doxmlparser/doxmlparser/index.py ${PROJECT_SOURCE_DIR}/templates/xml/index.xsd
		  COMMAND generateDS --no-dates --no-versions -f -o ${PROJECT_SOURCE_DIR}/addon/doxmlparser/doxmlparser/compound.py ${PROJECT_SOURCE_DIR}/templates/xml/compound.xsd
		  DEPENDS ${PROJECT_SOURCE_DIR}/templates/xml/index.xsd
		  DEPENDS ${PROJECT_SOURCE_DIR}/templates/xml/compound.xsd
	         )
