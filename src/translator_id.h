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

#ifndef TRANSLATOR_ID_H
#define TRANSLATOR_ID_H

/*
 * Translasi berikut didasarkan pada versi translator_en.h dalam
 * Doxygen 1.7.5.1.
 *
 * Penterjemah: Adhi Hargo <cadmus_sw at yahoo.com>
 */
class TranslatorIndonesian : public TranslatorAdapter_1_8_0
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
    QCString latexLanguageSupportCommand() override
    {
      return "\\usepackage[bahasa]{babel}";
    }

    QCString trISOLang() override
    {
      return "id";
    }
    QCString getLanguageString() override
    {
      return "0x421 Indonesian";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    QCString trRelatedFunctions() override
    { return "Fungsi-fungsi Terkait"; }

    /*! subscript for the related functions. */
    QCString trRelatedSubscript() override
    { return "(Perhatikan bahwa fungsi-fungsi tersebut bukan fungsi anggota.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    QCString trDetailedDescription() override
    { return "Keterangan Lengkap"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    QCString trDetails() override
    { return "Detail"; }

    /*! header that is put before the list of typedefs. */
    QCString trMemberTypedefDocumentation() override
    { return "Dokumentasi Anggota: Tipe"; }

    /*! header that is put before the list of enumerations. */
    QCString trMemberEnumerationDocumentation() override
    { return "Dokumentasi Anggota: Enumerasi"; }

    /*! header that is put before the list of member functions. */
    QCString trMemberFunctionDocumentation() override
    { return "Dokumentasi Anggota: Fungsi"; }

    /*! header that is put before the list of member attributes. */
    QCString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Dokumentasi Variabel";
      }
      else
      {
        return "Dokumentasi Anggota: Data";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    QCString trMore() override
    { return "Selengkapnya..."; }

    /*! put in the class documentation */
    QCString trListOfAllMembers() override
    { return "Daftar semua anggota"; }

    /*! used as the title of the "list of all members" page of a class */
    QCString trMemberList() override
    { return "Daftar anggota"; }

    /*! this is the first part of a sentence that is followed by a class name */
    QCString trThisIsTheListOfAllMembers() override
    { return "Berikut ini daftar lengkap anggota untuk"; }

    /*! this is the remainder of the sentence after the class name */
    QCString trIncludingInheritedMembers() override
    { return ", termasuk semua anggota yang diwariskan."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="Dibangkitkan secara otomatis oleh Doxygen";
      if (!s.isEmpty()) result+=" untuk "+s;
      result+=" dari kode sumber.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    QCString trEnumName() override
    { return "nama enumerasi"; }

    /*! put after an enum value in the list of all members */
    QCString trEnumValue() override
    { return "nilai enumerasi"; }

    /*! put after an undocumented member in the list of all members */
    QCString trDefinedIn() override
    { return "didefinisikan di"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    QCString trModules() override
    { return "Modul"; }

    /*! This is put above each page as a link to the class hierarchy */
    QCString trClassHierarchy() override
    { return "Hierarki Kelas"; }

    /*! This is put above each page as a link to the list of annotated classes */
    QCString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Struktur Data";
      }
      else
      {
        return "Daftar Kelas";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    QCString trFileList() override
    { return "Daftar File"; }

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Variabel Data";
      }
      else
      {
        return "Daftar Anggota Kelas";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    QCString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Daftar Definisi Global";
      }
      else
      {
        return "Daftar Anggota File";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    QCString trRelatedPages() override
    { return "Halaman Terkait"; }

    /*! This is put above each page as a link to all examples. */
    QCString trExamples() override
    { return "Contoh"; }

    /*! This is put above each page as a link to the search engine. */
    QCString trSearch() override
    { return "Cari"; }

    /*! This is an introduction to the class hierarchy. */
    QCString trClassHierarchyDescription() override
    { return "Hierarki kelas ini diurutkan kurang-lebih"
	" berdasarkan abjad:";
    }

    /*! This is an introduction to the list with all files. */
    QCString trFileListDescription(bool extractAll) override
    {
      QCString result="Berikut ini daftar seluruh file";
      if (!extractAll) result+=" yang didokumentasikan";
      result += ", dengan penjelasan singkat:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    QCString trCompoundListDescription() override
    {

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Berikut ini daftar struktur data, dengan penjelasan singkat:";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
      {
        return "Berikut ini daftar kelas, dengan penjelasan singkat:";
      }
      else
      {
        return "Berikut ini daftar kelas, struct, union, dan interface, dengan penjelasan singkat:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    QCString trCompoundMembersDescription(bool extractAll) override
    {
      QCString result="Berikut ini daftar seluruh ";
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="variabel anggota struct dan union";
      }
      else
      {
        result+="kelas";
      }
      if (!extractAll)
      {
        result+=" yang didokumentasikan";
      }
      result+=", dengan tautan ke ";
      if (!extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="dokumentasi struct/union untuk setiap variabel:";
        }
        else
        {
          result+="dokumentasi kelas untuk setiap anggota:";
        }
      }
      else
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="struct/union yang memuatnya:";
        }
        else
        {
          result+="kelas yang memuatnya:";
        }
      }
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    QCString trFileMembersDescription(bool extractAll) override
    {
      QCString result="Berikut ini daftar ";

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="fungsi, variabel, makro definisi, enumerasi, dan tipe";
      }
      else
      {
        result+="file";
      }
      if (!extractAll) result+=" yang didokumentasikan";
      result+=", dengan tautan ke ";
      if (extractAll)
        result+="file yang memuatnya:";
      else
        result+="dokumentasinya:";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    QCString trExamplesDescription() override
    { return "Berikut ini daftar semua contoh:"; }

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription() override
    { return "Berikut ini daftar semua halaman dokumentasi yang terkait:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    QCString trModulesDescription() override
    { return "Berikut ini daftar semua modul:"; }

    // index titles (the project name is prepended for these)

    /*! This is used in HTML as the title of index.html. */
    QCString trDocumentation(const QCString &projName) override
    { return (!projName.isEmpty()?projName + " " : "") + "Dokumentasi"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    QCString trModuleIndex() override
    { return "Indeks Modul"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    QCString trHierarchicalIndex() override
    { return "Indeks Hierarki Kelas"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    QCString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
    QCString trFileIndex() override
    { return "Indeks File"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    QCString trModuleDocumentation() override
    { return "Dokumentasi Modul"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Dokumentasi Struktur Data";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return "Dokumentasi Kelas";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    QCString trFileDocumentation() override
    { return "Dokumentasi File"; }

    /*! This is used in LaTeX as the title of the document */
    QCString trReferenceManual() override
    { return "Dokumen Referensi"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    QCString trDefines() override
    { return "Makro Definisi"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    QCString trTypedefs() override
    { return "Definisi Tipe"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    QCString trEnumerations() override
    { return "Enumerasi"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    QCString trFunctions() override
    { return "Fungsi"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trVariables() override
    { return "Variabel"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trEnumerationValues() override
    { return "Nilai enumerasi"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    QCString trDefineDocumentation() override
    { return "Dokumentasi Makro Definisi"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    QCString trTypedefDocumentation() override
    { return "Dokumentasi Definisi Tipe"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation() override
    { return "Dokumentasi Tipe Enumerasi"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    QCString trFunctionDocumentation() override
    { return "Dokumentasi Fungsi"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    QCString trVariableDocumentation() override
    { return "Dokumentasi Variabel"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    QCString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
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
    QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result="Dibangkitkan pada tanggal "+date;
      if (!projName.isEmpty()) result+=" untuk "+projName;
      result+=" oleh";
      return result;
    }

    /*! this text is put before a class diagram */
    QCString trClassDiagram(const QCString &clName) override
    {
      return "Diagram hierarki kelas untuk "+clName+":";
    }

    /*! this text is generated when the \\warning command is used. */
    QCString trWarning() override
    { return "Peringatan"; }

    /*! this text is generated when the \\version command is used. */
    QCString trVersion() override
    { return "Versi"; }

    /*! this text is generated when the \\date command is used. */
    QCString trDate() override
    { return "Tanggal"; }

    /*! this text is generated when the \\return command is used. */
    QCString trReturns() override
    { return "Mengembalikan"; }

    /*! this text is generated when the \\sa command is used. */
    QCString trSeeAlso() override
    { return "Lihat juga"; }

    /*! this text is generated when the \\param command is used. */
    QCString trParameters() override
    { return "Parameter"; }

    /*! this text is generated when the \\exception command is used. */
    QCString trExceptions() override
    { return "Eksepsi"; }

    /*! this text is used in the title page of a LaTeX document. */
    QCString trGeneratedBy() override
    { return "Dibangkitkan oleh"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    QCString trNamespaceList() override
    { return "Daftar Namespace"; }

    /*! used as an introduction to the namespace list */
    QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="Berikut ini daftar namespace";
      if (!extractAll) result+=" yang didokumentasikan";
      result+=", dengan keterangan singkat:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    QCString trFriends() override
    { return "Friend"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    QCString trRelatedFunctionDocumentation() override
    { return "Dokumentasi Friend Dan Fungsi Terkait"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    QCString trCompoundReference(const QCString &clName,
        ClassDef::CompoundType compType,
        bool isTemplate) override
    {
      QCString result="Referensi";
      switch(compType)
      {
        case ClassDef::Class:      result+=" Kelas "; break;
        case ClassDef::Struct:     result+=" Struct "; break;
        case ClassDef::Union:      result+=" Union "; break;
        case ClassDef::Interface:  result+=" Interface "; break;
        case ClassDef::Protocol:   result+=" Protokol "; break;
        case ClassDef::Category:   result+=" Kategori "; break;
        case ClassDef::Exception:  result+=" Eksepsi "; break;
        default: break;
      }
      if (isTemplate) result+=" Template ";
      result+=clName;
      return result;
    }

    /*! used as the title of the HTML page of a file */
    QCString trFileReference(const QCString &fileName) override
    {
      QCString result="Referensi File ";
      result+=fileName;
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    QCString trNamespaceReference(const QCString &namespaceName) override
    {
      QCString result="Referensi Namespace ";
      result+=namespaceName;
      return result;
    }

    QCString trPublicMembers() override
    { return "Fungsi Anggota Publik"; }
    QCString trPublicSlots() override
    { return "Slot Publik"; }
    QCString trSignals() override
    { return "Signal"; }
    QCString trStaticPublicMembers() override
    { return "Fungsi Anggota Publik Statis"; }
    QCString trProtectedMembers() override
    { return "Fungsi Anggota Diproteksi"; }
    QCString trProtectedSlots() override
    { return "Slot Diproteksi"; }
    QCString trStaticProtectedMembers() override
    { return "Fungsi Anggota Diproteksi Statis"; }
    QCString trPrivateMembers() override
    { return "Fungsi Anggota Privat"; }
    QCString trPrivateSlots() override
    { return "Slot Privat"; }
    QCString trStaticPrivateMembers() override
    { return "Fungsi Anggota Privat Statis"; }

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
            result+=", dan ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritsList(int numEntries) override
    {
      return "Mewarisi "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritedByList(int numEntries) override
    {
      return "Diwariskan ke "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    QCString trReimplementedFromList(int numEntries) override
    {
      return "Di-reimplementasikan dari "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    QCString trReimplementedInList(int numEntries) override
    {
      return "Diimplementasikan ulang di "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    QCString trNamespaceMembers() override
    { return "Anggota Namespace"; }

    /*! This is an introduction to the page with all namespace members */
    QCString trNamespaceMemberDescription(bool extractAll) override
    {
      QCString result="Berikut ini daftar anggota namespace";
      if (!extractAll) result+=" yang didokumentasikan";
      result+=", dengan tautan ke ";
      if (extractAll)
        result+="dokumentasi namespace untuk setiap anggota:";
      else
        result+="namespace yang memuatnya:";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    QCString trNamespaceIndex() override
    { return "Indeks Namespace"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation() override
    { return "Dokumentasi Namespace"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces() override
    { return "Daftar Namespace"; }

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
      QCString result="Dokumentasi untuk ";
      switch(compType)
      {
        case ClassDef::Class:      result+="kelas"; break;
        case ClassDef::Struct:     result+="struct"; break;
        case ClassDef::Union:      result+="union"; break;
        case ClassDef::Interface:  result+="interface"; break;
        case ClassDef::Protocol:   result+="protokol"; break;
        case ClassDef::Category:   result+="kategori"; break;
        case ClassDef::Exception:  result+="eksepsi"; break;
        default: break;
      }
      result+=" ini dibangkitkan dari file";
      result+=single ? "" : "-file";
      result+=" berikut:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    QCString trReturnValues() override
    { return "Nilai kembali"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage() override
    { return "Halaman Utama"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    QCString trPageAbbreviation() override
    { return "hal."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    QCString trDefinedAtLineInSourceFile() override
    {
      return "Definisi pada baris @0 dalam file @1.";
    }
    QCString trDefinedInSourceFile() override
    {
      return "Definisi dalam file @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated() override
    {
      return "Kadaluarsa";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const QCString &clName) override
    {
      return "Diagram kolaborasi untuk "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const QCString &fName) override
    {
      return "Bagan kebergantungan pemuatan untuk "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation() override
    {
      return "Dokumentasi Konstruktor & Destruktor";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode() override
    {
      return "Ke kode sumber file ini.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation() override
    {
      return "Ke dokumentasi file ini.";
    }
    /*! Text for the \\pre command */
    QCString trPrecondition() override
    {
      return "Kondisi Awal";
    }
    /*! Text for the \\post command */
    QCString trPostcondition() override
    {
      return "Kondisi Akhir";
    }
    /*! Text for the \\invariant command */
    QCString trInvariant() override
    {
      return "Invarian";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue() override
    {
      return "Nilai awal:";
    }
    /*! Text used the source code in the file index */
    QCString trCode() override
    {
      return "kode";
    }
    QCString trGraphicalHierarchy() override
    {
      return "Hierarki Kelas Secara Grafis";
    }
    QCString trGotoGraphicalHierarchy() override
    {
      return "Ke bagan grafis hierarki kelas";
    }
    QCString trGotoTextualHierarchy() override
    {
      return "Ke bagan tekstual hierarki kelas";
    }
    QCString trPageIndex() override
    {
      return "Indeks Halaman";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    QCString trNote() override
    {
      return "Catatan";
    }
    QCString trPublicTypes() override
    {
      return "Tipe Publik";
    }
    QCString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Variabel Data";
      }
      else
      {
        return "Atribut Publik";
      }
    }
    QCString trStaticPublicAttribs() override
    {
      return "Atribut Publik Statis";
    }
    QCString trProtectedTypes() override
    {
      return "Tipe Diproteksi";
    }
    QCString trProtectedAttribs() override
    {
      return "Atribut Diproteksi";
    }
    QCString trStaticProtectedAttribs() override
    {
      return "Atribut Diproteksi Statis";
    }
    QCString trPrivateTypes() override
    {
      return "Tipe Privat";
    }
    QCString trPrivateAttribs() override
    {
      return "Atribut Privat";
    }
    QCString trStaticPrivateAttribs() override
    {
      return "Atribut Privat Statis";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    QCString trTodo() override
    {
      return "Tugas";
    }
    /*! Used as the header of the todo list */
    QCString trTodoList() override
    {
      return "Daftar Tugas";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy() override
    {
      return "Direferensikan oleh";
    }
    QCString trRemarks() override
    {
      return "Keterangan";
    }
    QCString trAttention() override
    {
      return "Perhatian";
    }
    QCString trInclByDepGraph() override
    {
      return "Bagan ini menunjukkan file-file yang memuat"
	" atau menggunakan file ini baik secara langsung maupun"
	" tidak langsung:";
    }
    QCString trSince() override
    {
      return "Sejak";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    QCString trLegendTitle() override
    {
      return "Keterangan Bagan";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    QCString trLegendDocs() override
    {
      return
        "Halaman ini berisi keterangan cara membaca bagan yang dibangkitkan "
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
        "diset ke 240 kode di atas akan menghasilkan bagan berikut:"
        "<p><center><img alt=\"\" src=\"graph_legend."+getDotImageExtension()+"\"></center>\n"
        "<p>\n"
        "Kotak-kotak pada bagan di atas mempunyai arti sebagai berikut:\n"
        "<ul>\n"
        "<li>%Kotak hitam merupakan struct atau kelas yang bersangkutan.\n"
        "<li>%Kotak berbingkai hitam adalah struct atau kelas yang mempunyai dokumentasi.\n"
        "<li>%Kotak dengan bingkai abu-abu adalah struct atau kelas tanpa dokumentasi.\n"
        "<li>%Kotak dengan bingkai merah merupakan struct atau kelas yang didokumentasikan tetapi"
        "tidak semua relasinya ditampilkan. %Sebuah bagan "
        "akan terpotong apabila lebih besar daripada ukuran yang ditentukan.\n"
        "</ul>\n"
        "Arti dari tanda-tanda panah adalah sebagai berikut:\n"
        "<ul>\n"
        "<li>%Panah biru tua menandakan pewarisan publik.\n"
        "<li>%Panah hijau tua untuk pewarisan diproteksi.\n"
        "<li>%Panah merah tua untuk pewarisan privat.\n"
        "<li>%Panah ungu putus-putus menandakan kelas tersebut berhubungan dengan kelas lain. "
        "Panah tersebut diberi judul sesuai dengan kelas atau struct tujuan.\n"
        "<li>%Panah kuning putus-putus menandakan hubungan antara sebuah template kelas dan "
        "instance dari template tersebut. Panah tersebut diberi judul sesuai dengan "
        "parameter template yang digunakan.\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    QCString trLegend() override
    {
      return "keterangan";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    QCString trTest() override
    {
      return "Tes";
    }
    /*! Used as the header of the test list */
    QCString trTestList() override
    {
      return "Daftar Tes";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    QCString trProperties() override
    {
      return "Daftar Property";
    }
    /*! Used as a section header for IDL property documentation */
    QCString trPropertyDocumentation() override
    {
      return "Dokumentasi Property";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    QCString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Struktur Data";
      }
      else
      {
        return "Daftar Kelas";
      }
    }
    /*! Used as the title of a Java package */
    QCString trPackage(const QCString &name) override
    {
      return "Paket "+name;
    }
    /*! The description of the package index page */
    QCString trPackageListDescription() override
    {
      return "Berikut ini daftar paket, dengan keterangan singkat (apabila tersedia):";
    }
    /*! The link name in the Quick links header for each page */
    QCString trPackages() override
    {
      return "Daftar Paket";
    }
    /*! Text shown before a multi-line define */
    QCString trDefineValue() override
    {
      return "Nilai:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    QCString trBug() override
    {
      return "Bug";
    }
    /*! Used as the header of the bug list */
    QCString trBugList() override
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
    QCString trRTFansicp() override
    {
      return "1252";
    }


    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    QCString trRTFCharSet() override
    {
      return "0";
    }

    /*! Used as header RTF general index */
    QCString trRTFGeneralIndex() override
    {
      return "Indeks";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trClass(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "kelas", "-kelas");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trFile(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "file", "-file");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trNamespace(bool first_capital, bool) override
    {
      return createNoun(first_capital, false, "namespace", "");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGroup(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "kelompok", "-kelompok");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trPage(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "halaman", "-halaman");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trMember(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "anggota", "-anggota");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGlobal(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "definisi", "-definisi") +
                " global";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    QCString trAuthor(bool first_capital, bool) override
    {
      return createNoun(first_capital, false, "penulis", "");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    QCString trReferences() override
    {
      return "Referensi";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    QCString trImplementedFromList(int numEntries) override
    {
      return "Mengimplementasikan "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    QCString trImplementedInList(int numEntries) override
    {
      return "Diimplementasikan di "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    QCString trRTFTableOfContents() override
    {
      return "Daftar Isi";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    QCString trDeprecatedList() override
    {
      return "Daftar Kadaluarsa";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    QCString trEvents() override
    {
      return "Event";
    }
    /*! Header used for the documentation section of a class' events. */
    QCString trEventDocumentation() override
    {
      return "Dokumentasi Event";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    QCString trPackageTypes() override
    {
      return "Jenis Paket";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    QCString trPackageFunctions() override
    {
      return "Daftar Fungsi Paket";
    }
    QCString trPackageMembers() override
    {
      return "Anggota-anggota Paket";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    QCString trStaticPackageFunctions() override
    {
      return "Daftar Fungsi Statis Paket";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    QCString trPackageAttribs() override
    {
      return "Daftar Atribut Paket";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    QCString trStaticPackageAttribs() override
    {
      return "Daftar Atribut Statis Paket";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    QCString trAll() override
    {
      return "Semua";
    }
    /*! Put in front of the call graph for a function. */
    QCString trCallGraph() override
    {
      return "Berikut ini bagan fungsi-terpanggil untuk fungsi ini:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    QCString trSearchResultsTitle() override
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
    QCString trSearchResults(int numDocuments) override
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
    QCString trSearchMatches() override
    {
      return "Kecocokan:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    QCString trSourceFile(const QCString& filename) override
    {
      return "Kode Sumber:" + filename;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    QCString trDirIndex() override
    { return "Hierarki Direktori"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    QCString trDirDocumentation() override
    { return "Dokumentasi Direktori"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    QCString trDirectories() override
    { return "Daftar Direktori"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    QCString trDirReference(const QCString &dirName) override
    { QCString result="Referensi Direktori "; result+=dirName; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    QCString trDir(bool first_capital, bool) override
    {
      return createNoun(first_capital, false, "direktori", "");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    QCString trOverloadText() override
    {
      return "Ini adalah fungsi anggota yang di-overload"
	" untuk kemudahan. Perbedaannya dengan fungsi di atas"
	" hanya pada parameter-parameter yang diterima.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    QCString trCallerGraph() override
    {
      return "Berikut ini bagan fungsi-pemanggil untuk fungsi ini:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    QCString trEnumerationValueDocumentation() override
    { return "Dokumentasi Nilai Enumerasi"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    QCString trMemberFunctionDocumentationFortran() override
    { return "Dokumentasi Fungsi/Subrutin Anggota"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    QCString trCompoundListFortran() override
    { return "Daftar Tipe Data"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    QCString trCompoundMembersFortran() override
    { return "Variabel Data"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    QCString trCompoundListDescriptionFortran() override
    { return "Berikut daftar tipe data, dengan penjelasan singkat:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    QCString trCompoundMembersDescriptionFortran(bool extractAll) override
    {
      QCString result="Berikut daftar semua anggota tipe data ";
      if (!extractAll)
      {
        result+="terdokumentasi ";
      }
      result+=" dengan tautan ke ";
      if (!extractAll)
      {
         result+="dokumentasi struktur data untuk setiap anggota:";
      }
      else
      {
         result+="tipe data yang memuatnya:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index (Fortran).
     */
    QCString trCompoundIndexFortran() override
    { return "Indeks Tipe Data"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    QCString trTypeDocumentation() override
    { return "Dokumentasi Tipe Data"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    QCString trSubprograms() override
    { return "Fungsi/Subrutin"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    QCString trSubprogramDocumentation() override
    { return "Dokumentasi Fungsi/Subrutin"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     QCString trDataTypes() override
    { return "Tipe Data"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    QCString trModulesList() override
    { return "Modules List"; }

    /*! used as an introduction to the modules list (Fortran) */
    QCString trModulesListDescription(bool extractAll) override
    {
      QCString result="Berikut daftar semua modul";
      if (!extractAll) result+=" terdokumentasi";
      result+=", dengan penjelasan singkat:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    QCString trCompoundReferenceFortran(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result="Referensi ";
      if (isTemplate) result+="Template ";
      switch(compType)
      {
        case ClassDef::Class:      result+="Modul "; break;
        case ClassDef::Struct:     result+="Tipe "; break;
        case ClassDef::Union:      result+="Union "; break;
        case ClassDef::Interface:  result+="Antarmuka "; break;
        case ClassDef::Protocol:   result+="Protokol "; break;
        case ClassDef::Category:   result+="Kategori "; break;
        case ClassDef::Exception:  result+="Eksepsi "; break;
        default: break;
      }
      result+=clName;
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    QCString trModuleReference(const QCString &namespaceName) override
    {
      QCString result="Referensi Modul ";
      result+=namespaceName;
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    QCString trModulesMembers() override
    { return "Anggota Modul"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    QCString trModulesMemberDescription(bool extractAll) override
    {
      QCString result="Berikut daftar semua anggota modul ";
      if (!extractAll) result+="terdokumentasi ";
      result+="dengan tautan ke ";
      if (extractAll)
      {
        result+="dokumentasi modul untuk setiap anggota:";
      }
      else
      {
        result+="modul yang memuatnya:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all modules (Fortran).
     */
    QCString trModulesIndex() override
    { return "Indeks Modul"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trModule(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "modul", "-modul");
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      QCString result="Dokumentasi untuk ";
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
      result+=" ini dibangkitkan dari file";
      if (!single) result+="-file ";
	  result+="berikut:";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trType(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "tipe", "-tipe");
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trSubprogram(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "subprogram", "-subprogram");
    }

    /*! C# Type Constraint list */
    QCString trTypeConstraints() override
    {
      return "Batasan Tipe";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    QCString trDirRelation(const QCString &name) override
    {
      return "Relasi "+QCString(name);
    }

    /*! Loading message shown when loading search results */
    QCString trLoading() override
    {
      return "Memuat...";
    }

    /*! Label used for search results in the global namespace */
    QCString trGlobalNamespace() override
    {
      return "Namespace Global";
    }

    /*! Message shown while searching */
    QCString trSearching() override
    {
      return "Mencari...";
    }

    /*! Text shown when no search results are found */
    QCString trNoMatches() override
    {
      return "Tidak Ditemukan";
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
      return "File dimuat dalam "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    QCString trIncludesFileIn(const QCString &name) override
    {
      return "Memuat file dalam "+name;
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
      static const char *days[]   = { "Senin","Selasa","Rabu","Kamis","Jumat","Sabtu","Minggu" };
      static const char *months[] = { "Januari","Februari","Maret","April","Mei","Juni","Juli","Agustus","September","Oktober","November","Desember" };
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
      return sdate;
    }
    QCString trDayOfWeek(int dayOfWeek, bool, bool full) override
    {
      static const char *days_short[]   = { "Sen", "Sel", "Rab", "Kam", "Jum", "Sab", "Min" };
      static const char *days_full[]    = { "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu", "Minggu" };
      return full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
    }
    QCString trMonth(int month, bool, bool full) override
    {
      static const char *months_short[] = { "Jan", "Feb", "Mar", "Apr", "Mei", "Jun", "Jul", "Agu", "Sep", "Okt", "Nov", "Des" };
      static const char *months_full[]  = { "Januari", "Februari", "Maret", "April", "Mei", "Juni", "Juli", "Agustus", "September", "Oktober", "November", "Desember" };
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
    { return "Daftar Pustaka"; }

    /*! Text for copyright paragraph */
    QCString trCopyright() override
    { return "Hak Cipta"; }

    /*! Header for the graph showing the directory dependencies */
    QCString trDirDepGraph(const QCString &name) override
    { return QCString("Bagan dependensi directori untuk ")+name+":"; }

};

#endif
