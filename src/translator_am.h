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
    QCString idLanguage() override
    { return "armenian"; }

    /* Used to get the command(s) for the language support. */
    QCString latexLanguageSupportCommand() override
    {
      return "\\usepackage[latin]{armtex}\n"
             "\\usepackage[armscii8]{inputenc}\n";
    }
    QCString trISOLang() override
    { return "hy"; }
    QCString getLanguageString() override
    {
      return "0x42b Armenian";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    QCString trRelatedFunctions() override
    { return "Դասին վերաբերվող ֆունկցիաներ"; }

    /*! subscript for the related functions. */
    QCString trRelatedSubscript() override
    { return "(Հաշվի առեք, որ սրանք անդամ ֆունկցիաներ չեն)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    QCString trDetailedDescription() override
    { return "Մանրամասն նկարագրություն"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    QCString trDetails() override
    { return "Մանրամասներ"; }

    /*! header that is put before the list of typedefs. */
    QCString trMemberTypedefDocumentation() override
    { return "Անդամ տիպի սահմանումներ (typedef)"; }

    /*! header that is put before the list of enumerations. */
    QCString trMemberEnumerationDocumentation() override
    { return "Անդամ hամարակալումներ"; }

    /*! header that is put before the list of member functions. */
    QCString trMemberFunctionDocumentation() override
    { return "Անդամ ֆունկցիաներ"; }

    /*! header that is put before the list of member attributes. */
    QCString trMemberDataDocumentation() override
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
    QCString trMore() override
    { return "Մանրամասն..."; }

    /*! put in the class documentation */
    /* Isn't used when optimization for C is on. */
    QCString trListOfAllMembers() override
    {
      return "Բոլոր անդամների ցուցակը";
    }

    /*! used as the title of the "list of all members" page of a class */
    /* Isn't used when optimization for C is on. */
    QCString trMemberList() override
    {
      return "Անդամների ցուցակ";
    }

    /*! this is the first part of a sentence that is followed by a class name */
    /* Isn't used when optimization for C is on. */
    QCString trThisIsTheListOfAllMembers() override
    { return "Սա դասի անդամների ամբողջական ցուցակն է"; }

    /*! this is the remainder of the sentence after the class name */
    /* Isn't used when optimization for C is on. */
    QCString trIncludingInheritedMembers() override
    { return ", ներառյալ բոլոր ժառանգված անդամները"; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="Ավտոմատ ստեղծված է ելքային կոդից, Doxygen-ի միջոցով, ";
      if (!s.isEmpty()) result+=s+" համար:";
      return result;
    }

    /*! put after an enum name in the list of all members */
    QCString trEnumName() override
    { return "համարակալման անուն"; }

    /*! put after an enum value in the list of all members */
    QCString trEnumValue() override
    { return "համարակալման արժեք"; }

    /*! put after an undocumented member in the list of all members */
    QCString trDefinedIn() override
    { return "սահմանված"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    QCString trModules() override
    { return "Մոդուլներ"; }

    /*! This is put above each page as a link to the class hierarchy */
    QCString trClassHierarchy() override
    { return "Դասերի հիերարխա"; }

    /*! This is put above each page as a link to the list of annotated classes */
    QCString trCompoundList() override
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
    QCString trFileList() override
    { return "Ֆայլերի ցուցակ"; }

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers() override
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
    QCString trFileMembers() override
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
    QCString trRelatedPages() override
    { return "Նմանատիպ էջեր"; }

    /*! This is put above each page as a link to all examples. */
    QCString trExamples() override
    { return "Օրինակներ"; }

    /*! This is put above each page as a link to the search engine. */
    QCString trSearch() override
    { return "Որոնում"; }

    /*! This is an introduction to the class hierarchy. */
    QCString trClassHierarchyDescription() override
    { return "Այս ժառանգման ցուցակը կոպտորեն է տեսակավորված, "
             "բայց ոչ ամբողջապես, այբբենական կարգով.";
	}

    /*! This is an introduction to the list with all files. */
    QCString trFileListDescription(bool extractAll) override
    {
      QCString result="Բոլոր ";
      if (!extractAll) result+="փաստագրված ";
      result+="ֆայլերի մի ցուցակ` կարճ բացատրություններով:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    QCString trCompoundListDescription() override
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
    QCString trCompoundMembersDescription(bool extractAll) override
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
    QCString trFileMembersDescription(bool extractAll) override
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
    QCString trExamplesDescription() override
    { return "Բոլոր օրինակների ցուցակը."; }

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription() override
    { return "Բոլոր նմանատիպ փաստագրության էջերի ցուցակը."; }

    /*! This is an introduction to the page with the list of class/file groups */
    QCString trModulesDescription() override
    { return "Բոլոր մոդուլների ցուցակը."; }

    // index titles (the project name is prepended for these)


    /*! This is used in HTML as the title of index.html. */
    QCString trDocumentation(const QCString &projName) override
    { return (!projName.isEmpty()?projName + " " : "") + " - Փաստագրություն"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    QCString trModuleIndex() override
    { return "Մոդուլներ"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    QCString trHierarchicalIndex() override
    { return "Դասակարգումներ"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    QCString trCompoundIndex() override
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
    QCString trFileIndex() override
    { return "Ֆայլեր"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    QCString trModuleDocumentation() override
    { return "Մոդուլներ"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trClassDocumentation() override
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
    QCString trFileDocumentation() override
    { return "Ֆայլեր"; }

    /*! This is used in LaTeX as the title of the document */
    QCString trReferenceManual() override
    { return "Հղումների ձեռնարկ"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    QCString trDefines() override
    { return "Մակրոսներ"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    QCString trTypedefs() override
    { return "Տիպի սահմանումներ (typedef)"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    QCString trEnumerations() override
    { return "Համարակալումներ"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    QCString trFunctions() override
    { return "Ֆունկցիաներ"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trVariables() override
    { return "Փոփոխականներ"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trEnumerationValues() override
    { return "Հաշվիչ"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    QCString trDefineDocumentation() override
    { return "Մակրոսներ"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    QCString trTypedefDocumentation() override
    { return "Տիպի սահմանումներ (typedef)"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation() override
    { return "Համարակալման տիպեր"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    QCString trFunctionDocumentation() override
    { return "Ֆունկցիաներ"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    QCString trVariableDocumentation() override
    { return "Փոփոխականներ"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    QCString trCompounds() override
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
    QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result=QCString("Ստեղծվել է ")+date;
      if (!projName.isEmpty()) result+=projName+" -ի համար,";
      result+=" հետևյալ համակարգով.";
      return result;
    }

    /*! this text is put before a class diagram */
    QCString trClassDiagram(const QCString &clName) override
    {
      return clName+QCString(" -ի ժառանգման գծագիրը.");
    }

    /*! this text is generated when the \\warning command is used. */
    QCString trWarning() override
    { return "Զգուշացում"; }

    /*! this text is generated when the \\version command is used. */
    QCString trVersion() override
    { return "Տարբերակ"; }

    /*! this text is generated when the \\date command is used. */
    QCString trDate() override
    { return "Տարեթիվ"; }

    /*! this text is generated when the \\return command is used. */
    QCString trReturns() override
    { return "Վերադարձնում է"; }

    /*! this text is generated when the \\sa command is used. */
    QCString trSeeAlso() override
    { return "Տեսեք նաև"; }

    /*! this text is generated when the \\param command is used. */
    QCString trParameters() override
    { return "Պարամետրեր"; }

    /*! this text is generated when the \\exception command is used. */
    QCString trExceptions() override
    { return "Բացառություններ"; }

    /*! this text is used in the title page of a LaTeX document. */
    QCString trGeneratedBy() override
    { return "Ստեղծված է հետևյալ համակարգի կողմից"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    QCString trNamespaceList() override
    { return "Անունների տարածությունների ցուցակ"; }

    /*! used as an introduction to the namespace list */
    QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="Բոլոր ";
      if (!extractAll) result+="փաստագրված ";
      result+="անունների տարածությունների ցուցակը` կարճ բացատրություններով.";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    QCString trFriends() override
    { return "Ընկերներ"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    QCString trRelatedFunctionDocumentation() override
    { return "Դասի ընկերներ և կապված ֆունկցիաներ"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    QCString trCompoundReference(const QCString &clName,
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
    QCString trFileReference(const QCString &fileName) override
    {
      return fileName+QCString(" ֆայլեր");
    }

    /*! used as the title of the HTML page of a namespace */
    QCString trNamespaceReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" անունների տարածություններ";
      return result;
    }

    QCString trPublicMembers() override
    { return "Բաց անդամ ֆունկցիաներ"; }
    QCString trPublicSlots() override
    { return "Բաց սլոթեր"; }
    QCString trSignals() override
    { return "Ազդանշաններ"; }
    QCString trStaticPublicMembers() override
    { return "Բաց ստատիկ անդամ ֆունկցիաներ"; }
    QCString trProtectedMembers() override
    { return "Պաշտպանված անդամ ֆունկցիաներ"; }
    QCString trProtectedSlots() override
    { return "Պաշտպանված սլոթեր"; }
    QCString trStaticProtectedMembers() override
    { return "Պաշտպանված ստատիկ անդամ ֆունկցիաներ"; }
    QCString trPrivateMembers() override
    { return "Փակ ֆունկցիաներ"; }
    QCString trPrivateSlots() override
    { return "Փակ սլոթեր"; }
    QCString trStaticPrivateMembers() override
    { return "Փակ ստատիկ անդամ ֆունկցիաներ"; }

    /*! this function is used to produce a comma-separated list of items.
     *  use generateMarker(i) to indicate where item i should be put.
     */
    QCString trWriteList(int numEntries) override
    {
      QCString result;
      // the inherits list contain `numEntries' classes
      for (int i=0;i<numEntries;i++)
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
    QCString trInheritsList(int numEntries) override
    {
      return "Հենքային դասեր - "+trWriteList(numEntries)+":";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritedByList(int numEntries) override
    {
      return "Ժառանգորդ դասեր - "+trWriteList(numEntries)+":";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    QCString trReimplementedFromList(int numEntries) override
    {
      return "Վերասահմանված ֆունկցիաներ - "+trWriteList(numEntries)+":";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    QCString trReimplementedInList(int numEntries) override
    {
      return "Վերասահմանված է "+trWriteList(numEntries)+" ում:";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    QCString trNamespaceMembers() override
    { return "Անունների տարածության անդամներ"; }

    /*! This is an introduction to the page with all namespace members */
    QCString trNamespaceMemberDescription(bool extractAll) override
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
    QCString trNamespaceIndex() override
    { return "Անունների տարածություններ"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation() override
    { return "Անունների տարածություն"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces() override
    { return "Անունների տարածություններ"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
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
    QCString trReturnValues() override
    { return "Վերադարձվող արժեքներ"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage() override
    { return "Գլխավոր էջ"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    QCString trPageAbbreviation() override
    { return "էջ:"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991106
//////////////////////////////////////////////////////////////////////////

    QCString trDefinedAtLineInSourceFile() override
    {
      return "Սահմանումը @1 ֆայլի @0 տողում է:";
    }
    QCString trDefinedInSourceFile() override
    {
      return "Սահմանումը @0 ֆայլում է:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated() override
    {
      return "Հնացած է";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const QCString &clName) override
    {
      return clName+"-ի համագործակցությունների գծագիր.";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const QCString &fName) override
    {
      return fName+"-ի ներառումների կախվածությունների գծագիր.";
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation() override
    {
      return "Կառուցիչներ";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode() override
    {
      return "Տե'ս այս ֆայլի ելքային կոդը";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation() override
    {
      return "Տե'ս այս ֆայլի փաստագրությունը:";
    }
    /*! Text for the \\pre command */
    QCString trPrecondition() override
    {
      return "Նախապայման";
    }
    /*! Text for the \\post command */
    QCString trPostcondition() override
    {
      return "Հետպայման";
    }
    /*! Text for the \\invariant command */
    QCString trInvariant() override
    {
      return "Անփոփոխ";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue() override
    {
      return "Նախնական արժեք";
    }
    /*! Text used the source code in the file index */
    QCString trCode() override
    {
      return "Ելքային կոդ";
    }
    QCString trGraphicalHierarchy() override
    {
      return "Գրաֆիկական դասերի հիերարխիա:";
    }
    QCString trGotoGraphicalHierarchy() override
    {
      return "Տե'ս դասերի գրաֆիկական հիերարխիան:";
    }
    QCString trGotoTextualHierarchy() override
    {
      return "Տե'ս դասերի տեքստային հիերարխիան:";
    }
    QCString trPageIndex() override
    {
      return "էջեր";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    QCString trNote() override
    {
      return "Նշում";
    }
    QCString trPublicTypes() override
    {
      return "Բաց տիպեր";
    }
    QCString trPublicAttribs() override
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
    QCString trStaticPublicAttribs() override
    {
      return "Բաց ստատիկ ատրիբուտներ";
    }
    QCString trProtectedTypes() override
    {
      return "Պաշտպանված տիպեր";
    }
    QCString trProtectedAttribs() override
    {
      return "Պաշտպանված ատրիբուտներ";
    }
    QCString trStaticProtectedAttribs() override
    {
      return "Պաշտպանված ստատիկ ատրիբուտներ";
    }
    QCString trPrivateTypes() override
    {
      return "Փակ տիպեր";
    }
    QCString trPrivateAttribs() override
    {
      return "Փակ ատրիբուտներ";
    }
    QCString trStaticPrivateAttribs() override
    {
      return "Փակ ստատիկ ատրիբուտներ";
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    QCString trTodo() override
    /*??*/
    {
      return "Կատարման ենթակա";
    }
    /*! Used as the header of the todo list */
    QCString trTodoList() override
    /*??*/
    {
      return "Խնդիրների ցուցակ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy() override
    {
      return "Օգտագործվում է հետևյալում - ";
    }
    QCString trRemarks() override
    {
      return "Դիտողություններ";
    }
    QCString trAttention() override
    {
      return "Ուշադրություն";
    }
    QCString trInclByDepGraph() override
    {
      return "Այս գրաֆը ցույց է տալիս, թե որ ֆայլերն են "
			"ուղղակի կամ անուղղակի ներառում տվյալ ֆայլը.";
    }
    QCString trSince() override
    /*??*/
    {
      return "Սկսած";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    QCString trLegendTitle() override
    {
      return "Լեգենդ";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    QCString trLegendDocs() override
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
    QCString trLegend() override
    {
      return "լեգենդ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    QCString trTest() override
    {
      return "Թեստ";
    }
    /*! Used as the header of the test list */
    QCString trTestList() override
    {
      return "Թեստերի ցուցակ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    QCString trProperties() override
    {
      return "Հատկություններ";
    }
    /*! Used as a section header for IDL property documentation */
    QCString trPropertyDocumentation() override
    {
      return "Հատկություններ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    QCString trClasses() override
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
    QCString trPackage(const QCString &name) override
    {
      return "Փաթեթ "+name;
    }
    /*! The description of the package index page */
    QCString trPackageListDescription() override
    {
      return "Բոլոր փաթեթները` կարճ բացատրություններով (եթե հասանելի են).";
    }
    /*! The link name in the Quick links header for each page */
    QCString trPackages() override
    {
      return "Փաթեթներ";
    }
    /*! Text shown before a multi-line define */
    QCString trDefineValue() override
    {
      return "Արժեքներ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    QCString trBug() override
    {
      return "Սխալ";
    }
    /*! Used as the header of the bug list */
    QCString trBugList() override
    {
      return "Սխալների ցուցակ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.6
//////////////////////////////////////////////////////////////////////////
    /*! Used as ansicpg for RTF file */
    QCString trRTFansicp() override
    {
      return "armscii-8";
    }
    /*! Used as ansicpg for RTF fcharset */
    QCString trRTFCharSet() override
    {
      return "0";
    }
    /*! Used as header RTF general index */
    QCString trRTFGeneralIndex() override
    {
      return "Ցուցիչ";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trClass(bool first_capital, bool singular) override
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
    QCString trFile(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Ֆայլ" : "ֆայլ"));
      if (!singular)  result+="եր";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trNamespace(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Անունների տարածություն" : "անունների տարածություն"));
      if (!singular)  result+="ներ";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGroup(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Խ" : "խ"));
      result+=(singular ? "ումբ" : "մբեր");
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trPage(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Էջ" : "էջ"));
      if (!singular)  result+="եր";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trMember(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Անդամ" : "անդամ"));
      if (!singular)  result+="ներ";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGlobal(bool first_capital, bool singular) override
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
    QCString trAuthor(bool first_capital, bool singular) override
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
    QCString trReferences() override
    {
      return "Հղումներ - ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    QCString trImplementedFromList(int numEntries) override
    {
      return "Իրագործում է հետևյալ դաս(եր)ի ֆունկցիաները - "+trWriteList(numEntries)+":";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implementation this member.
     */
    QCString trImplementedInList(int numEntries) override
    {
      return "Իրագործվում է հետևյալում - "+trWriteList(numEntries)+":";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    QCString trRTFTableOfContents() override
    {
      return "Բովանդակություն";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    QCString trDeprecatedList() override
    {
      return "Հնացած սահմանումների ցուցակը";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    QCString trEvents() override
    {
      return "Պատահարներ";
    }
    /*! Header used for the documentation section of a class' events. */
    QCString trEventDocumentation() override
    {
      return "Պատահարների ցուցակը";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    QCString trPackageTypes() override
    {
      return "Փաթեթի տիպեր";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    QCString trPackageFunctions() override
    {
      return "Փաթեթի ֆունկցիաներ";
    }
    QCString trPackageMembers() override
    {
      return "Փաթեթի անդամներ";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    QCString trStaticPackageFunctions() override
    {
      return "Փաթեթի ստատիկ ֆունկցիաներ";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    QCString trPackageAttribs() override
    {
      return "Փաթեթի ատրիբուտներ";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    QCString trStaticPackageAttribs() override
    {
      return "Փաթեթի ստատիկ ատրիբուտներ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    QCString trAll() override
    {
      return "Բոլոր";
    }
    /*! Put in front of the call graph for a function. */
    QCString trCallGraph() override
    {
      return "Այս ֆունկցիայի կանչերի գրաֆը.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    QCString trSearchResultsTitle() override
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
    QCString trSearchResults(int numDocuments) override
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
    QCString trSearchMatches() override
    {
      return "Որոնման արդյունքներ:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    QCString trSourceFile(const QCString& filename) override
    {
      return "Ելակետային ֆայլ " + filename;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    QCString trDirIndex() override
    { return "Ֆայլադարանների հիերարխիա"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    QCString trDirDocumentation() override
    { return "Ֆայլադարաններ"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    QCString trDirectories() override
    { return "Ֆայլադրաններ"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    QCString trDirReference(const QCString &dirName) override
    { QCString result=dirName; result+=" Ֆայլադարան"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    QCString trDir(bool first_capital, bool singular) override
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
    QCString trOverloadText() override
    {
       return "Սա վերաբեռնված ֆունկցիա է` տրամադրված հարմարության համար: "
			  "Այն տարբերվում է նախնականից միայն արգումնետներով:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    QCString trCallerGraph() override
    {
      return "Այս ֆունկցիայի կանչերի գրաֆը.";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    QCString trEnumerationValueDocumentation() override
    { return "Համարակալումներ"; }


//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////
    /*! header that is put before the list of member subprograms (Fortran). */
    QCString trMemberFunctionDocumentationFortran() override
    { return "Անդամ ֆունցիաներ/ենթածրագրեր"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    QCString trCompoundListFortran() override
    { return "Տվյալների տիպերի ցուցակը"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    QCString trCompoundMembersFortran() override
    { return "Տվյալների դաշտեր"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    QCString trCompoundListDescriptionFortran() override
    { return "Տվյալների տիպերը` կարճ բացատրություններով."; }

    /*! This is an introduction to the page with all data types (Fortran). */
    QCString trCompoundMembersDescriptionFortran(bool extractAll) override
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
    QCString trCompoundIndexFortran() override
    { return "Տվյալների տիպեր"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    QCString trTypeDocumentation() override
    { return "Տվյալների տիպեր"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    QCString trSubprograms() override
    { return "Ֆունկցիաներ/ենթածրագրեր"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    QCString trSubprogramDocumentation() override
    { return "Ֆունկցիաներ/ենթածրագրեր"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     QCString trDataTypes() override
    { return "Տվյալների տիպեր"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    QCString trModulesList() override
    { return "Մոդուլների ցուցակ"; }

    /*! used as an introduction to the modules list (Fortran) */
    QCString trModulesListDescription(bool extractAll) override
    {
      QCString result="Բոլոր";
      if (!extractAll) result+="փաստագրված ";
      result+="մոդուլների ցուցակը` կարճ բացատրություններով.";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    QCString trCompoundReferenceFortran(const QCString &clName,
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
    QCString trModuleReference(const QCString &namespaceName) override
    {
      return QCString("Մոդուլ ") + namespaceName;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    QCString trModulesMembers() override
    { return "Մոդուլի անդամներ"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    QCString trModulesMemberDescription(bool extractAll) override
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
    QCString trModulesIndex() override
    { return "Մոդուլներ"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trModule(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Մոդուլ" : "մոդուլ"));
      if (!singular)  result+="ներ";
      return result;
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
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
    QCString trType(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Տիպ" : "տիպ"));
      if (!singular)  result+="եր";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trSubprogram(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Ե" : "ե"));
      if (singular)  result+="նթածրագիր"; else result+="նթածրագրեր";
      return result;
    }

    /*! C# Type Constraint list */
    QCString trTypeConstraints() override
    {
      return "Տիպերի Սահմանափակումներ";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    QCString trDirRelation(const QCString &name) override
    {
      return QCString(name)+" Կապ";
    }

    /*! Loading message shown when loading search results */
    QCString trLoading() override
    {
      return "Բեռնում...";
    }

    /*! Label used for search results in the global namespace */
    QCString trGlobalNamespace() override
    {
      return "Գլոբալ անունների տարածություն";
    }

    /*! Message shown while searching */
    QCString trSearching() override
    {
      return "Որոնում...";
    }

    /*! Text shown when no search results are found */
    QCString trNoMatches() override
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
    QCString trFileIn(const QCString &name) override
    {
      return "Ֆայլը " + name + " ում";
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    QCString trIncludesFileIn(const QCString &name) override
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
    QCString trDateTime(int year,int month,int day,int dayOfWeek,
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
    QCString trDayOfWeek(int dayOfWeek, bool, bool full) override
    {
      static const char *days_short[]   = { "Երկ", "Երք", "Չրք", "Հնգ", "Ուր", "Շբթ", "Կիր" };
      static const char *days_full[]    = { "Երկուշաբթի", "Երեքշաբթի", "Չորեքշաբթի", "Հինգշաբթի", "Ուրբաթ", "Շաբաթ", "Կիրակի" };
      return full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
    }
    QCString trMonth(int month, bool, bool full) override
    {
      static const char *months_short[] = { "Հնվ", "Փտվ", "Մրտ", "Ապր", "Մյս", "Հնս", "Հլս", "Օգս", "Սպտ", "Հկտ", "Նյմ", "Դկտ" };
      static const char *months_full[]  = { "Հունվար", "Փետրվար", "Մարտ", "Ապրիլ", "Մայիս", "Հունիս", "Հուլիս", "Օգոստոս", "Սեպտեմբեր", "Հոկտեմբեր", "Նոյեմբեր", "Դեկտեմբեր" };
      return full? months_full[month-1] : months_short[month-1];
    }
    QCString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "AM", "PM" };
      return dayPeriod[period?1:0];
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    QCString trCiteReferences() override
    { return "Գրականություն"; }

    /*! Text for copyright paragraph */
    QCString trCopyright() override
    { return "Հեղինակային իրավունք"; }

    /*! Header for the graph showing the directory dependencies */
    QCString trDirDepGraph(const QCString &name) override
    { return name + QCString("-ի ֆայլադարանների կախվածությունների գծագիր:"); }

};
#endif
