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

class TranslatorAdapter_1_3_8 : public TranslatorAdapterBase
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

class TranslatorAdapter_1_2_7 : public TranslatorAdapter_1_2_11
{
  public:
    virtual QCString updateNeededMessage() 
    { return createUpdateNeededMessage(idLanguage(),"release 1.2.7"); }
   
    /*! These are the default implementations of the obsolete methods
     *  for introducing author/authors (possibly localized).
     */
    virtual QCString trAuthors()
    { return "Author(s)"; }
   
    virtual QCString trAuthor()
    { return "Author"; }
   
    /*! This is the localized implementation of newer equivalent
     *  using the obsolete methods trAuthors() and trAuthor().
     */
    virtual QCString trAuthor(bool first_capital, bool singular)
    { 
      if (first_capital)
        return (singular) ? trAuthor() : trAuthors();  // possibly localized
      else
        return english.trAuthor(first_capital, singular);        
    }
};


/*! \brief Translator adapter class for release 1.2.6
 *  
 *  Translator adapter for dealing with translator changes since 
 *  release 1.2.6
 */
class TranslatorAdapter_1_2_6 : public TranslatorAdapter_1_2_7
{
  public:
    virtual QCString updateNeededMessage() 
    { return createUpdateNeededMessage(idLanguage(),"release 1.2.6"); }

    /*! Used as ansicpg for RTF file */
    virtual QCString trRTFansicp()
    {
      return "1252";
    }
    
    /*! Used as ansicpg for RTF fcharset */
    virtual QCString trRTFCharSet()
    {
      return "0";
    }

    /*! Used as header RTF general index */
    virtual QCString trRTFGeneralIndex()
    {
      return "Index";
    }
   
    //////////////////////////////////////////////////////////////////
    // The following methods were replaced by newer equivalent, but
    // the obsolete method may be localized.  Let's use the localized
    // result if possible.
       
       
    /*! This is the default implementation of the obsolete method
     * used in the documentation of a group before the list of
     * links to documented files.  This is possibly localized.
     */
    virtual QCString trFiles()
    { return "Files"; }
   
    /*! This is the localized implementation of newer equivalent
     * using the obsolete method trFiles().
     */
    virtual QCString trFile(bool first_capital, bool singular)
    { 
      if (first_capital && !singular)
        return trFiles();  // possibly localized
      else
        return english.trFile(first_capital, singular);        
    }

    /*! The latexBabelPackage() was superceeded by 
     * latexLanguageSupportCommand().  The default implementation
     * of the obsolete method follows. 
     */
    virtual QCString latexBabelPackage()
    { return ""; }
       
    /*! Default implementation of the newer method. */
    virtual QCString latexLanguageSupportCommand()
    { 
      QCString result(latexBabelPackage());
      if (!result.isEmpty())
      {
        result = "\\usepackage[" + result;
        result += "]{babel}\n";
      }
      return result;
    }

    virtual QCString idLanguageCharset()
    { return english.idLanguageCharset(); }

    virtual QCString trClass(bool first_capital, bool singular)
    { return english.trClass(first_capital,singular); }

    virtual QCString trNamespace(bool first_capital, bool singular)
    { return english.trNamespace(first_capital,singular); }

    virtual QCString trGroup(bool first_capital, bool singular)
    { return english.trGroup(first_capital,singular); }

    virtual QCString trPage(bool first_capital, bool singular)
    { return english.trPage(first_capital,singular); }

    virtual QCString trMember(bool first_capital, bool singular)
    { return english.trMember(first_capital,singular); }
   
    virtual QCString trField(bool first_capital, bool singular)
    { return english.trField(first_capital,singular); }

    virtual QCString trGlobal(bool first_capital, bool singular)
    { return english.trGlobal(first_capital,singular); }

    
};

/*! \brief Translator adapter class for release 1.2.5
 *  
 *  Translator adapter for dealing with translator changes since 
 *  release 1.2.5
 */
class TranslatorAdapter_1_2_5 : public TranslatorAdapter_1_2_6
{
  public:
    virtual QCString updateNeededMessage() 
    { return createUpdateNeededMessage(idLanguage(),"release 1.2.5"); }

    virtual QCString trBug()
    { return english.trBug(); }
               
    virtual QCString trBugList()
    { return english.trBugList(); }
};

/*! \brief Translator adapter class for release 1.2.4
 *  
 *  Translator adapter for dealing with translator changes since 
 *  release 1.2.4
 */
class TranslatorAdapter_1_2_4 : public TranslatorAdapter_1_2_5
{
  public:
    virtual QCString updateNeededMessage() 
    { return createUpdateNeededMessage(idLanguage(),"release 1.2.4"); }

    virtual QCString trInterfaces()
    { return english.trInterfaces(); }
       
    virtual QCString trClasses()
    { return english.trClasses(); }
       
    virtual QCString trPackage(const char *name)
    { return english.trPackage(name); }
       
    virtual QCString trPackageList()
    { return english.trPackageList(); }
       
    virtual QCString trPackageListDescription()
    { return english.trPackageListDescription(); }
       
    virtual QCString trPackages()
    { return english.trPackages(); }
       
    virtual QCString trPackageDocumentation()
    { return english.trPackageDocumentation(); }
       
    virtual QCString trDefineValue()
    { return english.trDefineValue(); }
       
};

/*! \brief Translator adapter class for release 1.2.2
 *  
 *  Translator adapter for dealing with translator changes since 
 *  release 1.2.2
 */
class TranslatorAdapter_1_2_2 : public TranslatorAdapter_1_2_4
{
  public:
    virtual QCString updateNeededMessage() 
    { return createUpdateNeededMessage(idLanguage(),"release 1.2.2"); }

    virtual QCString trProperties()
    { return english.trProperties(); }
       
    virtual QCString trPropertyDocumentation()
    { return english.trPropertyDocumentation(); }
};

#endif
