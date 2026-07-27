%Doxygen Internals {#mainpage}
=================

[TOC]

Generated on \showdate "%A, %B %-d, %Y at %-I:%M %p"

Introduction
============

This page provides a high-level overview of the internals of doxygen, with
links to the relevant parts of the code. This document is intended for
developers who want to work on doxygen. Users of doxygen are referred to the
[User Manual](https://www.doxygen.nl/manual/index.html).

The generic starting point of the application is of course the main() function.

Configuration options
=====================

Configuration file data is stored in singleton class Config and can be
accessed using wrapper macros
Config_getString(), Config_getInt(), Config_getList(),
Config_getEnum(), and Config_getBool() depending on the type of the
option.

The format of the configuration file (options and types) is defined
by the file `config.xml`. As part of the build process,
the python script `configgen.py` will create the files `configoptions.cpp`, 
`configvalues.h` and `configvalues.cpp` from this, which serves as the input
for the configuration file parser that is invoked using Config::parse(). 
The script `configgen.py` will also create the documentation for the
configuration items, creating the file
`config.doc`.

Gathering Input files
=====================

After the configuration is known, the input files are searched using
searchInputFiles() and any tag files are read using readTagFile()

Parsing Input files
===================

The function parseFilesSingleThreading() takes care of parsing all files
(in case `NUM_PROC_THREADS!=1`, the function
parseFilesMultiThreading() is used instead).

These functions use the ParserManager singleton factory to create a suitable parser object
for each file. Each parser implements two abstract interfaces: OutlineParserInterface
en CodeParserInterface. The OutlineParserInterface is used to collect information
about the symbols that can be documented but does not look into the body of functions.
The CodeParserInterface is used for syntax highlighting, but also to collect the symbol
references needed for cross reference relations.

If the parser indicates it needs preprocessing
via OutlineParserInterface::needsPreprocessing(), doxygen will call Preprocessor::processFile()
on the file.

A second step is to convert multiline C++-style comments into C style comments
for easier processing later on. As side effect of this step also
aliases (ALIASES option) are resolved. The function that performs these
2 tasks is called convertCppComments().

*Note:* Alias resolution should better be done in a separate step as it is
now coupled to C/C++ code and does not work automatically for other languages!

The third step is the actual language parsing and is done by calling
OutlineParserInterface::parseInput() on the parser interface returned by
the ParserManager.

The result of parsing is a tree of Entry objects.
Each Entry object roughly contains the raw data for a symbol and is later
converted into a Definition object.

When a parser finds a special comment block in the input, it will do a first
pass parsing via CommentScanner::parseCommentBlock(). During this pass the comment block
is split into multiple parts if needed. Some data that is later needed is
extracted like section labels, xref items, and formulas.
Also Markdown markup is processed via Markdown::process() during this pass.

Resolving relations
===================

The Entry objects created and filled during parsing and stored as a tree of Entry nodes,
which is kept in memory.

%Doxygen does a number of tree walks over the Entry nodes in the tree to
build up the data structures needed to produce the output.

The resulting data structures are all children of the generic base class
called Definition which holds all non-specific data for a symbol definition.

Definition is an abstract base class. Concrete subclasses are
- ClassDef: for storing class/struct/union related data
- ConceptDef: for storing C++20 concept definitions
- NamespaceDef: for storing namespace related data
- FileDef: for storing file related data
- DirDef: for storing directory related data

For doxygen specific concepts the following subclasses are available
- GroupDef: for storing grouping related data
- PageDef: for storing page related data

Finally the data for members of classes, namespaces, and files is stored in
the subclass MemberDef. This class is used for functions, variables, enums, etc, as indicated by
MemberDef::memberType().

Producing tracing and debug output
==================================

Within doxygen there are a number of ways to obtain debug output. Besides the
invasive method of putting print statements in the code there are a number of
easier ways to get debug information.

For a debug build (build option `-DCMAKE_BUILD_TYPE=%Debug`)
these options are always available, but for a release build some debug capabilities have to be enabled explicitly
(see build options `-Denable_tracing=YES` and `-Denable_lex_debug=YES`).

To enable tracing use the `-t` option. You can optionally specify a name of a trace file,
if omitted `trace.txt` will be used.
When running doxygen with tracing enabled, doxygen will write a lot of internal information to the trace file,
which can be used (by experts) to diagnose problems.

During a run of doxygen it is possible to specify the `-d` command line option with one of the
following values (each option has to be preceded by `-d`):

  - `preprocessor`<br>
    Shows the results of the preprocessing phase, i.e. results from include files,
    <tt>\#define</tt> statements etc., definitions in the doxygen configuration file like:
    `EXPAND_ONLY_PREDEF`, `PREDEFINED` and `MACRO_EXPANSION`.
  - `nolineno`<br>
    In case the line numbers in the results of the preprocessing phase are not wanted they
    can be removed by means of this option (without `-d preprocessor` this option has no effect.
  - `commentcnv`<br>
    Shows the results of the comment conversion, the comment conversion does the
    following:
     - It converts multi-line C++ style comment blocks (that are aligned)
       to C style comment blocks (if `MULTILINE_CPP_IS_BRIEF` is set to `NO`).
     - It replaces aliases with their definition (see `ALIASES`)
     - It handles conditional sections (<tt>\\cond ... \\endcond</tt> blocks)
  - `commentscan`<br>
    Will print each comment block before and after the comment is interpreted by
    the comment scanner.
  - `printtree`<br>
    Give the results in pretty print way, i.e. in an XML like way with each
    level indented by a `"."` (dot).
  - `time`<br>
    Provides information of the different stages of the doxygen process.
  - `extcmd`<br>
    Shows which external commands are executed and which pipes are opened.
  - `markdown`<br>
    Will print each comment block before and after Markdown processing.
  - `filteroutput`<br>
    Gives the output of the output as result of the filter command (when a filter
    command is specified)
  - `cite`<br>
    Retains the temporary files as created and used for the non LaTeX output results of the 
    generation of the bibliographical references.
  - `fortranfixed2free`<br>
    Shows the result of the conversion of Fortran fixed formatted files to Fortran free formatted 
    files as done by doxygen.
  - `plantuml`<br>
    Shows information about the plantuml process run and the used input / output filenames, the content of the
    input file.
  - `rtf`<br>
    - Shows the original names and the anchors names (called bookmarks in RTF) where they are mapped to.
    - At the end of the generation of the RTF files these files are merged into one large RTF file, with
      this option the original files are retained.
  - `qhp`<br>
    - The qhp file is created with indentation for better readability (normally no indentation so the file is smaller).
    - When the setting `QHG_LOCATION` is pointing to the `qhelpgenerator` besides generating the `qch` file 
      also some extra checks are done by means of the `-c` flag of the `qhelpgenerator`.
  - `tag`<br>
    Shows the results of reading the tag files.
  - `alias`<br>
    Show the results of the `ALIASES` resolving process
  - `entries`<br>
    Dump the tree of Entries as found by doxygen
  - `formula`<br>
    Don't remove the intermediate files generated by the creation of the images with the formulas.
  - `sections`<br>
    Show the sections as found by doxygen
  - `lex`<br>
    Provide output of the `lex` files used. When a lexer is started and when a lexer
    ends the name of the `lex` file is given so it is possible to see in which lexer the
    problem occurs. This makes it easier to select the file to be compiled in `lex` debug mode.
  - `lex:lexer`<br>
    Enables output for a specific lexer only, where `lexer` should be replaced by the name of the specific lexer.

Producing output
================

TODO

Documentation Topics TODO
===========
- Grouping of files in Model / Parser / Generator categories
- Index files based on IndexIntf
  - HTML navigation
  - HTML Help (chm)
  - Documentation Sets (XCode)
  - Qt Help (qhp)
  - Eclipse Help
- Search index
  - JavaScript based
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
- Output formats: OutputGenerator, OutputList, and DocVisitor
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
  - Tag files
- Marshaling to/from disk
- Portability functions
- Utility functions

