# git_watcher.cmake
#
# License: MIT
# Source: https://raw.githubusercontent.com/andrew-hardin/cmake-git-version-tracking/master/git_watcher.cmake


# This file defines the functions and targets needed to monitor
# the state of a git repo. If the state changes (e.g. a commit is made),
# then a file gets reconfigured.
#
# The behavior of this script can be modified by defining any of these variables:
#
#   PRE_CONFIGURE_GIT_VERSION_FILE (REQUIRED)
#   -- The path to the file that'll be configured.
#
#   POST_CONFIGURE_GIT_VERSION_FILE (REQUIRED)
#   -- The path to the configured PRE_CONFIGURE_GIT_VERSION_FILE.
#
#   GIT_STATE_FILE (OPTIONAL)
#   -- The path to the file used to store the previous build's git state.
#      Defaults to the current binary directory.
#
#   GIT_WORKING_DIR (OPTIONAL)
#   -- The directory from which git commands will be run.
#      Defaults to the directory with the top level CMakeLists.txt.
#
#   GIT_EXECUTABLE (OPTIONAL)
#   -- The path to the git executable. It'll automatically be set if the
#      user doesn't supply a path.
#
# Script design:
#   - This script was designed similar to a Python application
#     with a Main() function. I wanted to keep it compact to
#     simplify "copy + paste" usage.
#
#   - This script is made to operate in two CMake contexts:
#       1. Configure time context (when build files are created).
#       2. Build time context (called via CMake -P)
#     If you see something odd (e.g. the NOT DEFINED clauses),
#     consider that it can run in one of two contexts.

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

CHECK_REQUIRED_VARIABLE(PRE_CONFIGURE_GIT_VERSION_FILE)
CHECK_REQUIRED_VARIABLE(POST_CONFIGURE_GIT_VERSION_FILE)
CHECK_REQUIRED_VARIABLE(GIT_CONFIG_DIR)
CHECK_OPTIONAL_VARIABLE(GIT_STATE_FILE "${GENERATED_SRC}/git_state")
#CHECK_REQUIRED_VARIABLE(GIT_STATE_FILE)
CHECK_OPTIONAL_VARIABLE(GIT_WORKING_DIR "${PROJECT_SOURCE_DIR}")

# Check the optional git variable.
# If it's not set, we'll try to find it using the CMake packaging system.
if(NOT DEFINED GIT_EXECUTABLE)
    find_package(Git QUIET)
endif()
CHECK_REQUIRED_VARIABLE(GIT_EXECUTABLE)



# Function: GitStateChangedAction
# Description: this function is executed when the state of the git
#              repo changes (e.g. a commit is made).
function(GitStateChangedAction _state_as_list)
    # Set variables by index, then configure the file w/ these variables defined.
    LIST(GET _state_as_list 0 GIT_RETRIEVED_STATE)
    LIST(GET _state_as_list 1 GIT_HEAD_SHA1)
    LIST(GET _state_as_list 2 GIT_IS_DIRTY)
    configure_file("${PRE_CONFIGURE_GIT_VERSION_FILE}" "${POST_CONFIGURE_GIT_VERSION_FILE}" @ONLY)
endfunction()



# Function: GetGitState
# Description: gets the current state of the git repo.
# Args:
#   _working_dir (in)  string; the directory from which git commands will be executed.
#   _state       (out) list; a collection of variables representing the state of the
#                            repository (e.g. commit SHA).
function(GetGitState _working_dir _state)

    # Get the hash for HEAD.
    set(_success "true")
    if(EXISTS "${GIT_CONFIG_DIR}")
        execute_process(COMMAND
            "${GIT_EXECUTABLE}" rev-parse --verify HEAD
            WORKING_DIRECTORY "${_working_dir}"
            RESULT_VARIABLE res
            OUTPUT_VARIABLE _hashvar
            ERROR_QUIET
            OUTPUT_STRIP_TRAILING_WHITESPACE)
        if(NOT res EQUAL 0)
            set(_success "false")
            set(_hashvar "GIT-NOTFOUND")
        endif()
    
        # Get whether or not the working tree is dirty.
        execute_process(COMMAND
            "${GIT_EXECUTABLE}" status --porcelain
            WORKING_DIRECTORY "${_working_dir}"
            RESULT_VARIABLE res
            OUTPUT_VARIABLE out
            ERROR_QUIET
            OUTPUT_STRIP_TRAILING_WHITESPACE)
        if(NOT res EQUAL 0)
            set(_success "false")
            set(_dirty "false")
        else()
            if(NOT "${out}" STREQUAL "")
                set(_dirty "true")
            else()
                set(_dirty "false")
            endif()
        endif()
    else()
        set(_success "false")
        set(_hashvar "GIT-NOTFOUND")
        set(_dirty "false")
    endif()

    # Return a list of our variables to the parent scope.
    set(${_state} ${_success} ${_hashvar} ${_dirty} PARENT_SCOPE)
