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
# PROP Scc_LocalPath ".."
CPP=xicl6.exe
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
# ADD CPP /nologo /W3 /GX /O2 /I "..\qtools" /I "..\src" /I "..\libpng" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "NDEBUG" /D "QT_NO_CODECS" /FR /YX /FD /Zm200 /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG" /d "QT_NO_CODECS"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\qtools" /I "..\src" /I "..\libpng" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "QT_NO_CODECS" /FR /YX /FD /Zm200 /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG" /d "QT_NO_CODECS"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Doxygen - Win32 Release"
# Name "Doxygen - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\libpng\adler32.c
# End Source File
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

SOURCE=..\src\code.cpp
# End Source File
# Begin Source File

SOURCE=..\src\commentcnv.cpp
# End Source File
# Begin Source File

SOURCE=..\libpng\compress.c
# End Source File
# Begin Source File

SOURCE=..\src\config.cpp
# End Source File
# Begin Source File

SOURCE=..\src\cppvalue.cpp
# End Source File
# Begin Source File

SOURCE=..\libpng\crc32.c
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

SOURCE=..\libpng\deflate.c
# End Source File
# Begin Source File

SOURCE=..\src\diagram.cpp
# End Source File
# Begin Source File

SOURCE=..\src\doc.cpp
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

SOURCE=..\libpng\gzio.c
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

SOURCE=..\libpng\infblock.c
# End Source File
# Begin Source File

SOURCE=..\libpng\infcodes.c
# End Source File
# Begin Source File

SOURCE=..\libpng\inffast.c
# End Source File
# Begin Source File

SOURCE=..\libpng\inflate.c
# End Source File
# Begin Source File

SOURCE=..\libpng\inftrees.c
# End Source File
# Begin Source File

SOURCE=..\libpng\infutil.c
# End Source File
# Begin Source File

SOURCE=..\src\instdox.cpp
# End Source File
# Begin Source File

SOURCE=..\src\language.cpp
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

SOURCE=..\libpng\png.c
# End Source File
# Begin Source File

SOURCE=..\src\pngenc.cpp
# End Source File
# Begin Source File

SOURCE=..\libpng\pngerror.c
# End Source File
# Begin Source File

SOURCE=..\libpng\pnggccrd.c
# End Source File
# Begin Source File

SOURCE=..\libpng\pngget.c
# End Source File
# Begin Source File

SOURCE=..\libpng\pngmem.c
# End Source File
# Begin Source File

SOURCE=..\libpng\pngpread.c
# End Source File
# Begin Source File

SOURCE=..\libpng\pngread.c
# End Source File
# Begin Source File

SOURCE=..\libpng\pngrio.c
# End Source File
# Begin Source File

SOURCE=..\libpng\pngrtran.c
# End Source File
# Begin Source File

SOURCE=..\libpng\pngrutil.c
# End Source File
# Begin Source File

SOURCE=..\libpng\pngset.c
# End Source File
# Begin Source File

SOURCE=..\libpng\pngtrans.c
# End Source File
# Begin Source File

SOURCE=..\libpng\pngvcrd.c
# End Source File
# Begin Source File

SOURCE=..\libpng\pngwio.c
# End Source File
# Begin Source File

SOURCE=..\libpng\pngwrite.c
# End Source File
# Begin Source File

SOURCE=..\libpng\pngwtran.c
# End Source File
# Begin Source File

SOURCE=..\libpng\pngwutil.c
# End Source File
# Begin Source File

SOURCE=..\src\pre.cpp
# End Source File
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
# Begin Source File

SOURCE=..\src\reflist.cpp
# End Source File
# Begin Source File

SOURCE=..\src\rtfgen.cpp
# End Source File
# Begin Source File

SOURCE=..\src\scanner.cpp
# End Source File
# Begin Source File

SOURCE=..\src\searchindex.cpp
# End Source File
# Begin Source File

SOURCE=..\src\suffixtree.cpp
# End Source File
# Begin Source File

SOURCE=..\src\tagreader.cpp
# End Source File
# Begin Source File

SOURCE=..\src\translator.cpp
# End Source File
# Begin Source File

SOURCE=..\libpng\trees.c
# End Source File
# Begin Source File

SOURCE=..\libpng\uncompr.c
# End Source File
# Begin Source File

SOURCE=..\src\util.cpp
# End Source File
# Begin Source File

SOURCE=..\src\version.cpp
# End Source File
# Begin Source File

SOURCE=..\src\xmlgen.cpp
# End Source File
# Begin Source File

SOURCE=..\libpng\zutil.c
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

SOURCE=..\libpng\deflate.h
# End Source File
# Begin Source File

