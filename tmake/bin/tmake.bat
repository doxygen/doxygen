@rem = '--*-PERL-*--';
@rem = '
@echo off
rem setlocal
set ARGS=
:loop
if .%1==. goto endloop
set ARGS=%ARGS% %1
shift
goto loop
:endloop
rem ***** This assumes PERL is in the PATH *****
perl.exe -S tmake.bat %ARGS%
goto endofperl
@rem ';
#!/usr/bin/perl
############################################################################
# $Id$
#
# Creates a Makefile from a template and a project file.
#
# Copyright (C) 1996-1998 by Troll Tech AS.  All rights reserved.
#
# Permission to use, copy, modify, and distribute this software and its
# documentation for any purpose and without fee is hereby granted, provided
# that this copyright notice appears in all copies.
# No representations are made about the suitability of this software for any
# purpose. It is provided "as is" without express or implied warranty.
#
#
# Some important, global variables in tmake:
#   cpp_ext	C++ extension added to moc output (.cpp)
#   obj_ext	Object file extension (.o on Unix, .obj otherwise)
#   moc_aware	Will scan for files containing Qt signals/slots
#   moc_pre	Moc prefix for generated moc file: x.h -> moc_x.cpp
#   moc_ext	Moc extension for generated moc file: x.cpp -> x.moc
#   moc_cmd	The moc command in your makefile, $(MOC)
#   linebreak	Line break character (\)
#   dir_sep	Directory separator (/ on Unix, \ on Windows)
#   is_unix	Autodetected. If not Unix, assume Windows (Win32).
#
# If you need to customize any of these settings, do it before
# calling StdInit() in the template file.
#
############################################################################

if ($] < 5.0) {
    &tmake_error("This program requires perl version 5 or newer");
}

$cpp_ext       = "cpp";
$obj_ext       = "o";
$moc_aware     = 0;
$moc_pre       = "moc_";
$moc_ext       = "moc";
$moc_cmd       = '$(MOC)';
$linebreak     = "\\";
$dir_sep       = "/";
$is_unix       = 1;
$really_unix   = &check_unix();
$guess_os      = 1;
$depend_path   = "";
$nodepend      = 0;
$output_count  = 0;

$template_name = "";
$project_name  = "";
$outfile       = "";
@project_extra = ();
@project_files = ();
@eval_expr     = ();
$eval_done     = 0;

while ( @ARGV ) {				# parse command line args
    $_ = shift @ARGV;
    if ( s/^-// ) {
	if ( /^e(.*)/ ) {
	    push( @eval_expr, ($1 eq "") ? shift @ARGV : $1);
	} elsif ( /^t(.*)/ ) {
	    $template_name = ($1 eq "") ? shift @ARGV : $1;
	} elsif ( /^o(.*)/ ) {
	    $outfile = ($1 eq "") ? shift @ARGV : $1;
	    ($outfile eq "-") && ($outfile = "");
	} elsif ( /^p(.*)/ ) {
	    push( @project_files, ($1 eq "") ? shift @ARGV : $1);
	} elsif ( /^unix$/ ) {
	    $guess_os = 0;
	    $is_unix = 1;
	} elsif ( /^win32$/ ) {
	    $guess_os = 0;
	    $is_unix = 0;
	} elsif ( /^nodepend$/ ) {
	    $nodepend = 1;			# don't generate dependencies
	} elsif ( /^v$/ ) {
	    $verbose = 1;
	} else {
	    &tmake_usage();
	}
    } elsif ( /^\s*(?:[\w\-]+:)?\w+\s*[\+\-\*\/]?=/ ) { # project override
	push( @project_extra, $_ );
    } else {
	$project_name &&
	    &tmake_error("You can only specify one project file");
	$project_name = $_;
    }
}

($project_name ne "") || &tmake_usage();

if ( $guess_os && ! check_unix() ) {		# probably non-Unix,
    $is_unix = 0;
    &tmake_verb("Win32 detected");
}
if ( ! $is_unix ) {
    $obj_ext = "obj";
    $dir_sep = "\\";
}
$outfile eq "" || open(STDOUT,">" . fix_path($outfile)) ||
    &tmake_error("Can't create \"$outfile\"");

