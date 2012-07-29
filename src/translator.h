/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2012 by Dimitri van Heesch.
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

/** Abstract base class for all translatable text fragments. */
class Translator
{
  private:
    /* Tables for encoding conversions. */
    static const char Win1250ToISO88592Tab[];
    static const char ISO88592ToWin1250Tab[];
    static const unsigned char Koi8RToWindows1251Tab[128];
    static const unsigned char Windows1251ToKoi8RTab[128];

  protected:
    /*! Returns the string converted from windows-1250 to iso-8859-2. */
    QCString Win1250ToISO88592(const QCString & sInput);
    
    /*! Returns the string converted from iso-8859-2 to windows-1250. */
    QCString ISO88592ToWin1250(const QCString & sInput);
    
    /*! Returns the string converted from koi8-r to windows-1251. */
    QCString Koi8RToWindows1251( const QCString & sInput );

    /*! Returns the string converted from windows-1251 to koi8-r. */
    QCString Windows1251ToKoi8R( const QCString & sInput );
    
    /*! Returns the string converted from Japanese-EUC to SJIS. */
    QCString JapaneseEucToSjis( const QCString & sInput );

  public:

    /*! This method is used to provide warning message that is displayed
     *  if the user chooses a language whose translation is not up to date. 
     *  It is implemented by the adapter classes.
     */
    virtual QCString updateNeededMessage() { return ""; }
    virtual ~Translator() {}
   
    // Please, have a look at comments inside the translator_en.h file
    // to learn the meaning of the following methods.  The translator_en.h
    // file contains the TranslatorEnglish implementation, which is 
    // always up-to-date (by definition).
   
    // --- Language control methods -------------------
    
    virtual QCString idLanguage() = 0;
    virtual QCString latexLanguageSupportCommand() = 0;
    virtual QCString idLanguageCharset() = 0;

    // --- Language translation methods -------------------

    virtual QCString trRelatedFunctions() = 0;
    virtual QCString trRelatedSubscript() = 0;
    virtual QCString trDetailedDescription() = 0;
    virtual QCString trMemberTypedefDocumentation() = 0;
    virtual QCString trMemberEnumerationDocumentation() = 0;
    virtual QCString trMemberFunctionDocumentation() = 0;
    virtual QCString trMemberDataDocumentation() = 0;
    virtual QCString trMore() = 0;
    virtual QCString trListOfAllMembers() = 0;
    virtual QCString trMemberList() = 0;
    virtual QCString trThisIsTheListOfAllMembers() = 0;
    virtual QCString trIncludingInheritedMembers() = 0;
    virtual QCString trGeneratedAutomatically(const char *s) = 0;
    virtual QCString trEnumName() = 0;
    virtual QCString trEnumValue() = 0;
    virtual QCString trDefinedIn() = 0;

    // quick reference sections

    virtual QCString trModules() = 0;
    virtual QCString trClassHierarchy() = 0;
    virtual QCString trCompoundList() = 0;
    virtual QCString trFileList() = 0;
    //virtual QCString trHeaderFiles() = 0;
    virtual QCString trCompoundMembers() = 0;
    virtual QCString trFileMembers() = 0;
    virtual QCString trRelatedPages() = 0;
    virtual QCString trExamples() = 0;
    virtual QCString trSearch() = 0;
    virtual QCString trClassHierarchyDescription() = 0;
    virtual QCString trFileListDescription(bool extractAll) = 0;
    virtual QCString trCompoundListDescription() = 0;
    virtual QCString trCompoundMembersDescription(bool extractAll) = 0;
    virtual QCString trFileMembersDescription(bool extractAll) = 0;
    //virtual QCString trHeaderFilesDescription() = 0;
    virtual QCString trExamplesDescription() = 0;
    virtual QCString trRelatedPagesDescription() = 0;
    virtual QCString trModulesDescription() = 0;
    //virtual QCString trNoDescriptionAvailable() = 0;
    
    // index titles (the project name is prepended for these) 

