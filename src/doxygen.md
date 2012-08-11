Doxygen Internals {#mainpage}
=================

Introduction
------------

This page provides a high-level overview of the internals of doxygen, with
links to the relevant parts of the code. This document is intended for
developers who want to work on doxygen. Users of doxygen are refered to the
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
that is invoked using Config::parse()

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

Finally the data for members of classes, namespaces, and files is stored is
the subclass MemberDef.

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
  - doc tokeninzer
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

