"""Script to generate reports on translator classes from Doxygen sources.

  The main purpose of the script is to extract the information from sources
  related to internationalization (the translator classes). It uses the
  information to generate documentation (language.doc,
  translator_report.txt) from templates (language.tpl, maintainers.txt).
  
  Simply run the script without parameters to get the reports and
  documentation for all supported languages. If you want to generate the
  translator report only for some languages, pass their codes as arguments
  to the script. In that case, the language.doc will not be generated.
  Example:
      
    python translator.py en nl cz
  
  Originally, the script was written in Perl and was known as translator.pl.
  The last Perl version was dated 2002/05/21
 
                               Petr Prikryl (prikrylp@skil.cz)
"""                               
                               
# History:
# --------
# 2002/05/21
#  - This was the last Perl version. 
# 2003/05/16
#  - If the script is given list of languages, only the translator report
#    is generated and only for those languages.
# 
################################################################

import os, re, sys

# Global informations are stored in the global 'info' dictionary.
# This dictionary should be accessed only via GetInfo() because the first
# call initializes the empty dictionary. (I was too lazy to create 
# a singleton.)
#
info = {}

def GetInfo():
    """Returns reference to the info dictionary.
    
    If the dictionary is empty, it will be filled with some initial values.
    """
    
    global info
    
    # If the dictionary with globally shared information is empty, then
    # fill the static values.
    if not info:
        # Get the name of the script without the path and the path without name.
        scriptpath, scriptname = os.path.split(os.path.abspath(sys.argv[0]))
        info['scriptname'] = scriptname

        # Determine the Doxygen's doc directory. If the DOXYGEN_DOCDIR
        # environment variable is defined, then it says where the directory
        # is. If it is not, then it will be directory where this script is
        # placed.
        docdir = os.getenv('DOXYGEN_DOCDIR', '*')
        if docdir == '*':
            docdir = scriptpath
            
        docdir = os.path.abspath(docdir)
        info['docdir'] = docdir

        # Doxygen's root directory is just one above the docdir.
        doxygenrootdir = os.path.abspath(os.path.join(docdir, '..'))
        info['doxygenrootdir'] = doxygenrootdir
        
        # Doxygen's src directory is just below its root.
        info['srcdir'] = os.path.join(doxygenrootdir, 'src')
        
        # Doxygen's current version is read from the 'version' file in the root.
        try:
            fver = file(os.path.join(doxygenrootdir, 'version'))
            doxversion = fver.readline().strip()
            fver.close()
            info['doxversion'] = doxversion
        except IOError:
            info['doxversion'] = 'unknown'
            
        
        # Names of the template files and other intput files (template for 
        # language.doc and the database of local language maintainers).
        info['flangtplname'] = 'language.tpl'
        info['flangtpl'] = os.path.join(docdir, info['flangtplname'])  
        info['fmaintainersname'] = 'maintainers.txt' 
        info['fmaintainers'] = os.path.join(docdir, info['fmaintainersname']) 

        # Names of the output files.
        info['ftranslatortxtname'] = 'translator_rep.txt'
        info['ftranslatortxt'] = os.path.join(docdir, info['ftranslatortxtname'])
        info['flangdocname'] = 'language.doc'
        info['flangdoc'] = os.path.join(docdir, info['flangdocname'])
        
        # If the script is given one or more arguments, they should be codes
        # of languages (two letters). Convert them into lower case and
        # build the list of them. Empty list will be interpreted as the request
        # for processing all languages.
        langlist = []
        if len(sys.argv) > 1:
            langlist = sys.argv[1:]
        info['languages'] = langlist
            
        # Create the dictionary of the required method. Keys will be the unified
        # method prototypes, values will be True (for easy testing).
        info['required_methods'] = {}
            
    return info

    
