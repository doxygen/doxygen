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
    virtual QCString updateNeededMessage() = 0;

};

class TranslatorAdapter_1_9_2 : public TranslatorAdapterBase
{
  public:
    virtual QCString updateNeededMessage()
    { return createUpdateNeededMessage(idLanguage(),"release 1.9.2"); }

    virtual QCString trConcept(bool first_capital,bool singular)
    { return english.trConcept(first_capital,singular); }

    virtual QCString trConceptReference(const QCString &conceptName)
    { return english.trConceptReference(conceptName); }

    virtual QCString trConceptList()
    { return english.trConceptList(); }

    virtual QCString trConceptIndex()
    { return english.trConceptIndex(); }

    virtual QCString trConceptDocumentation()
    { return english.trConceptDocumentation(); }

    virtual QCString trConceptListDescription(bool extractAll)
    { return english.trConceptListDescription(extractAll); }

    virtual QCString trConceptDefinition()
    { return english.trConceptDefinition(); }
};

class TranslatorAdapter_1_8_19 : public TranslatorAdapter_1_9_2
{
  public:
    virtual QCString updateNeededMessage()
    { return createUpdateNeededMessage(idLanguage(),"release 1.8.19"); }

    virtual QCString trDesignUnitDocumentation()
    { return english.trDesignUnitDocumentation(); }
};

class TranslatorAdapter_1_8_15 : public TranslatorAdapter_1_8_19
{
  public:
    virtual QCString updateNeededMessage()
    { return createUpdateNeededMessage(idLanguage(),"release 1.8.15"); }

    virtual QCString trDesignUnitHierarchy()
    { return english.trDesignUnitHierarchy(); }

    virtual QCString trDesignUnitList()
    { return english.trDesignUnitList(); }

    virtual QCString trDesignUnitMembers()
    { return english.trDesignUnitMembers(); }

    virtual QCString trDesignUnitListDescription()
    { return english.trDesignUnitListDescription(); }

    virtual QCString trDesignUnitIndex()
    { return english.trDesignUnitIndex(); }

    virtual QCString trDesignUnits()
    { return english.trDesignUnits(); }

    virtual QCString trFunctionAndProc()
    { return english.trFunctionAndProc(); }

    virtual QCString trVhdlType(uint64 type,bool single)
    { return english.trVhdlType(type,single); }

    virtual QCString trCustomReference(const QCString &name)
    { return english.trCustomReference(name); }

    virtual QCString trConstants()
    { return english.trConstants(); }

    virtual QCString trConstantDocumentation()
    { return english.trConstantDocumentation(); }

    virtual QCString trSequences()
    { return english.trSequences(); }

    virtual QCString trSequenceDocumentation()
    { return english.trSequenceDocumentation(); }

    virtual QCString trDictionaries()
    { return english.trDictionaries(); }

    virtual QCString trDictionaryDocumentation()
    { return english.trDictionaryDocumentation(); }

    virtual QCString trSliceInterfaces()
    { return english.trSliceInterfaces(); }

    virtual QCString trInterfaceIndex()
    { return english.trInterfaceIndex(); }

    virtual QCString trInterfaceList()
    { return english.trInterfaceList(); }

    virtual QCString trInterfaceListDescription()
    { return english.trInterfaceListDescription(); }

    virtual QCString trInterfaceHierarchy()
    { return english.trInterfaceHierarchy(); }

    virtual QCString trInterfaceHierarchyDescription()
    { return english.trInterfaceHierarchyDescription(); }

    virtual QCString trInterfaceDocumentation()
    { return english.trInterfaceDocumentation(); }

    virtual QCString trStructs()
    { return english.trStructs(); }

    virtual QCString trStructIndex()
    { return english.trStructIndex(); }

    virtual QCString trStructList()
    { return english.trStructList(); }

    virtual QCString trStructListDescription()
    { return english.trStructListDescription(); }

    virtual QCString trStructDocumentation()
    { return english.trStructDocumentation(); }

    virtual QCString trExceptionIndex()
    { return english.trExceptionIndex(); }

    virtual QCString trExceptionList()
    { return english.trExceptionList(); }

    virtual QCString trExceptionListDescription()
    { return english.trExceptionListDescription(); }

