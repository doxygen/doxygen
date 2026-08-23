#ifndef TRANSLATOR_ADAPTER_H
#define TRANSLATOR_ADAPTER_H

#include "translator.h"
#include "version.h"

/** Base of the translator adapter tree.
 *
 *  This abstract class provides access to the english
 *  translations, to be used as a substitute for not implemented
 *  local translations.
 */
class TranslatorAdapterBase : public Translator
{
  protected:
    TranslatorEnglish english;

    /*! An auxiliary inline method used by the updateNeededMessage()
     *  for building a warning message.
     */
    inline DString createUpdateNeededMessage(const DString & languageName,
                                              const DString & versionString)
    {
      return DString("The selected output language \"")
             + languageName
             + "\" has not been updated\nsince "
             + versionString
             + ".  As a result some sentences may appear in English.\n\n";
    }
  public:
    /*! This method is used to generate a warning message to signal
     *  the user that the translation of his/her language of choice
     *  needs updating.  It must be implemented by the translator
     *  adapter class (pure virtual).
     *
     *  \sa createUpdateNeededMessage()
     */
    virtual DString updateNeededMessage() override = 0;
};

class TranslatorAdapter_1_16_0 : public TranslatorAdapterBase
{
  public:
    DString updateNeededMessage() override
    { return createUpdateNeededMessage(idLanguage(),"release 1.16.0"); }
    DString trRequirements() override
    { return english.trRequirements(); }
    DString trRequirementID() override
    { return english.trRequirementID(); }
    DString trSatisfies(bool singular) override
    { return english.trSatisfies(singular); }
    DString trSatisfiedBy(const DString &list) override
    { return english.trSatisfiedBy(list); }
    DString trUnsatisfiedRequirements() override
    { return english.trUnsatisfiedRequirements(); }
    DString trUnsatisfiedRequirementsText(bool singular,const DString &list) override
    { return english.trUnsatisfiedRequirementsText(singular,list); }
    DString trVerifies(bool singular) override
    { return english.trVerifies(singular); }
    DString trVerifiedBy(const DString &list) override
    { return english.trVerifiedBy(list); }
    DString trUnverifiedRequirements() override
    { return english.trUnverifiedRequirements(); }
    DString trUnverifiedRequirementsText(bool singular,const DString &list) override
    { return english.trUnverifiedRequirementsText(singular,list); }
};

class TranslatorAdapter_1_11_0 : public TranslatorAdapter_1_16_0
{
  public:
    DString updateNeededMessage() override
    { return createUpdateNeededMessage(idLanguage(),"release 1.11.0"); }
    DString trImportant() override
    { return english.trImportant(); }
};

class TranslatorAdapter_1_10_0 : public TranslatorAdapter_1_11_0
{
  public:
    DString updateNeededMessage() override
    { return createUpdateNeededMessage(idLanguage(),"release 1.10.0"); }
    DString trCopyToClipboard() override
    { return english.trCopyToClipboard(); }
};

class TranslatorAdapter_1_9_8 : public TranslatorAdapter_1_10_0
{
  public:
    DString updateNeededMessage() override
    { return createUpdateNeededMessage(idLanguage(),"release 1.9.8"); }

    DString trTopics() override
    { return english.trTopics(); }
    DString trTopicDocumentation() override
    { return english.trTopicDocumentation(); }
    DString trTopicList() override
    { return english.trTopicList(); }
    DString trTopicIndex() override
    { return english.trTopicIndex(); }
    DString trTopicListDescription() override
    { return english.trTopicListDescription(); }
    DString trModuleMembersDescriptionTotal(ModuleMemberHighlight::Enum hl) override
    { return english.trModuleMembersDescriptionTotal(hl); }
    DString trExportedModules() override
    { return english.trExportedModules(); }

};

class TranslatorAdapter_1_9_6 : public TranslatorAdapter_1_9_8
{
  public:
    DString updateNeededMessage() override
    { return createUpdateNeededMessage(idLanguage(),"release 1.9.6"); }

    /*! Small trick to use the original functions as the wording has been changed */
    DString trRelatedSymbols() override
    { return trRelatedFunctions(); }
    DString trRelatedSymbolsSubscript() override
    { return trRelatedSubscript(); }
    DString trRelatedSymbolDocumentation() override
    { return trRelatedFunctionDocumentation(); }

    DString trCompoundType(ClassDef::CompoundType compType, SrcLangExt lang) override
    { return english.trCompoundType(compType, lang); }

