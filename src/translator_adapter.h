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

class TranslatorAdapter_1_2_11 : public TranslatorAdapterBase
{
  public:
    virtual QCString updateNeededMessage() 
    { return createUpdateNeededMessage(idLanguage(),"release 1.2.11"); }
   
    // Put new adapter methods below...
    // 
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

/*! \brief Translator adapter class for release 1.2.1
 *  
 *  Translator adapter for dealing with translator changes since 
 *  release 1.2.1
 */
class TranslatorAdapter_1_2_1 : public TranslatorAdapter_1_2_2
{
  public:
    virtual QCString updateNeededMessage() 
    { return createUpdateNeededMessage(idLanguage(),"release 1.2.1"); }

    virtual QCString trDCOPMethods()
    { return english.trDCOPMethods(); }
};

/*! \brief Translator adapter class for release 1.2.0
 *  
 *  Translator adapter for dealing with translator changes since 
 *  release 1.2.0
 */
class TranslatorAdapter_1_2_0 : public TranslatorAdapter_1_2_1
{
  public:
    virtual QCString updateNeededMessage() 
    { return createUpdateNeededMessage(idLanguage(),"release 1.2.0"); }

    virtual QCString trTest()
    { return english.trTest(); }
       
    virtual QCString trTestList()
    { return english.trTestList(); }
       
};

/*! \brief Translator adapter class for release 1.1.5
 *  
 *  Translator adapter for dealing with translator changes since 
 *  release 1.1.5
 */
class TranslatorAdapter_1_1_5 : public TranslatorAdapter_1_2_0
{
  public:
    virtual QCString updateNeededMessage() 
    { return createUpdateNeededMessage(idLanguage(),"release 1.1.5"); }

    virtual QCString trLegendTitle()
    { return english.trLegendTitle(); }
       
    virtual QCString trLegendDocs()
    { return english.trLegendDocs(); }
       
    virtual QCString trLegend()
    { return english.trLegend(); }
};

/*! \brief Translator adapter class for release 1.1.4
 *  
 *  Translator adapter for dealing with translator changes since 
 *  release 1.1.4
 */
class TranslatorAdapter_1_1_4 : public TranslatorAdapter_1_1_5
{
  public:
    virtual QCString updateNeededMessage() 
    { return createUpdateNeededMessage(idLanguage(),"release 1.1.4"); }

    virtual QCString trReferencedBy()
    { return english.trReferencedBy(); }
       
    virtual QCString trRemarks()
    { return english.trRemarks(); }
       
    virtual QCString trAttention()
    { return english.trAttention(); }
       
    virtual QCString trInclByDepGraph()
    { return english.trInclByDepGraph(); }
       
    virtual QCString trSince()
    { return english.trSince(); }
};

/*! \brief Translator adapter class for release 1.1.3
 *  
 *  Translator adapter for dealing with translator changes since 
 *  release 1.1.3
 */
class TranslatorAdapter_1_1_3 : public TranslatorAdapter_1_1_4
{
  public:
    virtual QCString updateNeededMessage() 
    { return createUpdateNeededMessage(idLanguage(),"release 1.1.3"); }

    virtual QCString trTodo()
    { return english.trTodo(); }
       
    virtual QCString trTodoList()
    { return english.trTodoList(); }
};

/*! \brief Translator adapter class for release 1.1.0
 *  
 *  Translator adapter for dealing with translator changes since 
 *  release 1.1.0
 */
class TranslatorAdapter_1_1_0 : public TranslatorAdapter_1_1_3
{
  public:
    virtual QCString updateNeededMessage() 
    { return createUpdateNeededMessage(idLanguage(),"release 1.1.0"); }

    virtual QCString trNote()
    { return english.trNote(); }
       
    virtual QCString trPublicTypes()
    { return english.trPublicTypes(); }
       
    virtual QCString trPublicAttribs()
    { return english.trPublicAttribs(); }
       
    virtual QCString trStaticPublicAttribs()
    { return english.trStaticPublicAttribs(); }
       
    virtual QCString trProtectedTypes()
    { return english.trProtectedTypes(); }
       
    virtual QCString trProtectedAttribs()
    { return english.trProtectedAttribs(); }
       