    virtual QCString trDocumentation() = 0;
    virtual QCString trModuleIndex() = 0;
    virtual QCString trHierarchicalIndex() = 0;
    virtual QCString trCompoundIndex() = 0;
    virtual QCString trFileIndex() = 0; 
    virtual QCString trModuleDocumentation() = 0;
    virtual QCString trClassDocumentation() = 0;
    virtual QCString trFileDocumentation() = 0;
    virtual QCString trExampleDocumentation() = 0;
    virtual QCString trPageDocumentation() = 0;
    virtual QCString trReferenceManual() = 0;
    virtual QCString trDefines() = 0;
    virtual QCString trFuncProtos() = 0;
    virtual QCString trTypedefs() = 0;
    virtual QCString trEnumerations() = 0;
    virtual QCString trFunctions() = 0;
    virtual QCString trVariables() = 0;
    virtual QCString trEnumerationValues() = 0;
    virtual QCString trDefineDocumentation() = 0;
    virtual QCString trFunctionPrototypeDocumentation() = 0;
    virtual QCString trTypedefDocumentation() = 0;
    virtual QCString trEnumerationTypeDocumentation() = 0;
    virtual QCString trFunctionDocumentation() = 0;
    virtual QCString trVariableDocumentation() = 0;
    virtual QCString trCompounds() = 0;
    virtual QCString trGeneratedAt(const char *date,const char *projName) = 0;
    virtual QCString trWrittenBy() = 0;
    virtual QCString trClassDiagram(const char *clName) = 0;
    virtual QCString trForInternalUseOnly() = 0;
    //virtual QCString trReimplementedForInternalReasons() = 0;
    virtual QCString trWarning() = 0;
    //virtual QCString trBugsAndLimitations() = 0;
    virtual QCString trVersion() = 0;
    virtual QCString trDate() = 0;
    virtual QCString trReturns() = 0;
    virtual QCString trSeeAlso() = 0;
    virtual QCString trParameters() = 0;
    virtual QCString trExceptions() = 0;
    virtual QCString trGeneratedBy() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////
    
    virtual QCString trNamespaceList() = 0;
    virtual QCString trNamespaceListDescription(bool extractAll) = 0;
    virtual QCString trFriends() = 0;
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
    virtual QCString trRelatedFunctionDocumentation() = 0;
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    virtual QCString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) = 0;

    virtual QCString trFileReference(const char *fileName) = 0;
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
    virtual QCString trWriteList(int numEntries) = 0;
    virtual QCString trInheritsList(int numEntries) = 0;
    virtual QCString trInheritedByList(int numEntries) = 0;
    virtual QCString trReimplementedFromList(int numEntries) = 0;
    virtual QCString trReimplementedInList(int numEntries) = 0;
    virtual QCString trNamespaceMembers() = 0;
    virtual QCString trNamespaceMemberDescription(bool extractAll) = 0;
    virtual QCString trNamespaceIndex() = 0;
    virtual QCString trNamespaceDocumentation() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    virtual QCString trNamespaces() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    virtual QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
                                          bool single) = 0;
    virtual QCString trAlphabeticalList() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReturnValues() = 0;
    virtual QCString trMainPage() = 0;
    virtual QCString trPageAbbreviation() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    //virtual QCString trSources() = 0;
    virtual QCString trDefinedAtLineInSourceFile() = 0;
    virtual QCString trDefinedInSourceFile() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    virtual QCString trCollaborationDiagram(const char *clName) = 0;
    virtual QCString trInclDepGraph(const char *fName) = 0;
    virtual QCString trConstructorDocumentation() = 0;
    virtual QCString trGotoSourceCode() = 0;
    virtual QCString trGotoDocumentation() = 0;
    virtual QCString trPrecondition() = 0;
    virtual QCString trPostcondition() = 0;
    virtual QCString trInvariant() = 0;
    virtual QCString trInitialValue() = 0;
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

    virtual QCString trTodo() = 0;
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

    virtual QCString trLegendTitle() = 0;
    virtual QCString trLegendDocs() = 0;
    virtual QCString trLegend() = 0;
    
//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////
    
    virtual QCString trTest() = 0;
    virtual QCString trTestList() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDCOPMethods() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    virtual QCString trProperties() = 0;
    virtual QCString trPropertyDocumentation() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    //virtual QCString trInterfaces() = 0;
    virtual QCString trClasses() = 0;
    virtual QCString trPackage(const char *name) = 0;
    virtual QCString trPackageList() = 0;
    virtual QCString trPackageListDescription() = 0;
    virtual QCString trPackages() = 0;
    //virtual QCString trPackageDocumentation() = 0;
    virtual QCString trDefineValue() = 0;
    
//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////
    
    virtual QCString trBug() = 0;
    virtual QCString trBugList() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.2.6
//////////////////////////////////////////////////////////////////////////

    virtual QCString trRTFansicp() = 0;
    virtual QCString trRTFCharSet() = 0;
    virtual QCString trRTFGeneralIndex() = 0;
   
    // Translation of the word

    virtual QCString trClass(bool first_capital, bool singular) = 0;
    virtual QCString trFile(bool first_capital, bool singular) = 0;
    virtual QCString trNamespace(bool first_capital, bool singular) = 0;
    virtual QCString trGroup(bool first_capital, bool singular) = 0;
    virtual QCString trPage(bool first_capital, bool singular) = 0;
    virtual QCString trMember(bool first_capital, bool singular) = 0;
    //virtual QCString trField(bool first_capital, bool singular) = 0;
    virtual QCString trGlobal(bool first_capital, bool singular) = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////
        
    virtual QCString trAuthor(bool first_capital, bool singular) = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferences() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    virtual QCString trImplementedFromList(int numEntries) = 0;
    virtual QCString trImplementedInList(int numEntries) = 0;
    
