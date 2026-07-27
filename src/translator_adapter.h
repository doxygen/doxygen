#ifndef TRANSLATOR_ADAPTER_H
#define TRANSLATOR_ADAPTER_H

#include "version.h"
#include "translator.h"

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
    inline QCString createUpdateNeededMessage(const QCString & languageName,
                                              const QCString & versionString)
    {
      return QCString("The selected output language \"")
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
    virtual QCString updateNeededMessage() override = 0;
};

class TranslatorAdapter_1_16_0 : public TranslatorAdapterBase
{
  public:
    QCString updateNeededMessage() override
    { return createUpdateNeededMessage(idLanguage(),"release 1.16.0"); }
    QCString trRequirements() override
    { return english.trRequirements(); }
    QCString trRequirementID() override
    { return english.trRequirementID(); }
    QCString trSatisfies(bool singular) override
    { return english.trSatisfies(singular); }
    QCString trSatisfiedBy(const QCString &list) override
    { return english.trSatisfiedBy(list); }
    QCString trUnsatisfiedRequirements() override
    { return english.trUnsatisfiedRequirements(); }
    QCString trUnsatisfiedRequirementsText(bool singular,const QCString &list) override
    { return english.trUnsatisfiedRequirementsText(singular,list); }
    QCString trVerifies(bool singular) override
    { return english.trVerifies(singular); }
    QCString trVerifiedBy(const QCString &list) override
    { return english.trVerifiedBy(list); }
    QCString trUnverifiedRequirements() override
    { return english.trUnverifiedRequirements(); }
    QCString trUnverifiedRequirementsText(bool singular,const QCString &list) override
    { return english.trUnverifiedRequirementsText(singular,list); }
};

class TranslatorAdapter_1_11_0 : public TranslatorAdapter_1_16_0
{
  public:
    QCString updateNeededMessage() override
    { return createUpdateNeededMessage(idLanguage(),"release 1.11.0"); }
    QCString trImportant() override
    { return english.trImportant(); }
};

class TranslatorAdapter_1_10_0 : public TranslatorAdapter_1_11_0
{
  public:
    QCString updateNeededMessage() override
    { return createUpdateNeededMessage(idLanguage(),"release 1.10.0"); }
    QCString trCopyToClipboard() override
    { return english.trCopyToClipboard(); }
};

class TranslatorAdapter_1_9_8 : public TranslatorAdapter_1_10_0
{
  public:
    QCString updateNeededMessage() override
    { return createUpdateNeededMessage(idLanguage(),"release 1.9.8"); }

    QCString trTopics() override
    { return english.trTopics(); }
    QCString trTopicDocumentation() override
    { return english.trTopicDocumentation(); }
    QCString trTopicList() override
    { return english.trTopicList(); }
    QCString trTopicIndex() override
    { return english.trTopicIndex(); }
    QCString trTopicListDescription() override
    { return english.trTopicListDescription(); }
    QCString trModuleMembersDescriptionTotal(ModuleMemberHighlight::Enum hl) override
    { return english.trModuleMembersDescriptionTotal(hl); }
    QCString trExportedModules() override
    { return english.trExportedModules(); }

};

class TranslatorAdapter_1_9_6 : public TranslatorAdapter_1_9_8
{
  public:
    QCString updateNeededMessage() override
    { return createUpdateNeededMessage(idLanguage(),"release 1.9.6"); }

    /*! Small trick to use the original functions as the wording has been changed */
    QCString trRelatedSymbols() override
    { return trRelatedFunctions(); }
    QCString trRelatedSymbolsSubscript() override
    { return trRelatedSubscript(); }
    QCString trRelatedSymbolDocumentation() override
    { return trRelatedFunctionDocumentation(); }

    QCString trCompoundType(ClassDef::CompoundType compType, SrcLangExt lang) override
    { return english.trCompoundType(compType, lang); }

    QCString trFileMembersDescriptionTotal(FileMemberHighlight::Enum hl) override
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
    QCString trCompoundMembersDescriptionTotal(ClassMemberHighlight::Enum hl) override
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
    QCString trNamespaceMembersDescriptionTotal(NamespaceMemberHighlight::Enum hl) override
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

    QCString trDefinition() override
    { return english.trDefinition(); }
    QCString trDeclaration() override
    { return english.trDeclaration(); }
};

