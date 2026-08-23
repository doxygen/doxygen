/******************************************************************************
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
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

#include "classdef.h"
#include "config.h"
#include "construct.h"
#include "datetime.h"
#include "index.h"

/** Abstract base class for all translatable text fragments. */
class Translator
{
  public:
    ABSTRACT_BASE_CLASS(Translator)

    /*! This method is used to provide warning message that is displayed
     *  if the user chooses a language whose translation is not up to date.
     *  It is implemented by the adapter classes.
     */
    virtual DString updateNeededMessage() { return DString(); }

    // Please, have a look at comments inside the translator_en.h file
    // to learn the meaning of the following methods.  The translator_en.h
    // file contains the TranslatorEnglish implementation, which is
    // always up-to-date (by definition).

    // --- Language control methods -------------------

    virtual DString idLanguage() = 0;
    virtual DString latexLanguageSupportCommand() = 0;
    /*!
     * Sets the LaTeX font encoding to be used. The default is set to `T1`,
     * in case another font encoding has to be used this can be specified with
     * this routine. In case no font encoding is required the empty string
     * can be returned.
     */
    virtual DString latexFontenc() { return DString("T1"); }
    virtual DString latexFont() {
      return DString("\\usepackage[scaled=.90]{helvet}\n"
             "\\usepackage{courier}\n"
             "\\renewcommand{\\familydefault}{\\sfdefault}\n");
    }
    /*!
     * Sets the commands to be inserted directly after the `\\begin{document}`
     * in the LaTeX document.
     */
    virtual DString latexDocumentPre() { return DString(""); }
    /*!
     * Sets the commands to be inserted directly before the `\\end{document}`
     * in the LaTeX document.
     */
    virtual DString latexDocumentPost() { return DString(""); }
    /*!
     * Set the name to be used as latex command.
     */
    virtual DString latexCommandName()
    {
      return p_latexCommandName("pdflatex");
    }
    virtual DString trISOLang() = 0;

    /** language codes for Html help

       <pre>
       0x402 Bulgarian
       0x405 Czech
       0x406 Danish
       0x413 Dutch
       0xC09 English (Australia)
       0x809 English (Britain)
       0x1009 English (Canada)
       0x1809 English (Ireland)
       0x1409 English (New Zealand)
       0x1C09 English (South Africa)
       0x409 English (United States)
       0x40B Finnish
       0x40C French
       0x407 German
       0x408 Greece
       0x439 Hindi
       0x40E Hungarian
       0x410 Italian
       0x814 Norwegian
       0x415 Polish
       0x816 Portuguese(Portugal)
       0x416 Portuguese(Brazil)
       0x419 Russian
       0x80A Spanish(Mexico)
       0xC0A Spanish(Modern Sort)
       0x40A Spanish(Traditional Sort)
       0x41D Swedish
       0x41F Turkey
       0x411 Japanese
       0x412 Korean
       0x804 Chinese (PRC)
       0x404 Chinese (Taiwan)
       </pre>

       New LCIDs:
       <pre>
       0x421 Indonesian
       0x41A Croatian
       0x418 Romanian
       0x424 Slovenian
       0x41B Slovak
       0x422 Ukrainian
       0x81A Serbian (Serbia, Latin)
       0x403 Catalan
       0x426 Latvian
       0x427 Lithuanian
       0x436 Afrikaans
       0x42A Vietnamese
       0x429 Persian (Iran)
       0xC01 Arabic (Egypt) - I don't know which version of arabic is used inside translator_ar.h ,
             so I have chosen Egypt at random
       </pre>

      Code for Esperanto should be as shown below but the htmlhelp compiler 1.3 does not support this
      (and no newer version is available).
      <pre>
      0x48f Esperanto
      </pre>
      So do a fallback to the default language
      <pre>
      0x409 English (United States)
      </pre>

      <pre>
      0xC1A Serbian (Serbia, Cyrillic)
      </pre>
    */
    virtual DString getLanguageString() = 0;

    /**
     * add punctuation at the end of a brief description when needed and supported by the language
    */
    virtual bool needsPunctuation() { return true; }

    // --- Language translation methods -------------------