%project = ();
&ScanProject( &find_template("tmake.conf") );
&tmake_verb("Reading the project file $project_name");
if ( ! ($project_name =~ /\.pro$/i) && -f fix_path($project_name . ".pro") ) {
    $project_name .= ".pro";
}
$project{"PROJECT"} = $project_name;
$project{"PROJECT"} =~ s/\.pro$//i;
$project{"TARGET"} = $project{"PROJECT"};

unshift(@project_files,$project_name);
foreach ( @project_files ) {
    if ( ! ($_ =~ /\.pro$/i) && -f fix_path($_ . ".pro") ) {
	$_ .= ".pro";
    }
    if ( !&ScanProject($_) ) {
	&tmake_error("Can't open project file \"$_\"");
    }
}
&Project( @project_extra );

if ( $template_name eq "" ) {
    $template_name = $project{"TEMPLATE"} ?
	    $project{"TEMPLATE"} : "default.t";
}

foreach ( @eval_expr ) {
    $text = "";
    eval( $_ );
    die $@ if $@;
    print $text . "\n" if ($text ne "");
    $eval_done = 1;
}
if ( $eval_done ) {
    &tmake_verb("Done!");
    exit 0;
}

$template_name = &find_template($template_name);
&IncludeTemplate($template_name);
&tmake_verb("Done!");
exit 0;						# finished!



##############################################################################
# Subroutines from here
##############################################################################

#
# tmake_usage()
#
# Prints a message about program usage and exits
#

sub tmake_usage {
    print STDERR "Usage:\n    tmake [options] project-file\n";
    print STDERR "Options:\n";
    print STDERR "    -e expr    Evaluate expression, ignore template file\n";
    print STDERR "    -nodepend  Don't generate dependency information\n";
    print STDERR "    -o file    Write output to file\n";
    print STDERR "    -p project Load additional project file\n";
    print STDERR "    -t file    Specify a template file\n";
    print STDERR "    -unix      Create output for Unix (auto detects)\n";
    print STDERR "    -v         Verbose/debug mode\n";
    print STDERR "    -win32     Create output for Win32 (auto detects)\n";
    exit 1;
}


#
# tmake_error(msg)
#
# Prints the message and exits
#

sub tmake_error {
    my($msg) = @_;
    print STDERR "tmake error: " . $msg . "\n";
    exit 1;
}


#
# tmake_verb()
#
# Prints a verbose message
#

sub tmake_verb {
    my($msg) = @_;
    $verbose && print STDERR "tmake: " . $msg . "\n";
}


#
# check_unix()
#
# Returns 1 if this is a Unix, 0 otherwise.
#

sub check_unix {
    my($r);
    $r = 0;
    if ( -f "/bin/uname" ) {
	$r = 1;
	(-f "\\bin\\uname") && ($r = 0);
    }
    return $r;
}


#
# find_template(filename)
#
# Looks for the template file.
#   1.	search the current directory
#   2.	search the directories in TMAKEPATH
#   3.	search in $HOME/.tmake
#

sub find_template {
    my($filename) = @_;
    my($tb,$d,$p,@dirs);
    $tb = ($template_base eq "") ? "" : $template_base . ";";
    $d = ";" . $tb . $ENV{"TMAKEPATH"} . ";" . $ENV{"HOME"} . "/.tmake/";
    @dirs = &split_path( $d );
    $filename .= ".t" unless ($filename =~ /\.\w+$/);
    for $d ( @dirs ) {
	$p = $d . $filename;
	if ( -f fix_path($p) ) {
	    if ( $filename eq "tmake.conf" ) {
		$tmake_platform = $d;
		$tmake_platform =~ s-.*[/\\]([^/\\]*)[/\\]-$1-;
		&tmake_verb("Detected platform $tmake_platform");
	    }
	    return $p;
	}
	return ($d . $filename) if ( -f fix_path($d . $filename) );
    }
    &tmake_error("Template file " . $filename . " not found");
}


