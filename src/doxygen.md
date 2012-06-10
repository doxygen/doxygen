Doxygen Internals {#mainpage}
=================

Introduction
------------

This page provides a high-level overview of the internals of doxygen, with
links to the relevant parts of the code.

The generic starting point of the application is ofcource the main() function.

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
on the file before calling ParserInterface::parseInput().

The result of parsing is a tree of Entry objects 
These Entry objects are wrapped in a EntryNav object and stored on disk using
Entry::createNavigationIndex() on the root node of the tree.

Each Entry object roughly contains the raw data for a symbol and is later
converted into a Definition object.

When a parser finds a special comment block in the input, it will do a first
pass parsing via parseCommentBlock(). During this pass the comment block
is split into multiple parts if needed. Some data that is later needed is
extracted like section labels, xref items, and formulas. 
Also Markdown markup is processed using processMarkdown() during this pass.