    virtual DString trRelatedFunctions() = 0;
    virtual DString trRelatedSubscript() = 0;
    virtual DString trDetailedDescription() = 0;
    virtual DString trDetails() = 0;
    virtual DString trMemberTypedefDocumentation() = 0;
    virtual DString trMemberEnumerationDocumentation() = 0;
    virtual DString trMemberFunctionDocumentation() = 0;
    virtual DString trMemberDataDocumentation() = 0;
    virtual DString trMore() = 0;
    virtual DString trListOfAllMembers() = 0;
    virtual DString trMemberList() = 0;
    virtual DString trThisIsTheListOfAllMembers() = 0;
    virtual DString trIncludingInheritedMembers() = 0;
    virtual DString trGeneratedAutomatically(const DString &s) = 0;
    virtual DString trEnumName() = 0;
    virtual DString trEnumValue() = 0;
    virtual DString trDefinedIn() = 0;

    // quick reference sections

    virtual DString trModules() = 0;
    virtual DString trClassHierarchy() = 0;
    virtual DString trCompoundList() = 0;
    virtual DString trFileList() = 0;
    virtual DString trCompoundMembers() = 0;
    virtual DString trFileMembers() = 0;
    virtual DString trRelatedPages() = 0;
    virtual DString trExamples() = 0;
    virtual DString trSearch() = 0;
    virtual DString trClassHierarchyDescription() = 0;
    virtual DString trFileListDescription(bool extractAll) = 0;
    virtual DString trCompoundListDescription() = 0;
    virtual DString trCompoundMembersDescription(bool extractAll) = 0;
    virtual DString trFileMembersDescription(bool extractAll) = 0;
    virtual DString trExamplesDescription() = 0;
    virtual DString trRelatedPagesDescription() = 0;
    virtual DString trModulesDescription() = 0;

    // index titles (the project name is prepended for these)

    virtual DString trDocumentation(const DString &projName) = 0;
    virtual DString trModuleIndex() = 0;
    virtual DString trHierarchicalIndex() = 0;
    virtual DString trCompoundIndex() = 0;
    virtual DString trFileIndex() = 0;
    virtual DString trModuleDocumentation() = 0;
    virtual DString trClassDocumentation() = 0;
    virtual DString trFileDocumentation() = 0;
    virtual DString trReferenceManual() = 0;
    virtual DString trDefines() = 0;
    virtual DString trTypedefs() = 0;
    virtual DString trEnumerations() = 0;
    virtual DString trFunctions() = 0;
    virtual DString trVariables() = 0;
    virtual DString trEnumerationValues() = 0;
    virtual DString trDefineDocumentation() = 0;
    virtual DString trTypedefDocumentation() = 0;
    virtual DString trEnumerationTypeDocumentation() = 0;
    virtual DString trFunctionDocumentation() = 0;
    virtual DString trVariableDocumentation() = 0;
    virtual DString trCompounds() = 0;
    virtual DString trGeneratedAt(const DString &date,const DString &projName) = 0;
    virtual DString trClassDiagram(const DString &clName) = 0;
    virtual DString trWarning() = 0;
    virtual DString trVersion() = 0;
    virtual DString trDate() = 0;
    virtual DString trReturns() = 0;
    virtual DString trSeeAlso() = 0;
    virtual DString trParameters() = 0;
    virtual DString trExceptions() = 0;
    virtual DString trGeneratedBy() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    virtual DString trNamespaceList() = 0;
    virtual DString trNamespaceListDescription(bool extractAll) = 0;
    virtual DString trFriends() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    virtual DString trRelatedFunctionDocumentation() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    virtual DString trCompoundReference(const DString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) = 0;

    virtual DString trFileReference(const DString &fileName) = 0;
    virtual DString trNamespaceReference(const DString &namespaceName) = 0;

