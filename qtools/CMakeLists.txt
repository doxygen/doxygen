include_directories(.)

list(APPEND qtools_src
qbuffer.cpp
qcollection.cpp
qcstring.cpp
qdatastream.cpp
qdir.cpp
qfile.cpp
qfileinfo.cpp
qgarray.cpp
qgdict.cpp
qglist.cpp
qglobal.cpp
qgstring.cpp
qiodevice.cpp
qregexp.cpp
qstring.cpp
qtextstream.cpp
qtextcodec.cpp
qstringlist.cpp
qcstringlist.cpp
qutfcodec.cpp
)

if (UNIX)
list(APPEND qtools_src
qfile_unix.cpp
qdir_unix.cpp
qfileinfo_unix.cpp
)
endif()

if (WIN32)
list(APPEND qtools_src
qfile_win32.cpp
qdir_win32.cpp
qfileinfo_win32.cpp

)
endif()

add_library(qtools STATIC
${qtools_src}
)