SOURCE=..\src\diagram.h
# End Source File
# Begin Source File

SOURCE=..\src\doc.h
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

SOURCE=..\libpng\infblock.h
# End Source File
# Begin Source File

SOURCE=..\libpng\infcodes.h
# End Source File
# Begin Source File

SOURCE=..\libpng\inffast.h
# End Source File
# Begin Source File

SOURCE=..\libpng\inffixed.h
# End Source File
# Begin Source File

SOURCE=..\libpng\inftrees.h
# End Source File
# Begin Source File

SOURCE=..\libpng\infutil.h
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

SOURCE=..\src\latexgen.h
# End Source File
# Begin Source File

SOURCE=..\src\logos.h
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

SOURCE=..\src\page.h
# End Source File
# Begin Source File

SOURCE=..\libpng\png.h
# End Source File
# Begin Source File

SOURCE=..\libpng\pngasmrd.h
# End Source File
# Begin Source File

SOURCE=..\libpng\pngconf.h
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

SOURCE=..\src\qtbc.h
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
# Begin Source File

SOURCE=..\src\reflist.h
# End Source File
# Begin Source File

SOURCE=..\src\rtfgen.h
# End Source File
# Begin Source File

SOURCE=..\src\scanner.h
# End Source File
# Begin Source File

SOURCE=..\src\searchindex.h
# End Source File
# Begin Source File

SOURCE=..\src\section.h
# End Source File
# Begin Source File

SOURCE=..\src\sortdict.h
# End Source File
# Begin Source File

SOURCE=..\src\suffixtree.h
# End Source File
# Begin Source File

SOURCE=..\src\tagreader.h
# End Source File
# Begin Source File

SOURCE=..\src\translator.h
# End Source File
# Begin Source File

SOURCE=..\src\translator_adapter.h
# End Source File
# Begin Source File

SOURCE=..\src\translator_br.h
# End Source File
# Begin Source File

SOURCE=..\src\translator_cn.h
# End Source File
# Begin Source File

SOURCE=..\src\translator_cz.h
# End Source File
# Begin Source File

SOURCE=..\src\translator_de.h
# End Source File
# Begin Source File

SOURCE=..\src\translator_dk.h
# End Source File
# Begin Source File

SOURCE=..\src\translator_en.h
# End Source File
# Begin Source File

SOURCE=..\src\translator_es.h
# End Source File
# Begin Source File

SOURCE=..\src\translator_fi.h
# End Source File
# Begin Source File

SOURCE=..\src\translator_fr.h
# End Source File
# Begin Source File

SOURCE=..\src\translator_gr.h
# End Source File
# Begin Source File

SOURCE=..\src\translator_hr.h
# End Source File
# Begin Source File

SOURCE=..\src\translator_hu.h
# End Source File
# Begin Source File

SOURCE=..\src\translator_it.h
# End Source File
# Begin Source File

SOURCE=..\src\translator_jp.h
# End Source File
# Begin Source File

SOURCE=..\src\translator_kr.h
# End Source File
# Begin Source File

SOURCE=..\src\translator_nl.h
# End Source File
# Begin Source File

SOURCE=..\src\translator_no.h
# End Source File
# Begin Source File

SOURCE=..\src\translator_pl.h
# End Source File
# Begin Source File

SOURCE=..\src\translator_pt.h
# End Source File
# Begin Source File

SOURCE=..\src\translator_ro.h
# End Source File
# Begin Source File

SOURCE=..\src\translator_ru.h
# End Source File
# Begin Source File

SOURCE=..\src\translator_se.h
# End Source File
# Begin Source File

SOURCE=..\src\translator_si.h
# End Source File
# Begin Source File

SOURCE=..\src\translator_sk.h
# End Source File
# Begin Source File

SOURCE=..\src\translator_std.h
# End Source File
# Begin Source File

SOURCE=..\src\translator_tw.h
# End Source File
# Begin Source File

SOURCE=..\src\translator_ua.h
# End Source File
# Begin Source File

SOURCE=..\libpng\trees.h
# End Source File
# Begin Source File

SOURCE=..\src\treeview.h
# End Source File
# Begin Source File

SOURCE=..\src\unistd.h
# End Source File
# Begin Source File

SOURCE=..\src\util.h
# End Source File
# Begin Source File

SOURCE=..\src\version.h
# End Source File
# Begin Source File

SOURCE=..\src\xmlgen.h
# End Source File
# Begin Source File

SOURCE=..\libpng\zconf.h
# End Source File
# Begin Source File

SOURCE=..\libpng\zlib.h
# End Source File
# Begin Source File

SOURCE=..\libpng\zutil.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
