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

#ifndef TRANSLATOR_VI_H
#define TRANSLATOR_VI_H

/*
 * 17 Oct 2008 : Translation to Vietnamese by
 *               Đặng Minh Tuấn <tuanvietkey@gmail.com>
 *
 */


/*!
 When defining a translator class for the new language, follow
 the description in the documentation.  One of the steps says
 that you should copy the translator_en.h (this) file to your
 translator_xx.h new file.  Your new language should use the
 Translator class as the base class.  This means that you need to
 implement exactly the same (pure virtual) override methods as the
 TranslatorEnglish does.  Because of this, it is a good idea to
 start with the copy of TranslatorEnglish and replace the strings
 one by one.

 It is not necessary to include "translator.h" or
 "translator_adapter.h" here.  The files are included in the
 language.cpp correctly.  Not including any of the mentioned
 files frees the maintainer from thinking about whether the
 first, the second, or both files should be included or not, and
 why.  This holds namely for localized translators because their
 base class is changed occasionally to adapter classes when the
 Translator class changes the interface, or back to the
 Translator class (by the local maintainer) when the localized
 translator is made up-to-date again.
*/
class TranslatorVietnamese : public TranslatorAdapter_1_6_0
{
  public:

    // --- Language control methods -------------------

    /*! Used for identification of the language. The identification
     * should not be translated. It should be replaced by the name
     * of the language in English using lower-case characters only
     * (e.g. "czech", "japanese", "russian", etc.). It should be equal to
     * the identification used in language.cpp.
     */
    virtual QCString idLanguage() override
    { return "vietnamese"; }

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
    virtual QCString latexLanguageSupportCommand() override
    {
      return
        "\\usepackage[vietnamese]{babel}\n";
    }
    virtual QCString latexFontenc() override
    {
      return "";
    }
    virtual QCString latexCommandName() override
    {
      QCString latex_command = Config_getString(LATEX_CMD_NAME);
      if (latex_command.isEmpty()) latex_command = "latex";
      if (Config_getBool(USE_PDFLATEX))
      {
        if (latex_command == "latex") latex_command = "xelatex";
      }
      return latex_command;
    }
    virtual QCString trISOLang() override
    {
      return "vi";
    }
    virtual QCString getLanguageString() override
    {
      return "0x42A Vietnamese";
    }
    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions() override
    { return "Những hàm liên quan"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript() override
    { return "(Chú ý những hàm này không phải là hàm thành viên.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription() override
    { return "Mô tả chi tiết"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    virtual QCString trDetails() override
    { return "Chi tiết"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation() override
    { return "Thông tin về Member Typedef"; }

    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation() override
    { return "Thông tin về Member Enumeration"; }

    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation() override
    { return "Thông tin về hàm thành viên"; }

    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Thông tin về trường";
      }
      else
      {
        return "Thông tin về Member Data";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore() override
    { return "Tiếp..."; }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers() override
    { return "Liệt kê tất cả các thành viên"; }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList() override
    { return "Danh sách thành viên"; }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers() override
    { return "Danh sách các thành viên đầy đủ cho "; }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers() override
    { return ", cùng với tất cả các thành viên kế thừa."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="Được tạo ra bởi Doxygen";
      if (!s.isEmpty()) result+=" cho "+s;
      result+=" từ mã nguồn.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName() override
    { return "tên enum"; }

    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue() override
    { return "giá trị enum"; }

    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn() override
    { return "được định nghĩa trong"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules() override
    { return "Các Modules"; }

    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy() override
    { return "Kiến trúc Class"; }

    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Cấu trúc cơ sở dữ liệu (Data Structures)";
      }
      else
      {
        return "Danh mục các Class";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList() override
    { return "Danh mục File"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Các trường dữ liệu";
      }
      else
      {
        return "Các thành viên của Class";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Toàn cục";
      }
      else
      {
        return "File thành viên";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages() override
    { return "Các trang liên quan"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples() override
    { return "Các ví dụ"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch() override
    { return "Tìm kiếm"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription() override
    { return "Danh sách kế thừa đã được sắp xếp theo ABC, "
             "nhưng chưa đầy đủ:";
    }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll) override
    {
      QCString result="Danh mục đầy đủ tất cả các ";
      if (!extractAll) result+="(đã được biên soạn) ";
      result+="files cùng với các mô tả tóm tắt:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription() override
    {

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Đây là cấu trúc cơ sở dữ liệu với mô tả tóm tắt:";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
      {
        return "Đây là các classes với các mô tả tóm tắt:";
      }
      else
      {
        return "Đây là các classes, structs, "
               "unions và interfaces với các mô tả tóm tắt:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll) override
    {
      QCString result="Danh mục tất cả các ";
      if (!extractAll)
      {
        result+="(đã được mô tả) ";
      }
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="struct và union fields";
      }
      else
      {
        result+="class members";
      }
      result+=" cùng với các các liên kết đến ";
      if (!extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="Thông tin về struct/union cho từng trường:";
        }
        else
        {
          result+="Thông tin về class cho từng thành viên:";
        }
      }
      else
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="các structures/unions thuộc:";
        }
        else
        {
          result+="các lớp thuộc:";
        }
      }
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    virtual QCString trFileMembersDescription(bool extractAll) override
    {
      QCString result="Danh sách tất cả các ";
      if (!extractAll) result+="(đã được mô tat) ";

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="functions, variables, defines, enums, và typedefs";
      }
      else
      {
        result+="các file thành viên";
      }
      result+=" cùng với links tới ";
      if (extractAll)
        result+="các files thuộc:";
      else
        result+="tài liệu:";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    virtual QCString trExamplesDescription() override
    { return "Danh sách tất cả các ví dụ:"; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription() override
    { return "Danh sách tất cả các trang Thông tin có liên quan:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription() override
    { return "Danh sách tất cả các thành viên:"; }

    // index titles (the project name is prepended for these)

    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation() override
    { return "Thông tin"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    virtual QCString trModuleIndex() override
    { return "Chỉ mục (Index) Module"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex() override
    { return "Hierarchical Index"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    virtual QCString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Index cấu trúc cơ sở dữ liệu";
      }
      else
      {
        return "Class Index";
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    virtual QCString trFileIndex() override
    { return "File Index"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation() override
    { return "Thông tin về các Module"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Thông tin về cấu trúc cơ sở dữ liệu";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return "Thông tin về Class";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation() override
    { return "Thông tin về File"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual() override
    { return "Thông tin tham chiếu"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    virtual QCString trDefines() override
    { return "Định nghĩa"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    virtual QCString trTypedefs() override
    { return "Typedefs"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    virtual QCString trEnumerations() override
    { return "Enumerations"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    virtual QCString trFunctions() override
    { return "Các hàm"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trVariables() override
    { return "Các biến"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues() override
    { return "Enumerator"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation() override
    { return "Thông tin về định nghĩa"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation() override
    { return "Thông tin về Typedef"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation() override
    { return "Thông tin về Enumeration Type"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation() override
    { return "Thông tin về hàm"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation() override
    { return "Thông tin về các biến"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Cấu trúc cơ sở dữ liệu";
      }
      else
      {
        return "Classes";
      }
    }

    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    virtual QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result="Được biên soạn vào "+date;
      if (!projName.isEmpty()) result+=" cho mã nguồn dự án "+projName;
      result+=" bởi";
      return result;
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const QCString &clName) override
    {
      return "Sơ đồ kế thừa cho "+clName+":";
    }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning() override
    { return "Lưu ý"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion() override
    { return "Phiên bản"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate() override
    { return "Ngày"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns() override
    { return "Giá trị trả về"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso() override
    { return "Xem thêm"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters() override
    { return "Các tham số"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions() override
    { return "Exceptions"; }

    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy() override
    { return "Được biên soạn bởi"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList() override
    { return "Danh sách Namespace"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="Danh sách tất cả các ";
      if (!extractAll) result+="(đã được biên tập) ";
      result+="namespaces với mô tả tóm tắt:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends() override
    { return "Friends"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    virtual QCString trRelatedFunctionDocumentation() override
    { return "Thông tin về Friends và các hàm liên quan"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const QCString &clName,
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
      result+=" Tham chiếu";
      return result;
    }

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const QCString &fileName) override
    {
      QCString result=fileName;
      result+=" File Tham chiếu";
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" Namespace Tham chiếu";
      return result;
    }

    virtual QCString trPublicMembers() override
    { return "Các hàm thành viên Public"; }
    virtual QCString trPublicSlots() override
    { return "Public Slots"; }
    virtual QCString trSignals() override
    { return "Signals"; }
    virtual QCString trStaticPublicMembers() override
    { return "Các hàm Static Public"; }
    virtual QCString trProtectedMembers() override
    { return "Các hàm thành viên Protected"; }
    virtual QCString trProtectedSlots() override
    { return "Protected Slots"; }
    virtual QCString trStaticProtectedMembers() override
    { return "Các hàm thành viên Static Protected"; }
    virtual QCString trPrivateMembers() override
    { return "Các hàm thành viên Private"; }
    virtual QCString trPrivateSlots() override
    { return "Private Slots"; }
    virtual QCString trStaticPrivateMembers() override
    { return "Các hàm thành viên Static Private"; }

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
            result+=", và ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries) override
    {
      return "Kế thừa "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries) override
    {
      return "Được kế thừa bởi "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries) override
    {
      return "Được thực thi lại từ "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries) override
    {
      return "Được thực thi lại trong "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers() override
    { return "Thành viên Namespace"; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll) override
    {
      QCString result="Danh sách tất cả các ";
      if (!extractAll) result+="(đã được biên soạn) ";
      result+="các thành viên namespace cùng với link tới ";
      if (extractAll)
        result+="Thông tin namespace cho từng thành viên:";
      else
        result+=" namespaces mà phụ thuộc bởi:";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    virtual QCString trNamespaceIndex() override
    { return "Namespace Index"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation() override
    { return "Thông tin về Namespace"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces() override
    { return "Namespaces"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFiles(ClassDef::CompoundType compType,bool) override
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      QCString result="Thông tin cho ";
      switch(compType)
      {
        case ClassDef::Class:      result+="class"; break;
        case ClassDef::Struct:     result+="struct"; break;
        case ClassDef::Union:      result+="union"; break;
        case ClassDef::Interface:  result+="interface"; break;
        case ClassDef::Protocol:   result+="protocol"; break;
        case ClassDef::Category:   result+="category"; break;
        case ClassDef::Exception:  result+="exception"; break;
        default: break;
      }
      result+=" được biên soạn từ các file sau đây";
      result+=":";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues() override
    { return "Các giá trị trả về"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage() override
    { return "Tranh chính"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation() override
    { return "tr."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDefinedAtLineInSourceFile() override
    {
      return "Định nghĩa tại dòng @0 trong file @1.";
    }
    virtual QCString trDefinedInSourceFile() override
    {
      return "Định nghĩa trong file @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated() override
    {
      return "Không tán thành";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const QCString &clName) override
    {
      return "Sơ đồ liên kết cho "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const QCString &fName) override
    {
      return "Kèm theo graph phụ thuộc cho "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation() override
    {
      return "Thông tin về Constructor và Destructor";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode() override
    {
      return "Tới mã nguồn của file này.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation() override
    {
      return "Tới Thông tin của file này.";
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition() override
    {
      return "Điều kiện trước";
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition() override
    {
      return "Điều kiện sau";
    }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant() override
    {
      return "Bất biến";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue() override
    {
      return "Giá trị khởi tạo:";
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode() override
    {
      return "mã nguồn";
    }
    virtual QCString trGraphicalHierarchy() override
    {
      return "Kiến trúc đồ họa của Class";
    }
    virtual QCString trGotoGraphicalHierarchy() override
    {
      return "Tới kiến trúc đồ họa của Class";
    }
    virtual QCString trGotoTextualHierarchy() override
    {
      return "Tới kiến trúc text của Class";
    }
    virtual QCString trPageIndex() override
    {
      return "Chỉ mục trang";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    virtual QCString trNote() override
    {
      return "Ghi chú";
    }
    virtual QCString trPublicTypes() override
    {
      return "Public Types";
    }
    virtual QCString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "các trường đữ liệu";
      }
      else
      {
        return "Các thuộc tính Public";
      }
    }
    virtual QCString trStaticPublicAttribs() override
    {
      return "Các thuộc tính Static Public";
    }
    virtual QCString trProtectedTypes() override
    {
      return "Các kiểu Protected";
    }
    virtual QCString trProtectedAttribs() override
    {
      return "các thuộc tính Protected";
    }
    virtual QCString trStaticProtectedAttribs() override
    {
      return "Các thuộc tính Static Protected";
    }
    virtual QCString trPrivateTypes() override
    {
      return "Các kiểu Private";
    }
    virtual QCString trPrivateAttribs() override
    {
      return "Các thuộc tính Private";
    }
    virtual QCString trStaticPrivateAttribs() override
    {
      return "Các thuộc tính Static Private";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    virtual QCString trTodo() override
    {
      return "Mục tiêu";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList() override
    {
      return "Danh sách hàng mục cần thực hiện";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy() override
    {
      return "Tham chiếu bởi";
    }
    virtual QCString trRemarks() override
    {
      return "Ghi chú";
    }
    virtual QCString trAttention() override
    {
      return "Chú ý";
    }
    virtual QCString trInclByDepGraph() override
    {
      return "Đồ thị này biểu thị những file nào trực tiếp hoặc"
             "không trực tiếp bao gồm file này:";
    }
    virtual QCString trSince() override
    {
      return "Từ";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle() override
    {
      return "Chú giải Graph";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    virtual QCString trLegendDocs() override
    {
      return
        "Trang này giải nghĩa các biểu đồ được biên soạn bởi "
        " doxygen.<p>\n"
        "Hãy xem xét các ví dụ sau:\n"
        "\\code\n"
        "/*! Các lướp không thấy được bởi sự cắt ngắn */\n"
        "Lớp không nhìn thấy class { };\n\n"
        "/*! class bị cắt, quan hệ kế thừa bị ẩn */\n"
        "class bị cắt : bị ẩn toàn cục { };\n\n"
        "/* Class không được mô tả với các chú giải doxygen */\n"
        "class không được mô tả { };\n\n"
        "/*! Class được kế thừa sử dụng các kế thừa public */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*!  template class */\n"
        "template<class T> class Templ { };\n\n"
        "/*! Class được kế thừa sử dụng kế thừa protected */\n"
        "class ProtectedBase { };\n\n"
        "/*! Class được kế thừa sử dụng kế thừa protected private */\n"
        "class PrivateBase { };\n\n"
        "/*! Class được sử dụng bởi các class kế thừa */\n"
        "class được sử dụng { };\n\n"
        "/*! Super class kế thừa một số các class khác */\n"
        "class được kế thừa : public PublicBase,\n"
        "                  protected ProtectedBase,\n"
        "                  private PrivateBase,\n"
        "                  public Undocumented,\n"
        "                  public Templ<int>\n"
        "{\n"
        "  private:\n"
        "    Used *m_usedClass;\n"
        "};\n"
        "\\endcode\n"
        "Kết quả trong biểu đồ sau đây:"
        "<p><center><img alt=\"\" src=\"graph_legend."+getDotImageExtension()+"\"></center>\n"
        "<p>\n"
        "Các hộp trong biểu đồ trên có ý nghĩa như sau:\n"
        "<ul>\n"
        "<li>%Hộp màu xám biểu thị cấu trúc clas cho "
        "biểu đồ được thể hiện.\n"
        "<li>%Hộp có khung mầu đen biểu thị struct hoặc class được mô tả.\n"
        "<li>%Hộp có khung mầu xám biểu thị struct hoặc class chưa được mô tả.\n"
        "<li>%Hộp có khung mầu đỏ biểu thị struct hoặc class được mổ tả "
        "khi mà không phải tất cả các quan hệ kế thừa/containment được biển diễn.. %Biểu đồ bị "
        "cắt nếu nó không nằm trọn trong các biên được cho trước..\n"
        "</ul>\n"
        "Các mũi tên có ý nghĩa như sau::\n"
        "<ul>\n"
        "<li>%Mũi tên mầu xanh đậm biểu thị các quan hệ kế thừa công khai "
        "giữa 2 class.\n"
        "<li>%Mũi tên màu xanh lá cây đậm biểu thị kế thừa được bảo về (protected).\n"
        "<li>%Mũi tên đỏ đậm biểu thị kế thừa dạng private.\n"
        "<li>%Mũi tên màu hồng đứt quảng biểu thị class được sử dụng "
        "bởi class khác. Mũi tên được gán nhãn với các giá trị "
        "mà các calsss hoặc struct được truy cập tới.\n"
        "<li>%Mũi tên vàng đắt quãng được thị quan hệ giữa template instance và "
        "template class được dẫn xuất từ đó. Mũi tên được gán nhãn với "
        "tham số của template.\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    virtual QCString trLegend() override
    {
      return "Chú giải";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    virtual QCString trTest() override
    {
      return "Test";
    }
    /*! Used as the header of the test list */
    virtual QCString trTestList() override
    {
      return "Danh sách Test";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties() override
    {
      return "Thuộc tính";
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation() override
    {
      return "Thông tin thuộc tính (Property)";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Cấu trúc dữ liệu";
      }
      else
      {
        return "Classes";
      }
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const QCString &name) override
    {
      return "Gói "+name;
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription() override
    {
      return "Danh sách các gói cùng với mô tả tóm tắt (khi có thể có):";
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages() override
    {
      return "Các gói";
    }
    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue() override
    {
      return "Giá trị:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug() override
    {
      return "Lỗi";
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList() override
    {
      return "Danh sách lỗi";
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
    virtual QCString trRTFansicp() override
    {
      return "1258";
    }


    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    virtual QCString trRTFCharSet() override
    {
      return "163";
    }

    /*! Used as header RTF general index */
    virtual QCString trRTFGeneralIndex() override
    {
      return "Chỉ số";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Class" : "class"));
      if (!singular)  result+="es";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trFile(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "File" : "file"));
      if (!singular)  result+="s";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Namespace" : "namespace"));
      if (!singular)  result+="s";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Group" : "group"));
      if (!singular)  result+="s";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Trang" : "trang"));
      if (!singular)  result+="";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Member" : "member"));
      if (!singular)  result+="s";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGlobal(bool first_capital, bool singular) override
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
    virtual QCString trAuthor(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Tác giả" : "tác giả"));
      if (!singular)  result+="";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences() override
    {
      return "Tham chiếu";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries) override
    {
      return "Thực hiện "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    virtual QCString trImplementedInList(int numEntries) override
    {
      return "Được thực hiện trong "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents() override
    {
      return "Mục lục";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    virtual QCString trDeprecatedList() override
    {
      return "Danh sách Deprecated";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    virtual QCString trEvents() override
    {
      return "Sự kiện";
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation() override
    {
      return "Thông tin về sự kiện";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes() override
    {
      return "Kiểu gói";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    virtual QCString trPackageFunctions() override
    {
      return "Các hàm Package";
    }
    virtual QCString trPackageMembers() override
    {
      return "Members Package";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    virtual QCString trStaticPackageFunctions() override
    {
      return "Các hàm Static Package";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    virtual QCString trPackageAttribs() override
    {
      return "Các thuộc tính Package";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    virtual QCString trStaticPackageAttribs() override
    {
      return "Các thuộc tính Static Package";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll() override
    {
      return "Tất cả";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph() override
    {
      return "Biểu đồ các lời gọi cho hàm này:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle() override
    {
      return "Kết quả tìm kiếm";
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
        return "Không có tài liệu nào thỏa mãn các truy vấn của bạn.";
      }
      else if (numDocuments==1)
      {
        return "Tìm thấy <b>1</b> tài liệu thỏa mã truy vấn của bạn.";
      }
      else
      {
        return "Tìm thấy tất cả <b>$num</b> tài liệu thỏa mã truy vấn của bạn. "
               "Hiển thị những thỏa mãn tốt nhất trước.";
      }
    }
    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    virtual QCString trSearchMatches() override
    {
      return "Các kết quả thỏa mãn đk:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename) override
    {
      return filename + " File nguồn";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex() override
    { return "Cấu trúc thư mục"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation() override
    { return "Thông tin về thư mục"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories() override
    { return "Các thư mục"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const QCString &dirName) override
    { QCString result=dirName; result+=" Tham chiếu thư mục"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool first_capital, bool) override
    {
      QCString result((first_capital ? "Thư mục" : "thư mục"));
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
       return "Hàm thành viên dạng overloaded, "
              "được chỉ ra cho việc tra cứu dễ dàng. Nó khác với hàm ở trên"
              "chỉ ở chỗ những tham số nào nó chấp nhận.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    virtual QCString trCallerGraph() override
    {
      return "Biểu đồ các lời gọi cho hàm này:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation() override
    { return "Thông tin Enumerator"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran() override
    { return "Thông tin về các hàm và các thủ tục thành viên"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    virtual QCString trCompoundListFortran() override
    { return "Danh sách kiêu dữ liệu"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran() override
    { return "Trường dữ liệu"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran() override
    { return "Kiểu dữ liệu với các mô tả tóm tắt:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll) override
    {
      QCString result="Danh sách tất cả ";
      if (!extractAll)
      {
        result+="(đã được mô tả) ";
      }
      result+="các kiểu dữ liệu thành viên";
      result+=" cùng với liên kết với ";
      if (!extractAll)
      {
         result+="Thông tin cấu trúc dữ liệu cho từng thành viên";
      }
      else
      {
         result+=" các kiểu dữ liệu thuộc:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index (Fortran).
     */
    virtual QCString trCompoundIndexFortran() override
    { return "Chỉ mục kiểu dữ liệu"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation() override
    { return "Thông tin về kiểu dữ liệu"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms() override
    { return "Functions/Subroutines"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation() override
    { return "Thông tin về Function/Subroutine"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes() override
    { return "Kiểu dữ liệu"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList() override
    { return "Danh sách Modules"; }

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll) override
    {
      QCString result="Danh sách tất cả ";
      if (!extractAll) result+="(đã được mô tả) ";
      result+="các module với mô tả tóm tắt:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    virtual QCString trCompoundReferenceFortran(const QCString &clName,
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
      result+=" Tham chiếu";
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    virtual QCString trModuleReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" Tham chiếu Module";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers() override
    { return "Thành viên Module"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll) override
    {
      QCString result="Danh sách tất cả ";
      if (!extractAll) result+="(đã được mô tả) ";
      result+="các module thành viên cùng với liên kết tới ";
      if (extractAll)
      {
        result+="Thông tin module cho từng thành viên:";
      }
      else
      {
        result+="các module thuộc:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all modules (Fortran).
     */
    virtual QCString trModulesIndex() override
    { return "Chỉ mục các Module"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trModule(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Module" : "module"));
      if (!singular)  result+="";
      return result;
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      QCString result="Thông tin cho ";
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
      result+=" được biên soạn từ các file sau đây";
      if (single) result+=":"; else result+="s:";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trType(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Kiểu" : "kiểu"));
      if (!singular)  result+="";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trSubprogram(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Chương trình con" : "chương trình con"));
      if (!singular)  result+="";
      return result;
    }

    /*! C# Type Constraint list */
    virtual QCString trTypeConstraints() override
    {
      return "Ràng buộc của kiểu (Type)";
    }

    virtual QCString trDayOfWeek(int dayOfWeek, bool, bool full) override
    {
      static const char *days_short[]   = { "T2", "T3", "T4", "T5", "T6", "T7", "CN" };
      static const char *days_full[]    = { "Thứ Hai", "Thứ Ba", "Thứ Tư", "Thứ Năm", "Thứ Sáu", "Thứ Bảy", "Chủ Nhật" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      return text;
    }
    virtual QCString trMonth(int month, bool, bool full) override
    {
      static const char *months_short[] = { "Thg1", "Thg2", "Thg3", "Thg4", "Thg5", "Thg6", "Thg7", "Thg8", "Thg9", "Thg10", "Thg11", "Thg12" };
      static const char *months_full[]  = { "Tháng Giêng", "Tháng Hai", "Tháng Ba", "Tháng Tư", "Tháng Năm", "Tháng Sáu", "Tháng Bảy", "Tháng Tám", "Tháng Chín", "Tháng Mười", "Tháng Mười Một", "Tháng Mười Hai" };
      QCString text  = full? months_full[month-1] : months_short[month-1];
      return text;
    }
    virtual QCString trDayPeriod(int period) override
    {
      static const char *dayPeriod[] = { "SA", "CH" };
      return dayPeriod[period];
    }
};

#endif
