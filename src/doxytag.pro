# 
# $Id$
#
# Copyright (C) 1997-1999 by Dimitri van Heesch.
# 
# Permission to use, copy, modify, and distribute this software and its
# documentation under the terms of the GNU General Public License is hereby 
# granted. No representations are made about the suitability of this software 
# for any purpose. It is provided "as is" without express or implied warranty.
# See the GNU General Public License for more details.
#
# All output generated with Doxygen is not covered by this license.
#
# TMake project file for doxytag

TEMPLATE     =  doxytag.t
CONFIG       =	console warn_on qt release
HEADERS      =  suffixtree.h searchindex.h logos.h version.h
SOURCES      =	doxytag.cpp suffixtree.cpp searchindex.cpp \
		logos.cpp version.cpp 
TARGET       =	../bin/doxytag
win32:INCLUDEPATH += .
#unix:INCLUDEPATH += /usr/include
OBJECTS_DIR  =  ../objects
