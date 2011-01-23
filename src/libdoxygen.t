#
# 
#
# Copyright (C) 1997-2011 by Dimitri van Heesch.
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
#$ IncludeTemplate("lib.t");

LEX        = flex
YACC       = bison
INCBUFSIZE = $(PERL) increasebuffer.pl

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
	$(LEX) -PscanYY -t scanner.l | $(INCBUFSIZE) >scanner.cpp

#$ GenerateDep("code.cpp","code.l");
	$(LEX) -PcodeYY -t code.l | $(INCBUFSIZE) >code.cpp

#$ GenerateDep("pyscanner.cpp","pyscanner.l");
	$(LEX) -PpyscanYY -t pyscanner.l | $(INCBUFSIZE) >pyscanner.cpp

#$ GenerateDep("pycode.cpp","pycode.l");
	$(LEX) -PpycodeYY -t pycode.l | $(INCBUFSIZE) >pycode.cpp

#$ GenerateDep("fortranscanner.cpp","fortranscanner.l");
	$(LEX) -i -PfscanYY -t fortranscanner.l | $(INCBUFSIZE) >fortranscanner.cpp

#$ GenerateDep("fortrancode.cpp","fortrancode.l");
	$(LEX) -i -PfcodeYY -t fortrancode.l | $(INCBUFSIZE) >fortrancode.cpp

#$ GenerateDep("vhdlscanner.cpp","vhdlscanner.l");
	$(LEX) -i -PvhdlscanYY -t vhdlscanner.l | $(INCBUFSIZE) >vhdlscanner.cpp

#$ GenerateDep("vhdlcode.cpp","vhdlcode.l");
	$(LEX) -i -PvhdlcodeYY -t vhdlcode.l | $(INCBUFSIZE) >vhdlcode.cpp

#$ GenerateDep("pre.cpp","pre.l");
	$(LEX) -PpreYY -t pre.l | $(INCBUFSIZE) >pre.cpp 

#$ GenerateDep("declinfo.cpp","declinfo.l");
	$(LEX) -PdeclinfoYY -t declinfo.l | $(INCBUFSIZE) >declinfo.cpp

#$ GenerateDep("defargs.cpp","defargs.l");
	$(LEX) -PdefargsYY -t defargs.l | $(INCBUFSIZE) >defargs.cpp

#$ GenerateDep("doctokenizer.cpp","doctokenizer.l");
	$(LEX) -PdoctokenizerYY -t doctokenizer.l | $(INCBUFSIZE) >doctokenizer.cpp

#$ GenerateDep("commentcnv.cpp","commentcnv.l");
	$(LEX) -PcommentcnvYY -t commentcnv.l | $(INCBUFSIZE) >commentcnv.cpp

#$GenerateDep("commentscan.cpp","commentscan.l");
	$(LEX) -PcommentScanYY -t commentscan.l | $(INCBUFSIZE) >commentscan.cpp

#$ GenerateDep("ce_lex.cpp","constexp.l","ce_parse.h");
	$(LEX) -PcppExpYY -t constexp.l | $(INCBUFSIZE) >ce_lex.cpp 

#$ GenerateDep("ce_parse.cpp","constexp.y");
	$(YACC) -l -p cppExpYY constexp.y -o ce_parse.cpp 

#$ GenerateDep("ce_parse.h","constexp.y");
	$(YACC) -l -d -p cppExpYY constexp.y -o ce_parse.c 
	-rm ce_parse.c	

TO_C_CMD=sed -e "s/\\\\/\\\\\\\\/g" -e "s/\"/\\\\\"/g" -e "s/^/\"/g" -e "s/$$/\\\\n\"/g"

index_xsd.h: index.xsd
	cat index.xsd | $(TO_C_CMD) >index_xsd.h 

compound_xsd.h: compound.xsd
	cat compound.xsd | $(TO_C_CMD) >compound_xsd.h 

layout_default.h: layout_default.xml
	cat layout_default.xml | $(TO_C_CMD) >layout_default.h 

search_php.h: search.php
	cat search.php | $(TO_C_CMD) >search_php.h 

search_js.h: search.js
	cat search.js | $(TO_C_CMD) >search_js.h 

search_css.h: search.css
	cat search.css | $(TO_C_CMD) >search_css.h 

doxygen_css.h: doxygen.css
	cat doxygen.css | $(TO_C_CMD) >doxygen_css.h 

navtree_js.h: navtree.js
	cat navtree.js | $(TO_C_CMD) >navtree_js.h

navindex_js.h: navindex.js
	cat navindex.js | $(TO_C_CMD) >navindex_js.h

resize_js.h: resize.js
	cat resize.js | $(TO_C_CMD) >resize_js.h

jquery_js.h: jquery.js
	cat jquery.js | $(TO_C_CMD) >jquery_js.h

jquery_ui_js.h: jquery_ui.js
	cat jquery_ui.js | $(TO_C_CMD) >jquery_ui_js.h

sizzle_js.h: sizzle.js
	cat sizzle.js | $(TO_C_CMD) >sizzle_js.h

navtree_css.h: navtree.css
	cat navtree.css | $(TO_C_CMD) >navtree_css.h

