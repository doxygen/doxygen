#ifndef TRANSLATOR_ADAPTER_H
#define TRANSLATOR_ADAPTER_H

#include "translator_en.h"

/*! A function to generate a warning message to signal the user
 *  that the translation of his/her language of choice needs updating
 *  \relates TranslatorAdapterBase
 */
inline QCString createUpdateNeededMessage(const char *languageName,
                              const char *versionString)
{
  return (QCString)"Warning: The selected output language \""+languageName+
                   "\" has not been updated\nsince release "+versionString+
                   ". As a result some sentences may appear in English.\n";
}

/*! \brief Base of the translator adapter tree
 *
 *  This class provides access to the english translations, to be used
 *  as a substitute for real translations.
 */
class TranslatorAdapterBase : public Translator
{
  protected:
    TranslatorEnglish english;
};

/*! \brief Translator adapter class for release 1.2.6
 *  
 *  Translator adapter for dealing with translator changes since 
 *  release 1.2.6
 */
class TranslatorAdapter_1_2_6 : public TranslatorAdapterBase
{
  public:
    QCString updateNeededMessage() 
    { return createUpdateNeededMessage(idLanguage(),"1.2.6"); }

    /*! Used as ansicpg for RTF file */
    QCString trRTFansicp()
    {
      return "1252";
    }
    
    /*! Used as ansicpg for RTF fcharset */
    QCString trRTFCharSet()
    {
      return "0";
    }

    /*! Used as header RTF general index */
    QCString trRTFGeneralIndex()
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
    QCString trFiles()
    { return "Files"; }
   
    /*! This is the localized implementation of newer equivalent
     * using the obsolete method trFiles().
     */
    QCString trFile(bool first_capital, bool singular)
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
    QCString latexBabelPackage()
    { return ""; }
       
    /*! Default implementation of the newer method. */
    QCString latexLanguageSupportCommand()
    { 
      QCString result(latexBabelPackage());
      if (!result.isEmpty())
      {
        result = "\\usepackage[" + result;
        result += "]{babel}\n";
      }
      return result;
    }

    QCString idLanguageCharset()
    { return english.idLanguageCharset(); }

    QCString trClass(bool first_capital, bool singular)
    { return english.trClass(first_capital,singular); }

    QCString trNamespace(bool first_capital, bool singular)
    { return english.trNamespace(first_capital,singular); }

    QCString trGroup(bool first_capital, bool singular)
    { return english.trGroup(first_capital,singular); }

    QCString trPage(bool first_capital, bool singular)
    { return english.trPage(first_capital,singular); }

    QCString trMember(bool first_capital, bool singular)
    { return english.trMember(first_capital,singular); }
   
    QCString trField(bool first_capital, bool singular)
    { return english.trField(first_capital,singular); }

    QCString trGlobal(bool first_capital, bool singular)
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
    QCString updateNeededMessage() 
    { return createUpdateNeededMessage(idLanguage(),"1.2.5"); }

    QCString trBug()
    { return english.trBug(); }
               
    QCString trBugList()
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
    QCString updateNeededMessage() 
    { return createUpdateNeededMessage(idLanguage(),"1.2.4"); }

    QCString trInterfaces()
    { return english.trInterfaces(); }
       
    QCString trClasses()
    { return english.trClasses(); }
       
    QCString trPackage(const char *name)
    { return english.trPackage(name); }
       
    QCString trPackageList()
    { return english.trPackageList(); }
       
    QCString trPackageListDescription()
    { return english.trPackageListDescription(); }
       
    QCString trPackages()
    { return english.trPackages(); }
       
    QCString trPackageDocumentation()
    { return english.trPackageDocumentation(); }
       
    QCString trDefineValue()
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
    QCString updateNeededMessage() 
    { return createUpdateNeededMessage(idLanguage(),"1.2.2"); }

    QCString trProperties()
    { return english.trProperties(); }
       
    QCString trPropertyDocumentation()
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
    QCString updateNeededMessage() 
    { return createUpdateNeededMessage(idLanguage(),"1.2.1"); }

    QCString trDCOPMethods()
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
    QCString updateNeededMessage() 
    { return createUpdateNeededMessage(idLanguage(),"1.2.0"); }

    QCString trTest()
    { return english.trTest(); }
       
    QCString trTestList()
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
    QCString updateNeededMessage() 
    { return createUpdateNeededMessage(idLanguage(),"1.1.5"); }

    QCString trLegendTitle()
    { return english.trLegendTitle(); }
       
    QCString trLegendDocs()
    { return english.trLegendDocs(); }
       
    QCString trLegend()
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
    QCString updateNeededMessage() 
    { return createUpdateNeededMessage(idLanguage(),"1.1.4"); }

    QCString trReferencedBy()
    { return english.trReferencedBy(); }
       
    QCString trRemarks()
    { return english.trRemarks(); }
       
    QCString trAttention()
    { return english.trAttention(); }
       
