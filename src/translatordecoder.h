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

#ifndef TRANSLATORDECODER_H
#define TRANSLATORDECODER_H

#include "translator.h"
#include "portable.h"

/** Decoder adapter for translator file that are not yet UTF-8 encoded. */
class TranslatorDecoder : public Translator
{
  public:

    TranslatorDecoder(Translator *translator) : m_translator(translator) 
    {
      m_toUtf8   = portable_iconv_open("UTF-8",translator->idLanguageCharset()); 
      m_fromUtf8 = portable_iconv_open(translator->idLanguageCharset(),"UTF-8"); 
    }
    virtual ~TranslatorDecoder() 
    { 
      if (m_toUtf8!=(void *)(-1))   portable_iconv_close(m_toUtf8);
      if (m_fromUtf8!=(void *)(-1)) portable_iconv_close(m_fromUtf8);
      delete m_translator; 
    }
   
    /*! This method is used to provide warning message that is displayed
     *  if the user chooses a language whose translation is not up to date. 
     *  It is implemented by the adapter classes.
     */
    virtual QCString updateNeededMessage() { return m_translator->updateNeededMessage(); }

    // Please, have a look at comments inside the translator_en.h file
    // to learn the meaning of the following methods.  The translator_en.h
    // file contains the TranslatorEnglish implementation, which is 
    // always up-to-date (by definition).
   
    // --- Language control methods -------------------
    
    QCString idLanguage()  
    { return m_translator->idLanguage(); }
    QCString latexLanguageSupportCommand()  
    { return m_translator->latexLanguageSupportCommand(); }
    QCString idLanguageCharset()  
    { return m_translator->idLanguageCharset(); }

    QCString recode(const QCString &s,void *cd) 
    {
      if (cd==(void*)(-1)) return s;
      int iSize        = s.length();
      int oSize        = iSize*4+1;
      QCString output(oSize);
      size_t iLeft     = iSize;
      size_t oLeft     = oSize;
      char *iPtr       = s.data();
      char *oPtr       = output.data();
      if (!portable_iconv(cd,&iPtr,&iLeft,&oPtr,&oLeft))
      {
        oSize -= oLeft;
        output.resize(oSize+1);
        output.at(oSize)='\0';
        return output;
      }
      else
      {
        return s;
      }
    }
    QCString toUtf8(const QCString &s) 
    {
      return recode(s,m_toUtf8);
    }
    QCString fromUtf8(const QCString &s) 
    {
      return recode(s,m_fromUtf8);
    }

    // --- Language translation methods -------------------

    QCString trRelatedFunctions()  
    { return toUtf8(m_translator->trRelatedFunctions()); }
    QCString trRelatedSubscript()  
    { return toUtf8(m_translator->trRelatedSubscript()); }
    QCString trDetailedDescription()  
    { return toUtf8(m_translator->trDetailedDescription()); }
    QCString trMemberTypedefDocumentation()  
    { return toUtf8(m_translator->trMemberTypedefDocumentation()); }
    QCString trMemberEnumerationDocumentation()  
    { return toUtf8(m_translator->trMemberEnumerationDocumentation()); }
        
    QCString trMemberFunctionDocumentation()  
    { return toUtf8(m_translator->trMemberFunctionDocumentation()); }
    QCString trMemberDataDocumentation()  
    { return toUtf8(m_translator->trMemberDataDocumentation()); }
    QCString trMore()  
    { return toUtf8(m_translator->trMore()); }
    QCString trListOfAllMembers()  
    { return toUtf8(m_translator->trListOfAllMembers()); }
    QCString trMemberList()  
    { return toUtf8(m_translator->trMemberList()); }
    QCString trThisIsTheListOfAllMembers()  
    { return toUtf8(m_translator->trThisIsTheListOfAllMembers()); }
    QCString trIncludingInheritedMembers()  
    { return toUtf8(m_translator->trIncludingInheritedMembers()); }
    QCString trGeneratedAutomatically(const char *s)  
    { return toUtf8(m_translator->trGeneratedAutomatically(fromUtf8(s))); }
    QCString trEnumName()  
    { return toUtf8(m_translator->trEnumName()); }
    QCString trEnumValue()  
    { return toUtf8(m_translator->trEnumValue()); }
    QCString trDefinedIn()  
    { return toUtf8(m_translator->trDefinedIn()); }

