# Microsoft Developer Studio Project File - Name="Doxygen" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=Doxygen - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Doxygen.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Doxygen.mak" CFG="Doxygen - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Doxygen - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Doxygen - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "Doxygen"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Doxygen - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "..\src" /I "..\qtools" /I "..\libpng" /D "ENGLISH_ONLY" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 qtools.lib libpng.lib zlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /libpath:"Release"

!ELSEIF  "$(CFG)" == "Doxygen - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\qtools" /I "..\libpng" /D "ENGLISH_ONLY" /D "_CONSOLE" /D "WIN32" /D "_DEBUG" /D "_MBCS" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
# SUBTRACT RSC /x
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 qtools.lib libpng.lib zlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"Debug"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "Doxygen - Win32 Release"
# Name "Doxygen - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\src\ce_lex.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ce_parse.cpp
# End Source File
# Begin Source File

SOURCE=..\src\classdef.cpp
# End Source File
# Begin Source File

SOURCE=..\src\classlist.cpp
# End Source File
# Begin Source File

SOURCE=..\src\cmdmapper.cpp
# End Source File
# Begin Source File

SOURCE=..\src\code.cpp
# End Source File
# Begin Source File

SOURCE=..\src\commentcnv.cpp
# End Source File
# Begin Source File

SOURCE=..\src\config.cpp
# End Source File
# Begin Source File

SOURCE=..\src\cppvalue.cpp
# End Source File
# Begin Source File

SOURCE=..\src\debug.cpp
# End Source File
# Begin Source File

SOURCE=..\src\declinfo.cpp
# End Source File
# Begin Source File

SOURCE=..\src\defargs.cpp
# End Source File
# Begin Source File

SOURCE=..\src\defgen.cpp
# End Source File
# Begin Source File

SOURCE=..\src\define.cpp
# End Source File
# Begin Source File

SOURCE=..\src\definition.cpp
# End Source File
# Begin Source File

SOURCE=..\src\diagram.cpp
# End Source File
# Begin Source File

SOURCE=..\src\docparser.cpp
# End Source File
# Begin Source File

SOURCE=..\src\doctokenizer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dot.cpp
# End Source File
# Begin Source File

SOURCE=..\src\doxygen.cpp
# End Source File
# Begin Source File

SOURCE=..\src\entry.cpp
# End Source File
# Begin Source File

SOURCE=..\src\filedef.cpp
# End Source File
# Begin Source File

SOURCE=..\src\filename.cpp
# End Source File
# Begin Source File

SOURCE=..\src\formula.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ftvhelp.cpp
# End Source File
# Begin Source File

SOURCE=..\src\groupdef.cpp
# End Source File
# Begin Source File

SOURCE=..\src\htmldocvisitor.cpp
# End Source File
# Begin Source File

SOURCE=..\src\htmlgen.cpp
# End Source File
# Begin Source File

SOURCE=..\src\htmlhelp.cpp
# End Source File
# Begin Source File

SOURCE=..\src\image.cpp
# End Source File
# Begin Source File

SOURCE=..\src\index.cpp
# End Source File
# Begin Source File

SOURCE=..\src\instdox.cpp
# End Source File
# Begin Source File

SOURCE=..\src\language.cpp
# End Source File
# Begin Source File

SOURCE=..\src\latexdocvisitor.cpp
# End Source File
# Begin Source File

SOURCE=..\src\latexgen.cpp
# End Source File
# Begin Source File

SOURCE=..\src\logos.cpp
# End Source File
# Begin Source File

SOURCE=..\src\main.cpp
# End Source File
# Begin Source File

SOURCE=..\src\mandocvisitor.cpp
# End Source File
# Begin Source File

SOURCE=..\src\mangen.cpp
# End Source File
# Begin Source File

SOURCE=..\src\memberdef.cpp
# End Source File
# Begin Source File

SOURCE=..\src\membergroup.cpp
# End Source File
# Begin Source File

SOURCE=..\src\memberlist.cpp
# End Source File
# Begin Source File

SOURCE=..\src\membername.cpp
# End Source File
# Begin Source File

SOURCE=..\src\message.cpp
# End Source File
# Begin Source File