class TranslatorAdapter_1_9_5 : public TranslatorAdapter_1_9_6
{
  public:
    QCString updateNeededMessage() override
    { return createUpdateNeededMessage(idLanguage(),"release 1.9.5"); }

    QCString trFlowchart() override
    { return english.trFlowchart(); }
};

class TranslatorAdapter_1_9_4 : public TranslatorAdapter_1_9_5
{
  public:
    QCString updateNeededMessage() override
    { return createUpdateNeededMessage(idLanguage(),"release 1.9.4"); }

    QCString trPackageList() override
    { return english.trPackageList(); }
};

class TranslatorAdapter_1_9_2 : public TranslatorAdapter_1_9_4
{
  public:
    QCString updateNeededMessage() override
    { return createUpdateNeededMessage(idLanguage(),"release 1.9.2"); }

    QCString trConcept(bool first_capital,bool singular) override
    { return english.trConcept(first_capital,singular); }

    QCString trConceptReference(const QCString &conceptName) override
    { return english.trConceptReference(conceptName); }

    QCString trConceptList() override
    { return english.trConceptList(); }

    QCString trConceptIndex() override
    { return english.trConceptIndex(); }

    QCString trConceptDocumentation() override
    { return english.trConceptDocumentation(); }

    QCString trConceptListDescription(bool extractAll) override
    { return english.trConceptListDescription(extractAll); }

    QCString trConceptDefinition() override
    { return english.trConceptDefinition(); }
};

class TranslatorAdapter_1_8_19 : public TranslatorAdapter_1_9_2
{
  public:
    QCString updateNeededMessage() override
    { return createUpdateNeededMessage(idLanguage(),"release 1.8.19"); }

    QCString trDesignUnitDocumentation() override
    { return english.trDesignUnitDocumentation(); }
};

class TranslatorAdapter_1_8_15 : public TranslatorAdapter_1_8_19
{
  public:
    QCString updateNeededMessage() override
    { return createUpdateNeededMessage(idLanguage(),"release 1.8.15"); }

    QCString trDesignUnitHierarchy() override
    { return english.trDesignUnitHierarchy(); }

    QCString trDesignUnitList() override
    { return english.trDesignUnitList(); }

    QCString trDesignUnitMembers() override
    { return english.trDesignUnitMembers(); }

    QCString trDesignUnitListDescription() override
    { return english.trDesignUnitListDescription(); }

    QCString trDesignUnitIndex() override
    { return english.trDesignUnitIndex(); }

    QCString trDesignUnits() override
    { return english.trDesignUnits(); }

    QCString trFunctionAndProc() override
    { return english.trFunctionAndProc(); }

    QCString trVhdlType(VhdlSpecifier type,bool single) override
    { return english.trVhdlType(type,single); }

    QCString trCustomReference(const QCString &name) override
    { return english.trCustomReference(name); }

    QCString trConstants() override
    { return english.trConstants(); }

    QCString trConstantDocumentation() override
    { return english.trConstantDocumentation(); }

    QCString trSequences() override
    { return english.trSequences(); }

    QCString trSequenceDocumentation() override
    { return english.trSequenceDocumentation(); }

    QCString trDictionaries() override
    { return english.trDictionaries(); }

    QCString trDictionaryDocumentation() override
    { return english.trDictionaryDocumentation(); }

    QCString trSliceInterfaces() override
    { return english.trSliceInterfaces(); }

    QCString trInterfaceIndex() override
    { return english.trInterfaceIndex(); }

    QCString trInterfaceList() override
    { return english.trInterfaceList(); }

    QCString trInterfaceListDescription() override
    { return english.trInterfaceListDescription(); }

    QCString trInterfaceHierarchy() override
    { return english.trInterfaceHierarchy(); }

    QCString trInterfaceHierarchyDescription() override
    { return english.trInterfaceHierarchyDescription(); }

    QCString trInterfaceDocumentation() override
    { return english.trInterfaceDocumentation(); }

    QCString trStructs() override
    { return english.trStructs(); }

    QCString trStructIndex() override
    { return english.trStructIndex(); }

    QCString trStructList() override
    { return english.trStructList(); }

