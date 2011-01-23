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

#ifndef TRANSLATOR_TR_H
#define TRANSLATOR_TR_H

/*! 
 When defining a translator class for the new language, follow
 the description in the documentation.  One of the steps says
 that you should copy the translator_en.h (this) file to your
 translator_xx.h new file.  Your new language should use the
 Translator class as the base class.  This means that you need to
 implement exactly the same (pure virtual) methods as the
 TranslatorEnglish does.  Because of this, it is a good idea to
 start with the copy of TranslatorEnglish and replace the strings
 one by one.

 It is not necessary to include "translator.h" or
 "translator_adapter.h" here.  The files are included in the
 language.cpp correctly.  Not including any of the mentioned
 files frees the maintainer from thinking about whether the
 first, the second, or both files should be included or not, and
 why.  This holds namely for localized translators because their
 base class is changed occasionaly to adapter classes when the
 Translator class changes the interface, or back to the
 Translator class (by the local maintainer) when the localized
 translator is made up-to-date again.
*/
class TranslatorTurkish : public Translator
{
  public:

    // --- Language control methods -------------------
    
    /*! Used for identification of the language. The identification 
     * should not be translated. It should be replaced by the name 
     * of the language in English using lower-case characters only
     * (e.g. "czech", "japanese", "russian", etc.). It should be equal to 
     * the identification used in language.cpp.
     */
    virtual QCString idLanguage()
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

    /*! used in the compound documentation before a list of related fonksiyonlar. */
    virtual QCString trRelatedFunctions()
    { return "İlgili Fonksiyonlar"; }

    /*! subscript for the related fonksiyonlar. */
    virtual QCString trRelatedSubscript()
    { return "(Not: Bu fonksiyonlar üye fonksiyon değildir.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return "Ayrıntılı tanımlama"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return "Üye Typedef Dokümantasyonu"; }
    
    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return "Üye Enümerasyon Dokümantasyonu"; }
    
