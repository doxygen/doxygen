/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * Documents produced by Doxygen are derivative works derived from the
 * input used in their production; they are not affected by this license.
 *
 */

#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include "qtbc.h"
#include "classdef.h"
#include "util.h"
#include "config.h"

class Translator
{
  protected:
    static const char WinToISOTab[];
    static const char ISOToWinTab[];
    static Q_UINT16 koi8_r[128];
    static Q_UINT16 windows_1251[128];
    /*! Returns the string converted from windows-1250 to iso-8859-2. */
    /* The method was designed initially for translator_cz.h. 
     * It is used for on-line encoding conversion related to
     * conditional compilation in Unix/MS Windows environments
     * (both use different encoding).  Later, the translator_hr.h
     * (by Boris Bralo) used and improved the same style. As the
     * method with the translation table was the same, the
     * decision to move it to this base class was made. The same
     * holds for ISO88592ToWin1250() method. 
     * 
     * Alexandr Chelpanov used the same approach for
     * Koi8RToWindows1251() and Windows1251ToKoi8R() methods.  Notice,
     * that he uses Unicode tables.
     * 
     * It is recommended for possibly other similar methods in future.
     */
    QCString Win1250ToISO88592(const QCString & sInput)
    {
      // The conversion table for characters >127
      // 
      
      QCString result;
      int len = sInput.length();

      for (int i = 0; i < len; ++i)
      {
        unsigned int c = sInput[i];  
        result += (c > 127) ? WinToISOTab[c & 0x7F] : c;
      }
      return result;
    }
    
    /*! returns the string converted from iso-8859-2 to windows-1250 */
    /* See the comments of the Win1250ToISO88592() method for details. */
    QCString ISO88592ToWin1250(const QCString & sInput)
    {
      // The conversion table for characters >127
      // 
      QCString result;
      int len = sInput.length();

      for (int i = 0; i < len; ++i)
      {
        unsigned int c = sInput[i];  
        result += (c > 127) ? ISOToWinTab[c & 0x7F] : c;
      }
      return result;
    }
    
    /*! Returns the string converted from koi8-r to windows-1251. */
    /* The method was designed initially for translator_cz.h. 
       It is used for on-line encoding conversion related to conditional
       compilation in Unix/MS Windows environments (both use different
       encoding). 
       Encoding table got from QT:qtextcodec.cpp
     */
    QCString Koi8RToWindows1251( const QCString & sInput )
    {

      QString result;
      int len = sInput.length();

      result.setUnicode(0, len);
      QChar* uc = (QChar*)result.unicode(); // const_cast
      const unsigned char * c = (const unsigned char *)(const char*)sInput;
      for( int i=0; i<len; i++ ) {
        if ( c[i] > 127 )
          uc[i] = koi8_r[c[i]-128];
        else
          uc[i] = c[i];
      }
      return result.local8Bit();
    }
    /*! returns the string converted from Windows-1251 to koi8-r */
    /* See the comments of the Koi8RToWindows1251() method for details.
       Encoding table got from QT:qtextcodec.cpp */
    QCString Windows1251ToKoi8R( const QCString & sInput )
    {
      QString result;
      int len = sInput.length();

      result.setUnicode(0, len);
      QChar* uc = (QChar*)result.unicode(); // const_cast
      const unsigned char * c = (const unsigned char *)(const char*)sInput;
      for( int i=0; i<len; i++ ) {
        if ( c[i] > 127 )
          uc[i] = windows_1251[c[i]-128];
        else
          uc[i] = c[i];
      }
      return result.local8Bit();
    }

  public:

    /*! This method is used to provide warning message that is displayed
     *  if the user chooses a language whose translation is not up to date. 
     *  It is implemented by the adapter classes.
     */
    virtual QCString updateNeededMessage() { return ""; }
   

    // --- Language control methods -------------------
    
    /*! Used for identification of the language. May resemble 
     * the string returned by latexBabelPackage(), but it is not used
     * for the same purpose. The identification should not be translated.
     * It should be replaced by the name of the language in English
     * (e.g. Czech, Japanese, Russian, etc.). It should be equal to 
     * the identification in language.cpp.
     */
    virtual QCString idLanguage() = 0;

    /*! Used to get the command(s) for the language support. This method
     *  was designed for languages which do not prefer babel package.
     *  If this methods returns empty string, then the latexBabelPackage()
     *  method is used to generate the command for using the babel package.
     */
    virtual QCString latexLanguageSupportCommand() = 0;