SOURCE=..\src\namespacedef.cpp
# End Source File
# Begin Source File

SOURCE=..\src\outputgen.cpp
# End Source File
# Begin Source File

SOURCE=..\src\outputlist.cpp
# End Source File
# Begin Source File

SOURCE=..\src\packagedef.cpp
# End Source File
# Begin Source File

SOURCE=..\src\pagedef.cpp
# End Source File
# Begin Source File

SOURCE=..\src\perlmodgen.cpp
# End Source File
# Begin Source File

SOURCE=..\src\pngenc.cpp
# End Source File
# Begin Source File

SOURCE=..\src\pre.cpp
# End Source File
# Begin Source File

SOURCE=..\src\reflist.cpp
# End Source File
# Begin Source File

SOURCE=..\src\rtfdocvisitor.cpp
# End Source File
# Begin Source File

SOURCE=..\src\rtfgen.cpp
# End Source File
# Begin Source File

SOURCE=..\src\rtfstyle.cpp
# End Source File
# Begin Source File

SOURCE=..\src\scanner.cpp
# End Source File
# Begin Source File

SOURCE=..\src\tagreader.cpp
# End Source File
# Begin Source File

SOURCE=..\src\translator.cpp
# End Source File
# Begin Source File

SOURCE=..\src\util.cpp
# End Source File
# Begin Source File

SOURCE=..\src\version.cpp
# End Source File
# Begin Source File

SOURCE=..\src\xmldocvisitor.cpp
# End Source File
# Begin Source File

SOURCE=..\src\xmlgen.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\src\bufstr.h
# End Source File
# Begin Source File

SOURCE=..\src\ce_parse.h
# End Source File
# Begin Source File

SOURCE=..\src\classdef.h
# End Source File
# Begin Source File

SOURCE=..\src\classlist.h
# End Source File
# Begin Source File

SOURCE=..\src\cmdmapper.h
# End Source File
# Begin Source File

SOURCE=..\src\code.h
# End Source File
# Begin Source File

SOURCE=..\src\commentcnv.h
# End Source File
# Begin Source File

SOURCE=..\src\config.h
# End Source File
# Begin Source File

SOURCE=..\src\constexp.h
# End Source File
# Begin Source File

SOURCE=..\src\cppvalue.h
# End Source File
# Begin Source File

SOURCE=..\src\debug.h
# End Source File
# Begin Source File

SOURCE=..\src\declinfo.h
# End Source File
# Begin Source File

SOURCE=..\src\defargs.h
# End Source File
# Begin Source File

SOURCE=..\src\defgen.h
# End Source File
# Begin Source File

SOURCE=..\src\define.h
# End Source File
# Begin Source File

SOURCE=..\src\definition.h
# End Source File
# Begin Source File

SOURCE=..\src\diagram.h
# End Source File
# Begin Source File

SOURCE=..\src\docparser.h
# End Source File
# Begin Source File

SOURCE=..\src\doctokenizer.h
# End Source File
# Begin Source File

SOURCE=..\src\docvisitor.h
# End Source File
# Begin Source File

SOURCE=..\src\dot.h
# End Source File
# Begin Source File

SOURCE=..\src\doxygen.h
# End Source File
# Begin Source File

SOURCE=..\src\entry.h
# End Source File
# Begin Source File

SOURCE=..\src\example.h
# End Source File
# Begin Source File

SOURCE=..\src\filedef.h
# End Source File
# Begin Source File

SOURCE=..\src\filename.h
# End Source File
# Begin Source File

SOURCE=..\src\formula.h
# End Source File
# Begin Source File

SOURCE=..\src\ftvhelp.h
# End Source File
# Begin Source File

SOURCE=..\src\groupdef.h
# End Source File
# Begin Source File

SOURCE=..\src\htmlattrib.h
# End Source File
# Begin Source File

SOURCE=..\src\htmldocvisitor.h
# End Source File
# Begin Source File

SOURCE=..\src\htmlgen.h
# End Source File
# Begin Source File

SOURCE=..\src\htmlhelp.h
# End Source File
# Begin Source File

