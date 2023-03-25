# doxygen_version.cmake
#

# This file defines the functions and targets needed to monitor
# doxygen VERSION file.
#
# The behavior of this script can be modified by defining any of these variables:
#
#   PRE_CONFIGURE_DOXYGEN_VERSION_FILE (REQUIRED)
#   -- The path to the file that'll be configured.
#
#   POST_CONFIGURE_DOXYGEN_VERSION_FILE (REQUIRED)
#   -- The path to the configured PRE_CONFIGURE_DOXYGEN_VERSION_FILE.
#
#   DOXY_STATE_FILE (OPTIONAL)
#   -- The path to the file used to store the doxygen version information.
#
# This file is based on git_watcher.cmake

# Short hand for converting paths to absolute.
macro(PATH_TO_ABSOLUTE var_name)
    get_filename_component(${var_name} "${${var_name}}" ABSOLUTE)
endmacro()

# Check that a required variable is set.
macro(CHECK_REQUIRED_VARIABLE var_name)
    if(NOT DEFINED ${var_name})
        message(FATAL_ERROR "The \"${var_name}\" variable must be defined.")
    endif()
    PATH_TO_ABSOLUTE(${var_name})
endmacro()

# Check that an optional variable is set, or, set it to a default value.
macro(CHECK_OPTIONAL_VARIABLE var_name default_value)
    if(NOT DEFINED ${var_name})
        set(${var_name} ${default_value})
    endif()
    PATH_TO_ABSOLUTE(${var_name})
endmacro()

CHECK_REQUIRED_VARIABLE(PRE_CONFIGURE_DOXYGEN_VERSION_FILE)
CHECK_REQUIRED_VARIABLE(POST_CONFIGURE_DOXYGEN_VERSION_FILE)
CHECK_OPTIONAL_VARIABLE(DOXY_STATE_FILE "${PROJECT_SOURCE_DIR}/VERSION")

# Function: DoxygenStateChangedAction
# Description: this function is executed when the 
#              doxygen version file has changed.
function(DoxygenStateChangedAction _state_as_list)
    # Set variables by index, then configure the file w/ these variables defined.
    LIST(GET _state_as_list 0 DOXYGEN_VERSION)
    configure_file("${PRE_CONFIGURE_DOXYGEN_VERSION_FILE}" "${POST_CONFIGURE_DOXYGEN_VERSION_FILE}" @ONLY)
endfunction()



# Function: SetupDoxyMonitoring
# Description: this function sets up custom commands that make the build system
#              check the doxygen version file before every build. If it has
#              changed, then a file is configured.
function(SetupDoxyMonitoring)
    add_custom_target(check_doxygen_version
        ALL
	DEPENDS ${PRE_CONFIGURE_DOXYGEN_VERSION_FILE}
	BYPRODUCTS ${POST_CONFIGURE_DOXYGEN_VERSION_FILE}
        COMMENT "Checking the doxygen version for changes..."
        COMMAND
            ${CMAKE_COMMAND}
	    -D_BUILD_TIME_CHECK_DOXY=TRUE
	    -DDOXY_STATE_FILE=${DOXY_STATE_FILE}
	    -DPRE_CONFIGURE_DOXYGEN_VERSION_FILE=${PRE_CONFIGURE_DOXYGEN_VERSION_FILE}
	    -DPOST_CONFIGURE_DOXYGEN_VERSION_FILE=${POST_CONFIGURE_DOXYGEN_VERSION_FILE}
            -P "${CMAKE_CURRENT_LIST_FILE}")
endfunction()



# Function: Main
# Description: primary entry-point to the script. Functions are selected based
#              on whether it's configure or build time.
function(Main)
    file(STRINGS "${DOXY_STATE_FILE}" DOXYGEN_VERSION)
    if(_BUILD_TIME_CHECK_DOXY)
        # Check if the doxygen version file has changed.
        # If so, run the change action.
	if(${DOXY_STATE_FILE} IS_NEWER_THAN ${POST_CONFIGURE_DOXYGEN_VERSION_FILE})
	    DoxygenStateChangedAction("${DOXYGEN_VERSION}")
        endif()
    else()
        # >> Executes at configure time.
	SetupDoxyMonitoring()
	DoxygenStateChangedAction("${DOXYGEN_VERSION}")
    endif()
endfunction()

# And off we go...
Main()