def CopyTemplateToLanguageDoc():
    """'flangtpl' + no src --> 'flangdoc' 
    
    The function takes the 'flangtpl' template and generates 'flangdoc'
    without using information from other sources. This function is called
    when source files were not found. The marks inside the template are
    replaced by warning-like explanations that something could not be done
    because sources were not available. Writes directly to the file, returns
    nothing.
    
    If the script was called only for selected languages, the documentation
    is not generated.
    """

    # Get the reference to the initialized info dictionary. If the script was
    # called for selected languages, return immediately.
    info = GetInfo()
    if not info['langlist']:
        return
    
    # Read the content of the template file.
    fin = file(info['flangtpl'])
    cont = fin.read()
    fin.close()
            
    # Replace the template marks by some notices.
    cont = re.sub(r'(?s)<notice>.+?</notice>', 
           """Warning: this file was generated from the %(flangtplname)s template
 *          by the %(scriptname)s script.  As doxygen sources were not available 
 *          in that time, some information could not be extracted 
 *          and inserted into this file.
 *
 *          Do not edit this file.  Edit the above mentioned files!""", cont)
    
    cont = re.sub(r'(?s)\$version', '%(doxversion)s', cont)
    cont = re.sub(r'(?s)\$numlang', 
        '<b>[number of supported languages could not be extracted -- no sources]</b>',
        cont)
            
    cont = re.sub(r'(?s)\$languages', 
        '<b>[names of languages could not be extracted -- no sources]</b>', cont)
            
    cont = re.sub(r'(?s)\$information_table',
        '<b>[Information table could not be extracted -- no sources.]</b>', cont)
            
    cont = re.sub(r'(?s)\$translator_report_file_name',
        '%(ftranslatortxt)s <b>[translator report could not be generated -- no sources]</b>',
        cont)
            
    cont = re.sub(r'(?s)\$translator_report_link',
        '<b>[no sources, no link]</b>', cont)

    # Replace the generated marks by the info from the info dictionary.
    #
    cont = cont % info
       
    # Write the template with replacements to the output doc.
    fout = file(info['flangdoc'], 'w')
    fout.write(cont)
    fout.close()

    
def GetPureVirtualFrom(filename):
    """Returns the list of pure virtual method prototypes from the filename.
    
    Each method prototype is returned as one string, one line, one list item).
    The input argument is the full name of the source file."""

    # Read the content of the file to one string and remove C comments, 
    # one line comments, leading text to the first 'virtual' keyword, 
    # text behind the class, and finally empty lines.  
    f = file(filename)
    cont = f.read()
    f.close()
    
    cont = re.sub(r'(?s)/\*.+?\*/', '', cont)   # C comments
    cont = re.sub(r'(?m)//.*$', '', cont)       # C++ comments
    cont = 'virtual ' + re.sub(r'(?s)^.+?virtual\s', '', cont) # prefix
    cont = re.sub(r'(?s)};.+$', '', cont)       # suffix
    cont = re.sub(r'(?s)\n\s*\n', r'\n', cont)  # empty lines
    
    # Remove the empty implementation of the updateNeededMessage() method
    # which is to be implemented by adapters only, not by translators.
    cont = re.sub(r'(?s)\s*virtual\s+QCString\s+updateNeededMessage.+?}.*?\n',
                  '', cont)

    # Erase anything between "=0;" and "virtual". Only the pure virtual 
    # methods will remain.  Remove also the text behind the last "= 0;"
    cont = re.sub(r'(?s)(=\s*0\s*;).*?(?P<vir>virtual)', r'=0;\n\g<vir>', cont)
    cont = re.sub(r'(?s)^(?P<x>.+=\s*0\s*;).*$', r'\g<x>', cont)

    # Replace all consequent white spaces (including \n) by a single 
    # space. Strip also the leading and the trailing space.
    cont = re.sub(r'(?s)\s+', ' ', cont)
    cont = cont.strip()
    
    # Split internally the string into lines by replacing the '= 0;' by '\n'.
    # Keep the string stil as one multiline string.
    cont = re.sub(r'(?s)\s*=\s*0\s*;\s*', r'\n', cont)

    # Remove the keyword 'virtual' because the derived classes may not use it.
    cont = re.sub(r'(?m)^virtual\s+', '', cont)

    # Split the string to the list of striped lines. Do strip the string 
    # first so that no empty line list item is generated.
    L = cont.strip().split('\n')
    
    # Build the list of unified prototypes and return it. 
    return L 
    