##############################################################################
# User functions
##############################################################################

#
# StdInit()
#
# Standard initialization
#

sub StdInit {
    my($p);
    return if $stdinit_done;
    $stdinit_done = 1;
    if ( defined($project{"OBJECTS_DIR"}) ) {
	$project{"OBJECTS_DIR"} = FixPath($project{"OBJECTS_DIR"});
    }
    if ( defined($project{"MOC_DIR"}) ) {
	$project{"MOC_DIR"} = FixPath($project{"MOC_DIR"});
    }
    $project{"OBJECTS"} = &Objects($project{"SOURCES"});
    if ( $moc_aware ) {
	$project{"_HDRMOC"} = &list_moc($project{"HEADERS"},$moc_pre,$cpp_ext);
	$project{"_SRCMOC"} = &list_moc($project{"SOURCES"},"",$moc_ext);
	$project{"OBJMOC"}  = &Objects($project{"_HDRMOC"});
	$p = $project{"_HDRMOC"} . " " . $project{"_SRCMOC"};
	$p =~ s/(^\s+|\s+$)//g;
	$project{"SRCMOC"} = $p;
    }
    &AddIncludePath("");
}


sub FixPath {
    my($p) = @_;
    if ( $p eq "." ) { $p = ""; }
    elsif ( length($p) > 0 ) {
	$p .= $dir_sep;
	$p =~ s-[\\/]+-${dir_sep}-g;
    }
    return $p;
}


#
# Config(name)
#
# Returns true if the project variable CONFIG contains the
# configuration name.
#

sub Config {
    my($name) = @_;
    return $project{"CONFIG"} =~ /\b$name\b/;
}


#
# DisableOutput()
#
# Disables tmake output.  Must be restored by calling a corresponding
# EnableOutput().
#

sub DisableOutput {
    $output_count++;
}


#
# EnableOutput()
#
# Enables tmake output again after DisableOutput() has been called.
#

sub EnableOutput {
    $output_count--;
}


#
# Now() - sets $text
#
# Sets $text to the current date and time.
#

sub Now {
    my($sec,$min,$hour,$mday,$mon,$year);
    ($sec,$min,$hour,$mday,$mon,$year) = localtime(time());
    $text = sprintf("%02d:%02d, %4d/%02d/%02d",
		    $hour, $min, 1900+$year, 1+$mon, $mday);
}


#
# expand_project_val(tag)
#
# Internal function for Project().
# Expands a project value string.
#

sub expand_project_val {
    my($v) = @_;
    my($c);
    $v =~ s/^\s+//;				# trim white space
    $v =~ s/\s+$//;
    $c = 0;
    while ( $c < 100 ) {			# expand $$
	if ( $v =~ s/(\$\$\w+)/\035/ ) {
	    $_ = $1;
	    s/\$\$//g;
	    $v =~ s/\035/$project{$_}/g;
	    $c++;
	} else {
	    $c = 100;
	}
    }
    return $v;
}


#
# Project(strings)
#
# This is a powerful function for setting or reading project variables.
# Returns the resulting project variables (joined with space between).
#
# Get a project variable:
#   $s = Project("TEMPLATE");	    -> $s = "TEMPLATE"
#
# Set a project variable:
#   Project("TEMPLATE = lib");	    -> TEMPLATE = lib
#   Project("CONFIG =";)	    -> CONFIG empty
#
# Append to a project variable:
#   Project("CONFIG = qt");	    -> CONFIG = qt
#   Project("CONFIG += debug");	    -> CONFIG = qt debug
#
# Append to a project variable if it does not contain the value already:
#   Project("CONFIG = qt release"); -> CONFIG = qt release
#   Project("CONFIG *= qt");	    -> CONFIG = qt release
#   Project("CONFIG *= opengl");    -> CONFIG = qt release opengl
#
# Subtract from a project variable:
#   Project("THINGS = abc xyz");    -> THINGS = abc xyz
#   Project("THINGS -= abc");	    -> THINGS = xyz
#
# Search/replace on a project variable:
#   Project("CONFIG = tq opengl");  -> CONFIG = tq opengl
#   Project("CONFIG /= s/tq/qt/");  -> CONFIG = qt opengl
#
# The operations can be performed on several project variables at a time.
#
#   Project("TEMPLATE = app", "CONFIG *= opengl", "THINGS += klm");
#