    QCString trStructListDescription() override
    { return english.trStructListDescription(); }

    QCString trStructDocumentation() override
    { return english.trStructDocumentation(); }

    QCString trExceptionIndex() override
    { return english.trExceptionIndex(); }

    QCString trExceptionList() override
    { return english.trExceptionList(); }

    QCString trExceptionListDescription() override
    { return english.trExceptionListDescription(); }

    QCString trExceptionHierarchy() override
    { return english.trExceptionHierarchy(); }

    QCString trExceptionHierarchyDescription() override
    { return english.trExceptionHierarchyDescription(); }

    QCString trExceptionDocumentation() override
    { return english.trExceptionDocumentation(); }

    QCString trCompoundReferenceSlice(const QCString &clName, ClassDef::CompoundType compType, bool isLocal) override
    { return english.trCompoundReferenceSlice(clName,compType,isLocal); }

    QCString trOperations() override
    { return english.trOperations(); }

    QCString trOperationDocumentation() override
    { return english.trOperationDocumentation(); }

    QCString trDataMembers() override
    { return english.trDataMembers(); }

    QCString trDataMemberDocumentation() override
    { return english.trDataMemberDocumentation(); }
};

class TranslatorAdapter_1_8_4 : public TranslatorAdapter_1_8_15
{
  public:
    QCString updateNeededMessage() override
    { return createUpdateNeededMessage(idLanguage(),"release 1.8.4"); }

    QCString trInterfaces() override
    { return english.trInterfaces(); }

    QCString trServices() override
    { return english.trServices(); }

    QCString trConstantGroups() override
    { return english.trConstantGroups(); }

    QCString trConstantGroupReference(const QCString &namespaceName) override
    { return english.trConstantGroupReference(namespaceName); }

    QCString trServiceReference(const QCString &sName) override
    { return english.trServiceReference(sName); }

    QCString trSingletonReference(const QCString &sName) override
    { return english.trSingletonReference(sName); }

    QCString trServiceGeneratedFromFiles(bool single) override
    { return english.trServiceGeneratedFromFiles(single); }

    QCString trSingletonGeneratedFromFiles(bool single) override
    { return english.trSingletonGeneratedFromFiles(single); }
};

class TranslatorAdapter_1_8_2 : public TranslatorAdapter_1_8_4
{
  public:
    QCString updateNeededMessage() override
    { return createUpdateNeededMessage(idLanguage(),"release 1.8.2"); }

    QCString trPanelSynchronisationTooltip(bool enable) override
    { return english.trPanelSynchronisationTooltip(enable); }

    QCString trProvidedByCategory() override
    { return english.trProvidedByCategory(); }

    QCString trExtendsClass() override
    { return english.trExtendsClass(); }

    QCString trClassMethods() override
    { return english.trClassMethods(); }

    QCString trInstanceMethods() override
    { return english.trInstanceMethods(); }

    QCString trMethodDocumentation() override
    { return english.trMethodDocumentation(); }
};


/** Adapter class for languages that only contain translations up to
 *  version 1.8.0.
 */
class TranslatorAdapter_1_8_0 : public TranslatorAdapter_1_8_2
{
  public:
    QCString updateNeededMessage() override
    { return createUpdateNeededMessage(idLanguage(),"release 1.8.0"); }

    QCString trDetailLevel() override
    { return english.trDetailLevel(); }

    QCString trTemplateParameters() override
    { return english.trTemplateParameters(); }

    QCString trAndMore(const QCString &number) override
    { return english.trAndMore(number); }

    QCString trEnumGeneratedFromFiles(bool single) override
    { return english.trEnumGeneratedFromFiles(single); }

    QCString trEnumReference(const QCString &name) override
    { return english.trEnumReference(name); }

    QCString trInheritedFrom(const QCString &members,const QCString &what) override
    { return english.trInheritedFrom(members,what); }

    QCString trAdditionalInheritedMembers() override
    { return english.trAdditionalInheritedMembers(); }

};

/** Adapter class for languages that only contain translations up to
 *  version 1.7.5.
 */
class TranslatorAdapter_1_7_5 : public TranslatorAdapter_1_8_0
{
  public:
    QCString updateNeededMessage() override
    { return createUpdateNeededMessage(idLanguage(),"release 1.7.5"); }

