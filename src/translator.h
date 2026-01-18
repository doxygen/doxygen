/******************************************************************************
 *
 *
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
#include "datetime.h"
#include "index.h"
#include "construct.h"

/** Abstract base class for all translatable text fragments. */
class Translator
{
  public:
    ABSTRACT_BASE_CLASS(Translator)

    /*! This method is used to provide warning message that is displayed
     *  if the user chooses a language whose translation is not up to date.
     *  It is implemented by the adapter classes.
     */
    virtual QCString updateNeededMessage() { return QCString(); }

    // Please, have a look at comments inside the translator_en.h file
    // to learn the meaning of the following methods.  The translator_en.h
    // file contains the TranslatorEnglish implementation, which is
    // always up-to-date (by definition).

    // --- Language control methods -------------------

    virtual QCString idLanguage() = 0;
    virtual QCString latexLanguageSupportCommand() = 0;
    /*!
     * Sets the LaTeX font encoding to be used. The default is set to `T1`,
     * in case another font encoding has to be used this can be specified with
     * this routine. In case no font encoding is required the empty string
     * can be returned.
     */
    virtual QCString latexFontenc() { return QCString("T1"); }
    virtual QCString latexFont() {
      return QCString("\\usepackage[scaled=.90]{helvet}\n"
             "\\usepackage{courier}\n"
             "\\renewcommand{\\familydefault}{\\sfdefault}\n");
    }
    /*!
     * Sets the commands to be inserted directly after the `\\begin{document}`
     * in the LaTeX document.
     */
    virtual QCString latexDocumentPre() { return QCString(""); }
    /*!
     * Sets the commands to be inserted directly before the `\\end{document}`
     * in the LaTeX document.
     */
    virtual QCString latexDocumentPost() { return QCString(""); }
    /*!
     * Set the name to be used as latex command.
     */
    virtual QCString latexCommandName()
    {
      return p_latexCommandName("pdflatex");
    }
    virtual QCString trISOLang() = 0;

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
    virtual QCString getLanguageString() = 0;

    /**
     * add punctuation at the end of a brief description when needed and supported by the language
    */
    virtual bool needsPunctuation() { return true; }

    // --- Language translation methods -------------------

    virtual QCString trRelatedFunctions() = 0;
    virtual QCString trRelatedSubscript() = 0;
    virtual QCString trDetailedDescription() = 0;
    virtual QCString trDetails() = 0;
    virtual QCString trMemberTypedefDocumentation() = 0;
    virtual QCString trMemberEnumerationDocumentation() = 0;
    virtual QCString trMemberFunctionDocumentation() = 0;
    virtual QCString trMemberDataDocumentation() = 0;
    virtual QCString trMore() = 0;
    virtual QCString trListOfAllMembers() = 0;
    virtual QCString trMemberList() = 0;
    virtual QCString trThisIsTheListOfAllMembers() = 0;
    virtual QCString trIncludingInheritedMembers() = 0;
    virtual QCString trGeneratedAutomatically(const QCString &s) = 0;
    virtual QCString trEnumName() = 0;
    virtual QCString trEnumValue() = 0;
    virtual QCString trDefinedIn() = 0;

    // quick reference sections

    virtual QCString trModules() = 0;
    virtual QCString trClassHierarchy() = 0;
    virtual QCString trCompoundList() = 0;
    virtual QCString trFileList() = 0;
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
    virtual QCString trExamplesDescription() = 0;
    virtual QCString trRelatedPagesDescription() = 0;
    virtual QCString trModulesDescription() = 0;

    // index titles (the project name is prepended for these)