def StripArgIdentifiers(prototype):
    """Returns the method prototype without argument identifiers.
    
    The goal of the function is to get only the necessary, single form 
    of the method prototype. This way the prototypes from derived classes
    can be compared one by one with the methods in the base class."""
    
    # Parse the prototype, and split the string of arguments it can be empty).
    m = re.match(r'^(?P<prefix>.+?)\((?P<args>.*?)\)(?P<suffix>.*)$', prototype)
    str_prefix = m.group('prefix')
    str_args = m.group('args')
    str_suffix = m.group('suffix')
    args = str_args.split(',')
        
    # Each argument will be stripped and put to the new list. Only the types
    # are important. Using the spaces has to be normalized because people
    # differ in opinion where to put spaces. Let's prepare regular 
    # expressions for the tasks.
    rex_type = re.compile(r'''^(?P<type>        # name of the type group
                                \s*             # there can be spaces behind comma,
                                (const\s+)?     # possibly const at the beginning
                                [A-Za-z0-9_:]+  # type identifier can be qualified
                                (\s*[*&])?      # could be reference or pointer
                               )                # ... the above is important,
                              .*$''',           # the rest contains the identifier
                          re.VERBOSE)
                          
    # People may differ in opinion whether a space should or should not 
    # be written between a type identifier and  the '*' or '&' (when 
    # the argument is a pointer or a reference).
    rex_norm = re.compile(r'\s*(?P<x>[*&])')
                                  
    # Strip each of the arguments and put them to the 'stripped' list.
    # Only the type of the identifier is important. Extract it, normalize 
    # the using of spaces, and append the result to the list of striped
    # arguments. (Sequence of more than one space is solved later.)
    stripped = []
    for arg in args:
        arg = rex_type.sub(r'\g<type>', arg)
        arg = rex_norm.sub(r' \g<x>', arg)
        stripped.append(arg)
    
    # Join the stripped arguments into one line again, and build the striped
    # form of the prototype. Remove the duplicit spaces.
    result = re.sub(r'\s+', ' ', 
                    str_prefix + '(' + ', '.join(stripped) + ')' + str_suffix) 

    return result

def GetInfoFrom(input_filename):
    """Returns list of info related to the parsed file.
    
    GetInfoFrom returns the list of information related to the
    parsed source file.  The input argument is the name of the 
    translator_xx.h file including path. 
    
    The output list contains the following items:
     - class identifier
     - base class identifier
     - method prototypes (each in a separate item)"""
     
    # Let's open the file and read it into a single string.
    f = file(input_filename)
    cont = f.read()
    f.close()
    
    # Remove comments and empty lines.
    cont = re.sub(r'(?m)//.*$', '', cont)       # C++ comments
    cont = re.sub(r'(?s)/\*.+?\*/', '', cont)   # C comments
    cont = re.sub(r'(?s)\n\s*\n', r'\n', cont)  # empty lines

    # Extract the class and base class identifiers.
    rex = re.compile(r'''^.*class\s+
                         (?P<class>Translator\w+?)\s*:
                         \s*public\s+(?P<base>\w+)\b
                         ''', re.VERBOSE | re.DOTALL)
    m = rex.match(cont)
    assert(m)
    assert(m.group('class'))
    assert(m.group('base'))
    
    # Put the class and the base class into the output list.
    result = [m.group('class'), m.group('base')]
    
    # Remove the opening curly brace.  Remove also the first "public:"
    cont = re.sub(r'(?s)^.*?{', '', cont)
    cont = re.sub(r'(?s)(^.*\spublic:\s+)?', '', cont)
    
    # Cut the things after the class.
    cont = re.sub(r'(?s)}\s*;\s*#endif\s*$', '', cont)
    
    # Remove the "virtual" keyword, because the derived class is not forced 
    # to use it.
    cont = re.sub(r'\bvirtual\s+', '', cont)

    # Remove all strings from lines.
    cont = re.sub(r'(?s)".*?"', '', cont)
    
    # Remove all bodies of methods
    while cont.find('{') >= 0:
        cont = re.sub(r'(?s){[^{}]+?}', '', cont)
    
    # Remove all private methods, i.e. from "private:" to "public:"
    # included.  Later, remove also all from "private:" to the end.
    cont = re.sub(r'(?s)\bprivate\s*:.*?\bpublic\s*:', '', cont)
    cont = re.sub(r'(?s)\bprivate\s*:.*$', '', cont)

    # Some of the translators use conditional compilation where 
    # the branches define the body of the method twice.  Remove
    # the ifdef/endif block content.
    cont = re.sub(r'(?s)#ifdef.*?#endif', '', cont)
    
    # Now the string should containt only method prototypes. Let's unify
    # their format by removing all spaces that are not necessary.
    cont = re.sub(r'\s+', ' ', cont)
    cont = re.sub(r'^\s+', '', cont)
    cont = re.sub(r'\s+$', '', cont)
    
    # Then let's put all of them on separate lines (one protototype -- 
    # one line; no empty lines).
    cont = re.sub(r'\s+\(', '(', cont)
    cont = re.sub(r'(?s)\)\s*', ')\n', cont)

    # Split the string, add it to the output list, and return the result.
    result.extend(cont.strip().split('\n'))
    return result
    
    