    QCString trCiteReferences() override
    { return english.trCiteReferences(); }

    QCString trCopyright() override
    { return english.trCopyright(); }

    QCString trDirDepGraph(const QCString &name) override
    { return english.trDirDepGraph(name); }

    QCString trFileIn(const QCString &name) override
    { return english.trFileIn(name); }
    QCString trIncludesFileIn(const QCString &name) override
    { return english.trIncludesFileIn(name); }
    QCString trDateTime(int year,int month,int day,int dayOfWeek,
                                int hour,int minutes,int seconds,
                                DateTimeType includeTime) override
    { return english.trDateTime(year,month,day,dayOfWeek,hour,minutes,seconds,includeTime); }
    QCString trDayPeriod(bool period) override
    { return english.trDayPeriod(period); }

};

/** Adapter class for languages that only contain translations up to
 *  version 1.6.0.
 */
class TranslatorAdapter_1_6_0 : public TranslatorAdapter_1_7_5
{
  public:
    QCString updateNeededMessage() override
    { return createUpdateNeededMessage(idLanguage(),"release 1.6.0"); }

    QCString trDirRelation(const QCString &name) override
    { return english.trDirRelation(name); }

    QCString trLoading() override
    { return english.trLoading(); }

    QCString trGlobalNamespace() override
    { return english.trGlobalNamespace(); }

    QCString trSearching() override
    { return english.trSearching(); }

    QCString trNoMatches() override
    { return english.trNoMatches(); }

    QCString trMemberFunctionDocumentationFortran() override
    { return english.trMemberFunctionDocumentationFortran(); }

    QCString trCompoundListFortran() override
    { return english.trCompoundListFortran(); }

    QCString trCompoundMembersFortran() override
    { return english.trCompoundMembersFortran(); }

    QCString trCompoundListDescriptionFortran() override
    { return english.trCompoundListDescriptionFortran(); }

    QCString trCompoundMembersDescriptionFortran(bool extractAll) override
    { return english.trCompoundMembersDescriptionFortran(extractAll); }

    QCString trCompoundIndexFortran() override
    { return english.trCompoundIndexFortran(); }

    QCString trTypeDocumentation() override
    { return english.trTypeDocumentation(); }

    QCString trSubprograms() override
    { return english.trSubprograms(); }

    QCString trSubprogramDocumentation() override
    { return english.trSubprogramDocumentation(); }

    QCString trDataTypes() override
    { return english.trDataTypes(); }

    QCString trModulesList() override
    { return english.trModulesList(); }

    QCString trModulesListDescription(bool extractAll) override
    { return english.trModulesListDescription(extractAll); }

    QCString trCompoundReferenceFortran(const QCString &clName,
                                 ClassDef::CompoundType compType,
                                 bool isTemplate) override
    { return english.trCompoundReferenceFortran(clName,compType,isTemplate); }

    QCString trModuleReference(const QCString &namespaceName) override
    { return english.trModuleReference(namespaceName); }

    QCString trModulesMembers() override
    { return english.trModulesMembers(); }

    QCString trModulesMemberDescription(bool extractAll) override
    { return english.trModulesMemberDescription(extractAll); }

    QCString trModulesIndex() override
    { return english.trModulesIndex(); }

    QCString trModule(bool first_capital, bool singular) override
    { return english.trModule(first_capital,singular); }

    QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
                     bool single) override
    { return english.trGeneratedFromFilesFortran(compType,single); }

    QCString trType(bool first_capital, bool singular) override
    { return english.trType(first_capital,singular); }

    QCString trSubprogram(bool first_capital, bool singular) override
    { return english.trSubprogram(first_capital,singular); }

    QCString trTypeConstraints() override
    { return english.trTypeConstraints(); }
};

/** Adapter class for languages that only contain translations up to
 *  version 1.4.6
 */
class TranslatorAdapter_1_4_6 : public TranslatorAdapter_1_6_0
{
  public:
    QCString updateNeededMessage() override
    { return createUpdateNeededMessage(idLanguage(),"release 1.4.6"); }
    QCString trCallerGraph() override
    { return english.trCallerGraph(); }
    QCString trEnumerationValueDocumentation() override
    { return english.trEnumerationValueDocumentation(); }
};

#endif