    virtual QCString trDocumentation(const QCString &projName) = 0;
    virtual QCString trModuleIndex() = 0;
    virtual QCString trHierarchicalIndex() = 0;
    virtual QCString trCompoundIndex() = 0;
    virtual QCString trFileIndex() = 0;
    virtual QCString trModuleDocumentation() = 0;
    virtual QCString trClassDocumentation() = 0;
    virtual QCString trFileDocumentation() = 0;
    virtual QCString trReferenceManual() = 0;
    virtual QCString trDefines() = 0;
    virtual QCString trTypedefs() = 0;
    virtual QCString trEnumerations() = 0;
    virtual QCString trFunctions() = 0;
    virtual QCString trVariables() = 0;
    virtual QCString trEnumerationValues() = 0;
    virtual QCString trDefineDocumentation() = 0;
    virtual QCString trTypedefDocumentation() = 0;
    virtual QCString trEnumerationTypeDocumentation() = 0;
    virtual QCString trFunctionDocumentation() = 0;
    virtual QCString trVariableDocumentation() = 0;
    virtual QCString trCompounds() = 0;
    virtual QCString trGeneratedAt(const QCString &date,const QCString &projName) = 0;
    virtual QCString trClassDiagram(const QCString &clName) = 0;
    virtual QCString trWarning() = 0;
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

    virtual QCString trCompoundReference(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) = 0;

    virtual QCString trFileReference(const QCString &fileName) = 0;
    virtual QCString trNamespaceReference(const QCString &namespaceName) = 0;

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

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReturnValues() = 0;
    virtual QCString trMainPage() = 0;
    virtual QCString trPageAbbreviation() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDefinedAtLineInSourceFile() = 0;
    virtual QCString trDefinedInSourceFile() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    virtual QCString trCollaborationDiagram(const QCString &clName) = 0;
    virtual QCString trInclDepGraph(const QCString &fName) = 0;
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
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    virtual QCString trProperties() = 0;
    virtual QCString trPropertyDocumentation() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trClasses() = 0;
    virtual QCString trPackage(const QCString &name) = 0;
    virtual QCString trPackageListDescription() = 0;
    virtual QCString trPackages() = 0;
    virtual QCString trDefineValue() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    virtual QCString trBug() = 0;
    virtual QCString trBugList() = 0;

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
    virtual QCString trRTFansicp() = 0;

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
    virtual QCString trRTFCharSet() = 0;
    virtual QCString trRTFGeneralIndex() = 0;

    // Translation of the word

    virtual QCString trClass(bool first_capital, bool singular) = 0;
    virtual QCString trFile(bool first_capital, bool singular) = 0;
    virtual QCString trNamespace(bool first_capital, bool singular) = 0;
    virtual QCString trGroup(bool first_capital, bool singular) = 0;
    virtual QCString trPage(bool first_capital, bool singular) = 0;
    virtual QCString trMember(bool first_capital, bool singular) = 0;
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
    virtual QCString trPackageFunctions() = 0;
    virtual QCString trPackageMembers() = 0;
    virtual QCString trStaticPackageFunctions() = 0;
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

    virtual QCString trSearchResultsTitle() = 0;
    virtual QCString trSearchResults(int numDocuments) = 0;
    virtual QCString trSearchMatches() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    virtual QCString trSourceFile(const QCString& filename) = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDirIndex() = 0;
    virtual QCString trDirDocumentation() = 0;
    virtual QCString trDirectories() = 0;
    virtual QCString trDirReference(const QCString &dirName) = 0;
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
    virtual QCString trCompoundReferenceFortran(const QCString &clName,
                                 ClassDef::CompoundType compType,
                                 bool isTemplate) = 0;
    virtual QCString trModuleReference(const QCString &namespaceName) = 0;
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

    virtual QCString trDirRelation(const QCString &name) = 0;
    virtual QCString trLoading() = 0;
    virtual QCString trGlobalNamespace() = 0;
    virtual QCString trSearching() = 0;
    virtual QCString trNoMatches() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.6.3
//////////////////////////////////////////////////////////////////////////

