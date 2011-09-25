/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2011 by Dimitri van Heesch.
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

/*
 * Arabic translation for Doxygen
 *
 * Please send your comments on this translation to moazreyad@yahoo.com
 *
 * Note : consider this file as Beta version for testing Arabic output with Doxygen.
 * 
 * Revision history :
 *
 * 6/2006 : Created initial version of Arabic translation.
 */

#ifndef TRANSLATOR_AR_H
#define TRANSLATOR_AR_H

class TranslatorArabic : public TranslatorAdapter_1_4_6
{
  protected:
    friend class TranslatorAdapterBase;
    virtual ~TranslatorArabic() {}

  public:

    // --- Language control methods -------------------
    
    /*! Used for identification of the language. The identification 
     * should not be translated. It should be replaced by the name 
     * of the language in English using lower-case characters only
     * (e.g. "czech", "japanese", "russian", etc.). It should be equal to 
     * the identification used in language.cpp.
     */
    virtual QCString idLanguage()
    { return "arabic"; }
    
    /*! Used to get the LaTeX command(s) for the language support. 
     *  This method should return string with commands that switch
     *  LaTeX to the desired language.  For example 
     *  <pre>"\\usepackage[german]{babel}\n"
     *  </pre>
     *  or
     *  <pre>"\\usepackage{polski}\n"
     *  "\\usepackage[latin2]{inputenc}\n"
     *  "\\usepackage[T1]{fontenc}\n"
     *  </pre>
     * 
     * The English LaTeX does not use such commands.  Because of this
     * the empty string is returned in this implementation.
     */
    virtual QCString latexLanguageSupportCommand()
    {
      return "";
    }

