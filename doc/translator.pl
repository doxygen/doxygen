#! /usr/bin/perl -w
# -*- mode: perl; mode: fold -*-

# This is a Perl script for Doxygen developers.  
# Its main purpose is to extract the information from sources
# related to internationalization (the translator classes).
#
#                              Petr Prikryl (prikrylp@skil.cz)
# History:
# --------
# 2001/04/27
#  - First version of the script.
#
# 2002/05/02
#  - Update to accept updateNeededMessage() in the Translator class.
#  - First version that generates doc/language.doc.
#
################################################################

require 5.005;
use strict;
use Carp;

# Global variables
#
my $doxygenrootdir = "..";
my $srcdir = "$doxygenrootdir/src";  
my $docdir = "$doxygenrootdir/doc";  

# Names of the output files.
#
my $ftxt = "translator_report.txt";
my $fdoc = "language.doc";


################################################################
# GetPureVirtual returns a hash of pure virtual method prototypes
# in a hash where the key is the method prototype, and the value
# is 1.  The input argument is the full name of the source file.
#
sub GetPureVirtualFrom  ##{{{
{
    my $fin = shift;  # Get the file name.
    
    # Let's open the file and read it into a single string.
    #
    open(FIN, "< $fin") or die "\nError when open < $fin: $!";
    my @content = <FIN>;
    close FIN;
    my $cont = join("", @content);
    
    # Remove comments and empty lines.
    #
    $cont =~ s{\s*//.*$}{}mg; # remove one-line comments
    while ($cont =~ s{/\*.+?\*/}{}sg ) {}   # remove C comments
    $cont =~ s{\n\s*\n}{\n}sg;  # remove empty lines    

    # Remove the beginning up to the first virtual method.
    # Remove also the text behind the class.
    #
    $cont =~ s/^.*?virtual/virtual/s;
    $cont =~ s/\n\};.*$//s;

    # Erase anything between "=0;" and "virtual". Only the pure 
    # virtual methods will remain.  Remove also the text behind 
    # the last "= 0;"
    #
    $cont =~ s{(=\s*0\s*;).*?(virtual)}{$1 $2}sg;
    $cont =~ s{^(.+=\s*0\s*;).*?$}{$1}s;
    
    # Remove the empty implementation of the updateNeededMessage() 
    # method which is to be implemented by adapters only, not by 
    # translators.
    #
    $cont =~ s{\s*virtual
               \s+QCString
               \s+updateNeededMessage\(\)
               \s+\{\s*return\s+"";\s*\}
              }
              {}xs;
    
    # Replace all consequent white spaces (including \n) by a single 
    # space. Trim also the leading and the trailing space.
    #
    $cont =~ s{\s+}{ }sg;
    $cont =~ s{^\s+}{}s;
    $cont =~ s{\s+$}{}s;
    
    # Split the result to the lines again.  Remove the "= 0;".
    #
    $cont =~ s{\s*=\s*0\s*;\s*}{\n}sg;
    
    # Remove the keyword "virtual" because the derived classes 
    # may not use it.
    #
    $cont =~ s{^virtual\s+}{}mg;
    
    # Split the string into array of lines and fill the output hash.
    #
    my %result = ();
    
    foreach (split(/\n/, $cont)) {
        $result{$_} = 1;
    }
    
    return %result;
}
##}}}


