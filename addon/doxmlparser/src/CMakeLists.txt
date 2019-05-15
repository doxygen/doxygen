include_directories(
	.
	../include
	${CMAKE_SOURCE_DIR}/qtools
)
add_library(doxmlparser STATIC
	basehandler.cpp
	compoundhandler.cpp
	debug.cpp
	dochandler.cpp
	graphhandler.cpp
	linkedtexthandler.cpp
	loamhandler.cpp
	mainhandler.cpp
	memberhandler.cpp
	paramhandler.cpp
	sectionhandler.cpp
)
