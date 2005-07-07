#ifndef TRANSLATOR_ADAPTER_H
#define TRANSLATOR_ADAPTER_H

#include "version.h"

/*! \brief Base of the translator adapter tree
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

class TranslatorAdapter_1_4_1 : public TranslatorAdapterBase
{
  public:
    virtual QCString updateNeededMessage()
    { return createUpdateNeededMessage(idLanguage(),"release 1.4.1"); }
    
    virtual QCString trOverloadText()
    { return english.trOverloadText(); }
};

class TranslatorAdapter_1_3_9 : public TranslatorAdapter_1_4_1
{
  public:
    virtual QCString updateNeededMessage()
    { return createUpdateNeededMessage(idLanguage(),"release 1.3.9"); }
    
    virtual QCString trDirIndex()
    { return english.trDirIndex(); }
    virtual QCString trDirDocumentation()
    { return english.trDirDocumentation(); }
    virtual QCString trDirectories()
    { return english.trDirectories(); }
    virtual QCString trDirDescription()
    { return english.trDirDescription(); }
    virtual QCString trDirReference(const char *dirName)
    { return english.trDirReference(dirName); }
    virtual QCString trDir(bool first_capital, bool singular)
    { return english.trDir(first_capital,singular); }
};

class TranslatorAdapter_1_3_8 : public TranslatorAdapter_1_3_9
{
  public:
    virtual QCString updateNeededMessage()
    { return createUpdateNeededMessage(idLanguage(),"release 1.3.8"); }
    
    virtual QCString trSourceFile(QCString& filename)
    { return english.trSourceFile(filename); }
};

class TranslatorAdapter_1_3_3 : public TranslatorAdapter_1_3_8
{
  public:
    virtual QCString updateNeededMessage() 
    { return createUpdateNeededMessage(idLanguage(),"release 1.3.3"); }

    virtual QCString trSearchForIndex()
    { return english.trSearchForIndex(); }
    virtual QCString trSearchResultsTitle()
    { return english.trSearchResultsTitle(); }
    virtual QCString trSearchResults(int numDocuments)
    { return english.trSearchResults(numDocuments); }
    virtual QCString trSearchMatches()
    { return english.trSearchMatches(); }

};

class TranslatorAdapter_1_3_1 : public TranslatorAdapter_1_3_3
{
  public:
    virtual QCString updateNeededMessage() 
    { return createUpdateNeededMessage(idLanguage(),"release 1.3.1"); }
    virtual QCString trAll()
    { return english.trAll(); }
    virtual QCString trCallGraph()
    { return english.trCallGraph(); }
};

class TranslatorAdapter_1_3 : public TranslatorAdapter_1_3_1
{
  public:
    virtual QCString updateNeededMessage() 
    { return createUpdateNeededMessage(idLanguage(),"release 1.3"); }

    virtual QCString trPackageTypes()
    { return english.trPackageTypes(); }
    virtual QCString trPackageMembers()
    { return english.trPackageMembers(); }
    virtual QCString trStaticPackageMembers()
    { return english.trStaticPackageMembers(); }
    virtual QCString trPackageAttribs()
    { return english.trPackageAttribs(); }
    virtual QCString trStaticPackageAttribs() 
    { return english.trStaticPackageAttribs(); }
};

class TranslatorAdapter_1_2_18 : public TranslatorAdapter_1_3
{
  public:
    virtual QCString updateNeededMessage() 
    { return createUpdateNeededMessage(idLanguage(),"release 1.2.18"); }

    virtual QCString trEvents()
    { return english.trEvents(); }
    virtual QCString trEventDocumentation()
    { return english.trEventDocumentation(); }
};

class TranslatorAdapter_1_2_17 : public TranslatorAdapter_1_2_18
{
  public:
    virtual QCString updateNeededMessage() 
    { return createUpdateNeededMessage(idLanguage(),"release 1.2.17"); }

    virtual QCString trDeprecatedList()
    { return english.trDeprecatedList(); }
};

class TranslatorAdapter_1_2_16 : public TranslatorAdapter_1_2_17
{
  public:
    virtual QCString updateNeededMessage() 
    { return createUpdateNeededMessage(idLanguage(),"release 1.2.16"); }

    virtual QCString trRTFTableOfContents()
    { return english.trRTFTableOfContents(); }
};

class TranslatorAdapter_1_2_13 : public TranslatorAdapter_1_2_16
{
  public:
    virtual QCString updateNeededMessage() 
    { return createUpdateNeededMessage(idLanguage(),"release 1.2.13"); }

    virtual QCString trImplementedFromList(int numEntries) 
    { return english.trImplementedFromList(numEntries); }

    virtual QCString trImplementedInList(int numEntries) 
    { return english.trImplementedInList(numEntries); }
};

class TranslatorAdapter_1_2_11 : public TranslatorAdapter_1_2_13
{
  public:
    virtual QCString updateNeededMessage() 
    { return createUpdateNeededMessage(idLanguage(),"release 1.2.11"); }
   
    virtual QCString trReferences()
    { return english.trReferences(); }
    
};

#endif