    QCString trInclByDepGraph()
    { return english.trInclByDepGraph(); }
       
    QCString trSince()
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
    QCString updateNeededMessage() 
    { return createUpdateNeededMessage(idLanguage(),"1.1.3"); }

    QCString trTodo()
    { return english.trTodo(); }
       
    QCString trTodoList()
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
    QCString updateNeededMessage() 
    { return createUpdateNeededMessage(idLanguage(),"1.1.0"); }

    QCString trNote()
    { return english.trNote(); }
       
    QCString trPublicTypes()
    { return english.trPublicTypes(); }
       
    QCString trPublicAttribs()
    { return english.trPublicAttribs(); }
       
    QCString trStaticPublicAttribs()
    { return english.trStaticPublicAttribs(); }
       
    QCString trProtectedTypes()
    { return english.trProtectedTypes(); }
       
    QCString trProtectedAttribs()
    { return english.trProtectedAttribs(); }
       
    QCString trStaticProtectedAttribs()
    { return english.trStaticProtectedAttribs(); }
       
    QCString trPrivateTypes()
    { return english.trPrivateTypes(); }
       
    QCString trPrivateAttribs()
    { return english.trPrivateAttribs(); }
       
    QCString trStaticPrivateAttribs()
    { return english.trStaticPrivateAttribs(); }
       
    QCString trEnumerationValueDocumentation()
    { return english.trEnumerationValueDocumentation(); }
};

/*! \brief Translator adapter class for release 1.0.0
 *  
 *  Translator adapter for dealing with translator changes since 
 *  release 1.0.0
 */
class TranslatorAdapter_1_0_0 : public TranslatorAdapter_1_1_0
{
    QCString updateNeededMessage() 
    { return createUpdateNeededMessage(idLanguage(),"1.0.0"); }

    QCString trIncludingInheritedMembers()
    { return english.trIncludingInheritedMembers(); }
   
    QCString trFuncProtos()
    { return english.trFuncProtos(); }

    QCString trNamespaces()
    { return english.trNamespaces(); }

    QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single)
    { return english.trGeneratedFromFiles(compType, single); }
        
    QCString trAlphabeticalList()
    { return english.trAlphabeticalList(); }

    QCString trReturnValues()
    { return english.trReturnValues(); }

    QCString trMainPage()
    { return english.trMainPage(); }

    QCString trPageAbbreviation()
    { return english.trPageAbbreviation(); }

    QCString trSources()
    { return english.trSources(); }
   
    QCString trStaticProtectedMembers()
    { return english.trStaticProtectedMembers(); }
   
    QCString trDefinedAtLineInSourceFile()
    { return english.trDefinedAtLineInSourceFile(); }
   
    QCString trDefinedInSourceFile()
    { return english.trDefinedInSourceFile(); }

    QCString trDeprecated()  
    { return english.trDeprecated(); }

    QCString trCollaborationDiagram(const char *clName)
    { return english.trCollaborationDiagram(clName); }
       
    QCString trInclDepGraph(const char *fName)
    { return english.trInclDepGraph(fName); }
       
    QCString trConstructorDocumentation()
    { return english.trConstructorDocumentation(); }
       
    QCString trGotoSourceCode()
    { return english.trGotoSourceCode(); }
       
    QCString trGotoDocumentation()
    { return english.trGotoDocumentation(); }
       
    QCString trPrecondition()
    { return english.trPrecondition(); }
       
    QCString trPostcondition()
    { return english.trPostcondition(); }
       
    QCString trInvariant()
    { return english.trInvariant(); }
       
    QCString trInitialValue()
    { return english.trInitialValue(); }
       
    QCString trCode()
    { return english.trCode(); }
       
    QCString trGraphicalHierarchy()
    { return english.trGraphicalHierarchy(); }
     
    QCString trGotoGraphicalHierarchy()
    { return english.trGotoGraphicalHierarchy(); }
       
    QCString trGotoTextualHierarchy()
    { return english.trGotoTextualHierarchy(); }
       
    QCString trPageIndex()
    { return english.trPageIndex(); }
       
    QCString trReimplementedForInternalReasons()
    { return english.trReimplementedForInternalReasons(); }
       
    QCString trClassHierarchyDescription()
    { return english.trClassHierarchyDescription(); }
       
    QCString trNoDescriptionAvailable()
    { return english.trNoDescriptionAvailable(); }
       
    QCString trEnumerationValues()
    { return english.trEnumerationValues(); }
       
    QCString trFunctionPrototypeDocumentation()
    { return english.trFunctionPrototypeDocumentation(); }
       
    QCString trEnumerationTypeDocumentation()
    { return english.trEnumerationTypeDocumentation(); }
       
    QCString trFunctionDocumentation() 
    { return english.trFunctionDocumentation(); }
       
    QCString trRelatedFunctionDocumentation()
    { return english.trRelatedFunctionDocumentation(); }    
       
};

#endif
