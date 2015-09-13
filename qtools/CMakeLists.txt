include_directories(.)

list(APPEND qtools_src
qbuffer.cpp
qcollection.cpp
qcstring.cpp
qdatastream.cpp
qdatetime.cpp
qdir.cpp
qfile.cpp
qfileinfo.cpp
qgarray.cpp
qgcache.cpp
qgdict.cpp
qglist.cpp
qglobal.cpp
qgstring.cpp
qgvector.cpp
qiodevice.cpp
qregexp.cpp
qstring.cpp
qtextstream.cpp
qtextcodec.cpp
qstringlist.cpp
qxml.cpp
qmap.cpp
qthread.cpp
qmutex.cpp
qutfcodec.cpp
)

if (UNIX)
list(APPEND qtools_src
qfile_unix.cpp
qdir_unix.cpp
qfileinfo_unix.cpp
qthread_unix.cpp
qmutex_unix.cpp
qwaitcondition_unix.cpp
)
endif()

if (WIN32)
list(APPEND qtools_src
qfile_win32.cpp
qdir_win32.cpp
qfileinfo_win32.cpp
qthread_win32.cpp
qmutex_win32.cpp
qwaitcondition_win32.cpp

)
endif()

add_library(qtools STATIC
${qtools_src}
)