sub Project {
    my @settings = @_;
    my($r,$if_tag,$t,$s,$v,$p,$c);
    $r = "";
    foreach ( @settings ) {
	$v = $_;
	if ( $v =~ s/^\s*([\w\-]+:)?(\w+)\s*(\+=|\*=|\-=|\/=|=)\s*// ) {
	    $if_tag = $1;
	    if ( $if_tag ne "" ) {		
		chop $if_tag;
		if ( $if_tag eq "unix" ) {
		    return "" if !$is_unix;
		} elsif ( $if_tag eq "win32" ) {
		    return "" if $is_unix;
		} elsif ( ($if_tag ne $tmake_platform) && !Config($if_tag) ) {
		    return "";
		}
	    }
	    $t = $2;
	    $s = $3;
	    $v = expand_project_val($v);
	    $p = $project{$t};
	    if ( $s eq "=" ) {			# set variable
		$p = $v;
	    } elsif ( $s eq "+=" ) {		# append
		if ( $p eq "" ) {
		    $p = $v;
		} else {
		    $p .= " " . $v;
		}
	    } elsif ( $s eq "*=" ) {		# append if not contained
		if ( !($p =~ /(?:^|\s)\Q$v\E(?:\s|$)/) ) {
		    if ( $p eq "" ) {
			$p = $v;
		    } else {
			$p .= " " . $v;
		    }
		}
	    } elsif ( $s eq "-=" ) {		# subtract
		$p =~ s/$v//g;
	    } elsif ( $s eq "/=" ) {		# sed
		$cmd = '$p =~ ' . $v;
		eval $cmd;
	    }
	    $project{$t} = expand_project_val($p);
	} else {
	    $p = expand_project_val($project{$v});
	}
	if ( $p ne "" ) {
	    $r = ($r eq "") ? $p : ($r . " " . $p);
	}
    }
    return $r;
}


#
# Substitute(string)
#
# This function substitutes project variables in a text.
#
# Example:
#   Substitute('The project name is "$$PROJECT"')
#

sub Substitute {
    my($subst) = @_;
    $text = expand_project_val($subst);
    return $text;
}


#
# ScanProject(file)
#
# Scans a project file. Inserts project variables into the global
# associative project array.
#