    virtual QCString trFileIn(const QCString &name) = 0;
    virtual QCString trIncludesFileIn(const QCString &name) = 0;
    virtual QCString trDateTime(int year,int month,int day,int dayOfWeek,
                                int hour,int minutes,int seconds,
                                DateTimeType includeTime) = 0;
    virtual QCString trDayOfWeek(int dayOfWeek, bool first_capital, bool full) = 0;
    virtual QCString trMonth(int month, bool first_capital, bool full) = 0;
    virtual QCString trDayPeriod(bool period) = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    virtual QCString trCiteReferences() = 0;
    virtual QCString trCopyright() = 0;
    virtual QCString trDirDepGraph(const QCString &name) = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDetailLevel() = 0;
    virtual QCString trTemplateParameters() = 0;
    virtual QCString trAndMore(const QCString &number) = 0;
    virtual QCString trEnumGeneratedFromFiles(bool single) = 0;
    virtual QCString trEnumReference(const QCString &name) = 0;
    virtual QCString trInheritedFrom(const QCString &members,const QCString &what) = 0;
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

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trInterfaces() = 0;
    virtual QCString trServices() = 0;
    virtual QCString trConstantGroups() = 0;
    virtual QCString trConstantGroupReference(const QCString &namespaceName) = 0;
    virtual QCString trServiceReference(const QCString &sName) = 0;
    virtual QCString trSingletonReference(const QCString &sName) = 0;
    virtual QCString trServiceGeneratedFromFiles(bool single) = 0;
    virtual QCString trSingletonGeneratedFromFiles(bool single) = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.8.15
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDesignUnitHierarchy() = 0;
    virtual QCString trDesignUnitList() = 0;
    virtual QCString trDesignUnitMembers() = 0;
    virtual QCString trDesignUnitListDescription() = 0;
    virtual QCString trDesignUnitIndex() = 0;
    virtual QCString trDesignUnits() = 0;
    virtual QCString trFunctionAndProc() = 0;
    virtual QCString trVhdlType(VhdlSpecifier type,bool single) = 0;
    virtual QCString trCustomReference(const QCString &name) = 0;

    virtual QCString trConstants() = 0;
    virtual QCString trConstantDocumentation() = 0;
    virtual QCString trSequences() = 0;
    virtual QCString trSequenceDocumentation() = 0;
    virtual QCString trDictionaries() = 0;
    virtual QCString trDictionaryDocumentation() = 0;
    virtual QCString trSliceInterfaces() = 0;
    virtual QCString trInterfaceIndex() = 0;
    virtual QCString trInterfaceList() = 0;
    virtual QCString trInterfaceListDescription() = 0;
    virtual QCString trInterfaceHierarchy() = 0;
    virtual QCString trInterfaceHierarchyDescription() = 0;
    virtual QCString trInterfaceDocumentation() = 0;
    virtual QCString trStructs() = 0;
    virtual QCString trStructIndex() = 0;
    virtual QCString trStructList() = 0;
    virtual QCString trStructListDescription() = 0;
    virtual QCString trStructDocumentation() = 0;
    virtual QCString trExceptionIndex() = 0;
    virtual QCString trExceptionList() = 0;
    virtual QCString trExceptionListDescription() = 0;
    virtual QCString trExceptionHierarchy() = 0;
    virtual QCString trExceptionHierarchyDescription() = 0;
    virtual QCString trExceptionDocumentation() = 0;
    virtual QCString trCompoundReferenceSlice(const QCString &clName, ClassDef::CompoundType compType, bool isLocal) = 0;
    virtual QCString trOperations() = 0;
    virtual QCString trOperationDocumentation() = 0;
    virtual QCString trDataMembers() = 0;
    virtual QCString trDataMemberDocumentation() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.8.19
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDesignUnitDocumentation() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.9.2
//////////////////////////////////////////////////////////////////////////

