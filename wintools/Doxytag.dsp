# Microsoft Developer Studio Project File - Name="Doxytag" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=Doxytag - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Doxytag.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Doxytag.mak" CFG="Doxytag - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Doxytag - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Doxytag - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "Doxytag"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Doxytag - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\src" /I "..\qtools" /D "_CONSOLE" /D "WIN32" /D "_MBCS" /D "NDEBUG" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 qtools.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /libpath:"Release"

!ELSEIF  "$(CFG)" == "Doxytag - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\qtools" /D "_CONSOLE" /D "WIN32" /D "_MBCS" /D "_DEBUG" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
# SUBTRACT RSC /x
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 qtools.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"Debug"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "Doxytag - Win32 Release"
# Name "Doxytag - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\src\doxytag.cpp
# End Source File
# Begin Source File

SOURCE=..\src\logos.cpp
# End Source File
# Begin Source File

SOURCE=..\src\searchindex.cpp
# End Source File
# Begin Source File

SOURCE=..\src\suffixtree.cpp
# End Source File
# Begin Source File

SOURCE=..\src\version.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\src\logos.h
# End Source File
# Begin Source File

SOURCE=..\src\searchindex.h
# End Source File
# Begin Source File

SOURCE=..\src\suffixtree.h
# End Source File
# Begin Source File

SOURCE=..\src\unistd.h

!IF  "$(CFG)" == "Doxytag - Win32 Release"

USERDEP__UNIST="..\src\doxytag.cpp"	
# Begin Custom Build
InputPath=..\src\unistd.h

"$(InputPath)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo /* this is a dummy file, that is needed for compiling files that are > $(InputPath) 
	echo  * generated with flex under Windows 95/NT. >> $(InputPath) 
	echo  */ >> $(InputPath) 
	echo #if defined(_MSC_VER) >> $(InputPath) 
	echo #include ^<io.h^> >> $(InputPath) 
	echo #endif >> $(InputPath) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Doxytag - Win32 Debug"

# PROP Ignore_Default_Tool 1
USERDEP__UNIST="..\src\doxytag.cpp"	
# Begin Custom Build
InputPath=..\src\unistd.h

"$(InputPath)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo /* this is a dummy file, that is needed for compiling files that are > $(InputPath) 
	echo  * generated with flex under Windows 95/NT. >> $(InputPath) 
	echo  */ >> $(InputPath) 
	echo #if defined(_MSC_VER) >> $(InputPath) 
	echo #include ^<io.h^> >> $(InputPath) 
	echo #endif >> $(InputPath) 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\version.h
# End Source File
# End Group
# End Target
# End Project