    /*! return the language charset. This will be used for the HTML output */
    virtual QCString idLanguageCharset()
    {
      return "cp1256";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions()
    { return "œÊ«· –«  ’·…"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return "(·«ÕŸ √‰ Â–Â «·œÊ«· ·Ì”  √⁄÷«¡)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return "Ê’›  ›’Ì·Ì"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return " ÊÀÌﬁ  ⁄—Ì›«  «·‰Ê⁄ «·√⁄÷«¡"; }
    
    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return " ÊÀÌﬁ «·⁄œœÌ«  «·√⁄÷«¡"; }
    
    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation()
    { return " ÊÀÌﬁ «·œÊ«· «·√⁄÷«¡"; }
    
    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return " ÊÀÌﬁ «·Õﬁ·"; 
      }
      else
      {
        return " ÊÀÌﬁ «·»Ì«‰ «·⁄÷Ê"; 
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore() 
    { return "«·„“Ìœ ..."; }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers()
    { return "«⁄—÷ ﬂ· «·√⁄÷«¡"; }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList()
    { return "ﬁ«∆„… «·√⁄÷«¡"; }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers()
    { return "Â–Â ›«∆„… »ﬂ· «·√⁄÷«¡ ›Ì "; }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers()
    { return ", „ ÷„‰… ﬂ· «·√⁄÷«¡ «·„Ê—ÊÀ…"; }
    
    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const char *s)
    { QCString result=" „  ﬂÊÌ‰Â« ¬·Ì« »Ê«”ÿ… Doxygen";
      if (s) result+=(QCString)" ·‹ "+s;
      result+=" „‰ „·›«  «·„’œ—."; 
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return "«·≈”„ «·⁄œœÌ"; }
    
    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return "«·ﬁÌ„… «·⁄œœÌ…"; }
    
    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return "„⁄—› ›Ì"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of 
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules()
    { return "„ﬂÊ‰« "; }
    
    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return "«·‘ﬂ· «·Â—„Ì ··›∆…"; }
    
    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "ÂÌ«ﬂ· «·»Ì«‰« ";
      }
      else
      {
        return "ﬁ«∆„… «·›∆« "; 
      }
    }
    
    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return "ﬁ«∆„… «·„·›« "; }

    /*! This is put above each page as a link to the list of all verbatim headers */
    virtual QCString trHeaderFiles()
    { return "«·„·›«  «·—√”Ì…"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "ÕﬁÊ· «·»Ì«‰« "; 
      }
      else
      {
        return "√⁄÷«¡ «·›∆…"; 
      }
    }

    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "«·⁄Ê«„"; 
      }
      else
      {
        return "√⁄÷«¡ «·„·›"; 
      }
    }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages()
    { return "’›Õ«  –«  ’·…"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples()
    { return "√„À·…"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch()
    { return "»ÕÀ"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription()
    { return "ﬁ«∆„… «· Ê«—À Â–Â  „  — Ì»Â« √»ÃœÌ« »’Ê—…  ﬁ—Ì»Ì… ° "
             "Ê·ﬂ‰ ·Ì” »’Ê—…  «„…:";
    }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result="Â–Â ﬁ«∆„… »ﬂ· «·„·›« ";
      if (!extractAll) result+="«·„ÊÀﬁ… ";
	  result+="„⁄ Ê’› „Œ ’— :";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
    { 
      
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Â–Â ÂÌ ÂÌ«ﬂ· «·»Ì«‰«  „⁄ Ê’› „Œ ’—:"; 
      }
      else
      {
        return "Â–Â ÂÌ «·›∆«  ° «·»‰«¡«  °"
			"«·≈ Õ«œ«  Ê«·Ê«ÃÂ«  „⁄ Ê’› „Œ ’—:"; 
      }
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="Â–Â ﬁ«∆„… »ﬂ· ";
      
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        result+="ÕﬁÊ· «·»‰«¡«  Ê«·≈ Õ«œ« ";
      }
      else
      {
        result+="√⁄÷«¡ «·›∆…";
      }
	  if (!extractAll)
      {
        result+=" «·„ÊÀﬁ… ";
      }
      result+=" „⁄ Ê’·«  ≈·Ï ";
      if (!extractAll) 
      {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
          result+=" ÊÀÌﬁ «·»‰«¡/«·≈ Õ«œ ·ﬂ· Õﬁ·:";
        }
        else
        {
          result+=" ÊÀÌﬁ «·›∆… ·ﬂ· ⁄÷Ê:";
        }
      }
      else 
      {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
          result+="«·»‰«¡« /«·≈ Õ«œ«  «· Ì Ì‰ „Ê‰ ≈·ÌÂ«:";
        }
        else
        {
          result+="«·›∆«  «· Ì Ì‰ „Ê‰ ≈·ÌÂ«:";
        }
      }
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    virtual QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Here is a list of all ";
      if (!extractAll) result+="documented ";
      
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        result+="functions, variables, defines, enums, and typedefs";
      }
      else
      {
        result+="file members";
      }
      result+=" with links to ";
      if (extractAll) 
        result+="the files they belong to:";
      else 
        result+="the documentation:";
      return result;
    }

    /*! This is an introduction to the page with the list of all header files. */
    virtual QCString trHeaderFilesDescription()
    { return "Here are the header files that make up the API:"; }

    /*! This is an introduction to the page with the list of all examples */
    virtual QCString trExamplesDescription()
    { return "Â–Â ﬁ«∆„… »ﬂ· «·√„À·…:"; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return "Â–Â ﬁ«∆„… »ﬂ· ’›Õ«  «· ÊÀÌﬁ –«  «·’·…:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return "Â–Â ﬁ«∆„… »ﬂ· «·„ﬂÊ‰« :"; }

    /*! This sentences is used in the annotated class/file lists if no brief
     * description is given. 
     */
    virtual QCString trNoDescriptionAvailable()
    { return "·« ÌÊÃœ Ê’› „ «Õ"; }
    
    // index titles (the project name is prepended for these) 


    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation()
    { return "«· ÊÀÌﬁ"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * index of all groups.
     */
    virtual QCString trModuleIndex()
    { return "›Â—” «·„ﬂÊ‰« "; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex()
    { return "«·›Â—” «·Â—„Ì"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index.
     */
    virtual QCString trCompoundIndex()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      { 
        return "›Â—” ÂÌﬂ· «·»Ì«‰« ";
      }
      else
      {
        return "›Â—” «·›∆…"; 
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    virtual QCString trFileIndex() 
    { return "›Â—” «·„·›« "; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation()
    { return " ÊÀÌﬁ «·„ﬂÊ‰"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "›Â—” ÂÌﬂ· «·»Ì«‰« "; 
      }
      else
      {
        return "›Â—” «·›∆…"; 
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation()
    { return " ÊÀÌﬁ «·„·›« "; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    virtual QCString trExampleDocumentation()
    { return " ÊÀÌﬁ «·√„À·…"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    virtual QCString trPageDocumentation()
    { return " ÊÀÌﬁ «·’›Õ« "; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual()
    { return "«·ﬂ Ì» «·„—Ã⁄Ì"; }
    
    /*! This is used in the documentation of a file as a header before the 
     *  list of defines
     */
    virtual QCString trDefines()
    { return "«· ⁄—Ì›« "; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of function prototypes
     */
    virtual QCString trFuncProtos()
    { return "Function Prototypes"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of typedefs
     */
    virtual QCString trTypedefs()
    { return "Typedefs"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of enumerations
     */
    virtual QCString trEnumerations()
    { return "«·⁄œœÌ« "; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) functions
     */
    virtual QCString trFunctions()
    { return "«·œÊ«·"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trVariables()
    { return "«·„ €Ì—« "; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues()
    { return "Enumerator"; }
    
    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return "Define Documentation"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for function prototypes
     */
    virtual QCString trFunctionPrototypeDocumentation()
    { return "Function Prototype Documentation"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation()
    { return "Typedef Documentation"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation()
    { return "Enumeration Type Documentation"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return "Enumerator Documentation"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation()
    { return " ÊÀÌﬁ «·œ«·…"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation()
    { return " ÊÀÌﬁ «·„ €Ì—"; }

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "ÂÌ«ﬂ· «·»Ì«‰« "; 
      }
      else
      {
        return "«·›∆« "; 
      }
    }

    /*! This is used in the standard footer of each page and indicates when 
     *  the page was generated 
     */
    virtual QCString trGeneratedAt(const char *date,const char *projName)
    { 
      QCString result=(QCString)"Generated on "+date;
      if (projName) result+=(QCString)" for "+projName;
      result+=(QCString)" by";
      return result;
    }
    /*! This is part of the sentence used in the standard footer of each page.
     */
    virtual QCString trWrittenBy()
    {
      return "ﬂ » »Ê«”ÿ…";
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const char *clName)
    {
      return (QCString)"Inheritance diagram for "+clName+":";
    }
    
    /*! this text is generated when the \\internal command is used. */
    virtual QCString trForInternalUseOnly()
    { return "··«” Œœ«„ «·œ«Œ·Ì ›ﬁÿ."; }

    /*! this text is generated when the \\reimp command is used. */
    virtual QCString trReimplementedForInternalReasons()
    { return "Reimplemented for internal reasons; the API is not affected."; }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning()
    { return " ‰»ÌÂ"; }

    /*! this text is generated when the \\bug command is used. */
    virtual QCString trBugsAndLimitations()
    { return "Bugs and limitations"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion()
    { return "≈’œ«—…"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate()
    { return " «—ÌŒ"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns()
    { return "Returns"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso()
    { return "«‰Ÿ— √Ì÷«"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters()
    { return "Parameters"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions()
    { return "«” À‰«¡« "; }
    
    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy()
    { return "«‰ Ã  »Ê«”ÿ…"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////
    
    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return "ﬁ«∆„… „Ã«·«  «·√”„«¡"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Here is a list of all ";
      if (!extractAll) result+="documented ";
      result+="namespaces with brief descriptions:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends()
    { return "Friends"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
    /*! used in the class documentation as a header before the list of all
     * related classes 
     */
    virtual QCString trRelatedFunctionDocumentation()
    { return "Friends And Related Function Documentation"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result=(QCString)clName;
      switch(compType)
      {
        case ClassDef::Class:      result+=" Class"; break;
        case ClassDef::Struct:     result+=" Struct"; break;
        case ClassDef::Union:      result+=" Union"; break;
        case ClassDef::Interface:  result+=" Interface"; break;
        case ClassDef::Protocol:   result+=" Protocol"; break;
        case ClassDef::Category:   result+=" Category"; break;
        case ClassDef::Exception:  result+=" Exception"; break;
      }
      if (isTemplate) result+=" Template";
      result+=" Reference";
      return result;
    }

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const char *fileName)
    {
      QCString result=fileName;
      result+=" File Reference"; 
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" Namespace Reference";
      return result;
    }
    
    virtual QCString trPublicMembers()
    { return "Public Member Functions"; }
    virtual QCString trPublicSlots()
    { return "Public Slots"; }
    virtual QCString trSignals()
    { return "Signals"; }
    virtual QCString trStaticPublicMembers()
    { return "Static Public Member Functions"; }
    virtual QCString trProtectedMembers()
    { return "Protected Member Functions"; }
    virtual QCString trProtectedSlots()
    { return "Protected Slots"; }
    virtual QCString trStaticProtectedMembers()
    { return "Static Protected Member Functions"; }
    virtual QCString trPrivateMembers()
    { return "Private Member Functions"; }
    virtual QCString trPrivateSlots()
    { return "Private Slots"; }
    virtual QCString trStaticPrivateMembers()
    { return "Static Private Member Functions"; }
    
    /*! this function is used to produce a comma-separated list of items.
     *  use generateMarker(i) to indicate where item i should be put.
     */
    virtual QCString trWriteList(int numEntries)
    {
      QCString result;
      int i;
      // the inherits list contain `numEntries' classes
      for (i=0;i<numEntries;i++) 
      {
        // use generateMarker to generate placeholders for the class links!
        result+=generateMarker(i); // generate marker for entry i in the list 
                                   // (order is left to right)
        
        if (i!=numEntries-1)  // not the last entry, so we need a separator
        {
          if (i<numEntries-2) // not the fore last entry 
            result+=", ";
          else                // the fore last entry
            result+=", and ";
        }
      }
      return result; 
    }
    
    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries)
    {
      return "Inherits "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries)
    {
      return "Inherited by "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of 
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries)
    {
      return "Reimplemented from "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries)
    {
      return "Reimplemented in "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers()
    { return "Namespace Members"; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
    { 
      QCString result="Here is a list of all ";
      if (!extractAll) result+="documented ";
      result+="namespace members with links to ";
      if (extractAll) 
        result+="the namespace documentation for each member:";
      else 
        result+="the namespaces they belong to:";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the 
     *  index of all namespaces.
     */
    virtual QCString trNamespaceIndex()
    { return "›Â—” „Ã«·«  «·√”„«¡"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
    { return " ÊÀÌﬁ „Ã«·«  «·√”„«¡"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return "„Ã«·«  «·√”„«¡"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single)
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      QCString result=(QCString)" ÊÀÌﬁ   ";
      switch(compType)
      {
        case ClassDef::Class:      result+="Â–Â «·›∆…"; break;
        case ClassDef::Struct:     result+="Â–« «·»‰«¡"; break;
        case ClassDef::Union:      result+="Â–« «·≈ Õ«œ"; break;
        case ClassDef::Interface:  result+="Â–Â «·Ê«ÃÂ…"; break;
        case ClassDef::Protocol:   result+="Â–« «·»—Ê ÊﬂÊ·"; break;
        case ClassDef::Category:   result+="Â–« «· ’‰Ì›"; break;
        case ClassDef::Exception:  result+="Â–« «·≈” À‰«¡"; break;
      }
      result+="  „  ﬂÊÌ‰Â „‰ ";
	  if (single) result+="Â–« «·„·›:"; else result+="Â–Â «·„·›« :";
      return result;
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    virtual QCString trAlphabeticalList()
    { return "ﬁ«∆„… √»ÃœÌ…"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues()
    { return "ﬁÌ„ «·⁄Êœ…"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return "«·’›Õ… «·—∆Ì”Ì…"; }

    /*! This is used in references to page that are put in the LaTeX 
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation()
    { return "’."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual QCString trSources()
    {
      return "„’«œ—";
    }
    virtual QCString trDefinedAtLineInSourceFile()
    {
      return "Definition at line @0 of file @1.";
    }
    virtual QCString trDefinedInSourceFile()
    {
      return "Definition in file @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated()
    {
      return "Deprecated";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)"—”„ «· ⁄«Ê‰ ·‹ "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName)
    {
      return (QCString)"—”„ «⁄ „«œÌ… «· ÷„Ì‰ ·‹ "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return " ÊÀÌﬁ œÊ«· «·»‰«¡ Ê«·Âœ„"; 
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    {
      return "«–Â» ≈·Ï «·ﬂÊœ «·Œ«’ »Â–« «·„·›.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return "«–Â» ≈·Ï  ÊÀÌﬁ Â–« «·„·›.";
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition()
    {
      return "‘—ÿ ”«»ﬁ";
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition()
    {
      return "‘—ÿ ·«Õﬁ";
    }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant()
    {
      return "À«» ";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue()
    {
		return "ﬁÌ„… „»œ∆Ì…:";
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode()
    {
      return "ﬂÊœ";
    }
    virtual QCString trGraphicalHierarchy()
    {
      return "«·‘ﬂ· «·—”Ê„Ì ··›∆« ";
    }
    virtual QCString trGotoGraphicalHierarchy()
    {
      return "«–Â» ≈·Ï «·‘ﬂ· «·Â—„Ì «·—”Ê„Ì ··›∆« ";
    }
    virtual QCString trGotoTextualHierarchy()
    {
      return "«–Â» ≈·Ï «·‘ﬂ· «·Â—„Ì «·‰’Ì ··›∆« ";
    }
    virtual QCString trPageIndex()
    {
      return "›Â—” «·’›Õ« ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////
    
    virtual QCString trNote()
    {
      return "„·«ÕŸ« ";
    }
    virtual QCString trPublicTypes()
    {
      return "√‰Ê«⁄ ⁄«„…";
    }
    virtual QCString trPublicAttribs()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "ÕﬁÊ· »Ì«‰« ";
      }
      else
      {
        return "’›«  ⁄«„…";
      }
    }
    virtual QCString trStaticPublicAttribs()
    {
      return "’›«  ”«ﬂ‰… ⁄«„…";
    }
    virtual QCString trProtectedTypes()
    {
      return "√‰Ê«⁄ „Õ„Ì…";
    }
    virtual QCString trProtectedAttribs()
    {
      return "’›«  „Õ„Ì…";
    }
    virtual QCString trStaticProtectedAttribs()
    {
      return "’›«  ”«ﬂ‰… „Õ„Ì…";
    }
    virtual QCString trPrivateTypes()
    {
      return "√‰Ê«⁄ Œ«’…";
    }
    virtual QCString trPrivateAttribs()
    {
      return "’›«  Œ«’…";
    }
    virtual QCString trStaticPrivateAttribs()
    {
      return "’›«  ”«ﬂ‰… Œ«’…";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    virtual QCString trTodo()
    {
      return "„Â«„";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    {
      return "ﬁ«∆„… «·„Â«„";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
      return "–ﬂ— »Ê«”ÿ…";
    }
    virtual QCString trRemarks()
    {
      return " ⁄·Ìﬁ« ";
    }
    virtual QCString trAttention()
    {
      return "«‰ »Â";
    }
    virtual QCString trInclByDepGraph()
    {
      return "Â–« «·—”„ ÌÊ÷Õ «·„·›«  «· Ì ÷„‰  Â–« «·„·›"
             "»’Ê—… „»«‘—… √Ê €Ì— „»«‘—…:";
    }
    virtual QCString trSince()
    {
      return "„‰–";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    {
      return "Graph Legend";
    }
    /*! page explaining how the dot graph's should be interpreted 
     *  The %A in the text below are to prevent link to classes called "A".
     */
    virtual QCString trLegendDocs()
    {
      return 
        "This page explains how to interpret the graphs that are generated "
        "by doxygen.<p>\n"
        "Consider the following example:\n"
        "\\code\n"
        "/*! Invisible class because of truncation */\n"
        "class Invisible { };\n\n"
        "/*! Truncated class, inheritance relation is hidden */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Class not documented with doxygen comments */\n"
        "class Undocumented { };\n\n"
        "/*! Class that is inherited using public inheritance */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! A template class */\n"
        "template<class T> class Templ { };\n\n"
        "/*! Class that is inherited using protected inheritance */\n"
        "class ProtectedBase { };\n\n"
        "/*! Class that is inherited using private inheritance */\n"
        "class PrivateBase { };\n\n"
        "/*! Class that is used by the Inherited class */\n"
        "class Used { };\n\n"
        "/*! Super class that inherits a number of other classes */\n"
        "class Inherited : public PublicBase,\n"
        "                  protected ProtectedBase,\n"
        "                  private PrivateBase,\n"
        "                  public Undocumented\n"
        "                  public Templ<int>\n"
        "{\n"
        "  private:\n"
        "    Used *m_usedClass;\n"
        "};\n"
        "\\endcode\n"
        "If the \\c MAX_DOT_GRAPH_HEIGHT tag in the configuration file "
        "is set to 240 this will result in the following graph:"
        "<p><center><img alt=\"\" src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
        "<p>\n"
        "The boxes in the above graph have the following meaning:\n"
        "<ul>\n"
        "<li>%A filled black box represents the struct or class for which the "
        "graph is generated.\n"
        "<li>%A box with a black border denotes a documented struct or class.\n"
        "<li>%A box with a grey border denotes an undocumented struct or class.\n"
        "<li>%A box with a red border denotes a documented struct or class for"
        "which not all inheritance/containment relations are shown. %A graph is "
        "truncated if it does not fit within the specified boundaries.\n"
        "</ul>\n"
        "The arrows have the following meaning:\n"
        "<ul>\n"
        "<li>%A dark blue arrow is used to visualize a public inheritance "
        "relation between two classes.\n"
        "<li>%A dark green arrow is used for protected inheritance.\n"
        "<li>%A dark red arrow is used for private inheritance.\n"
        "<li>%A purple dashed arrow is used if a class is contained or used "
        "by another class. The arrow is labeled with the variable(s) "
        "through which the pointed class or struct is accessible.\n"
        "<li>%A yellow dashed arrow denotes a relation between a template instance and "
        "the template class it was instantiated from. The arrow is labeled with "
        "the template parameters of the instance.\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    virtual QCString trLegend()
    {
      return "legend";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////
    
    /*! Used as a marker that is put before a test item */
    virtual QCString trTest()
    {
      return "«Œ »«—";
    }
    /*! Used as the header of the test list */
    virtual QCString trTestList()
    {
      return "ﬁ«∆„… «·≈Œ »«—« ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for KDE-2 IDL methods */
    virtual QCString trDCOPMethods()
    {
      return "DCOP Member Functions";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties()
    {
      return "Œ’«∆’";
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
      return " ÊÀÌﬁ «·Œ«’Ì…";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java interfaces in the summary section of Java packages */
    virtual QCString trInterfaces()
    {
      return "Ê«ÃÂ« ";
    }
    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "ÂÌ«ﬂ· «·»Ì«‰« ";
      }
      else
      {
        return "›∆« ";
      }
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const char *name)
    {
      return (QCString)"Õ“„… "+name;
    }
    /*! Title of the package index page */
    virtual QCString trPackageList()
    {
      return "ﬁ«∆„… «·Õ“„";
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
		return "Â–Â ÂÌ «·Õ“„ „⁄ Ê’› „Œ ’— ·ﬂ· „‰Â« )≈‰ ÊÃœ( :";	  
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return "Õ“„";
    }
    /*! Used as a chapter title for Latex & RTF output */
    virtual QCString trPackageDocumentation()
    {
      return " ÊÀÌﬁ «·Õ“„";
    }
    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue()
    {
		return "«·ﬁÌ„…:";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////
    
    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug()
    {
      return "À€—…";
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList()
    {
      return "ﬁ«∆„… «·À€—« ";
    }

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
    virtual QCString trRTFansicp()
    {
      return "1256";
    }
    

    /*! Used as ansicpg for RTF fcharset 
     *  \see trRTFansicp() for a table of possible values.
     */
    virtual QCString trRTFCharSet()
    {
      return "0";
    }

    /*! Used as header RTF general index */
    virtual QCString trRTFGeneralIndex()
    {
      return "›Â—”";
    }
   
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trClass(bool /*first_capital*/, bool singular)
    { 
      QCString result("›∆…");
      if (!singular)  result="›∆« ";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trFile(bool /*first_capital*/, bool singular)
    { 
      QCString result("„·›");
      if (!singular)  result="„·›« ";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trNamespace(bool /*first_capital*/, bool singular)
    { 
      QCString result("„Ã«· √”„«¡");
      if (!singular)  result="„Ã«·«  √”„«¡";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGroup(bool /*first_capital*/, bool singular)
    { 
      QCString result("„Ã„Ê⁄…");
      if (!singular)  result="„Ã„Ê⁄« ";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trPage(bool /*first_capital*/, bool singular)
    { 
      QCString result("’›Õ…");
      if (!singular)  result="’›Õ« ";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trMember(bool /*first_capital*/, bool singular)
    { 
      QCString result("⁄÷Ê");
      if (!singular)  result="√⁄÷«¡";
      return result; 
    }
   
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trField(bool /*first_capital*/, bool singular)
    { 
      QCString result("Õﬁ·");
      if (!singular)  result="ÕﬁÊ·";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGlobal(bool /*first_capital*/, bool singular)
    { 
      QCString result("⁄«„");
      if (!singular)  result="⁄Ê«„";
      return result; 
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    virtual QCString trAuthor(bool /*first_capital*/, bool singular)
    {                                                                         
      QCString result("«·„ƒ·›");
      if (!singular)  result="«·„ƒ·›Ê‰";
      return result; 
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences()
    {
      return "„—Ã⁄Ì« ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of 
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    {
      return "Implements "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      return "Implemented in "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      return "ÃœÊ· «·„Õ ÊÌ« ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been 
     *  flagged deprecated 
     */
    virtual QCString trDeprecatedList()
    {
      return "Deprecated List";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in 
     * a C# program
     */
    virtual QCString trEvents()
    {
      return "«·√Õœ«À";
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      return " ÊÀÌﬁ «·√Õœ«À";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    { 
      return "√‰Ê«⁄ «·Õ“„…";
    }
    /*! Used as a heading for a list of Java class functions with package 
     * scope. 
     */
    virtual QCString trPackageMembers()
    { 
      return "œÊ«· «·Õ“„…";
    }
    /*! Used as a heading for a list of static Java class functions with 
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    { 
      return "œÊ«· ”«ﬂ‰… ··Õ“„…";
    }
    /*! Used as a heading for a list of Java class variables with package 
     * scope.
     */
    virtual QCString trPackageAttribs()
    { 
      return "Œ’«∆’ «·Õ“„…";
    }
    /*! Used as a heading for a list of static Java class variables with 
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    { 
      return "Œ’«∆’ ”«ﬂ‰… ··Õ“„…";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page 
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll()
    {
      return "«·ﬂ·";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph()
    {
		return "Â–« ÂÊ —”„ «·«” œ⁄«¡ ·Â–Â «·œ«·…:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! When the search engine is enabled this text is put in the header 
     *  of each page before the field where one can enter the text to search 
     *  for. 
     */
    virtual QCString trSearchForIndex()
    {
      return "»ÕÀ ⁄‰";
    }
    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle()
    {
      return "‰ «∆Ã «·»ÕÀ";
    }
    /*! This string is put just before listing the search results. The
     *  text can be different depending on the number of documents found.
     *  Inside the text you can put the special marker $num to insert
     *  the number representing the actual number of search results.
     *  The @a numDocuments parameter can be either 0, 1 or 2, where the 
     *  value 2 represents 2 or more matches. HTML markup is allowed inside
     *  the returned string.
     */
    virtual QCString trSearchResults(int numDocuments)
    {
      if (numDocuments==0)
      {
        return "⁄›Ê« ° ·« ÌÊÃœ  ÊÀÌﬁ „·«∆„ ·„« »ÕÀ  ⁄‰Â.";
      }
      else if (numDocuments==1)
      {
        return "ÊÀÌﬁ… „‰«”»… ·„« »ÕÀ  ⁄‰Â. <b>1</b>  „ «·⁄ÀÊ— ⁄·Ï.";
      }
      else 
      {
        return "ÊÀÌﬁ… „ÿ«»ﬁ… ·„« »ÕÀ  ⁄‰Â <b>$num</b>  „ ≈ÌÃ«œ ."
               "Ê „  — Ì»Â„ »ÕÌÀ ÌﬂÊ‰ «·√ﬁ—» ·‰ ÌÃ… «·»ÕÀ √Ê·«.";
      }
    }
    /*! This string is put before the list of matched words, for each search 
     *  result. What follows is the list of words that matched the query.
     */
    virtual QCString trSearchMatches()
    {
		return "Ì ÿ«»ﬁ „⁄:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename)
    {
      return " „·› «·„’œ—" + filename ;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex()
    { return "‘ﬂ· Â—„Ì ··√œ·…"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation()
    { return " ÊÀÌﬁ «·√œ·…"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories()
    { return "«·√œ·…"; }

    /*! This returns a sentences that introduces the directory hierarchy. 
     *  and the fact that it is sorted alphabetically per level
     */
    virtual QCString trDirDescription()
    { return "Â–« «·‘ﬂ· «·Â—„Ì ··√œ·…  „  — Ì»Â √»ÃœÌ« »’Ê—…  ﬁ—Ì»Ì…° "
	"Ê·Ì”  — Ì»« √»ÃœÌ« ﬂ«„·«:";
    }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const char *dirName)
    { QCString result=" „—Ã⁄ «·œ·Ì·"; result+=dirName; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Director" : "director"));
      if (singular) result+="y"; else result+="ies";
      return result; 
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    virtual QCString trOverloadText()
    {
       return "Â–Â œ«·… ⁄÷Ê “«∆œ… «· Õ„Ì· °"
              "ÊÃœ √‰Â« „·«∆„…. ≈‰Â«  Œ ·› ⁄‰ «·œ«·… √⁄·«Â"
              "›ﬁÿ ›Ì ‰Ê⁄Ì… «· argument(s) «· Ì  ﬁ»·Â«.";
    }
};

#endif