    virtual DString trPublicMembers() = 0;
    virtual DString trPublicSlots() = 0;
    virtual DString trSignals() = 0;
    virtual DString trStaticPublicMembers() = 0;
    virtual DString trProtectedMembers() = 0;
    virtual DString trProtectedSlots() = 0;
    virtual DString trStaticProtectedMembers() = 0;
    virtual DString trPrivateMembers() = 0;
    virtual DString trPrivateSlots() = 0;
    virtual DString trStaticPrivateMembers() = 0;
    virtual DString trWriteList(int numEntries) = 0;
    virtual DString trInheritsList(int numEntries) = 0;
    virtual DString trInheritedByList(int numEntries) = 0;
    virtual DString trReimplementedFromList(int numEntries) = 0;
    virtual DString trReimplementedInList(int numEntries) = 0;
    virtual DString trNamespaceMembers() = 0;
    virtual DString trNamespaceMemberDescription(bool extractAll) = 0;
    virtual DString trNamespaceIndex() = 0;
    virtual DString trNamespaceDocumentation() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    virtual DString trNamespaces() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    virtual DString trGeneratedFromFiles(ClassDef::CompoundType compType,
                                          bool single) = 0;

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    virtual DString trReturnValues() = 0;
    virtual DString trMainPage() = 0;
    virtual DString trPageAbbreviation() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual DString trDefinedAtLineInSourceFile() = 0;
    virtual DString trDefinedInSourceFile() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual DString trDeprecated() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    virtual DString trCollaborationDiagram(const DString &clName) = 0;
    virtual DString trInclDepGraph(const DString &fName) = 0;
    virtual DString trConstructorDocumentation() = 0;
    virtual DString trGotoSourceCode() = 0;
    virtual DString trGotoDocumentation() = 0;
    virtual DString trPrecondition() = 0;
    virtual DString trPostcondition() = 0;
    virtual DString trInvariant() = 0;
    virtual DString trInitialValue() = 0;
    virtual DString trCode() = 0;

    virtual DString trGraphicalHierarchy() = 0;
    virtual DString trGotoGraphicalHierarchy() = 0;
    virtual DString trGotoTextualHierarchy() = 0;
    virtual DString trPageIndex() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    virtual DString trNote() = 0;
    virtual DString trPublicTypes() = 0;
    virtual DString trPublicAttribs() = 0;
    virtual DString trStaticPublicAttribs() = 0;
    virtual DString trProtectedTypes() = 0;
    virtual DString trProtectedAttribs() = 0;
    virtual DString trStaticProtectedAttribs() = 0;
    virtual DString trPrivateTypes() = 0;
    virtual DString trPrivateAttribs() = 0;
    virtual DString trStaticPrivateAttribs() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    virtual DString trTodo() = 0;
    virtual DString trTodoList() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual DString trReferencedBy() = 0;
    virtual DString trRemarks() = 0;
    virtual DString trAttention() = 0;
    virtual DString trInclByDepGraph() = 0;
    virtual DString trSince() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    virtual DString trLegendTitle() = 0;
    virtual DString trLegendDocs() = 0;
    virtual DString trLegend() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    virtual DString trTest() = 0;
    virtual DString trTestList() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    virtual DString trProperties() = 0;
    virtual DString trPropertyDocumentation() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    virtual DString trClasses() = 0;
    virtual DString trPackage(const DString &name) = 0;
    virtual DString trPackageListDescription() = 0;
    virtual DString trPackages() = 0;
    virtual DString trDefineValue() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    virtual DString trBug() = 0;
    virtual DString trBugList() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.2.6
//////////////////////////////////////////////////////////////////////////


    /*! Used as ansicpg for RTF file
     *
     * The following table shows the correlation of Charset name, Charset Value and
     * <pre>
     * Codepage number:
     * Charset Name       Charset Value(hex)  Codepage number
     * ------------------------------------------------------
     * DEFAULT_CHARSET           1 (x01)
     * SYMBOL_CHARSET            2 (x02)
     * OEM_CHARSET             255 (xFF)
     * ANSI_CHARSET              0 (x00)            1252
     * RUSSIAN_CHARSET         204 (xCC)            1251
     * EE_CHARSET              238 (xEE)            1250
     * GREEK_CHARSET           161 (xA1)            1253
     * TURKISH_CHARSET         162 (xA2)            1254
     * BALTIC_CHARSET          186 (xBA)            1257
     * HEBREW_CHARSET          177 (xB1)            1255
     * ARABIC _CHARSET         178 (xB2)            1256
     * SHIFTJIS_CHARSET        128 (x80)             932
     * HANGEUL_CHARSET         129 (x81)             949
     * GB2313_CHARSET          134 (x86)             936
     * CHINESEBIG5_CHARSET     136 (x88)             950
     * </pre>
     *
     */
    virtual DString trRTFansicp() = 0;

