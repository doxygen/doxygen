Doxygen Internals {#mainpage}
=================

Introduction
------------

This page provides a high-level overview of the internals of doxygen, with
links to the relevant parts of the code. This document is intended for
developers who want to work on doxygen. Users of doxygen are referred to the
[User Manual](http://www.doxygen.org/manual.html).

The generic starting point of the application is of cource the main() function.

Configuration options
---------------------

Configuration file data is stored in singleton class Config and can be
accessed using wrapper macros 
Config_getString(), Config_getInt(), Config_getList(),
Config_getEnum(), and Config_getBool() depending on the type of the
option. 

The format of the configuration file (options and types) is defined
by the file `config.xml`. As part of the build process, 
the python script `configgen.py` will create a file configoptions.cpp 
from this, which serves as the input for the configuration file parser
that is invoked using Config::parse(). The script `configgen.py` will also
create the documentation for the configuration items, creating the file
`config.doc`.

Gathering Input files
---------------------

After the configuration is known, the input files are searched using
searchInputFiles() and any tag files are read using readTagFile()

Parsing Input files
-------------------

The function parseFiles() takes care of parsing all files.
It uses the ParserManager singleton factory to create a suitable parser object
for each file. Each parser implements the abstract interface ParserInterface.

If the parser indicates it needs preprocessing
via ParserInterface::needsPreprocessing(), doxygen will call preprocessFile()
on the file. 

A second step is to convert multiline C++-style comments into C style comments
for easier processing later on. As side effect of this step also 
aliases (ALIASES option) are resolved. The function that performs these 
2 tasks is called convertCppComments().

*Note:* Alias resolution should better be done in a separate step as it is
now coupled to C/C++ code and does not work automatically for other languages!

The third step is the actual language parsing and is done by calling 
ParserInterface::parseInput() on the parser interface returned by 
the ParserManager.

The result of parsing is a tree of Entry objects.
These Entry objects are wrapped in a EntryNav object and stored on disk using
Entry::createNavigationIndex() on the root node of the tree.

Each Entry object roughly contains the raw data for a symbol and is later
converted into a Definition object.

When a parser finds a special comment block in the input, it will do a first
pass parsing via parseCommentBlock(). During this pass the comment block
is split into multiple parts if needed. Some data that is later needed is
extracted like section labels, xref items, and formulas. 
Also Markdown markup is processed using processMarkdown() during this pass.

Resolving relations
-------------------

The Entry objects created and filled during parsing are stored on disk 
(to keep memory needs low). The name, parent/child relation, and 
location on disk of each Entry is stored as a tree of EntryNav nodes, which is 
kept in memory.

Doxygen does a number of tree walks over the EntryNav nodes in the tree to
build up the data structures needed to produce the output. 

The resulting data structures are all children of the generic base class
called Definition which holds all non-specific data for a symbol definition.

Definition is an abstract base class. Concrete subclasses are
- ClassDef: for storing class/struct/union related data
- NamespaceDef: for storing namespace related data
- FileDef: for storing file related data
- DirDef: for storing directory related data

For doxygen specific concepts the following subclasses are available
- GroupDef: for storing grouping related data
- PageDef: for storing page related data

Finally the data for members of classes, namespaces, and files is stored in
the subclass MemberDef.

Producing debug output
----------------------

Within doxygen there are a number of ways to obtain debug output. Besides the
invasive method of  putting print statements in the code there are a number of
easy ways to get debug information.

- Compilation of `.l` files<br>
  This is also an invasive method but it will be automatically done by the
  `flex / lex` command. The result is that of each input line the (lex) rule(s)
  that are applied on it are shown.
  - windows
    - in the Visual C++ GUI
      - find the required `.l` file
      - select the `Properties` of this file
      - set the item `Write used lex rules` to `Yes`
      - see to it that the `.l` file is newer than the corresponding `.cpp` file
        or remove the corresponding `.cpp` file
  - unices
    - global change<br>
      In the chapter "Doxygen's internals" a `perl` script is given to toggle the
      possibility of having the rules debug information.
    - command line change<br>
      It is possible to the option `LEX="flex -d"` with the `make` command on the
      command line. In this case the `.l` that are converted to the corresponding
      `.cpp` files during this `make` get the rules debug information.<br>
      To undo the rules debug information output just recompile the file with
      just `make`.<br>
      Note this method applies for all the `.l` files that are rebuild to `.cpp`
      files so be sure that only the `.l` files(s) of which you want to have the
      rules debug information is (are) newer than the corresponding `.cpp`
      file(s).
- Running doxygen<br>
  During a run of doxygen it is possible to specify the `-d` option with the
  following possibilities (each option has to be preceded by `-d`):
  - findmembers<br>
    Gives of global, class, module members its scope, arguments and other relevant information.
  - functions<br>
    Gives of functions its scope, arguments and other relevant information.
  - variables<br>
    Gives of variables its scope and other relevant information.
  - classes<br>
    Gives of classes en modules its scope and other relevant information.
  - preprocessor<br>
    Shows the results of the preprocessing phase, i.e. results from include files, 
    <tt>\#define</tt> statements etc., definitions in the doxygen configuration file like:
    `EXPAND_ONLY_PREDEF`, `PREDEFINED` and `MACRO_EXPANSION`. 
  - commentcnv<br>
    Shows the results of the comment conversion, the comment conversion does the
    following:
     - It converts multi-line C++ style comment blocks (that are aligned)
       to C style comment blocks (if `MULTILINE_CPP_IS_BRIEF` is set to `NO`).
     - It replaces aliases with their definition (see `ALIASES`)
     - It handles conditional sections (<tt>\\cond ... \\endcond</tt> blocks)
  - commentscan<br>
    Will print each comment block before and after the comment is interpreted by
    the comment scanner.
  - printtree<br>
    Give the results in in pretty print way, i.e. in an XML like way with each
    level indented by a `"."` (dot).
  - time<br>
    Provides information of the different stages of the doxygen process.
  - extcmd<br>
    Shows which external commands are executed and which pipes are opened.
  - markdown<br>
    Will print each comment block before and after Markdown processing.
  - filteroutput<br>
    Gives the output of the output as result of the filter command (when a filter
    command is specified)
  - validate<br>
    Currently not used
  - lex<br>
    Provide output of the `lex` files used. When a lexer is started and when a lexer
    ends the name of the `lex` file is given so it is possible to see in which lexer the
    problem occurs. This makes it easier to select the file to be compiled in `lex` debug mode.

Producing output
----------------

TODO

Topics TODO
-----------
- Grouping of files in Model / Parser / Generator categories
- Index files based on IndexIntf
  - HTML navigation
  - HTML Help (chm)
  - Documentation Sets (XCode)
  - Qt Help (qhp)
  - Eclipse Help
- Search index
  - Javascript based
  - Server based
  - External
- Citations
  - via bibtex
- Various processing steps for a comment block
  - comment conversion
  - comment scanner
  - markdown processor
  - doc tokenizer
  - doc parser
  - doc visitors
- Diagrams and Images
  - builtin
  - via Graphviz dot
  - via mscgen
  - PNG generation
- Output formats: OutputGen, OutputList, and DocVisitor
  - Html:  HtmlGenerator and HtmlDocVisitor
  - Latex: LatexGenerator and LatexDocVisitor
  - RTF:   RTFGenerator and RTFDocVisitor
  - Man:   ManGenerator and ManDocVisitor
  - XML:   generateXML() and XmlDocVisitor
  - print: debugging via PrintDocVisitor
  - text:  TextDocVisitor for tooltips
  - perlmod
- i18n via Translator and language.cpp
- Customizing the layout via LayoutDocManager
- Parsers 
  - C Preprocessing 
    - const expression evaluation
  - C link languages
  - Python
  - Fortran
  - VHDL
  - TCL
  - Tag files
- Marshaling to/from disk
- Portability functions
- Utility functions