################################################################
# GetInfoFrom returns the list of information related to the
# parsed source file.  The input argument is the name of the 
# translator_xx.h file including path. 
#
# The output list contains the following items:
#  - class identifier
#  - base class identifier
#  - method prototypes (each in a separate item)
#
sub GetInfoFrom  ##{{{
{
    # Get the file name.
    #
    my $fin = shift;

    # Let's open the file and read it into a single string.
    #
    open(FIN, "< $fin") or die "\nError when open < $fin: $!";
    my @content = <FIN>;
    close FIN;
    my $cont = join("", @content);
    
    # Remove comments and empty lines.
    #
    $cont =~ s{\s*//.*$}{}mg;    # remove one-line comments
    $cont =~ s{/\*.+?\*/}{}sg;   # remove C comments
    $cont =~ s{\n\s*\n}{\n}sg;   # remove empty lines    

    # Extract the class and base class identifiers.  Remove the 
    # opening curly brace.  Remove also the first "public:"
    # Put the class and the base class into the output list.
    #
    $cont =~ s{^.*class\s+(Translator\w+)[^:]*:
                \s*public\s+(\w+)\b.*?\{\s*
                (public\s*:\s+)?
              }
              {}sx;
    
    @content = ($1, $2);
    
    # Cut the things after the class.
    #
    $cont =~ s{\}\s*;\s*#endif\s*$}{}s;
    
    # Remove the "virtual" keyword, because some the derived class
    # is not forced to use it.
    #
    $cont =~ s{^\s*virtual\s+}{}mg;

    # Remove all strings from lines.
    #
    $cont =~ s{".*?"}{}mg;
    
    # Remove all bodies of methods;
    #
    while ($cont =~ s/{[^{}]+?}//sg) {}
    
    # Remove all private methods, i.e. from "private:" to "public:"
    # included.  Later, remove also all from "private:" to the end.
    #
    $cont =~ s{private\s*:.*?public\s*:}{}sg;
    $cont =~ s{private\s*:.*$}{}s;

    # Some of the translators use conditional compilation where 
    # the branches define the body of the method twice.  Remove
    # the ifdef/endif block content.
    # 
    $cont =~ s{#ifdef.*?#endif}{}sg;
    
    # Now the string should containt only method prototypes.
    # Let's unify their format by removing all spaces that 
    # are not necessary.  Then let's put all of them on separate
    # lines (one protototype -- one line; no empty lines).
    #
    $cont =~ s{\s+}{ }sg;
    $cont =~ s{^\s+}{}s;
    $cont =~ s{\s+$}{}s;
    
    $cont =~ s{\)\s*}{)\n}g;
    
    # Split the string and add it to the ouptut list.
    #
    @content = (@content, split(/\n/, $cont));
    return @content;
}
##}}}


################################################################
# GenerateLanguageDoc takes document templates and code sources
# generates the content as expected in the language.doc file (the
# part of the Doxygen documentation), and returns the result as a
# string.
#
sub GenerateLanguageDoc ##{{{
{
    # Get the references to the hash of class/base class.
    #
    my $rcb = shift;
    
    # Define templates for HTML table parts of the documentation. #{{{
    #
    my $htmlTableHead = <<'xxxTABLE_HEADxxx';
\htmlonly
<TABLE ALIGN=center CELLSPACING=0 CELLPADDING=0 BORDER=0>
<TR BGCOLOR="#000000">
<TD>
  <TABLE CELLSPACING=1 CELLPADDING=2 BORDER=0>
  <TR BGCOLOR="#4040c0">
  <TD ><b><font size=+1 color="#ffffff"> Language </font></b></TD>
  <TD ><b><font size=+1 color="#ffffff"> Maintainer </font></b></TD>
  <TD ><b><font size=+1 color="#ffffff"> Contact address </font>
          <font size=-2 color="#ffffff">(remove the NOSPAM.)</font></b></TD>
  <TD ><b><font size=+1 color="#ffffff"> Status </font></b></TD>
  </TR>
xxxTABLE_HEADxxx
  
    my $htmlTableRow = <<'xxxTABLE_ROWxxx';
  <TR BGCOLOR="#ffffff">
      <TD>$lang</TD>  
      <TD>$maintainer</TD>
      <TD>$email</TD>
      <TD>$status</TD>
  </TR>
xxxTABLE_ROWxxx

    my $htmlTableFoot = <<'xxxTABLE_FOOTxxx';
  </TABLE>
</TD>
</TR>
</TABLE>
\endhtmlonly
xxxTABLE_FOOTxxx
    ##}}}
    
    # Define templates for LaTeX table parts of the documentation. #{{{
    #
    my $latexTableHead = <<'xxxTABLE_HEADxxx';
\latexonly
\begin{tabular}{|l|l|l|l|}
  \hline 
  {\bf Language} & {\bf Maintainer} & {Contact address} & {Status} \\
xxxTABLE_HEADxxx
  
    my $latexTableRow = <<'xxxTABLE_ROWxxx';
  $lang & $maintainer & {\tt $email} & $status \\
xxxTABLE_ROWxxx

    my $latexTableFoot = <<'xxxTABLE_FOOTxxx';
  \hline
\end{tabular}
\endlatexonly
xxxTABLE_FOOTxxx
    ##}}}
    
    # Read the template of the documentation, and join the content
    # to a single string. #{{{
    #
    my $fin = "$docdir/language.tpl";
    open(FIN, "< $fin") or die "\nError when open < $fin: $!";
    my @content = <FIN>;
    close FIN;
    
    my $output = join("", @content);
    ##}}}
    
    # Make and substitute the list of supported languages and their
    # number. #{{{
    #
    my @languages = sort grep { s{^Translator}{} } keys %{$rcb};
    
    my $numlang = @languages;
    
    $output =~ s{\$numlang}{$numlang};
    
    my $languages =  join(", ", @languages);
    $languages =~ s{((\w+,\s){5})}{$1\n}g;
    $languages =~ s{Brazilian}{Brazilian Portuguese};
    $languages =~ s{(,\s+)(\w+)$}{$1and $2}s;
    
    $output =~ s{\$languages}{$languages};
    ##}}}

    # Create the hash of languages with the initial info. #{{{
    #
    my %language = ();
    
    foreach (@languages) {
        $language{$_} = $$rcb{"Translator$_"} . "<msep/>unknown: unknown";
    }
    ##}}}
    
    # Read the information related to maintainers into the
    # string using suitable separators -- one line, one language. #{{{
    #
    $fin = "$docdir/maintainers.txt";
    open(FIN, "< $fin") or die "\nError when open < $fin: $!";
    my @maintainers = <FIN>;
    close FIN;
    
    my $maintainers = join("", @maintainers);

    # Trim the spaces on the lines.  Strip the comment lines that
    # start with % sign.
    #
    $maintainers =~ s{^[ \t]+}{}mg;    
    $maintainers =~ s{[ \t]+$}{}mg;
    
    $maintainers =~ s{^%.*$}{}mg;

    # Join the information for one language into one line,
    # and remove empty lines.
    #
    $maintainers =~ s{\b\n\b}{<sep/>}sg;
    $maintainers =~ s{\n{2,}}{\n}sg;    
    $maintainers =~ s{^\n+}{}s;
    $maintainers =~ s{\n+$}{}s;
    ##}}}
    
    # Split the string back to the list, and update the information
    # in the hash with information for languages. #{{{
    #
    foreach my $line (sort split(/\n/, $maintainers)) {
        
        # Split the line for one language to separate lines for
        # the language and one or more maintainers.
        #
        my @info = split(/<sep\/>/, $line);
        
        my $lang = shift @info;
        
        # Ensure that the language starts with uppercase and 
        # continues with lowercase.
        #
        $lang =~ s{^(\w)(\w+)}{\U$1\L$2\E};

        # Add information to the %language hash.  If the language
        # was not defined in sources, add the question mark to the
        # language identifier.
        # 
        if (defined $language{$lang}) { 
            $language{$lang} = $$rcb{"Translator$lang"} . '<msep/>' 
                               . join("<sep/>", @info);
        }
        else {
            $lang .= " (?)";
            $language{$lang} = "unknown<msep/>" . join("<sep/>", @info);
        }
    }
    ##}}}

    # Now, the %language hash contains all the information needed for
    # generating the tables (HTML and LaTeX).  Define string variables
    # for each of the tables, and initialize them.  #{{{
    #
    my $tableHTML = $htmlTableHead;
    my $tableLATEX = $latexTableHead;
    ##}}}
    
    # Loop through sorted keys for the languages, parse the
    # information, and add it to the tables. #{{{
    #
    foreach my $lang (sort keys %language) {
        
        # Read the line with info for the language and separate 
        # the information of status. #{{{
        #
        my @list = split(/<msep\/>/, $language{$lang});
        my $status = shift @list;
        
        my $i = $status =~ s{^Translator$}{up-to-date};
        
        if ($i == 0) { 
            $i = $status =~ s{^TranslatorAdapter_(\d)_(\d)_(\d)}
                             {$1.$2.$3}x;
        }
        
        if ($i == 0) { $status = '?'; }
        
        ##}}}
        
        # Split the rest of the list (should be a single item) into
        # the list with one or more maintainers -- one line, one
        # maintainer. #{{{
        #
        my $rest = shift @list;
        @list = split(/<sep\/>/, $rest);
        ##}}}
        
        # In HTML table, maintainer names are placed in the same
        # cell.  Also their e-mails are placed in a single cell.
        # Extract the string with concatenated names and the string
        # with concatenated e-mails.  Add the row to the HTML
        # table. #{{{
        #
        my $name = '';
        my $email = '';

        foreach my $maintainer (@list) {
        
            if ($name ne '') { $name .= '<br>'; }
            if ($email ne '') { $email .= '<br>'; }

            $maintainer =~ m{^\s*(.+?)\s*:\s*(.+?)\s*$};
        
            $name .= $1;
            $email .= $2;
        }
        
        # Prepare the HTML row template, modify it, and add the
        # result to the HTML table.
        #
        my $item = $htmlTableRow;
        
        $item =~ s{\$lang}{$lang};
        $item =~ s{\$maintainer}{$name};
        $item =~ s{\$email}{$email};
        $item =~ s{\$status}{$status};
        
        $tableHTML .= $item;        
        
        ##}}}
        
        # For LaTeX, more maintainers for the same language are
        # placed on separate rows in the table.  The line separator
        # in the table is placed explicitly above the first
        # maintainer.  Add rows for all maintainers to the LaTeX
        # table. #{{{
        #
        # Prepare the LATEX row template, modify it, and add the
        # result to the LATEX table.
        #
        $item = $latexTableRow;

        my $first = shift @list;  # the first maintainer.
        $first =~ m{^\s*(.+?)\s*:\s*(.+?)\s*$};
        
        $name = $1;
        $email = $2;
        
        $item =~ s{\$lang}{$lang};
        $item =~ s{\$maintainer}{$name};
        $item =~ s{\$email}{$email};
        $item =~ s{\$status}{$status};
        
        $tableLATEX .= "  \\hline\n" . $item;        
        
        # List the other maintainers for the language. Do not set
        # lang and status for them.
        #
        while (@list) {
            my $next = shift @list;
            $next =~ m{^\s*(.+?)\s*:\s*(.+?)\s*$};
        
            my $name = $1;
            my $email = $2;
            my $item = $latexTableRow;
        
            $item =~ s{\$lang}{};
            $item =~ s{\$maintainer}{$name};
            $item =~ s{\$email}{$email};
            $item =~ s{\$status}{};
        
            $tableLATEX .= $item;        
        }
        ##}}}
    }
    ##}}}
    
    # Finish the tables, and substitute the mark in the doc
    # template by the contatenation of the tables. Add NOSPAM to
    # email addresses in the HTML table.  Replace the special
    # character sequences. #{{{
    #
    $tableHTML .= $htmlTableFoot;
    $tableLATEX .= $latexTableFoot;

    $tableHTML =~ s{@}{\@NOSPAM.}sg; 
    $tableHTML =~ s{&rcaron;}{&#x0159;}sg; 
    
    $tableLATEX =~ s/&rcaron;/\\v{r}/sg;
    $tableLATEX =~ s/&aacute;/\\'{a}/sg;
    $tableLATEX =~ s/&auml;/\\"{a}/sg;
    $tableLATEX =~ s/&ouml;/\\"{o}/sg;
    $tableLATEX =~ s/_/\\_/sg;
    
    $output =~ s{\$information_table}{$tableHTML$tableLATEX};    
    ##}}}

    # Replace the introduction notice in the output. #{{{
    #
    $output =~ s{<notice>.+?</notice>}
{Warning: this file was generated from the language.tpl template
 *          Do not edit this file.  Edit the template!}sx;
    ##}}}

    # Return the content of the generated output file.
    #
    return $output;
}
##}}}


################################################################
# Body
#
{
    # The translator base class must be present.  Exit otherwise. #{{{
    #
    if (!-f "$srcdir/translator.h")
    {
        print STDERR "\n\nThe translator.h not found in $srcdir.\n\n\a";
        exit 1;
    }
    ##}}}

    # Find all translator_xx.h files. #{{{
    #
    my @entries = ();  # init

    opendir DIR, $srcdir or confess "opendir error for $srcdir: $!";
    foreach (readdir(DIR)) { if (!/^\./) { push @entries, $_; } }
    closedir DIR;          # ignore names with dot at the beginning

    my @files = sort 
                grep { -f "$srcdir/$_" && m{^translator_..\.h$}i } 
                @entries;
    ##}}}
    
    # Get only the pure virtual methods from the Translator class
    # into a hash structure for later testing present/not present.
    #
    my %required = GetPureVirtualFrom("$srcdir/translator.h");

    # Collect base classes of translators the hash.
    #
    my %cb = ();
    
    # The details for translators will be collected into the output
    # string.
    #
    my $output = '';

    # Loop through all translator files.  Extract the implemented
    # virtual methods and compare it with the requirements. Prepare
    # the output.
    #
    foreach (@files) {                               

        # Get the information from the sources.  Remember the base
        # class for each of the classes. #{{{
        #
        my @info = GetInfoFrom("$srcdir/$_");
        
        my $class = shift @info;
        my $base = shift @info;

        $cb{$class} = $base;
        ##}}}
        
        # Set the value of the required methods to 1 (true).  Let
        # this indicate that the method was not defined in the
        # translator class.
        #
        foreach (keys %required) { $required{$_} = 1; }
        
        # Loop through all items and compare the prototypes. Mark
        # the implemented method and collect the old ones.  #{{{
        #
        my @old_methods = ();

        foreach my $method (@info) {
            if (defined $required{$method}) { $required{$method} = 0; }
            else                        {push(@old_methods, $method); }
        }
        ##}}}

        # Loop through the required hash and collect the missing
        # (new) methods.  Do this only when it derives from
        # Translator or TranslatorAdapter classes. #{{{
        #
        my @missing_methods = ();
        
        if ($base =~ m/^Translator(Adapter.*)?$/) {
            foreach (keys %required) { 
                if ($required{$_}) { push(@missing_methods, $_); }
            }
        }
        ##}}}

        # The detailed output will be produced only when it is
        # needed. #{{{
        #
        if (@old_methods || @missing_methods
            || $base !~ m/^Translator(Adapter.*)?$/) {

            $output .= "\n\n\n";
            $output .= $class . " ($base)\n" . '-' x length($class) . "\n";
            
            if ($base !~ m/^Translator(Adapter.*)?$/) {
                $output .= "\nThis is the unusual implementation of the "
                         . "translator.  Its class is derived\n"
                         . "from the $base base class. The usual translator"
                         . "class derives\n"
                         . "or from the Translator class or from some "
                         . "TranslatorAdapter_x_x_x classes.\n"
                         . "Because of that, nothing can be guessed about "
                         . "missing methods.\n";
            }
                
            if (@missing_methods) {
                $output .= "\nMissing methods (should be implemented):\n\n";
                foreach (@missing_methods) { $output .= "  $_\n"; }
            }    

            if (@old_methods) {
                $output .= "\nObsolete methods (should be removed):\n\n";
                foreach (sort @old_methods) { $output .= "  $_\n"; }
            }    
        }
        ##}}}
    }

    
    # Generate the textual output file.
    #
    my $fout = "$docdir/$ftxt";
    
    # Open it first.
    #
    open(FOUT, "> $fout") or die "\nError when open > $fout: $!";

    # List the supported languages. #{{{
    #
    my @all_translators = keys %cb;

    print FOUT "Doxygen supports the following (" . @all_translators
             . ") languages (sorted alphabetically):\n\n";
    
    foreach (sort grep { s/^Translator(\w+)\b.*$/$1/ } @all_translators) {
        print FOUT "  $_\n";
    }
    ##}}}

    # If there are up-to-date translators, list them.  #{{{
    #
    my @list = sort grep { $cb{$_} =~ m/^Translator$/ } keys %cb;
    
    if (@list) {
        print FOUT "\n" .'-' x 70 . "\n";
        print FOUT "The following translator classes are up-to-date " 
                 . "(sorted alphabetically).\n"
                 . "This means that they derive from the Translator class.  "
                 . "However, there still\n" 
                 . "may be some details listed below "
                 . "for them.  Please, check it.\n\n";
        
        foreach (@list) { print FOUT "  $_\n"; }
    }
    ##}}}

    # If there are obsolete translators, list them.  #{{{
    #
    @list = sort grep { $cb{$_} =~ m/^TranslatorAdapter/ } keys %cb;

    if (@list) {
        print FOUT "\n" .'-' x 70 . "\n";
        print FOUT "The following translator classes are obsolete "
                 . "(sorted alphabetically).\n"
                 . "This means that they derive from some of " 
                 . "the adapter classes.\n\n";
        
        foreach (@list) { print FOUT "  $_\t($cb{$_})\n"; }        
    }
    ##}}}

    # If there are other translators, list them. #{{{
    #
    @list = sort 
            grep { $cb{$_} !~ m/^Translator$/ } 
            grep { $cb{$_} !~ m/^TranslatorAdapter/ } 
            keys %cb;

    if (@list) {
        print FOUT "\n" .'-' x 70 . "\n";
        print FOUT "The following translator classes are somehow different\n"
                 . "(sorted alphabetically).  This means that they "
                 . "do not derive from\n"
                 . "the Translator class, nor from some of the adapter classes.\n\n";
        
        foreach (@list) { print FOUT "  $_\t($cb{$_})\n"; }        
    }
    ##}}}

    # List the methods that are expected to be implemented.  #{{{
    print FOUT "\n\n" .'-' x 70 . "\n";
    print FOUT "Localized translators are expected to implement "
             . "the following methods\n"
             . "(prototypes sorted aplhabetically):\n\n";

    foreach (sort keys(%required)) {
        print FOUT "$_\n";
    }
    ##}}}

    # If there are some details for the translators, show them.  #{{{
    #
    if ($output !~ m/^\s*$/) {
        print FOUT "\n\n" .'-' x 70 . "\n";
        print FOUT "Details related to specific translator classes follows.\n"
                 . "Notice that the prototypes are recognized only when they\n"
                 . "are the same as in the Translator class.";

        print FOUT $output . "\n";
    }
    ##}}}

    # Close the output file
    #
    close FOUT;

    # Generate the language.doc file.  
    #
    $fout = "$docdir/$fdoc";
    
    # Open it first for the output.
    #
    open(FOUT, "> $fout") or die "\nError when open > $fout: $!";

    print FOUT GenerateLanguageDoc(\%cb);

    # Close the output file
    #
    close FOUT;

    
    exit 0;
}
# end of body
################################################################