    /*! Character sets
     *  <pre>
     *   0 — ANSI
     *   1 — Default
     *   2 — Symbol
     *   3 — Invalid
     *  77 — Mac
     * 128 — Shift Jis
     * 129 — Hangul
     * 130 — Johab
     * 134 — GB2312
     * 136 — Big5
     * 161 — Greek
     * 162 — Turkish
     * 163 — Vietnamese
     * 177 — Hebrew
     * 178 — Arabic
     * 179 — Arabic Traditional
     * 180 — Arabic user
     * 181 — Hebrew user
     * 186 — Baltic
     * 204 — Russian
     * 222 — Thai
     * 238 — Eastern European
     * 254 — PC 437
     * 255 — OEM
     * </pre>
     */
    virtual DString trRTFCharSet() = 0;
    virtual DString trRTFGeneralIndex() = 0;

    // Translation of the word

    virtual DString trClass(bool first_capital, bool singular) = 0;
    virtual DString trFile(bool first_capital, bool singular) = 0;
    virtual DString trNamespace(bool first_capital, bool singular) = 0;
    virtual DString trGroup(bool first_capital, bool singular) = 0;
    virtual DString trPage(bool first_capital, bool singular) = 0;
    virtual DString trMember(bool first_capital, bool singular) = 0;
    virtual DString trGlobal(bool first_capital, bool singular) = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    virtual DString trAuthor(bool first_capital, bool singular) = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    virtual DString trReferences() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    virtual DString trImplementedFromList(int numEntries) = 0;
    virtual DString trImplementedInList(int numEntries) = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    virtual DString trRTFTableOfContents() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    virtual DString trDeprecatedList() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    virtual DString trEvents() = 0;
    virtual DString trEventDocumentation() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    virtual DString trPackageTypes() = 0;
    virtual DString trPackageFunctions() = 0;
    virtual DString trPackageMembers() = 0;
    virtual DString trStaticPackageFunctions() = 0;
    virtual DString trPackageAttribs() = 0;
    virtual DString trStaticPackageAttribs() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    virtual DString trAll() = 0;
    virtual DString trCallGraph() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    virtual DString trSearchResultsTitle() = 0;
    virtual DString trSearchResults(int numDocuments) = 0;
    virtual DString trSearchMatches() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    virtual DString trSourceFile(const DString& filename) = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    virtual DString trDirIndex() = 0;
    virtual DString trDirDocumentation() = 0;
    virtual DString trDirectories() = 0;
    virtual DString trDirReference(const DString &dirName) = 0;
    virtual DString trDir(bool first_capital, bool singular) = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    virtual DString trOverloadText() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    virtual DString trCallerGraph() = 0;
    virtual DString trEnumerationValueDocumentation() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4
//////////////////////////////////////////////////////////////////////////

    virtual DString trMemberFunctionDocumentationFortran() = 0;
    virtual DString trCompoundListFortran() = 0;
    virtual DString trCompoundMembersFortran() = 0;
    virtual DString trCompoundListDescriptionFortran() = 0;
    virtual DString trCompoundMembersDescriptionFortran(bool extractAll) = 0;
    virtual DString trCompoundIndexFortran() = 0;
    virtual DString trTypeDocumentation() = 0;
    virtual DString trSubprograms() = 0;
    virtual DString trSubprogramDocumentation() = 0;
    virtual DString trDataTypes() = 0;
    virtual DString trModulesList() = 0;
    virtual DString trModulesListDescription(bool extractAll) = 0;
    virtual DString trCompoundReferenceFortran(const DString &clName,
                                 ClassDef::CompoundType compType,
                                 bool isTemplate) = 0;
    virtual DString trModuleReference(const DString &namespaceName) = 0;
    virtual DString trModulesMembers() = 0;
    virtual DString trModulesMemberDescription(bool extractAll) = 0;
    virtual DString trModulesIndex() = 0;
    virtual DString trModule(bool first_capital, bool singular) = 0;
    virtual DString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
                     bool single) = 0;
    virtual DString trType(bool first_capital, bool singular) = 0;
    virtual DString trSubprogram(bool first_capital, bool singular) = 0;
    virtual DString trTypeConstraints() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0