sub ScanProject {
    my($file) = @_;
    my($tag,$var,@v,$more,$line);

    $tag = "";
    $line = 0;
    open(TMP,fix_path($file)) || return 0;

    while ( <TMP> ) {
	$line++;
	s/\#.*//;				# strip comment
	s/^\s+//;				# strip white space
	s/\s+$//;
	if ( /^\s*((?:[\w\-]+:)?\w+\s*(\+|\-|\*|\/)?=)/ ) {
	    $tag = $1;				# tag also contains the ".="
	    s/^.*?=\s*//;
	}
	if ( $tag ne "" ) {
	    $more = ( $_ =~ s/\s*\\\s*$// );	# more if \ at end of line
	    push( @v, split( /\s+/, $_ ) );
	    if ( ! $more ) {
		$var = join(" ",@v);
		Project( $tag . $var );
		$tag = "";
		@v = ();
	    }
	} elsif ( $_ ne "" ) {
	    tmake_error("$file:$line: Syntax error");
	}
    }
    close(TMP);
    return 1;
}


#
# IncludeTemplate(template_name)
#
# Includes and processes a template file.
#
# Below, we read the template file and executes any perl code found.
# Perl code comes after "#$". The variable $text contains the text
# to replace the perl code that was executed.
# Template comments begin with "#!".
#

sub IncludeTemplate {
    my($t_name) = @_;
    my($cmd,$cmd_block,$cmd_end,$is_cmd_block,$saveline,$spaceonly);
    local($text);
    local(*T);

    $t_name = &find_template($t_name);
    if ( $tmake_template_dict{$t_name} ) {
	&tmake_error("Cyclic template inclusion for $t_name");
    } else {
	$tmake_template_dict{$t_name} = 1;
    }
    $template_base = $t_name;
    $template_base =~ s-(.*[/\\]).*-$1-;
    &tmake_verb("Reading the template $t_name");
    open(T,fix_path($t_name)) ||
	&tmake_error("Can't open template file \"$t_name\"");

    while ( <T> ) {
	if ( /\#\!/ ) {				# tmake comment
	    s/\s*\#\!.*//;
	    next if /^$/;
	}
	if ( /\#\$(\{)?\s*(.*)\n/ ) {		# code
	    $cmd = $2;
	    $is_cmd_block = ($1 eq "{");
	    s/\#\$.*\n//;
	    if ( $is_cmd_block ) {		# code block #${ ...
		$saveline = $_;
		$cmd_block = $cmd;
		$cmd_end = 0;
		while ( <T> ) {
		    $cmd = $_;
		    $cmd =~ s/\s*\#\!.*//;	# tmake comment
		    if ( $cmd =~ /^\s*\#\$\}/ ) {
			$_ = "";
			$cmd_end = 1;
			last;
		    }
		    $cmd =~ s/^\s*\#(\$)?\s*//;
		    $cmd_block .= $cmd;
		}
		$cmd_end || &tmake_error('#$} expected but not found');
		$cmd = $cmd_block;
		$_ = $saveline;
	    }
	    $spaceonly = /^\s*$/;
	    $saveline = $_;
	    &tmake_verb("Evaluate: $cmd");
	    $text = "";
	    eval $cmd;
	    die $@ if $@;
	    next if $spaceonly && ($text =~ /^\s*$/);
	    print $saveline . $text . "\n" if  $output_count <= 0;
	} else {				# something else
	    print if $output_count <= 0;
	}
    }
    close( T );
}


#
# Expand(tag) - appends to $text
#
# Expands a list of $project{} variables with a space character between them.
#

sub Expand {
    my @tags = @_;
    my($t);
    $t = Project(@tags);
    if ( $text eq "" ) {
	$text = $t;
    } elsif ( $t ne "" ) {
	$text .= " " . $t;
    }
    return $text;
}


#
# ExpandGlue(tag,prepend,glue,append) - appends to $text
#
# Expands a $project{} tag, splits on whitespace
# and joins with $glue. $prepend is put at the start
# of the string and $append is put at the end of the
# string. The resulting string becomes "" if the project
# tag is empty or not defined.
#
# Example:
#
#   The project file defines:
#	SOURCES = a b c
#
#   ExpandGlue("SOURCES","<","-",">")
#
#   The result:
#	$text = "<a-b-c>"
#

sub ExpandGlue {
    my($tag,$prepend,$glue,$append) = @_;
    my($t,$v);
    $v = Project($tag);
    if ( $v eq "" ) {
	$t = "";
    } else {
	$t = $prepend . join($glue,split(/\s+/,$v)) . $append;
    }
    if ( $text eq "" ) {
	$text = $t;
    } elsif ( $t ne "" ) {
	$text .= " " . $t;
    }
    return $text;
}


#
# ExpandList(tag) - sets $text.
#
# Suitable for expanding HEADERS = ... etc. in a Makefile
#

sub ExpandList {
    my($tag) = @_;
    return ExpandGlue($tag,""," ${linebreak}\n\t\t","");
}


#
# TmakeSelf()
#
# Generates makefile rule to regenerate the makefile using tmake.
#

sub TmakeSelf {
    my $a = "tmake $project_name";
    if ( $nodepend ) {
	$a .= " -nodepend";
    }
    if ( $outfile ) {
	$text = "tmake: $outfile\n\n$outfile: $project_name\n\t";
	$a .= " -o $outfile";
    } else {
	$text = "tmake:\n\t";
    }
    $text .= $a
}


#
# Objects(files)
#
# Replaces any extension with .o ($obj_ext).
#

sub Objects {
    local($_) = @_;
    my(@a);
    @a = split(/\s+/,$_);
    foreach ( @a ) {
	s-\.\w+$-.${obj_ext}-;
	if ( defined($project{"OBJECTS_DIR"}) ) {
	    s-^.*[\\/]--;
	    $_ = $project{"OBJECTS_DIR"} . $_;
	}
    }
    return join(" ",@a);
}


#
# list_moc(files,prefix,extension)
#
# Scans all files and selects all files that contain Q_OBJECT.
# Insert a prefix before the filename and replaces the filename extention.
#

sub list_moc {
    my($files,$pre,$ext) = @_;
    my(@v,@m,@lines,$contents,$n,$f,$t);
    @v = split(/\s+/,$files);
    undef $/;
    foreach $f ( @v ) {
	if ( open(TMP,fix_path($f)) ) {
	    $contents = <TMP>;
	    close(TMP);
	    $n = 0;
	    @lines = split(/\n/,$contents);
	    grep( /tmake\s+ignore\s+Q_OBJECT/ && $n--, @lines );
	    $contents =~ s-/\*.*?\*/--gs; # strip C/C++ comments
	    $contents =~ s-//.*\n--g;
	    @lines = split(/\n/,$contents);
	    grep( /(^|\W)Q_OBJECT(\W|$)/ && $n++, @lines );
	    if ( $n > 0 ) {
		$t = $f;
		$t =~ s-^(.*[/\\])?([^/\\]*?)\.(\w+)$-$1${pre}$2.${ext}-;
		if ( defined($project{"MOC_DIR"}) ) {
		    $t =~ s-^.*[\\/]--;
		    $t = $project{"MOC_DIR"} . $t;
		}
		$moc_output{$f} = $t;
		$moc_input{$t}	= $f;
		push(@m,$t);
	    }
	    $contents = "";
	}
    }
    $/ = "\n";
    return join(" ",@m);
}


#
# BuildObj(objects,sources)
#
# Builds the object files.
#

sub BuildObj {
    my($obj,$src) = @_;
    my(@objv,$srcv,$i,$s,$o,$d,$c);
    @objv = split(/\s+/,$obj);
    @srcv = split(/\s+/,$src);
    for $i ( 0..$#objv ) {
	$s = $srcv[$i];
	$o = $objv[$i];
	next if $s eq "";
	$text .= $o . ": " . $s;
	if ( $moc_output{$s} ne "" ) {
	    $text .= " ${linebreak}\n\t\t" . $moc_output{$s};
	}
	$d = &make_depend($s);
	$text .= " ${linebreak}\n\t\t" . $d if $d ne "";
	if ( defined($project{"OBJECTS_DIR"}) ||
	     !defined($project{"TMAKE_COMPILE_IMP"}) ) {
	    $c = $project{"TMAKE_COMPILE"};
	    $c =~ s/\$src/$s/;
	    $c =~ s/\$obj/$o/;
	    $text .= "\n\t$c";
	}
	$text .= "\n\n";
    }
    chop $text;
}


#
# BuildMocObj(objects,sources)
#
# Builds the moc object files.
#

sub BuildMocObj {
    my($obj,$src) = @_;
    my(@objv,$srcv,$i,$s,$o,$hdr,$d);
    @objv = split(/\s+/,$obj);
    @srcv = split(/\s+/,$src);
    for $i ( 0..$#objv ) {
	$s = $srcv[$i];
	$o = $objv[$i];
	$hdr = $moc_input{$srcv[$i]};
	$text .= $o . ": " . $s . " ${linebreak}\n\t\t" . $hdr;
	$d = &make_depend($hdr);
	$text .= " ${linebreak}\n\t\t" . $d if $d ne "";
	if ( defined($project{"OBJECTS_DIR"}) || defined($project{"MOC_DIR"})||
	     !defined($project{"TMAKE_COMPILE_IMP"}) ) {
	    $c = $project{"TMAKE_COMPILE"};
	    $c =~ s/\$src/$s/;
	    $c =~ s/\$obj/$o/;
	    $text .= "\n\t$c";
	}
	$text .= "\n\n";
    }
    chop $text;
}


#
# BuildMocSrc(files)
#
# Builds the moc source files from headers and sources.
#

sub BuildMocSrc {
    my($f) = @_;
    my(@v,$m,$o);
    @v = split(/\s+/,$f);
    foreach $m ( @v ) {
	$o = $moc_output{$m};
	if ( $o ne "" ) {
	    $text .= "$o: $m\n\t$moc_cmd $m -o $o\n\n";
	}
    }
    chop $text;
}


#
# AddIncludePath(path)
#
# Adds path to the current include path, $project{"INCLUDEPATH"}.
#

sub AddIncludePath {
    my($path) = @_;
    my($p);
    if ( $project{"INCPATH"} &&
	 ($project{"INCPATH"} =~ /(?:^|\s)\Q$path\E(?:\s|$)/) ) {
	return;
    }
    $p = $project{"INCLUDEPATH"};
    $p = ($p && $path) ? ($p . ";" . $path) : ($p . $path);
    $project{"INCLUDEPATH"} = $p;
    $p = join(" ",&split_path($p));
    $p =~ s=[\\/](\s|$)= =g;
    $project{"INCPATH"} = $p;
}


#
# FindHighestLibVersion(dir,name)
#
# Returns the newest library version. Scans all the files in the specifies
# directory and returns the highest version number.
#
# Used on Windows only.
#
# Example:
#    FindHighestLibVersion("c:\qt\lib","qt") returns "200" if
#    the c:\qt\lib directory contains qt141.lib and qt200.lib.
#

sub FindHighestLibVersion {
    my($dir,$name) = @_;
    my(@files,$f,$v,$highest);
    $highest = "";
    @files = find_files($dir,"${name}.*\.lib");
    for $f ( @files ) {
	if ( $f =~ /(\d+)\.lib/ ) {
	    $v = $1;
	    if ( $highest eq "" || $v > $highest ) {
		$highest = $v;
	    }
	}
    }
    return $highest;
}


#
# Finds files.
#
# Examples:
#   find_files("/usr","\.cpp$",1)   - finds .cpp files in /usr and below
#   find_files("/tmp","^#",0)	    - finds #* files in /tmp
#

sub find_files {
    my($dir,$match,$descend) = @_;
    my($file,$p,@files);
    local(*D);
    $dir =~ s=\\=/=g;
    ($dir eq "") && ($dir = ".");
    if ( opendir(D,fix_path($dir)) ) {
	if ( $dir eq "." ) {
	    $dir = "";
	} else {
	    ($dir =~ /\/$/) || ($dir .= "/");
	}
	foreach $file ( readdir(D) ) {
	    next if ( $file  =~ /^\.\.?$/ );
	    $p = $dir . $file;
	    if ( $is_unix ) {
		($file =~ /$match/) && (push @files, $p);
	    } else {
		($file =~ /$match/i) && (push @files, $p);
	    }
	    if ( $descend && -d $p && ! -l $p ) {
		push @files, &find_files($p,$match,$descend);
	    }
	}
	closedir(D);
    }
    return @files;
}


#
# make_depend(file)
#
# Returns a list of included files.
# Uses the global $depend_path variable.
#

sub make_depend {
    my($file) = @_;
    my($i,$count);
    if ( $nodepend ) {
	return "";
    }
    if ( ! $depend_path_fixed ) {
	$depend_path_fixed = 1;
	$depend_path = $project{"DEPENDPATH"};
	$count = 0;
	while ( $count < 100 ) {
	    if ( $depend_path =~ s/(\$[\{\(]?\w+[\}\)]?)/035/ ) {
		$_ = $1;
		s/[\$\{\}\(\)]//g;
		$depend_path =~ s/035/$ENV{$_}/g;
	    } else {
		$count = 100;
	    }
	}
	@dep_path = &split_path($depend_path);
    }
    @cur_dep_path = @dep_path;
    if ( $file =~ /(.*[\/\\])/ ) {
	$dep_curdir = $1;
	splice( @cur_dep_path, 0, 0, $dep_curdir );
    } else {
	$dep_curdir = "";
    }
    $dep_file = $file;
    &canonical_dep($file);
    %dep_dict = ();
    $i = &build_dep($file);
    chop $i;
    $i =~ s=/=$dir_sep=g unless $is_unix;
    $i =~ s=([a-zA-Z]):/=//$1/=g if $gnuwin32;
    return join(" ${linebreak}\n\t\t",split(/ /,$i) );
}

#
# build_dep() - Internal for make_depend()
#

sub build_dep {
    my($file) = @_;
    my(@i,$a,$n);
    $a = "";
    return $a if !(defined $depend_dict{$file});
    @i = split(/ /,$depend_dict{$file});
    for $n ( @i ) {
	if ( !defined($dep_dict{$n}) && defined($full_path{$n}) ) {
	    $dep_dict{$n} = 1;
	    $a .= $full_path{$n} . " " . &build_dep($n);
	}
    }
    return $a;
}

#
# canonical_dep(file) - Internal for make_depend()
#
# Reads the file and all included files recursively.
# %depend_dict associates a file name to a list of included files.
#

sub canonical_dep {
    my($file) = @_;
    my(@inc,$i);
    @inc = &scan_dep($file);
    if ( @inc ) {
	$depend_dict{$file} = join(" ",@inc);
	for $i ( @inc ) {
	    &canonical_dep($i) if !defined($depend_dict{$i});
	}
    }
}

#
# scan_dep(file) - Internal for make_depend()
#
# Returns an array of included files.
#

sub scan_dep {
    my($file) = @_;
    my($dir,$path,$found,@allincs,@includes,%incs);
    $path = ($file eq $dep_file) ? $file : $dep_curdir . $file;
    @includes = ();
    return @includes if $file =~ /\.$moc_ext$/; # avoid .moc files
    if ( ! (-f fix_path($path)) ) {
	$found = 0;
	for $dir ( @cur_dep_path ) {
	    $path = $dir . $file;
	    last if ( $found = (-f fix_path($path)) );
	}
	return @includes if ! $found;
    }
    undef $/;
    if ( open(TMP,fix_path($path)) ) {
	$full_path{$file} = $path;
	$_ = <TMP>;
	s-/\*.*?\*/--gs;			# strip C/C++ comments
	s-//.*\n-\n-g;
	@allincs = split(/\n/,$_);
	@allincs = grep(/^\s*#\s*include/,@allincs);
	foreach ( @allincs ) {			# all #include lines
	    next if !(/^\s*#\s*include\s+[<"]([^>"]*)[>"]/) || defined($incs{$1});
	    push(@includes,$1);
	    $incs{$1} = "1";
	}
	close(TMP);
    }
    $/ = "\n";
    return @includes;
}


#
# split_path(path)
#
# Splits a path containing : (Unix) or ; (MSDOS, NT etc.) separators.
# Returns an array.
#

sub split_path {
    my($p) = @_;
    $p =~ s=:=;=g if $is_unix;
    $p =~ s=[/\\]+=/=g;
    $p =~ s=([^/:]);=$1/;=g;
    $p =~ s=([^:;/])$=$1/=;
    $p =~ s=/=$dir_sep=g unless $is_unix;
    return split(/;/,$p);
}


#
# fix_path(path)
#
# Converts all '\' to '/' if this really seems to be a Unix box.
#

sub fix_path {
    my($p) = @_;
    if ( $really_unix ) {
	$p =~ s-\\-/-g;
    } else {
	$p =~ s-/-\\-g;
    }
    return $p;
}
__END__
:endofperl