    // quick reference sections

    QCString trModules()  
    { return toUtf8(m_translator->trModules()); }
    QCString trClassHierarchy()  
    { return toUtf8(m_translator->trClassHierarchy()); }
    QCString trCompoundList()  
    { return toUtf8(m_translator->trCompoundList()); }
    
    QCString trFileList()  
    { return toUtf8(m_translator->trFileList()); }
    QCString trCompoundMembers()  
    { return toUtf8(m_translator->trCompoundMembers()); }
    QCString trFileMembers()  
    { return toUtf8(m_translator->trFileMembers()); }
    QCString trRelatedPages()  
    { return toUtf8(m_translator->trRelatedPages()); }
    QCString trExamples()  
    { return toUtf8(m_translator->trExamples()); }
    QCString trSearch()  
    { return toUtf8(m_translator->trSearch()); }
    QCString trClassHierarchyDescription()  
    { return toUtf8(m_translator->trClassHierarchyDescription()); }
    QCString trFileListDescription(bool extractAll)  
    { return toUtf8(m_translator->trFileListDescription(extractAll)); }
    QCString trCompoundListDescription()  
    { return toUtf8(m_translator->trCompoundListDescription()); }
    QCString trCompoundMembersDescription(bool extractAll)  
    { return toUtf8(m_translator->trCompoundMembersDescription(extractAll)); }
    QCString trFileMembersDescription(bool extractAll)  
    { return toUtf8(m_translator->trFileMembersDescription(extractAll)); }
    QCString trExamplesDescription()  
    { return toUtf8(m_translator->trExamplesDescription()); }
    QCString trRelatedPagesDescription()  
    { return toUtf8(m_translator->trRelatedPagesDescription()); }
    QCString trModulesDescription()  
    { return toUtf8(m_translator->trModulesDescription()); }
    
    // index titles (the project name is prepended for these) 

    QCString trDocumentation()  
    { return toUtf8(m_translator->trDocumentation()); }
    QCString trModuleIndex()  
    { return toUtf8(m_translator->trModuleIndex()); }
    QCString trHierarchicalIndex()  
    { return toUtf8(m_translator->trHierarchicalIndex()); }
    QCString trCompoundIndex()  
    { return toUtf8(m_translator->trCompoundIndex()); }
    QCString trFileIndex()  
    { return toUtf8(m_translator->trFileIndex()); } 
    QCString trModuleDocumentation()  
    { return toUtf8(m_translator->trModuleDocumentation()); }
    QCString trClassDocumentation()  
    { return toUtf8(m_translator->trClassDocumentation()); }
    QCString trFileDocumentation()  
    { return toUtf8(m_translator->trFileDocumentation()); }
    QCString trExampleDocumentation()  
    { return toUtf8(m_translator->trExampleDocumentation()); }
    QCString trPageDocumentation()  
    { return toUtf8(m_translator->trPageDocumentation()); }
    QCString trReferenceManual()  
    { return toUtf8(m_translator->trReferenceManual()); }
    QCString trDefines()  
    { return toUtf8(m_translator->trDefines()); }
    QCString trFuncProtos()  
    { return toUtf8(m_translator->trFuncProtos()); }
    QCString trTypedefs()  
    { return toUtf8(m_translator->trTypedefs()); }
    QCString trEnumerations()  
    { return toUtf8(m_translator->trEnumerations()); }
    QCString trFunctions()  
    { return toUtf8(m_translator->trFunctions()); }
    QCString trVariables()  
    { return toUtf8(m_translator->trVariables()); }
    QCString trEnumerationValues()  
    { return toUtf8(m_translator->trEnumerationValues()); }
    QCString trDefineDocumentation()  
    { return toUtf8(m_translator->trDefineDocumentation()); }
    QCString trFunctionPrototypeDocumentation()  
    { return toUtf8(m_translator->trFunctionPrototypeDocumentation()); }
    QCString trTypedefDocumentation()  
    { return toUtf8(m_translator->trTypedefDocumentation()); }
    QCString trEnumerationTypeDocumentation()  
    { return toUtf8(m_translator->trEnumerationTypeDocumentation()); }
    QCString trFunctionDocumentation()  
    { return toUtf8(m_translator->trFunctionDocumentation()); }
    QCString trVariableDocumentation()  
    { return toUtf8(m_translator->trVariableDocumentation()); }
    QCString trCompounds()  
    { return toUtf8(m_translator->trCompounds()); }
    QCString trGeneratedAt(const char *date,const char *projName)  
    { return toUtf8(m_translator->trGeneratedAt(fromUtf8(date),fromUtf8(projName))); }
    QCString trWrittenBy()  
    { return toUtf8(m_translator->trWrittenBy()); }
    QCString trClassDiagram(const char *clName)  
    { return toUtf8(m_translator->trClassDiagram(fromUtf8(clName))); }
    QCString trForInternalUseOnly()  
    { return toUtf8(m_translator->trForInternalUseOnly()); }
    QCString trWarning()  
    { return toUtf8(m_translator->trWarning()); }
    QCString trVersion()  
    { return toUtf8(m_translator->trVersion()); }
    QCString trDate()  
    { return toUtf8(m_translator->trDate()); }
    QCString trReturns()  
    { return toUtf8(m_translator->trReturns()); }
    QCString trSeeAlso()  
    { return toUtf8(m_translator->trSeeAlso()); }
    QCString trParameters()  
    { return toUtf8(m_translator->trParameters()); }
    QCString trExceptions()  
    { return toUtf8(m_translator->trExceptions()); }
    QCString trGeneratedBy()  
    { return toUtf8(m_translator->trGeneratedBy()); }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////
    