    /*! return the language charset. This will be used for the HTML output */
    virtual QCString idLanguageCharset() = 0;

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions() = 0;

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript() = 0;

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription() = 0;

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation() = 0;
    
    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation() = 0;
    
    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation() = 0;
    
    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation() = 0;

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore() = 0;

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers() = 0;

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList() = 0;

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers() = 0;

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers() = 0;
    
    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const char *s) = 0;


    virtual QCString trEnumName() = 0;
    
    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue() = 0;
    
    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn() = 0;

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of 
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules() = 0;
    
    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy() = 0;
    
    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList() = 0;
    
    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList() = 0;

    /*! This is put above each page as a link to the list of all verbatim headers */
    virtual QCString trHeaderFiles() = 0;

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers() = 0;

    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers() = 0;

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages() = 0;

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples() = 0;

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch() = 0;

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription() = 0;

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll) = 0;

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription() = 0;

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll) = 0;

    /*! This is an introduction to the page with all file members. */
    virtual QCString trFileMembersDescription(bool extractAll) = 0;

    /*! This is an introduction to the page with the list of all header files. */
    virtual QCString trHeaderFilesDescription() = 0;

    /*! This is an introduction to the page with the list of all examples */
    virtual QCString trExamplesDescription() = 0;

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription() = 0;

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription() = 0;

    /*! This sentences is used in the annotated class/file lists if no brief
     * description is given. 
     */
    virtual QCString trNoDescriptionAvailable() = 0;
    
    // index titles (the project name is prepended for these) 


    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation() = 0;

    /*! This is used in LaTeX as the title of the chapter with the 
     * index of all groups.
     */
    virtual QCString trModuleIndex() = 0;

    /*! This is used in LaTeX as the title of the chapter with the 
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex() = 0;

    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index.
     */
    virtual QCString trCompoundIndex() = 0;

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    virtual QCString trFileIndex() = 0; 

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation() = 0;

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation() = 0;

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation() = 0;

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    virtual QCString trExampleDocumentation() = 0;

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    virtual QCString trPageDocumentation() = 0;

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual() = 0;
    
    /*! This is used in the documentation of a file as a header before the 
     *  list of defines
     */
    virtual QCString trDefines() = 0;

    /*! This is used in the documentation of a file as a header before the 
     *  list of function prototypes
     */
    virtual QCString trFuncProtos() = 0;

    /*! This is used in the documentation of a file as a header before the 
     *  list of typedefs
     */
    virtual QCString trTypedefs() = 0;

    /*! This is used in the documentation of a file as a header before the 
     *  list of enumerations
     */
    virtual QCString trEnumerations() = 0;

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) functions
     */
    virtual QCString trFunctions() = 0;

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trVariables() = 0;

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues() = 0;
    
    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation() = 0;

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for function prototypes
     */
    virtual QCString trFunctionPrototypeDocumentation() = 0;

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation() = 0;

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation() = 0;

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation() = 0;

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation() = 0;

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation() = 0;

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds() = 0;

    /*! This is used in the standard footer of each page and indicates when 
     *  the page was generated 
     */
    virtual QCString trGeneratedAt(const char *date,const char *projName) = 0;

    /*! This is part of the sentence used in the standard footer of each page.
     */
    virtual QCString trWrittenBy() = 0;

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const char *clName) = 0;
    
    /*! this text is generated when the \\internal command is used. */
    virtual QCString trForInternalUseOnly() = 0;

    /*! this text is generated when the \\reimp command is used. */
    virtual QCString trReimplementedForInternalReasons() = 0;

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning() = 0;

    /*! this text is generated when the \\bug command is used. */
    virtual QCString trBugsAndLimitations() = 0;

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion() = 0;

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate() = 0;

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns() = 0;

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso() = 0;

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters() = 0;

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions() = 0;
    
    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////
    
    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList() = 0;

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll) = 0;

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends() = 0;
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
    /*! used in the class documentation as a header before the list of all
     * related classes 
     */
    virtual QCString trRelatedFunctionDocumentation() = 0;
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) = 0;

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const char *fileName) = 0;

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const char *namespaceName) = 0;
    
    virtual QCString trPublicMembers() = 0;
    virtual QCString trPublicSlots() = 0;
    virtual QCString trSignals() = 0;
    virtual QCString trStaticPublicMembers() = 0;
    virtual QCString trProtectedMembers() = 0;
    virtual QCString trProtectedSlots() = 0;
    virtual QCString trStaticProtectedMembers() = 0;
    virtual QCString trPrivateMembers() = 0;
    virtual QCString trPrivateSlots() = 0;
    virtual QCString trStaticPrivateMembers() = 0;
    
    /*! this function is used to produce a comma-separated list of items.
     *  Use generateMarker(i) to indicate where item i should be put.
     */
    virtual QCString trWriteList(int numEntries) = 0;
    
    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries) = 0;

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries) = 0;

    /*! used in member documentation blocks to produce a list of 
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries) = 0;

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries) = 0;

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers() = 0;

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll) = 0;

    /*! This is used in LaTeX as the title of the chapter with the 
     *  index of all namespaces.
     */
    virtual QCString trNamespaceIndex() = 0;

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single) = 0;

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    virtual QCString trAlphabeticalList() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues() = 0;

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage() = 0;

    /*! This is used in references to page that are put in the LaTeX 
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual QCString trSources() = 0;

    virtual QCString trDefinedAtLineInSourceFile() = 0;

    virtual QCString trDefinedInSourceFile() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const char *clName) = 0;

    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName) = 0;

    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation() = 0;

    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode() = 0;

    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation() = 0;

    /*! Text for the \\pre command */
    virtual QCString trPrecondition() = 0;

    /*! Text for the \\post command */
    virtual QCString trPostcondition() = 0;

    /*! Text for the \\invariant command */
    virtual QCString trInvariant() = 0;

    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue() = 0;

    /*! Text used the source code in the file index */
    virtual QCString trCode() = 0;

    virtual QCString trGraphicalHierarchy() = 0;
    virtual QCString trGotoGraphicalHierarchy() = 0;
    virtual QCString trGotoTextualHierarchy() = 0;
    virtual QCString trPageIndex() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////
    
    virtual QCString trNote() = 0;
    virtual QCString trPublicTypes() = 0;
    virtual QCString trPublicAttribs() = 0;
    virtual QCString trStaticPublicAttribs() = 0;
    virtual QCString trProtectedTypes() = 0;
    virtual QCString trProtectedAttribs() = 0;
    virtual QCString trStaticProtectedAttribs() = 0;
    virtual QCString trPrivateTypes() = 0;
    virtual QCString trPrivateAttribs() = 0;
    virtual QCString trStaticPrivateAttribs() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    virtual QCString trTodo() = 0;

    /*! Used as the header of the todo list */
    virtual QCString trTodoList() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy() = 0;
    virtual QCString trRemarks() = 0;
    virtual QCString trAttention() = 0;
    virtual QCString trInclByDepGraph() = 0;
    virtual QCString trSince() = 0;
    
