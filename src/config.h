/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-1999 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * All output generated with Doxygen is not covered by this license.
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

struct Config
{
  static void init();

  static QCString projectName;        // the name of the project               
  static QCString projectNumber;      // the number of the project
  static QCString outputDir;          // the global output directory
  static QCString htmlOutputDir;      // the directory to put the HTML files  
  static QCString latexOutputDir;     // the directory to put the Latex files  
  static QCString manOutputDir;       // the directory to put the man pages
  static QCString outputLanguage;     // the output language
  static QCString headerFile;         // the name of the personal HTML header 
  static QCString footerFile;         // the name of the personal HTML footer 
  static QCString cgiName;            // the name of the CGI binary           
  static QCString cgiURL;             // the absolute URL to the CGI binary   
  static QCString docURL;             // the absolute URL to the documentation
  static QCString binAbsPath;         // the absolute path to the doxysearch  
  static QCString docAbsPath;         // the absolute path to the documentation
  static QCString perlPath;           // the absolute path to perl            
  static QCString genTagFile;         // the tag file to generate
  static QCString inputFilter;        // a filter command that is applied to input files
  static QCString paperType;          // the page type to generate docs for
  static QCString manExtension;       // extension the man page files
  static QStrList includePath;        // list of include paths                
  static QStrList examplePath;        // list of example paths                
  static QStrList inputSources;       // list of input files                  
  static QStrList excludeSources;     // list of files to exclude from the input
  static QStrList filePatternList;    // list of file patterns                
  static QStrList excludePatternList; // list of patterns to exclude from input
  static QStrList tagFileList;        // list of tag files                    
  static QStrList extDocPathList;     // list of external doc. directories.   
  static QStrList predefined;         // list of predefined macro names.
  static QStrList extraPackageList;   // list of extra LaTeX packages.
  static QStrList stripFromPath;      // list of candidates to strip from the file path
  static bool     quietFlag;          // generate progress messages flag      
  static bool     warningFlag;        // generate warnings flag               
  static bool     recursiveFlag;      // scan directories recursively         
  static bool     allExtFlag;         // include all external classes flag    
  static bool     searchEngineFlag;   // generate search engine flag          
  static bool     extractAllFlag;     // gererate docs for all classes flag   
  static bool     extractPrivateFlag; // generate docs for private members flag
  static bool     noIndexFlag;        // generate condensed index flag
  static bool     generateHtml;       // generate HTML output
  static bool     generateLatex;      // generate Latex output
  static bool     generateMan;        // generate Man pages
  static bool     preprocessingFlag;  // enable preprocessing 
  static bool     briefMemDescFlag;   // enable `inline' brief member descr.
  static bool     hideMemberFlag;     // hide undocumented members.
  static bool     hideClassFlag;      // hide undocumented members.
  static bool     searchIncludeFlag;  // search for included files
  static bool     macroExpansionFlag; // expand macros in the source.
  static bool     onlyPredefinedFlag; // expand only predefined macros
  static bool     fullPathNameFlag;   // using full path name in output
  static bool     classDiagramFlag;   // enable the generation of class diagrams.
  static bool     compactLatexFlag;   // generate compact LaTeX documentation.
  static bool     repeatBriefFlag;    // repeat brief descriptions.
  static bool     internalDocsFlag;   // determines what happens to internal docs.
  static bool     caseSensitiveNames; // determines if output can be mixed case.
  static bool     verbatimHeaderFlag; // enable/disable generation of verb headers.
  static bool     htmlAlignMemberFlag; // align members in HTML using tables.
  static bool     includeSourceFlag;  // include source code in documentation.  
  static bool     autoBriefFlag;      // javadoc comments behaves as Qt comments.
  static bool     htmlHelpFlag;       // should html help files be generated?
  static bool     alphaIndexFlag;     // should an alphabetical index be generated?
};

#endif
