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
  public:

    // --- Language control methods -------------------

    /*! Used for identification of the language. The identification
     * should not be translated. It should be replaced by the name
     * of the language in English using lower-case characters only
     * (e.g. "czech", "japanese", "russian", etc.). It should be equal to
     * the identification used in language.cpp.
     */
    QCString idLanguage() override
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
    QCString latexLanguageSupportCommand() override
    {
      return "";
    }

    QCString latexCommandName() override
    {
      return p_latexCommandName("xelatex");
    }

    QCString trISOLang() override
    { return "ar-EG"; }
    QCString getLanguageString() override
    {
      return "0xC01 Arabic (Egypt)";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    QCString trRelatedFunctions() override
    { return "دوال ذات صلة"; }

    /*! subscript for the related functions. */
    QCString trRelatedSubscript() override
    { return "(لاحظ أن هذه الدوال ليست أعضاء)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    QCString trDetailedDescription() override
    { return "وصف تفصيلي"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    QCString trDetails() override
    { return "التفاصيل"; }

    /*! header that is put before the list of typedefs. */
    QCString trMemberTypedefDocumentation() override
    { return "توثيق تعريفات النوع الأعضاء"; }

    /*! header that is put before the list of enumerations. */
    QCString trMemberEnumerationDocumentation() override
    { return "توثيق العدديات الأعضاء"; }

    /*! header that is put before the list of member functions. */
    QCString trMemberFunctionDocumentation() override
    { return "توثيق الدوال الأعضاء"; }

    /*! header that is put before the list of member attributes. */
    QCString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "توثيق الحقل";
      }
      else
      {
        return "توثيق البيان العضو";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    QCString trMore() override
    { return "المزيد ..."; }

    /*! put in the class documentation */
    QCString trListOfAllMembers() override
    { return "اعرض كل الأعضاء"; }

    /*! used as the title of the "list of all members" page of a class */
    QCString trMemberList() override
    { return "قائمة الأعضاء"; }

    /*! this is the first part of a sentence that is followed by a class name */
    QCString trThisIsTheListOfAllMembers() override
    { return "هذه فائمة بكل الأعضاء في"; }

    /*! this is the remainder of the sentence after the class name */
    QCString trIncludingInheritedMembers() override
    { return ", متضمنة كل الأعضاء الموروثة"; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="تم تكوينها آليا بواسطة Doxygen";
      if (!s.isEmpty()) result+=" لـ "+s;
      result+=" من ملفات المصدر.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    QCString trEnumName() override
    { return "الإسم العددي"; }

    /*! put after an enum value in the list of all members */
    QCString trEnumValue() override
    { return "القيمة العددية"; }

    /*! put after an undocumented member in the list of all members */
    QCString trDefinedIn() override
    { return "معرف في"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    QCString trModules() override
    { return "مكونات"; }

    /*! This is put above each page as a link to the class hierarchy */
    QCString trClassHierarchy() override
    { return "الشكل الهرمي للفئة"; }

    /*! This is put above each page as a link to the list of annotated classes */
    QCString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "هياكل البيانات";
      }
      else
      {
        return "قائمة الفئات";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    QCString trFileList() override
    { return "قائمة الملفات"; }

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "حقول البيانات";
      }
      else
      {
        return "أعضاء الفئة";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    QCString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "العوام";
      }
      else
      {
        return "أعضاء الملف";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    QCString trRelatedPages() override
    { return "صفحات ذات صلة"; }

    /*! This is put above each page as a link to all examples. */
    QCString trExamples() override
    { return "أمثلة"; }

    /*! This is put above each page as a link to the search engine. */
    QCString trSearch() override
    { return "بحث"; }

    /*! This is an introduction to the class hierarchy. */
    QCString trClassHierarchyDescription() override
    { return "قائمة التوارث هذه تم ترتيبها أبجديا بصورة تقريبية ، "
             "ولكن ليس بصورة تامة:";
    }

    /*! This is an introduction to the list with all files. */
    QCString trFileListDescription(bool extractAll) override
    {
      QCString result="هذه قائمة بكل الملفات";
      if (!extractAll) result+="الموثقة ";
	  result+="مع وصف مختصر :";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    QCString trCompoundListDescription() override
    {

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "هذه هي هياكل البيانات مع وصف مختصر:";
      }
      else
      {
        return "هذه هي الفئات ، البناءات ،"
			"الإتحادات والواجهات مع وصف مختصر:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    QCString trCompoundMembersDescription(bool extractAll) override
    {
      QCString result="هذه قائمة بكل ";

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="حقول البناءات والإتحادات";
      }
      else
      {
        result+="أعضاء الفئة";
      }
	  if (!extractAll)
      {
        result+=" الموثقة ";
      }
      result+=" مع وصلات إلى ";
      if (!extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="توثيق البناء/الإتحاد لكل حقل:";
        }
        else
        {
          result+="توثيق الفئة لكل عضو:";
        }
      }
      else
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="البناءات/الإتحادات التي ينتمون إليها:";
        }
        else
        {
          result+="الفئات التي ينتمون إليها:";
        }
      }
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    QCString trFileMembersDescription(bool extractAll) override
    {
      QCString result="Here is a list of all ";
      if (!extractAll) result+="documented ";

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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

    /*! This is an introduction to the page with the list of all examples */
    QCString trExamplesDescription() override
    { return "هذه قائمة بكل الأمثلة:"; }

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription() override
    { return "هذه قائمة بكل صفحات التوثيق ذات الصلة:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    QCString trModulesDescription() override
    { return "هذه قائمة بكل المكونات:"; }


    /*! This is used in HTML as the title of index.html. */
    QCString trDocumentation(const QCString &projName) override
    { return (!projName.isEmpty()?projName + " " : "") + "التوثيق"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    QCString trModuleIndex() override
    { return "فهرس المكونات"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    QCString trHierarchicalIndex() override
    { return "الفهرس الهرمي"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    QCString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "فهرس هيكل البيانات";
      }
      else
      {
        return "فهرس الفئة";
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    QCString trFileIndex() override
    { return "فهرس الملفات"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    QCString trModuleDocumentation() override
    { return "توثيق المكون"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "فهرس هيكل البيانات";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return "فهرس الفئة";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    QCString trFileDocumentation() override
    { return "توثيق الملفات"; }

    /*! This is used in LaTeX as the title of the document */
    QCString trReferenceManual() override
    { return "الكتيب المرجعي"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    QCString trDefines() override
    { return "التعريفات"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    QCString trTypedefs() override
    { return "Typedefs"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    QCString trEnumerations() override
    { return "العدديات"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    QCString trFunctions() override
    { return "الدوال"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trVariables() override
    { return "المتغيرات"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trEnumerationValues() override
    { return "Enumerator"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    QCString trDefineDocumentation() override
    { return "Define Documentation"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    QCString trTypedefDocumentation() override
    { return "Typedef Documentation"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation() override
    { return "Enumeration Type Documentation"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    QCString trEnumerationValueDocumentation() override
    { return "Enumerator Documentation"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    QCString trFunctionDocumentation() override
    { return "توثيق الدالة"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    QCString trVariableDocumentation() override
    { return "توثيق المتغير"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    QCString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "هياكل البيانات";
      }
      else
      {
        return "الفئات";
      }
    }

    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result="Generated on "+date;
      if (!projName.isEmpty()) result+=" for "+projName;
      result+=" by";
      return result;
    }

    /*! this text is put before a class diagram */
    QCString trClassDiagram(const QCString &clName) override
    {
      return "Inheritance diagram for "+clName+":";
    }

    /*! this text is generated when the \\warning command is used. */
    QCString trWarning() override
    { return "تنبيه"; }

    /*! this text is generated when the \\version command is used. */
    QCString trVersion() override
    { return "إصدارة"; }

    /*! this text is generated when the \\date command is used. */
    QCString trDate() override
    { return "تاريخ"; }

    /*! this text is generated when the \\return command is used. */
    QCString trReturns() override
    { return "Returns"; }

    /*! this text is generated when the \\sa command is used. */
    QCString trSeeAlso() override
    { return "انظر أيضا"; }

    /*! this text is generated when the \\param command is used. */
    QCString trParameters() override
    { return "Parameters"; }

    /*! this text is generated when the \\exception command is used. */
    QCString trExceptions() override
    { return "استثناءات"; }

    /*! this text is used in the title page of a LaTeX document. */
    QCString trGeneratedBy() override
    { return "انتجت بواسطة"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    QCString trNamespaceList() override
    { return "قائمة مجالات الأسماء"; }

    /*! used as an introduction to the namespace list */
    QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="Here is a list of all ";
      if (!extractAll) result+="documented ";
      result+="namespaces with brief descriptions:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    QCString trFriends() override
    { return "Friends"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    QCString trRelatedFunctionDocumentation() override
    { return "Friends And Related Function Documentation"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    QCString trCompoundReference(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result=clName;
      switch(compType)
      {
        case ClassDef::Class:      result+=" Class"; break;
        case ClassDef::Struct:     result+=" Struct"; break;
        case ClassDef::Union:      result+=" Union"; break;
        case ClassDef::Interface:  result+=" Interface"; break;
        case ClassDef::Protocol:   result+=" Protocol"; break;
        case ClassDef::Category:   result+=" Category"; break;
        case ClassDef::Exception:  result+=" Exception"; break;
        default: break;
      }
      if (isTemplate) result+=" Template";
      result+=" Reference";
      return result;
    }

    /*! used as the title of the HTML page of a file */
    QCString trFileReference(const QCString &fileName) override
    {
      QCString result=fileName;
      result+=" File Reference";
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    QCString trNamespaceReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" Namespace Reference";
      return result;
    }

    QCString trPublicMembers() override
    { return "Public Member Functions"; }
    QCString trPublicSlots() override
    { return "Public Slots"; }
    QCString trSignals() override
    { return "Signals"; }
    QCString trStaticPublicMembers() override
    { return "Static Public Member Functions"; }
    QCString trProtectedMembers() override
    { return "Protected Member Functions"; }
    QCString trProtectedSlots() override
    { return "Protected Slots"; }
    QCString trStaticProtectedMembers() override
    { return "Static Protected Member Functions"; }
    QCString trPrivateMembers() override
    { return "Private Member Functions"; }
    QCString trPrivateSlots() override
    { return "Private Slots"; }
    QCString trStaticPrivateMembers() override
    { return "Static Private Member Functions"; }

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
            result+=", and ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritsList(int numEntries) override
    {
      return "Inherits "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritedByList(int numEntries) override
    {
      return "Inherited by "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    QCString trReimplementedFromList(int numEntries) override
    {
      return "Reimplemented from "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    QCString trReimplementedInList(int numEntries) override
    {
      return "Reimplemented in "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    QCString trNamespaceMembers() override
    { return "Namespace Members"; }

    /*! This is an introduction to the page with all namespace members */
    QCString trNamespaceMemberDescription(bool extractAll) override
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
    QCString trNamespaceIndex() override
    { return "فهرس مجالات الأسماء"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation() override
    { return "توثيق مجالات الأسماء"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces() override
    { return "مجالات الأسماء"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      QCString result="توثيق   ";
      switch(compType)
      {
        case ClassDef::Class:      result+="هذه الفئة"; break;
        case ClassDef::Struct:     result+="هذا البناء"; break;
        case ClassDef::Union:      result+="هذا الإتحاد"; break;
        case ClassDef::Interface:  result+="هذه الواجهة"; break;
        case ClassDef::Protocol:   result+="هذا البروتوكول"; break;
        case ClassDef::Category:   result+="هذا التصنيف"; break;
        case ClassDef::Exception:  result+="هذا الإستثناء"; break;
        default: break;
      }
      result+=" تم تكوينه من ";
	  if (single) result+="هذا الملف:"; else result+="هذه الملفات:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    QCString trReturnValues() override
    { return "قيم العودة"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage() override
    { return "الصفحة الرئيسية"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    QCString trPageAbbreviation() override
    { return "ص."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    QCString trDefinedAtLineInSourceFile() override
    {
      return "Definition at line @0 of file @1.";
    }
    QCString trDefinedInSourceFile() override
    {
      return "Definition in file @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated() override
    {
      return "Deprecated";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const QCString &clName) override
    {
      return "رسم التعاون لـ "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const QCString &fName) override
    {
      return "رسم اعتمادية التضمين لـ "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation() override
    {
      return "توثيق دوال البناء والهدم";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode() override
    {
      return "اذهب إلى الكود الخاص بهذا الملف.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation() override
    {
      return "اذهب إلى توثيق هذا الملف.";
    }
    /*! Text for the \\pre command */
    QCString trPrecondition() override
    {
      return "شرط سابق";
    }
    /*! Text for the \\post command */
    QCString trPostcondition() override
    {
      return "شرط لاحق";
    }
    /*! Text for the \\invariant command */
    QCString trInvariant() override
    {
      return "ثابت";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue() override
    {
		return "قيمة مبدئية:";
    }
    /*! Text used the source code in the file index */
    QCString trCode() override
    {
      return "كود";
    }
    QCString trGraphicalHierarchy() override
    {
      return "الشكل الرسومي للفئات";
    }
    QCString trGotoGraphicalHierarchy() override
    {
      return "اذهب إلى الشكل الهرمي الرسومي للفئات";
    }
    QCString trGotoTextualHierarchy() override
    {
      return "اذهب إلى الشكل الهرمي النصي للفئات";
    }
    QCString trPageIndex() override
    {
      return "فهرس الصفحات";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    QCString trNote() override
    {
      return "ملاحظات";
    }
    QCString trPublicTypes() override
    {
      return "أنواع عامة";
    }
    QCString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "حقول بيانات";
      }
      else
      {
        return "صفات عامة";
      }
    }
    QCString trStaticPublicAttribs() override
    {
      return "صفات ساكنة عامة";
    }
    QCString trProtectedTypes() override
    {
      return "أنواع محمية";
    }
    QCString trProtectedAttribs() override
    {
      return "صفات محمية";
    }
    QCString trStaticProtectedAttribs() override
    {
      return "صفات ساكنة محمية";
    }
    QCString trPrivateTypes() override
    {
      return "أنواع خاصة";
    }
    QCString trPrivateAttribs() override
    {
      return "صفات خاصة";
    }
    QCString trStaticPrivateAttribs() override
    {
      return "صفات ساكنة خاصة";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    QCString trTodo() override
    {
      return "مهام";
    }
    /*! Used as the header of the todo list */
    QCString trTodoList() override
    {
      return "قائمة المهام";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy() override
    {
      return "ذكر بواسطة";
    }
    QCString trRemarks() override
    {
      return "تعليقات";
    }
    QCString trAttention() override
    {
      return "انتبه";
    }
    QCString trInclByDepGraph() override
    {
      return "هذا الرسم يوضح الملفات التي ضمنت هذا الملف"
             "بصورة مباشرة أو غير مباشرة:";
    }
    QCString trSince() override
    {
      return "منذ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    QCString trLegendTitle() override
    {
      return "Graph Legend";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    QCString trLegendDocs() override
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
        "<p><center><img alt=\"\" src=\"graph_legend."+getDotImageExtension()+"\"></center>\n"
        "<p>\n"
        "The boxes in the above graph have the following meaning:\n"
        "<ul>\n"
        "<li>%A filled black box represents the struct or class for which the "
        "graph is generated.\n"
        "<li>%A box with a black border denotes a documented struct or class.\n"
        "<li>%A box with a gray border denotes an undocumented struct or class.\n"
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
    QCString trLegend() override
    {
      return "legend";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    QCString trTest() override
    {
      return "اختبار";
    }
    /*! Used as the header of the test list */
    QCString trTestList() override
    {
      return "قائمة الإختبارات";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    QCString trProperties() override
    {
      return "خصائص";
    }
    /*! Used as a section header for IDL property documentation */
    QCString trPropertyDocumentation() override
    {
      return "توثيق الخاصية";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java interfaces in the summary section of Java packages */
    QCString trInterfaces() override
    {
      return "واجهات";
    }
    /*! Used for Java classes in the summary section of Java packages */
    QCString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "هياكل البيانات";
      }
      else
      {
        return "فئات";
      }
    }
    /*! Used as the title of a Java package */
    QCString trPackage(const QCString &name) override
    {
      return "حزمة "+name;
    }
    /*! The description of the package index page */
    QCString trPackageListDescription() override
    {
		return "هذه هي الحزم مع وصف مختصر لكل منها )إن وجد( :";
    }
    /*! The link name in the Quick links header for each page */
    QCString trPackages() override
    {
      return "حزم";
    }
    /*! Text shown before a multi-line define */
    QCString trDefineValue() override
    {
		return "القيمة:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    QCString trBug() override
    {
      return "ثغرة";
    }
    /*! Used as the header of the bug list */
    QCString trBugList() override
    {
      return "قائمة الثغرات";
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
    QCString trRTFansicp() override
    {
      return "1256";
    }


    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    QCString trRTFCharSet() override
    {
      return "178";
    }

    /*! Used as header RTF general index */
    QCString trRTFGeneralIndex() override
    {
      return "فهرس";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trClass(bool /*first_capital*/, bool singular) override
    {
      QCString result("فئة");
      if (!singular)  result="فئات";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trFile(bool /*first_capital*/, bool singular) override
    {
      QCString result("ملف");
      if (!singular)  result="ملفات";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trNamespace(bool /*first_capital*/, bool singular) override
    {
      QCString result("مجال أسماء");
      if (!singular)  result="مجالات أسماء";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGroup(bool /*first_capital*/, bool singular) override
    {
      QCString result("مجموعة");
      if (!singular)  result="مجموعات";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trPage(bool /*first_capital*/, bool singular) override
    {
      QCString result("صفحة");
      if (!singular)  result="صفحات";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trMember(bool /*first_capital*/, bool singular) override
    {
      QCString result("عضو");
      if (!singular)  result="أعضاء";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGlobal(bool /*first_capital*/, bool singular) override
    {
      QCString result("عام");
      if (!singular)  result="عوام";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    QCString trAuthor(bool /*first_capital*/, bool singular) override
    {
      QCString result("المؤلف");
      if (!singular)  result="المؤلفون";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    QCString trReferences() override
    {
      return "مرجعيات";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    QCString trImplementedFromList(int numEntries) override
    {
      return "Implements "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    QCString trImplementedInList(int numEntries) override
    {
      return "Implemented in "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    QCString trRTFTableOfContents() override
    {
      return "جدول المحتويات";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    QCString trDeprecatedList() override
    {
      return "Deprecated List";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    QCString trEvents() override
    {
      return "الأحداث";
    }
    /*! Header used for the documentation section of a class' events. */
    QCString trEventDocumentation() override
    {
      return "توثيق الأحداث";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    QCString trPackageTypes() override
    {
      return "أنواع الحزمة";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    QCString trPackageFunctions() override
    {
      return "دوال الحزمة";
    }
    QCString trPackageMembers() override
    {
      return "أعضاء الحزمة";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    QCString trStaticPackageFunctions() override
    {
      return "دوال ساكنة للحزمة";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    QCString trPackageAttribs() override
    {
      return "خصائص الحزمة";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    QCString trStaticPackageAttribs() override
    {
      return "خصائص ساكنة للحزمة";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    QCString trAll() override
    {
      return "الكل";
    }
    /*! Put in front of the call graph for a function. */
    QCString trCallGraph() override
    {
		return "هذا هو رسم الاستدعاء لهذه الدالة:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    QCString trSearchResultsTitle() override
    {
      return "نتائج البحث";
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
        return "عفوا ، لا يوجد توثيق ملائم لما بحثت عنه.";
      }
      else if (numDocuments==1)
      {
        return "وثيقة مناسبة لما بحثت عنه. <b>1</b> تم العثور على.";
      }
      else
      {
        return "وثيقة مطابقة لما بحثت عنه <b>$num</b> تم إيجاد ."
               "وتم ترتيبهم بحيث يكون الأقرب لنتيجة البحث أولا.";
      }
    }
    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    QCString trSearchMatches() override
    {
		return "يتطابق مع:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    QCString trSourceFile(const QCString& filename) override
    {
      return " ملف المصدر" + filename ;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    QCString trDirIndex() override
    { return "شكل هرمي للأدلة"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    QCString trDirDocumentation() override
    { return "توثيق الأدلة"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    QCString trDirectories() override
    { return "الأدلة"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    QCString trDirReference(const QCString &dirName) override
    { QCString result=" مرجع الدليل"; result+=dirName; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    QCString trDir(bool first_capital, bool singular) override
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
    QCString trOverloadText() override
    {
       return "هذه دالة عضو زائدة التحميل ،"
              "وجد أنها ملائمة. إنها تختلف عن الدالة أعلاه"
              "فقط في نوعية ال argument(s) التي تقبلها.";
    }

    QCString trDayOfWeek(int dayOfWeek, bool, bool full) override
    {
      static const char *days_short[]   = { "الإثنين", "الثلاثاء", "الأربعاء", "الخميس", "الجمعة", "السبت", "الأحد" };
      static const char *days_full[]    = { "الإثنين", "الثلاثاء", "الأربعاء", "الخميس", "الجمعة", "السبت", "الأحد" };
      return full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
    }
    QCString trMonth(int month, bool, bool full) override
    {
      static const char *months_short[] = { "يناير", "فبراير", "مارس", "أبريل", "مايو", "يونيو", "يوليو", "أغسطس", "سبتمبر", "أكتوبر", "نوفمبر", "ديسمبر" };
      static const char *months_full[]  = { "يناير", "فبراير", "مارس", "أبريل", "مايو", "يونيو", "يوليو", "أغسطس", "سبتمبر", "أكتوبر", "نوفمبر", "ديسمبر" };
      return full? months_full[month-1] : months_short[month-1];
    }
    QCString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "ص", "م" };
      return dayPeriod[period?1:0];
    }
};

#endif