def GetAdapterClassesInfo(required):
    """Returns the list of strings with information  related to the adapter classes.
    
    Each one-line string contains the identifier of the adapter class and
    the number of required methods that are implemented by the adapter.
    
    The function takes one agument -- the reference to the hash with
    stripped prototypes of the required methods."""

    # Let's open the file with the translator adapter classes.
    info = GetInfo()
    filename = os.path.join(info['srcdir'], 'translator_adapter.h')
    f = file(filename)
    cont = f.read()
    f.close()

    # Remove the preprocessor directives.
    cont = re.sub(r'(?m)^\s*#\w+.+$', '', cont)
    
    # Remove comments and empty lines.
    cont = re.sub(r'(?m)//.*$', '', cont)       # C++ comments
    cont = re.sub(r'(?s)/\*.+?\*/', '', cont)   # C comments
    cont = re.sub(r'(?s)\n\s*\n', r'\n', cont)  # empty lines

    # Place delimiters to separate the classes, and remove 
    # the TranslatorAdapterBase class.
    #
    cont = re.sub(r'(?s)};\s*class\s+', '<class>', cont)
    cont = re.sub(r'(?s)class\s+TranslatorAdapterBase\s+.+?<class>', '<class>', cont)
    cont = re.sub(r'(?s)};', '', cont)
    
    # Remove the base classes and the beginning of the the class definitions.
    cont = re.sub(r'(?s)(TranslatorAdapter[_0-9]+)\s*:.+?{\s*(public\s*:)?', 
                  '\g<1>', cont)

    # Remove all bodies of methods
    while cont.find('{') >= 0:
        cont = re.sub(r'(?s){[^{}]+?}', '', cont)

    # Remove the "virtual" keywords.
    cont = re.sub(r'(?m)^\s*virtual\s+', '', cont)

    # Remove the empty lines.
    cont = re.sub(r'(?s)\n\s*\n', '\n', cont)

    # Trim the spaces.
    cont = re.sub(r'(?m)^\s+', '', cont)
    cont = re.sub(r'(?m)\s+$', '', cont)
    
    # Split the string into the lines again.
    content = cont.split('\n')

    # Now the list contains only two kinds of lines.  The first
    # kind of lines starts with the <class> tag and contains the
    # identifier of the class.  The following lines list the
    # non-stripped prototypes of implemented methods without the
    # "virtual" keyword.
    #
    # Now we will produce the result by looping through all the
    # lines and counting the prototypes of the required methods
    # that are implemented by the adapter class.
    #
    cinfo = ''
    cnt = 0
    methods = ''
    result = []

    rex_class = re.compile(r'^<class>(?P<class>\w+)\s*$')     
    for line in content:
        m = rex_class.match(line)
        if m:
            # Another adapter class found. 
            adapter_class = m.group('class')
            
            # If the cinfo is not empty then it contains partial
            # information about the previously processed adapter. 
            if cinfo !=  '':
                # Complete the cinfo and push it into the result.
                s = ''
                if cnt != 1: 
                    s = 's'
                cinfo += '\timplements %2d required method%s...\n' % (cnt, s)
                result.append(cinfo + methods)
            
            # Initialize the counter and store the adapter class identifier
            # in the cinfo.
            #
            cinfo = adapter_class
            cnt = 0;
            methods = ''
        else:
            # The line contains the prototype of the implemented method.
            # If it is the required method, count it, and add it to the 
            # string of methods.
            stripped_prototype = StripArgIdentifiers(line)

            if required.has_key(stripped_prototype): 
                cnt += 1
                methods += '    %s\n' % line
    
    # If the cinfo is not empty then it contains partial
    # information about the last processed adapter. 
    if cinfo != '':
        # Complete the cinfo and push it into the @result.
        s = ''
        if cnt != 1: 
            s = 's'
        cinfo += '\timplements %2d required method%s...\n' % (cnt, s)
        result.append(cinfo + methods)

    # Return the result list.
    return result


def GetLanguagesInfo(seq_or_dic):
    """Returns (numlang, langlst, formated langstr).
    
    The numlang is the number of supported languages. The langlst is the 
    list of pairs like ('TranslatorBrazilian', 'Brazilian Portuguese')
    """
    
    languages = []               
    for k in seq_or_dic:
        # Remove the 'Translator' prefix from the class name to obtain
        # the brief name of the language.
        assert(k[:len('Translator')] == 'Translator')
        lang = k[len('Translator'):]
        
        # Do correction of the language name for the selected languages.
        if lang == 'Brazilian':           lang = 'Brazilian Portuguese'
        if lang == 'Chinesetraditional':  lang = 'Chinese Traditional'

        # Append the language to the list.
        languages.append((k, lang))
        
    # Sort the languages and construct the output string. Insert new line
    # after each line. Add the 'and' before the last language
    languages.sort()
    count = 0
    output = ''
    for L in languages:
        output += L[1]                     # readable form of the language
        count += 1
        
        if count < len(languages) - 1:     # separate by comma
            output += ', '
        elif count == len(languages) - 1:  # separate by comma and 'and'
            output += ', and '
        
        if count % 5 == 0:
            output += '\n'

    return (len(languages), languages, output)
    
    
