/******************************************************************************
 *
 * Copyright (C) 1997-2026 by Dimitri van Heesch.
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
 *   >> Add English to Persian digit converter. (for date/time digits)
 *
 * Translation feedbacks are really appreciated.
 */

#ifndef TRANSLATOR_FA_H
#define TRANSLATOR_FA_H

#define HtmlRightToLeft     DString("<div dir=\"rtl\">")
#define HtmlLeftToRight     DString("<div dir=\"ltr\">")
#define HtmlDivEnd          DString("</div>")


class TranslatorPersian : public TranslatorAdapter_1_7_5
{
  private:
    /** Converts english digits of an input string to persian equivalents.  */
    DString convertDigitsToFarsi(DString str)
    {
      DString output;
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
    DString idLanguage() override
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
    DString latexLanguageSupportCommand() override
    {
      return "";
    }

    DString latexCommandName() override
    {
      return p_latexCommandName("xelatex");
    }

    DString trISOLang() override
    {
      return "fa";
    }

    DString getLanguageString() override
    {
      return "0x429 Persian (Iran)";
    }
    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    DString trRelatedFunctions() override
    { return "توابع مربوط"; }

    /*! subscript for the related functions. */
    DString trRelatedSubscript() override
    { return "(لازم به ذکر است که اينها توابع عضو نيستند)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    DString trDetailedDescription() override
    { return "توضيحات جزئی"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    DString trDetails() override
    { return "جزئیات"; }

    /*! header that is put before the list of typedefs. */
    DString trMemberTypedefDocumentation() override
    { return "مستندات تعریف گونه ها"; }

    /*! header that is put before the list of enumerations. */
    DString trMemberEnumerationDocumentation() override
    { return "های عضو Enumeration مستندات"; }

    /*! header that is put before the list of member functions. */
    DString trMemberFunctionDocumentation() override
    { return "توضيحات توابع عضو"; }

    /*! header that is put before the list of member attributes. */
    DString trMemberDataDocumentation() override
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
    DString trMore() override
    { return "بيشتر..."; }

    /*! put in the class documentation */
    DString trListOfAllMembers() override
    { return "ليست تمام اعضاء "; }

    /*! used as the title of the "list of all members" page of a class */
    DString trMemberList() override
    { return "ليست اعضاء"; }

    /*! this is the first part of a sentence that is followed by a class name */
    DString trThisIsTheListOfAllMembers() override
    { return "اين ليستی کامل از همه اعضای"; }

    /*! this is the remainder of the sentence after the class name */
    DString trIncludingInheritedMembers() override
    { return "شامل همه ی اعضای به ارث برده شده می باشد."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    DString trGeneratedAutomatically(const DString &s) override
    { DString result="تولید شده توسط نرم افزار دی اکسیژن ";
      if (!s.empty()) result+=" برای "+s;
      result+=" از کد برنامه ";
      return result;
    }

    /*! put after an enum name in the list of all members */
    DString trEnumName() override
    { return "enum نام"; }

    /*! put after an enum value in the list of all members */
    DString trEnumValue() override
    { return "enum مقدار"; }

    /*! put after an undocumented member in the list of all members */
    DString trDefinedIn() override
    { return "تعریف شده در"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    DString trModules() override
    { return "Modules"; }

    /*! This is put above each page as a link to the class hierarchy */
    DString trClassHierarchy() override
    { return "سلسله مراتب کلاس ها"; }

    /*! This is put above each page as a link to the list of annotated classes */
    DString trCompoundList() override
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
    DString trFileList() override
    { return "ليست فايل ها"; }

    /*! This is put above each page as a link to all members of compounds. */
    DString trCompoundMembers() override
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
    DString trFileMembers() override
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
    DString trRelatedPages() override
    { return "صفحات مربوط"; }

    /*! This is put above each page as a link to all examples. */
    DString trExamples() override
    { return "مثال ها"; }

    /*! This is put above each page as a link to the search engine. */
    DString trSearch() override
    { return "جستجو"; }

    /*! This is an introduction to the class hierarchy. */
    DString trClassHierarchyDescription() override
    { return "ليست و سلسله مراتب درختی کلاس ها به صورت مرتب شده :";
    }

    /*! This is an introduction to the list with all files. */
    DString trFileListDescription(bool extractAll) override
    {
      DString result="ليست همه ي پرونده های ";
      if (!extractAll) result+="(مستند شده) ";
      result+=" :";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    DString trCompoundListDescription() override
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
    DString trCompoundMembersDescription(bool extractAll) override
    {
      DString result=" در ذيل ليست  ";
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
    DString trFileMembersDescription(bool extractAll) override
    {
      DString result="ليست همه ی توابع ، متغيير ها، تعاريف و ... ";
      if (!extractAll) result+="(مستند شده) ";

      result+=" به همراه ارتباط آنها ";
      result+="با پرونده هايی که به آن مربوط اند :";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    DString trExamplesDescription() override
    { return "ليست همه ی مثال ها :"; }

    /*! This is an introduction to the page with the list of related pages */
    DString trRelatedPagesDescription() override
    { return "لیست تمام صفحات و مستندات مربوطه :"; }

    /*! This is an introduction to the page with the list of class/file groups */
    DString trModulesDescription() override
    { return "لیست تمام ماژول ها:"; }

    // index titles (the project name is prepended for these)


    /*! This is used in HTML as the title of index.html. */
    DString trDocumentation(const DString &projName) override
    { return "مستندات" + (!projName.empty()?" " + projName : ""); }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    DString trModuleIndex() override
    { return "فهرست ماژول ها"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    DString trHierarchicalIndex() override
    { return "فهرست سلسله مراتب"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    DString trCompoundIndex() override
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
    DString trFileIndex() override
    { return "فهرست پرونده ها"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    DString trModuleDocumentation() override
    { return "مستندات ماژول"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    DString trClassDocumentation() override
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
    DString trFileDocumentation() override
    { return "مستندات فایل"; }

    /*! This is used in LaTeX as the title of the document */
    DString trReferenceManual() override
    { return "راهنمای مرجع"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    DString trDefines() override
    { return "تعاريف"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    DString trTypedefs() override
    { return "تعریف گونه ها"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    DString trEnumerations() override
    { return "تعاريف"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    DString trFunctions() override
    { return "توابع"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    DString trVariables() override
    { return "متغيير ها"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    DString trEnumerationValues() override
    { return "معرف ها"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    DString trDefineDocumentation() override
    { return "Define Documentation"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    DString trTypedefDocumentation() override
    { return "Typedef"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    DString trEnumerationTypeDocumentation() override
    { return "مستندات تعريف"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    DString trEnumerationValueDocumentation() override
    { return "مستندات معرف"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    DString trFunctionDocumentation() override
    { return "توضيح تابع"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    DString trVariableDocumentation() override
    { return "توضيح متغير"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    DString trCompounds() override
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
    DString trGeneratedAt(const DString &date,const DString &projName) override
    {
      DString result = HtmlDivEnd + HtmlRightToLeft + DString("توليد شده در ") +date ;
      if (!projName.empty()) result+=" برای "+projName;
      result+=" توسط";
      return result;
    }

    /*! this text is put before a class diagram */
    DString trClassDiagram(const DString &clName) override
    {
      return ""+clName+" نمودار وراثت برای  :";
    }

    /*! this text is generated when the \\warning command is used. */
    DString trWarning() override
    { return "اخطار"; }

    /*! this text is generated when the \\version command is used. */
    DString trVersion() override
    { return "نسخه"; }

    /*! this text is generated when the \\date command is used. */
    DString trDate() override
    { return "تاريخ"; }

    /*! this text is generated when the \\return command is used. */
    DString trReturns() override
    { return "خروجی"; }

    /*! this text is generated when the \\sa command is used. */
    DString trSeeAlso() override
    { return "See also"; }

    /*! this text is generated when the \\param command is used. */
    DString trParameters() override
    { return "پارامترها"; }

    /*! this text is generated when the \\exception command is used. */
    DString trExceptions() override
    { return "استثناء ها"; }

    /*! this text is used in the title page of a LaTeX document. */
    DString trGeneratedBy() override
    { return "توليد شده توسط"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    DString trNamespaceList() override
    { return "ليست فضاهای نام"; }

    /*! used as an introduction to the namespace list */
    DString trNamespaceListDescription(bool extractAll) override
    {
      DString result="در ذيل ليستی از همه ی فضاهای نام  ";
      if (!extractAll) result+="(مستند سازی شده) ";
      result+="به همراه توضيح کوتاه آنها مشاهده می کنيد :";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    DString trFriends() override
    { return "کلاس های دوست"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    DString trRelatedFunctionDocumentation() override
    { return "مستندات توابع مربوط و دوست"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    DString trCompoundReference(const DString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      DString result;
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
      result=clName + " مرجع" +result ;
      return result;
    }

    /*! used as the title of the HTML page of a file */
    DString trFileReference(const DString &fileName) override
    {
      DString result=fileName;
      result+=" مرجع پرونده";
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    DString trNamespaceReference(const DString &namespaceName) override
    {
      DString result=namespaceName;
      result+=" مرجع فضای نام";
      return result;
    }

    DString trPublicMembers() override
    { return "توابع عمومی عضو کلاس"; }
    DString trPublicSlots() override
    { return "های عمومی Slot"; }
    DString trSignals() override
    { return "سيگنال ها"; }
    DString trStaticPublicMembers() override
    { return "توابع ثابت عمومی عضو کلاس"; }
    DString trProtectedMembers() override
    { return "توابع خصوصی عضو کلاس"; }
    DString trProtectedSlots() override
    { return "های محافظت شده Slot"; }
    DString trStaticProtectedMembers() override
    { return "توابع ثابت محافظت شده عضو کلاس"; }
    DString trPrivateMembers() override
    { return "توابع خصوصی عضو کلاس"; }
    DString trPrivateSlots() override
    { return "های خصوصی Slot"; }
    DString trStaticPrivateMembers() override
    { return "توابع خصوصی ثابت عضو کلاس"; }

    /*! this function is used to produce a comma-separated list of items.
     *  use generateMarker(i) to indicate where item i should be put.
     */
    DString trWriteList(int numEntries) override
    {
      DString result;
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
    DString trInheritsList(int numEntries) override
    {
      return ".را به ارث می برد "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    DString trInheritedByList(int numEntries) override
    {
      return ".اين کلاس را به ارث برده است "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    DString trReimplementedFromList(int numEntries) override
    {
      return "Reimplemented from "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    DString trReimplementedInList(int numEntries) override
    {
      return "تعریف شده است "+trWriteList(numEntries)+"دوباره در ";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    DString trNamespaceMembers() override
    { return "اعضای فضاهای نام"; }

    /*! This is an introduction to the page with all namespace members */
    DString trNamespaceMemberDescription(bool extractAll) override
    {
      DString result="ليست همه اعضای فضای نام  ";
      if (!extractAll) result+="(مستند شده) ";
      result+=" با ارتباطشان با";
      result+="فضاهای نامی که به آن مربوط اند را مشاهده ميکنيد :";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    DString trNamespaceIndex() override
    { return "فهرست فضاهای نام"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    DString trNamespaceDocumentation() override
    { return "توضيحات فضای نام"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    DString trNamespaces() override
    { return "فضاهای نام"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    DString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      DString result="مستندات اين  ";
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
    DString trReturnValues() override
    { return "مقادير بازگشتی"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    DString trMainPage() override
    { return "صفحه ی اصلی"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    DString trPageAbbreviation() override
    { return "ص."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////
    DString trDefinedAtLineInSourceFile() override
    {
      return ".در خط @0 از پرونده @1 تعريف شده است";
    }
    DString trDefinedInSourceFile() override
    {
      return ".در فايل @0 تعريف شده است";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    DString trDeprecated() override
    {
      return "منسوخ شده";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    DString trCollaborationDiagram(const DString &clName) override
    {
      return "Collaboration diagram for "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    DString trInclDepGraph(const DString &fName) override
    {
      return "نمودار شامل شدن ها برای "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    DString trConstructorDocumentation() override
    {
      return "مستندات توباع آغازین و پایانی";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    DString trGotoSourceCode() override
    {
      return "کد منبع اين پرونده.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    DString trGotoDocumentation() override
    {
      return "توضيحات اين پرونده.";
    }
    /*! Text for the \\pre command */
    DString trPrecondition() override
    {
      return "پیش شرط";
    }
    /*! Text for the \\post command */
    DString trPostcondition() override
    {
      return "پس شرط";
    }
    /*! Text for the \\invariant command */
    DString trInvariant() override
    {
      return "Invariant";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    DString trInitialValue() override
    {
      return "مقدار اوليه :";
    }
    /*! Text used the source code in the file index */
    DString trCode() override
    {
      return "کد";
    }
    DString trGraphicalHierarchy() override
    {
      return "سلسله مراتب گرافيکی کلاس";
    }
    DString trGotoGraphicalHierarchy() override
    {
      return "نمایش نمودار درختی گرافیکی کلاس";
    }
    DString trGotoTextualHierarchy() override
    {
      return "نمایش نمودار درختی متنی کلاس";
    }
    DString trPageIndex() override
    {
      return "فهرست صفحات";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    DString trNote() override
    {
      return "نکته";
    }
    DString trPublicTypes() override
    {
      return "های عمومی Type";
    }
    DString trPublicAttribs() override
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
    DString trStaticPublicAttribs() override
    {
      return "خواص (متغييرهای) عمومی ثابت";
    }
    DString trProtectedTypes() override
    {
      return "های حفاظت شده Type";
    }
    DString trProtectedAttribs() override
    {
      return "خواص (متغييرهای) حفاظت شده";
    }
    DString trStaticProtectedAttribs() override
    {
      return "خواص (متغييرهای) حفاظت شده ثابت";
    }
    DString trPrivateTypes() override
    {
      return "های خصوصی Type";
    }
    DString trPrivateAttribs() override
    {
      return "خواص (متغييرهای) خصوصی";
    }
    DString trStaticPrivateAttribs() override
    {
      return "خواص (متغييرهای) خصوصی ثابت";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    DString trTodo() override
    {
      return "برای انجام";
    }
    /*! Used as the header of the todo list */
    DString trTodoList() override
    {
      return "ليست کارهاي آينده";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    DString trReferencedBy() override
    {
      return " استفاده شده توسط ";
    }
    DString trRemarks() override
    {
      return "ملاحظات";
    }
    DString trAttention() override
    {
      return "توجه";
    }
    DString trInclByDepGraph() override
    {
      return "این نمودار فایل هایی را که این فایل را به طور مستقیم یا غیر مستقیم استفاده کرده اند نشان می دهد";
    }
    DString trSince() override
    {
      return "از";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    DString trLegendTitle() override
    {
      return "راهنمای نمودار";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    DString trLegendDocs() override
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
    DString trLegend() override
    {
      return "راهنما";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    DString trTest() override
    {
      return "تست";
    }
    /*! Used as the header of the test list */
    DString trTestList() override
    {
      return "Test List";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    DString trProperties() override
    {
      return "خاصیت ها";
    }
    /*! Used as a section header for IDL property documentation */
    DString trPropertyDocumentation() override
    {
      return "مستندات خاصیت";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    DString trClasses() override
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
    DString trPackage(const DString &name) override
    {
      return "Package "+name;
    }
    /*! The description of the package index page */
    DString trPackageListDescription() override
    {
      return "لیست بسته ها به همراه توضیح مختر در صورت وجود :";
    }
    /*! The link name in the Quick links header for each page */
    DString trPackages() override
    {
      return "بسته ها";
    }
    /*! Text shown before a multi-line define */
    DString trDefineValue() override
    {
      return "مقدار:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    DString trBug() override
    {
      return "اشکال";
    }
    /*! Used as the header of the bug list */
    DString trBugList() override
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
    DString trRTFansicp() override
    {
      return "1256";
    }


    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    DString trRTFCharSet() override
    {
      return "178";
    }

    /*! Used as header RTF general index */
    DString trRTFGeneralIndex() override
    {
      return "فهرست";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trClass(bool first_capital, bool singular) override
    {
      DString result((first_capital ? "کلاس" : "کلاس"));
      if (!singular)  result+="ها";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trFile(bool /*first_capital*/, bool singular) override
    {
      DString result("پرونده");
      if (!singular) result=result+" ها";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trNamespace(bool first_capital, bool singular) override
    {
      DString result((first_capital ? "فضای نام " : "فضای نام "));
      if (!singular)  result+="ها";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trGroup(bool first_capital, bool singular) override
    {
      DString result((first_capital ? "Group" : "group"));
      if (!singular)  result+="s";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trPage(bool first_capital, bool singular) override
    {
      DString result((first_capital ? "صفحه" : "صفحه"));
      if (!singular)  result+=" ها ";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trMember(bool first_capital, bool singular) override
    {
      DString result((first_capital ? "عضو" : "عضو"));
      if (!singular)  result+="ها";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trGlobal(bool first_capital, bool singular) override
    {
      DString result((first_capital ? "Global" : "global"));
      if (!singular)  result+="s";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    DString trAuthor(bool first_capital, bool singular) override
    {
      DString result((first_capital ? "Author" : "author"));
      if (!singular)  result+="s";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    DString trReferences() override
    {
      return "را استفاده ميکند ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    DString trImplementedFromList(int numEntries) override
    {
      return "را تکميل می کند "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    DString trImplementedInList(int numEntries) override
    {
      return ".تکميل شده است "+trWriteList(numEntries)+" در";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    DString trRTFTableOfContents() override
    {
      return "فهرست";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    DString trDeprecatedList() override
    {
      return "لیست آیتم های از رده خارج";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    DString trEvents() override
    {
      return "رویداد ها";
    }
    /*! Header used for the documentation section of a class' events. */
    DString trEventDocumentation() override
    {
      return "مستندات رویداد";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    DString trPackageTypes() override
    {
      return "انواع بسته ها";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    DString trPackageFunctions() override
    {
      return "توابع بسته ها";
    }
    DString trPackageMembers() override
    {
      return "عضوها بسته ها";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    DString trStaticPackageFunctions() override
    {
      return "Static Package Functions";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    DString trPackageAttribs() override
    {
      return "خواص بسته ها";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    DString trStaticPackageAttribs() override
    {
      return "Static Package Attributes";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    DString trAll() override
    {
      return "همه";
    }
    /*! Put in front of the call graph for a function. */
    DString trCallGraph() override
    {
      return "نمودار صدا زدن برای این تابع :";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    DString trSearchResultsTitle() override
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
    DString trSearchResults(int numDocuments) override
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
    DString trSearchMatches() override
    {
      return "Matches:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    DString trSourceFile(const DString& filename) override
    {
      return filename + " کد و پرونده منبع";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    DString trDirIndex() override
    { return "ساختار و سلسله مراتب شاخه ها"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    DString trDirDocumentation() override
    { return "مستندات دايرکتوری"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    DString trDirectories() override
    { return "شاخه ها"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    DString trDirReference(const DString &dirName) override
    { DString result=dirName; result+=" مرجع شاخه ی"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in singular or plural form (\a singular).
     */
    DString trDir(bool /*first_capital*/, bool singular) override
    {
      DString result("شاخه");
      if (singular) result+=" ها "+result;
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    DString trOverloadText() override
    {
       return "This is an overloaded member function, "
              "provided for convenience. It differs from the above "
              "function only in what argument(s) it accepts.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    DString trCallerGraph() override
    {
      return "نمودار صدا زننده های این تابع:";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    DString trMemberFunctionDocumentationFortran() override
    { return "مستندات توابع عضو"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    DString trCompoundListFortran() override
    { return "لیست ساختار های داده"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    DString trCompoundMembersFortran() override
    { return "فیلدهای اطلاعات"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    DString trCompoundListDescriptionFortran() override
    { return "ساختارهای داده به همراه توضیح کوتاه :"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    DString trCompoundMembersDescriptionFortran(bool) override
    {
      return "توضیحات اعضا ساختارها به همراه مستندات ساختار داده ی مربوطه";
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index (Fortran).
     */
    DString trCompoundIndexFortran() override
    { return "Data Type Index"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    DString trTypeDocumentation() override
    { return "Data Type Documentation"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    DString trSubprograms() override
    { return "توابع و زیربرنامه ها"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    DString trSubprogramDocumentation() override
    { return "مستندات توابع و زیربرنامه ها"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     DString trDataTypes() override
    { return "ساختار های داده"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    DString trModulesList() override
    { return "لیست ماژول ها"; }

    /*! used as an introduction to the modules list (Fortran) */
    DString trModulesListDescription(bool) override
    {
      DString result="لیست ماژول ها به همراه توضیحات کوتاه";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    DString trCompoundReferenceFortran(const DString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      DString result=clName;
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
    DString trModuleReference(const DString &namespaceName) override
    {
      DString result=namespaceName;
      result+=" Module Reference";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    DString trModulesMembers() override
    { return "اعضاء ماژول"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    DString trModulesMemberDescription(bool) override
    {
      DString result="لیست اعضاء ماژول ها به همراه مستندات ماژول مربوطه";
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all modules (Fortran).
     */
    DString trModulesIndex() override
    { return "Modules Index"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trModule(bool, bool singular) override
    {
      DString result("ماژول");
      if (!singular)  result+=" ها";
      return result;
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    DString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      DString result="The documentation for this ";
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
    DString trType(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "type", "s");
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trSubprogram(bool, bool singular) override
    {
      DString result("زیربرنامه");
      if (!singular)  result+=" ها ";
      return result;
    }

    /*! C# Type Constraint list */
    DString trTypeConstraints() override
    {
      return "Type Constraints";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    DString trDirRelation(const DString &name) override
    {
      return name + " Relation";
    }

    /*! Loading message shown when loading search results */
    DString trLoading() override
    {
      return "در حال بارگذاری...";
    }

    /*! Label used for search results in the global namespace */
    DString trGlobalNamespace() override
    {
      return "فضای نام جهانی";
    }

    /*! Message shown while searching */
    DString trSearching() override
    {
      return "در حال جستجو...";
    }

    /*! Text shown when no search results are found */
    DString trNoMatches() override
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
    DString trFileIn(const DString &name) override
    {
      return "پرونده ای در "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    DString trIncludesFileIn(const DString &name) override
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
    DString trDateTime(int year,int month,int day,int dayOfWeek,
                                int hour,int minutes,int seconds,
                                DateTimeType includeTime) override
    {
      static const char *days[]   = { "دوشنبه","سه‌شنبه","چهارشنبه","پنجشنبه","جمعه","شنبه","یکشنبه" };
      static const char *months[] = { "ژانویه","فوریه","مارس","آوریل","می","جون","جولای","آگوست","سپتامبر","اکتبر","نوامبر","دسامبر" };
      DString sdate;
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Date)
      {
        sdate.sprintf("%s %d %s %d",days[dayOfWeek-1],day,months[month-1],year);
      }
      if (includeTime == DateTimeType::DateTime) sdate += " ";
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Time)
      {
        DString stime;
        stime.sprintf("%.2d:%.2d:%.2d",hour,minutes,seconds);
        sdate+=stime;
      }
      return convertDigitsToFarsi(sdate);
    }
    DString trDayOfWeek(int dayOfWeek, bool, bool full) override
    {
      static const char *days_short[]   = { "دوشنبه", "سه شنبه", "چهارشنبه", "پنجشنبه", "جمعه", "شنبه", "يكشنبه" };
      static const char *days_full[]    = { "دوشنبه", "سه شنبه", "چهارشنبه", "پنجشنبه", "جمعه", "شنبه", "يكشنبه" };
      DString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      return text;
    }
    DString trMonth(int month, bool, bool full) override
    {
      static const char *months_short[] = { "ژانويه", "فوريه", "مارس", "آوريل", "مه", "ژوئن", "ژوئيه", "اوت", "سپتامبر", "اُكتبر", "نوامبر", "دسامبر" };
      static const char *months_full[]  = { "ژانويه", "فوريه", "مارس", "آوريل", "مه", "ژوئن", "ژوئيه", "اوت", "سپتامبر", "اُكتبر", "نوامبر", "دسامبر" };
      DString text  = full? months_full[month-1] : months_short[month-1];
      return text;
    }
    DString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "قبل‌ازظهر", "بعدازظهر" };
      return dayPeriod[period?1:0];
    }

};

#endif
