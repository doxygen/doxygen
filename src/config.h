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

#include <qstring.h>
#include <qstrlist.h>
#include <qfile.h>

extern void parseConfig(const QString &config);
extern void writeTemplateConfig(QFile *f,bool shortList);

extern QString  projectName;        // the name of the project               
extern QString  projectNumber;      // the number of the project
extern QString  htmlOutputDir;      // the directory to put the HTML files  
extern QString  latexOutputDir;     // the directory to put the Latex files  
extern QString  manOutputDir;       // the directory to put the man pages
extern QString  headerFile;         // the name of the personal HTML header 
extern QString  footerFile;         // the name of the personal HTML footer 
extern QString  cgiName;            // the name of the CGI binary           
extern QString  cgiURL;             // the absolute URL to the CGI binary   
extern QString  docURL;             // the absolute URL to the documentation
extern QString  binAbsPath;         // the absolute path to the doxysearch  
extern QString  docAbsPath;         // the absolute path to the documentation
extern QString  perlPath;           // the absolute path to perl            
extern QString  genTagFile;         // the tag file to generate
extern QString  inputFilter;        // a filter command that is applied to input files
extern QString  paperType;          // the page type to generate docs for
extern QString  stripFromPath;      // the string to strip from the file path
extern QString  manExtension;       // extension the man page files
extern QStrList includePath;        // list of include paths                
extern QStrList examplePath;        // list of example paths                
extern QStrList inputSources;       // list of input files                  
extern QStrList excludeSources;     // list of files to exclude from the input
extern QStrList filePatternList;    // list of file patterns                
extern QStrList excludePatternList; // list of patterns to exclude from input
extern QStrList tagFileList;        // list of tag files                    
extern QStrList extDocPathList;     // list of external doc. directories.   
extern QStrList predefined;         // list of predefined macro names.
extern QStrList extraPackageList;   // list of extra LaTeX packages.
extern bool     quietFlag;          // generate progress messages flag      
extern bool     warningFlag;        // generate warnings flag               
extern bool     recursiveFlag;      // scan directories recursively         
extern bool     allExtFlag;         // include all external classes flag    
extern bool     searchEngineFlag;   // generate search engine flag          
extern bool     extractAllFlag;     // gererate docs for all classes flag   
extern bool     extractPrivateFlag; // generate docs for private members flag
extern bool     noIndexFlag;        // generate condensed index flag
extern bool     generateHtml;       // generate HTML output
extern bool     generateLatex;      // generate Latex output
extern bool     generateMan;        // generate Man pages
extern bool     preprocessingFlag;  // enable preprocessing 
extern bool     briefMemDescFlag;   // enable `inline' brief member descr.
extern bool     hideMemberFlag;     // hide undocumented members.
extern bool     hideClassFlag;      // hide undocumented members.
extern bool     searchIncludeFlag;  // search for included files
extern bool     macroExpansionFlag; // expand macros in the source.
extern bool     onlyPredefinedFlag; // expand only predefined macros
extern bool     fullPathNameFlag;   // using full path name in output
extern bool     classDiagramFlag;   // enable the generation of class diagrams.
extern bool     compactLatexFlag;   // generate compact LaTeX documentation.
extern bool     repeatBriefFlag;    // repeat brief descriptions.
extern bool     internalDocsFlag;   // determines what happens to internal docs.
extern bool     caseSensitiveNames; // determines if output can be mixed case.
extern bool     verbatimHeaderFlag; // enable/disable generation of verb headers.
extern bool     htmlAlignMemberFlag; // align members in HTML using tables.

#endif
