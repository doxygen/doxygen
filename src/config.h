/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#ifndef CONFIG_H
#define CONFIG_H

#ifndef DOXYWIZARD
#include "qtbc.h"
#endif
#include <qstrlist.h>
#include <qfile.h>

extern void parseConfig(const QCString &config);
extern void writeTemplateConfig(QFile *f,bool shortList);
extern void checkConfig();
extern void configStrToVal();
extern void substituteEnvironmentVars();

struct Config
{
  static void init();

  static QCString projectName; // the name of the project
  static QCString projectNumber; // the number of the project
  static QCString outputDir; // the global output directory
  static QCString outputLanguage; // the output language
  static bool     quietFlag; // generate progress messages flag
  static bool     warningFlag; // generate warnings flag
  static bool     noIndexFlag; // generate condensed index flag
  static bool     extractAllFlag; // gererate docs for all classes flag
  static bool     extractPrivateFlag; // generate docs for private members flag
  static bool     hideMemberFlag; // hide undocumented members.
  static bool     hideClassFlag; // hide undocumented members.
  static bool     briefMemDescFlag; // enable `inline' brief member descr.
  static bool     repeatBriefFlag; // repeat brief descriptions.
  static bool     alwaysDetailsFlag; // show details description even if there is only a brief description?
  static bool     fullPathNameFlag; // using full path name in output
  static QStrList stripFromPath; // list of candidates to strip from the file path
  static bool     internalDocsFlag; // determines what happens to internal docs.
  static bool     classDiagramFlag; // enable the generation of class diagrams.
  static bool     sourceBrowseFlag; // include source code in documentation.
  static bool     inlineSourceFlag; // inline the definition bodies in the docs?
  static bool     stripCommentsFlag; // strip special comments from code fragments?
  static bool     caseSensitiveNames; // determines if output can be mixed case.
  static bool     verbatimHeaderFlag; // enable/disable generation of verb headers.
  static bool     showIncFileFlag; // show include file in file documentation?
  static bool     autoBriefFlag; // javadoc comments behaves as Qt comments.
  static bool     inheritDocsFlag; // inheritance of documentation enabled?
  static bool     inlineInfoFlag; // show info about inline members?
  static bool     sortMembersFlag; // sort members alphabetically?
  static int      tabSize; // number of spaces in a tab
  static QStrList sectionFilterList; // list of section filters that are enabled
  static QStrList inputSources; // list of input files
  static QStrList filePatternList; // list of file patterns
  static bool     recursiveFlag; // scan directories recursively
  static QStrList excludeSources; // list of files to exclude from the input
  static QStrList excludePatternList; // list of patterns to exclude from input
  static QStrList examplePath; // list of example paths
  static QStrList examplePatternList; // list of example patterns
  static QStrList imagePath; // list of image paths
  static QCString inputFilter; // a filter command that is applied to input files
  static bool     alphaIndexFlag; // should an alphabetical index be generated?
  static int      colsInAlphaIndex; // number of columns in the alphabetical index
  static QStrList ignorePrefixList; // list of prefixes to ignore for the alphabetical index
  static bool     generateHtml; // generate HTML output
  static QCString htmlOutputDir; // the directory to put the HTML files
  static QCString headerFile; // the name of the personal HTML header
  static QCString footerFile; // the name of the personal HTML footer
  static QCString htmlStyleSheet; // user defined cascading style sheet
  static bool     htmlAlignMemberFlag; // align members in HTML using tables.
  static bool     htmlHelpFlag; // should html help files be generated?
  static bool     generateLatex; // generate Latex output
  static QCString latexOutputDir; // the directory to put the Latex files
  static bool     compactLatexFlag; // generate compact LaTeX documentation.
  static QCString paperType; // the page type to generate docs for
  static QStrList extraPackageList; // list of extra LaTeX packages.
  static QCString latexHeaderFile; // the name of the personal LaTeX header
  static bool     pdfHyperFlag; // generate latex prepared creating hyperlinked pdfs.
  static bool     latexBatchModeFlag; // continue after latex errors?
  static bool     generateRTF; // generate RTF flag
  static QCString rtfOutputDir; // the directory to put the RTF files
  static bool     compactRTFFlag; // generate more compact RTF
  static bool     rtfHyperFlag; // generate hyper links in RTF
  static bool     generateMan; // generate Man pages
  static QCString manOutputDir; // the directory to put the man pages
  static QCString manExtension; // extension the man page files
  static bool     preprocessingFlag; // enable preprocessing
  static bool     macroExpansionFlag; // expand macros in the source.
  static bool     searchIncludeFlag; // search for included files
  static QStrList includePath; // list of include paths
  static QStrList predefined; // list of predefined macro names.
  static bool     onlyPredefinedFlag; // expand only predefined macros
  static QStrList tagFileList; // list of tag files
  static QCString genTagFile; // the tag file to generate
  static bool     allExtFlag; // include all external classes flag
  static QCString perlPath; // the absolute path to perl
  static bool     haveDotFlag; // indicates wether or not dot is present
  static bool     classGraphFlag; // class graph
  static bool     collGraphFlag; // collaboration graph
  static bool     includeGraphFlag; // include graph
  static bool     gfxHierarchyFlag; // flag to enable graphical hierarchy
  static bool     searchEngineFlag; // generate search engine flag
  static QCString cgiName; // the name of the CGI binary
  static QCString cgiURL; // the absolute URL to the CGI binary
  static QCString docURL; // the absolute URL to the documentation
  static QCString docAbsPath; // the absolute path to the documentation
  static QCString binAbsPath; // the absolute path to the doxysearch
  static QStrList extDocPathList; // list of external doc. directories.
};

#endif
