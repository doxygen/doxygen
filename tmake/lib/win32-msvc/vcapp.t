#!
#! This TMAKE template - Microsoft Visual C++ 5.0 applications
#!
#${
    if ( Config("qt") ) {
	if ( !(Project("DEFINES") =~ /QT_NODLL/) &&
	     ((Project("DEFINES") =~ /QT_(?:MAKE)?DLL/) ||
	      ($ENV{"QT_DLL"} && !$ENV{"QT_NODLL"})) ) {
	    Project('TMAKE_QT_DLL = 1');
	    if ( (Project("TARGET") eq "qt") && Project("TMAKE_LIB_FLAG") ) {
		Project('CONFIG += dll');
	    }
	}
    }
    if ( Config("qt") || Config("opengl") ) {
	Project('CONFIG += windows');
    }
    if ( Config("qt") ) {
	$moc_aware = 1;
	AddIncludePath(Project('TMAKE_INCDIR_QT'));
	if ( Config("opengl") ) {
	    Project('TMAKE_LIBS *= $$TMAKE_LIBS_QT_OPENGL');
	}
	if ( Project("TMAKE_QT_DLL") ) {
	    Project('DEFINES *= QT_DLL');
	}
	Project('TMAKE_LIBS *= $$TMAKE_LIBS_QT');
	if ( Project("TMAKE_QT_DLL") ) {
	    my $qtver =FindHighestLibVersion($ENV{"QTDIR"} . "/lib", "qt");
	    Project("TMAKE_LIBS /= s/qt.lib/qt${qtver}.lib/");
	    Project('TMAKE_LIBS *= $$TMAKE_LIBS_QT_DLL');
	}
    }
    if ( Config("opengl") ) {
	Project('TMAKE_LIBS *= $$TMAKE_LIBS_OPENGL');
    }
    Project('TMAKE_LIBS += $$LIBS');

    if ( Config("console") ) {
	$project{"VC_PROJ_TYPE"} = 'Win32 (x86) Console Application';
	$project{"VC_PROJ_CODE"} = '0x0103';
	$vc_base_libs = 'kernel32.lib user32.lib gdi32.lib winspool.lib ' .
			'comdlg32.lib advapi32.lib shell32.lib ole32.lib ' .
			'oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ';
	$vc_libs = $vc_base_libs;
	$vc_link_release = '/nologo /subsystem:console /machine:I386';
	$vc_link_debug   = '/nologo /subsystem:console /debug /machine:I386 /pdbtype:sept';
	$vc_cpp_def_release = '/D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" ';
	$vc_cpp_def_debug   = '/D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" ';
    } else {
	$project{"VC_PROJ_TYPE"} = 'Win32 (x86) Application';
	$project{"VC_PROJ_CODE"} = '0x0101';
	$vc_base_libs = 'kernel32.lib user32.lib gdi32.lib winspool.lib ' .
			'comdlg32.lib advapi32.lib shell32.lib ole32.lib ' .
			'oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ';
	$vc_libs = $vc_base_libs . 'wsock32.lib ';
	$vc_link_release = '/nologo /subsystem:windows /machine:I386';
	$vc_link_debug   = '/nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept';
	$vc_cpp_def_release = '/D "NDEBUG" /D "WIN32" /D "_WINDOWS" ';
	$vc_cpp_def_debug   = '/D "_DEBUG" /D "WIN32" /D "_WINDOWS" ';
    }
    $project{"VC_BASE_LINK_RELEASE"} = $vc_base_libs . $vc_link_release;
    $project{"VC_BASE_LINK_DEBUG"}   = $vc_base_libs . $vc_link_debug;
    $tmake_libs = Project('TMAKE_LIBS') ? (Project('TMAKE_LIBS') . " ") : "";
    $project{"VC_LINK_RELEASE"} = $vc_libs . $tmake_libs . $vc_link_release;
    $project{"VC_LINK_DEBUG"}   = $vc_libs . $tmake_libs . $vc_link_debug;

    $vc_cpp_opt_release = '/nologo /W3 /GX /O2 ';
    $vc_cpp_opt_debug   = '/nologo /W3 /Gm /GX /Zi /Od ';
    $vc_cpp_opt_common  = '/YX /FD /c';
    $project{"VC_BASE_CPP_RELEASE"} = $vc_cpp_opt_release . $vc_cpp_def_release . $vc_cpp_opt_common;
    $project{"VC_BASE_CPP_DEBUG"}   = $vc_cpp_opt_debug   . $vc_cpp_def_debug   . $vc_cpp_opt_common;
    ExpandPath("INCPATH",'/I ',' /I ','');
    if ( $text ne "" ) { $vc_inc = $text . " ";  $text = ""; } else { $vc_inc = ""; }
    ExpandGlue("DEFINES",'/D "','" /D "','"');
    if ( $text ne "" ) { $vc_def = $text . " ";  $text = ""; } else { $vc_def = ""; }
    $project{"VC_CPP_RELEASE"} = $vc_cpp_opt_release . $vc_inc . $vc_cpp_def_release . $vc_def . $vc_cpp_opt_common;
    $project{"VC_CPP_DEBUG"}   = $vc_cpp_opt_debug   . $vc_inc . $vc_cpp_def_debug   . $vc_def . $vc_cpp_opt_common;

    $project{"MAKEFILE"}  = $project{"PROJECT"} . ".mak";
    $project{"TARGETAPP"} = $project{"TARGET"}  . ".exe";
    Project('TMAKE_FILETAGS = HEADERS SOURCES TARGET DESTDIR $$FILETAGS');
    foreach ( split(/\s/,Project('TMAKE_FILETAGS')) ) {
	$project{$_} =~ s-/-\\-g;
    }
    StdInit();
    if ( defined($project{"DESTDIR"}) ) {
	$project{"TARGETAPP"} = $project{"DESTDIR"} . "\\" . $project{"TARGETAPP"};
	$project{"TARGETAPP"} =~ s/\\+/\\/g;
    }
    %all_files = ();
    @files = split(/\s+/,$project{"HEADERS"});
    foreach ( @files ) { $all_files{$_} = "h" };
    @files = split(/\s+/,$project{"SOURCES"});
    foreach ( @files ) { $all_files{$_} = "s" };
    if ( $moc_aware ) {
        @files = split(/\s+/,$project{"_HDRMOC"});
	foreach ( @files ) { $all_files{$_} = "m"; }
	@files = split(/\s+/,$project{"_SRCMOC"});
	foreach ( @files ) { $all_files{$_} = "i"; }
    }
    %file_names = ();
    foreach $f ( %all_files ) {
	$n = $f;
	$n =~ s/^.*\\//;
	$file_names{$n} = $f;
	$file_path{$n}  = ".\\" . $f;
	$file_path2{$n} = (($f =~ /^\./) ? "" : ".\\") . $f;
    }
