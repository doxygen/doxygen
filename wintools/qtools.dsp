# Microsoft Developer Studio Project File - Name="qtools" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=qtools - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "qtools.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "qtools.mak" CFG="qtools - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "qtools - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "qtools - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "qtools"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "qtools - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\qtools" /D "QT_NO_CODECS" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "qtools - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\qtools" /D "QT_NO_CODECS" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /I /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "qtools - Win32 Release"
# Name "qtools - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\qtools\qbuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\qtools\qcollection.cpp
# End Source File
# Begin Source File

SOURCE=..\qtools\qcstring.cpp
# End Source File
# Begin Source File

SOURCE=..\qtools\qdatastream.cpp
# End Source File
# Begin Source File

SOURCE=..\qtools\qdatetime.cpp
# End Source File
# Begin Source File

SOURCE=..\qtools\qdir.cpp
# End Source File
# Begin Source File

SOURCE=..\qtools\qdir_win32.cpp
# End Source File
# Begin Source File

SOURCE=..\qtools\qfile.cpp
# End Source File
# Begin Source File

SOURCE=..\qtools\qfile_win32.cpp
# End Source File
# Begin Source File

SOURCE=..\qtools\qfileinfo.cpp
# End Source File
# Begin Source File

SOURCE=..\qtools\qfileinfo_win32.cpp
# End Source File
# Begin Source File

SOURCE=..\qtools\qgarray.cpp
# End Source File
# Begin Source File

SOURCE=..\qtools\qgdict.cpp
# End Source File
# Begin Source File

SOURCE=..\qtools\qglist.cpp
# End Source File
# Begin Source File

SOURCE=..\qtools\qglobal.cpp
# End Source File
# Begin Source File

SOURCE=..\qtools\qgvector.cpp
# End Source File
# Begin Source File

SOURCE=..\qtools\qiodevice.cpp
# End Source File
# Begin Source File

SOURCE=..\qtools\qmap.cpp
# End Source File
# Begin Source File

SOURCE=..\qtools\qregexp.cpp
# End Source File
# Begin Source File

SOURCE=..\qtools\qstring.cpp
# End Source File
# Begin Source File

SOURCE=..\qtools\qstringlist.cpp
# End Source File
# Begin Source File

SOURCE=..\qtools\qtextcodec.cpp
# End Source File
# Begin Source File

SOURCE=..\qtools\qtextstream.cpp
# End Source File
# Begin Source File

SOURCE=..\qtools\qxml.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\qtools\qarray.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qasciidict.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qbuffer.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qcollection.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qconfig.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qcstring.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qdatastream.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qdatetime.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qdict.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qdir.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qfeatures.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qfile.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qfiledefs_p.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qfileinfo.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qgarray.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qgdict.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qgeneric.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qglist.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qglobal.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qgvector.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qintdict.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qiodevice.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qlist.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qmap.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qmodules.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qptrdict.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qqueue.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qregexp.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qshared.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qsortedlist.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qstack.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qstring.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qstringlist.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qstrlist.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qstrvec.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qtextcodec.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qtextstream.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qtl.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qvaluelist.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qvaluestack.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qvector.h
# End Source File
# Begin Source File

SOURCE=..\qtools\qxml.h
# End Source File
# End Group
# End Target
# End Project