    virtual QCString trConcept(bool first_capital, bool singular) = 0;
    virtual QCString trConceptReference(const QCString &conceptName) = 0;
    virtual QCString trConceptList() = 0;
    virtual QCString trConceptIndex() = 0;
    virtual QCString trConceptDocumentation() = 0;
    virtual QCString trConceptListDescription(bool extractAll) = 0;
    virtual QCString trConceptDefinition() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.9.4
//////////////////////////////////////////////////////////////////////////
    virtual QCString trPackageList() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.9.6
//////////////////////////////////////////////////////////////////////////
    virtual QCString trFlowchart() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.9.7
//////////////////////////////////////////////////////////////////////////
    virtual QCString trRelatedSymbols() = 0;
    virtual QCString trRelatedSymbolsSubscript() = 0;
    virtual QCString trRelatedSymbolDocumentation() = 0;

    virtual QCString trCompoundType(ClassDef::CompoundType compType, SrcLangExt lang) = 0;

    virtual QCString trFileMembersDescriptionTotal(FileMemberHighlight::Enum hl) = 0;
    virtual QCString trCompoundMembersDescriptionTotal(ClassMemberHighlight::Enum hl) = 0;
    virtual QCString trNamespaceMembersDescriptionTotal(NamespaceMemberHighlight::Enum hl) = 0;
    virtual QCString trDefinition() = 0;
    virtual QCString trDeclaration() = 0;
//////////////////////////////////////////////////////////////////////////
// new since 1.9.8
//////////////////////////////////////////////////////////////////////////
    virtual QCString trTopics() = 0;
    virtual QCString trTopicDocumentation() = 0;
    virtual QCString trTopicList() = 0;
    virtual QCString trTopicIndex() = 0;
    virtual QCString trTopicListDescription() = 0;
    virtual QCString trModuleMembersDescriptionTotal(ModuleMemberHighlight::Enum hl) = 0;
    virtual QCString trExportedModules() = 0;
//////////////////////////////////////////////////////////////////////////
// new since 1.10.0
//////////////////////////////////////////////////////////////////////////
    virtual QCString trCopyToClipboard() = 0;
//////////////////////////////////////////////////////////////////////////
// new since 1.11.0
//////////////////////////////////////////////////////////////////////////
    virtual QCString trImportant() = 0;

//////////////////////////////////////////////////////////////////////////
// new since 1.16.0
//////////////////////////////////////////////////////////////////////////
    virtual QCString trRequirements() = 0;
    virtual QCString trRequirementID() = 0;
    virtual QCString trSatisfies(bool singular) = 0;
    virtual QCString trSatisfiedBy(const QCString &list) = 0;
    virtual QCString trUnsatisfiedRequirements() = 0;
    virtual QCString trUnsatisfiedRequirementsText(bool singular,const QCString &list) = 0;
    virtual QCString trVerifies(bool singular) = 0;
    virtual QCString trVerifiedBy(const QCString &list) = 0;
    virtual QCString trUnverifiedRequirements() = 0;
    virtual QCString trUnverifiedRequirementsText(bool singular,const QCString &list) = 0;



//////////////////////////////////////////////////////////////////////////
// helpers
//////////////////////////////////////////////////////////////////////////
  protected:
    QCString p_latexCommandName(const QCString &latexCmd)
    {
      QCString latex_command = Config_getString(LATEX_CMD_NAME);
      if (latex_command.isEmpty()) latex_command = "latex";
      if (Config_getBool(USE_PDFLATEX))
      {
        if (latex_command == "latex") latex_command = latexCmd;
      }
      return latex_command;
    }
    /*! For easy flexible-noun implementation.
     *  \internal
     */
    QCString createNoun(bool first_capital, bool singular,
                        const QCString &base,
                        const QCString &plurSuffix, const QCString &singSuffix = "" )
    {
      QCString result;
      if (first_capital)
      {
        QCString lc = getUTF8CharAt(base.str(),0);
        QCString uc = convertUTF8ToUpper(lc.str());
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