    virtual QCString trExceptionHierarchy()
    { return english.trExceptionHierarchy(); }

    virtual QCString trExceptionHierarchyDescription()
    { return english.trExceptionHierarchyDescription(); }

    virtual QCString trExceptionDocumentation()
    { return english.trExceptionDocumentation(); }

    virtual QCString trCompoundReferenceSlice(const QCString &clName, ClassDef::CompoundType compType, bool isLocal)
    { return english.trCompoundReferenceSlice(clName,compType,isLocal); }

    virtual QCString trOperations()
    { return english.trOperations(); }

    virtual QCString trOperationDocumentation()
    { return english.trOperationDocumentation(); }

    virtual QCString trDataMembers()
    { return english.trDataMembers(); }

    virtual QCString trDataMemberDocumentation()
    { return english.trDataMemberDocumentation(); }
};

class TranslatorAdapter_1_8_4 : public TranslatorAdapter_1_8_15
{
  public:
    virtual QCString updateNeededMessage()
    { return createUpdateNeededMessage(idLanguage(),"release 1.8.4"); }

    virtual QCString trInterfaces()
    { return english.trInterfaces(); }

    virtual QCString trServices()
    { return english.trServices(); }

    virtual QCString trConstantGroups()
    { return english.trConstantGroups(); }

    virtual QCString trConstantGroupReference(const QCString &namespaceName)
    { return english.trConstantGroupReference(namespaceName); }

    virtual QCString trServiceReference(const QCString &sName)
    { return english.trServiceReference(sName); }

    virtual QCString trSingletonReference(const QCString &sName)
    { return english.trSingletonReference(sName); }

    virtual QCString trServiceGeneratedFromFiles(bool single)
    { return english.trServiceGeneratedFromFiles(single); }

    virtual QCString trSingletonGeneratedFromFiles(bool single)
    { return english.trSingletonGeneratedFromFiles(single); }
};

class TranslatorAdapter_1_8_2 : public TranslatorAdapter_1_8_4
{
  public:
    virtual QCString updateNeededMessage()
    { return createUpdateNeededMessage(idLanguage(),"release 1.8.2"); }

    virtual QCString trPanelSynchronisationTooltip(bool enable)
    { return english.trPanelSynchronisationTooltip(enable); }

    virtual QCString trProvidedByCategory()
    { return english.trProvidedByCategory(); }

    virtual QCString trExtendsClass()
    { return english.trExtendsClass(); }

    virtual QCString trClassMethods()
    { return english.trClassMethods(); }

    virtual QCString trInstanceMethods()
    { return english.trInstanceMethods(); }

    virtual QCString trMethodDocumentation()
    { return english.trMethodDocumentation(); }
};


/** Adapter class for languages that only contain translations up to
 *  version 1.8.0.
 */
class TranslatorAdapter_1_8_0 : public TranslatorAdapter_1_8_2
{
  public:
    virtual QCString updateNeededMessage()
    { return createUpdateNeededMessage(idLanguage(),"release 1.8.0"); }

    virtual QCString trDetailLevel()
    { return english.trDetailLevel(); }

    virtual QCString trTemplateParameters()
    { return english.trTemplateParameters(); }

    virtual QCString trAndMore(const QCString &number)
    { return english.trAndMore(number); }

    virtual QCString trEnumGeneratedFromFiles(bool single)
    { return english.trEnumGeneratedFromFiles(single); }

    virtual QCString trEnumReference(const QCString &name)
    { return english.trEnumReference(name); }

    virtual QCString trInheritedFrom(const QCString &members,const QCString &what)
    { return english.trInheritedFrom(members,what); }

    virtual QCString trAdditionalInheritedMembers()
    { return english.trAdditionalInheritedMembers(); }

};

/** Adapter class for languages that only contain translations up to
 *  version 1.7.5.
 */
class TranslatorAdapter_1_7_5 : public TranslatorAdapter_1_8_0
{
  public:
    virtual QCString updateNeededMessage()
    { return createUpdateNeededMessage(idLanguage(),"release 1.7.5"); }

    virtual QCString trCiteReferences()
    { return english.trCiteReferences(); }

