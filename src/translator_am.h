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
 */

/*
 * Translation by
 * Armen Tangamyan <armen.tangamyan@anu.edu.au>
 */

#ifndef TRANSLATOR_AM_H
#define TRANSLATOR_AM_H

class TranslatorArmenian : public TranslatorAdapter_1_8_0
{
  public:
    /*! Used for identification of the language. */
    virtual QCString idLanguage()
    { return "armenian"; }

    /* Used to get the command(s) for the language support. */
    virtual QCString latexLanguageSupportCommand()
    {
        return "<pre>\\usepackage[latin]{armtex}\n"
			   "\\usepackage[armscii8]{inputenc}\n</pre>"; 
    }

    /*! return the language charset. This will be used for the HTML output */
    virtual QCString idLanguageCharset()
    {
        return "utf-8"; 
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions()
    { return "Դասին վերաբերվող ֆունկցիաներ"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return "(Հաշվի առեք, որ սրանք անդամ ֆունկցիաներ չեն)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return "Մանրամասն նկարագրություն"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return "Անդամ տիպի սահմանումներ (typedef)"; }

    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return "Անդամ hամարակալումներ"; }

    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation()
    { return "Անդամ ֆունկցիաներ"; }

    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Դաշտեր";
      }
      else
      {
        return "Անդամ տվյալներ";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore()
    { return "Մանրամասն..."; }

    /*! put in the class documentation */
    /* Dosn't use when optimization for C is on. */
    virtual QCString trListOfAllMembers()
    {
      return "Բոլոր անդամների ցուցակը";
    }

    /*! used as the title of the "list of all members" page of a class */
    /* Dosn't use when optimization for C is on. */
    virtual QCString trMemberList()
    {
      return "Անդամների ցուցակ";
    }

    /*! this is the first part of a sentence that is followed by a class name */
    /* Dosn't use when optimization for C is on. */
    virtual QCString trThisIsTheListOfAllMembers()
    { return "Սա դասի անդամների ամբողջական ցուցակն է "; }

    /*! this is the remainder of the sentence after the class name */
    /* Dosn't use when optimization for C is on. */
    virtual QCString trIncludingInheritedMembers()
    { return ", ներառյալ բոլոր ժառանգված անդամները"; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const char *s)
    { QCString result="Ավտոմատ ստեղծված է ելքային կոդից, Doxygen-ի միջոցով, ";
      if (s) result+=s+(QCString)" համար:";
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return "համարակալման անուն"; }

    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return "համարակալման արժեք"; }

    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return "սահմանված"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of 
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules()
    { return "Մոդուլներ"; }

    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return "Դասերի հիերարխա"; }

    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Տվյալների կառուցվածք";
      }
      else
      {
        return "Դասերի ցուցակ";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return "Ֆայլերի ցուցակ"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Տվյալների դաշտեր";
      }
      else
      {
        return "Դասի անդամներ";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    /*??*/
    virtual QCString trFileMembers()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Գլոբալներ"; 
      }
      else
      {
        return "Ֆայլի անդամներ"; 
      }
    }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages()
    { return "Նմանատիպ էջեր"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples()
    { return "Օրինակներ"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch()
    { return "Որոնում"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription()
    { return "Այս ժառանգման ցուցակը կոպտորեն է տեսակավորված, "
             "բայց ոչ ամբողջապես, այբբենական կարգով.";
	}

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result="Բոլոր ";
      if (!extractAll) result+="փաստագրված ";
      result+="ֆայլերի մի ցուցակ` կարճ բացատրություններով:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Տվյալների կառուցվածքը` կարճ բացատրություններով.";
      }
      else
      {
        return "Դասերը, կառուցվածքները, միավորումները "
				"և ինտերֆեյսները` կարճ բացատրություններով.";
      }
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll)
    {
        QCString result="Բոլոր ";
        if(!extractAll) result+="փաստագրված ";
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
          result+="կառուցվածքների և միավորումների դաշտերի ";
        else
          result+="դասի անդամների ";
		result+="ցուցակը`";
		result+=" հղումներով դեպի ";
        if(!extractAll)
        {
          if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
            result+="կառուցվածք/միավորում փաստագրությունները բոլոր անդամների համար.";
          else
            result+="դասի փաստագրությունը բոլոր անդամների համար.";
        }
        else
        {
          if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
            result += "կառուցվածքները/միավորումները, որոնց նրանք պատկանում են.";
          else
            result += "դասերը, որոնց նրանք պատկանում են.";
        }
        return result;
    }

    /*! This is an introduction to the page with all file members. */
    virtual QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Բոլոր ";
      if (!extractAll) result+="փաստագրված ";

      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        result+="ֆունկցիաների, փոփոխականների, մակրո-հայտարարությունների, "
                "համարակալումների և տիպի սահմանումների (typedef)";
      }
      else
      {
        result+="ֆայլի անդամների ";
      }
	  result+="ցուցակը`";
      result+=" հղումներով դեպի ";
      if (extractAll)
        result+="ֆայլերը, որոնց նրանք պատկանում են.";
      else
        result+="փաստագրությունը.";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    virtual QCString trExamplesDescription()
    { return "Բոլոր օրինակների ցուցակը."; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return "Բոլոր նմանատիպ փաստագրության էջերի ցուցակը."; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return "Բոլոր մոդուլների ցուցակը."; }

    // index titles (the project name is prepended for these) 


    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation()
    { return " - Փաստագրություն"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * index of all groups.
     */
    virtual QCString trModuleIndex()
    { return "Մոդուլներ"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex()
    { return "Դասակարգումներ"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index.
     */
    virtual QCString trCompoundIndex()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Տվյալների կառուցվածք";
      }
      else
      {
        return "Դասեր";
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    virtual QCString trFileIndex()
    { return "Ֆայլեր"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation()
    { return "Մոդուլներ"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Տվյալների կառուցվածքներ";
      }
      else
      {
        return "Դասեր";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation()
    { return "Ֆայլեր"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    virtual QCString trExampleDocumentation()
    { return "Օրինակներ"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    virtual QCString trPageDocumentation()
    { return "Էջեր"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual()
    { return "Հղումների ձեռնարկ"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of defines
     */
    virtual QCString trDefines()
    { return "Մակրոսներ"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of function prototypes
     */
    virtual QCString trFuncProtos()
    { return "Ֆունկցիաների նախատիպեր"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of typedefs
     */
    virtual QCString trTypedefs()
    { return "Տիպի սահմանումներ (typedef)"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of enumerations
     */
    virtual QCString trEnumerations()
    { return "Համարակալումներ"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) functions
     */
    virtual QCString trFunctions()
    { return "Ֆունկցիաներ"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trVariables()
    { return "Փոփոխականներ"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues()
    { return "Հաշվիչ"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return "Մակրոսներ"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for function prototypes
     */
    virtual QCString trFunctionPrototypeDocumentation()
    { return "Ֆունկցիաների նախատիպեր"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation()
    { return "Տիպի սահմանումներ (typedef)"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation()
    { return "Համարակալման տիպեր"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation()
    { return "Ֆունկցիաներ"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation()
    { return "Փոփոխականներ"; }

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Տվյալների կառուցվածք";
      }
      else
      { 
        return "Դասեր";
      }

    }

    /*! This is used in the documentation of a group before the list of 
     *  links to documented files
     */
    /*! This is used in the standard footer of each page and indicates when 
     *  the page was generated 
     */
    virtual QCString trGeneratedAt(const char *date,const char *projName)
    { 
      QCString result=(QCString)"Ստեղծվել է "+date;
      if (projName) result+=projName+QCString(" -ի համար,");
      result+=(QCString)" հետևյալ համակարգով.";
      return result;
    }
    /*! This is part of the sentence used in the standard footer of each page.
     */
    virtual QCString trWrittenBy()
    {
      return "Հեղինակ - ";
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const char *clName)
    {
      return clName+QCString(" -ի ժառանգման գծագիրը.");
    }

    /*! this text is generated when the \\internal command is used. */
    virtual QCString trForInternalUseOnly()
    { return "Միայն ներքին օգտագործման համար"; }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning()
    { return "Զգուշացում"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion()
    { return "Տարբերակ"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate()
    { return "Տարեթիվ"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns()
    { return "Վերադարձնում է"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso()
    { return "Տեսեք նաև"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters()
    { return "Պարամետրեր"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions()
    { return "Բացառություններ"; }

    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy()
    { return "Ստեղծված է հետևյալ համակարգի կողմից"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307 
//////////////////////////////////////////////////////////////////////////
    
    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return "Անունների տարածությունների ցուցակ"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Բոլոր ";
      if (!extractAll) result+="փաստագրված ";
      result+="անունների տարածությունների ցուցակը` կարճ բացատրություններով.";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends()
    { return "Ընկերներ"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
    /*! used in the class documentation as a header before the list of all
     * related classes 
     */
    virtual QCString trRelatedFunctionDocumentation()
    { return "Դասի ընկերներ և կապված ֆունկցիաներ"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const char *clName,
                                 ClassDef::CompoundType compType,
                                 bool isTemplate)
    {
		QCString result=(QCString)clName;
		if (isTemplate)
		{		  
		  switch(compType)
		  {
			case ClassDef::Class:      result+=" Դասի"; break;
			case ClassDef::Struct:     result+=" Կառուցվածքի"; break;
			case ClassDef::Union:      result+=" Միավորման"; break;
			case ClassDef::Interface:  result+=" Ինտերֆեյսի"; break;
			case ClassDef::Protocol:   result+=" Արձանագրության"; break;
			case ClassDef::Category:   result+=" Դասակարգման"; break;
			case ClassDef::Exception:  result+=" Բացառության"; break;
		  }
		  result+=" Ձևանմուշներ";
		}
		else
		{
		  switch(compType)
		  {
			case ClassDef::Class:      result+=" Դաս"; break;
			case ClassDef::Struct:     result+=" Կառուցվածք"; break;
			case ClassDef::Union:      result+=" Միավորում"; break;
			case ClassDef::Interface:  result+=" Ինտերֆեյս"; break;
			case ClassDef::Protocol:   result+=" Արձանագրություն"; break;
			case ClassDef::Category:   result+=" Դասակարգում"; break;
			case ClassDef::Exception:  result+=" Բացառություն"; break;
		  }
		}
		return result;
	}

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const char *fileName)
    {
      return fileName+QCString(" ֆայլեր");
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" անունների տարածություններ";
      return result;
    }
    
    virtual QCString trPublicMembers()
    { return "Բաց անդամ ֆունկցիաներ"; }
    virtual QCString trPublicSlots()
    { return "Բաց սլոթեր"; }
    virtual QCString trSignals()
    { return "Ազդանշաններ"; }
    virtual QCString trStaticPublicMembers()
    { return "Բաց ստատիկ անդամ ֆունկցիաներ"; }
    virtual QCString trProtectedMembers()
    { return "Պաշտպանված անդամ ֆունկցիաներ"; }
    virtual QCString trProtectedSlots()
    { return "Պաշտպանված սլոթեր"; }
    virtual QCString trStaticProtectedMembers()
    { return "Պաշտպանված ստատիկ անդամ ֆունկցիաներ"; }
    virtual QCString trPrivateMembers()
    { return "Փակ ֆունկցիաներ"; }
    virtual QCString trPrivateSlots()
    { return "Փակ սլոթեր"; }
    virtual QCString trStaticPrivateMembers()
    { return "Փակ ստատիկ անդամ ֆունկցիաներ"; }
    
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
            result+=" և ";
        }
      }
      return result; 
    }
    
    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries)
    {
      return "Հենքային դասեր - "+trWriteList(numEntries)+":";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries)
    {
      return "Ժառանգորդ դասեր - "+trWriteList(numEntries)+":";
    }

    /*! used in member documentation blocks to produce a list of 
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries)
    {
      return "Վերասահմանված ֆունկցիաներ - "+trWriteList(numEntries)+":";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries)
    {
      return "Վերասահմանված է "+trWriteList(numEntries)+" ում:";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers()
    { return "Անունների տարածության անդամներ"; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
    {
      QCString result="Բոլոր ";
      if (!extractAll) result+="փաստագրված ";
      result+="անունների տարածության անդամների ցուցակը` "
			"հղումներով դեպի ";
      if (extractAll) 
        result+="բոլոր անդամների անունների տարածության փաստագրությունը.";
      else 
        result+="անունների տարածությունը, որին նրանք պատկանում են.";
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the 
     *  index of all namespaces.
     */
    virtual QCString trNamespaceIndex()
    { return "Անունների տարածություններ"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
    { return "Անունների տարածություն"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return "Անունների տարածություններ"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single)
    { 
      QCString result = (QCString)"Այս ";
      switch(compType)
      {
        case ClassDef::Class:      result+="դասի"; break;
        case ClassDef::Struct:     result+="կառուցվածքի"; break;
        case ClassDef::Union:      result+="միավորման"; break;
        case ClassDef::Interface:  result+="ինտերֆեյսի"; break;
        case ClassDef::Protocol:   result+="արձանագրության"; break;
        case ClassDef::Category:   result+="դասակարգման"; break;
        case ClassDef::Exception:  result+="բացառության"; break;
      }
      result+=" փաստագրությունը ստեղծվել է հետևյալ ֆայլ";
      if (single) result+="ից."; else result+="երից.";
      return result;
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    virtual QCString trAlphabeticalList()
    { return "Այբբենական ցուցակ"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues()
    { return "Վերադարձվող արժեքներ"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return "Գլխավոր էջ"; }

    /*! This is used in references to page that are put in the LaTeX 
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation()
    { return "էջ:"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991106
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDefinedAtLineInSourceFile()
    {
      return "Սահմանումը @1 ֆայլի @0 տողում է:";
    }
    virtual QCString trDefinedInSourceFile()
    {
      return "Սահմանումը @0 ֆայլում է:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated()
    {
      return "Հնացած է";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)clName+"-ի համագործակցությունների գծագիր.";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName)
    {
      return (QCString)fName+"-ի ներառումների կախվածությունների գծագիր.";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return "Կառուցիչներ"; 
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    {
      return "Տե'ս այս ֆայլի ելքային կոդը";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return "Տե'ս այս ֆայլի փաստագրությունը:";
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition()
    {
      return "Նախապայման";
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition()
    {
      return "Հետպայման";
    }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant()
    {
      return "Անփոփոխ";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue()
    {
      return "Նախնական արժեք";
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode()
    {
      return "Ելքային կոդ";
    }
    virtual QCString trGraphicalHierarchy()
    {
      return "Գրաֆիկական դասերի հիերարխիա:";
    }
    virtual QCString trGotoGraphicalHierarchy()
    {
      return "Տե'ս դասերի գրաֆիկական հիերարխիան:";
    }
    virtual QCString trGotoTextualHierarchy()
    {
      return "Տե'ս դասերի տեքստային հիերարխիան:";
    }
    virtual QCString trPageIndex()
    {
      return "էջեր";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////
    
    virtual QCString trNote()
    {
      return "Նշում";
    }
    virtual QCString trPublicTypes()
    {
      return "Բաց տիպեր";
    }
    virtual QCString trPublicAttribs()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Տվյալների դաշտեր";
      }
      else
      {
        return "Բաց ատրիբուտներ";
      }
    }
    virtual QCString trStaticPublicAttribs()
    {
      return "Բաց ստատիկ ատրիբուտներ";
    }
    virtual QCString trProtectedTypes()
    {
      return "Պաշտպանված տիպեր";
    }
    virtual QCString trProtectedAttribs()
    {
      return "Պաշտպանված ատրիբուտներ";
    }
    virtual QCString trStaticProtectedAttribs()
    {
      return "Պաշտպանված ստատիկ ատրիբուտներ";
    }
    virtual QCString trPrivateTypes()
    {
      return "Փակ տիպեր";
    }
    virtual QCString trPrivateAttribs()
    {
      return "Փակ ատրիբուտներ";
    }
    virtual QCString trStaticPrivateAttribs()
    {
      return "Փակ ստատիկ ատրիբուտներ";
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    virtual QCString trTodo()
    /*??*/
    {
      return "Կատարման ենթակա";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    /*??*/
    {
      return "Խնդիրների ցուցակ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
      return "Օգտագործվում է հետևյալում - ";
    }
    virtual QCString trRemarks()
    {
      return "Դիտողություններ";
    }
    virtual QCString trAttention()
    {
      return "Ուշադրություն";
    }
    virtual QCString trInclByDepGraph()
    {
      return "Այս գրաֆը ցույց է տալիս, թե որ ֆայլերն են "
			"ուղղակի կամ անուղղակի ներառում տվյալ ֆայլը.";
    }
    virtual QCString trSince()
    /*??*/
    {
      return "Սկսած";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    {
      return "Լեգենդ";
    }
    /*! page explaining how the dot graph's should be interpreted 
     *  The %A in the text below are to prevent link to classes called "A".
     */
    virtual QCString trLegendDocs()
    {
      return 
		"Այս էջը նկարագրում է, թե ինչպես մեկնաբանել doxygen-ի ստեղծած գրաֆները:<p>\n"
        "Դիտարկենք հետևյալ օրինակը.\n"
        "\\code\n"
        "/*! Կրճատման հետևանքով անտեսանելի դաս */\n"
        "class Invisible { };\n\n"
        "/*! Կրճատված դաս, ժառանգությունների հարաբերությունը փակ է */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Չփաստագրված դաս */\n"
        "class Undocumented { };\n\n"
        "/*! Բաց ժառանգում */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! Դասի ձևաչափ */\n"
        "template<class T> class Templ {};\n\n"
        "/*! Պաշտպանված ժառանգում */\n"
        "class ProtectedBase { };\n\n"
        "/*! Փակ ժառանգում */\n"
        "class PrivateBase { };\n\n"
        "/*! Դաս, որը օգտագործվում է Inherited դասի կողմից */\n"
        "class Used { };\n\n"
        "/*! Դաս, որը ժառանգում է մի շարք այլ դասերից */\n"
        "class Inherited : public PublicBase,\n"
        "                  protected ProtectedBase,\n"
        "                  private PrivateBase,\n"
        "                  public Undocumented,\n"
        "                  public Templ<int>\n"
        "{\n"
        "  private:\n"
        "    Used *m_usedClass;\n"
        "};\n"
        "\\endcode\n"
        "Սրանով կստանանք հետևյալ գրաֆը."
        "<p><center><img src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
        "<p>\n"
        "Այս գրաֆի ուղղանկյունները ունեն հետևյալ իմաստը.\n"
        "<ul>\n"
        "<li>%A լցոնվաց մոխրագույն ուղղանկյունը ներկայացնում է այն դասը կամ կառուցվածքը, "
		"որի համար ստեղծվել է տվյալ գրաֆը:</li>\n"
		"<li>%A սև եզրերով ուղղանկյունը նշանակում է փաստագրված դաս կամ կարուցվածք:</li>\n"
		"<li>%A մոխրագույն եզրերով ուղղանկյունը նշանակում է չփաստագրված դաս կամ կառուցվածք:</li>\n"
        "<li>%A կարմիր եզրերով ուղղանկյունը նշանակում է դաս կամ կառուցվածք, որի համար\n"
		" ոչ բոլոր ժառանգում/պարունակում կապերն են ցուցադրված: Գրաֆը կրճատված է, "
		"եթե այն չի տեղավորվում նշված սահմաններում:</li>\n"
        "</ul>\n"
        "Սլաքները ունեն հետևյալ իմաստը.\n"
        "<ul>\n"
		"<li>%A մուգ կապույտ սլաքը օգտագործվում է երկու դասերի միջև բաց ժառանգում "
		"կապը ցուցադրելու համար:</li>\n"
		"<li>%A մուգ կանաչ սլաքը օգտագործվում է պաշտպանված ժառանգման համար:</li>\n"
        "<li>%A մուգ կարմիր սլաքը օգտագործվում է փակ ժառանգման համար:</li>\n"
        "<li>%A մանուշակագույն կետագիծ սլաքը օգտագորշվում է, եթե դասը պարունակվում է"
		"այլ դասում կամ օգտագորշվում է այլ դասի կողմից: Սլաքը պիտակավորվաշ է"
		"փոփոխական(ներ)ով, որի միջոցով մատնանշված դասը կամ կառուցվածքը հասանելի է:</li>\n"
        "<li>Դեզին կետագիծ սլաքը ցույց է տալիս ձևանմուշի օրինակի կապը այն ձևանմուշի հետ, "
		"որից այն իրականցվել է. Սլաքը պիտակավորված է օրինակի ձևանմուշային պարամետրերով:</li>\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    virtual QCString trLegend()
    {
      return "լեգենդ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////
    
    /*! Used as a marker that is put before a test item */
    virtual QCString trTest()
    {
      return "Թեստ";
    }
    /*! Used as the header of the test list */
    virtual QCString trTestList()
    {
      return "Թեստերի ցուցակ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for KDE-2 IDL methods */
    virtual QCString trDCOPMethods()
    {
      return "DCOP անդամ ֆունկցիաներ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties()
    {
      return "Հատկություններ";
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
      return "Հատկություններ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Տվյալների կառուցվածք";
      }
      else
      {
        return "Դասեր";
      }
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const char *name)
    {
      return (QCString)"Փաթեթ "+name;
    }
    /*! Title of the package index page */
    virtual QCString trPackageList()
    {
      return "Փաթեթների ցուցակ";
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return "Բոլոր փաթեթները` կարճ բացատրություններով (եթե հասանելի են).";
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return "Փաթեթներ";
    }
    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue()
    {
      return "Արժեքներ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////
    
    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug()
    {
      return "Սխալ";
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList()
    {
      return "Սխալների ցուցակ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.6
//////////////////////////////////////////////////////////////////////////
    /*! Used as ansicpg for RTF file */
    virtual QCString trRTFansicp()
    {
      return "armscii-8";
    }
    /*! Used as ansicpg for RTF fcharset */
    virtual QCString trRTFCharSet()
    {
      return "0";
    }
    /*! Used as header RTF general index */
    virtual QCString trRTFGeneralIndex()
    {
      return "Ցուցիչ";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular)
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        QCString result((first_capital ? "Տվյալների կառուցվածք" : "տվյալների կառուցվածք"));
        return result; 
      }
      else
      {
        QCString result((first_capital ? "Դաս" : "դաս"));
        if(!singular) result+="եր";
        return result;
      }
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trFile(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Ֆայլ" : "ֆայլ"));
      if (!singular)  result+="եր";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Անունների տարածություն" : "անունների տարածություն"));
      if (!singular)  result+="ներ";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Խ" : "խ"));
      result+=(singular ? "ումբ" : "մբեր");
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Էջ" : "էջ"));
      if (!singular)  result+="եր";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Անդամ" : "անդամ"));
      if (!singular)  result+="ներ";
      return result; 
    }
   
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGlobal(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Գլոբալ" : "գլոբալ"));
      if (!singular)  result+="ներ";
      return result; 
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    virtual QCString trAuthor(bool first_capital, bool singular)
    {                                                                         
      QCString result((first_capital ? "Հեղինակ" : "հեղինակ"));
      if (!singular) result+="ներ";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences()
    {
      return "Հղումներ - ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of 
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    {
      return "Իրագործում է հետևյալ դաս(եր)ի ֆունկցիաները - "+trWriteList(numEntries)+":";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implementation this member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      return "Իրագործվում է հետևյալում - "+trWriteList(numEntries)+":";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      return "Բովանդակություն";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been 
     *  flagged deprecated 
     */
    virtual QCString trDeprecatedList()
    {
      return "Հնացած սահմանումների ցուցակը";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in 
     * a C# program
     */
    virtual QCString trEvents()
    {
      return "Պատահարներ";
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      return "Պատահարների ցուցակը";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    { 
      return "Փաթեթի տիպեր";
    }
    /*! Used as a heading for a list of Java class functions with package 
     * scope. 
     */
    virtual QCString trPackageMembers()
    { 
      return "Փաթեթի ֆունկցիաներ";
    }
    /*! Used as a heading for a list of static Java class functions with 
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    { 
      return "Փաթեթի ստատիկ ֆունկցիաներ";
    }
    /*! Used as a heading for a list of Java class variables with package 
     * scope.
     */
    virtual QCString trPackageAttribs()
    { 
      return "Փաթեթի ատրիբուտներ";
    }
    /*! Used as a heading for a list of static Java class variables with 
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    { 
      return "Փաթեթի ստատիկ ատրիբուտներ";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page 
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll()
    {
      return "Բոլոր";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph()
    {
      return "Այս ֆունկցիայի կանչերի գրաֆը.";
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
      return "Որոնում";
    }
    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle()
    {
      return "Որոնման արդյունքները";
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
        return "Ներեցեք, բայց Ձեր որոնումը արդյունք չտվեց:";
      }
      else if( numDocuments == 1 )
      {
        return "Հայտնաբերվել է 1 փաստաթուղթ:";
      }
      else 
      {
        return "Հայտնաբերվել է <b>$num</b> փաստաթուղթ:"
          "Փաստաթղթերը դասակարգված են ըստ համապասխանության";
      }
    }
    /*! This string is put before the list of matched words, for each search 
     *  result. What follows is the list of words that matched the query.
     */
    virtual QCString trSearchMatches()
    {
      return "Որոնման արդյունքներ:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename)
    {
      return "Ելակետային ֆայլ " + filename;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex()
    { return "Ֆայլադարանների հիերարխիա"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation()
    { return "Ֆայլադարաններ"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories()
    { return "Ֆայլադրաններ"; }

    /*! This returns a sentences that introduces the directory hierarchy. 
     *  and the fact that it is sorted alphabetically per level
     */
    virtual QCString trDirDescription()
    { return "Այս ֆայլադարանների հիերարխիան կարգավորված է կոպտորեն, "
			"բայց ոչ ամբողջապես, այբբենական կարգով."; 
	}

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const char *dirName)
    { QCString result=dirName; result+=" Ֆայլադարան"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Ֆայլադարան" : "ֆայլադարան"));
      if (!singular) result+="ներ";
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
       return "Սա վերաբեռնված ֆունկցիա է` տրամադրված հարմարության համար: "
			  "Այն տարբերվում է նախնականից միայն արգումնետներով:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    virtual QCString trCallerGraph()
    {
      return "Այս ֆունկցիայի կանչերի գրաֆը.";
    }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return "Համարակալումներ"; }


//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////
    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran()
    { return "Անդամ ֆունցիաներ/ենթածրագրեր"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */    
    virtual QCString trCompoundListFortran()
    { return "Տվյալների տիպերի ցուցակը"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran()
    { return "Տվյալների դաշտեր"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran()
    { return "Տվյալների տիպերը` կարճ բացատրություններով."; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll)
    {
      QCString result="Բոլոր ";
      if (!extractAll)
      {
        result+="փաստագրված ";
      }
      result+="տվյալների տիպերի անդամների ցուցակը` հղումներով դեպի ";
      if (!extractAll) 
      {
		result+="բոլոր անդամների տվյալների կառուցվածքի փաստագրությունը";
      }
      else 
      {
        result+="տվյալների տիպերը, որոնց նրանք պատկանում են";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index (Fortran).
     */
    virtual QCString trCompoundIndexFortran()
    { return "Տվյալների տիպեր"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation()
    { return "Տվյալների տիպեր"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms()
    { return "Ֆունկցիաներ/ենթածրագրեր"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation()
    { return "Ֆունկցիաներ/ենթածրագրեր"; }

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes()
    { return "Տվյալների տիպեր"; }
    
    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList()
    { return "Մոդուլների ցուցակ"; }

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll)
    {
      QCString result="Բոլոր";
      if (!extractAll) result+="փաստագրված ";
      result+="մոդուլների ցուցակը` կարճ բացատրություններով.";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    virtual QCString trCompoundReferenceFortran(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result=(QCString)clName;
      if (!isTemplate) 
      {
		switch(compType)
		{
		  case ClassDef::Class:      result+=" Մոդուլ"; break;
		  case ClassDef::Struct:     result+=" Տիպ"; break;
		  case ClassDef::Union:      result+=" Միավորում"; break;
		  case ClassDef::Interface:  result+=" Ինտերֆեյս"; break;
		  case ClassDef::Protocol:   result+=" Արձանագրություն"; break;
		  case ClassDef::Category:   result+=" Դասակարգում"; break;
		  case ClassDef::Exception:  result+=" Բացառություն"; break;
		}
      }
      else
      {
		switch(compType)
		{
		  case ClassDef::Class:      result+=" Մոդուլի"; break;
		  case ClassDef::Struct:     result+=" Տիպի"; break;
		  case ClassDef::Union:      result+=" Միավորման"; break;
		  case ClassDef::Interface:  result+=" Ինտերֆեյսի"; break;
		  case ClassDef::Protocol:   result+=" Արձանագրության"; break;
		  case ClassDef::Category:   result+=" Դասակարգման"; break;
		  case ClassDef::Exception:  result+=" Բացառության"; break;
		}
		result+=" Ձևանմուշ";
      }
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    virtual QCString trModuleReference(const char *namespaceName)
    {
      return QCString("Մոդուլ ") + namespaceName;
    }
    
    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers()
    { return "Մոդուլի անդամներ"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll)
    { 
      QCString result="Մոդուլի բոլոր ";
      if (!extractAll) result+="փաստագրված ";
      result+="անդամների ցուցակը` հղումներով դեպի ";
      if (extractAll) 
      {
        result+="բոլոր անդամների փաստագրությունները.";
      }
      else 
      {
        result+="մոդուլները, որոնց նրանք պատկանում են.";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the 
     *  index of all modules (Fortran).
     */
    virtual QCString trModulesIndex()
    { return "Մոդուլներ"; }
    
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trModule(bool first_capital, bool singular)
    {       
      QCString result((first_capital ? "Մոդուլ" : "մոդուլ"));
      if (!singular)  result+="ներ"; 
      return result; 
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single)
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      QCString result=(QCString)"Այս ";
      switch(compType)
      {
        case ClassDef::Class:      result+="մոդուլի"; break;
        case ClassDef::Struct:     result+="տիպի"; break;
        case ClassDef::Union:      result+="միավորման"; break;
        case ClassDef::Interface:  result+="ինտերֆեյսի"; break;
        case ClassDef::Protocol:   result+="արձանագրության"; break;
        case ClassDef::Category:   result+="դասակարգման"; break;
        case ClassDef::Exception:  result+="բացառության"; break;
      }
      result+=" փաստագրությունը ստեղծվել է հետևալ ֆայլ";
      if (single) result+="ից."; else result+="երից.";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trType(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Տիպ" : "տիպ"));
      if (!singular)  result+="եր";
      return result; 
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trSubprogram(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Ե" : "ե"));
      if (singular)  result+="նթածրագիր"; else result+="նթածրագրեր"; 
      return result; 
    }

    /*! C# Type Constraint list */
    virtual QCString trTypeConstraints()
    {
      return "Տիպերի Սահմանափակումներ";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    virtual QCString trDirRelation(const char *name)
    {
      return QCString(name)+" Կապ";
    }

    /*! Loading message shown when loading search results */
    virtual QCString trLoading()
    {
      return "Բեռնում...";
    }

    /*! Label used for search results in the global namespace */
    virtual QCString trGlobalNamespace()
    {
      return "Գլոբալ անունների տարածություն";
    }

    /*! Message shown while searching */
    virtual QCString trSearching()
    {
      return "Որոնում...";
    }

    /*! Text shown when no search results are found */
    virtual QCString trNoMatches()
    {
      return "Անարդյունք";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.3 (missing items for the directory pages)
//////////////////////////////////////////////////////////////////////////

    /*! introduction text for the directory dependency graph */
    virtual QCString trDirDependency(const char *name)
    {
      return (QCString)name + " -ի կախվածությունների գծագիր";
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the first column mentions the
     *  source file that has a relation to another file.
     */
    virtual QCString trFileIn(const char *name)
    {
      return (QCString)"Ֆայլը " + name + " ում";
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    virtual QCString trIncludesFileIn(const char *name)
    {
      return (QCString)"Ներառում է ֆայլը " + name + " ում";
    }

    /** Compiles a date string.
     *  @param year Year in 4 digits
     *  @param month Month of the year: 1=January
     *  @param day Day of the Month: 1..31
     *  @param dayOfWeek Day of the week: 1=Monday..7=Sunday
     *  @param hour Hour of the day: 0..23
     *  @param minutes Minutes in the hour: 0..59
     *  @param seconds Seconds within the minute: 0..59
     *  @param includeTime Include time in the result string?
     */
    virtual QCString trDateTime(int year,int month,int day,int dayOfWeek,
                                int hour,int minutes,int seconds,
                                bool includeTime)
    {
      static const char *days[]   = { "Երկուշաբթի,","Երեքշաբթի,","Չորեքշաբթի,","Հինգշաբթի,",
								"Ուրբաթ,","Շաբաթ,","Կիրակի," };
      static const char *months[] = { "Հունիսի","Փետրվարի","Մարտի","Ապրրիլի","Մայիսի","Հունիսի",
								"Հուլիսի","Օգոստոսի","Սեպտեմբերի","Հոկտեբմերի","Նոյեմբերի","Դեկտեմբերի" };
      QCString sdate;
      sdate.sprintf("%s %d %s %d",days[dayOfWeek-1],day,months[month-1],year);
      if (includeTime)
      {
        QCString stime;
        stime.sprintf(" %.2d:%.2d:%.2d ",hour,minutes,seconds);
        sdate+=stime;
      }
      return sdate;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    virtual QCString trCiteReferences()
    { return "Գրականություն"; }

    /*! Text for copyright paragraph */
    virtual QCString trCopyright()
    { return "Հեղինակային իրավունք"; }

    /*! Header for the graph showing the directory dependencies */
    virtual QCString trDirDepGraph(const char *name)
    { return name + QCString("-ի ֆայլադարանների կախվածությունների գծագիր:"); }

};

#endif
