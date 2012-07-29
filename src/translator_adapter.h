#ifndef TRANSLATOR_ADAPTER_H
#define TRANSLATOR_ADAPTER_H

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
    virtual ~TranslatorAdapterBase() {}
    TranslatorEnglish english;

    /*! An auxiliary inline method used by the updateNeededMessage()
     *  for building a warning message.
     */
    inline QCString createUpdateNeededMessage(const QCString & languageName,
                                              const QCString & versionString)
    {
      return QCString("Warning: The selected output language \"")
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

class TranslatorAdapter_1_8_2 : public TranslatorAdapterBase
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

    virtual QCString trDesignOverview()
    { return english.trDesignOverview(); }
};


/** Adapter class for languages that only contain translations upto 
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

    virtual QCString trEnumReference(const char *name)
    { return english.trEnumReference(name); }

    virtual QCString trInheritedFrom(const char *members,const char *what)
    { return english.trInheritedFrom(members,what); }

    virtual QCString trAdditionalInheritedMembers()
    { return english.trAdditionalInheritedMembers(); }

};

/** Adapter class for languages that only contain translations upto 
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

    virtual QCString trDirDepGraph(const char *name)
    { return english.trDirDepGraph(name); }
};

/** Adapter class for languages that only contain translations upto 
 *  version 1.6.3.
 */
class TranslatorAdapter_1_6_3 : public TranslatorAdapter_1_7_5
{
  public:
    virtual QCString updateNeededMessage()
    { return createUpdateNeededMessage(idLanguage(),"release 1.6.3"); }

    virtual QCString trDirDependency(const char *name)
    { return english.trDirDependency(name); }
    virtual QCString trFileIn(const char *name)
    { return english.trFileIn(name); }
    virtual QCString trIncludesFileIn(const char *name)
    { return english.trIncludesFileIn(name); }
    virtual QCString trDateTime(int year,int month,int day,int dayOfWeek,
                                int hour,int minutes,int seconds,
                                bool includeTime)
    { return english.trDateTime(year,month,day,dayOfWeek,hour,minutes,seconds,includeTime); }
};

/** Adapter class for languages that only contain translations upto 
 *  version 1.6.0.
 */
class TranslatorAdapter_1_6_0 : public TranslatorAdapter_1_6_3
{
  public:
    virtual QCString updateNeededMessage()
    { return createUpdateNeededMessage(idLanguage(),"release 1.6.0"); }

    virtual QCString trDirRelation(const char *name)
    { return english.trDirRelation(name); }

    virtual QCString trLoading()
    { return english.trLoading(); }

    virtual QCString trGlobalNamespace()
    { return english.trGlobalNamespace(); }

    virtual QCString trSearching()
    { return english.trSearching(); }

    virtual QCString trNoMatches()
    { return english.trNoMatches(); }
};

/** Adapter class for languages that only contain translations upto 
 *  version 1.5.4
 */
class TranslatorAdapter_1_5_4 : public TranslatorAdapter_1_6_0
{
  public:
    virtual QCString updateNeededMessage()
    { return createUpdateNeededMessage(idLanguage(),"release 1.5.4"); }

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

    virtual QCString trCompoundReferenceFortran(const char *clName,
                                 ClassDef::CompoundType compType,
                                 bool isTemplate)
    { return english.trCompoundReferenceFortran(clName,compType,isTemplate); }

    virtual QCString trModuleReference(const char *namespaceName)
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

/** Adapter class for languages that only contain translations upto 
 *  version 1.4.6
 */
class TranslatorAdapter_1_4_6 : public TranslatorAdapter_1_5_4
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

/** Adapter class for languages that only contain translations upto 
 *  version 1.4.1
 */
class TranslatorAdapter_1_4_1 : public TranslatorAdapter_1_4_6
{
  public:
    virtual QCString updateNeededMessage()
    { return createUpdateNeededMessage(idLanguage(),"release 1.4.1"); }
    
    virtual QCString trOverloadText()
    { return english.trOverloadText(); }
};

#endif