    virtual QCString trCopyright()
    { return english.trCopyright(); }

    virtual QCString trDirDepGraph(const QCString &name)
    { return english.trDirDepGraph(name); }

    virtual QCString trFileIn(const QCString &name)
    { return english.trFileIn(name); }
    virtual QCString trIncludesFileIn(const QCString &name)
    { return english.trIncludesFileIn(name); }
    virtual QCString trDateTime(int year,int month,int day,int dayOfWeek,
                                int hour,int minutes,int seconds,
                                bool includeTime)
    { return english.trDateTime(year,month,day,dayOfWeek,hour,minutes,seconds,includeTime); }
};

/** Adapter class for languages that only contain translations up to
 *  version 1.6.0.
 */
class TranslatorAdapter_1_6_0 : public TranslatorAdapter_1_7_5
{
  public:
    virtual QCString updateNeededMessage()
    { return createUpdateNeededMessage(idLanguage(),"release 1.6.0"); }

    virtual QCString trDirRelation(const QCString &name)
    { return english.trDirRelation(name); }

    virtual QCString trLoading()
    { return english.trLoading(); }

    virtual QCString trGlobalNamespace()
    { return english.trGlobalNamespace(); }

    virtual QCString trSearching()
    { return english.trSearching(); }

    virtual QCString trNoMatches()
    { return english.trNoMatches(); }

    virtual QCString trMemberFunctionDocumentationFortran()
    { return english.trMemberFunctionDocumentationFortran(); }

    virtual QCString trCompoundListFortran()
    { return english.trCompoundListFortran(); }

    virtual QCString trCompoundMembersFortran()
    { return english.trCompoundMembersFortran(); }

    virtual QCString trCompoundListDescriptionFortran()
    { return english.trCompoundListDescriptionFortran(); }

    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll)
    { return english.trCompoundMembersDescriptionFortran(extractAll); }

    virtual QCString trCompoundIndexFortran()
    { return english.trCompoundIndexFortran(); }

    virtual QCString trTypeDocumentation()
    { return english.trTypeDocumentation(); }

    virtual QCString trSubprograms()
    { return english.trSubprograms(); }

    virtual QCString trSubprogramDocumentation()
    { return english.trSubprogramDocumentation(); }

    virtual QCString trDataTypes()
    { return english.trDataTypes(); }

    virtual QCString trModulesList()
    { return english.trModulesList(); }

    virtual QCString trModulesListDescription(bool extractAll)
    { return english.trModulesListDescription(extractAll); }

    virtual QCString trCompoundReferenceFortran(const QCString &clName,
                                 ClassDef::CompoundType compType,
                                 bool isTemplate)
    { return english.trCompoundReferenceFortran(clName,compType,isTemplate); }

    virtual QCString trModuleReference(const QCString &namespaceName)
    { return english.trModuleReference(namespaceName); }

    virtual QCString trModulesMembers()
    { return english.trModulesMembers(); }

    virtual QCString trModulesMemberDescription(bool extractAll)
    { return english.trModulesMemberDescription(extractAll); }

    virtual QCString trModulesIndex()
    { return english.trModulesIndex(); }

    virtual QCString trModule(bool first_capital, bool singular)
    { return english.trModule(first_capital,singular); }

    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
                     bool single)
    { return english.trGeneratedFromFilesFortran(compType,single); }

    virtual QCString trType(bool first_capital, bool singular)
    { return english.trType(first_capital,singular); }

    virtual QCString trSubprogram(bool first_capital, bool singular)
    { return english.trSubprogram(first_capital,singular); }

    virtual QCString trTypeConstraints()
    { return english.trTypeConstraints(); }
};

/** Adapter class for languages that only contain translations up to
 *  version 1.4.6
 */
class TranslatorAdapter_1_4_6 : public TranslatorAdapter_1_6_0
{
  public:
    virtual QCString updateNeededMessage()
    { return createUpdateNeededMessage(idLanguage(),"release 1.4.6"); }

    virtual QCString trCallerGraph()
    {
      return english.trCallerGraph();
    }
    virtual QCString trEnumerationValueDocumentation()
    {
      return english.trEnumerationValueDocumentation();
    }
};

#endif