//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle() = 0;

    /*! page explaining how the dot graph's should be interpreted */
    virtual QCString trLegendDocs() = 0;

    /*! text for the link to the legend page */
    virtual QCString trLegend() = 0;
    
//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////
    
    /*! Used as a marker that is put before a test item */
    virtual QCString trTest() = 0;

    /*! Used as the header of the test list */
    virtual QCString trTestList() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for KDE-2 IDL methods */
    virtual QCString trDCOPMethods() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties() = 0;

    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java interfaces in the summary section of Java packages */
    virtual QCString trInterfaces() = 0;

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses() = 0;

    /*! Used as the title of a Java package */
    virtual QCString trPackage(const char *name) = 0;

    /*! Title of the package index page */
    virtual QCString trPackageList() = 0;

    /*! The description of the package index page */
    virtual QCString trPackageListDescription() = 0;

    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages() = 0;

    /*! Used as a chapter title for Latex & RTF output */
    virtual QCString trPackageDocumentation() = 0;

    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue() = 0;
    
//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////
    
    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug() = 0;

    /*! Used as the header of the bug list */
    virtual QCString trBugList() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.2.6
//////////////////////////////////////////////////////////////////////////

    /*! Used as ansicpg for RTF file */
    virtual QCString trRTFansicp() = 0;
    
    /*! Used as ansicpg for RTF fcharset */
    virtual QCString trRTFCharSet() = 0;

    /*! Used as header RTF general index */
    virtual QCString trRTFGeneralIndex() = 0;
   
    /*! The following are used for translation of the word that will
     *  possibly be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular) = 0;
    virtual QCString trFile(bool first_capital, bool singular) = 0;
    virtual QCString trNamespace(bool first_capital, bool singular) = 0;
    virtual QCString trGroup(bool first_capital, bool singular) = 0;
    virtual QCString trPage(bool first_capital, bool singular) = 0;
    virtual QCString trMember(bool first_capital, bool singular) = 0;
    virtual QCString trField(bool first_capital, bool singular) = 0;
    virtual QCString trGlobal(bool first_capital, bool singular) = 0;

    
    //////////////////////////////////////////////////////////////////////////
    // new since 1.2.7
    //////////////////////////////////////////////////////////////////////////
        
    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. 
     */
    virtual QCString trAuthor(bool first_capital, bool singular) = 0;

};

#endif