    QCString trNamespaceList()  
    { return toUtf8(m_translator->trNamespaceList()); }
    QCString trNamespaceListDescription(bool extractAll)  
    { return toUtf8(m_translator->trNamespaceListDescription(extractAll)); }
    QCString trFriends()  
    { return toUtf8(m_translator->trFriends()); }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
    QCString trRelatedFunctionDocumentation()  
    { return toUtf8(m_translator->trRelatedFunctionDocumentation()); }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    QCString trCompoundReference(const char *clName,
                                 ClassDef::CompoundType compType,
                                 bool isTemplate)  
    { return toUtf8(m_translator->trCompoundReference(fromUtf8(clName),compType,isTemplate)); }

    
    QCString trFileReference(const char *fileName)  
    { return toUtf8(m_translator->trFileReference(fromUtf8(fileName))); }
    QCString trNamespaceReference(const char *namespaceName)  
    { return toUtf8(m_translator->trNamespaceReference(fromUtf8(namespaceName))); }
    
    QCString trPublicMembers()  
    { return toUtf8(m_translator->trPublicMembers()); }
    QCString trPublicSlots()  
    { return toUtf8(m_translator->trPublicSlots()); }
    QCString trSignals()  
    { return toUtf8(m_translator->trSignals()); }
    QCString trStaticPublicMembers()  
    { return toUtf8(m_translator->trStaticPublicMembers()); }
    QCString trProtectedMembers()  
    { return toUtf8(m_translator->trProtectedMembers()); }
    QCString trProtectedSlots()  
    { return toUtf8(m_translator->trProtectedSlots()); }
    QCString trStaticProtectedMembers()  
    { return toUtf8(m_translator->trStaticProtectedMembers()); }
    QCString trPrivateMembers()  
    { return toUtf8(m_translator->trPrivateMembers()); }
    QCString trPrivateSlots()  
    { return toUtf8(m_translator->trPrivateSlots()); }
    QCString trStaticPrivateMembers()  
    { return toUtf8(m_translator->trStaticPrivateMembers()); }
    QCString trWriteList(int numEntries)  
    { return toUtf8(m_translator->trWriteList(numEntries)); }
    QCString trInheritsList(int numEntries)  
    { return toUtf8(m_translator->trInheritsList(numEntries)); }
    QCString trInheritedByList(int numEntries)  
    { return toUtf8(m_translator->trInheritedByList(numEntries)); }
    QCString trReimplementedFromList(int numEntries)  
    { return toUtf8(m_translator->trReimplementedFromList(numEntries)); }
    QCString trReimplementedInList(int numEntries)  
    { return toUtf8(m_translator->trReimplementedInList(numEntries)); }
    QCString trNamespaceMembers()  
    { return toUtf8(m_translator->trNamespaceMembers()); }
    QCString trNamespaceMemberDescription(bool extractAll)  
    { return toUtf8(m_translator->trNamespaceMemberDescription(extractAll)); }
    