SOURCE=..\src\image.h
# End Source File
# Begin Source File

SOURCE=..\src\index.h
# End Source File
# Begin Source File

SOURCE=..\src\instdox.h
# End Source File
# Begin Source File

SOURCE=..\src\lang_cfg.h
# End Source File
# Begin Source File

SOURCE=..\src\language.h
# End Source File
# Begin Source File

SOURCE=..\src\latexdocvisitor.h
# End Source File
# Begin Source File

SOURCE=..\src\latexgen.h
# End Source File
# Begin Source File

SOURCE=..\src\logos.h
# End Source File
# Begin Source File

SOURCE=..\src\mandocvisitor.h
# End Source File
# Begin Source File

SOURCE=..\src\mangen.h
# End Source File
# Begin Source File

SOURCE=..\src\memberdef.h
# End Source File
# Begin Source File

SOURCE=..\src\membergroup.h
# End Source File
# Begin Source File

SOURCE=..\src\memberlist.h
# End Source File
# Begin Source File

SOURCE=..\src\membername.h
# End Source File
# Begin Source File

SOURCE=..\src\message.h
# End Source File
# Begin Source File

SOURCE=..\src\namespacedef.h
# End Source File
# Begin Source File

SOURCE=..\src\outputgen.h
# End Source File
# Begin Source File

SOURCE=..\src\outputlist.h
# End Source File
# Begin Source File

SOURCE=..\src\packagedef.h
# End Source File
# Begin Source File

SOURCE=..\src\pagedef.h
# End Source File
# Begin Source File

SOURCE=..\src\perlmodgen.h
# End Source File
# Begin Source File

SOURCE=..\src\pngenc.h
# End Source File
# Begin Source File

SOURCE=..\src\pre.h
# End Source File
# Begin Source File

SOURCE=..\src\printdocvisitor.h
# End Source File
# Begin Source File

SOURCE=..\src\qtbc.h
# End Source File
# Begin Source File

SOURCE=..\src\reflist.h
# End Source File
# Begin Source File

SOURCE=..\src\rtfdocvisitor.h
# End Source File
# Begin Source File

SOURCE=..\src\rtfgen.h
# End Source File
# Begin Source File

SOURCE=..\src\rtfstyle.h
# End Source File
# Begin Source File

SOURCE=..\src\scanner.h
# End Source File
# Begin Source File

SOURCE=..\src\section.h
# End Source File
# Begin Source File

SOURCE=..\src\sortdict.h
# End Source File
# Begin Source File

SOURCE=..\src\tagreader.h
# End Source File
# Begin Source File

SOURCE=..\src\translator.h
# End Source File
# Begin Source File

SOURCE=..\src\translator_en.h
# End Source File
# Begin Source File

SOURCE=..\src\unistd.h

!IF  "$(CFG)" == "Doxygen - Win32 Release"

USERDEP__UNIST="..\src\ce_lex.cpp"	"..\src\code.cpp"	"..\src\commentcnv.cpp"	"..\src\config.cpp"	"..\src\declinfo.cpp"	"..\src\defargs.cpp"	"..\src\doctokenizer.cpp"	"..\src\formula.cpp"	"..\src\pre.cpp"	"..\src\scanner.cpp"	"..\src\util.cpp"	
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

!ELSEIF  "$(CFG)" == "Doxygen - Win32 Debug"

# PROP Ignore_Default_Tool 1
USERDEP__UNIST="..\src\ce_lex.cpp"	"..\src\code.cpp"	"..\src\commentcnv.cpp"	"..\src\config.cpp"	"..\src\declinfo.cpp"	"..\src\defargs.cpp"	"..\src\doctokenizer.cpp"	"..\src\formula.cpp"	"..\src\pre.cpp"	"..\src\scanner.cpp"	"..\src\util.cpp"	
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

SOURCE=..\src\util.h
# End Source File
# Begin Source File

SOURCE=..\src\version.h
# End Source File
# Begin Source File

SOURCE=..\src\xmldocvisitor.h
# End Source File
# Begin Source File

SOURCE=..\src\xmlgen.h
# End Source File
# End Group
# End Target
# End Project