def GenerateLanguageDoc(cb):
    """Generates the content as expected in the 'flangdoc' file.
    
    GenerateLanguageDoc takes document templates and code sources
    generates the content as expected in the 'flangdoc' file (the
    part of the Doxygen documentation), and returns the result as a
    string.
    
    The input parameter is the reference to the class/base dictionary."""
    
    # Define templates for HTML table parts of the documentation.
    htmlTableHead = r'''\htmlonly
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
'''
  
    htmlTableRow = r'''  <TR BGCOLOR="#ffffff">
      <TD>$lang</TD>  
      <TD>$maintainer</TD>
      <TD>$email</TD>
      <TD>$status</TD>
  </TR>
'''

    htmlTableFoot = r'''  </TABLE>
</TD>
</TR>
</TABLE>
\endhtmlonly
'''
    
    # Define templates for LaTeX table parts of the documentation.
    latexTableHead = r'''\latexonly
\begin{tabular}{|l|l|l|l|}
  \hline 
  {\bf Language} & {\bf Maintainer} & {\bf Contact address} & {\bf Status} \\
  \hline
'''
  
    latexTableRow = r'''  $lang & $maintainer & {\tt $email} & $status \\
'''

    latexTableFoot = r'''  \hline
\end{tabular}
\endlatexonly
'''

    # Read the template of the documentation, and join the content
    # to a single string.
    info = GetInfo()
    filename = os.path.join(info['docdir'], info['flangtpl'])
    f = file(filename)
    output = f.read()
    f.close()
    
    # Get the number of languages, list of their names and formated string
    # with the list in human readable English form.
    (numlang, langlst, langstr) = GetLanguagesInfo(cb)
    
    # Substitute the marks inside the template.
    output = re.sub(r'(?s)\$version', info['doxversion'], output)
    output = re.sub(r'(?s)\$numlang', str(numlang), output)
    output = re.sub(r'(?s)\$languages', langstr, output)

    # Create the dictionary for info for each language.
    langinfo = {}
    for (trClass, langname) in langlst:
        langinfo[langname] = cb[trClass] + '<msep/>unknown: unknown'

    # Read the information related to maintainers into the
    # string using suitable separators -- one line, one language. #{{{
    filename = os.path.join(info['docdir'], info['fmaintainers'])
    print filename
    f = file(filename)
    maintainers = f.read()
    f.close()
    
    # Trim the spaces on the lines.  Strip the comment lines that
    # start with % sign.
    maintainers = re.sub(r'(?m)^[ \t]+', '', maintainers)    
    maintainers = re.sub(r'(?m)[ \t]+$', '', maintainers)    
    maintainers = re.sub(r'(?m)^%.*$', '', maintainers)    
    
    # Join the information for one language into one line,
    # and remove empty lines.
    maintainers = re.sub(r'(?s)\b\n\b', '<sep/>', maintainers)
    maintainers = re.sub(r'(?s)\n{2,}', '\n', maintainers)
    maintainers = re.sub(r'(?s)^\n+', '', maintainers)
    maintainers = re.sub(r'(?s)\n+$', '', maintainers)
    
    # Split the string back to the list, and update the information
    # in the hash with information for languages.
    lst = maintainers.split('\n')
    lst.sort()
    for line in lst:
        # Split the line for one language to separate lines for
        # the language and one or more maintainers.  Ensure that the language
        # starts with uppercase and continues with lowercase. (It will be used
        # for reconstructing the translator class identifier.)
        linfo = line.split('<sep/>')
        lang = linfo[0].capitalize()
        del linfo[0]
        
        # Add information to the langinfo dictionary.  If the language
        # was not defined in sources, add the question mark to the
        # language identifier.
        # 
        if langinfo.has_key(lang): 
            langinfo[lang] = cb['Translator' + lang] + '<msep/>' + \
                             '<sep/>'.join(linfo)
        else:
            lang += ' (?)'
            langinfo[lang] = 'unknown<msep/>' + '<sep/>'.join(linfo)

    # Now, the langinfo dictionary contains all the information needed for
    # generating the tables (HTML and LaTeX).  Define string variables
    # for each of the tables, and initialize them.
    #
    tableHTML = htmlTableHead
    tableLATEX = latexTableHead
    
    # Loop through sorted keys for the languages, parse the
    # information, and add it to the tables.
    langs = langinfo.keys()
    langs.sort()
    for lang in langs:
        # Transform the key for the language into more human readable
        # form.  Basically, only languages with two words are going to be
        # corrected.
        if lang == 'Brazilian':
            lang_readable = 'Brazilian Portuguese'
        elif lang == 'Chinesetraditional':
            lang_readable = 'Chinese Traditional'
        else:
            lang_readable = lang
            
        print lang, lang_readable            
    """
        
        # Read the line with info for the language and separate 
        # the status. #{{{
        #
        my @list = split(/<msep\/>/, $language{$lang});
        my $status = shift @list;
        
        my $i = $status =~ s{^Translator$}{up-to-date};

        if ($i == 0) { 
            $i = $status =~ s{^TranslatorAdapter_(\d)_(\d)_(\d)}
                             {$1.$2.$3}x;
        }
        
        if ($i == 0) { 
            $i = $status =~ s{^TranslatorEnglish$}
                             {obsolete}x;
        }
        
        if ($i == 0) { $status = 'strange'; }
        
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
        
        $item =~ s{\$lang}{$lang_readable};
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
        
        $item =~ s{\$lang}{$lang_readable};
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
    # template by the concatenation of the tables.  Add NOSPAM to
    # email addresses in the HTML table.  Replace the special
    # character sequences. #{{{
    #
    $tableHTML .= $htmlTableFoot;
    $tableLATEX .= $latexTableFoot;

    $tableHTML =~ s{@}{\@NOSPAM.}sg; 
    $tableHTML =~ s{&ccaron;}{&#x010d;}sg; 
    $tableHTML =~ s{&rcaron;}{&#x0159;}sg; 
    
    $tableLATEX =~ s/&aacute;/\\'{a}/sg;
    $tableLATEX =~ s/&auml;/\\"{a}/sg;
    $tableLATEX =~ s/&ouml;/\\"{o}/sg;
    $tableLATEX =~ s/&oslash;/\\o{}/sg;
    $tableLATEX =~ s/&ccaron;/\\v{c}/sg;
    $tableLATEX =~ s/&rcaron;/\\v{r}/sg;
    $tableLATEX =~ s/_/\\_/sg;
    
    $output =~ s{\$information_table}{$tableHTML$tableLATEX};

    ##}}}

    # Replace the other symbols in the template by the expected
    # information. ##{{{
    #
    $output =~ s{\$version}{$doxversion};

    $output =~ s{\$translator_report_file_name}
                {<code>doxygen/doc/$ftranslatortxt</code>}x;

    $output =~ s{\$translator_report_link}
                {<a href=\"../doc/$ftranslatortxt\">
                 <code>doxygen/doc/$ftranslatortxt</code></a>}x;
    ##}}}

    # Replace the introduction notice in the output. #{{{
    #
    $output =~ s{<notice>.+?</notice>}
{Warning: this file was generated from the $flangtpl template
 *          and the $fmaintainers files by the $0 script.  
 *
 *          Do not edit this file.  Edit the above mentioned files!}sx;
    ##}}}

    # Return the content of the generated output file.
    #
    return $output;
}
"""
    return output
    
    