//////////////////////////////////////////////////////////////////////////

    virtual DString trDirRelation(const DString &name) = 0;
    virtual DString trLoading() = 0;
    virtual DString trGlobalNamespace() = 0;
    virtual DString trSearching() = 0;
    virtual DString trNoMatches() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.6.3
//////////////////////////////////////////////////////////////////////////

    virtual DString trFileIn(const DString &name) = 0;
    virtual DString trIncludesFileIn(const DString &name) = 0;
    virtual DString trDateTime(int year,int month,int day,int dayOfWeek,
                                int hour,int minutes,int seconds,
                                DateTimeType includeTime) = 0;
    virtual DString trDayOfWeek(int dayOfWeek, bool first_capital, bool full) = 0;
    virtual DString trMonth(int month, bool first_capital, bool full) = 0;
    virtual DString trDayPeriod(bool period) = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    virtual DString trCiteReferences() = 0;
    virtual DString trCopyright() = 0;
    virtual DString trDirDepGraph(const DString &name) = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    virtual DString trDetailLevel() = 0;
    virtual DString trTemplateParameters() = 0;
    virtual DString trAndMore(const DString &number) = 0;
    virtual DString trEnumGeneratedFromFiles(bool single) = 0;
    virtual DString trEnumReference(const DString &name) = 0;
    virtual DString trInheritedFrom(const DString &members,const DString &what) = 0;
    virtual DString trAdditionalInheritedMembers() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    virtual DString trPanelSynchronisationTooltip(bool enable) = 0;
    virtual DString trProvidedByCategory() = 0;
    virtual DString trExtendsClass() = 0;
    virtual DString trClassMethods() = 0;
    virtual DString trInstanceMethods() = 0;
    virtual DString trMethodDocumentation() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    virtual DString trInterfaces() = 0;
    virtual DString trServices() = 0;
    virtual DString trConstantGroups() = 0;
    virtual DString trConstantGroupReference(const DString &namespaceName) = 0;
    virtual DString trServiceReference(const DString &sName) = 0;
    virtual DString trSingletonReference(const DString &sName) = 0;
    virtual DString trServiceGeneratedFromFiles(bool single) = 0;
    virtual DString trSingletonGeneratedFromFiles(bool single) = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.8.15
//////////////////////////////////////////////////////////////////////////

    virtual DString trDesignUnitHierarchy() = 0;
    virtual DString trDesignUnitList() = 0;
    virtual DString trDesignUnitMembers() = 0;
    virtual DString trDesignUnitListDescription() = 0;
    virtual DString trDesignUnitIndex() = 0;
    virtual DString trDesignUnits() = 0;
    virtual DString trFunctionAndProc() = 0;
    virtual DString trVhdlType(VhdlSpecifier type,bool single) = 0;
    virtual DString trCustomReference(const DString &name) = 0;

    virtual DString trConstants() = 0;
    virtual DString trConstantDocumentation() = 0;
    virtual DString trSequences() = 0;
    virtual DString trSequenceDocumentation() = 0;
    virtual DString trDictionaries() = 0;
    virtual DString trDictionaryDocumentation() = 0;
    virtual DString trSliceInterfaces() = 0;
    virtual DString trInterfaceIndex() = 0;
    virtual DString trInterfaceList() = 0;
    virtual DString trInterfaceListDescription() = 0;
    virtual DString trInterfaceHierarchy() = 0;
    virtual DString trInterfaceHierarchyDescription() = 0;
    virtual DString trInterfaceDocumentation() = 0;
    virtual DString trStructs() = 0;
    virtual DString trStructIndex() = 0;
    virtual DString trStructList() = 0;
    virtual DString trStructListDescription() = 0;
    virtual DString trStructDocumentation() = 0;
    virtual DString trExceptionIndex() = 0;
    virtual DString trExceptionList() = 0;
    virtual DString trExceptionListDescription() = 0;
    virtual DString trExceptionHierarchy() = 0;
    virtual DString trExceptionHierarchyDescription() = 0;
    virtual DString trExceptionDocumentation() = 0;
    virtual DString trCompoundReferenceSlice(const DString &clName, ClassDef::CompoundType compType, bool isLocal) = 0;
    virtual DString trOperations() = 0;
    virtual DString trOperationDocumentation() = 0;
    virtual DString trDataMembers() = 0;
    virtual DString trDataMemberDocumentation() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.8.19