    QCString trNamespaceIndex()  
    { return toUtf8(m_translator->trNamespaceIndex()); }
    QCString trNamespaceDocumentation()  
    { return toUtf8(m_translator->trNamespaceDocumentation()); }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    QCString trNamespaces()  
    { return toUtf8(m_translator->trNamespaces()); }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
                                          bool single)  
    { return toUtf8(m_translator->trGeneratedFromFiles(compType,single)); }
    
    
    QCString trAlphabeticalList()  
    { return toUtf8(m_translator->trAlphabeticalList()); }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    QCString trReturnValues()  
    { return toUtf8(m_translator->trReturnValues()); }
    QCString trMainPage()  
    { return toUtf8(m_translator->trMainPage()); }
    QCString trPageAbbreviation()  
    { return toUtf8(m_translator->trPageAbbreviation()); }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    QCString trDefinedAtLineInSourceFile()  
    { return toUtf8(m_translator->trDefinedAtLineInSourceFile()); }
    QCString trDefinedInSourceFile()  
    { return toUtf8(m_translator->trDefinedInSourceFile()); }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated()  
    { return toUtf8(m_translator->trDeprecated()); }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    QCString trCollaborationDiagram(const char *clName)  
    { return toUtf8(m_translator->trCollaborationDiagram(fromUtf8(clName))); }
    QCString trInclDepGraph(const char *fName)  
    { return toUtf8(m_translator->trInclDepGraph(fromUtf8(fName))); }
    QCString trConstructorDocumentation()  
    { return toUtf8(m_translator->trConstructorDocumentation()); }
    QCString trGotoSourceCode()  
    { return toUtf8(m_translator->trGotoSourceCode()); }
    QCString trGotoDocumentation()  
    { return toUtf8(m_translator->trGotoDocumentation()); }
    QCString trPrecondition()  
    { return toUtf8(m_translator->trPrecondition()); }
    QCString trPostcondition()  
    { return toUtf8(m_translator->trPostcondition()); }
    QCString trInvariant()  
    { return toUtf8(m_translator->trInvariant()); }
    QCString trInitialValue()  
    { return toUtf8(m_translator->trInitialValue()); }
    QCString trCode()  
    { return toUtf8(m_translator->trCode()); }

    QCString trGraphicalHierarchy()  
    { return toUtf8(m_translator->trGraphicalHierarchy()); }
    QCString trGotoGraphicalHierarchy()  
    { return toUtf8(m_translator->trGotoGraphicalHierarchy()); }
    QCString trGotoTextualHierarchy()  
    { return toUtf8(m_translator->trGotoTextualHierarchy()); }
    QCString trPageIndex()  
    { return toUtf8(m_translator->trPageIndex()); }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////
    
    QCString trNote()  
    { return toUtf8(m_translator->trNote()); }
    QCString trPublicTypes()  
    { return toUtf8(m_translator->trPublicTypes()); }
    QCString trPublicAttribs()  
    { return toUtf8(m_translator->trPublicAttribs()); }
    QCString trStaticPublicAttribs()  
    { return toUtf8(m_translator->trStaticPublicAttribs()); }
    QCString trProtectedTypes()  
    { return toUtf8(m_translator->trProtectedTypes()); }
    QCString trProtectedAttribs()  
    { return toUtf8(m_translator->trProtectedAttribs()); }
    QCString trStaticProtectedAttribs()  
    { return toUtf8(m_translator->trStaticProtectedAttribs()); }
    QCString trPrivateTypes()  
    { return toUtf8(m_translator->trPrivateTypes()); }
    QCString trPrivateAttribs()  
    { return toUtf8(m_translator->trPrivateAttribs()); }
    QCString trStaticPrivateAttribs()  
    { return toUtf8(m_translator->trStaticPrivateAttribs()); }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    QCString trTodo()  
    { return toUtf8(m_translator->trTodo()); }
    QCString trTodoList()  
    { return toUtf8(m_translator->trTodoList()); }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy()  
    { return toUtf8(m_translator->trReferencedBy()); }
    QCString trRemarks()  
    { return toUtf8(m_translator->trRemarks()); }
    QCString trAttention()  
    { return toUtf8(m_translator->trAttention()); }
    QCString trInclByDepGraph()  
    { return toUtf8(m_translator->trInclByDepGraph()); }
    QCString trSince()  
    { return toUtf8(m_translator->trSince()); }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    QCString trLegendTitle()  
    { return toUtf8(m_translator->trLegendTitle()); }
    QCString trLegendDocs()  
    { return toUtf8(m_translator->trLegendDocs()); }
    QCString trLegend()  
    { return toUtf8(m_translator->trLegend()); }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////
    
    QCString trTest()  
    { return toUtf8(m_translator->trTest()); }
    QCString trTestList()  
    { return toUtf8(m_translator->trTestList()); }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    QCString trDCOPMethods()  
    { return toUtf8(m_translator->trDCOPMethods()); }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    QCString trProperties()  
    { return toUtf8(m_translator->trProperties()); }
    QCString trPropertyDocumentation()  
    { return toUtf8(m_translator->trPropertyDocumentation()); }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    QCString trClasses()  
    { return toUtf8(m_translator->trClasses()); }
    QCString trPackage(const char *name)  
    { return toUtf8(m_translator->trPackage(fromUtf8(name))); }
    QCString trPackageList()  
    { return toUtf8(m_translator->trPackageList()); }
    QCString trPackageListDescription()  
    { return toUtf8(m_translator->trPackageListDescription()); }
    QCString trPackages()  
    { return toUtf8(m_translator->trPackages()); }
    QCString trDefineValue()  
    { return toUtf8(m_translator->trDefineValue()); }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////
    
    QCString trBug()  
    { return toUtf8(m_translator->trBug()); }
    QCString trBugList()  
    { return toUtf8(m_translator->trBugList()); }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.6
