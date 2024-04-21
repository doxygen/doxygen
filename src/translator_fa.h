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
 *
 * Description : Doxygen Persian (Farsi) Translator
 * Author : Ali Nadalizadeh < nadalizadeh at gmail dot com >
 *
 * ChangeLog :
 *   Thu 06 Jul 2006 11:54:09 PM IRDT <nadalizadeh at gmail dot com>
 *   >> First version of persian language support has been completed.
 *
 *   Mon 04 Feb 2008 11:52:09 AM IRDT <nadalizadeh at gmail dot com>
 *   >> Obsolete methods removed. Translated more string(s) to persian. Upgraded to 1_5_4 adapter.
 *
 *   Fri 04 Jun 2010 04:05:24 PM IRDT <nadalizadeh at gmail dot com>
 *   >> Implement missing new methods since 1.6.0.
 *   >> Add English to Persian digit convertor. (for date/time digits)
 *
 * Translation feedbacks are really appreciated.
 */

#ifndef TRANSLATOR_FA_H
#define TRANSLATOR_FA_H

#define HtmlRightToLeft     QCString("<div dir=\"rtl\">")
#define HtmlLeftToRight     QCString("<div dir=\"ltr\">")
#define HtmlDivEnd          QCString("</div>")


class TranslatorPersian : public TranslatorAdapter_1_7_5
{
  private:
  	/** Converts english digits of an input string to persian equivalents.
	 */
	QCString convertDigitsToFarsi(QCString str)
	{
		QCString output;
		const char * PersianDigits[] = { "۰", "۱", "۲", "۳", "۴", "۵", "۶", "۷", "۸", "۹" };
		for(unsigned i=0; i<str.length(); i++)
		{
			if (str.at(i) >= '0' && str.at(i) <= '9')
				output += PersianDigits[ str.at(i) - '0' ];
			else
				output += str.at(i);
		}

		return output;
	}

  public:

    // --- Language control methods -------------------

    /*! Used for identification of the language. The identification
     * should not be translated. It should be replaced by the name
     * of the language in Persian using lower-case characters only
     * (e.g. "czech", "japanese", "russian", etc.). It should be equal to
     * the identification used in language.cpp.
     */
    QCString idLanguage() override
    { return "persian"; }

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
     * The Persian LaTeX does not use such commands.  Because of this
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
    {
      return "fa";
    }

    QCString getLanguageString() override
    {
      return "0x429 Persian (Iran)";
    }
    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    QCString trRelatedFunctions() override
    { return "توابع مربوط"; }