#$}
# Microsoft Developer Studio Project File - #$ Substitute('Name="$$TARGET" - Package Owner=<4>');
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE #$ Substitute('"$$VC_PROJ_TYPE" $$VC_PROJ_CODE');

CFG=#$ Substitute('$$TARGET - Win32 Debug');
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "#$ ExpandGlue('MAKEFILE','','','".');
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f #$ Substitute('"$$MAKEFILE" CFG="$$TARGET - Win32 Debug"');
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE #$ Substitute('"$$TARGET - Win32 Release" (based on "$$VC_PROJ_TYPE")');
!MESSAGE #$ Substitute('"$$TARGET - Win32 Debug" (based on "$$VC_PROJ_TYPE")');
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
#$ Config("windows") && ($text='MTL=midl.exe');
RSC=rc.exe

!IF  "$(CFG)" == #$ Substitute('"$$TARGET - Win32 Release"');

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
#$ Config("windows") && ($text='# PROP Ignore_Export_Lib 0');
# PROP Target_Dir ""
# ADD BASE CPP #$ Expand("VC_BASE_CPP_RELEASE");
# ADD CPP #$ Expand("VC_CPP_RELEASE");
#$ Config("windows") || DisableOutput();
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
#$ Config("windows") || EnableOutput();
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 #$ Expand("VC_BASE_LINK_RELEASE");
# ADD LINK32 #$ Expand("VC_LINK_RELEASE");

!ELSEIF  "$(CFG)" == #$ Substitute('"$$TARGET - Win32 Debug"');

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
#$ Config("windows") && ($text='# PROP Ignore_Export_Lib 0');
# PROP Target_Dir ""
# ADD BASE CPP #$ Expand("VC_BASE_CPP_DEBUG");
# ADD CPP #$ Expand("VC_CPP_DEBUG");
#$ Config("windows") || DisableOutput();
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
#$ Config("windows") || EnableOutput();
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 #$ Expand("VC_BASE_LINK_DEBUG");
# ADD LINK32 #$ Expand("VC_LINK_DEBUG");

!ENDIF 

# Begin Target

# Name #$Substitute('"$$TARGET - Win32 Release"');
# Name #$Substitute('"$$TARGET - Win32 Debug"');
#${
    foreach $n ( sort keys %file_names ) {
	$f  = $file_names{$n};
	$p  = $file_path{$n};
	$p2 = $file_path2{$n};
	$t  = $all_files{$f};
	if ( ($t eq "h") && $moc_output{$f} ) {
	    my($build);
	    $build = "\n\n# Begin Custom Build - Running moc...\nInputPath=" . $p2 . "\n\n"
		   . '"' . $moc_output{$f} . '" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"'
		   . "\n\tmoc $p2 -o " . $moc_output{$f} . "\n\n"
		   . "# End Custom Build\n\n";
	    $text .= ("# Begin Source File\n\nSOURCE=$p\n\n"
		   . '!IF  "$(CFG)" == "' . $project{"TARGET"} . ' - Win32 Release"'
		   . $build);
	    $text .= ('!ELSEIF  "$(CFG)" == "' . $project{"TARGET"} . ' - Win32 Debug"'
		   . $build
		   . "!ENDIF \n\n# End Source File\n");
	} elsif ( $t eq "i" ) {
	    my($build,$dn);
	    $build = "\n\n# Begin Custom Build - Running moc...\nInputPath=" . $p2 . "\n\n"
		   . '"' . $f . '" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"'
		   . "\n\tmoc ". $moc_input{$f} . " -o $f\n\n"
		   . "# End Custom Build\n\n";
	    $dn = $n;
	    $dn =~ s/\..*//;
	    $dn =~ tr/a-z/A-Z/;
	    $text .= ("# Begin Source File\n\nSOURCE=$p\n"
		   . "USERDEP__$dn=" . '"' . $moc_input{$f} . '"' . "\n\n"
		   . '!IF  "$(CFG)" == "' . $project{"TARGET"} . ' - Win32 Release"'
		   . $build);
	    $text .= ('!ELSEIF  "$(CFG)" == "' . $project{"TARGET"} . ' - Win32 Debug"'
		   . $build
		   . "!ENDIF \n\n# End Source File\n");
	} elsif ( $t eq "s" || $t eq "m" || $t eq "h" ) {
	    $text .= "# Begin Source File\n\nSOURCE=$p\n# End Source File\n";
	}
    }
    chop $text;
#$}
# End Target
# End Project