//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    virtual QCString trRTFTableOfContents() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecatedList() = 0;
    
//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    virtual QCString trEvents() = 0;
    virtual QCString trEventDocumentation() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    virtual QCString trPackageTypes() = 0;
    virtual QCString trPackageMembers() = 0;
    virtual QCString trStaticPackageMembers() = 0;
    virtual QCString trPackageAttribs() = 0;
    virtual QCString trStaticPackageAttribs() = 0;
    
//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    virtual QCString trAll() = 0;
    virtual QCString trCallGraph() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    virtual QCString trSearchForIndex() = 0;
    virtual QCString trSearchResultsTitle() = 0;
    virtual QCString trSearchResults(int numDocuments) = 0;
    virtual QCString trSearchMatches() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    virtual QCString trSourceFile(QCString& filename) = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDirIndex() = 0;
    virtual QCString trDirDocumentation() = 0;
    virtual QCString trDirectories() = 0;
    virtual QCString trDirDescription() = 0;
    virtual QCString trDirReference(const char *dirName) = 0;
    virtual QCString trDir(bool first_capital, bool singular) = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    virtual QCString trOverloadText() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////
    
    virtual QCString trCallerGraph() = 0;
    virtual QCString trEnumerationValueDocumentation() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4
//////////////////////////////////////////////////////////////////////////
    
    virtual QCString trMemberFunctionDocumentationFortran() = 0;
    virtual QCString trCompoundListFortran() = 0;
    virtual QCString trCompoundMembersFortran() = 0;
    virtual QCString trCompoundListDescriptionFortran() = 0;
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll) = 0;
    virtual QCString trCompoundIndexFortran() = 0;
    virtual QCString trTypeDocumentation() = 0;
    virtual QCString trSubprograms() = 0;
    virtual QCString trSubprogramDocumentation() = 0;
    virtual QCString trDataTypes() = 0;
    virtual QCString trModulesList() = 0;
    virtual QCString trModulesListDescription(bool extractAll) = 0;
    virtual QCString trCompoundReferenceFortran(const char *clName,
                                 ClassDef::CompoundType compType,
                                 bool isTemplate) = 0;
    virtual QCString trModuleReference(const char *namespaceName) = 0;
    virtual QCString trModulesMembers() = 0;
    virtual QCString trModulesMemberDescription(bool extractAll) = 0;
    virtual QCString trModulesIndex() = 0;
    virtual QCString trModule(bool first_capital, bool singular) = 0;
    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
                     bool single) = 0;
    virtual QCString trType(bool first_capital, bool singular) = 0;
    virtual QCString trSubprogram(bool first_capital, bool singular) = 0;
    virtual QCString trTypeConstraints() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDirRelation(const char *name) = 0;
    virtual QCString trLoading() = 0;
    virtual QCString trGlobalNamespace() = 0;
    virtual QCString trSearching() = 0;
    virtual QCString trNoMatches() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.6.3
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDirDependency(const char *name) = 0;
    virtual QCString trFileIn(const char *name) = 0;
    virtual QCString trIncludesFileIn(const char *name) = 0;
    virtual QCString trDateTime(int year,int month,int day,int dayOfWeek,
                                int hour,int minutes,int seconds,
                                bool includeTime) = 0;
                       
//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    virtual QCString trCiteReferences() = 0;
    virtual QCString trCopyright() = 0;
    virtual QCString trDirDepGraph(const char *name) = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDetailLevel() = 0;
    virtual QCString trTemplateParameters() = 0;
    virtual QCString trAndMore(const QCString &number) = 0;
    virtual QCString trEnumGeneratedFromFiles(bool single) = 0;
    virtual QCString trEnumReference(const char *name) = 0;
    virtual QCString trInheritedFrom(const char *members,const char *what) = 0;
    virtual QCString trAdditionalInheritedMembers() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    virtual QCString trPanelSynchronisationTooltip(bool enable) = 0;
    virtual QCString trProvidedByCategory() = 0;
    virtual QCString trExtendsClass() = 0;
    virtual QCString trClassMethods() = 0;
    virtual QCString trInstanceMethods() = 0;
    virtual QCString trMethodDocumentation() = 0;
    virtual QCString trDesignOverview() = 0;

};

#endif
