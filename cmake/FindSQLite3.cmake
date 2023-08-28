# Copyright (C) 2007-2009 LuaDist.
# Created by Peter Kapec <kapecp@gmail.com>
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
#	Note:
#		Searching headers and libraries is very simple and is NOT as powerful as scripts
#		distributed with CMake, because LuaDist defines directories to search for.
#		Everyone is encouraged to contact the author with improvements. Maybe this file
#		becomes part of CMake distribution sometimes.

# - Find sqlite3
# Find the native SQLite3 headers and libraries.
#
# SQLite3_INCLUDE_DIRS	- where to find sqlite3.h, etc.
# SQLite3_LIBRARIES	- List of libraries when using sqlite.
# SQLite3_FOUND	- True if sqlite found.

# Look for the header file.
FIND_PATH(SQLite3_INCLUDE_DIR NAMES sqlite3.h)

# Look for the library.
FIND_LIBRARY(SQLite3_LIBRARY NAMES sqlite3)

# Handle the QUIETLY and REQUIRED arguments and set SQLITE3_FOUND to TRUE if all listed variables are TRUE.
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SQLite3 DEFAULT_MSG SQLite3_LIBRARY SQLite3_INCLUDE_DIR)

# Copy the results to the output variables.
IF(SQLite3_FOUND)
	SET(SQLite3_LIBRARIES ${SQLite3_LIBRARY})
	SET(SQLite3_INCLUDE_DIRS ${SQLite3_INCLUDE_DIR})
ELSE(SQLite3_FOUND)
	SET(SQLite3_LIBRARIES)
	SET(SQLite3_INCLUDE_DIRS)
ENDIF(SQLite3_FOUND)

MARK_AS_ADVANCED(SQLite3_INCLUDE_DIRS SQLite3_LIBRARIES)

