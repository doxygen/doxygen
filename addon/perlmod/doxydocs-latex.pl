#!/usr/bin/perl
# Copyright (C) 1997-2002 by Dimitri van Heesch.
# Author: Miguel Lobo.
#
# Permission to use, copy, modify, and distribute this software and its
# documentation under the terms of the GNU General Public License is hereby 
# granted. No representations are made about the suitability of this software 
# for any purpose. It is provided "as is" without express or implied warranty.
# See the GNU General Public License for more details.
#
# Documents produced by Doxygen are derivative works derived from the
# input used in their production; they are not affected by this license.

# This script can be used to generate LaTeX output using the new Perl
# intermediate format.  It is incomplete and experimental, so for the moment
# you should use Doxygen"s standard LaTeX output.
#
# If you want to try this out, you should do:
# doxygen configName		# This creates DoxyDocs.pm
# perl -I. doxydocs-latex.pl	# This creates doxydocs.tex and doxytemplate.tex
# latex doxydocs-latex.ltx	# This creates doxydocs-latex.dvi
#
# Notes:
# - Your config file has to have Perl output enabled, of course.
# - The Perl output format is only complete enough to document
#   a simple C file; no classes by the moment :-).

use DoxyModel;
use DoxyDocs;

sub latex_quote($) {
    my $text = $_[0];
    $text =~ s/[\\_&]/\\$&/g;
    print TEXDOCS $text;
}

sub generate_doc($) {
    my $doc = $_[0];
    for my $item (@$doc) {
	my $type = $$item{type};
	if ($type eq "text") {
	    latex_quote($$item{content});
	} elsif ($type eq "para") {
	    generate_doc($$item{content});
#	    print TEXDOCS "\\par\n";
	} elsif ($type eq "bold") {
	    print TEXDOCS "\\textbf{";
	    generate_doc($$item{content});
	    print TEXDOCS "}";
	} elsif ($type eq "emphasis") {
	    print TEXDOCS "\\textit{";
	    generate_doc($$item{content});
	    print TEXDOCS "}";
	} elsif ($type eq "url") {
	    latex_quote($$item{content});
	}
    }
}

sub generate($$) {
    my ($item, $model) = @_;
    my ($type, $name) = @$model[0, 1];
    if ($type eq "field") {
	print TEXDOCS "\\" . $name . "{";
	latex_quote($item);
	print TEXDOCS "}";
    } elsif ($type eq "doc") {
	if (@$item) {
	    print TEXDOCS "\\" . $name . "{";
	    generate_doc($item);
	    print TEXDOCS "}%\n";
	} else {
	    print TEXDOCS "\\" . $name . "Empty%\n";
	}
    } elsif ($type eq "hash") {
	my ($key, $value);
	while (($key, $submodel) = each %{$$model[2]}) {
	    my $name = $$submodel[1];
	    print TEXDOCS "\\def\\f" . $name . "{";
	    if ($$item{$key}) {
		generate($$item{$key}, $submodel);
	    } else {
	    	print TEXDOCS "\\" . $name . "Empty";
	    }
	    print TEXDOCS "}%\n";
	}
	print TEXDOCS "\\" . $name . "%\n";
    } elsif ($type eq "list") {
	my $index = 0;
	if (@$item) {
	    print TEXDOCS "\\" . $name . "{%\n";
	    for my $subitem (@$item) {
		if ($index) {
		    print TEXDOCS "\\" . $name . "Sep%\n";
		}
		generate($subitem, $$model[2]);
		$index++;
	    }
	    print TEXDOCS "}%\n";
	} else {
	    print TEXDOCS "\\" . $name . "Empty%\n";
	}
    }
}

open TEXDOCS, ">doxydocs.tex";
generate($doxydocs, $doxymodel);
close TEXDOCS;
