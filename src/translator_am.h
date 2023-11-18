/******************************************************************************
 *
 *
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
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
    virtual QCString idLanguage() override
    { return "armenian"; }

    /* Used to get the command(s) for the language support. */
    virtual QCString latexLanguageSupportCommand() override
    {
      return "\\usepackage[latin]{armtex}\n"
             "\\usepackage[armscii8]{inputenc}\n";
    }
    virtual QCString trISOLang() override
    { return "hy"; }
    virtual QCString getLanguageString() override
    {
      return "0x42b Armenian";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions() override
    { return "Դասին վերաբերվող ֆունկցիաներ"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript() override
    { return "(Հաշվի առեք, որ սրանք անդամ ֆունկցիաներ չեն)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription() override
    { return "Մանրամասն նկարագրություն"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    virtual QCString trDetails() override
    { return "Մանրամասներ"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation() override
    { return "Անդամ տիպի սահմանումներ (typedef)"; }

    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation() override
    { return "Անդամ hամարակալումներ"; }

    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation() override
    { return "Անդամ ֆունկցիաներ"; }

    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Դաշտեր";
      }
      else
      {
        return "Անդամ տվյալներ";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore() override
    { return "Մանրամասն..."; }

    /*! put in the class documentation */
    /* Isn't used when optimization for C is on. */
    virtual QCString trListOfAllMembers() override
    {
      return "Բոլոր անդամների ցուցակը";
    }

    /*! used as the title of the "list of all members" page of a class */
    /* Isn't used when optimization for C is on. */
    virtual QCString trMemberList() override
    {
      return "Անդամների ցուցակ";
    }

    /*! this is the first part of a sentence that is followed by a class name */
    /* Isn't used when optimization for C is on. */
    virtual QCString trThisIsTheListOfAllMembers() override
    { return "Սա դասի անդամների ամբողջական ցուցակն է "; }

    /*! this is the remainder of the sentence after the class name */
    /* Isn't used when optimization for C is on. */
    virtual QCString trIncludingInheritedMembers() override
    { return ", ներառյալ բոլոր ժառանգված անդամները"; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="Ավտոմատ ստեղծված է ելքային կոդից, Doxygen-ի միջոցով, ";
      if (!s.isEmpty()) result+=s+" համար:";
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName() override
    { return "համարակալման անուն"; }

    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue() override
    { return "համարակալման արժեք"; }

    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn() override
    { return "սահմանված"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules() override
    { return "Մոդուլներ"; }

    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy() override
    { return "Դասերի հիերարխա"; }

    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Տվյալների կառուցվածք";
      }
      else
      {
        return "Դասերի ցուցակ";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList() override
    { return "Ֆայլերի ցուցակ"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
    virtual QCString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Գլոբալներ";
      }
      else
      {
        return "Ֆայլի անդամներ";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages() override
    { return "Նմանատիպ էջեր"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples() override
    { return "Օրինակներ"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch() override
    { return "Որոնում"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription() override
    { return "Այս ժառանգման ցուցակը կոպտորեն է տեսակավորված, "
             "բայց ոչ ամբողջապես, այբբենական կարգով.";
	}

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll) override
    {
      QCString result="Բոլոր ";
      if (!extractAll) result+="փաստագրված ";
      result+="ֆայլերի մի ցուցակ` կարճ բացատրություններով:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
    virtual QCString trCompoundMembersDescription(bool extractAll) override
    {
        QCString result="Բոլոր ";
        if(!extractAll) result+="փաստագրված ";
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
          result+="կառուցվածքների և միավորումների դաշտերի ";
        else
          result+="դասի անդամների ";
		result+="ցուցակը`";
		result+=" հղումներով դեպի ";
        if(!extractAll)
        {
          if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
            result+="կառուցվածք/միավորում փաստագրությունները բոլոր անդամների համար.";
          else
            result+="դասի փաստագրությունը բոլոր անդամների համար.";
        }
        else
        {
          if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
            result += "կառուցվածքները/միավորումները, որոնց նրանք պատկանում են.";
          else
            result += "դասերը, որոնց նրանք պատկանում են.";
        }
        return result;
    }

    /*! This is an introduction to the page with all file members. */
    virtual QCString trFileMembersDescription(bool extractAll) override
    {
      QCString result="Բոլոր ";
      if (!extractAll) result+="փաստագրված ";

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
    virtual QCString trExamplesDescription() override
    { return "Բոլոր օրինակների ցուցակը."; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription() override
    { return "Բոլոր նմանատիպ փաստագրության էջերի ցուցակը."; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription() override
    { return "Բոլոր մոդուլների ցուցակը."; }

    // index titles (the project name is prepended for these)


    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation() override
    { return " - Փաստագրություն"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    virtual QCString trModuleIndex() override
    { return "Մոդուլներ"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex() override
    { return "Դասակարգումներ"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    virtual QCString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
    virtual QCString trFileIndex() override
    { return "Ֆայլեր"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation() override
    { return "Մոդուլներ"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Տվյալների կառուցվածքներ";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return "Դասեր";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation() override
    { return "Ֆայլեր"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual() override
    { return "Հղումների ձեռնարկ"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    virtual QCString trDefines() override
    { return "Մակրոսներ"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    virtual QCString trTypedefs() override
    { return "Տիպի սահմանումներ (typedef)"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    virtual QCString trEnumerations() override
    { return "Համարակալումներ"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    virtual QCString trFunctions() override
    { return "Ֆունկցիաներ"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trVariables() override
    { return "Փոփոխականներ"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues() override
    { return "Հաշվիչ"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation() override
    { return "Մակրոսներ"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation() override
    { return "Տիպի սահմանումներ (typedef)"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation() override
    { return "Համարակալման տիպեր"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation() override
    { return "Ֆունկցիաներ"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation() override
    { return "Փոփոխականներ"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
    virtual QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result=QCString("Ստեղծվել է ")+date;
      if (!projName.isEmpty()) result+=projName+" -ի համար,";
      result+=" հետևյալ համակարգով.";
      return result;
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const QCString &clName) override
    {
      return clName+QCString(" -ի ժառանգման գծագիրը.");
    }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning() override
    { return "Զգուշացում"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion() override
    { return "Տարբերակ"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate() override
    { return "Տարեթիվ"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns() override
    { return "Վերադարձնում է"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso() override
    { return "Տեսեք նաև"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters() override
    { return "Պարամետրեր"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions() override
    { return "Բացառություններ"; }

    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy() override
    { return "Ստեղծված է հետևյալ համակարգի կողմից"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList() override
    { return "Անունների տարածությունների ցուցակ"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="Բոլոր ";
      if (!extractAll) result+="փաստագրված ";
      result+="անունների տարածությունների ցուցակը` կարճ բացատրություններով.";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends() override
    { return "Ընկերներ"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    virtual QCString trRelatedFunctionDocumentation() override
    { return "Դասի ընկերներ և կապված ֆունկցիաներ"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const QCString &clName,
                                 ClassDef::CompoundType compType,
                                 bool isTemplate) override
    {
      QCString result=clName;
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
          default: break;
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
          default: break;
        }
      }
      return result;
    }

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const QCString &fileName) override
    {
      return fileName+QCString(" ֆայլեր");
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" անունների տարածություններ";
      return result;
    }

    virtual QCString trPublicMembers() override
    { return "Բաց անդամ ֆունկցիաներ"; }
    virtual QCString trPublicSlots() override
    { return "Բաց սլոթեր"; }
    virtual QCString trSignals() override
    { return "Ազդանշաններ"; }
    virtual QCString trStaticPublicMembers() override
    { return "Բաց ստատիկ անդամ ֆունկցիաներ"; }
    virtual QCString trProtectedMembers() override
    { return "Պաշտպանված անդամ ֆունկցիաներ"; }
    virtual QCString trProtectedSlots() override
    { return "Պաշտպանված սլոթեր"; }
    virtual QCString trStaticProtectedMembers() override
    { return "Պաշտպանված ստատիկ անդամ ֆունկցիաներ"; }
    virtual QCString trPrivateMembers() override
    { return "Փակ ֆունկցիաներ"; }
    virtual QCString trPrivateSlots() override
    { return "Փակ սլոթեր"; }
    virtual QCString trStaticPrivateMembers() override
    { return "Փակ ստատիկ անդամ ֆունկցիաներ"; }

    /*! this function is used to produce a comma-separated list of items.
     *  use generateMarker(i) to indicate where item i should be put.
     */
    virtual QCString trWriteList(int numEntries) override
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
    virtual QCString trInheritsList(int numEntries) override
    {
      return "Հենքային դասեր - "+trWriteList(numEntries)+":";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries) override
    {
      return "Ժառանգորդ դասեր - "+trWriteList(numEntries)+":";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries) override
    {
      return "Վերասահմանված ֆունկցիաներ - "+trWriteList(numEntries)+":";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries) override
    {
      return "Վերասահմանված է "+trWriteList(numEntries)+" ում:";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers() override
    { return "Անունների տարածության անդամներ"; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll) override
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
    virtual QCString trNamespaceIndex() override
    { return "Անունների տարածություններ"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation() override
    { return "Անունների տարածություն"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces() override
    { return "Անունների տարածություններ"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single) override
    {
      QCString result = "Այս ";
      switch(compType)
      {
        case ClassDef::Class:      result+="դասի"; break;
        case ClassDef::Struct:     result+="կառուցվածքի"; break;
        case ClassDef::Union:      result+="միավորման"; break;
        case ClassDef::Interface:  result+="ինտերֆեյսի"; break;
        case ClassDef::Protocol:   result+="արձանագրության"; break;
        case ClassDef::Category:   result+="դասակարգման"; break;
        case ClassDef::Exception:  result+="բացառության"; break;
        default: break;
      }
      result+=" փաստագրությունը ստեղծվել է հետևյալ ֆայլ";
      if (single) result+="ից."; else result+="երից.";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues() override
    { return "Վերադարձվող արժեքներ"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage() override
    { return "Գլխավոր էջ"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation() override
    { return "էջ:"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991106
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDefinedAtLineInSourceFile() override
    {
      return "Սահմանումը @1 ֆայլի @0 տողում է:";
    }
    virtual QCString trDefinedInSourceFile() override
    {
      return "Սահմանումը @0 ֆայլում է:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated() override
    {
      return "Հնացած է";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const QCString &clName) override
    {
      return clName+"-ի համագործակցությունների գծագիր.";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const QCString &fName) override
    {
      return fName+"-ի ներառումների կախվածությունների գծագիր.";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation() override
    {
      return "Կառուցիչներ";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode() override
    {
      return "Տե'ս այս ֆայլի ելքային կոդը";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation() override
    {
      return "Տե'ս այս ֆայլի փաստագրությունը:";
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition() override
    {
      return "Նախապայման";
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition() override
    {
      return "Հետպայման";
    }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant() override
    {
      return "Անփոփոխ";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue() override
    {
      return "Նախնական արժեք";
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode() override
    {
      return "Ելքային կոդ";
    }
    virtual QCString trGraphicalHierarchy() override
    {
      return "Գրաֆիկական դասերի հիերարխիա:";
    }
    virtual QCString trGotoGraphicalHierarchy() override
    {
      return "Տե'ս դասերի գրաֆիկական հիերարխիան:";
    }
    virtual QCString trGotoTextualHierarchy() override
    {
      return "Տե'ս դասերի տեքստային հիերարխիան:";
    }
    virtual QCString trPageIndex() override
    {
      return "էջեր";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    virtual QCString trNote() override
    {
      return "Նշում";
    }
    virtual QCString trPublicTypes() override
    {
      return "Բաց տիպեր";
    }
    virtual QCString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Տվյալների դաշտեր";
      }
      else
      {
        return "Բաց ատրիբուտներ";
      }
    }
    virtual QCString trStaticPublicAttribs() override
    {
      return "Բաց ստատիկ ատրիբուտներ";
    }
    virtual QCString trProtectedTypes() override
    {
      return "Պաշտպանված տիպեր";
    }
    virtual QCString trProtectedAttribs() override
    {
      return "Պաշտպանված ատրիբուտներ";
    }
    virtual QCString trStaticProtectedAttribs() override
    {
      return "Պաշտպանված ստատիկ ատրիբուտներ";
    }
    virtual QCString trPrivateTypes() override
    {
      return "Փակ տիպեր";
    }
    virtual QCString trPrivateAttribs() override
    {
      return "Փակ ատրիբուտներ";
    }
    virtual QCString trStaticPrivateAttribs() override
    {
      return "Փակ ստատիկ ատրիբուտներ";
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    virtual QCString trTodo() override
    /*??*/
    {
      return "Կատարման ենթակա";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList() override
    /*??*/
    {
      return "Խնդիրների ցուցակ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy() override
    {
      return "Օգտագործվում է հետևյալում - ";
    }
    virtual QCString trRemarks() override
    {
      return "Դիտողություններ";
    }
    virtual QCString trAttention() override
    {
      return "Ուշադրություն";
    }
    virtual QCString trInclByDepGraph() override
    {
      return "Այս գրաֆը ցույց է տալիս, թե որ ֆայլերն են "
			"ուղղակի կամ անուղղակի ներառում տվյալ ֆայլը.";
    }
    virtual QCString trSince() override
    /*??*/
    {
      return "Սկսած";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle() override
    {
      return "Լեգենդ";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    virtual QCString trLegendDocs() override
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
        "<p><center><img src=\"graph_legend."+getDotImageExtension()+"\"></center>\n"
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
    virtual QCString trLegend() override
    {
      return "լեգենդ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    virtual QCString trTest() override
    {
      return "Թեստ";
    }
    /*! Used as the header of the test list */
    virtual QCString trTestList() override
    {
      return "Թեստերի ցուցակ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties() override
    {
      return "Հատկություններ";
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation() override
    {
      return "Հատկություններ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Տվյալների կառուցվածք";
      }
      else
      {
        return "Դասեր";
      }
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const QCString &name) override
    {
      return "Փաթեթ "+name;
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription() override
    {
      return "Բոլոր փաթեթները` կարճ բացատրություններով (եթե հասանելի են).";
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages() override
    {
      return "Փաթեթներ";
    }
    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue() override
    {
      return "Արժեքներ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug() override
    {
      return "Սխալ";
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList() override
    {
      return "Սխալների ցուցակ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.6
//////////////////////////////////////////////////////////////////////////
    /*! Used as ansicpg for RTF file */
    virtual QCString trRTFansicp() override
    {
      return "armscii-8";
    }
    /*! Used as ansicpg for RTF fcharset */
    virtual QCString trRTFCharSet() override
    {
      return "0";
    }
    /*! Used as header RTF general index */
    virtual QCString trRTFGeneralIndex() override
    {
      return "Ցուցիչ";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular) override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
    virtual QCString trFile(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Ֆայլ" : "ֆայլ"));
      if (!singular)  result+="եր";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Անունների տարածություն" : "անունների տարածություն"));
      if (!singular)  result+="ներ";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Խ" : "խ"));
      result+=(singular ? "ումբ" : "մբեր");
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Էջ" : "էջ"));
      if (!singular)  result+="եր";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Անդամ" : "անդամ"));
      if (!singular)  result+="ներ";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGlobal(bool first_capital, bool singular) override
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
    virtual QCString trAuthor(bool first_capital, bool singular) override
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
    virtual QCString trReferences() override
    {
      return "Հղումներ - ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries) override
    {
      return "Իրագործում է հետևյալ դաս(եր)ի ֆունկցիաները - "+trWriteList(numEntries)+":";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implementation this member.
     */
    virtual QCString trImplementedInList(int numEntries) override
    {
      return "Իրագործվում է հետևյալում - "+trWriteList(numEntries)+":";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents() override
    {
      return "Բովանդակություն";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    virtual QCString trDeprecatedList() override
    {
      return "Հնացած սահմանումների ցուցակը";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    virtual QCString trEvents() override
    {
      return "Պատահարներ";
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation() override
    {
      return "Պատահարների ցուցակը";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes() override
    {
      return "Փաթեթի տիպեր";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    virtual QCString trPackageFunctions() override
    {
      return "Փաթեթի ֆունկցիաներ";
    }
    virtual QCString trPackageMembers() override
    {
      return "Փաթեթի անդամներ";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    virtual QCString trStaticPackageFunctions() override
    {
      return "Փաթեթի ստատիկ ֆունկցիաներ";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    virtual QCString trPackageAttribs() override
    {
      return "Փաթեթի ատրիբուտներ";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    virtual QCString trStaticPackageAttribs() override
    {
      return "Փաթեթի ստատիկ ատրիբուտներ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll() override
    {
      return "Բոլոր";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph() override
    {
      return "Այս ֆունկցիայի կանչերի գրաֆը.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle() override
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
    virtual QCString trSearchResults(int numDocuments) override
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
    virtual QCString trSearchMatches() override
    {
      return "Որոնման արդյունքներ:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename) override
    {
      return "Ելակետային ֆայլ " + filename;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex() override
    { return "Ֆայլադարանների հիերարխիա"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation() override
    { return "Ֆայլադարաններ"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories() override
    { return "Ֆայլադրաններ"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const QCString &dirName) override
    { QCString result=dirName; result+=" Ֆայլադարան"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool first_capital, bool singular) override
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
    virtual QCString trOverloadText() override
    {
       return "Սա վերաբեռնված ֆունկցիա է` տրամադրված հարմարության համար: "
			  "Այն տարբերվում է նախնականից միայն արգումնետներով:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    virtual QCString trCallerGraph() override
    {
      return "Այս ֆունկցիայի կանչերի գրաֆը.";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation() override
    { return "Համարակալումներ"; }


//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////
    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran() override
    { return "Անդամ ֆունցիաներ/ենթածրագրեր"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    virtual QCString trCompoundListFortran() override
    { return "Տվյալների տիպերի ցուցակը"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran() override
    { return "Տվյալների դաշտեր"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran() override
    { return "Տվյալների տիպերը` կարճ բացատրություններով."; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll) override
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
    virtual QCString trCompoundIndexFortran() override
    { return "Տվյալների տիպեր"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation() override
    { return "Տվյալների տիպեր"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms() override
    { return "Ֆունկցիաներ/ենթածրագրեր"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation() override
    { return "Ֆունկցիաներ/ենթածրագրեր"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes() override
    { return "Տվյալների տիպեր"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList() override
    { return "Մոդուլների ցուցակ"; }

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll) override
    {
      QCString result="Բոլոր";
      if (!extractAll) result+="փաստագրված ";
      result+="մոդուլների ցուցակը` կարճ բացատրություններով.";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    virtual QCString trCompoundReferenceFortran(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result=clName;
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
          default: break;
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
          default: break;
        }
        result+=" Ձևանմուշ";
      }
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    virtual QCString trModuleReference(const QCString &namespaceName) override
    {
      return QCString("Մոդուլ ") + namespaceName;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers() override
    { return "Մոդուլի անդամներ"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll) override
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
    virtual QCString trModulesIndex() override
    { return "Մոդուլներ"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trModule(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Մոդուլ" : "մոդուլ"));
      if (!singular)  result+="ներ";
      return result;
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      QCString result="Այս ";
      switch(compType)
      {
        case ClassDef::Class:      result+="մոդուլի"; break;
        case ClassDef::Struct:     result+="տիպի"; break;
        case ClassDef::Union:      result+="միավորման"; break;
        case ClassDef::Interface:  result+="ինտերֆեյսի"; break;
        case ClassDef::Protocol:   result+="արձանագրության"; break;
        case ClassDef::Category:   result+="դասակարգման"; break;
        case ClassDef::Exception:  result+="բացառության"; break;
        default: break;
      }
      result+=" փաստագրությունը ստեղծվել է հետևալ ֆայլ";
      if (single) result+="ից."; else result+="երից.";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trType(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Տիպ" : "տիպ"));
      if (!singular)  result+="եր";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trSubprogram(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Ե" : "ե"));
      if (singular)  result+="նթածրագիր"; else result+="նթածրագրեր";
      return result;
    }

    /*! C# Type Constraint list */
    virtual QCString trTypeConstraints() override
    {
      return "Տիպերի Սահմանափակումներ";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    virtual QCString trDirRelation(const QCString &name) override
    {
      return QCString(name)+" Կապ";
    }

    /*! Loading message shown when loading search results */
    virtual QCString trLoading() override
    {
      return "Բեռնում...";
    }

    /*! Label used for search results in the global namespace */
    virtual QCString trGlobalNamespace() override
    {
      return "Գլոբալ անունների տարածություն";
    }

    /*! Message shown while searching */
    virtual QCString trSearching() override
    {
      return "Որոնում...";
    }

    /*! Text shown when no search results are found */
    virtual QCString trNoMatches() override
    {
      return "Անարդյունք";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.3 (missing items for the directory pages)
//////////////////////////////////////////////////////////////////////////

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the first column mentions the
     *  source file that has a relation to another file.
     */
    virtual QCString trFileIn(const QCString &name) override
    {
      return "Ֆայլը " + name + " ում";
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    virtual QCString trIncludesFileIn(const QCString &name) override
    {
      return "Ներառում է ֆայլը " + name + " ում";
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
                                DateTimeType includeTime) override
    {
      static const char *days[]   = { "Երկուշաբթի,","Երեքշաբթի,","Չորեքշաբթի,","Հինգշաբթի,",
								"Ուրբաթ,","Շաբաթ,","Կիրակի," };
      static const char *months[] = { "Հունիսի","Փետրվարի","Մարտի","Ապրրիլի","Մայիսի","Հունիսի",
								"Հուլիսի","Օգոստոսի","Սեպտեմբերի","Հոկտեբմերի","Նոյեմբերի","Դեկտեմբերի" };
      QCString sdate;
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Date)
      {
        sdate.sprintf("%s %d %s %d",days[dayOfWeek-1],day,months[month-1],year);
      }
      if (includeTime == DateTimeType::DateTime) sdate += " ";
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Time)
      {
        QCString stime;
        stime.sprintf("%.2d:%.2d:%.2d ",hour,minutes,seconds);
        sdate+=stime;
      }
      return sdate;
    }
    virtual QCString trDayOfWeek(int dayOfWeek, bool, bool full) override
    {
      static const char *days_short[]   = { "Երկ", "Երք", "Չրք", "Հնգ", "Ուր", "Շբթ", "Կիր" };
      static const char *days_full[]    = { "Երկուշաբթի", "Երեքշաբթի", "Չորեքշաբթի", "Հինգշաբթի", "Ուրբաթ", "Շաբաթ", "Կիրակի" };
      return full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
    }
    virtual QCString trMonth(int month, bool, bool full) override
    {
      static const char *months_short[] = { "Հնվ", "Փտվ", "Մրտ", "Ապր", "Մյս", "Հնս", "Հլս", "Օգս", "Սպտ", "Հկտ", "Նյմ", "Դկտ" };
      static const char *months_full[]  = { "Հունվար", "Փետրվար", "Մարտ", "Ապրիլ", "Մայիս", "Հունիս", "Հուլիս", "Օգոստոս", "Սեպտեմբեր", "Հոկտեմբեր", "Նոյեմբեր", "Դեկտեմբեր" };
      return full? months_full[month-1] : months_short[month-1];
    }
    virtual QCString trDayPeriod(int period) override
    {
      static const char *dayPeriod[] = { "AM", "PM" };
      return dayPeriod[period];
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    virtual QCString trCiteReferences() override
    { return "Գրականություն"; }

    /*! Text for copyright paragraph */
    virtual QCString trCopyright() override
    { return "Հեղինակային իրավունք"; }

    /*! Header for the graph showing the directory dependencies */
    virtual QCString trDirDepGraph(const QCString &name) override
    { return name + QCString("-ի ֆայլադարանների կախվածությունների գծագիր:"); }

};
#endif