    /*! subscript for the related functions. */
    QCString trRelatedSubscript() override
    { return "(لازم به ذکر است که اينها توابع عضو نيستند)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    QCString trDetailedDescription() override
    { return "توضيحات جزئی"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    QCString trDetails() override
    { return "جزئیات"; }

    /*! header that is put before the list of typedefs. */
    QCString trMemberTypedefDocumentation() override
    { return "مستندات تعریف گونه ها"; }

    /*! header that is put before the list of enumerations. */
    QCString trMemberEnumerationDocumentation() override
    { return "های عضو Enumeration مستندات"; }

    /*! header that is put before the list of member functions. */
    QCString trMemberFunctionDocumentation() override
    { return "توضيحات توابع عضو"; }

    /*! header that is put before the list of member attributes. */
    QCString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Field مستندات";
      }
      else
      {
        return "مستندات متغيير های عضو";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    QCString trMore() override
    { return "بيشتر..."; }

    /*! put in the class documentation */
    QCString trListOfAllMembers() override
    { return "ليست تمام اعضاء "; }

    /*! used as the title of the "list of all members" page of a class */
    QCString trMemberList() override
    { return "ليست اعضاء"; }

    /*! this is the first part of a sentence that is followed by a class name */
    QCString trThisIsTheListOfAllMembers() override
    { return "اين ليستی کامل از همه اعضای"; }

    /*! this is the remainder of the sentence after the class name */
    QCString trIncludingInheritedMembers() override
    { return "شامل همه ی اعضای به ارث برده شده می باشد."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="تولید شده توسط نرم افزار دی اکسیژن ";
      if (!s.isEmpty()) result+=" برای "+s;
      result+=" از کد برنامه ";
      return result;
    }

    /*! put after an enum name in the list of all members */
    QCString trEnumName() override
    { return "enum نام"; }

    /*! put after an enum value in the list of all members */
    QCString trEnumValue() override
    { return "enum مقدار"; }

    /*! put after an undocumented member in the list of all members */
    QCString trDefinedIn() override
    { return "تعریف شده در"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    QCString trModules() override
    { return "Modules"; }

    /*! This is put above each page as a link to the class hierarchy */
    QCString trClassHierarchy() override
    { return "سلسله مراتب کلاس ها"; }

    /*! This is put above each page as a link to the list of annotated classes */
    QCString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "ساختار های داده ";
      }
      else
      {
        return "ليست کلاس ها ";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    QCString trFileList() override
    { return "ليست فايل ها"; }

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "فضاهای داده ها";
      }
      else
      {
        return "اعضاء کلاس ها";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    QCString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Globals";
      }
      else
      {
        return "اعضاء پرونده";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    QCString trRelatedPages() override
    { return "صفحات مربوط"; }

    /*! This is put above each page as a link to all examples. */
    QCString trExamples() override
    { return "مثال ها"; }

    /*! This is put above each page as a link to the search engine. */
    QCString trSearch() override
    { return "جستجو"; }

    /*! This is an introduction to the class hierarchy. */
    QCString trClassHierarchyDescription() override
    { return "ليست و سلسله مراتب درختی کلاس ها به صورت مرتب شده :";
    }

    /*! This is an introduction to the list with all files. */
    QCString trFileListDescription(bool extractAll) override
    {
      QCString result="ليست همه ي پرونده های ";
      if (!extractAll) result+="(مستند شده) ";
      result+=" :";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    QCString trCompoundListDescription() override
    {

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "ليست ساختارهای داده به همراه توضيحی کوتاه :";
      }
      else
      {
        return "ليست کلاس ها ، ساختارهای داده و ... به همراه توضيحات مختصر راجع به آنها :";
      }
    }

    /*! This is an introduction to the page with all class members. */
    QCString trCompoundMembersDescription(bool extractAll) override
    {
      QCString result=" در ذيل ليست  ";
      if (!extractAll)
      {
        result+="آن اعضايي که مستند شده اند ";
      }
      else
      {
        result+="همه ی اعضاء ";
      }
      result+=" به همراه ارتباطشان با ";
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="ساختارهای داده ";
      }
      else
      {
        result+="كلاس ها ";
      }

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
          result+="و مستندات ساختار داده برای هر عضو ";
      }
        else
      {
          result+="و مستندات کلاس برای هر عضو ";
      }
      result+="را مشاهده ميکنيد :";
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    QCString trFileMembersDescription(bool extractAll) override
    {
      QCString result="ليست همه ی توابع ، متغيير ها، تعاريف و ... ";
      if (!extractAll) result+="(مستند شده) ";

      result+=" به همراه ارتباط آنها ";
      result+="با پرونده هايی که به آن مربوط اند :";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    QCString trExamplesDescription() override
    { return "ليست همه ی مثال ها :"; }

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription() override
    { return "لیست تمام صفحات و مستندات مربوطه :"; }

    /*! This is an introduction to the page with the list of class/file groups */
    QCString trModulesDescription() override
    { return "لیست تمام ماژول ها:"; }

    // index titles (the project name is prepended for these)


    /*! This is used in HTML as the title of index.html. */
    QCString trDocumentation(const QCString &projName) override
    { return "مستندات" + (!projName.isEmpty()?" " + projName : ""); }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    QCString trModuleIndex() override
    { return "فهرست ماژول ها"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    QCString trHierarchicalIndex() override
    { return "فهرست سلسله مراتب"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    QCString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "فهرست ساختار داده ها";
      }
      else
      {
        return "فهرست کلاس های";
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    QCString trFileIndex() override
    { return "فهرست پرونده ها"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    QCString trModuleDocumentation() override
    { return "مستندات ماژول"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "مستندات ساختار داده ها";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return "مستندات کلاس ها";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    QCString trFileDocumentation() override
    { return "مستندات فایل"; }

    /*! This is used in LaTeX as the title of the document */
    QCString trReferenceManual() override
    { return "راهنمای مرجع"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    QCString trDefines() override
    { return "تعاريف"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    QCString trTypedefs() override
    { return "تعریف گونه ها"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    QCString trEnumerations() override
    { return "تعاريف"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    QCString trFunctions() override
    { return "توابع"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trVariables() override
    { return "متغيير ها"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trEnumerationValues() override
    { return "معرف ها"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    QCString trDefineDocumentation() override
    { return "Define Documentation"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    QCString trTypedefDocumentation() override
    { return "Typedef"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation() override
    { return "مستندات تعريف"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    QCString trEnumerationValueDocumentation() override
    { return "مستندات معرف"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    QCString trFunctionDocumentation() override
    { return "توضيح تابع"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    QCString trVariableDocumentation() override
    { return "توضيح متغير"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    QCString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "ساختارهای داده";
      }
      else
      {
        return "کلاس ها";
      }
    }

    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result = HtmlDivEnd + HtmlRightToLeft + QCString("توليد شده در ") +date ;
      if (!projName.isEmpty()) result+=" برای "+projName;
      result+=" توسط";
      return result;
    }

    /*! this text is put before a class diagram */
    QCString trClassDiagram(const QCString &clName) override
    {
      return ""+clName+" نمودار وراثت برای  :";
    }

    /*! this text is generated when the \\warning command is used. */
    QCString trWarning() override
    { return "اخطار"; }

    /*! this text is generated when the \\version command is used. */
    QCString trVersion() override
    { return "نسخه"; }

    /*! this text is generated when the \\date command is used. */
    QCString trDate() override
    { return "تاريخ"; }

    /*! this text is generated when the \\return command is used. */
    QCString trReturns() override
    { return "خروجی"; }

    /*! this text is generated when the \\sa command is used. */
    QCString trSeeAlso() override
    { return "See also"; }

    /*! this text is generated when the \\param command is used. */
    QCString trParameters() override
    { return "پارامترها"; }

    /*! this text is generated when the \\exception command is used. */
    QCString trExceptions() override
    { return "استثناء ها"; }

    /*! this text is used in the title page of a LaTeX document. */
    QCString trGeneratedBy() override
    { return "توليد شده توسط"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    QCString trNamespaceList() override
    { return "ليست فضاهای نام"; }

    /*! used as an introduction to the namespace list */
    QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="در ذيل ليستی از همه ی فضاهای نام  ";
      if (!extractAll) result+="(مستند سازی شده) ";
      result+="به همراه توضيح کوتاه آنها مشاهده می کنيد :";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    QCString trFriends() override
    { return "کلاس های دوست"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    QCString trRelatedFunctionDocumentation() override
    { return "مستندات توابع مربوط و دوست"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    QCString trCompoundReference(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result;
      switch(compType)
      {
        case ClassDef::Class:      result=" کلاس"; break;
        case ClassDef::Struct:     result=" ساختار داده"; break;
        case ClassDef::Union:      result=" Union"; break;
        case ClassDef::Interface:  result=" Interface"; break;
        case ClassDef::Protocol:   result=" Protocol"; break;
        case ClassDef::Category:   result=" Category"; break;
        case ClassDef::Exception:  result=" استثناء"; break;
        default: break;
      }
      if (isTemplate) result+=" قالب";
      result=QCString(clName) + " مرجع" +result ;
      return result;
    }

    /*! used as the title of the HTML page of a file */
    QCString trFileReference(const QCString &fileName) override
    {
      QCString result=fileName;
      result+=" مرجع پرونده";
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    QCString trNamespaceReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" مرجع فضای نام";
      return result;
    }

    QCString trPublicMembers() override
    { return "توابع عمومی عضو کلاس"; }
    QCString trPublicSlots() override
    { return "های عمومی Slot"; }
    QCString trSignals() override
    { return "سيگنال ها"; }
    QCString trStaticPublicMembers() override
    { return "توابع ثابت عمومی عضو کلاس"; }
    QCString trProtectedMembers() override
    { return "توابع خصوصی عضو کلاس"; }
    QCString trProtectedSlots() override
    { return "های محافظت شده Slot"; }
    QCString trStaticProtectedMembers() override
    { return "توابع ثابت محافظت شده عضو کلاس"; }
    QCString trPrivateMembers() override
    { return "توابع خصوصی عضو کلاس"; }
    QCString trPrivateSlots() override
    { return "های خصوصی Slot"; }
    QCString trStaticPrivateMembers() override
    { return "توابع خصوصی ثابت عضو کلاس"; }

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
            result+=" و ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritsList(int numEntries) override
    {
      return ".را به ارث می برد "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritedByList(int numEntries) override
    {
      return ".اين کلاس را به ارث برده است "+trWriteList(numEntries)+".";
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
      return "تعریف شده است "+trWriteList(numEntries)+"دوباره در ";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    QCString trNamespaceMembers() override
    { return "اعضای فضاهای نام"; }

    /*! This is an introduction to the page with all namespace members */
    QCString trNamespaceMemberDescription(bool extractAll) override
    {
      QCString result="ليست همه اعضای فضای نام  ";
      if (!extractAll) result+="(مستند شده) ";
      result+=" با ارتباطشان با";
      result+="فضاهای نامی که به آن مربوط اند را مشاهده ميکنيد :";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    QCString trNamespaceIndex() override
    { return "فهرست فضاهای نام"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation() override
    { return "توضيحات فضای نام"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces() override
    { return "فضاهای نام"; }

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
      QCString result="مستندات اين  ";
      switch(compType)
      {
        case ClassDef::Class:      result+="کلاس"; break;
        case ClassDef::Struct:     result+="ساختار داده"; break;
        case ClassDef::Union:      result+="union"; break;
        case ClassDef::Interface:  result+="interface"; break;
        case ClassDef::Protocol:   result+="protocol"; break;
        case ClassDef::Category:   result+="category"; break;
        case ClassDef::Exception:  result+="exception"; break;
        default: break;
      }
      result+=" از روی پرونده ";
      if (single) result+=""; else result+="های ";
      result+="زير توليد شده است :";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    QCString trReturnValues() override
    { return "مقادير بازگشتی"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage() override
    { return "صفحه ی اصلی"; }

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
      return ".در خط @0 از پرونده @1 تعريف شده است";
    }
    QCString trDefinedInSourceFile() override
    {
      return ".در فايل @0 تعريف شده است";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated() override
    {
      return "منسوخ شده";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const QCString &clName) override
    {
      return "Collaboration diagram for "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const QCString &fName) override
    {
      return "نمودار شامل شدن ها برای "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation() override
    {
      return "مستندات توباع آغازین و پایانی";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode() override
    {
      return "کد منبع اين پرونده.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation() override
    {
      return "توضيحات اين پرونده.";
    }
    /*! Text for the \\pre command */
    QCString trPrecondition() override
    {
      return "پیش شرط";
    }
    /*! Text for the \\post command */
    QCString trPostcondition() override
    {
      return "پس شرط";
    }
    /*! Text for the \\invariant command */
    QCString trInvariant() override
    {
      return "Invariant";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue() override
    {
      return "مقدار اوليه :";
    }
    /*! Text used the source code in the file index */
    QCString trCode() override
    {
      return "کد";
    }
    QCString trGraphicalHierarchy() override
    {
      return "سلسله مراتب گرافيکی کلاس";
    }
    QCString trGotoGraphicalHierarchy() override
    {
      return "نمایش نمودار درختی گرافیکی کلاس";
    }
    QCString trGotoTextualHierarchy() override
    {
      return "نمایش نمودار درختی متنی کلاس";
    }
    QCString trPageIndex() override
    {
      return "فهرست صفحات";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    QCString trNote() override
    {
      return "نکته";
    }
    QCString trPublicTypes() override
    {
      return "های عمومی Type";
    }
    QCString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "فضاهای داده ها";
      }
      else
      {
        return "خواص (متغييرهای) عمومی";
      }
    }
    QCString trStaticPublicAttribs() override
    {
      return "خواص (متغييرهای) عمومی ثابت";
    }
    QCString trProtectedTypes() override
    {
      return "های حفاظت شده Type";
    }
    QCString trProtectedAttribs() override
    {
      return "خواص (متغييرهای) حفاظت شده";
    }
    QCString trStaticProtectedAttribs() override
    {
      return "خواص (متغييرهای) حفاظت شده ثابت";
    }
    QCString trPrivateTypes() override
    {
      return "های خصوصی Type";
    }
    QCString trPrivateAttribs() override
    {
      return "خواص (متغييرهای) خصوصی";
    }
    QCString trStaticPrivateAttribs() override
    {
      return "خواص (متغييرهای) خصوصی ثابت";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    QCString trTodo() override
    {
      return "برای انجام";
    }
    /*! Used as the header of the todo list */
    QCString trTodoList() override
    {
      return "ليست کارهاي آينده";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy() override
    {
      return " استفاده شده توسط ";
    }
    QCString trRemarks() override
    {
      return "ملاحظات";
    }
    QCString trAttention() override
    {
      return "توجه";
    }
    QCString trInclByDepGraph() override
    {
      return "این نمودار فایل هایی را که این فایل را به طور مستقیم یا غیر مستقیم استفاده کرده اند نشان می دهد";
    }
    QCString trSince() override
    {
      return "از";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    QCString trLegendTitle() override
    {
      return "راهنمای نمودار";
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
      return "راهنما";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    QCString trTest() override
    {
      return "تست";
    }
    /*! Used as the header of the test list */
    QCString trTestList() override
    {
      return "Test List";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    QCString trProperties() override
    {
      return "خاصیت ها";
    }
    /*! Used as a section header for IDL property documentation */
    QCString trPropertyDocumentation() override
    {
      return "مستندات خاصیت";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    QCString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "ساختار های داده";
      }
      else
      {
        return "كلاس ها";
      }
    }
    /*! Used as the title of a Java package */
    QCString trPackage(const QCString &name) override
    {
      return "Package "+name;
    }
    /*! The description of the package index page */
    QCString trPackageListDescription() override
    {
      return "لیست بسته ها به همراه توضیح مختر در صورت وجود :";
    }
    /*! The link name in the Quick links header for each page */
    QCString trPackages() override
    {
      return "بسته ها";
    }
    /*! Text shown before a multi-line define */
    QCString trDefineValue() override
    {
      return "مقدار:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    QCString trBug() override
    {
      return "اشکال";
    }
    /*! Used as the header of the bug list */
    QCString trBugList() override
    {
      return "ليست اشکالات";
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
      return "فهرست";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trClass(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "کلاس" : "کلاس"));
      if (!singular)  result+="ها";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trFile(bool /*first_capital*/, bool singular) override
    {
      QCString result("پرونده");
      if (!singular) result=result+" ها";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trNamespace(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "فضای نام " : "فضای نام "));
      if (!singular)  result+="ها";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGroup(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Group" : "group"));
      if (!singular)  result+="s";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trPage(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "صفحه" : "صفحه"));
      if (!singular)  result+=" ها ";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trMember(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "عضو" : "عضو"));
      if (!singular)  result+="ها";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGlobal(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Global" : "global"));
      if (!singular)  result+="s";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    QCString trAuthor(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Author" : "author"));
      if (!singular)  result+="s";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    QCString trReferences() override
    {
      return "را استفاده ميکند ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    QCString trImplementedFromList(int numEntries) override
    {
      return "را تکميل می کند "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    QCString trImplementedInList(int numEntries) override
    {
      return ".تکميل شده است "+trWriteList(numEntries)+" در";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    QCString trRTFTableOfContents() override
    {
      return "فهرست";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    QCString trDeprecatedList() override
    {
      return "لیست آیتم های از رده خارج";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    QCString trEvents() override
    {
      return "رویداد ها";
    }
    /*! Header used for the documentation section of a class' events. */
    QCString trEventDocumentation() override
    {
      return "مستندات رویداد";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    QCString trPackageTypes() override
    {
      return "انواع بسته ها";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    QCString trPackageFunctions() override
    {
      return "توابع بسته ها";
    }
    QCString trPackageMembers() override
    {
      return "عضوها بسته ها";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    QCString trStaticPackageFunctions() override
    {
      return "Static Package Functions";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    QCString trPackageAttribs() override
    {
      return "خواص بسته ها";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    QCString trStaticPackageAttribs() override
    {
      return "Static Package Attributes";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    QCString trAll() override
    {
      return "همه";
    }
    /*! Put in front of the call graph for a function. */
    QCString trCallGraph() override
    {
      return "نمودار صدا زدن برای این تابع :";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    QCString trSearchResultsTitle() override
    {
      return "نتايج جستجو";
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
        return "متاسفانه هیچ صفحه ای برای جستجو ی شما یافت نشد.";
      }
      else if (numDocuments==1)
      {
        return "یک سند برای این مورد یافت شد.";
      }
      else
      {
        return "Found <b>$num</b> documents matching your query. "
               "Showing best matches first.";
      }
    }
    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    QCString trSearchMatches() override
    {
      return "Matches:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    QCString trSourceFile(const QCString& filename) override
    {
      return filename + " کد و پرونده منبع";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    QCString trDirIndex() override
    { return "ساختار و سلسله مراتب شاخه ها"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    QCString trDirDocumentation() override
    { return "مستندات دايرکتوری"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    QCString trDirectories() override
    { return "شاخه ها"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    QCString trDirReference(const QCString &dirName) override
    { QCString result=dirName; result+=" مرجع شاخه ی"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    QCString trDir(bool /*first_capital*/, bool singular) override
    {
      QCString result("شاخه");
      if (singular) result+=" ها "+result;
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
       return "This is an overloaded member function, "
              "provided for convenience. It differs from the above "
              "function only in what argument(s) it accepts.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    QCString trCallerGraph() override
    {
      return "نمودار صدا زننده های این تابع:";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    QCString trMemberFunctionDocumentationFortran() override
    { return "مستندات توابع عضو"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    QCString trCompoundListFortran() override
    { return "لیست ساختار های داده"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    QCString trCompoundMembersFortran() override
    { return "فیلدهای اطلاعات"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    QCString trCompoundListDescriptionFortran() override
    { return "ساختارهای داده به همراه توضیح کوتاه :"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    QCString trCompoundMembersDescriptionFortran(bool) override
    {
      return "توضیحات اعضا ساختارها به همراه مستندات ساختار داده ی مربوطه";
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index (Fortran).
     */
    QCString trCompoundIndexFortran() override
    { return "Data Type Index"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    QCString trTypeDocumentation() override
    { return "Data Type Documentation"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    QCString trSubprograms() override
    { return "توابع و زیربرنامه ها"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    QCString trSubprogramDocumentation() override
    { return "مستندات توابع و زیربرنامه ها"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     QCString trDataTypes() override
    { return "ساختار های داده"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    QCString trModulesList() override
    { return "لیست ماژول ها"; }

    /*! used as an introduction to the modules list (Fortran) */
    QCString trModulesListDescription(bool) override
    {
      QCString result="لیست ماژول ها به همراه توضیحات کوتاه";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    QCString trCompoundReferenceFortran(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result=clName;
      switch(compType)
      {
        case ClassDef::Class:      result+=" Module"; break;
        case ClassDef::Struct:     result+=" Type"; break;
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
    /*! used as the title of the HTML page of a module (Fortran) */
    QCString trModuleReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" Module Reference";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    QCString trModulesMembers() override
    { return "اعضاء ماژول"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    QCString trModulesMemberDescription(bool) override
    {
      QCString result="لیست اعضاء ماژول ها به همراه مستندات ماژول مربوطه";
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all modules (Fortran).
     */
    QCString trModulesIndex() override
    { return "Modules Index"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trModule(bool, bool singular) override
    {
      QCString result("ماژول");
      if (!singular)  result+=" ها";
      return result;
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      QCString result="The documentation for this ";
      switch(compType)
      {
        case ClassDef::Class:      result+="module"; break;
        case ClassDef::Struct:     result+="type"; break;
        case ClassDef::Union:      result+="union"; break;
        case ClassDef::Interface:  result+="interface"; break;
        case ClassDef::Protocol:   result+="protocol"; break;
        case ClassDef::Category:   result+="category"; break;
        case ClassDef::Exception:  result+="exception"; break;
        default: break;
      }
      result+=" was generated from the following file";
      if (single) result+=":"; else result+="s:";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trType(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "type", "s");
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trSubprogram(bool, bool singular) override
    {
      QCString result("زیربرنامه");
      if (!singular)  result+=" ها ";
      return result;
    }

    /*! C# Type Constraint list */
    QCString trTypeConstraints() override
    {
      return "Type Constraints";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    QCString trDirRelation(const QCString &name) override
    {
      return QCString(name) + " Relation";
    }

    /*! Loading message shown when loading search results */
    QCString trLoading() override
    {
      return "در حال بارگذاری...";
    }

    /*! Label used for search results in the global namespace */
    QCString trGlobalNamespace() override
    {
      return "فضای نام جهانی";
    }

    /*! Message shown while searching */
    QCString trSearching() override
    {
      return "در حال جستجو...";
    }

    /*! Text shown when no search results are found */
    QCString trNoMatches() override
    {
      return "یافت نشد";
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
      return "پرونده ای در "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    QCString trIncludesFileIn(const QCString &name) override
    {
      return "Includes file in "+name;
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
      static const char *days[]   = { "دوشنبه","سه‌شنبه","چهارشنبه","پنجشنبه","جمعه","شنبه","یکشنبه" };
      static const char *months[] = { "ژانویه","فوریه","مارس","آوریل","می","جون","جولای","آگوست","سپتامبر","اکتبر","نوامبر","دسامبر" };
      QCString sdate;
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Date)
      {
        sdate.sprintf("%s %d %s %d",days[dayOfWeek-1],day,months[month-1],year);
      }
      if (includeTime == DateTimeType::DateTime) sdate += " ";
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Time)
      {
        QCString stime;
        stime.sprintf("%.2d:%.2d:%.2d",hour,minutes,seconds);
        sdate+=stime;
      }
      return convertDigitsToFarsi(sdate);
    }
    QCString trDayOfWeek(int dayOfWeek, bool, bool full) override
    {
      static const char *days_short[]   = { "دوشنبه", "سه شنبه", "چهارشنبه", "پنجشنبه", "جمعه", "شنبه", "يكشنبه" };
      static const char *days_full[]    = { "دوشنبه", "سه شنبه", "چهارشنبه", "پنجشنبه", "جمعه", "شنبه", "يكشنبه" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      return text;
    }
    QCString trMonth(int month, bool, bool full) override
    {
      static const char *months_short[] = { "ژانويه", "فوريه", "مارس", "آوريل", "مه", "ژوئن", "ژوئيه", "اوت", "سپتامبر", "اُكتبر", "نوامبر", "دسامبر" };
      static const char *months_full[]  = { "ژانويه", "فوريه", "مارس", "آوريل", "مه", "ژوئن", "ژوئيه", "اوت", "سپتامبر", "اُكتبر", "نوامبر", "دسامبر" };
      QCString text  = full? months_full[month-1] : months_short[month-1];
      return text;
    }
    QCString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "قبل‌ازظهر", "بعدازظهر" };
      return dayPeriod[period?1:0];
    }

};

#endif