    DString trFileMembersDescriptionTotal(FileMemberHighlight::Enum hl) override
    {
      if (hl==FileMemberHighlight::All)
      {
        return trFileMembersDescription(Config_getBool(EXTRACT_ALL));
      }
      else
      {
        // hack to work around a mozilla bug, which refuses to switch to
        // normal lists otherwise
        return "&nbsp;";
      }
    }
    DString trCompoundMembersDescriptionTotal(ClassMemberHighlight::Enum hl) override
    {
      if (hl==ClassMemberHighlight::All)
      {
        return trCompoundMembersDescription(Config_getBool(EXTRACT_ALL));
      }
      else
      {
        // hack to work around a mozilla bug, which refuses to switch to
        // normal lists otherwise
        return "&nbsp;";
      }
    }
    DString trNamespaceMembersDescriptionTotal(NamespaceMemberHighlight::Enum hl) override
    {
      if (hl==NamespaceMemberHighlight::All)
      {
        return trNamespaceMemberDescription(Config_getBool(EXTRACT_ALL));
      }
      else
      {
        // hack to work around a mozilla bug, which refuses to switch to
        // normal lists otherwise
        return "&nbsp;";
      }
    }

    DString trDefinition() override
    { return english.trDefinition(); }
    DString trDeclaration() override
    { return english.trDeclaration(); }
};

class TranslatorAdapter_1_9_5 : public TranslatorAdapter_1_9_6
{
  public:
    DString updateNeededMessage() override
    { return createUpdateNeededMessage(idLanguage(),"release 1.9.5"); }

    DString trFlowchart() override
    { return english.trFlowchart(); }
};

class TranslatorAdapter_1_9_4 : public TranslatorAdapter_1_9_5
{
  public:
    DString updateNeededMessage() override
    { return createUpdateNeededMessage(idLanguage(),"release 1.9.4"); }

    DString trPackageList() override
    { return english.trPackageList(); }
};

class TranslatorAdapter_1_9_2 : public TranslatorAdapter_1_9_4
{
  public:
    DString updateNeededMessage() override
    { return createUpdateNeededMessage(idLanguage(),"release 1.9.2"); }

    DString trConcept(bool first_capital,bool singular) override
    { return english.trConcept(first_capital,singular); }

    DString trConceptReference(const DString &conceptName) override
    { return english.trConceptReference(conceptName); }

    DString trConceptList() override
    { return english.trConceptList(); }

    DString trConceptIndex() override
    { return english.trConceptIndex(); }

    DString trConceptDocumentation() override
    { return english.trConceptDocumentation(); }

    DString trConceptListDescription(bool extractAll) override
    { return english.trConceptListDescription(extractAll); }

    DString trConceptDefinition() override
    { return english.trConceptDefinition(); }
};

class TranslatorAdapter_1_8_19 : public TranslatorAdapter_1_9_2
{
  public:
    DString updateNeededMessage() override
    { return createUpdateNeededMessage(idLanguage(),"release 1.8.19"); }

    DString trDesignUnitDocumentation() override
    { return english.trDesignUnitDocumentation(); }
};

class TranslatorAdapter_1_8_15 : public TranslatorAdapter_1_8_19
{
  public:
    DString updateNeededMessage() override
    { return createUpdateNeededMessage(idLanguage(),"release 1.8.15"); }

    DString trDesignUnitHierarchy() override
    { return english.trDesignUnitHierarchy(); }

    DString trDesignUnitList() override
    { return english.trDesignUnitList(); }

    DString trDesignUnitMembers() override
    { return english.trDesignUnitMembers(); }

    DString trDesignUnitListDescription() override
    { return english.trDesignUnitListDescription(); }

    DString trDesignUnitIndex() override
    { return english.trDesignUnitIndex(); }

    DString trDesignUnits() override
    { return english.trDesignUnits(); }

    DString trFunctionAndProc() override
    { return english.trFunctionAndProc(); }

    DString trVhdlType(VhdlSpecifier type,bool single) override
    { return english.trVhdlType(type,single); }

    DString trCustomReference(const DString &name) override
    { return english.trCustomReference(name); }

    DString trConstants() override
    { return english.trConstants(); }

    DString trConstantDocumentation() override
    { return english.trConstantDocumentation(); }

    DString trSequences() override
    { return english.trSequences(); }

    DString trSequenceDocumentation() override
    { return english.trSequenceDocumentation(); }

    DString trDictionaries() override
    { return english.trDictionaries(); }

    DString trDictionaryDocumentation() override
    { return english.trDictionaryDocumentation(); }