//////////////////////////////////////////////////////////////////////////

    QCString trRTFansicp()  
    { return toUtf8(m_translator->trRTFansicp()); }
    QCString trRTFCharSet()  
    { return toUtf8(m_translator->trRTFCharSet()); }
    QCString trRTFGeneralIndex()  
    { return toUtf8(m_translator->trRTFGeneralIndex()); }
   
    // Translation of the word

    QCString trClass(bool first_capital, bool singular)  
    { return toUtf8(m_translator->trClass(first_capital,singular)); }
    QCString trFile(bool first_capital, bool singular)  
    { return toUtf8(m_translator->trFile(first_capital,singular)); }
    QCString trNamespace(bool first_capital, bool singular)  
    { return toUtf8(m_translator->trNamespace(first_capital,singular)); }
    
    QCString trGroup(bool first_capital, bool singular)  
    { return toUtf8(m_translator->trGroup(first_capital,singular)); }
    QCString trPage(bool first_capital, bool singular)  
    { return toUtf8(m_translator->trPage(first_capital,singular)); }
    QCString trMember(bool first_capital, bool singular)  
    { return toUtf8(m_translator->trMember(first_capital,singular)); }
    QCString trGlobal(bool first_capital, bool singular)  
    { return toUtf8(m_translator->trGlobal(first_capital,singular)); }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////
        
    QCString trAuthor(bool first_capital, bool singular)  
    { return toUtf8(m_translator->trAuthor(first_capital,singular)); }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    QCString trReferences()  
    { return toUtf8(m_translator->trReferences()); }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    QCString trImplementedFromList(int numEntries)  
    { return toUtf8(m_translator->trImplementedFromList(numEntries)); }
    QCString trImplementedInList(int numEntries)  
    { return toUtf8(m_translator->trImplementedInList(numEntries)); }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    QCString trRTFTableOfContents()  
    { return toUtf8(m_translator->trRTFTableOfContents()); }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecatedList()  
    { return toUtf8(m_translator->trDeprecatedList()); }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    QCString trEvents()  
    { return toUtf8(m_translator->trEvents()); }
    QCString trEventDocumentation()  
    { return toUtf8(m_translator->trEventDocumentation()); }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    QCString trPackageTypes()  
    { return toUtf8(m_translator->trPackageTypes()); }
    QCString trPackageMembers()  
    { return toUtf8(m_translator->trPackageMembers()); }
    QCString trStaticPackageMembers()  
    { return toUtf8(m_translator->trStaticPackageMembers()); }
    QCString trPackageAttribs()  
    { return toUtf8(m_translator->trPackageAttribs()); }
    QCString trStaticPackageAttribs()  
    { return toUtf8(m_translator->trStaticPackageAttribs()); }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    QCString trAll()  
    { return toUtf8(m_translator->trAll()); }
    QCString trCallGraph()  
    { return toUtf8(m_translator->trCallGraph()); }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    QCString trSearchForIndex()  
    { return toUtf8(m_translator->trSearchForIndex()); }
    QCString trSearchResultsTitle()  
    { return toUtf8(m_translator->trSearchResultsTitle()); }
    QCString trSearchResults(int numDocuments)  
    { return toUtf8(m_translator->trSearchResults(numDocuments)); }
    QCString trSearchMatches()  
    { return toUtf8(m_translator->trSearchMatches()); }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    QCString trSourceFile(QCString& filename)  
    { return toUtf8(m_translator->trSourceFile(filename)); }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    QCString trDirIndex()  
    { return toUtf8(m_translator->trDirIndex()); }
    QCString trDirDocumentation()  
    { return toUtf8(m_translator->trDirDocumentation()); }
    QCString trDirectories()  
    { return toUtf8(m_translator->trDirectories()); }
    QCString trDirDescription()  
    { return toUtf8(m_translator->trDirDescription()); }
    QCString trDirReference(const char *dirName)  
    { return toUtf8(m_translator->trDirReference(fromUtf8(dirName))); }
    QCString trDir(bool first_capital, bool singular)  
    { return toUtf8(m_translator->trDir(first_capital,singular)); }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    QCString trOverloadText()  
    { return toUtf8(m_translator->trOverloadText()); }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////
    
    QCString trCallerGraph()  
    { return toUtf8(m_translator->trCallerGraph()); }
    QCString trEnumerationValueDocumentation()  
    { return toUtf8(m_translator->trEnumerationValueDocumentation()); }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4