    virtual QCString trStaticProtectedAttribs()
    { return english.trStaticProtectedAttribs(); }
       
    virtual QCString trPrivateTypes()
    { return english.trPrivateTypes(); }
       
    virtual QCString trPrivateAttribs()
    { return english.trPrivateAttribs(); }
       
    virtual QCString trStaticPrivateAttribs()
    { return english.trStaticPrivateAttribs(); }
       
    virtual QCString trEnumerationValueDocumentation()
    { return english.trEnumerationValueDocumentation(); }
};

/*! \brief Translator adapter class for release 1.0.0
 *  
 *  Translator adapter for dealing with translator changes since 
 *  release 1.0.0
 */
class TranslatorAdapter_1_0_0 : public TranslatorAdapter_1_1_0
{
    virtual QCString updateNeededMessage() 
    { return createUpdateNeededMessage(idLanguage(),"release 1.0.0"); }

    virtual QCString trIncludingInheritedMembers()
    { return english.trIncludingInheritedMembers(); }
   
    virtual QCString trFuncProtos()
    { return english.trFuncProtos(); }

    virtual QCString trNamespaces()
    { return english.trNamespaces(); }

    virtual QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single)
    { return english.trGeneratedFromFiles(compType, single); }
        
    virtual QCString trAlphabeticalList()
    { return english.trAlphabeticalList(); }

    virtual QCString trReturnValues()
    { return english.trReturnValues(); }

    virtual QCString trMainPage()
    { return english.trMainPage(); }

    virtual QCString trPageAbbreviation()
    { return english.trPageAbbreviation(); }

    virtual QCString trSources()
    { return english.trSources(); }
   
    virtual QCString trStaticProtectedMembers()
    { return english.trStaticProtectedMembers(); }
   
    virtual QCString trDefinedAtLineInSourceFile()
    { return english.trDefinedAtLineInSourceFile(); }
   
    virtual QCString trDefinedInSourceFile()
    { return english.trDefinedInSourceFile(); }

    virtual QCString trDeprecated()  
    { return english.trDeprecated(); }

    virtual QCString trCollaborationDiagram(const char *clName)
    { return english.trCollaborationDiagram(clName); }
       
    virtual QCString trInclDepGraph(const char *fName)
    { return english.trInclDepGraph(fName); }
       
    virtual QCString trConstructorDocumentation()
    { return english.trConstructorDocumentation(); }
       
    virtual QCString trGotoSourceCode()
    { return english.trGotoSourceCode(); }
       
    virtual QCString trGotoDocumentation()
    { return english.trGotoDocumentation(); }
       
    virtual QCString trPrecondition()
    { return english.trPrecondition(); }
       
    virtual QCString trPostcondition()
    { return english.trPostcondition(); }
       
    virtual QCString trInvariant()
    { return english.trInvariant(); }
       
    virtual QCString trInitialValue()
    { return english.trInitialValue(); }
       
    virtual QCString trCode()
    { return english.trCode(); }
       
    virtual QCString trGraphicalHierarchy()
    { return english.trGraphicalHierarchy(); }
     
    virtual QCString trGotoGraphicalHierarchy()
    { return english.trGotoGraphicalHierarchy(); }
       
    virtual QCString trGotoTextualHierarchy()
    { return english.trGotoTextualHierarchy(); }
       
    virtual QCString trPageIndex()
    { return english.trPageIndex(); }
       
    virtual QCString trReimplementedForInternalReasons()
    { return english.trReimplementedForInternalReasons(); }
       
    virtual QCString trClassHierarchyDescription()
    { return english.trClassHierarchyDescription(); }
       
    virtual QCString trNoDescriptionAvailable()
    { return english.trNoDescriptionAvailable(); }
       
    virtual QCString trEnumerationValues()
    { return english.trEnumerationValues(); }
       
    virtual QCString trFunctionPrototypeDocumentation()
    { return english.trFunctionPrototypeDocumentation(); }
       
    virtual QCString trEnumerationTypeDocumentation()
    { return english.trEnumerationTypeDocumentation(); }
       
    virtual QCString trFunctionDocumentation() 
    { return english.trFunctionDocumentation(); }
       
    virtual QCString trRelatedFunctionDocumentation()
    { return english.trRelatedFunctionDocumentation(); }    
       
};

#endif