    DString trSliceInterfaces() override
    { return english.trSliceInterfaces(); }

    DString trInterfaceIndex() override
    { return english.trInterfaceIndex(); }

    DString trInterfaceList() override
    { return english.trInterfaceList(); }

    DString trInterfaceListDescription() override
    { return english.trInterfaceListDescription(); }

    DString trInterfaceHierarchy() override
    { return english.trInterfaceHierarchy(); }

    DString trInterfaceHierarchyDescription() override
    { return english.trInterfaceHierarchyDescription(); }

    DString trInterfaceDocumentation() override
    { return english.trInterfaceDocumentation(); }

    DString trStructs() override
    { return english.trStructs(); }

    DString trStructIndex() override
    { return english.trStructIndex(); }

    DString trStructList() override
    { return english.trStructList(); }

    DString trStructListDescription() override
    { return english.trStructListDescription(); }

    DString trStructDocumentation() override
    { return english.trStructDocumentation(); }

    DString trExceptionIndex() override
    { return english.trExceptionIndex(); }

    DString trExceptionList() override
    { return english.trExceptionList(); }

    DString trExceptionListDescription() override
    { return english.trExceptionListDescription(); }

    DString trExceptionHierarchy() override
    { return english.trExceptionHierarchy(); }

    DString trExceptionHierarchyDescription() override
    { return english.trExceptionHierarchyDescription(); }

    DString trExceptionDocumentation() override
    { return english.trExceptionDocumentation(); }

    DString trCompoundReferenceSlice(const DString &clName, ClassDef::CompoundType compType, bool isLocal) override
    { return english.trCompoundReferenceSlice(clName,compType,isLocal); }

    DString trOperations() override
    { return english.trOperations(); }

    DString trOperationDocumentation() override
    { return english.trOperationDocumentation(); }

    DString trDataMembers() override
    { return english.trDataMembers(); }

    DString trDataMemberDocumentation() override
    { return english.trDataMemberDocumentation(); }
};

class TranslatorAdapter_1_8_4 : public TranslatorAdapter_1_8_15
{
  public:
    DString updateNeededMessage() override
    { return createUpdateNeededMessage(idLanguage(),"release 1.8.4"); }

    DString trInterfaces() override
    { return english.trInterfaces(); }

    DString trServices() override
    { return english.trServices(); }

    DString trConstantGroups() override
    { return english.trConstantGroups(); }

    DString trConstantGroupReference(const DString &namespaceName) override
    { return english.trConstantGroupReference(namespaceName); }

    DString trServiceReference(const DString &sName) override
    { return english.trServiceReference(sName); }

    DString trSingletonReference(const DString &sName) override
    { return english.trSingletonReference(sName); }

    DString trServiceGeneratedFromFiles(bool single) override
    { return english.trServiceGeneratedFromFiles(single); }

    DString trSingletonGeneratedFromFiles(bool single) override
    { return english.trSingletonGeneratedFromFiles(single); }
};

class TranslatorAdapter_1_8_2 : public TranslatorAdapter_1_8_4
{
  public:
    DString updateNeededMessage() override
    { return createUpdateNeededMessage(idLanguage(),"release 1.8.2"); }

    DString trPanelSynchronisationTooltip(bool enable) override
    { return english.trPanelSynchronisationTooltip(enable); }

    DString trProvidedByCategory() override
    { return english.trProvidedByCategory(); }

    DString trExtendsClass() override
    { return english.trExtendsClass(); }

    DString trClassMethods() override
    { return english.trClassMethods(); }

    DString trInstanceMethods() override
    { return english.trInstanceMethods(); }

    DString trMethodDocumentation() override
    { return english.trMethodDocumentation(); }
};


/** Adapter class for languages that only contain translations up to
 *  version 1.8.0.
 */
class TranslatorAdapter_1_8_0 : public TranslatorAdapter_1_8_2
{
  public:
    DString updateNeededMessage() override
    { return createUpdateNeededMessage(idLanguage(),"release 1.8.0"); }

    DString trDetailLevel() override
    { return english.trDetailLevel(); }

    DString trTemplateParameters() override
    { return english.trTemplateParameters(); }

    DString trAndMore(const DString &number) override
    { return english.trAndMore(number); }

    DString trEnumGeneratedFromFiles(bool single) override
    { return english.trEnumGeneratedFromFiles(single); }

    DString trEnumReference(const DString &name) override
    { return english.trEnumReference(name); }

    DString trInheritedFrom(const DString &members,const DString &what) override
    { return english.trInheritedFrom(members,what); }

