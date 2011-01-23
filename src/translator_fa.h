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
#define HtmlLeftToRight     QCString("<div dir=\"rtl\">")
#define HtmlDivEnd          QCString("</div>")


class TranslatorPersian : public Translator
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
			if (str[i] >= '0' && str[i] <= '9')
				output += PersianDigits[ str[i] - '0' ];
			else
				output += str[i];
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
    virtual QCString idLanguage()
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
    virtual QCString latexLanguageSupportCommand()
    {
      return "";
    }

    /*! return the language charset. This will be used for the HTML output */
    virtual QCString idLanguageCharset()
    {
      return "utf-8";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions()
    { return "توابع مربوط"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return "(لازم به ذکر است که اينها توابع عضو نيستند)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return "توضيحات جزئی"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return "مستندات تعریف گونه ها"; }
    
    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return "های عضو Enumeration مستندات"; }
    
    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation()
    { return "توضيحات توابع عضو"; }
    
    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Field مستندات"; 
      }
      else
      {
        return "مستندات متغيير های عضو"; 
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore() 
    { return "بيشتر..."; }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers()
    { return "ليست تمام اعضاء ."; }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList()
    { return "ليست اعضاء"; }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers()
    { return "اين ليستی کامل از همه اعضای  "; }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers()
    { return "شامل همه ی اعضای به ارث برده شده می باشد."; }
    
    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const char *s)
    { QCString result="تولید شده توسط نرم افزار دی اکسیژن ";
      if (s) result+=(QCString)" برای "+s;
      result+=" از کد برنامه "; 
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return "enum نام"; }
    
    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return "enum مقدار"; }
    
    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return "تعریف شده در"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of 
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules()
    { return "Modules"; }
    
    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return "سلسله مراتب کلاس ها"; }
    
    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "ساختار های داده ";
      }
      else
      {
        return "ليست کلاس ها "; 
      }
    }
    
    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return "ليست فايل ها"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "فضاهای داده ها"; 
      }
      else
      {
        return "اعضاء کلاس ها"; 
      }
    }

    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Globals"; 
      }
      else
      {
        return "اعضاء پرونده"; 
      }
    }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages()
    { return "صفحات مربوط"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples()
    { return "مثال ها"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch()
    { return "جستجو"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription()
    { return "ليست و سلسله مراتب درختی کلاس ها به صورت مرتب شده :";
    }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result="ليست همه ي پرونده های ";
      if (!extractAll) result+="(مستند شده) ";
      result+=" :";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
    { 
      
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "ليست ساختارهای داده به همراه توضيحی کوتاه :"; 
      }
      else
      {
        return "ليست کلاس ها ، ساختارهای داده و ... به همراه توضيحات مختصر راجع به آنها :";
      }
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll)
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
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        result+="ساختارهای داده ";
      }
      else
      {
        result+="كلاس ها ";
      }

      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="ليست همه ی توابع ، متغيير ها، تعاريف و ... ";
      if (!extractAll) result+="(مستند شده) ";
      
      result+=" به همراه ارتباط آنها ";
      result+="با پرونده هايی که به آن مربوط اند :";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    virtual QCString trExamplesDescription()
    { return "ليست همه ی مثال ها :"; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return "لیست تمام صفحات و مستندات مربوطه :"; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return "لیست تمام ماژول ها:"; }

    // index titles (the project name is prepended for these) 


    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation()
    { return "مستندات"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * index of all groups.
     */
    virtual QCString trModuleIndex()
    { return "فهرست ماژول ها"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex()
    { return "فهرست سلسله مراتب"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index.
     */
    virtual QCString trCompoundIndex()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trFileIndex() 
    { return "فهرست پرونده ها"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation()
    { return "مستندات ماژول"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "مستندات ساختار داده ها"; 
      }
      else
      {
        return "مستندات کلاس ها"; 
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation()
    { return "مستندات فایل"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    virtual QCString trExampleDocumentation()
    { return "مستندات مثال"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    virtual QCString trPageDocumentation()
    { return "مستندات صفحه"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual()
    { return "راهنمای مرجع"; }
    
    /*! This is used in the documentation of a file as a header before the 
     *  list of defines
     */
    virtual QCString trDefines()
    { return "تعاريف"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of function prototypes
     */
    virtual QCString trFuncProtos()
    { return "پیش تعریف های توابع"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of typedefs
     */
    virtual QCString trTypedefs()
    { return "تعریف گونه ها"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of enumerations
     */
    virtual QCString trEnumerations()
    { return "تعاريف"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) functions
     */
    virtual QCString trFunctions()
    { return "توابع"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trVariables()
    { return "متغيير ها"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues()
    { return "معرف ها"; }
    
    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return "Define Documentation"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for function prototypes
     */
    virtual QCString trFunctionPrototypeDocumentation()
    { return "مستندات تعاریف اولیه توابع"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation()
    { return "Typedef"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation()
    { return "مستندات تعريف"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return "مستندات معرف"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation()
    { return "توضيح تابع"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation()
    { return "توضيح متغير"; }

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trGeneratedAt(const char *date,const char *projName)
    { 
      QCString result = HtmlDivEnd + HtmlRightToLeft + QCString("توليد شده در ") +date ;
      if (projName) result+=(QCString)" برای "+projName;
      result+=(QCString)" توسط";
      return result;
    }
    /*! This is part of the sentence used in the standard footer of each page.
     */
    virtual QCString trWrittenBy()
    {
      return "نوشته شده توسط";
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const char *clName)
    {
      return (QCString)""+clName+" نمودار وراثت برای  :";
    }
    
    /*! this text is generated when the \\internal command is used. */
    virtual QCString trForInternalUseOnly()
    { return ".فقط برای استعمال داخلی"; }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning()
    { return "اخطار"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion()
    { return "نسخه"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate()
    { return "تاريخ"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns()
    { return "خروجی"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso()
    { return "See also"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters()
    { return "پارامترها"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions()
    { return "استثناء ها"; }
    
    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy()
    { return "توليد شده توسط"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////
    
    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return "ليست فضاهای نام"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="در ذيل ليستی از همه ی فضاهای نام  ";
      if (!extractAll) result+="(مستند سازی شده) ";
      result+="به همراه توضيح کوتاه آنها مشاهده می کنيد :";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends()
    { return "کلاس های دوست"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
    /*! used in the class documentation as a header before the list of all
     * related classes 
     */
    virtual QCString trRelatedFunctionDocumentation()
    { return "مستندات توابع مربوط و دوست"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
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
      }
      if (isTemplate) result+=" قالب";
      result=QCString(clName) + " مرجع" +result ;
      return result;
    }

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const char *fileName)
    {
      QCString result=fileName;
      result+=" مرجع پرونده"; 
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" مرجع فضای نام";
      return result;
    }
    
    virtual QCString trPublicMembers()
    { return "توابع عمومی عضو کلاس"; }
    virtual QCString trPublicSlots()
    { return "های عمومی Slot"; }
    virtual QCString trSignals()
    { return "سيگنال ها"; }
    virtual QCString trStaticPublicMembers()
    { return "توابع ثابت عمومی عضو کلاس"; }
    virtual QCString trProtectedMembers()
    { return "توابع خصوصی عضو کلاس"; }
    virtual QCString trProtectedSlots()
    { return "های محافظت شده Slot"; }
    virtual QCString trStaticProtectedMembers()
    { return "توابع ثابت محافظت شده عضو کلاس"; }
    virtual QCString trPrivateMembers()
    { return "توابع خصوصی عضو کلاس"; }
    virtual QCString trPrivateSlots()
    { return "های خصوصی Slot"; }
    virtual QCString trStaticPrivateMembers()
    { return "توابع خصوصی ثابت عضو کلاس"; }
    
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
            result+=" و ";
        }
      }
      return result; 
    }
    
    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries)
    {
      return ".را به ارث می برد "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries)
    {
      return ".اين کلاس را به ارث برده است "+trWriteList(numEntries)+".";
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
      return "تعریف شده است "+trWriteList(numEntries)+"دوباره در ";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers()
    { return "اعضای فضاهای نام"; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
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
    virtual QCString trNamespaceIndex()
    { return "فهرست فضاهای نام"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
    { return "توضيحات فضای نام"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return "فضاهای نام"; }

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
      QCString result=(QCString)"مستندات اين  ";
      switch(compType)
      {
        case ClassDef::Class:      result+="کلاس"; break;
        case ClassDef::Struct:     result+="ساختار داده"; break;
        case ClassDef::Union:      result+="union"; break;
        case ClassDef::Interface:  result+="interface"; break;
        case ClassDef::Protocol:   result+="protocol"; break;
        case ClassDef::Category:   result+="category"; break;
        case ClassDef::Exception:  result+="exception"; break;
      }
      result+=" از روی پرونده ";
      if (single) result+=""; else result+="های ";
      result+="زير توليد شده است :";
      return result;
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    virtual QCString trAlphabeticalList()
    { return "ليست الفبايي"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues()
    { return "مقادير بازگشتی"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return "صفحه ی اصلی"; }

    /*! This is used in references to page that are put in the LaTeX 
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation()
    { return "ص."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////
    virtual QCString trDefinedAtLineInSourceFile()
    {
      return ".در خط @0 از پرونده @1 تعريف شده است";
    }
    virtual QCString trDefinedInSourceFile()
    {
      return ".در فايل @0 تعريف شده است";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated()
    {
      return "منسوخ شده";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)"Collaboration diagram for "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName)
    {
      return (QCString)"نمودار شامل شدن ها برای "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return "مستندات توباع آغازین و پایانی"; 
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    {
      return "کد منبع اين پرونده.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return "توضيحات اين پرونده.";
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition()
    {
      return "پیش شرط";
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition()
    {
      return "پس شرط";
    }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant()
    {
      return "Invariant";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue()
    {
      return "مقدار اوليه :";
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode()
    {
      return "کد";
    }
    virtual QCString trGraphicalHierarchy()
    {
      return "سلسله مراتب گرافيکی کلاس";
    }
    virtual QCString trGotoGraphicalHierarchy()
    {
      return "نمایش نمودار درختی گرافیکی کلاس";
    }
    virtual QCString trGotoTextualHierarchy()
    {
      return "نمایش نمودار درختی متنی کلاس";
    }
    virtual QCString trPageIndex()
    {
      return "فهرست صفحات";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////
    
    virtual QCString trNote()
    {
      return "نکته";
    }
    virtual QCString trPublicTypes()
    {
      return "های عمومی Type";
    }
    virtual QCString trPublicAttribs()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "فضاهای داده ها";
      }
      else
      {
        return "خواص (متغييرهای) عمومی";
      }
    }
    virtual QCString trStaticPublicAttribs()
    {
      return "خواص (متغييرهای) عمومی ثابت";
    }
    virtual QCString trProtectedTypes()
    {
      return "های حفاظت شده Type";
    }
    virtual QCString trProtectedAttribs()
    {
      return "خواص (متغييرهای) حفاظت شده";
    }
    virtual QCString trStaticProtectedAttribs()
    {
      return "خواص (متغييرهای) حفاظت شده ثابت";
    }
    virtual QCString trPrivateTypes()
    {
      return "های خصوصی Type";
    }
    virtual QCString trPrivateAttribs()
    {
      return "خواص (متغييرهای) خصوصی";
    }
    virtual QCString trStaticPrivateAttribs()
    {
      return "خواص (متغييرهای) خصوصی ثابت";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    virtual QCString trTodo()
    {
      return "برای انجام";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    {
      return "ليست کارهاي آينده";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
      return " استفاده شده توسط ";
    }
    virtual QCString trRemarks()
    {
      return "ملاحظات";
    }
    virtual QCString trAttention()
    {
      return "توجه";
    }
    virtual QCString trInclByDepGraph()
    {
      return "این نمودار فایل هایی را که این فایل را به طور مستقیم یا غیر مستقیم استفاده کرده اند نشان می دهد";
    }
    virtual QCString trSince()
    {
      return "از";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    {
      return "راهنمای نمودار";
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
      return "راهنما";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////
    
    /*! Used as a marker that is put before a test item */
    virtual QCString trTest()
    {
      return "تست";
    }
    /*! Used as the header of the test list */
    virtual QCString trTestList()
    {
      return "Test List";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for KDE-2 IDL methods */
    virtual QCString trDCOPMethods()
    {
      return "DCOP توابع عضو";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties()
    {
      return "خاصیت ها";
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
      return "مستندات خاصیت";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "ساختار های داده";
      }
      else
      {
        return "كلاس ها";
      }
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const char *name)
    {
      return (QCString)"Package "+name;
    }
    /*! Title of the package index page */
    virtual QCString trPackageList()
    {
      return "لیست بسته ها";
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return "لیست بسته ها به همراه توضیح مختر در صورت وجود :";
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return "بسته ها";
    }
    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue()
    {
      return "مقدار:";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////
    
    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug()
    {
      return "اشکال";
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList()
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
      return "فهرست";
    }
   
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "کلاس" : "کلاس"));
      if (!singular)  result+="ها";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trFile(bool /*first_capital*/, bool singular)
    { 
      QCString result("پرونده");
      if (!singular) result=result+" ها";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "فضای نام " : "فضای نام "));
      if (!singular)  result+="ها";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Group" : "group"));
      if (!singular)  result+="s";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "صفحه" : "صفحه"));
      if (!singular)  result+=" ها ";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "عضو" : "عضو"));
      if (!singular)  result+="ها";
      return result; 
    }
   
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGlobal(bool first_capital, bool singular)
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
    virtual QCString trAuthor(bool first_capital, bool singular)
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
    virtual QCString trReferences()
    {
      return "را استفاده ميکند ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of 
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    {
      return "را تکميل می کند "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      return ".تکميل شده است "+trWriteList(numEntries)+" در";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      return "فهرست";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been 
     *  flagged deprecated 
     */
    virtual QCString trDeprecatedList()
    {
      return "لیست آیتم های از رده خارج";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in 
     * a C# program
     */
    virtual QCString trEvents()
    {
      return "رویداد ها";
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      return "مستندات رویداد";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    { 
      return "انواع بسته ها";
    }
    /*! Used as a heading for a list of Java class functions with package 
     * scope. 
     */
    virtual QCString trPackageMembers()
    { 
      return "توابع بسته ها";
    }
    /*! Used as a heading for a list of static Java class functions with 
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    { 
      return "Static Package Functions";
    }
    /*! Used as a heading for a list of Java class variables with package 
     * scope.
     */
    virtual QCString trPackageAttribs()
    { 
      return "خواص بسته ها";
    }
    /*! Used as a heading for a list of static Java class variables with 
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    { 
      return "Static Package Attributes";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page 
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll()
    {
      return "همه";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph()
    {
      return "نمودار صدا زدن برای این تابع :";
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
      return "جستجو برای";
    }
    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle()
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
    virtual QCString trSearchResults(int numDocuments)
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
    virtual QCString trSearchMatches()
    {
      return "Matches:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename)
    {
      return filename + " کد و پرونده منبع";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex()
    { return "ساختار و سلسله مراتب شاخه ها"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation()
    { return "مستندات دايرکتوری"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories()
    { return "شاخه ها"; }

    /*! This returns a sentences that introduces the directory hierarchy. 
     *  and the fact that it is sorted alphabetically per level
     */
    virtual QCString trDirDescription()
    { return "در ذيل ساختار شاخه ها و دايرکتوری ها را نسبتا مرتب شده می بينيد :";
    }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const char *dirName)
    { QCString result=dirName; result+=" مرجع شاخه ی"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool /*first_capital*/, bool singular)
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
    virtual QCString trOverloadText()
    {
       return "This is an overloaded member function, "
              "provided for convenience. It differs from the above "
              "function only in what argument(s) it accepts.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    virtual QCString trCallerGraph()
    {
      return "نمودار صدا زننده های این تابع:";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////
    
    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran()
    { return "مستندات توابع عضو"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */    
    virtual QCString trCompoundListFortran()
    { return "لیست ساختار های داده"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran()
    { return "فیلدهای اطلاعات"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran()
    { return "ساختارهای داده به همراه توضیح کوتاه :"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool)
    {
      return "توضیحات اعضا ساختارها به همراه مستندات ساختار داده ی مربوطه";
    }

    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index (Fortran).
     */
    virtual QCString trCompoundIndexFortran()
    { return "Data Type Index"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation()
    { return "Data Type Documentation"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms()
    { return "توابع و زیربرنامه ها"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation()
    { return "مستندات توابع و زیربرنامه ها"; }

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes()
    { return "ساختار های داده"; }
    
    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList()
    { return "لیست ماژول ها"; }

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool)
    {
      QCString result="لیست ماژول ها به همراه توضیحات کوتاه";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    virtual QCString trCompoundReferenceFortran(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result=(QCString)clName;
      switch(compType)
      {
        case ClassDef::Class:      result+=" Module"; break;
        case ClassDef::Struct:     result+=" Type"; break;
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
    /*! used as the title of the HTML page of a module (Fortran) */
    virtual QCString trModuleReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" Module Reference";        
      return result;
    }
    
    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers()
    { return "اعضاء ماژول"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool)
    { 
      QCString result="لیست اعضاء ماژول ها به همراه مستندات ماژول مربوطه";
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the 
     *  index of all modules (Fortran).
     */
    virtual QCString trModulesIndex()
    { return "Modules Index"; }
    
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trModule(bool, bool singular)
    {       
      QCString result("ماژول");
      if (!singular)  result+=" ها";
      return result; 
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single)
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      QCString result=(QCString)"The documentation for this ";
      switch(compType)
      {
        case ClassDef::Class:      result+="module"; break;
        case ClassDef::Struct:     result+="type"; break;
        case ClassDef::Union:      result+="union"; break;
        case ClassDef::Interface:  result+="interface"; break;
        case ClassDef::Protocol:   result+="protocol"; break;
        case ClassDef::Category:   result+="category"; break;
        case ClassDef::Exception:  result+="exception"; break;
      }
      result+=" was generated from the following file";
      if (single) result+=":"; else result+="s:";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trType(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Type" : "type"));
      if (!singular)  result+="s";
      return result; 
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trSubprogram(bool, bool singular)
    { 
      QCString result("زیربرنامه");
      if (!singular)  result+=" ها ";
      return result; 
    }

    /*! C# Type Constraint list */
    virtual QCString trTypeConstraints()
    {
      return "Type Constraints";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    virtual QCString trDirRelation(const char *name)
    {
      return QCString(name) + " Relation";
    }

    /*! Loading message shown when loading search results */
    virtual QCString trLoading()
    {
      return "در حال بارگذاری...";
    }

    /*! Label used for search results in the global namespace */
    virtual QCString trGlobalNamespace()
    {
      return "فضای نام جهانی";
    }

    /*! Message shown while searching */
    virtual QCString trSearching()
    {
      return "در حال جستجو...";
    }

    /*! Text shown when no search results are found */
    virtual QCString trNoMatches()
    {
      return "یافت نشد";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.3 (missing items for the directory pages)
//////////////////////////////////////////////////////////////////////////

    /*! introduction text for the directory dependency graph */
    virtual QCString trDirDependency(const char *name)
    {
      return (QCString)"نمودار وابستگی دایرکتوری ها برای "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the first column mentions the
     *  source file that has a relation to another file.
     */
    virtual QCString trFileIn(const char *name)
    {
      return (QCString)"پرونده ای در "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    virtual QCString trIncludesFileIn(const char *name)
    {
      return (QCString)"Includes file in "+name;
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
      static const char *days[]   = { "دوشنبه","سه‌شنبه","چهارشنبه","پنجشنبه","جمعه","شنبه","یکشنبه" };
      static const char *months[] = { "ژانویه","فوریه","مارس","آوریل","می","جون","جولای","آگوست","سپتامبر","اکتبر","نوامبر","دسامبر" };
      QCString sdate;
      sdate.sprintf("%s %d %s %d",days[dayOfWeek-1],day,months[month-1],year);
      if (includeTime)
      {
        QCString stime;
        stime.sprintf(" %.2d:%.2d:%.2d",hour,minutes,seconds);
        sdate+=stime;
      }
      return convertDigitsToFarsi(sdate);
    }

};

#endif
