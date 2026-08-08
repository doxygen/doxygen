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

#ifndef TRANSLATOR_TR_H
#define TRANSLATOR_TR_H

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
class TranslatorTurkish : public TranslatorAdapter_1_7_5
{
  public:

    // --- Language control methods -------------------

    /*! Used for identification of the language. The identification
     * should not be translated. It should be replaced by the name
     * of the language in English using lower-case characters only
     * (e.g. "czech", "japanese", "russian", etc.). It should be equal to
     * the identification used in language.cpp.
     */
    DString idLanguage() override
    { return "turkish"; }

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
    DString latexLanguageSupportCommand() override
    {
      return "\\usepackage[turkish]{babel}\n";
    }
    DString trISOLang() override
    {
      return "tr";
    }
    DString getLanguageString() override
    {
      return "0x41F Turkey";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related fonksiyonlar. */
    DString trRelatedFunctions() override
    { return "İlgili Fonksiyonlar"; }

    /*! subscript for the related fonksiyonlar. */
    DString trRelatedSubscript() override
    { return "(Not: Bu fonksiyonlar üye fonksiyon değildir.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    DString trDetailedDescription() override
    { return "Ayrıntılı tanımlama"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    DString trDetails() override
    { return "Ayrıntılar"; }

    /*! header that is put before the list of typedefs. */
    DString trMemberTypedefDocumentation() override
    { return "Üye Typedef Dokümantasyonu"; }

    /*! header that is put before the list of enumerations. */
    DString trMemberEnumerationDocumentation() override
    { return "Üye Enümerasyon Dokümantasyonu"; }

    /*! header that is put before the list of member fonksiyonlar. */
    DString trMemberFunctionDocumentation() override
    { return "Üye Fonksiyon Dokümantasyonu"; }

    /*! header that is put before the list of member attributes. */
    DString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Alan Dokümantasyonu";
      }
      else
      {
        return "Üye Veri Dokümantasyonu";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    DString trMore() override
    { return "Ayrıntılar..."; }

    /*! put in the class documentation */
    DString trListOfAllMembers() override
    { return "Tüm üyelerin listesi"; }

    /*! used as the title of the "list of all members" page of a class */
    DString trMemberList() override
    { return "Üye Listesi"; }

    /*! this is the first part of a sentence that is followed by a class name */
    DString trThisIsTheListOfAllMembers() override
    { return "Şu sınıfın tüm üyelerinin listesidir:"; }

    /*! this is the remainder of the sentence after the class name */
    DString trIncludingInheritedMembers() override
    { return "  (kalıtımla gelen üyeleri de içerir)."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    DString trGeneratedAutomatically(const DString &s) override
    { DString result="Doxygen tarafından";
      if (!s.empty()) result+=s+" için ";
      result+=" kaynak koddan otomatik üretilmiştir.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    DString trEnumName() override
    { return "enum adı"; }

    /*! put after an enum value in the list of all members */
    DString trEnumValue() override
    { return "enum değeri"; }

    /*! put after an undocumented member in the list of all members */
    DString trDefinedIn() override
    { return "tanımlandığı yer"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    DString trModules() override
    { return "Modüller"; }

    /*! This is put above each page as a link to the class hierarchy */
    DString trClassHierarchy() override
    { return "Sınıf Hiyerarşisi"; }

    /*! This is put above each page as a link to the list of annotated classes */
    DString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Veri Yapıları";
      }
      else
      {
        return "Sınıf Listesi";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    DString trFileList() override
    { return "Dosya Listesi"; }

    /*! This is put above each page as a link to all members of compounds. */
    DString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Veri Alanları";
      }
      else
      {
        return "Sınıf Üyeleri";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    DString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Globaller";
      }
      else
      {
        return "Dosya Üyeleri";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    DString trRelatedPages() override
    { return "İlgili Sayfalar"; }

    /*! This is put above each page as a link to all examples. */
    DString trExamples() override
    { return "Örnekler"; }

    /*! This is put above each page as a link to the search engine. */
    DString trSearch() override
    { return "Ara"; }

    /*! This is an introduction to the class hierarchy. */
    DString trClassHierarchyDescription() override
    { return "Bu kalıtım listesi tamamen olmasa da yaklaşık "
             "olarak alfabetik sıraya konulmuştur:";
    }

    /*! This is an introduction to the list with all files. */
    DString trFileListDescription(bool extractAll) override
    {
      DString result="Bu liste tüm ";
      if (!extractAll) result+="dokümante edilmiş ";
      result+="dosyaları kısa açıklamalarıyla göstermektedir:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    DString trCompoundListDescription() override
    {

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Kısa tanımlarıyla birlikte veri yapıları:";
      }
      else
      {
        return "Kısa tanımlarıyla sınıflar, yapılar (struct), "
               "birleşimler(union) ve arayüzler:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    DString trCompoundMembersDescription(bool extractAll) override
    {
      DString result="Bu liste tüm ";
      if (!extractAll)
      {
        result+="dokümante edilmiş ";
      }
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="yapı(struct) ve birleşim(union) yapılarını içerir";
      }
      else
      {
        result+="sınıf üyelerini içerir";
      }
      result+=" ve ";
      if (!extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="her alanı yapı(struct)/birleşim(union) dokümantasyonuna bağlar:";
        }
        else
        {
          result+="her üye için sınıf dokümantasyonuna bağlar:";
        }
      }
      else
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="ait olduğu yapı(struct)/birleşime(union) bağlar:";
        }
        else
        {
          result+="ait olduğu sınıfa bağlar:";
        }
      }
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    DString trFileMembersDescription(bool extractAll) override
    {
      DString result="Bu liste tüm ";
      if (!extractAll) result+="dokümante edilmiş ";

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="fonksiyonları, değişkenleri, makroları, enümerasyonları ve typedef\'leri içerir";
      }
      else
      {
        result+="dosya üyelerini içerir";
      }
      result+=" ve ";
      if (extractAll)
        result+="ait olduğu dosyalar bağlar:";
      else
        result+="dokümantasyona bağlar:";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    DString trExamplesDescription() override
    { return "Tüm örneklerin listesi aşağıdadır:"; }

    /*! This is an introduction to the page with the list of related pages */
    DString trRelatedPagesDescription() override
    { return "İlgili tüm dokümantasyon sayfalarının listesi aşağıdadır:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    DString trModulesDescription() override
    { return "Tüm modüllerin listesi aşağıdadır:"; }

    // index titles (the project name is prepended for these)

    /*! This is used in HTML as the title of index.html. */
    DString trDocumentation(const DString &projName) override
    { return (!projName.empty()?projName + " " : "") + "Dokümantasyonu"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    DString trModuleIndex() override
    { return "Modül İndeksi"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    DString trHierarchicalIndex() override
    { return "Hiyerarşik İndeksi"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    DString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Veri Yapıları İndeksi";
      }
      else
      {
        return "Sınıf İndeksi";
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    DString trFileIndex() override
    { return "Dosya İndeksi"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    DString trModuleDocumentation() override
    { return "Modül Dokümantasyonu"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    DString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Veri Yapıları Dokümantasyonu";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return "Sınıf Dokümantasyonu";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    DString trFileDocumentation() override
    { return "Dosya Dokümantasyonu"; }

    /*! This is used in LaTeX as the title of the document */
    DString trReferenceManual() override
    { return "Referans Kitabı"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    DString trDefines() override
    { return "Makrolar"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    DString trTypedefs() override
    { return "Typedef\'ler"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    DString trEnumerations() override
    { return "Enümerasyonlar"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    DString trFunctions() override
    { return "Fonksiyonlar"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    DString trVariables() override
    { return "Değişkenler"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    DString trEnumerationValues() override
    { return "Enümeratör"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    DString trDefineDocumentation() override
    { return "Makro Dokümantasyonu"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    DString trTypedefDocumentation() override
    { return "Typedef Dokümantasyonu"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    DString trEnumerationTypeDocumentation() override
    { return "Enümerasyon Tipi Dokümantasyonu"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    DString trFunctionDocumentation() override
    { return "Fonksiyon Dokümantasyonu"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    DString trVariableDocumentation() override
    { return "Değişken Dokümantasyonu"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    DString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Veri Yapıları";
      }
      else
      {
        return "Sınıflar";
      }
    }

    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    DString trGeneratedAt(const DString &date,const DString &projName) override
    {
      DString result=projName+" için "+date;
      if (!projName.empty()) result+=" tarihinde ";
      result+=" üreten: ";
      return result;
    }

    /*! this text is put before a class diagram */
    DString trClassDiagram(const DString &clName) override
    {
      return clName+" için kalıtım şeması:";
    }

    /*! this text is generated when the \\warning command is used. */
    DString trWarning() override
    { return "Uyarı"; }

    /*! this text is generated when the \\version command is used. */
    DString trVersion() override
    { return "Sürüm"; }

    /*! this text is generated when the \\date command is used. */
    DString trDate() override
    { return "Tarih"; }

    /*! this text is generated when the \\return command is used. */
    DString trReturns() override
    { return "Döndürdüğü değer"; }

    /*! this text is generated when the \\sa command is used. */
    DString trSeeAlso() override
    { return "Ayrıca Bakınız"; }

    /*! this text is generated when the \\param command is used. */
    DString trParameters() override
    { return "Parametreler"; }

    /*! this text is generated when the \\exception command is used. */
    DString trExceptions() override
    { return "İstisnalar"; }

    /*! this text is used in the title page of a LaTeX document. */
    DString trGeneratedBy() override
    { return "Oluşturan"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    DString trNamespaceList() override
    { return "Namespace Listesi"; }

    /*! used as an introduction to the namespace list */
    DString trNamespaceListDescription(bool extractAll) override
    {
      DString result="Tüm ";
      if (!extractAll) result+="dokümante edilmiş ";
      result+="namespace\'lerin kısa tanımlarıyla listesi aşağıdadır:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    DString trFriends() override
    { return "Arkadaşları"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    DString trRelatedFunctionDocumentation() override
    { return "Arkadaş Ve İlgili Fonksiyon Dokümantasyonu"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    DString trCompoundReference(const DString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      DString result=clName;
      switch(compType)
      {
        case ClassDef::Class:      result+=" Sınıf"; break;
        case ClassDef::Struct:     result+=" Yapı(Struct)"; break;
        case ClassDef::Union:      result+=" Birleşim(Union)"; break;
        case ClassDef::Interface:  result+=" Arayüz(Interface)"; break;
        case ClassDef::Protocol:   result+=" Protokol"; break;
        case ClassDef::Category:   result+=" Kategori"; break;
        case ClassDef::Exception:  result+=" İstisna"; break;
        default: break;
      }
      if (isTemplate) result+=" Şablon";
      result+=" Referans";
      return result;
    }

    /*! used as the title of the HTML page of a file */
    DString trFileReference(const DString &fileName) override
    {
      DString result=fileName;
      result+=" Dosya Referansı";
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    DString trNamespaceReference(const DString &namespaceName) override
    {
      DString result=namespaceName;
      result+=" Namespace Referansı";
      return result;
    }

    DString trPublicMembers() override
    { return "Public Üye Fonksiyonlar"; }
    DString trPublicSlots() override
    { return "Public Slotlar"; }
    DString trSignals() override
    { return "Sinyaller"; }
    DString trStaticPublicMembers() override
    { return "Statik Public Üye Fonksiyonlar"; }
    DString trProtectedMembers() override
    { return "Korumalı Üye Fonksiyonlar"; }
    DString trProtectedSlots() override
    { return "Korumalı Slotlar"; }
    DString trStaticProtectedMembers() override
    { return "Statik Korumalı Üye Fonksiyonlar"; }
    DString trPrivateMembers() override
    { return "Özel Üye Fonksiyonlar"; }
    DString trPrivateSlots() override
    { return "Özel Slotlar"; }
    DString trStaticPrivateMembers() override
    { return "Statik Özel Üye Fonksiyonlar"; }

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
            result+=" ve ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    DString trInheritsList(int numEntries) override
    {
      return "Şu sınıflardan türemiştir : "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    DString trInheritedByList(int numEntries) override
    {
      return "Şu sınıfların atasıdır : "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    DString trReimplementedFromList(int numEntries) override
    {
      return "Şu üyeleri yeniden tanımlar : "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    DString trReimplementedInList(int numEntries) override
    {
      return "Şu üyeler tarafından yeniden tanımlanmıştır : "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    DString trNamespaceMembers() override
    { return "Namespace Üyeleri"; }

    /*! This is an introduction to the page with all namespace members */
    DString trNamespaceMemberDescription(bool extractAll) override
    {
      DString result="Bu liste tüm ";
      if (!extractAll) result+="dokümante edilmiş ";
      result+="namespace üyelerini listeler ve ";
      if (extractAll)
        result+="her üye için üye dokümantasyonuna bağlar:";
      else
        result+="ait olduğu namespace\'lere bağlar:";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    DString trNamespaceIndex() override
    { return "Namespace İndeksi"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    DString trNamespaceDocumentation() override
    { return "Namespace Dokümantasyonu"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    DString trNamespaces() override
    { return "Namespace\'ler"; }

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
      DString result="Bu ";
      switch(compType)
      {
        case ClassDef::Class:      result+="sınıf"; break;
        case ClassDef::Struct:     result+="yapı(struct)"; break;
        case ClassDef::Union:      result+="birleşim(union)"; break;
        case ClassDef::Interface:  result+="arayüz(interface)"; break;
        case ClassDef::Protocol:   result+="protokol"; break;
        case ClassDef::Category:   result+="kategori"; break;
        case ClassDef::Exception:  result+="istisna"; break;
        default: break;
      }
      result+=" için dokümantasyon aşağıdaki dosya";
      if (single) result+=""; else result+="lar";
      result+="dan üretilmiştir:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    DString trReturnValues() override
    { return "Döndürdüğü değerler"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    DString trMainPage() override
    { return "Ana Sayfa"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    DString trPageAbbreviation() override
    { return "sf."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    DString trDefinedAtLineInSourceFile() override
    {
      return "@1 dosyasının @0 numaralı satırında tanımlanmıştır.";
    }
    DString trDefinedInSourceFile() override
    {
      return "@0 dosyasında tanımlanmıştır.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    DString trDeprecated() override
    {
      return "Kullanımdan kalkmıştır";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    DString trCollaborationDiagram(const DString &clName) override
    {
      return clName+" için işbirliği (collaboration) şeması:";
    }
    /*! this text is put before an include dependency graph */
    DString trInclDepGraph(const DString &fName) override
    {
      return fName+" için içerme bağımlılık grafiği:";
    }
    /*! header that is put before the list of constructor/destructors. */
    DString trConstructorDocumentation() override
    {
      return "Constructor & Destructor Dokümantasyonu";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    DString trGotoSourceCode() override
    {
      return "Bu dosyanın kaynak koduna git.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    DString trGotoDocumentation() override
    {
      return "Bu dosyanın dokümantasyonuna git.";
    }
    /*! Text for the \\pre command */
    DString trPrecondition() override
    {
      return "Ön şart";
    }
    /*! Text for the \\post command */
    DString trPostcondition() override
    {
      return "Son şart (Postcondition)";
    }
    /*! Text for the \\invariant command */
    DString trInvariant() override
    {
      return "Değişmez(Invariant)";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    DString trInitialValue() override
    {
      return "İlk değer:";
    }
    /*! Text used the source code in the file index */
    DString trCode() override
    {
      return "kod";
    }
    DString trGraphicalHierarchy() override
    {
      return "Grafiksel Sınıf Hiyerarşisi";
    }
    DString trGotoGraphicalHierarchy() override
    {
      return "Grafiksel sınıf hiyerarşisine git";
    }
    DString trGotoTextualHierarchy() override
    {
      return "Metinsel sınıf hiyerarşisine git";
    }
    DString trPageIndex() override
    {
      return "Sayfa İndeksi";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    DString trNote() override
    {
      return "Not";
    }
    DString trPublicTypes() override
    {
      return "Public Tipler";
    }
    DString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Veri Alanları";
      }
      else
      {
        return "Public Özellikler(attribute)";
      }
    }
    DString trStaticPublicAttribs() override
    {
      return "Statik Public Özellikler(attribute)";
    }
    DString trProtectedTypes() override
    {
      return "Korumalı Tipler";
    }
    DString trProtectedAttribs() override
    {
      return "Korumalı Özellikler(attribute)";
    }
    DString trStaticProtectedAttribs() override
    {
      return "Statik Korumalı Attributes";
    }
    DString trPrivateTypes() override
    {
      return "Özel Tipler";
    }
    DString trPrivateAttribs() override
    {
      return "Özel Özellikler(attribute)";
    }
    DString trStaticPrivateAttribs() override
    {
      return "Statik Özel Özellikler(attribute)";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    DString trTodo() override
    {
      return "Yapılacak";
    }
    /*! Used as the header of the todo list */
    DString trTodoList() override
    {
      return "Yapılacaklar Listesi";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    DString trReferencedBy() override
    {
      return "Referans veren";
    }
    DString trRemarks() override
    {
      return "Dipnotlar";
    }
    DString trAttention() override
    {
      return "Dikkat";
    }
    DString trInclByDepGraph() override
    {
      return "Bu şekil hangi dosyaların doğrudan ya da "
             "dolaylı olarak bu dosyayı içerdiğini gösterir:";
    }
    DString trSince() override
    {
      return "Şu zamandan beri";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    DString trLegendTitle() override
    {
      return "Şekil Lejantı";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    DString trLegendDocs() override
    {
      return
        "Bu dosya doxygen tarafından üretilen grafiklerin nasıl anlaşılacağını "
        "açıklar.<p>\n"
        "Aşağıdaki örneğe bakalım:\n"
        "\\code\n"
        "/*! Sadeleşme yüzünden görünmeyen sınıf */\n"
        "class Invisible { };\n\n"
        "/*! Sadeleşmiş sınıf, kalıtım ilişkisi gizli */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Doxygen yorumlarıyla dokümante edilmemiş sınıf */\n"
        "class Undocumented { };\n\n"
        "/*! Public kalıtımla türetilen sınıf */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! Bir şablon sınıfı */\n"
        "template<class T> class Templ { };\n\n"
        "/*! Korumalı kalıtımla türetilen sınıf */\n"
        "class ProtectedBase { };\n\n"
        "/*! Özel kalıtımla türetilen sınıf */\n"
        "class PrivateBase { };\n\n"
        "/*! Türetilen sınıf tarafından kullanılan sınıf */\n"
        "class Used { };\n\n"
        "/*! Pekçok sınıftan türeyen bir sınıf */\n"
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
        "Bu kod aşağıdaki şemayı oluşturur:"
        "<p><center><img alt=\"\" src=\"graph_legend."+getDotImageExtension()+"\"></center>\n"
        "<p>\n"
        "Yukarıdaki şemadaki kutular aşağıda açıklanmıştır:\n"
        "<ul>\n"
        "<li>Dolu gri kutu şemanın ait olduğu yapı(struct) ya da sınıfı "
        "gösterir.\n"
        "<li>Siyah çerçeveli bir kutu dokümante edilmiş bir yapı(struct) ya da sınıfı gösterir.\n"
        "<li>Gri çerçeveli bir kutu dokümante edilmemiş bir yapı(struct) ya da sınıfı gösterir.\n"
        "<li>Kırmızı çerçeveli bir kutu tüm kalıtım/içerme ilişkileri gösterilmemiş ve dokümante "
        "edilmiş bir yapı(struct) ya da sınıfı gösterir. %A şema belirlenen sınırlara "
        "sığmıyorsa sadeleştirilir.\n"
        "</ul>\n"
        "Okların anlamı aşağıdaki gibidir:\n"
        "<ul>\n"
        "<li>Koyu mavi ok iki sınıf arasındaki public kalıtım ilişkisini "
        "göstermekte kullanılır.\n"
        "<li>Koyu yeşil ok korumalı kalıtımı gösterir.\n"
        "<li>Koyu kırmızı ok özel kalıtımı gösterir.\n"
        "<li>Mor kesikli çizgi bir sınıfın diğeri tarafından içeriliyor ya da kullanılıyor "
        "olduğunu gösterir. Ok işaret edilen sınıfın hangi değişken(ler) tarafından erişildiğini "
        "gösteren etiketle işaretleniştir.\n"
        "<li>Sarı kesikli çizgi şablondan üretilen bir sınıf ve ilgili şablon sınıfı "
        "arasındaki ilişkiyi gösterir. Ok türeyen sınıfın şablon parametreleriyle "
        "etiketlenmiştir.\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    DString trLegend() override
    {
      return "lejant";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    DString trTest() override
    {
      return "Test";
    }
    /*! Used as the header of the test list */
    DString trTestList() override
    {
      return "Test Listesi";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    DString trProperties() override
    {
      return "Özellikler";
    }
    /*! Used as a section header for IDL property documentation */
    DString trPropertyDocumentation() override
    {
      return "Özellik Dokümantasyonu";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    DString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Veri Yapıları";
      }
      else
      {
        return "Sınıflar";
      }
    }
    /*! Used as the title of a Java package */
    DString trPackage(const DString &name) override
    {
      return "Paket "+name;
    }
    /*! The description of the package index page */
    DString trPackageListDescription() override
    {
      return "Paketler ve kısa tanımları (var ise):";
    }
    /*! The link name in the Quick links header for each page */
    DString trPackages() override
    {
      return "Paketler";
    }
    /*! Text shown before a multi-line define */
    DString trDefineValue() override
    {
      return "Değer:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    DString trBug() override
    {
      return "Hata";
    }
    /*! Used as the header of the bug list */
    DString trBugList() override
    {
      return "Hata Listesi";
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
      return "1254";
    }


    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    DString trRTFCharSet() override
    {
      return "162";
    }

    /*! Used as header RTF general index */
    DString trRTFGeneralIndex() override
    {
      return "İndeks";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trClass(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "sınıf", "lar");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trFile(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "dosya", "lar");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trNamespace(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "namespace", "\'ler");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trGroup(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "grup", "lar");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trPage(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "sayfa", "lar");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trMember(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "üye", "ler");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trGlobal(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "global değişken", "ler");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    DString trAuthor(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "yazar", "lar");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    DString trReferences() override
    {
      return "Referanslar";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    DString trImplementedFromList(int numEntries) override
    {
      return "Şu üyeleri gerçekler: "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    DString trImplementedInList(int numEntries) override
    {
      return "Şu üyelerce gerçeklenir: "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    DString trRTFTableOfContents() override
    {
      return "İçindekiler";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    DString trDeprecatedList() override
    {
      return "Kullanımdan Kalkanlar Listesl";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    DString trEvents() override
    {
      return "Olaylar";
    }
    /*! Header used for the documentation section of a class' events. */
    DString trEventDocumentation() override
    {
      return "Olay Dokümantasyonu";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    DString trPackageTypes() override
    {
      return "Paket Tipleri";
    }
    /*! Used as a heading for a list of Java class fonksiyonlar with package
     * scope.
     */
    DString trPackageFunctions() override
    {
      return "Paket Fonksiyonlar";
    }
    DString trPackageMembers() override
    {
      return "Paket Üyeler";
    }
    /*! Used as a heading for a list of static Java class fonksiyonlar with
     *  package scope.
     */
    DString trStaticPackageFunctions() override
    {
      return "Static Pakat Fonksiyonları";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    DString trPackageAttribs() override
    {
      return "Packet Özellikleri(attribute)";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    DString trStaticPackageAttribs() override
    {
      return "Statik Packet Özellikleri(attribute)";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    DString trAll() override
    {
      return "Tümü";
    }
    /*! Put in front of the call graph for a function. */
    DString trCallGraph() override
    {
      return "Bu fonksiyon için çağırılma şeması:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    DString trSearchResultsTitle() override
    {
      return "Arama sonuçları";
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
        return "Üzgünüm, sorgunuzla eşleşen doküman bulunamadı.";
      }
      else if (numDocuments==1)
      {
        return "Sorgunuzla eşleşen <b>1</b> doküman bulundu.";
      }
      else
      {
        return "Sorgunuzla eşleşen <b>$num</b> doküman bulundu. "
               "Önce en iyi eşleşmeler gösteriliyor.";
      }
    }
    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    DString trSearchMatches() override
    {
      return "Eşleşme:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    DString trSourceFile(const DString& filename) override
    {
      return filename + " Kaynak Dosyası";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    DString trDirIndex() override
    { return "Dizin Hiyerarşisi"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    DString trDirDocumentation() override
    { return "Dizin Dokümantasyonu"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    DString trDirectories() override
    { return "Dizinler"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    DString trDirReference(const DString &dirName) override
    { DString result=dirName; result+=" Dizin Referansı"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in singular or plural form (\a singular).
     */
    DString trDir(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "dizin", "ler");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    DString trOverloadText() override
    {
       return "Bu çok anlamlı (overloaded) bir üye fonksiyondur, "
              "tamamlık açısından burada verilmektedir. Yukarıdaki fonksiyondan "
              "sadece aldığı argüman(lar) açısından farklıdır.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    DString trCallerGraph() override
    {
      return "Bu fonksiyon için çağırılma şeması:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    DString trEnumerationValueDocumentation() override
    { return "Enümerasyon Dokümantasyonu"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    DString trMemberFunctionDocumentationFortran() override
    { return "Üye Fonksiyon/Subroutine Dokümantasyonu"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    DString trCompoundListFortran() override
    { return "Veri Tipleri Listesi"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    DString trCompoundMembersFortran() override
    { return "Veri Alanları"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    DString trCompoundListDescriptionFortran() override
    { return "Kısa tanımlarıyla veri tipleri:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    DString trCompoundMembersDescriptionFortran(bool extractAll) override
    {
      DString result="Bu liste tüm ";
      if (!extractAll)
      {
        result+="dokümante edilmiş ";
      }
      result+="veri tiplerini içerir ve ";
      if (!extractAll)
      {
         result+="her üyeyi veri yapısı dokümantasyonuna bağlar:";
      }
      else
      {
         result+="ait oldukları veri tiplerine bağlar:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index (Fortran).
     */
    DString trCompoundIndexFortran() override
    { return "Veri Tipi İndeksi"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    DString trTypeDocumentation() override
    { return "Veri Tipi Dokümantasyonu"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    DString trSubprograms() override
    { return "Fonksiyonlar/Subroutine\'ler"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    DString trSubprogramDocumentation() override
    { return "Fonksiyon/Subroutine Dokümantasyonu"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     DString trDataTypes() override
    { return "Veri Tipleri"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    DString trModulesList() override
    { return "Modüller Listesi"; }

    /*! used as an introduction to the modules list (Fortran) */
    DString trModulesListDescription(bool extractAll) override
    {
      DString result="Bu liste tüm ";
      if (!extractAll) result+="dokümante edilmiş ";
      result+="modülleri kısa tanımlarıya içerir:";
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
        case ClassDef::Class:      result+=" Modül"; break;
        case ClassDef::Struct:     result+=" Tip"; break;
        case ClassDef::Union:      result+=" Birleşim(Union)"; break;
        case ClassDef::Interface:  result+=" Arayüz"; break;
        case ClassDef::Protocol:   result+=" Protokol"; break;
        case ClassDef::Category:   result+=" Kategori"; break;
        case ClassDef::Exception:  result+=" İstisna"; break;
        default: break;
      }
      if (isTemplate) result+=" Şablon";
      result+=" Referans";
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    DString trModuleReference(const DString &namespaceName) override
    {
      DString result=namespaceName;
      result+=" Modül Referansı";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    DString trModulesMembers() override
    { return "Modül Üyeleri"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    DString trModulesMemberDescription(bool extractAll) override
    {
      DString result="Bu liste tüm ";
      if (!extractAll) result+="dokümante edilmiş ";
      result+="modül üyelerini içerir ve ";
      if (extractAll)
      {
        result+="her üyeyi modül dokümantasyonuna bağlar:";
      }
      else
      {
        result+="ait oldukları modüllere bağlar:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all modules (Fortran).
     */
    DString trModulesIndex() override
    { return "Modüller Indeksi"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trModule(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "modül", "ler");
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    DString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      DString result="Bu ";
      switch(compType)
      {
        case ClassDef::Class:      result+="modül"; break;
        case ClassDef::Struct:     result+="tip"; break;
        case ClassDef::Union:      result+="birleşim(union)"; break;
        case ClassDef::Interface:  result+="arayüz"; break;
        case ClassDef::Protocol:   result+="protokol"; break;
        case ClassDef::Category:   result+="kategori"; break;
        case ClassDef::Exception:  result+="istisna"; break;
        default: break;
      }
      result+=" için dokümantasyon aşağıdaki dosya";
      if (single) result+=":"; else result+="lar";
      result="dan üretilmiştir:";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trType(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "tip", "ler");
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trSubprogram(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "alt program", "lar");
    }

    /*! C# Type Constraint list */
    DString trTypeConstraints() override
    {
      return "Tip Sınırlamaları";
    }


  //////////////////////////////////////////////////////////////////////////
  // new since 1.6.0 (mainly for the new search engine)
  //////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    DString trDirRelation(const DString &name) override
    {
    return name+" İlişkisi";
    }

    /*! Loading message shown when loading search results */
    DString trLoading() override
    {
    return "Yüklüyor...";
    }

    /*! Label used for search results in the global namespace */
    DString trGlobalNamespace() override
    {
    return "En Üst Seviye";
    }

    /*! Message shown while searching */
    DString trSearching() override
    {
    return "Arıyor...";
    }

    /*! Text shown when no search results are found */
    DString trNoMatches() override
    {
    return "Eşleşme Yok";
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
    return name+" dizinindeki dosya";
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    DString trIncludesFileIn(const DString &name) override
    {
    return name + " dizinindeki dosyayı kapsıyor";
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
    static const char *days[]   = { "Pzt","Sal","Çar","Per","Cma","Cmt","Pzr" };
    static const char *months[] = { "Oca","Şub","Mar","Nis","May","Haz","Tem","Ağu","Eyl","Ekm","Kas","Ara" };
    DString sdate;
    if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Date)
    {
    sdate.sprintf("%s %s %d %d",days[dayOfWeek-1],months[month-1],day,year);
    }
    if (includeTime == DateTimeType::DateTime) sdate += " ";
    if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Time)
    {
      DString stime;
      stime.sprintf("%.2d:%.2d:%.2d",hour,minutes,seconds);
      sdate+=stime;
    }
    return sdate;
    }
    DString trDayOfWeek(int dayOfWeek, bool, bool full) override
    {
      static const char *days_short[]   = { "Pzt", "Sal", "Çar", "Per", "Cum", "Cmt", "Paz" };
      static const char *days_full[]    = { "Pazartesi", "Salı", "Çarşamba", "Perşembe", "Cuma", "Cumartesi", "Pazar" };
      DString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      return text;
    }
    DString trMonth(int month, bool, bool full) override
    {
      static const char *months_short[] = { "Oca", "Şub", "Mar", "Nis", "May", "Haz", "Tem", "Ağu", "Eyl", "Eki", "Kas", "Ara" };
      static const char *months_full[]  = { "Ocak", "Şubat", "Mart", "Nisan", "Mayıs", "Haziran", "Temmuz", "Ağustos", "Eylül", "Ekim", "Kasım", "Aralık" };
      DString text  = full? months_full[month-1] : months_short[month-1];
      return text;
    }
    DString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "ÖÖ", "ÖS" };
      return dayPeriod[period?1:0];
    }

};

#endif
