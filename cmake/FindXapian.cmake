# Find Xapian search engine library
#
#  XAPIAN_FOUND - system has Xapian
#  XAPIAN_INCLUDE_DIR - the Xapian include directory
#  XAPIAN_LIBRARIES - the libraries needed to use Xapian
#
# Copyright © 2010 Harald Sitter <apachelogger@ubuntu.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if(XAPIAN_INCLUDE_DIR AND XAPIAN_LIBRARIES)
    # Already in cache, be silent
    set(Xapian_FIND_QUIETLY TRUE)
endif(XAPIAN_INCLUDE_DIR AND XAPIAN_LIBRARIES)

FIND_PATH(XAPIAN_INCLUDE_DIR xapian/version.h)

FIND_LIBRARY(XAPIAN_LIBRARIES NAMES xapian)

IF(XAPIAN_INCLUDE_DIR AND XAPIAN_LIBRARIES)
   SET(XAPIAN_FOUND TRUE)
ELSE(XAPIAN_INCLUDE_DIR AND XAPIAN_LIBRARIES)
   SET(XAPIAN_FOUND FALSE)
ENDIF(XAPIAN_INCLUDE_DIR AND XAPIAN_LIBRARIES)

IF(XAPIAN_FOUND)
   IF(NOT Xapian_FIND_QUIETLY)
      MESSAGE(STATUS "Found Xapian: ${XAPIAN_LIBRARIES}")
   ENDIF(NOT Xapian_FIND_QUIETLY)
ELSE(XAPIAN_FOUND)
   IF(Xapian_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find Xapian")
   ENDIF(Xapian_FIND_REQUIRED)
   IF(NOT Xapian_FIND_QUIETLY)
      MESSAGE(STATUS "Could not find Xapian")
   ENDIF(NOT Xapian_FIND_QUIETLY)
ENDIF(XAPIAN_FOUND)

# show the XAPIAN_INCLUDE_DIR and XAPIAN_LIBRARIES variables only in the advanced view
MARK_AS_ADVANCED(XAPIAN_INCLUDE_DIR XAPIAN_LIBRARIES)