//////////////////////////////////////////////////////////////////////////

    QCString trMemberFunctionDocumentationFortran()  
    { return toUtf8(m_translator->trMemberFunctionDocumentationFortran()); }
    QCString trCompoundListFortran()  
    { return toUtf8(m_translator->trCompoundListFortran()); }
    QCString trCompoundMembersFortran()  
    { return toUtf8(m_translator->trCompoundMembersFortran()); }
    QCString trCompoundListDescriptionFortran()  
    { return toUtf8(m_translator->trCompoundListDescriptionFortran()); }
    QCString trCompoundMembersDescriptionFortran(bool extractAll)  
    { return toUtf8(m_translator->trCompoundMembersDescriptionFortran(extractAll)); }
    QCString trCompoundIndexFortran()  
    { return toUtf8(m_translator->trCompoundIndexFortran()); }
    QCString trTypeDocumentation()  
    { return toUtf8(m_translator->trTypeDocumentation()); }
    QCString trSubprograms()  
    { return toUtf8(m_translator->trSubprograms()); }
    QCString trSubprogramDocumentation()  
    { return toUtf8(m_translator->trSubprogramDocumentation()); }
    QCString trDataTypes()  
    { return toUtf8(m_translator->trDataTypes()); }
    QCString trModulesList()  
    { return toUtf8(m_translator->trModulesList()); }
    QCString trModulesListDescription(bool extractAll)  
    { return toUtf8(m_translator->trModulesListDescription(extractAll)); }
    QCString trCompoundReferenceFortran(const char *clName,
                                 ClassDef::CompoundType compType,
                                 bool isTemplate)  
    { return toUtf8(m_translator->trCompoundReferenceFortran(fromUtf8(clName),compType,isTemplate)); }
    QCString trModuleReference(const char *namespaceName)  
    { return toUtf8(m_translator->trModuleReference(fromUtf8(namespaceName))); }
    QCString trModulesMembers()  
    { return toUtf8(m_translator->trModulesMembers()); }
    QCString trModulesMemberDescription(bool extractAll)  
    { return toUtf8(m_translator->trModulesMemberDescription(extractAll)); }
    QCString trModulesIndex()  
    { return toUtf8(m_translator->trModulesIndex()); }
    QCString trModule(bool first_capital, bool singular)  
    { return toUtf8(m_translator->trModule(first_capital,singular)); }
    QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
                                          bool single)  
    { return toUtf8(m_translator->trGeneratedFromFilesFortran(compType,single)); }
    QCString trType(bool first_capital, bool singular)  
    { return toUtf8(m_translator->trType(first_capital,singular)); }
    QCString trSubprogram(bool first_capital, bool singular)  
    { return toUtf8(m_translator->trSubprogram(first_capital,singular)); }
    QCString trTypeConstraints()
    { return toUtf8(m_translator->trTypeConstraints()); }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0
