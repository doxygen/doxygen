#
# Below is a stripped down version of bib2xhtml used by doxygen.
# For the full version see http://www.spinellis.gr/sw/textproc/bib2xhtml/
#
# Convert from bibtex to XHTML.
#
# (C) Copyright 1995, 1996 David Hull.
# (David Hull / hull@cs.uiuc.edu / http://www.uiuc.edu/ph/www/dlhull)
#
# (C) Copyright 2002-2010 Diomidis Spinellis
# http://www.spinellis.gr
#
# This program is free software.  You can redistribute it and/or modify
# it under the terms of the GNU General Public License.  See the
# files README and COPYING for details.
#
# This source code contains UTF-8 characters.  You might want to use
# an appropriate editor, if you want to view/modify the LaTeX to Unicode
# substitution commands.
#

use Getopt::Std;
use open IO => ':crlf';
$label_styles{'numbered'} = 	$LABEL_NUMBERED = 	2;
$list_start[$LABEL_NUMBERED] = 'dl class="citelist"';
$list_end[$LABEL_NUMBERED] = "/dl";
@tmpfiles = ();
sub html_ent {
	s/\\i\b/i/g;
	s/\\\'(\001\d+)\{([AEIOUaeiou])\1\}/&$2acute;/gs;
	s/\\\'([AEIOUaeiou])/&$1acute;/g;
	s/\\\`(\001\d+)\{([AEIOUaeiou])\1\}/&$2grave;/gs;
	s/\\\`([AEIOUaeiou])/&$1grave;/g;
	s/\\\"(\001\d+)\{([AEIOUaeiouy])\1\}/&$2uml;/gs;
	s/\\\"([AEIOUaeiouy])/&$1uml;/g;
	s/\\\~(\001\d+)\{([ANOano])\1\}/&$2tilde;/gs;
	s/\\\~([ANOano])/&$1tilde;/g;
	s/\\\^(\001\d+)\{([AEIOUaeiou])\1\}/&$2circ;/gs;
	s/\\\^([AEIOUaeiou])/&$1circ;/g;
	s/\\c(\001\d+)\{([Cc])\1\}/&$2cedil;/gs;
	s/\\u(\001\d+)\{(.)\1\}/$2/gs;
	s/\\v(\001\d+)\{(.)\1\}/$2/gs;
	s/\\([lL])\b/$1/g;
	s/\\\=(\001\d+)\{(.)\1\}/$2/gs;
	s/\\\=(.)/$1/g;
	s/\\\.(\001\d+)\{(.)\1\}/$2/gs;
	s/\\\.(.)/$1/g;
	s/\\([Oo])\b\s*/&$1slash;/g;
	s/\\AA\b\s*/&Aring;/g;
	s/\\aa\b\s*/&aring;/g;
	s/\\AE\b\s*/&AElig;/g;
	s/\\ae\b\s*/&aelig;/g;
	s/\\ss\b\s*/&szlig;/g;
	s/\\S\b\s*/&sect;/g;
	s/\\P\b\s*/&para;/g;
	s/\\pounds\b\s*/&pound;/g;
	s/\?\`/&iquest;/g;
	s/\!\`/&iexcl;/g;
	s/\-\-\-/&mdash;/g;
	s/([^\!])\-\-([^\>])/$1&ndash;$2/g;
	s/(CITEREF_[^\!])&ndash;([^\>])/$1--$2/g;
	s/(CITEREF_[^\!])&mdash;([^\>])/$1---$2/g;
	s/\\([aA]lpha)\b/&$1;/g;
	s/\\([bB]eta)\b/&$1;/g;
	s/\\([gG]amma)\b/&$1;/g;
	s/\\([dD]elta)\b/&$1;/g;
	s/\\varepsilon\b/&epsilon;/g;
	s/\\([eE]psilon)\b/&$1;/g;
	s/\\([zZ]eta)\b/&$1;/g;
	s/\\([eE]ta)\b/&$1;/g;
	s/\\([tT]heta)\b/&$1;/g;
	s/\\vartheta\b/&theta;/g;
	s/\\([iI]ota)\b/&$1;/g;
	s/\\([kK]appa)\b/&$1;/g;
	s/\\([lL]ambda)\b/&$1;/g;
	s/\\([mM]u)\b/&$1;/g;
	s/\\([nN]u)\b/&$1;/g;
	s/\\([xX]i)\b/&$1;/g;
	s/\\([oO]micron)\b/&$1;/g;
	s/\\([pP]i)\b/&$1;/g;
	s/\\varpi\b/&pi;/g;
	s/\\([rR]ho)\b/&$1;/g;
	s/\\varrho\b/&rho;/g;
	s/\\([sS]igma)\b/&$1;/g;
	s/\\varsigma\b/&sigmaf;/g;
	s/\\([tT]au)\b/&$1;/g;
	s/\\([uU]psilon)\b/&$1;/g;
	s/\\([pP]hi)\b/&$1;/g;
	s/\\varphi\b/&phi;/g;
	s/\\([cC]hi)\b/&$1;/g;
	s/\\([pP]si)\b/&$1;/g;
	s/\\([oO]mega)\b/&$1;/g;
	s/\\S\b/&sect;/g;
	s/^\\circ\b/&deg;/g;
	s/\\infty\b/&infin;/g;
	s/\\emptyset\b/&empty;/g;
	s/\\pm\b/&plusmn;/g;
	s/\\times\b/&times;/g;
	s/\\cdot\b/&sdot;/g;
	s/\\partial\b/&part;/g;
	s/\\nabla\b/&nabla;/g;
	s/\\surd\b/&radic;/g;
	s/\\perp\b/&perp;/g;
	s/\\sum\b/&sum;/g;
	s/\\int\b/&int;/g;
	s/\\prod\b/&prod;/g;
	s/\\sim\b/&sim;/g;
	s/\\approx\b/&asymp;/g;
	s/\\ne\b/&ne;/g;
	s/\\equiv\b/&equiv;/g;
	s/\\propto\b/&prop;/g;
	s/\\le\b/&le;/g;
	s/\\ge\b/&ge;/g;
	s/\\leftarrow\b/&larr;/g;
	s/\\rightarrow\b/&rarr;/g;
	s/\\in\b/&isin;/g;
	s/\\notin\b/&notin;/g;
	s/\\lceil\b/&lceil;/g;
	s/\\rceil\b/&rceil;/g;
	s/\\lfloor\b/&lfloor;/g;
	s/\\rfloor\b/&rfloor;/g;
	s/``/&ldquo;/g;
	s/''/&rdquo;/g;
	s/`/&lsquo;/g;
	s/'/&rsquo;/g;
}
$bdebug = 0;
foreach (@ARGV) {
  if (/\.bib$/) {
    $bibfile = $_;
    $bibfile =~ s/\.bib$//;
    push(@bibfiles,$bibfile);
  } elsif ("$_" eq "-d") {
    $bdebug = 1;
  } else {
    $htmlfile = $_;
  }
}
exit(1) unless defined($htmlfile);
$bibdatacmd="\\bibdata{".join(',',@bibfiles)."}";
$label_style = $LABEL_NUMBERED;
$bstfile = "doxygen";
umask(077);
open(HTMLFILE,">$htmlfile$$");
if (open(OHTMLFILE, "$htmlfile")) {
    $mode = (stat OHTMLFILE)[2] & 0xfff;
} else {
  print "Error opening $htmlfile\n";
  exit(1);
}
$beginstring = "<!-- BEGIN CITATIONS -->";
$endstring = "<!-- END CITATIONS -->";
@citations = ();
loop:
while (<OHTMLFILE>) {
  print HTMLFILE;
  last loop if m/^$beginstring$/;
}
loop:
while (<OHTMLFILE>) {
  print HTMLFILE;
  last loop if m/^$endstring$/;
  push(@citations, $2) if m/^([^\\]*)?(.+\})(.*)?$/;
}
push(@citations, $bibdatacmd);
$auxfile = "bib$$";
push(@tmpfiles, "$auxfile.aux");
open(AUXFILE, ">$auxfile" . ".aux");
print AUXFILE "\\relax\n\\bibstyle{$bstfile}\n";
foreach $citation (@citations) {
  print AUXFILE "$citation\n";
}
close(AUXFILE);
push(@tmpfiles, "$auxfile.blg");
push(@tmpfiles, "$auxfile.bbl");
`bibtex $auxfile 2>&1`;
if ($?==-1)
{
  print "bibtex command failed: $!\n";
}
$beginstring = "<!-- BEGIN BIBLIOGRAPHY -->";
$endstring = "<!-- END BIBLIOGRAPHY -->";
loop:
while (<OHTMLFILE>) {
  last loop if m/^$beginstring$/;
  print HTMLFILE;
}
loop:
while (<OHTMLFILE>) {
  last loop if m/^$endstring$/;
}
print HTMLFILE "$beginstring\n";
$t = $auxfile . ".bbl";
$/ = "";
open(BBLFILE, "<$t") || die "error opening $t: $!\n";
$nentry = 0;
loop:
while (<BBLFILE>) {
    if (($nentry == 0) && (m/^#/)) {
	if ((m/#\s*label-style:\s*(\S+)/) && (! defined $label_style)) {
	    $label_style = $label_styles{$1};
	    if (! defined $label_style) {
		print STDERR "label style unknown: \n";
		next loop;
	    }
	}
	next loop;
    }
    $nentry++;
    ($bcite, $blabel) = m:<dt><a\s+name=\"([^\"]*)\">\[([^\]]*)\]</a></dt><dd>:;
    $blabel = "$nentry,$2$3";
    $bibcite{$bcite} = $blabel;
    $bibnum{$bcite}  = $nentry;
}
close(BBLFILE);
$label_style = $LABEL_DEFAULT if (! defined $label_style);
$list_start = $list_start[$label_style];
$list_end = $list_end[$label_style];
print HTMLFILE "<$list_start>\n\n";
open(BBLFILE, "<$t") || die "error opening $t: $!\n";
$nentry = 0;
loop:
while (<BBLFILE>) {
    next loop if (($nentry == 0) && (m/^#/));
    $nentry++;
    s/\\\{/\002/g;
    s/\\\}/\003/g;
    s/\\\$/\004/g;
    s/\n  / /g;
    {
	local ($c, $l, $z) = (0, 0, ());
	s/([\{\}])/join("","\001",($1 eq "\{" ? $z[$l++]=$c++ : $z[--$l]),$1)/ge;
    }
    s/\%\n//g;
    s/(\.(<\/cite>|<\/a>|\')+)\./$1/g;
    s:(<dt><a\s+name=\"[^\"]*\">)\[([^\]]*)\](</a></dt><dd>):$1\[$nentry\]<!--\[$nentry,$2\]-->$3:;
    while (m/(\\(cite(label)?)(\001\d+)\{([^\001]+)\4\})/) {
	$old = $1;
	$cmd = $2;
	$doxref = defined($3);
	$bcite = $5;
	if (! defined $bibcite{$bcite}) {
	    $blabel = " [" . $bcite . "]";
	} elsif ($doxref) {
	    $blabel = " <a href=\"#$bcite\">[" . $bibnum{$bcite} . "]<\/a>";
	} else {
	    $blabel = " [" . $bibcite{$bcite} . "]";
	}
	$old =~ s/(\W)/\\$1/g;
	s/\s*$old/$blabel/g;
    }
    s/In (<a href=\"[^\"]*\">)([^\[]+) \[(\2)/In $1\[$2/;
    s/\\htmladdnormallink(foot)?(\001\d+)\{([^\001]+)\2\}(\001\d+)\{([^\001]+)\4\}/<a href="$5">$3<\/a>/gs;
    s/\&amp;/\005/g;
    s/\\?&/&amp;/g;
    s/\005/&amp;/g;
    html_ent();
    while (m/\\char([\'\"]?[0-9a-fA-F]+)/) {
	$o = $r = $1;
	if ($r =~ s/^\'//) {
	    $r = oct($r);
	} elsif ($r =~ s/^\"//) {
	    $r = hex($r);
	}
	s/\\char$o\s*/&#$r;/g;
    }
    s/{\\etalchar\001(\d+)\{(.)}\001\1\}/$2/g;
    s/\\par\b/<p \/>/g;
    s/\\url(\001\d+)\{(.*)\1\}/<a href="$2">$2<\/a>/gs;
    s/\\href(\001\d+)\{(.*)\1\}(\001\d+)\{([^\001]*)\3\}/<a href="$2">$4<\/a>/gs;
    s/\\href(\001\d+)\{(.*)\1\}/<a href="$2">$2<\/a>/gs;
    s/(\001\d+)\{\\rm\s+(.*)\1\}/$2/gs;
    s/\\textrm(\001\d+)\{(.*)\1\}/$2/gs;
    s/(\001\d+)\{\\em\s+(.*)\1\}/<em>$2<\/em>/gs;
    s/(\001\d+)\{\\it\s+(.*)\1\}/<i>$2<\/i>/gs;
    s/(\001\d+)\{\\bf\s+(.*)\1\}/<b>$2<\/b>/gs;
    s/(\001\d+)\{\\tt\s+(.*)\1\}/<tt>$2<\/tt>/gs;
    s/\\emph(\001\d+)\{(.*)\1\}/<em>$2<\/em>/gs;
    s/\\textit(\001\d+)\{(.*)\1\}/<i>$2<\/i>/gs;
    s/\\textbf(\001\d+)\{(.*)\1\}/<b>$2<\/b>/gs;
    s/\\texttt(\001\d+)\{(.*)\1\}/<tt>$2<\/tt>/gs;
    s/\\mathrm(\001\d+)\{(.*)\1\}/$2/gs;
    s/\\mathnormal(\001\d+)\{(.*)\1\}/$2/gs;
    s/\\mathsf(\001\d+)\{(.*)\1\}/$2/gs;
    s/\\mathbf(\001\d+)\{(.*)\1\}/<b>$2<\/b>/gs;
    s/\\mathcal(\001\d+)\{(.*)\1\}/<i>$2<\/i>/gs;
    s/\\mathit(\001\d+)\{(.*)\1\}/<i>$2<\/i>/gs;
    s/\\mathtt(\001\d+)\{(.*)\1\}/<tt>$2<\/tt>/gs;
    s/\\bibxhtmlname(\001\d+)\{(.*)\1\}/$2/ges;
    sub domath {
      local($t) = @_;
      $t =~ s/\^(\001\d+)\{\\circ\1\}/\&\#176;/gs;
      $t =~ s/\^\\circ/\&\#176;/g;
      $t =~ s/\^(\001\d+)\{(.*)\1\}/<sup>$2<\/sup>/gs;
      $t =~ s/\^(\w)/<sup>$1<\/sup>/g;
      $t =~ s/\_(\001\d+)\{(.*)\1\}/<sub>$2<\/sub>/gs;
      $t =~ s/\_(\w)/<sub>$1<\/sub>/g;
      $t;
    }
    s/(\$([^\$]+)\$)/&domath($2)/ge;
    s/(\\\((([^\\]|\\[^\(\)])+)\\\))/&domath($2)/ge;
    s/\\mbox(\001\d+)\{(.*)\1\}/$2/gs;
    while (s/(\<a href\=\"[^"]*?)\~/$1\005/g) { ; }
    s/et~al/et al/g;
    s/([^\\])~/$1&nbsp;/g;
    s/\\\,/&thinsp;/g;
    s/\\ldots\b/&hellip;/g;
    s/\\dots\b/&hellip;/g;
    s/\005/\~/g;
    s/\\ / /g;
    s/\\textasciitilde\b\s*/~/g;
    s/\\([\#\&\%\~\_\^\|])/$1/g;
    s/\\\W//g;
    s/\001(\d+)\{\\[A-Za-z]+\001(\d+)\{([^\001]*)\001\2\}\001\1\}/$3/g;
    s/\\([A-Za-z]+)/ $1 /g;
    s+In <a href=\"[^\"]*\"></a>++;
    s/\001\d+[\{\}]//gs;
    tr/\002\003\004/{}$/;
    print HTMLFILE $_;
}
close(BBLFILE);
print HTMLFILE "<$list_end>\n\n$endstring\n";
while (<OHTMLFILE>) {
  print HTMLFILE;
}
close (OHTMLFILE);
close(HTMLFILE);
chmod($mode, "$htmlfile$$");
rename("$htmlfile$$", $htmlfile);
if ($bdebug == 0) {
  unlink(@tmpfiles);
}
exit(0);
