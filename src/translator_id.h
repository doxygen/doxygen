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

#ifndef TRANSLATOR_ID_H
#define TRANSLATOR_ID_H

/*! 
  Indonesian translator based on Doxygen 1.4.2.

  \author Hendy Irawan <ceefour@gauldong.net>
 */
class TranslatorIndonesian : public TranslatorAdapter_1_4_6
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
    { return "indonesian"; }

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
      return "\\usepackage[indonesian]{babel}";
    }

    /*! return the language charset. This will be used for the HTML output */
    virtual QCString idLanguageCharset()
    {
      return "iso-8859-1";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions()
    { return "Fungsi-fungsi Terkait"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return "(Catatan: Fungsi-fungsi tersebut bukan fungsi anggota.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return "Keterangan Lengkap"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return "Dokumentasi Anggota: Tipe"; }

    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return "Dokumentasi Anggota: Enumerasi"; }

    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation()
    { return "Dokumentasi Anggota: Fungsi"; }

    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Dokumentasi Variabel"; 
      }
      else
      {
        return "Dokumentasi Anggota: Data"; 
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore() 
    { return "Selengkapnya..."; }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers()
    { return "Daftar semua anggota."; }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList()
    { return "Daftar anggota"; }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers()
    { return "Berikut ini daftar lengkap anggota untuk "; }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers()
    { return ", termasuk semua anggota yang diwariskan."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const char *s)
    { QCString result="Dihasilkan secara otomatis oleh Doxygen";
      if (s) result+=(QCString)" untuk "+s;
      result+=" dari kode sumber."; 
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return "nama enumerasi"; }

    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return "nilai enumerasi"; }

    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return "didefinisikan di"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of 
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules()
    { return "Modul"; }

    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return "Struktur Kelas"; }

    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Struktur Data";
      }
      else
      {
        return "Daftar Kelas"; 
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return "Daftar File"; }

    /*! This is put above each page as a link to the list of all verbatim headers */
    virtual QCString trHeaderFiles()
    { return "File Header"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Variabel Data"; 
      }
      else
      {
        return "Daftar Anggota: Kelas"; 
      }
    }

    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Daftar Global"; 
      }
      else
      {
        return "Daftar File"; 
      }
    }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages()
    { return "Halaman Terkait"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples()
    { return "Contoh"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch()
    { return "Cari"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription()
    { return "Struktur kelas ini diurutkan hampir sepenuhnya diurutkan berdasarkan abjad:";
    }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result="Berikut ini daftar seluruh file dengan penjelasan singkat";
      if (!extractAll) result+=" yang didokumentasikan";
      result += ":";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
    { 

      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Berikut ini daftar struktur data beserta penjelasan singkat:"; 
      }
      else
      {
        return "Berikut ini daftar kelas, struktur, gabungan, dan interface beserta penjelasan singkat:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="Berikut ini daftar seluruh ";
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        result+="variabel struktur dan gabungan";
      }
      else
      {
        result+="kelas";
      }
      if (!extractAll)
      {
        result+=" yang didokumentasikan";
      }
      result+=" dengan link ke ";
      if (!extractAll) 
      {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
          result+="dokumentasi struktur/gabungan untuk tiap variabel:";
        }
        else
        {
          result+="dokumentasi kelas untuk tiap anggota:";
        }
      }
      else 
      {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
        {
          result+="struktur/gabungan induk:";
        }
        else
        {
          result+="kelas induk:";
        }
      }
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    virtual QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Berikut ini daftar ";

      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        result+="fungsi, variabel, konstanta, enumerasi, dan tipe";
      }
      else
      {
        result+="file";
      }
      if (!extractAll) result+=" yang didokumentasikan";
      result+=" dengan link ke ";
      if (extractAll) 
        result+="file induk:";
      else 
        result+="dokumentasi:";
      return result;
    }

    /*! This is an introduction to the page with the list of all header files. */
    virtual QCString trHeaderFilesDescription()
    { return "Berikut ini daftar file header yang melingkupi API (Application Programming Interface) ini:"; }

    /*! This is an introduction to the page with the list of all examples */
    virtual QCString trExamplesDescription()
    { return "Berikut ini daftar semua contoh:"; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return "Berikut ini daftar semua halaman dokumentasi yang terkait:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return "Berikut ini daftar semua modul:"; }

    /*! This sentences is used in the annotated class/file lists if no brief
     * description is given. 
     */
    virtual QCString trNoDescriptionAvailable()
    { return "Tidak ada keterangan tersedia"; }

    // index titles (the project name is prepended for these) 


    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation()
    { return "Dokumentasi"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * index of all groups.
     */
    virtual QCString trModuleIndex()
    { return "Indeks Modul"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex()
    { return "Indeks Struktur Kelas"; }

    /*! This is used in LaTeX as the title of the chapter with the 
     * annotated compound index.
     */
    virtual QCString trCompoundIndex()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      { 
        return "Indeks Struktur Data";
      }
      else
      {
        return "Indeks Kelas"; 
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    virtual QCString trFileIndex() 
    { return "Indeks File"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation()
    { return "Dokumentasi Modul"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Dokumentasi Struktur Data"; 
      }
      else
      {
        return "Dokumentasi Kelas"; 
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation()
    { return "Dokumentasi File"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    virtual QCString trExampleDocumentation()
    { return "Dokumentasi Contoh"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    virtual QCString trPageDocumentation()
    { return "Dokumentasi Halaman"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual()
    { return "Dokumentasi Referensi"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of defines
     */
    virtual QCString trDefines()
    { return "Konstanta"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of function prototypes
     */
    virtual QCString trFuncProtos()
    { return "Prototipe Fungsi"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of typedefs
     */
    virtual QCString trTypedefs()
    { return "Definisi Tipe"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of enumerations
     */
    virtual QCString trEnumerations()
    { return "Enumerasi"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) functions
     */
    virtual QCString trFunctions()
    { return "Fungsi"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trVariables()
    { return "Variabel"; }

    /*! This is used in the documentation of a file as a header before the 
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues()
    { return "Nilai enumerasi"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return "Dokumentasi Konstanta"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for function prototypes
     */
    virtual QCString trFunctionPrototypeDocumentation()
    { return "Dokumentasi Prototipe Fungsi"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation()
    { return "Dokumentasi Definisi Tipe"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation()
    { return "Dokumentasi Tipe Enumerasi"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return "Dokumentasi Nilai Enumerasi"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation()
    { return "Dokumentasi Fungsi"; }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation()
    { return "Dokumentasi Variabel"; }

    /*! This is used in the documentation of a file/namespace/group before 
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds()
    { 
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Struktur Data"; 
      }
      else
      {
        return "Kelas"; 
      }
    }

    /*! This is used in the standard footer of each page and indicates when 
     *  the page was generated 
     */
    virtual QCString trGeneratedAt(const char *date,const char *projName)
    { 
      QCString result=(QCString)"Dihasilkan pada tanggal "+date;
      if (projName) result+=(QCString)" untuk "+projName;
      result+=(QCString)" oleh";
      return result;
    }
    /*! This is part of the sentence used in the standard footer of each page.
     */
    virtual QCString trWrittenBy()
    {
      return "ditulis oleh";
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const char *clName)
    {
      return (QCString)"Diagram struktur kelas untuk "+clName+":";
    }

    /*! this text is generated when the \\internal command is used. */
    virtual QCString trForInternalUseOnly()
    { return "Untuk kalangan sendiri."; }

    /*! this text is generated when the \\reimp command is used. */
    virtual QCString trReimplementedForInternalReasons()
    { return "Di-reimplementasikan karena alasan internal; API tidak berubah."; }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning()
    { return "Peringatan"; }

    /*! this text is generated when the \\bug command is used. */
    virtual QCString trBugsAndLimitations()
    { return "Bug dan keterbatasan"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion()
    { return "Versi"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate()
    { return "Tanggal"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns()
    { return "Mengembalikan"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso()
    { return "Lihat juga"; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters()
    { return "Parameter"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions()
    { return "Exception"; }

    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy()
    { return "Dihasilkan oleh"; }

    //////////////////////////////////////////////////////////////////////////
    // new since 0.49-990307
    //////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return "Daftar Namespace"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Berikut ini daftar namespace beserta keterangan singkat";
      if (!extractAll) result+=" yang didokumentasikan";
      result+=":";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends()
    { return "Friend"; }

    //////////////////////////////////////////////////////////////////////////
    // new since 0.49-990405
    //////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes 
     */
    virtual QCString trRelatedFunctionDocumentation()
    { return "Dokumentasi Friend Dan Fungsi Terkait"; }

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
        case ClassDef::Class:      result+=" Kelas"; break;
        case ClassDef::Struct:     result+=" Struktur"; break;
        case ClassDef::Union:      result+=" Gabungan"; break;
        case ClassDef::Interface:  result+=" Interface"; break;
        case ClassDef::Protocol:   result+=" Protokol"; break;
        case ClassDef::Category:   result+=" Kategori"; break;
        case ClassDef::Exception:  result+=" Exception"; break;
      }
      if (isTemplate) result+=" Template";
      result+=" Referensi";
      return result;
    }

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const char *fileName)
    {
      QCString result=fileName;
      result+=" Referensi File"; 
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" Referensi Namespace";
      return result;
    }

    virtual QCString trPublicMembers()
    { return "Fungsi Anggota Publik"; }
    virtual QCString trPublicSlots()
    { return "Slot Publik"; }
    virtual QCString trSignals()
    { return "Signal"; }
    virtual QCString trStaticPublicMembers()
    { return "Fungsi Anggota Publik Statis"; }
    virtual QCString trProtectedMembers()
    { return "Fungsi Anggota Diproteksi"; }
    virtual QCString trProtectedSlots()
    { return "Slot Diproteksi"; }
    virtual QCString trStaticProtectedMembers()
    { return "Fungsi Anggota Diproteksi Statis"; }
    virtual QCString trPrivateMembers()
    { return "Fungsi Anggota Privat"; }
    virtual QCString trPrivateSlots()
    { return "Slot Privat"; }
    virtual QCString trStaticPrivateMembers()
    { return "Fungsi Anggota Privat Statis"; }

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
            result+=", dan ";
        }
      }
      return result; 
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries)
    {
      return "Mewarisi "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries)
    {
      return "Diwariskan ke "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of 
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries)
    {
      return "Di-reimplementasikan dari "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries)
    {
      return "Di-reimplementasikan di "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers()
    { return "Anggota Namespace"; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
    { 
      QCString result="Berikut ini daftar anggota namespace ";
      if (!extractAll) result+="yang didokumentasikan ";
      result+="dengan link ke ";
      if (extractAll) 
        result+="dokumentasi namespace untuk tiap anggota:";
      else 
        result+="namespace induk:";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the 
     *  index of all namespaces.
     */
    virtual QCString trNamespaceIndex()
    { return "Indeks Namespace"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
    { return "Dokumentasi Namespace"; }

    //////////////////////////////////////////////////////////////////////////
    // new since 0.49-990522
    //////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return "Daftar Namespace"; }

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
      QCString result=(QCString)"Dokumentasi untuk ";
      switch(compType)
      {
        case ClassDef::Class:      result+="kelas"; break;
        case ClassDef::Struct:     result+="struktur"; break;
        case ClassDef::Union:      result+="gabungan"; break;
        case ClassDef::Interface:  result+="interface"; break;
        case ClassDef::Protocol:   result+="protokol"; break;
        case ClassDef::Category:   result+="kategori"; break;
        case ClassDef::Exception:  result+="exception"; break;
      }
      result+=" ini dihasilkan dari file";
      result+=single ? "" : "-file";
      result+=" berikut:";
      return result;
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    virtual QCString trAlphabeticalList()
    { return "Daftar Berdasarkan Abjad"; }

    //////////////////////////////////////////////////////////////////////////
    // new since 0.49-990901
    //////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues()
    { return "Nilai kembali"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return "Halaman Utama"; }

    /*! This is used in references to page that are put in the LaTeX 
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation()
    { return "hal."; }

    //////////////////////////////////////////////////////////////////////////
    // new since 0.49-991003
    //////////////////////////////////////////////////////////////////////////

    virtual QCString trSources()
    {
      return "Sumber";
    }
    virtual QCString trDefinedAtLineInSourceFile()
    {
      return "Definisi pada baris @0 di file @1.";
    }
    virtual QCString trDefinedInSourceFile()
    {
      return "Definisi pada baris @0.";
    }

    //////////////////////////////////////////////////////////////////////////
    // new since 0.49-991205
    //////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated()
    {
      return "Kadaluarsa";
    }

    //////////////////////////////////////////////////////////////////////////
    // new since 1.0.0
    //////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)"Diagram kolaborasi untuk "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName)
    {
      return (QCString)"Diagram ketergantungan untuk "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return "Dokumentasi Konstruktor & Destruktor"; 
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    {
      return "Ke kode sumber file ini.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return "Ke dokumentasi file ini.";
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition()
    {
      return "Kondisi Awal";
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition()
    {
      return "Kondisi Akhir";
    }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant()
    {
      return "Invariant";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue()
    {
      return "Nilai awal:";
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode()
    {
      return "kode";
    }
    virtual QCString trGraphicalHierarchy()
    {
      return "Struktur Kelas Berbasis Grafis";
    }
    virtual QCString trGotoGraphicalHierarchy()
    {
      return "Ke struktur kelas berbasis grafis";
    }
    virtual QCString trGotoTextualHierarchy()
    {
      return "Ke struktur kelas berbasis teks";
    }
    virtual QCString trPageIndex()
    {
      return "Indeks Halaman";
    }

    //////////////////////////////////////////////////////////////////////////
    // new since 1.1.0
    //////////////////////////////////////////////////////////////////////////

    virtual QCString trNote()
    {
      return "Catatan";
    }
    virtual QCString trPublicTypes()
    {
      return "Tipe Publik";
    }
    virtual QCString trPublicAttribs()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Variabel Data";
      }
      else
      {
        return "Atribut Publik";
      }
    }
    virtual QCString trStaticPublicAttribs()
    {
      return "Atribut Publik Statis";
    }
    virtual QCString trProtectedTypes()
    {
      return "Tipe Diproteksi";
    }
    virtual QCString trProtectedAttribs()
    {
      return "Atribut Diproteksi";
    }
    virtual QCString trStaticProtectedAttribs()
    {
      return "Atribut Diproteksi Statis";
    }
    virtual QCString trPrivateTypes()
    {
      return "Tipe Privat";
    }
    virtual QCString trPrivateAttribs()
    {
      return "Atribut Privat";
    }
    virtual QCString trStaticPrivateAttribs()
    {
      return "Atribut Privat Statis";
    }

    //////////////////////////////////////////////////////////////////////////
    // new since 1.1.3
    //////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    virtual QCString trTodo()
    {
      return "Tugas";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    {
      return "Daftar Tugas";
    }

    //////////////////////////////////////////////////////////////////////////
    // new since 1.1.4
    //////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
      return "Direferensikan oleh";
    }
    virtual QCString trRemarks()
    {
      return "Keterangan";
    }
    virtual QCString trAttention()
    {
      return "Perhatian";
    }
    virtual QCString trInclByDepGraph()
    {
      return "Diagram ini menampilkan file-file yang membutuhkan file ini baik secara langsung maupun tidak langsung:";
    }
    virtual QCString trSince()
    {
      return "Sejak";
    }

    //////////////////////////////////////////////////////////////////////////
    // new since 1.1.5
    //////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    {
      return "Legenda Diagram";
    }
    /*! page explaining how the dot graph's should be interpreted 
     *  The %A in the text below are to prevent link to classes called "A".
     */
    virtual QCString trLegendDocs()
    {
      return 
        "Halaman ini berisi keterangan cara membaca diagram yang dihasilkan "
        "oleh doxygen.<p>\n"
        "Contoh:\n"
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
        "                  public Undocumented,\n"
        "                  public Templ<int>\n"
        "{\n"
        "  private:\n"
        "    Used *m_usedClass;\n"
        "};\n"
        "\\endcode\n"
        "Apabila tag \\c MAX_DOT_GRAPH_HEIGHT di file konfigurasi "
        "diset ke 240 kode di atas akan menghasilkan diagram berikut:"
        "<p><center><img alt=\"\" src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
        "<p>\n"
        "Kotak-kotak pada diagram di atas mempunyai arti sebagai berikut:\n"
        "<ul>\n"
        "<li>%Kotak hitam merupakan struktur atau kelas yang bersangkutan.\n"
        "<li>%Kotak berbingkai hitam adalah struktur atau kelas yang mempunyai dokumentasi.\n"
        "<li>%Kotak dengan bingkai abu-abu adalah struktur atau kelas tanpa dokumentasi.\n"
        "<li>%Kotak dengan bingkai merah merupakan struktur atau kelas yang didokumentasikan tetapi"
        "tidak semua relasinya ditampilkan. %Sebuah diagram "
        "akan terpotong apabila lebih besar daripada ukuran yang ditentukan.\n"
        "</ul>\n"
        "Arti dari tanda-tanda panah adalah sebagai berikut:\n"
        "<ul>\n"
        "<li>%Panah biru tua menandakan pewarisan publik.\n"
        "<li>%Panah hijau tua untuk pewarisan diproteksi.\n"
        "<li>%Panah merah tua untuk pewarisan privat.\n"
        "<li>%Panah ungu putus-putus menandakan kelas tersebut berhubungan dengan kelas lain. "
        "Panah tersebut diberi judul sesuai dengan kelas atau struktur tujuan.\n"
        "<li>%Panah kuning putus-putus menandakan hubungan antara sebuah template kelas dan "
        "instance dari template tersebut. Panah tersebut diberi judul sesuai dengan "
        "parameter template yang digunakan.\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    virtual QCString trLegend()
    {
      return "legenda";
    }

    //////////////////////////////////////////////////////////////////////////
    // new since 1.2.0
    //////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    virtual QCString trTest()
    {
      return "Tes";
    }
    /*! Used as the header of the test list */
    virtual QCString trTestList()
    {
      return "Daftar Tes";
    }

    //////////////////////////////////////////////////////////////////////////
    // new since 1.2.1
    //////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for KDE-2 IDL methods */
    virtual QCString trDCOPMethods()
    {
      return "Fungsi Anggota: DCOP";
    }

    //////////////////////////////////////////////////////////////////////////
    // new since 1.2.2
    //////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties()
    {
      return "Daftar Property";
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
      return "Dokumentasi Property";
    }

    //////////////////////////////////////////////////////////////////////////
    // new since 1.2.4
    //////////////////////////////////////////////////////////////////////////

    /*! Used for Java interfaces in the summary section of Java packages */
    virtual QCString trInterfaces()
    {
      return "Daftar Interface";
    }
    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Struktur Data";
      }
      else
      {
        return "Daftar Kelas";
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
      return "Daftar Paket";
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return "Berikut ini daftar paket beserta keterangan singkat (apabila tersedia):";
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return "Daftar Paket";
    }
    /*! Used as a chapter title for Latex & RTF output */
    virtual QCString trPackageDocumentation()
    {
      return "Dokumentasi Paket";
    }
    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue()
    {
      return "Nilai:";
    }

    //////////////////////////////////////////////////////////////////////////
    // new since 1.2.5
    //////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug()
    {
      return "Bug";
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList()
    {
      return "Daftar Bug";
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
      return "1252";
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
      return "Indeks";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Kelas" : "kelas"));
      if (!singular)  result+="-kelas";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trFile(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "File" : "file"));
      if (!singular)  result+="-file";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool)
    { 
      QCString result((first_capital ? "Namespace" : "namespace"));
      //if (!singular)  result+="s";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Kelompok" : "kelompok"));
      if (!singular)  result+="-kelompok";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool )
    { 
      QCString result((first_capital ? "Halaman" : "halaman"));
      //if (!singular)  result+="s";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool )
    { 
      QCString result((first_capital ? "Anggota" : "anggota"));
      //if (!singular)  result+="s";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trField(bool first_capital, bool)
    { 
      QCString result((first_capital ? "Variabel" : "variabel"));
      //if (!singular)  result+="s";
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGlobal(bool first_capital, bool)
    { 
      QCString result((first_capital ? "Global" : "global"));
      //if (!singular)  result+="s";
      return result; 
    }

    //////////////////////////////////////////////////////////////////////////
    // new since 1.2.7
    //////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    virtual QCString trAuthor(bool first_capital, bool)
    {                                                                         
      QCString result((first_capital ? "Penulis" : "penulis"));
      //if (!singular)  result+="s";
      return result; 
    }

    //////////////////////////////////////////////////////////////////////////
    // new since 1.2.11
    //////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences()
    {
      return "Referensi";
    }

    //////////////////////////////////////////////////////////////////////////
    // new since 1.2.13
    //////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of 
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    {
      return "Mengimplementasikan "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      return "Diimplementasikan di "+trWriteList(numEntries)+".";
    }

    //////////////////////////////////////////////////////////////////////////
    // new since 1.2.16
    //////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      return "Daftar Isi";
    }

    //////////////////////////////////////////////////////////////////////////
    // new since 1.2.17
    //////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been 
     *  flagged deprecated 
     */
    virtual QCString trDeprecatedList()
    {
      return "Daftar Kadaluarsa";
    }

    //////////////////////////////////////////////////////////////////////////
    // new since 1.2.18
    //////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in 
     * a C# program
     */
    virtual QCString trEvents()
    {
      return "Event";
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      return "Dokumentasi Event";
    }

    //////////////////////////////////////////////////////////////////////////
    // new since 1.3
    //////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    { 
      return "Jenis Paket";
    }
    /*! Used as a heading for a list of Java class functions with package 
     * scope. 
     */
    virtual QCString trPackageMembers()
    { 
      return "Daftar Fungsi Paket";
    }
    /*! Used as a heading for a list of static Java class functions with 
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    { 
      return "Daftar Fungsi Statis Paket";
    }
    /*! Used as a heading for a list of Java class variables with package 
     * scope.
     */
    virtual QCString trPackageAttribs()
    { 
      return "Daftar Atribut Paket";
    }
    /*! Used as a heading for a list of static Java class variables with 
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    { 
      return "Daftar Atribut Statis Paket";
    }

    //////////////////////////////////////////////////////////////////////////
    // new since 1.3.1
    //////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page 
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll()
    {
      return "Semua";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph()
    {
      return "Berikut ini diagram pemanggilan untuk fungsi ini:";
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
      return "Cari";
    }
    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle()
    {
      return "Hasil Pencarian";
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
        return "Maaf, tidak ada dokumen yang cocok.";
      }
      else if (numDocuments==1)
      {
        return "Ditemukan <b>1</b> dokumen yang cocok.";
      }
      else 
      {
        return "Ditemukan <b>$num</b> documents yang cocok. "
               "Menampilkan hasil terbaik lebih awal.";
      }
    }
    /*! This string is put before the list of matched words, for each search 
     *  result. What follows is the list of words that matched the query.
     */
    virtual QCString trSearchMatches()
    {
      return "Kecocokan:";
    }

    //////////////////////////////////////////////////////////////////////////
    // new since 1.3.8
    //////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename)
    {
      return "Kode Sumber:" + filename;
    }

    //////////////////////////////////////////////////////////////////////////
    // new since 1.3.9
    //////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex()
    { return "Struktur Direktori"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation()
    { return "Dokumentasi Direktori"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories()
    { return "Daftar Direktori"; }

    /*! This returns a sentences that introduces the directory hierarchy. 
     *  and the fact that it is sorted alphabetically per level
     */
    virtual QCString trDirDescription()
    { return "Struktur direktori ini diurutkan hampir berdasarkan abjad:";
    }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const char *dirName)
    { QCString result="Referensi Direktori "; result+=dirName; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool first_capital, bool)
    { 
      QCString result((first_capital ? "Direktori" : "direktori"));
      //if (singular) result+="y"; else result+="ies";
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
      return "Ini adalah fungsi yang di-overload. Perbedaannya dengan fungsi di atas hanya pada "
             "parameter-parameter yang diterima.";
    }
};

#endif