    DString trAdditionalInheritedMembers() override
    { return english.trAdditionalInheritedMembers(); }

};

/** Adapter class for languages that only contain translations up to
 *  version 1.7.5.
 */
class TranslatorAdapter_1_7_5 : public TranslatorAdapter_1_8_0
{
  public:
    DString updateNeededMessage() override
    { return createUpdateNeededMessage(idLanguage(),"release 1.7.5"); }

    DString trCiteReferences() override
    { return english.trCiteReferences(); }

    DString trCopyright() override
    { return english.trCopyright(); }

    DString trDirDepGraph(const DString &name) override
    { return english.trDirDepGraph(name); }

    DString trFileIn(const DString &name) override
    { return english.trFileIn(name); }
    DString trIncludesFileIn(const DString &name) override
    { return english.trIncludesFileIn(name); }
    DString trDateTime(int year,int month,int day,int dayOfWeek,
                                int hour,int minutes,int seconds,
                                DateTimeType includeTime) override
    { return english.trDateTime(year,month,day,dayOfWeek,hour,minutes,seconds,includeTime); }
    DString trDayPeriod(bool period) override
    { return english.trDayPeriod(period); }

};

/** Adapter class for languages that only contain translations up to
 *  version 1.6.0.
 */
class TranslatorAdapter_1_6_0 : public TranslatorAdapter_1_7_5
{
  public:
    DString updateNeededMessage() override
    { return createUpdateNeededMessage(idLanguage(),"release 1.6.0"); }

    DString trDirRelation(const DString &name) override
    { return english.trDirRelation(name); }

    DString trLoading() override
    { return english.trLoading(); }

    DString trGlobalNamespace() override
    { return english.trGlobalNamespace(); }

    DString trSearching() override
    { return english.trSearching(); }

    DString trNoMatches() override
    { return english.trNoMatches(); }

    DString trMemberFunctionDocumentationFortran() override
    { return english.trMemberFunctionDocumentationFortran(); }

    DString trCompoundListFortran() override
    { return english.trCompoundListFortran(); }

    DString trCompoundMembersFortran() override
    { return english.trCompoundMembersFortran(); }

    DString trCompoundListDescriptionFortran() override
    { return english.trCompoundListDescriptionFortran(); }

    DString trCompoundMembersDescriptionFortran(bool extractAll) override
    { return english.trCompoundMembersDescriptionFortran(extractAll); }

    DString trCompoundIndexFortran() override
    { return english.trCompoundIndexFortran(); }

    DString trTypeDocumentation() override
    { return english.trTypeDocumentation(); }

    DString trSubprograms() override
    { return english.trSubprograms(); }

    DString trSubprogramDocumentation() override
    { return english.trSubprogramDocumentation(); }

    DString trDataTypes() override
    { return english.trDataTypes(); }

    DString trModulesList() override
    { return english.trModulesList(); }

    DString trModulesListDescription(bool extractAll) override
    { return english.trModulesListDescription(extractAll); }

    DString trCompoundReferenceFortran(const DString &clName,
                                 ClassDef::CompoundType compType,
                                 bool isTemplate) override
    { return english.trCompoundReferenceFortran(clName,compType,isTemplate); }

    DString trModuleReference(const DString &namespaceName) override
    { return english.trModuleReference(namespaceName); }

    DString trModulesMembers() override
    { return english.trModulesMembers(); }

    DString trModulesMemberDescription(bool extractAll) override
    { return english.trModulesMemberDescription(extractAll); }

    DString trModulesIndex() override
    { return english.trModulesIndex(); }

    DString trModule(bool first_capital, bool singular) override
    { return english.trModule(first_capital,singular); }

    DString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
                     bool single) override
    { return english.trGeneratedFromFilesFortran(compType,single); }

    DString trType(bool first_capital, bool singular) override
    { return english.trType(first_capital,singular); }

    DString trSubprogram(bool first_capital, bool singular) override
    { return english.trSubprogram(first_capital,singular); }

    DString trTypeConstraints() override
    { return english.trTypeConstraints(); }
};

/** Adapter class for languages that only contain translations up to
 *  version 1.4.6
 */
class TranslatorAdapter_1_4_6 : public TranslatorAdapter_1_6_0
{
  public:
    DString updateNeededMessage() override
    { return createUpdateNeededMessage(idLanguage(),"release 1.4.6"); }
    DString trCallerGraph() override
    { return english.trCallerGraph(); }
    DString trEnumerationValueDocumentation() override
    { return english.trEnumerationValueDocumentation(); }
};

#endif