endfunction()



# Function: CheckGit
# Description: check if the git repo has changed. If so, update the state file.
# Args:
#   _working_dir    (in)  string; the directory from which git commands will be ran.
#   _state_changed (out)    bool; whether or no the state of the repo has changed.
#   _state         (out)    list; the repository state as a list (e.g. commit SHA).
function(CheckGit _working_dir _state_changed _state)

    # Get the current state of the repo.
    GetGitState("${_working_dir}" state)

    # Set the output _state variable.
    # (Passing by reference in CMake is awkward...)
    set(${_state} ${state} PARENT_SCOPE)

    if(EXISTS "${POST_CONFIGURE_GIT_VERSION_FILE}")
        if("${PRE_CONFIGURE_GIT_VERSION_FILE}" IS_NEWER_THAN "${POST_CONFIGURE_GIT_VERSION_FILE}")
            file(REMOVE "${POST_CONFIGURE_GIT_VERSION_FILE}")
            file(REMOVE "${GIT_STATE_FILE}")
            set(${_state_changed} "true" PARENT_SCOPE)
            return()
        endif()
    else()
       file(REMOVE "${GIT_STATE_FILE}")
       set(${_state_changed} "true" PARENT_SCOPE)
       return()
    endif()

    # Check if the state has changed compared to the backup on disk.
    if(EXISTS "${GIT_STATE_FILE}")
        file(READ "${GIT_STATE_FILE}" OLD_HEAD_CONTENTS)
        if(OLD_HEAD_CONTENTS STREQUAL "${state}")
            # State didn't change.
            set(${_state_changed} "false" PARENT_SCOPE)
            return()
        endif()
    endif()

    # The state has changed.
    # We need to update the state file on disk.
    # Future builds will compare their state to this file.
    file(WRITE "${GIT_STATE_FILE}" "${state}")
    set(${_state_changed} "true" PARENT_SCOPE)
endfunction()



# Function: SetupGitMonitoring
# Description: this function sets up custom commands that make the build system
#              check the state of git before every build. If the state has
#              changed, then a file is configured.
function(SetupGitMonitoring)
    add_custom_target(check_git_repository
        ALL
	DEPENDS ${PRE_CONFIGURE_GIT_VERSION_FILE}
	BYPRODUCTS ${POST_CONFIGURE_GIT_VERSION_FILE}
	BYPRODUCTS ${GIT_STATE_FILE}
        COMMENT "Checking the git repository for changes..."
        COMMAND
            ${CMAKE_COMMAND}
            -D_BUILD_TIME_CHECK_GIT=TRUE
            -DGIT_WORKING_DIR=${GIT_WORKING_DIR}
            -DGIT_EXECUTABLE=${GIT_EXECUTABLE}
            -DGIT_STATE_FILE=${GIT_STATE_FILE}
	    -DGIT_CONFIG_DIR=${GIT_CONFIG_DIR}
	    -DPRE_CONFIGURE_GIT_VERSION_FILE=${PRE_CONFIGURE_GIT_VERSION_FILE}
	    -DPOST_CONFIGURE_GIT_VERSION_FILE=${POST_CONFIGURE_GIT_VERSION_FILE}
            -P "${CMAKE_CURRENT_LIST_FILE}")
endfunction()



# Function: Main
# Description: primary entry-point to the script. Functions are selected based
#              on whether it's configure or build time.
function(Main)
    if(_BUILD_TIME_CHECK_GIT)
        # Check if the repo has changed.
        # If so, run the change action.
        CheckGit("${GIT_WORKING_DIR}" did_change state)
        if(did_change)
            GitStateChangedAction("${state}")
        endif()
    else()
        # >> Executes at configure time.
        SetupGitMonitoring()
    endif()
endfunction()

# And off we go...
Main()