//////////////////////////////////////////////////////////////////////////

    QCString trDirRelation(const char *name)
    { return toUtf8(m_translator->trDirRelation(fromUtf8(name))); }
    QCString trLoading()
    { return toUtf8(m_translator->trLoading()); }
    QCString trGlobalNamespace()
    { return toUtf8(m_translator->trGlobalNamespace()); }
    QCString trSearching()
    { return toUtf8(m_translator->trSearching()); }
    QCString trNoMatches()
    { return toUtf8(m_translator->trNoMatches()); }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.3
//////////////////////////////////////////////////////////////////////////

    QCString trDirDependency(const char *name)
    { return toUtf8(m_translator->trDirDependency(fromUtf8(name))); }
    QCString trFileIn(const char *name)
    { return toUtf8(m_translator->trFileIn(fromUtf8(name))); }
    QCString trIncludesFileIn(const char *name)
    { return toUtf8(m_translator->trIncludesFileIn(fromUtf8(name))); }
    QCString trDateTime(int year,int month,int day,int dayOfWeek,
                        int hour,int minutes,int seconds,bool includeTime)
    { return toUtf8(m_translator->trDateTime(year,month,day,dayOfWeek,
                                             hour,minutes,seconds,includeTime)); 
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    QCString trCiteReferences()
    { return toUtf8(m_translator->trCiteReferences()); }
    QCString trCopyright()
    { return toUtf8(m_translator->trCopyright()); }
    QCString trDirDepGraph(const char *name)
    { return toUtf8(m_translator->trDirDepGraph(fromUtf8(name))); }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    QCString trDetailLevel()
    { return toUtf8(m_translator->trDetailLevel()); }
    QCString trTemplateParameters()
    { return toUtf8(m_translator->trTemplateParameters()); }
    QCString trAndMore(const QCString &number)
    { return toUtf8(m_translator->trAndMore(fromUtf8(number))); }
    QCString trEnumGeneratedFromFiles(bool single)
    { return toUtf8(m_translator->trEnumGeneratedFromFiles(single)); }
    QCString trEnumReference(const char *name)
    { return toUtf8(m_translator->trEnumReference(fromUtf8(name))); }
    QCString trInheritedFrom(const char *members,const char *what)
    { return toUtf8(m_translator->trInheritedFrom(fromUtf8(members),fromUtf8(what))); }
    QCString trAdditionalInheritedMembers()
    { return toUtf8(m_translator->trAdditionalInheritedMembers()); }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    QCString trPanelSynchronisationTooltip(bool enable)
    { return toUtf8(m_translator->trPanelSynchronisationTooltip(enable)); }
    QCString trProvidedByCategory()
    { return toUtf8(m_translator->trProvidedByCategory()); }
    QCString trExtendsClass()
    { return toUtf8(m_translator->trExtendsClass()); }
    QCString trClassMethods()
    { return toUtf8(m_translator->trClassMethods()); }
    QCString trInstanceMethods()
    { return toUtf8(m_translator->trInstanceMethods()); }
    QCString trMethodDocumentation()
    { return toUtf8(m_translator->trMethodDocumentation()); }
    QCString trDesignOverview()
    { return toUtf8(m_translator->trDesignOverview()); }

//////////////////////////////////////////////////////////////////////////
  private:
    Translator *m_translator;
    void *m_toUtf8;
    void *m_fromUtf8;

};

#endif