################################################################# Body
if __name__ == '__main__':
    # Get the reference to the initialized dictionary with the shared info. 
    info = GetInfo()
    
    # File with the translator base class must be present.  Exit otherwise,
    # but be kind to those who already have the documentation
    # generated by this script ready, but who do not have sources. 
    # If no 'flangdoc' is present, copy the template to it.
    if not os.path.isfile(os.path.join(info['srcdir'], 'translator.h')):
        sys.stderr.write(('\nThe %(scriptname)s warning:\n' +
                         '\tThe translator.h not found in %(srcdir)s.\n' +
                         '\tThe %(ftranslatortxtname)s will not be ' +
                         "generated (you don't need it).\n") % info)

        # $flangdoc is not present, copy the template to it, and do the simplified
        # replacement of the markers inside the template. Generate the warning
        # about 'flangdoc' content.
        if not os.path.isfile(info['flangdoc']):
            CopyTemplateToLanguageDoc();
            sys.stderr.write(('\nThe %(scriptname)s warning:\n' +
                  "\tThe %(flangdoc)s not found in the '%(docdir)s' directory.\n" +
                  '\tThe %(flangtpl)s template content copied into it.\n' +
                  '\tAs the sources are not available, some information\n' +
                  '\tcould not be extracted and inserted into %(flangdoc)s.\n') % info)

        # Exit as if nothing happened.
        sys.exit(0)

    # Create the list of all translator_xxxx.h file names. If the script was
    # given a list of languages (two letters for each language), then fill
    # the list only by the translator files for the languages.
    directory = info['srcdir']
    langlist = info['languages']
    if langlist:
        files = [os.path.join(directory, 'translator_%s.h' % lang)
                 for lang in langlist
                 if os.path.isfile(os.path.join(directory, 
                                                'translator_%s.h' % lang))]
    else:
        rex_tr = re.compile(r'^translator_\w\w\w?\.h$', re.IGNORECASE)
        files = [os.path.join(directory, f) 
                 for f in os.listdir(directory) 
                 if os.path.isfile(os.path.join(directory, f)) 
                    and rex_tr.match(f)]

    # Get only the pure virtual methods from the Translator class
    # into a list for later testing present/not present.
    expected_lst = GetPureVirtualFrom(os.path.join(info['srcdir'], 
                                                   'translator.h'))
    # Fill the 'required_methods' dictionary for unified form 
    # of the prototypes.
    required = info['required_methods']
    for method in expected_lst:
        prototype = StripArgIdentifiers(method)
        required[prototype] = True

    # The details for translators will be collected into the output
    # string. If some details are listed for a translator, the flag
    # will be set to produce possible warning to the list of
    # up-to-date translators.
    output = ''
    details = {}
    
    # Collect base classes of translators in the hash. CB stands
    # for Class and Base.
    cb = {}
        
    # Loop through all translator files.  Extract the implemented
    # virtual methods and compare it with the requirements. Prepare
    # the output.
    rex_trAdapter = re.compile(r'^TranslatorAdapter_')
    rex_trEN = re.compile(r'^TranslatorEnglish$')
    
    for filename in files:
        # Get the information from the sources.  Remember the base
        # class for each of the classes.  Clear the flag for
        # details for the class.
        finfo = GetInfoFrom(filename)
        (class_, base_) = finfo[0:2]
        cb[class_] = base_
        details[class_] = False
        
        # Set the value of the required methods to 1 (true).  Let
        # this indicate that the method was not defined in the
        # translator class.
        for method in required:
            required[method] = True

        # Loop through all items and compare the prototypes. Mark
        # the implemented method and collect the old ones.
        old_methods = []
        for method in finfo[2:]:
            # Get only the necessary form of the prototype.
            prototype = StripArgIdentifiers(method)
            
            # Mark as recognized when the prototype is required.
            # Otherwise, remember it as old method which is
            # implemented, but not required.
            if (required.has_key(prototype)): 
                required[prototype] = False       # satisfaction
            else:
                old_methods.append(method)
        
        # Loop through the list of expected methods and collect 
        # the missing (new) methods.  Do this only when it derives
        # from Translator or TranslatorAdapter classes (i.e. ignore
        # any unusual kind of TranslatorXxxx implementation). 
        # Accept also deriving from TranslatorEnglish, that can
        # be done by doxygen developers to solve problems with
        # some really outdated translators.
        missing_methods = []
        if rex_trAdapter.match(base_) or rex_trEN.match(base_):
            
            for method in expected_lst:
                # Get the stripped version of the prototype.
                prototype = StripArgIdentifiers(method) 
                
                # If the prototype is stored in the required
                # table, and if it was not marked as implemented,
                # then it should be.  It is a missing method.
                #try:
                if required[prototype]:
                    missing_methods.append(method)

        # The detailed output will be produced only when it is needed.
        if old_methods or missing_methods or rex_trAdapter.match(base_):
            output += '\n\n\n'
            output += '%s (%s)\n%s\n' % (class_, base_, '-' * len(class_))
            
            if rex_trEN.match(base_):
                output += '''
This translator is implemented via deriving from the English translator.
This should be done only in the case when the language maintainer
or the doxygen developers need to update some really old-dated translator.
Otherwise, deriving from the translator adapter classes should be used
for obsolete translators.  If you still want some texts to be in English
copy the sources of the English translator.

The obsolete and missing method lists (below) reflect what have to be done
to derive directly from the Translator class (i.e. to reach up-to-date status).
'''

            elif not rex_trAdapter.match(base_):
                output += '''
This is some unusual implementation of the translator class.  It is derived
from the %s base class. The usual translator class derives
or from the Translator class or from some TranslatorAdapter_x_x_x classes.
Because of that, nothing can be guessed about missing or obsolete methods.
''' % base_
                
            if missing_methods:
                output += '\nMissing methods (should be implemented):\n\n'
                for m in missing_methods:
                    output += '  ' + m + '\n'

            if old_methods:
                output += '\nObsolete methods (should be removed):\n\n'
                old_methods.sort()
                for m in old_methods:
                    output += '  ' + m + '\n'
                
            # Some details were listed, set the details flag for the class.
            details[class_] = 1;


    # Generate the ASCII output file.
    fout_name = info['ftranslatortxt']
    
    # Open it first, and output the version information.
    fout = file(fout_name, 'w')
    fout.write('(version %s)\n\n' % info['doxversion'])

    # List the supported languages.
    (numlang, langlst, langstr) = GetLanguagesInfo(cb)
    fout.write('Doxygen supports the following (' + str(numlang) +
               ') languages (sorted alphabetically):\n\n')
    fout.write(langstr + '.\n')
    
    # If there are up-to-date translators, list them.
    L = [k for k in cb if cb[k] == 'Translator']
    L.sort()
    
    if L:
        fout.write('\n' + '-' * 70 + '\n')
        fout.write('''The \
following translator classes are up-to-date (sorted alphabetically).
This means that they derive from the Translator class.  Anyway, there still
may be some details listed even for the up-to-date translators.
Please, check the text below if the translator is marked so.

''')
        for tr in L: 
            # Print the class name.  If some details were listed for 
            # the translator class, add a notice.
            fout.write('  ' + tr)
            if details[tr]:
                fout.write('\t-- see details below in the report') 
            fout.write('\n')
            
    # If there are obsolete translators, list them.
    L = [k for k in cb if rex_trAdapter.match(cb[k])]
    L.sort()

    if L:
        fout.write('\n' + '-' * 70 + '\n')
        fout.write('''The \
following translator classes are obsolete (sorted alphabetically).
This means that they derive from some of the adapter classes.

''')
        for tr in L:
            fout.write('  %s\t(%s)\n' % (tr, cb[tr]))        

    # If there are translators derived from TranslatorEnglish, list them
    # and name them as really obsolete.
    L = [k for k in cb if cb[k] == 'TranslatorEnglish']
    L.sort()
    if L:
        fout.write('\n' + '-' * 70 + '\n')
        fout.write('''The \
following translator classes are implemented via deriving
from the English translator.  This should be done only in the case
when the language maintainer or the doxygen developers need to update
some really outdated translator.  Otherwise, deriving from
the translator adapter classes should be prefered for obsolete translators.
See details below in the report.

''')
        for tr in L:
            fout.write('  %s\t(%s)\n' % (tr, cb[tr]))        

    # If there are other translators, list them. #{{{
    #
    L = [k for k in cb 
         if not rex_trAdapter.match(cb[k])
            and cb[k] != 'TranslatorEnglish'
            and cb[k] != 'Translator'
        ]
    L.sort()
    
    if L:
        fout.write('\n' + '-' * 70 + '\n')
        fout.write('''The \
following translator classes are somehow different
(sorted alphabetically).  This means that they do not derive from
the Translator class, nor from some of the adapter classes,
nor from the TranslatorEnglish.  Nothing can be guessed about the methods.

''')
        for tr in L:
            fout.write('  %s\t(%s)\n' % (tr, cb[tr]))        

    # List all the translator adapter classes to show for which versions
    # the adapters had to be created.  Show, how many and what new methods
    # are implemented by the adapters.
    #
    fout.write('\n' + '-' * 70 + '\n')
    fout.write('''The \
following translator adapter classes are implemented -- the older (with
lower number) are always derived from the newer. They implement the
listed required methods. Notice that some versions of doxygen did not
introduce any changes related to the language translators.  From here you may
guess how much work should be done to update your translator:

''')
    adapter_info = GetAdapterClassesInfo(required)
    
    for ad in adapter_info:
        fout.write('  %s\n' % ad)
    
    # List the methods that are expected to be implemented.
    fout.write('\n' + '-' * 70 + '\n')
    fout.write('''Localized \
translators are expected to implement the following methods
(prototypes sorted aplhabetically):

''')

    expected_lst.sort()
    for m in expected_lst:
        fout.write('%s\n' % m)

    # If there are some details for the translators, show them.
    if output != '':
        fout.write('\n\n' + '=' * 70 + '\n')
        fout.write('Details related to specific translator classes follow.\n')
        fout.write(output + '\n')

    # Close the ASCII output file
    fout.close()
        
    # Generate the same using the original perl script.
    os.system('translator.pl')    
        
    # Generate the language.doc file.  
    filename = os.path.join(info['docdir'], info['flangdoc'])
    f = file(filename, 'w')
    f.write(GenerateLanguageDoc(cb))
    f.close()

    sys.exit(0)