    /*! header that is put before the list of member fonksiyonlar. */
    virtual QCString trMemberFunctionDocumentation()
    { return "Üye Fonksiyon Dokümantasyonu"; }
    
    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Alan Dokümantasyonu"; 
      }
      else
      {
        return "Üye Veri Dokümantasyonu"; 
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore() 
    { return "Ayrıntılar..."; }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers()
    { return "Tüm üyelerin listesi."; }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList()
    { return "Üye Listesi"; }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers()
    { return "Şu sınıfın tüm üyelerinin listesidir: "; }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers()
    { return "  (kalıtımla gelen üyeleri de içerir)."; }
    
    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const char *s)
    { QCString result="Doxygen tarafından";
      if (s) result+=s+(QCString)" için ";
      result+=" kaynak koddan otomatik üretilmiştir."; 
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return "enum adı"; }
    
    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return "enum değeri"; }
    
    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return "tanımlandığı yer"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of 
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules()
    { return "Modüller"; }
    
    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return "Sınıf Hiyerarşisi"; }
    
    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Veri Yapıları";
      }
      else
      {
        return "Sınıf Listesi"; 
      }
    }
    
    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return "Dosya Listesi"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Veri Alanları"; 
      }
      else
      {
        return "Sınıf Üyeleri"; 
      }
    }

    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Globaller"; 
      }
      else
      {
        return "Dosya Üyeleri"; 
      }
    }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages()
    { return "İlgili Sayfalar"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples()
    { return "Örnekler"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch()
    { return "Ara"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription()
    { return "Bu kalıtım listesi tamamen olmasa da yaklaşık "
             "olarak alfabetik sıraya konulmuştur:";
    }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result="Bu liste tüm ";
      if (!extractAll) result+="dokümante edilmiş ";
      result+="dosyaları kısa açıklamalarıyla göstermektedir:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
    { 
      
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="Bu liste tüm ";
      if (!extractAll)
      {
        result+="dokümante edilmiş ";
      }
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Bu liste tüm ";
      if (!extractAll) result+="dokümante edilmiş ";
      
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trExamplesDescription()
    { return "Tüm örneklerin listesi aşağıdadır:"; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return "İlgili tüm dokümantasyon sayfalarının listesi aşağıdadır:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return "Tüm modüllerin listesi aşağıdadır:"; }

    // index titles (the project name is prepended for these) 

    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation()
    { return "Dokümantasyonu"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * index of all groups.
     */
    virtual QCString trModuleIndex()
    { return "Modül İndeksi"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex()
    { return "Hiyerarşik İndeksi"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index.
     */
    virtual QCString trCompoundIndex()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trFileIndex() 
    { return "Dosya İndeksi"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation()
    { return "Modül Dokümantasyonu"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Veri Yapıları Dokümantasyonu"; 
      }
      else
      {
        return "Sınıf Dokümantasyonu"; 
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation()
    { return "Dosya Dokümantasyonu"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    virtual QCString trExampleDocumentation()
    { return "Örnek Dokümantasyonu"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    virtual QCString trPageDocumentation()
    { return "Sayfa Dokümantasyonu"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual()
    { return "Referans Kitabı"; }
    
    /*! This is used in the documentation of a file as a header before the 
     *  list of defines
     */
    virtual QCString trDefines()
    { return "Makrolar"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of function prototypes
     */
    virtual QCString trFuncProtos()
    { return "Fonksiyon Prototipleri"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of typedefs
     */
    virtual QCString trTypedefs()
    { return "Typedef\'ler"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of enumerations
     */
    virtual QCString trEnumerations()
    { return "Enümerasyonlar"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) functions
     */
    virtual QCString trFunctions()
    { return "Fonksiyonlar"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trVariables()
    { return "Değişkenler"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues()
    { return "Enümeratör"; }
    
    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return "Makro Dokümantasyonu"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for function prototypes
     */
    virtual QCString trFunctionPrototypeDocumentation()
    { return "Fonksiyon Prototip Dokümantasyonu"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation()
    { return "Typedef Dokümantasyonu"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation()
    { return "Enümerasyon Tipi Dokümantasyonu"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation()
    { return "Fonksiyon Dokümantasyonu"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation()
    { return "Değişken Dokümantasyonu"; }

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trGeneratedAt(const char *date,const char *projName)
    { 
      QCString result=projName+(QCString)" için "+date;
      if (projName) result+=(QCString)" tarihinde ";
      result+=(QCString)" üreten: ";
      return result;
    }
    /*! This is part of the sentence used in the standard footer of each page.
     */
    virtual QCString trWrittenBy()
    {
      return "yazan";
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const char *clName)
    {
      return clName+(QCString)" için kalıtım şeması:";
    }
    
    /*! this text is generated when the \\internal command is used. */
    virtual QCString trForInternalUseOnly()
    { return "İç kullanıma ayrılmıştır."; }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning()
    { return "Uyarı"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion()
    { return "Sürüm"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate()
    { return "Tarih"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns()
    { return "Döndürdüğü değer"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso()
    { return "Ayrıca Bakınız"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters()
    { return "Parametreler"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions()
    { return "İstisnalar"; }
    
    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy()
    { return "Oluşturan"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////
    
    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return "Namespace Listesi"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Tüm ";
      if (!extractAll) result+="dokümante edilmiş ";
      result+="namespace\'lerin kısa tanımlarıyla listesi aşağıdadır:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends()
    { return "Arkadaşları"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
    /*! used in the class documentation as a header before the list of all
     * related classes 
     */
    virtual QCString trRelatedFunctionDocumentation()
    { return "Arkadaş Ve İlgili Fonksiyon Dokümantasyonu"; }
    
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
        case ClassDef::Class:      result+=" Sınıf"; break;
        case ClassDef::Struct:     result+=" Yapı(Struct)"; break;
        case ClassDef::Union:      result+=" Birleşim(Union)"; break;
        case ClassDef::Interface:  result+=" Arayüz(Interface)"; break;
        case ClassDef::Protocol:   result+=" Protokol"; break;
        case ClassDef::Category:   result+=" Kategori"; break;
        case ClassDef::Exception:  result+=" İstisna"; break;
      }
      if (isTemplate) result+=" Şablon";
      result+=" Referans";
      return result;
    }

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const char *fileName)
    {
      QCString result=fileName;
      result+=" Dosya Referansı"; 
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" Namespace Referansı";
      return result;
    }
    
    virtual QCString trPublicMembers()
    { return "Public Üye Fonksiyonlar"; }
    virtual QCString trPublicSlots()
    { return "Public Slotlar"; }
    virtual QCString trSignals()
    { return "Sinyaller"; }
    virtual QCString trStaticPublicMembers()
    { return "Statik Public Üye Fonksiyonlar"; }
    virtual QCString trProtectedMembers()
    { return "Korumalı Üye Fonksiyonlar"; }
    virtual QCString trProtectedSlots()
    { return "Korumalı Slotlar"; }
    virtual QCString trStaticProtectedMembers()
    { return "Statik Korumalı Üye Fonksiyonlar"; }
    virtual QCString trPrivateMembers()
    { return "Özel Üye Fonksiyonlar"; }
    virtual QCString trPrivateSlots()
    { return "Özel Slotlar"; }
    virtual QCString trStaticPrivateMembers()
    { return "Statik Özel Üye Fonksiyonlar"; }
    
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
            result+=" ve ";
        }
      }
      return result; 
    }
    
    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries)
    {
      return "Şu sınıflardan türemiştir : "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries)
    {
      return "Şu sınıfların atasıdır : "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of 
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries)
    {
      return "Şu üyeleri yeniden tanımlar : "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries)
    {
      return "Şu üyeler tarafından yeniden tanımlanmıştır : "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers()
    { return "Namespace Üyeleri"; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
    { 
      QCString result="Bu liste tüm ";
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
    virtual QCString trNamespaceIndex()
    { return "Namespace İndeksi"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
    { return "Namespace Dokümantasyonu"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return "Namespace\'ler"; }

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
      QCString result=(QCString)"Bu ";
      switch(compType)
      {
        case ClassDef::Class:      result+="sınıf"; break;
        case ClassDef::Struct:     result+="yapı(struct)"; break;
        case ClassDef::Union:      result+="birleşim(union)"; break;
        case ClassDef::Interface:  result+="arayüz(interface)"; break;
        case ClassDef::Protocol:   result+="protokol"; break;
        case ClassDef::Category:   result+="kategori"; break;
        case ClassDef::Exception:  result+="istisna"; break;
      }
      result+=" için dokümantasyon aşağıdaki dosya";
      if (single) result+=""; else result+="lar";
      result+="dan üretilmiştir:";
      return result;
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    virtual QCString trAlphabeticalList()
    { return "Alfabetik Liste"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues()
    { return "Döndürdüğü değerler"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return "Ana Sayfa"; }

    /*! This is used in references to page that are put in the LaTeX 
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation()
    { return "sf."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDefinedAtLineInSourceFile()
    {
      return "@1 dosyasının @0 numaralı satırında tanımlanmıştır.";
    }
    virtual QCString trDefinedInSourceFile()
    {
      return "@0 dosyasında tanımlanmıştır.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated()
    {
      return "Kullanımdan kalkmıştır";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const char *clName)
    {
      return clName+(QCString)" için işbirliği (collaboration) şeması:";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName)
    {
      return fName+(QCString)" için içerme bağımlılık grafiği:";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return "Constructor & Destructor Dokümantasyonu"; 
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    {
      return "Bu dosyanın kaynak koduna git.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return "Bu dosyanın dokümantasyonuna git.";
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition()
    {
      return "Ön şart";
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition()
    {
      return "Son şart (Postcondition)";
    }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant()
    {
      return "Değişmez(Invariant)";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue()
    {
      return "İlk değer:";
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode()
    {
      return "kod";
    }
    virtual QCString trGraphicalHierarchy()
    {
      return "Grafiksel Sınıf Hiyerarşisi";
    }
    virtual QCString trGotoGraphicalHierarchy()
    {
      return "Grafiksel sınıf hiyerarşisine git";
    }
    virtual QCString trGotoTextualHierarchy()
    {
      return "Metinsel sınıf hiyerarşisine git";
    }
    virtual QCString trPageIndex()
    {
      return "Sayfa İndeksi";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////
    
    virtual QCString trNote()
    {
      return "Not";
    }
    virtual QCString trPublicTypes()
    {
      return "Public Tipler";
    }
    virtual QCString trPublicAttribs()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Veri Alanları";
      }
      else
      {
        return "Public Özellikler(attribute)";
      }
    }
    virtual QCString trStaticPublicAttribs()
    {
      return "Statik Public Özellikler(attribute)";
    }
    virtual QCString trProtectedTypes()
    {
      return "Korumalı Tipler";
    }
    virtual QCString trProtectedAttribs()
    {
      return "Korumalı Özellikler(attribute)";
    }
    virtual QCString trStaticProtectedAttribs()
    {
      return "Statik Korumalı Attributes";
    }
    virtual QCString trPrivateTypes()
    {
      return "Özel Tipler";
    }
    virtual QCString trPrivateAttribs()
    {
      return "Özel Özellikler(attribute)";
    }
    virtual QCString trStaticPrivateAttribs()
    {
      return "Statik Özel Özellikler(attribute)";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    virtual QCString trTodo()
    {
      return "Yapılacak";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    {
      return "Yapılacaklar Listesi";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
      return "Referans veren";
    }
    virtual QCString trRemarks()
    {
      return "Dipnotlar";
    }
    virtual QCString trAttention()
    {
      return "Dikkat";
    }
    virtual QCString trInclByDepGraph()
    {
      return "Bu şekil hangi dosyaların doğrudan ya da "
             "dolaylı olarak bu dosyayı içerdiğini gösterir:";
    }
    virtual QCString trSince()
    {
      return "Şu zamandan beri";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    {
      return "Şekil Lejantı";
    }
    /*! page explaining how the dot graph's should be interpreted 
     *  The %A in the text below are to prevent link to classes called "A".
     */
    virtual QCString trLegendDocs()
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
        "<p><center><img alt=\"\" src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
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
    virtual QCString trLegend()
    {
      return "lejant";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////
    
    /*! Used as a marker that is put before a test item */
    virtual QCString trTest()
    {
      return "Test";
    }
    /*! Used as the header of the test list */
    virtual QCString trTestList()
    {
      return "Test Listesi";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for KDE-2 IDL methods */
    virtual QCString trDCOPMethods()
    {
      return "DCOP Üye Fonksiyonlar";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties()
    {
      return "Özellikler";
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
      return "Özellik Dokümantasyonu";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Veri Yapıları";
      }
      else
      {
        return "Sınıflar";
      }
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const char *name)
    {
      return (QCString)"Paket "+name;
    }
    /*! Title of the package index page */
    virtual QCString trPackageList()
    {
      return "Paket Listesi";
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return "Paketler ve kısa tanımları (var ise):";
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return "Paketler";
    }
    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue()
    {
      return "Değer:";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////
    
    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug()
    {
      return "Hata";
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList()
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
    virtual QCString trRTFansicp()
    {
      return "1254";
    }
    

    /*! Used as ansicpg for RTF fcharset 
     *  \see trRTFansicp() for a table of possible values.
     */
    virtual QCString trRTFCharSet()
    {
      return "162";
    }

    /*! Used as header RTF general index */
    virtual QCString trRTFGeneralIndex()
    {
      return "İndeks";
    }
   
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Sınıf" : "sınıf"));
      if (!singular)  result+="lar";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trFile(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Dosya" : "dosya"));
      if (!singular)  result+="lar";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Namespace" : "namespace"));
      if (!singular)  result+="\'ler";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Grup" : "grup"));
      if (!singular)  result+="lar";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Sayfa" : "sayfa"));
      if (!singular)  result+="lar";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Üye" : "üye"));
      if (!singular)  result+="ler";
      return result; 
    }
   
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGlobal(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Global değişken" : "global değişken"));
      if (!singular)  result+="ler";
      return result; 
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    virtual QCString trAuthor(bool first_capital, bool singular)
    {                                                                         
      QCString result((first_capital ? "Yazar" : "yazar"));
      if (!singular)  result+="lar";
      return result; 
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences()
    {
      return "Referanslar";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of 
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    {
      return "Şu üyeleri gerçekler: "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      return "Şu üyelerce gerçeklenir: "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      return "İçindekiler";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been 
     *  flagged deprecated 
     */
    virtual QCString trDeprecatedList()
    {
      return "Kullanımdan Kalkanlar Listesl";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in 
     * a C# program
     */
    virtual QCString trEvents()
    {
      return "Olaylar";
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      return "Olay Dokümantasyonu";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    { 
      return "Paket Tipleri";
    }
    /*! Used as a heading for a list of Java class fonksiyonlar with package 
     * scope. 
     */
    virtual QCString trPackageMembers()
    { 
      return "Paket Fonksiyonlar";
    }
    /*! Used as a heading for a list of static Java class fonksiyonlar with 
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    { 
      return "Static Pakat Fonksiyonları";
    }
    /*! Used as a heading for a list of Java class variables with package 
     * scope.
     */
    virtual QCString trPackageAttribs()
    { 
      return "Packet Özellikleri(attribute)";
    }
    /*! Used as a heading for a list of static Java class variables with 
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    { 
      return "Statik Packet Özellikleri(attribute)";
    }
    
//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page 
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll()
    {
      return "Tümü";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph()
    {
      return "Bu fonksiyon için çağırılma şeması:";
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
      return "Arama yapılacak metin";
    }
    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle()
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
    virtual QCString trSearchResults(int numDocuments)
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
    virtual QCString trSearchMatches()
    {
      return "Eşleşme:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename)
    {
      return filename + " Kaynak Dosyası";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex()
    { return "Dizin Hiyerarşisi"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation()
    { return "Dizin Dokümantasyonu"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories()
    { return "Dizinler"; }

    /*! This returns a sentences that introduces the directory hierarchy. 
     *  and the fact that it is sorted alphabetically per level
     */
    virtual QCString trDirDescription()
    { return "Bu dizin hiyerarşisi tamamen olmasa da yaklaşık "
             "olarak alfabetik sıraya konulmuştur:";
    }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const char *dirName)
    { QCString result=dirName; result+=" Dizin Referansı"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Dizin" : "dizin"));
      if (!singular) result+="ler";
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
       return "Bu çok anlamlı (overloaded) bir üye fonksiyondur, "
              "tamamlık açısından burada verilmektedir. Yukarıdaki fonksiyondan "
              "sadece aldığı argüman(lar) açısından farklıdır.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    virtual QCString trCallerGraph()
    {
      return "Bu fonksiyon için çağırılma şeması:";
    }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return "Enümerasyon Dokümantasyonu"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////
    
    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran()
    { return "Üye Fonksiyon/Subroutine Dokümantasyonu"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */    
    virtual QCString trCompoundListFortran()
    { return "Veri Tipleri Listesi"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran()
    { return "Veri Alanları"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran()
    { return "Kısa tanımlarıyla veri tipleri:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll)
    {
      QCString result="Bu liste tüm ";
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
    virtual QCString trCompoundIndexFortran()
    { return "Veri Tipi İndeksi"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation()
    { return "Veri Tipi Dokümantasyonu"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms()
    { return "Fonksiyonlar/Subroutine\'ler"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation()
    { return "Fonksiyon/Subroutine Dokümantasyonu"; }

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes()
    { return "Veri Tipleri"; }
    
    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList()
    { return "Modüller Listesi"; }

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll)
    {
      QCString result="Bu liste tüm ";
      if (!extractAll) result+="dokümante edilmiş ";
      result+="modülleri kısa tanımlarıya içerir:";
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
        case ClassDef::Class:      result+=" Modül"; break;
        case ClassDef::Struct:     result+=" Tip"; break;
        case ClassDef::Union:      result+=" Birleşim(Union)"; break;
        case ClassDef::Interface:  result+=" Arayüz"; break;
        case ClassDef::Protocol:   result+=" Protokol"; break;
        case ClassDef::Category:   result+=" Kategori"; break;
        case ClassDef::Exception:  result+=" İstisna"; break;
      }
      if (isTemplate) result+=" Şablon";
      result+=" Referans";
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    virtual QCString trModuleReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" Modül Referansı";        
      return result;
    }
    
    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers()
    { return "Modül Üyeleri"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll)
    { 
      QCString result="Bu liste tüm ";
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
    virtual QCString trModulesIndex()
    { return "Modüller Indeksi"; }
    
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trModule(bool first_capital, bool singular)
    {       
      QCString result((first_capital ? "Modül" : "modül"));
      if (!singular)  result+="ler";
      return result; 
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single)
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      QCString result=(QCString)"Bu ";
      switch(compType)
      {
        case ClassDef::Class:      result+="mdoül"; break;
        case ClassDef::Struct:     result+="tip"; break;
        case ClassDef::Union:      result+="birleşim(union)"; break;
        case ClassDef::Interface:  result+="arayüz"; break;
        case ClassDef::Protocol:   result+="protokol"; break;
        case ClassDef::Category:   result+="kategori"; break;
        case ClassDef::Exception:  result+="istisna"; break;
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
    virtual QCString trType(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Tip" : "tip"));
      if (!singular)  result+="ler";
      return result; 
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trSubprogram(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Alt program" : "alt program"));
      if (!singular)  result+="lar";
      return result; 
    }

    /*! C# Type Constraint list */
    virtual QCString trTypeConstraints()
    {
      return "Tip Sınırlamaları";
    }
	
	
	//////////////////////////////////////////////////////////////////////////
	// new since 1.6.0 (mainly for the new search engine)
	//////////////////////////////////////////////////////////////////////////
	
    /*! directory relation for \a name */
    virtual QCString trDirRelation(const char *name)
    {
		return QCString(name)+" İlişkisi";
    }
	
    /*! Loading message shown when loading search results */
    virtual QCString trLoading()
    {
		return "Yüklüyor...";
    }
	
    /*! Label used for search results in the global namespace */
    virtual QCString trGlobalNamespace()
    {
		return "En Üst Seviye";
    }
	
    /*! Message shown while searching */
    virtual QCString trSearching()
    {
		return "Arıyor...";
    }
	
    /*! Text shown when no search results are found */
    virtual QCString trNoMatches()
    {
		return "Eşleşme Yok";
    }
	
	//////////////////////////////////////////////////////////////////////////
	// new since 1.6.3 (missing items for the directory pages)
	//////////////////////////////////////////////////////////////////////////
	
    /*! introduction text for the directory dependency graph */
    virtual QCString trDirDependency(const char *name)
    {
		return (QCString)(name) + (QCString)" için dizin bağımlılık grafiği";
    }
	
    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the first column mentions the
     *  source file that has a relation to another file.
     */
    virtual QCString trFileIn(const char *name)
    {
		return (QCString)(name) + (QCString)" dizinindeki dosya";
    }
	
    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    virtual QCString trIncludesFileIn(const char *name)
    {
		return (QCString)(name) + (QCString)" dizinindeki dosyayı kapsıyor";
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
		static const char *days[]   = { "Pzt","Sal","Çar","Per","Cma","Cmt","Pzr" };
		static const char *months[] = { "Oca","Şub","Mar","Nis","May","Haz","Tem","Ağu","Eyl","Ekm","Kas","Ara" };
		QCString sdate;
		sdate.sprintf("%s %s %d %d",days[dayOfWeek-1],months[month-1],day,year);
		if (includeTime)
		{
			QCString stime;
			stime.sprintf(" %.2d:%.2d:%.2d",hour,minutes,seconds);
			sdate+=stime;
		}
		return sdate;
    }
	
	
};

#endif