//////////////////////////////////////////////////////////////////////////

    virtual DString trDesignUnitDocumentation() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.9.2
//////////////////////////////////////////////////////////////////////////

    virtual DString trConcept(bool first_capital, bool singular) = 0;
    virtual DString trConceptReference(const DString &conceptName) = 0;
    virtual DString trConceptList() = 0;
    virtual DString trConceptIndex() = 0;
    virtual DString trConceptDocumentation() = 0;
    virtual DString trConceptListDescription(bool extractAll) = 0;
    virtual DString trConceptDefinition() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.9.4
//////////////////////////////////////////////////////////////////////////
    virtual DString trPackageList() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.9.6
//////////////////////////////////////////////////////////////////////////
    virtual DString trFlowchart() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.9.7
//////////////////////////////////////////////////////////////////////////
    virtual DString trRelatedSymbols() = 0;
    virtual DString trRelatedSymbolsSubscript() = 0;
    virtual DString trRelatedSymbolDocumentation() = 0;

    virtual DString trCompoundType(ClassDef::CompoundType compType, SrcLangExt lang) = 0;

    virtual DString trFileMembersDescriptionTotal(FileMemberHighlight::Enum hl) = 0;
    virtual DString trCompoundMembersDescriptionTotal(ClassMemberHighlight::Enum hl) = 0;
    virtual DString trNamespaceMembersDescriptionTotal(NamespaceMemberHighlight::Enum hl) = 0;
    virtual DString trDefinition() = 0;
    virtual DString trDeclaration() = 0;
//////////////////////////////////////////////////////////////////////////
// new since 1.9.8
//////////////////////////////////////////////////////////////////////////
    virtual DString trTopics() = 0;
    virtual DString trTopicDocumentation() = 0;
    virtual DString trTopicList() = 0;
    virtual DString trTopicIndex() = 0;
    virtual DString trTopicListDescription() = 0;
    virtual DString trModuleMembersDescriptionTotal(ModuleMemberHighlight::Enum hl) = 0;
    virtual DString trExportedModules() = 0;
//////////////////////////////////////////////////////////////////////////
// new since 1.10.0
//////////////////////////////////////////////////////////////////////////
    virtual DString trCopyToClipboard() = 0;
//////////////////////////////////////////////////////////////////////////
// new since 1.11.0
//////////////////////////////////////////////////////////////////////////
    virtual DString trImportant() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.16.0
//////////////////////////////////////////////////////////////////////////
    virtual DString trRequirements() = 0;
    virtual DString trRequirementID() = 0;
    virtual DString trSatisfies(bool singular) = 0;
    virtual DString trSatisfiedBy(const DString &list) = 0;
    virtual DString trUnsatisfiedRequirements() = 0;
    virtual DString trUnsatisfiedRequirementsText(bool singular,const DString &list) = 0;
    virtual DString trVerifies(bool singular) = 0;
    virtual DString trVerifiedBy(const DString &list) = 0;
    virtual DString trUnverifiedRequirements() = 0;
    virtual DString trUnverifiedRequirementsText(bool singular,const DString &list) = 0;



//////////////////////////////////////////////////////////////////////////
// helpers
//////////////////////////////////////////////////////////////////////////
  protected:
    DString p_latexCommandName(const DString &latexCmd)
    {
      DString latex_command = Config_getString(LATEX_CMD_NAME);
      if (latex_command.empty()) latex_command = "latex";
      if (Config_getBool(USE_PDFLATEX))
      {
        if (latex_command == "latex") latex_command = latexCmd;
      }
      return latex_command;
    }
    /*! For easy flexible-noun implementation.
     *  \internal
     */
    DString createNoun(bool first_capital, bool singular,
                        const DString &base,
                        const DString &plurSuffix, const DString &singSuffix = "" )
    {
      DString result;
      if (first_capital)
      {
        DString lc = getUTF8CharAt(base.str(),0);
        DString uc = convertUTF8ToUpper(lc.str());
        result = uc + base.mid(lc.length());
      }
      else
      {
        result = base;
      }
      result += (singular ? singSuffix : plurSuffix);
      return result;
    }

};

#endif
