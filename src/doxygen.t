#
# 
#
# Copyright (C) 1997-2001 by Dimitri van Heesch.
#
# Permission to use, copy, modify, and distribute this software and its
# documentation under the terms of the GNU General Public License is hereby 
# granted. No representations are made about the suitability of this software 
# for any purpose. It is provided "as is" without express or implied warranty.
# See the GNU General Public License for more details.
#
# Documents produced by Doxygen are derivative works derived from the
# input used in their production; they are not affected by this license.
#!
#! doxygen.t: This is a custom template for building Doxygen
#!
#$ IncludeTemplate("app.t");

LEX     =   flex
YACC    =   bison

#${
sub GenerateDep {
    my($obj,$src,$dep) = @_;
    my(@objv,$srcv,$i,$s,$o,$d,$c);
    @objv = split(/\s+/,$obj);
    @srcv = split(/\s+/,$src);
    for $i ( 0..$#objv ) {
        $s = $srcv[$i];
        $o = $objv[$i];
        next if $s eq "";
        $text .= $o . ": " . $s;
	$text .= " ${linebreak}\n\t\t" . $dep if $dep ne "";
        if ( $moc_output{$s} ne "" ) {
            $text .= " ${linebreak}\n\t\t" . $moc_output{$s};
        }
        $d = &make_depend($s);
        $text .= " ${linebreak}\n\t\t" . $d if $d ne "";
	$text .= "\n";
    }
    chop $text;
}
#$} 

####################

#$ GenerateDep("scanner.cpp","scanner.l");
	$(LEX) -PscanYY -t scanner.l >scanner.cpp

#$ GenerateDep("code.cpp","code.l");
	$(LEX) -PcodeYY -t code.l >code.cpp

#$ GenerateDep("pre.cpp","pre.l");
	$(LEX) -PpreYY -t pre.l >pre.cpp 

#$ GenerateDep("config.cpp","config.l");
	$(LEX) -PconfigYY -t config.l >config.cpp 

#$ GenerateDep("declinfo.cpp","declinfo.l");
	$(LEX) -PdeclinfoYY -t declinfo.l >declinfo.cpp

#$ GenerateDep("defargs.cpp","defargs.l");
	$(LEX) -PdefargsYY -t defargs.l >defargs.cpp

#$ GenerateDep("doc.cpp","doc.l");
	$(LEX) -PdocYY -t doc.l >doc.cpp

#$ GenerateDep("ce_lex.cpp","constexp.l","ce_parse.h");
	$(LEX) -PcppExpYY -t constexp.l >ce_lex.cpp 

#$ GenerateDep("ce_parse.cpp","constexp.y");
	$(YACC) -l -p cppExpYY constexp.y -o ce_parse.cpp 

#$ GenerateDep("ce_parse.h","constexp.y");
	$(YACC) -l -d -p cppExpYY constexp.y -o ce_parse.c 
	-rm ce_parse.c	

xml_dtd.h: doxygen.dtd
	cat doxygen.dtd | sed -e "s/\"/\\\\\"/g" -e "s/^/\"/g" -e "s/$$/\\\\n\"/g" >xml_dtd.h 

treeview.h: treeview.js
	cat treeview.js | sed -e "s/\\\\/\\\\\\\\/g" -e "s/\"/\\\\\"/g" -e "s/^/\"/g" -e "s/$$/\\\\n\"/g" >treeview.h 

