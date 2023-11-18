/******************************************************************************
 *
 *
 *
 * Copyright (C) 1997-2014 by Dimitri van Heesch.
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
 * 15 Dec 2001 : Translation to greek by
 *               Harry Kalogirou <no email>
 *
 * 01 Jan 2009 : Greek maintenance by
 *               Paul Gessos <gessos.paul@gmail.com>
 *
 *
 * Δουλεύω με C, C++, Java, PHP και Python. Άλλες γλώσσες (π.χ. VHDL) μου είναι
 * άγνωστες.
 * Αν παρατηρήσετε σφάλματα ΠΑΡΑΚΑΛΩ ΠΟΛΥ επικοινωνήστε μαζί μου.
 */

/*	English to Greek keyword dictionary
	(Many words have more clear meaning in English than Greek)
	template -> πρότυπο
	instance -> υπόσταση
	module -> υπομονάδα
	interface -> διεπαφή (ενναλλακτικά μπορεί να χρησιμοποιηθεί: διασύνδεση)
	singleton -> μονοσύνολο
	service -> υπηρεσία
	exception -> εξαίρεση
	namespace -> χώρος ονομάτων
	enumeration -> απαρίθμηση
	concept -> έννοια
	signal -> σήμα
	instantiation -> ενσάρκωση
*/


#ifndef TRANSLATOR_GR_H
#define TRANSLATOR_GR_H

class TranslatorGreek : public TranslatorAdapter_1_9_6
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
    { return "greek"; }

    virtual QCString latexLanguageSupportCommand() override
    {
      return "\\usepackage{fontspec}\n"
             "\\usepackage[greek]{babel}\n";
    }

    virtual QCString trISOLang() override
    {
      return "el";
    }

    virtual QCString latexFontenc() override
    {
      return "";
    }
    virtual QCString latexFont() override
    {
      return "\\setmainfont{Libertinus Sans}\n"
             "\\setmonofont{Courier New}\n";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions() override
    { return "Σχετικές συναρτήσεις"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript() override
    { return "(Σημειώστε ότι αυτές δεν είναι συναρτήσεις μέλη.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription() override
    { return "Λεπτομερής Περιγραφή"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    virtual QCString trDetails() override
    { return "Λεπτομέρειες"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation() override
    { return "Τεκμηρίωση Μελών Ορισμών Τύπων"; }

    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation() override
    { return "Τεκμηρίωση Απαριθμήσεων Μελών"; }

    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation() override
    { return "Τεκμηρίωση Συναρτήσεων Μελών"; }

    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Τεκμηρίωση Πεδίων";
      }
      else
      {
        return "Τεκμηρίωση Δεδομένων Μελών";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore() override
    { return "Περισσότερα..."; }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers() override
    { return "Λίστα όλων των μελών"; }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList() override
    { return "Λίστα μελών"; }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers() override
    { return "Ακολουθεί η πλήρης λίστα των μελών της"; }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers() override
    { return ", περιλαμβανομένων όλων των κληρονομημένων μελών."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="Δημιουργήθηκε αυτόματα από το Doxygen";
      if (!s.isEmpty()) result+=" για "+s;
      result+=" από τον πηγαίο κώδικα.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName() override
    { return "όνομα απαρίθμησης"; }

    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue() override
    { return "τιμή απαρίθμησης"; }

    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn() override
    { return "ορισμένο στο "; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules() override
    { return "Υπομονάδες"; }

    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy() override
    { return "Ιεραρχία Κλάσεων"; }

    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Δομές Δεδομένων";
      }
      else
      {
        return "Λίστα Κλάσεων";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList() override
    { return "Λίστα Αρχείων"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Μέλη Δομών Δεδομένων";
      }
      else
      {
        return "Μέλη Κλάσεων";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Καθολικοί ορισμοί";
      }
      else
      {
        return "Μέλη Αρχείων";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages() override
    { return "Σχετικές Σελίδες"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples() override
    { return "Παραδείγματα"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch() override
    { return "Αναζήτηση"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription() override
    { return "Αυτή η λίστα κληρονομικότητας είναι μερικώς ταξινομημένη αλφαβητικά:"; }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll) override
    {
      QCString result="Ακολουθεί μια λίστα όλων των ";
      if (!extractAll) result+="τεκμηριωμένων ";
      result+="αρχείων με σύντομες περιγραφές:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription() override
    {

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Ακολουθούν οι δομές δεδομένων με σύντομες περιγραφές:";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
      {
        return "Ακολουθούν οι κλάσεις με σύντομες περιγραφές:";
      }
      else
      {
        return "Ακολουθούν οι κλάσεις, οι δομές, "
               "οι ενώσεις και οι διασυνδέσεις με σύντομες περιγραφές:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll) override
    {
      QCString result="Ακολουθεί η λίστα όλων των ";
      if (!extractAll)
      {
        result+="τεκμηριωμένων ";
      }
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="μελών δομών δεδομένων και ενώσεων ";
      }
      else
      {
        result+="κλάσεων";
      }
      result+=" με συνδέσμους ";
      if (!extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="στην τεκμηρίωση των δομών/ενώσεων για κάθε πεδίο:";
        }
        else
        {
          result+="στην τεκμηρίωση των κλάσεων για κάθε πεδίο:";
        }
      }
      else
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="στις δομές/ενώσεις που ανήκουν:";
        }
        else
        {
          result+="στις κλάσεις που ανήκουν:";
        }
      }
      return result;
    }

    /*! This is an introduction to the page with all file members. */
    virtual QCString trFileMembersDescription(bool extractAll) override
    {
      QCString result="Ακολουθεί η λίστα όλων των ";
      if (!extractAll) result+="τεκμηριωμένων ";

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="συναρτήσεων, μεταβλητών, ορισμών, απαριθμήσεων, και ορισμών τύπων";
      }
      else
      {
        result+="μελών αρχείων";
      }
      result+=" με συνδέσμους ";
      if (extractAll)
        result+="στα αρχεία που ανήκουν:";
      else
        result+="στην τεκμηρίωση:";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    virtual QCString trExamplesDescription() override
    { return "Ακολουθεί η λίστα με τα παραδείγματα:"; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription() override
    { return "Ακολουθεί η λίστα όλων των σχετικών σελίδων τεκμηρίωσης:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription() override
    { return "Ακολουθεί η λίστα όλων των υπομονάδων:"; }

    // index titles (the project name is prepended for these)


    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation() override
    { return "Τεκμηρίωση"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    virtual QCString trModuleIndex() override
    { return "Ευρετήριο υπομονάδων"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex() override
    { return "Ιεραρχικό Ευρετήριο"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    virtual QCString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Ευρετήριο δομών δεδομένων";
      }
      else
      {
        return "Συμπαγές Ευρετήριο";
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    virtual QCString trFileIndex() override
    { return "Ευρετήριο Αρχείων"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation() override
    { return "Τεκμηρίωση Υπομονάδων"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Τεκμηρίωση Δομών Δεδομένων";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return "Τεκμηρίωση Κλάσεων";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation() override
    { return "Τεκμηρίωση Αρχείων"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual() override
    { return "Εγχειρίδιο Τεκμηρίωσης"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    virtual QCString trDefines() override
    { return "Ορισμοί"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    virtual QCString trTypedefs() override
    { return "Ορισμοί Τύπων"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    virtual QCString trEnumerations() override
    { return "Απαριθμήσεις"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    virtual QCString trFunctions() override
    { return "Συναρτήσεις"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trVariables() override
    { return "Μεταβλητές"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues() override
    { return "Τιμές Απαριθμήσεων"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation() override
    { return "Τεκμηρίωση Ορισμών"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation() override
    { return "Τεκμηρίωση Ορισμών Τύπων"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation() override
    { return "Τεκμηρίωση Απαριθμήσεων"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation() override
    { return "Τεκμηρίωση Συναρτήσεων"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation() override
    { return "Τεκμηρίωση Μεταβλητών"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Δομές Δεδομένων";
      }
      else
      {
        return "Κλάσεις";
      }
    }

    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    virtual QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result="Δημιουργήθηκε στις "+date;
      if (!projName.isEmpty()) result+=" για "+projName;
      result+=" από";
      return result;
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const QCString &clName) override
    {
      return "Διάγραμμα κληρονομικότητας για την "+clName+":";
    }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning() override
    { return "Προειδοποίηση"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion() override
    { return "Έκδοση"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate() override
    { return "Ημερομηνία"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns() override
    { return "Επιστρέφει"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso() override
    { return "Κοιτάξτε επίσης "; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters() override
    { return "Παράμετροι"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions() override
    { return "Εξαίρεση"; }

    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy() override
    { return "Δημιουργήθηκε από "; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList() override
    { return "Λίστα Χώρων Ονομάτων"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="Ακολουθέι η λίστα όλων των ";
      if (!extractAll) result+="τεκμηριωμένων ";
      result+="Χώρων Ονομάτων με σύντομες περιγραφές:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends() override
    { return "Φίλοι"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    virtual QCString trRelatedFunctionDocumentation() override
    { return "Τεκμηρίωση Φιλικών και Συσχετιζόμενων Συναρτήσεων"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result="Τεκμηρίωση";
      if (isTemplate) result+=" Προτύπου";
      switch(compType)
      {
        case ClassDef::Class:      result+=" Κλάσης "; break;
        case ClassDef::Struct:     result+=" Δομής "; break;
        case ClassDef::Union:      result+=" Ένωσης "; break;
        case ClassDef::Interface:  result+=" Διεπαφής "; break;
        case ClassDef::Protocol:   result+=" Πρωτοκόλλου "; break;
        case ClassDef::Category:   result+=" Κατηγορίας "; break;
        case ClassDef::Exception:  result+=" Εξαίρεσης "; break;
        default: break;
      }
      result+=clName;
      return result;
    }

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const QCString &fileName) override
    {
      QCString result="Τεκμηρίωση Αρχείου ";
      result+=fileName;
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const QCString &namespaceName) override
    {
      QCString result="Τεκμηρίωση Χώρου Ονομάτων ";
      result+=namespaceName;
      return result;
    }

    virtual QCString trPublicMembers() override
    { return "Δημόσιες Μέθοδοι"; }
    virtual QCString trPublicSlots() override
    { return "Δημόσιες Υποδοχές"; }
    virtual QCString trSignals() override
    { return "Σήματα"; }
    virtual QCString trStaticPublicMembers() override
    { return "Στατικές Δημόσιες Μέθοδοι"; }
    virtual QCString trProtectedMembers() override
    { return "Προστατευμένες Μέθοδοι"; }
    virtual QCString trProtectedSlots() override
    { return "Προστατευμένες Υποδοχές"; }
    virtual QCString trStaticProtectedMembers() override
    { return "Στατικές Προστατευμένες Μέθοδοι"; }
    virtual QCString trPrivateMembers() override
    { return "Ιδιωτικές Μέθοδοι"; }
    virtual QCString trPrivateSlots() override
    { return "Ιδιωτικές Υποδοχές"; }
    virtual QCString trStaticPrivateMembers() override
    { return "Στατικές Ιδιωτικές Μέθοδοι"; }

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
            result+=", και ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries) override
    {
      return QCString("Κληρονομεί ")+(numEntries==1?"την ":"τις ")+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries) override
    {
      return QCString("Κληρονομείται από ")+(numEntries==1?"την ":"τις ")+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries) override
    {
      return QCString("Επαναϋλοποιείται από ")+(numEntries==1?"την ":"τις ")+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries) override
    {
      return QCString("Επαναϋλοποιείται ")+(numEntries==1?"στην ":"στις ")+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers() override
    { return "Μέλη Χώρου Ονομάτων"; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll) override
    {
      QCString result="Ακολουθεί η λίστα όλων των ";
      if (!extractAll) result+="τεκμηριωμένων ";
      result+="μελών χώρων ονομάτων με συνδέσμους ";
      if (extractAll)
        result+="στην τεκμηρίωση του χώρου ονομάτων για κάθε μέλος:";
      else
        result+="στους χώρους ονομάτων που ανήκουν:";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    virtual QCString trNamespaceIndex() override
    { return "Ευρετήριο Χώρων Ονομάτων"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation() override
    { return "Τεκμηρίωση Χώρων Ονομάτων"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces() override
    { return "Χώροι Ονομάτων"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      QCString result="Η τεκμηρίωση για ";
      switch(compType)
      {
        case ClassDef::Class:      result+="αυτή την κλάση"; break;
        case ClassDef::Struct:     result+="αυτή τη δομή"; break;
        case ClassDef::Union:      result+="αυτή την ένωση"; break;
        case ClassDef::Interface:  result+="αυτή τη διεπαφή"; break;
        case ClassDef::Protocol:   result+="αυτό το πρωτόκολλο"; break;
        case ClassDef::Category:   result+="αυτή την κατηγορία"; break;
        case ClassDef::Exception:  result+="αυτή την εξαίρεση"; break;
        default: break;
      }
      result+=" δημιουργήθηκε από ";
      if (single) result+="το ακόλουθο αρχείο:"; else result+="τα ακόλουθα αρχεία:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues() override
    { return "Τιμές Επιστροφής"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage() override
    { return "Κύρια Σελίδα"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation() override
    { return "σελ."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDefinedAtLineInSourceFile() override
    {
      return "Ορισμός στη γραμμή @0 του αρχείου @1.";
    }
    virtual QCString trDefinedInSourceFile() override
    {
      return "Ορισμός στο αρχείο @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated() override
    {
      return "Αποσυρμένο";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const QCString &clName) override
    {
      return "Διάγραμμα Συνεργασίας για την κλάση "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const QCString &fName) override
    {
      return "Διάγραμμα εξάρτησης αρχείου συμπερίληψης για το "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation() override
    {
      return "Τεκμηρίωση Constructor & Destructor";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode() override
    {
      return "Πήγαινε στον πηγαίο κώδικα του αρχείου.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation() override
    {
      return "Πήγαινε στην τεκμηρίωση του αρχείου.";
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition() override
    {
      return "Προϋποθέσεις";
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition() override
    {
      return "Μεταϋποθέσεις";
    }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant() override
    {
      return "Αμετάβλητα";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue() override
    {
      return "Αρχική τιμή:";
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode() override
    {
      return "κώδικας";
    }
    virtual QCString trGraphicalHierarchy() override
    {
      return "Γραφική Αναπαράσταση Της Ιεραρχίας Των Κλάσεων";
    }
    virtual QCString trGotoGraphicalHierarchy() override
    {
      return "Πήγαινε στην γραφική αναπαράσταση της ιεραρχίας των κλάσεων";
    }
    virtual QCString trGotoTextualHierarchy() override
    {
      return "Πήγαινε στην με κείμενο αναπαράσταση της ιεραρχίας των κλάσεων";
    }
    virtual QCString trPageIndex() override
    {
      return "Ευρετήριο Σελίδων";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    virtual QCString trNote() override
    {
      return "Σημείωση";
    }
    virtual QCString trPublicTypes() override
    {
      return "Δημόσιοι Τυποι";
    }
    virtual QCString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Πεδία Δεδομένων";
      }
      else
      {
        return "Δημόσια Χαρακτηριστικά";
      }
    }
    virtual QCString trStaticPublicAttribs() override
    {
      return "Στατικά Δημόσια Χαρακτηριστικά";
    }
    virtual QCString trProtectedTypes() override
    {
      return "Προστατευμένοι Τύποι";
    }
    virtual QCString trProtectedAttribs() override
    {
      return "Προστατευμένα Χαρακτηριστικά";
    }
    virtual QCString trStaticProtectedAttribs() override
    {
      return "Στατικά Προστατευμένα Χαρακτηριστικά";
    }
    virtual QCString trPrivateTypes() override
    {
      return "Ιδιωτικοί Τύποι";
    }
    virtual QCString trPrivateAttribs() override
    {
      return "Ιδιωτικα Χαρακτηριστικά";
    }
    virtual QCString trStaticPrivateAttribs() override
    {
      return "Στατικά Ιδιωτικα Χαρακτηριστικά";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    virtual QCString trTodo() override
    {
      return "Προς Υλοποίηση";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList() override
    {
      return "Λίστα των Προς Υλοποίηση";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy() override
    {
      return "Αναφορά από";
    }
    virtual QCString trRemarks() override
    {
      return "Παρατήρηση";
    }
    virtual QCString trAttention() override
    {
      return "Προσοχή";
    }
    virtual QCString trInclByDepGraph() override
    {
      return "Το διάγραμμα αυτό παρουσιάζει ποιά αρχεία άμεσα ή "
             "έμμεσα περιλαμβάνουν αυτό το αρχείο:";
    }
    virtual QCString trSince() override
    {
      return "Από";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle() override
    {
      return "Υπόμνημα Διαγραμμάτων";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    virtual QCString trLegendDocs() override
    {
      return
        "Αυτή η σελίδα εξηγεί το πως ερμηνεύονται τα διαγράμματα που δημιουργούνται "
        "από το doxygen.<p>\n"
        "Θεωρείστε το παρακάτω παράδειγμα:"
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
        "Αυτό οδηγεί στο επόμενο διάγραμμα:"
        "<p><center><img alt=\"\" src=\"graph_legend."+getDotImageExtension()+"\"></center></p>\n"
        "<p>\n"
        "Τα κουτιά στο παραπάνω διάγραμμα έχουν την ακόλουθη σημασία:\n"
        "</p>\n"
        "<ul>\n"
        "<li>Ένα γεμισμένο μαύρο κουτί αναπαριστά τη δομή ή την κλάση για την οποία"
        "αφορά το διάγραμμα.\n"
        "<li>Ένα κουτί με μαύρο περίγραμμα αναπαριστά μία τεκμηριωμένη δομή ή κλάση.\n"
        "<li>Ένα κουτί με γκρίζο περίγραμμα αναπαριστά μία μη τεκμηριωμένη δομή ή κλάση.\n"
        "<li>Ένα κουτί με κόκκινο περίγραμμα αναπαριστά μία τεκμηριωμένη δομή ή κλάση για "
        "την οποία δεν παρουσιάζονται όλες οι σχέσεις κληρονομικότητας και περιεχομένου. %Ένα "
        "διάγραμμα μειώνεται αν δεν χωράει στις ορισμένες διαστάσεις."
        "</ul>\n"
        "<p>\n"
        "Τα βέλη έχουν τις ακόλουθες σημασίες:\n"
        "</p>\n"
        "<ul>\n"
        "<li>Ένα βαθύ μπλε βέλος χρησιμοποιείται για να παρουσιάσει μία δημόσια σχέση κληρονομικότητας "
        "μεταξύ δύο κλάσεων.</li>\n"
        "<li>Ένα βαθύ πράσινο βέλος χρησιμοποιείται για προστατευμένη κληρονομικότητα.</li>\n"
        "<li>Ένα βαθύ κόκκινο βέλος χρησιμοποιείται για ιδιωτική κληρονομικότητα.</li>\n"
        "<li>Ένα μωβ διακεκομένο βέλος χρησιμοποιείται αν μία κλάση περιέχεται ή χρησιμοποιείται "
        "από μία άλλη κλάση. Το βέλος ονομάζεται από το όνομα της μεταβλητής(ων) "
        "μέσω της οποίας η κλάση ή δομή είναι προσβάσιμη.</li>\n"
        "<li>Ένα κίτρινο διακεκομμένο βέλος χρησιμοποιείται για μια σχέση μεταξύ ενός προτύπου αντικειμένου και "
        "της πρότυπης κλάσης από την οποία δημιουργήθηκε. Το βέλος ονομάζεται με τις παραμέτρους του προτύπου "
        "του αντικειμένου.</li>\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    virtual QCString trLegend() override
    {
      return "υπόμνημα";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    virtual QCString trTest() override
    {
      return "Δοκιμαστικό";
    }
    /*! Used as the header of the test list */
    virtual QCString trTestList() override
    {
      return "Λίστα Δοκιμαστικών";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties() override
    {
      return "Ιδιότητες";
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation() override
    {
      return "Τεκμηρίωση Ιδιοτήτων";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Δομές Δεδομένων";
      }
      else
      {
        return "Κλάσεις";
      }
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const QCString &name) override
    {
      return "Πακέτο "+name;
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription() override
    {
      return "Ακολουθεί η λίστα των πακέτων με σύντομη περιγραφή (αν υπάρχει):";
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages() override
    {
      return "Πακέτα";
    }
    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue() override
    {
      return "Τιμή:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug() override
    {
      return "Πρόβλημα";
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList() override
    {
      return "Λίστα Προβλημάτων";
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
      return "1253";
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

    /*! Used as ansicpg for RTF fcharset
     */
    virtual QCString trRTFCharSet() override
    {
      return "161";
    }

    /*! Used as header RTF general index */
    virtual QCString trRTFGeneralIndex() override
    {
      return "Ευρετήριο";
    }
    virtual QCString getLanguageString() override
    {
      return "0x408 Greece";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Κλάση" : "κλάση"));
      if (!singular)  result+="";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trFile(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Αρχεί" : "αρχεί"));
      if (!singular)  result+="α"; else result+="ο";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Χ" : "χ"));
      if (!singular)  result+="ώροι"; else result+="ώρος";
	  result+=" ονομάτων";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Ομάδ" : "ομάδ"));
      if (!singular)  result+="ες"; else result+="α";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Σελίδ" : "σελίδ"));
      if (!singular)  result+="ες"; else result+="α";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Μέλ" : "μέλ"));
      if (!singular)  result+="η"; else result+="ος";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGlobal(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Καθολικ" : "καθολικ"));
      if (!singular) result+="ές"; else result+="ή";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    virtual QCString trAuthor(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Συγραφ" : "συγραφ"));
      if (!singular)  result+=""; else result+="έας";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences() override
    {
      return "Αναφορές";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries) override
    {
      return "Υλοποιεί "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    virtual QCString trImplementedInList(int numEntries) override
    {
      return "Υλοποιείται από "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents() override
    {
      return "Πίνακας Περιεχομένων";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    virtual QCString trDeprecatedList() override
    {
      return "Λίστα Καταργημένων";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    virtual QCString trEvents() override
    {
      return "Συμβάντα";
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation() override
    {
      return "Τεκμηρίωση Συμβάντων";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes() override
    {
      return "Τύποι Πακέτων";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    virtual QCString trPackageFunctions() override
    {
      return "Συναρτήσεις Πακέτου";
    }
    virtual QCString trPackageMembers() override
    {
      return "Μέλη Πακέτου";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    virtual QCString trStaticPackageFunctions() override
    {
      return "Στατικές Συναρτήσεις Πακέτου";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    virtual QCString trPackageAttribs() override
    {
      return "Μεταβλητές Πακέτου";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    virtual QCString trStaticPackageAttribs() override
    {
      return "Στατικές Μεταβλητές Πακέτου";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll() override
    {
      return "Όλα";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph() override
    {
      return "Το διάγραμμα δείχνει ποιές συναρτήσεις καλούνται από αυτή:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle() override
    {
      return "Αποτελέσματα Αναζήτησης";
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
        return "Συγγνώμη, δεν υπάρχει κείμενο που να ταιριάζει με την αίτησή σας.";
      }
      else if (numDocuments==1)
      {
        return "Βρέθηκε <b>1</b> κείμενο που ταιριάζει με την αίτησή σας.";
      }
      else
      {
        return "Βρέθηκαν <b>$num</b> κείμενα που ταιριάζουν με την αίτησή σας. "
               "Πρώτα εμφανίζονται τα κείμενα που ταιριάζουν πιο πολύ.";
      }
    }
    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    virtual QCString trSearchMatches() override
    {
      return "Ταίριαξαν:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename) override
    {
      return "Αρχείο κώδικα " + filename;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex() override
    { return "Ιεραρχία Καταλόγου"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation() override
    { return "Τεκμηρίωση Καταλόγου"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories() override
    { return "Κατάλογοι"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const QCString &dirName) override
    { QCString result=QCString("Αναφορά του Καταλόγου ") + dirName; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Κατάλογο" : "κατάλογο"));
      if (singular) result+="ς"; else result+="ι";
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
       return "Αυτή είναι μια υπερφορτωμένη συνάρτηση-μέλος, "
              "που παρέχεται για ευκολία. Διαφέρει από την παραπάνω "
              "συνάρτηση μόνο στον τύπο των παραμέτρων που δέχεται.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    virtual QCString trCallerGraph() override
    {
      return "Το διάγραμμα δείχνει από ποιές συναρτήσεις καλείται αυτή η συνάρτηση:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation() override
    { return "Τεκμηρίωση Απαρίθμησης"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran() override
    { return "Τεκμηρίωση Συνάρτησης/Υπορουτίνας Μέλους"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    virtual QCString trCompoundListFortran() override
    { return "Λίστα Τύπων Δεδομένων"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran() override
    { return "Πεδία Δεδομένων"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran() override
    { return "Εδώ είναι οι τύποι δεδομένων με σύντομη περιγραφή:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll) override
    {
      QCString result="Εδώ είναι η λίστα όλων των ";
      if (!extractAll)
      {
        result+="τεκμηριωμένων ";
      }
      result+="μελών τύπων δεδομένων";
      result+=" με συνδέσεις ";
      if (!extractAll)
      {
         result+="στην τεκμηρίωση της δομής δεδομένων για κάθε μέλος";
      }
      else
      {
         result+="στους τύπους δεδομένων που ανήκουν:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index (Fortran).
     */
    virtual QCString trCompoundIndexFortran() override
    { return "Περιεχόμενα Τύπων Δεδομένων"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation() override
    { return "Τεκμηρίωση Τύπων Δεδομένων"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms() override
    { return "Συναρτήσεις/Υπορουτίνες"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation() override
    { return "Τεκμηρίωση Συνάρτησης/Υπορουτίνας"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes() override
    { return "Τύποι Δεδομένων"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList() override
    { return "Λίστα Υπομονάδων"; }

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll) override
    {
      QCString result="Εδώ είναι μια λίστα με όλες τις ";
      if (!extractAll) result+="τεκμηριωμένες ";
      result+="υπομονάδες με σύντομή περιγραφή:";
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
        case ClassDef::Class:      result+=" Υπομονάδα"; break;
        case ClassDef::Struct:     result+=" Τύπος"; break;
        case ClassDef::Union:      result+=" Ένωση"; break;
        case ClassDef::Interface:  result+=" Διεπαφή"; break;
        case ClassDef::Protocol:   result+=" Πρωτόκολλο"; break;
        case ClassDef::Category:   result+=" Κατηγορία"; break;
        case ClassDef::Exception:  result+=" Εξαίρεση"; break;
        default: break;
      }
      if (isTemplate) result+=" Πρότυπο";
      result+=" Δήλωση";
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    virtual QCString trModuleReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" Δηλώσεις Υπομονάδων";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers() override
    { return "Υπομονάδες Μέλη"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll) override
    {
      QCString result="Εδώ είναι μια λίστα με όλες τις ";
      if (!extractAll) result+="τεκμηριωμένες ";
      result+="μέλη υπομονάδες με συνδέσεις ";
      if (extractAll)
      {
        result+="στην τεκμηρίωση της υπομονάδας για κάθε μέλος:";
      }
      else
      {
        result+="στις υπομονάδες που ανήκουν:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all modules (Fortran).
     */
    virtual QCString trModulesIndex() override
    { return "Περιεχόμενα Υπομονάδων"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trModule(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Υπομονάδ" : "υπομονάδ"));
      if (!singular)  result+="ες"; else result+="α";
      return result;
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      QCString result="Η τεκμηρίωση για ";
      switch(compType)
      {
        case ClassDef::Class:      result+="αυτή την υπομονάδα"; break;
        case ClassDef::Struct:     result+="αυτό τον τύπο δεδομένων"; break;
        case ClassDef::Union:      result+="αυτή την ένωση"; break;
        case ClassDef::Interface:  result+="αυτή τη διεπαφή"; break;
        case ClassDef::Protocol:   result+="αυτό το πρωτόκολλο"; break;
        case ClassDef::Category:   result+="αυτή την κατηγορία"; break;
        case ClassDef::Exception:  result+="αυτή την εξαίρεση"; break;
        default: break;
      }
      result+=" δημιουργήθηκε από ";
      if (single) result+="το παρακάτω αρχείο:"; else result+="τα παρακάτω αρχεία:";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trType(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Τύπο" : "τύπο"));
      if (!singular)  result+="ι"; else result+="ος";
      result+= first_capital ? " Δεδομένων" : "  δεδομένων";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trSubprogram(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Υποπρ" : "υποπρ"));
      if (!singular)  result+="ογράμματα"; else result+="όγραμμα";
      return result;
    }

    /*! C# Type Constraint list */
    virtual QCString trTypeConstraints() override
    {
      return "Περιορισμοί Τύπων Δεδομένων";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    virtual QCString trDirRelation(const QCString &name) override
    {
      return "Σχέση του "+QCString(name);
    }

    /*! Loading message shown when loading search results */
    virtual QCString trLoading() override
    {
      return "Φόρτωση...";
    }

    /*! Label used for search results in the global namespace */
    virtual QCString trGlobalNamespace() override
    {
      return "Γενικός χώρος ονομάτων";
    }

    /*! Message shown while searching */
    virtual QCString trSearching() override
    {
      return "Αναζήτηση...";
    }

    /*! Text shown when no search results are found */
    virtual QCString trNoMatches() override
    {
      return "Δεν βρέθηκαν αποτελέσματα αναζήτησης";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.3 (missing items for the directory pages)
//////////////////////////////////////////////////////////////////////////

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the first column mentions the
     *  source file that has a relation to another file.
     */
    virtual QCString trFileIn(const QCString &name) override
    {
      return "Αρχείο σε "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    virtual QCString trIncludesFileIn(const QCString &name) override
    {
      return "Εσωκλείει το αρχείο στο "+name;
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
                                DateTimeType includeTime) override
    {
      static const char *days[]   = { "Δευ","Τρι","Τετ","Πεμ","Παρ","Σαβ","Κυρ" };
      static const char *months[] = { "Ιαν","Φεβ","Μαρ","Απρ","Μαι","Ιουν","Ιουλ","Αυγ","Σεπ","Οκτ","Νοε","Δεκ" };
      QCString sdate;
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Date)
      {
        sdate.sprintf("%s %.2d %s %d",days[dayOfWeek-1],day,months[month-1],year);
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
    virtual QCString trDayOfWeek(int dayOfWeek, bool, bool full) override
    {
      static const char *days_short[]   = { "Δευ", "Τρι", "Τετ", "Πεμ", "Παρ", "Σαβ", "Κυρ" };
      static const char *days_full[]    = { "Δευτέρα", "Τρίτη", "Τετάρτη", "Πέμπτη", "Παρασκευή", "Σάββατο", "Κυριακή" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      return text;
    }
    virtual QCString trMonth(int month, bool, bool full) override
    {
      static const char *months_short[] = { "Ιαν", "Φεβ", "Μαρ", "Απρ", "Μαϊ", "Ιουν", "Ιουλ", "Αυγ", "Σεπ", "Οκτ", "Νοε", "Δεκ" };
      static const char *months_full[]  = { "Ιανουάριος", "Φεβρουάριος", "Μάρτιος", "Απρίλιος", "Μάιος", "Ιούνιος", "Ιούλιος", "Αύγουστος", "Σεπτέμβριος", "Οκτώβριος", "Νοέμβριος", "Δεκέμβριος" };
      QCString text  = full? months_full[month-1] : months_short[month-1];
      return text;
    }
    virtual QCString trDayPeriod(int period) override
    {
      static const char *dayPeriod[] = { "π.μ.", "μ.μ." };
      return dayPeriod[period];
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    virtual QCString trCiteReferences() override
    { return "Αναφορές Βιβλιογραφίας"; }

    /*! Text for copyright paragraph */
    virtual QCString trCopyright() override
    { return "Copyright"; }

    /*! Header for the graph showing the directory dependencies */
    virtual QCString trDirDepGraph(const QCString &name) override
    { return QCString("Διάγραμμα εξάρτησης φακέλων για ")+name+":"; }


//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    virtual QCString trDetailLevel() override
    { return "Επίπεδο λεπτομέρειας"; }

    /*! Section header for list of template parameters */
    virtual QCString trTemplateParameters() override
    { return "Παράμετροι Προτύπου"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    virtual QCString trAndMore(const QCString &number) override
    { return "και "+number+" ακόμη..."; }

    /*! Used file list for a Java enum */
    virtual QCString trEnumGeneratedFromFiles(bool single) override
    { QCString result = "Η τεκμηρίωση αυτής της απαρίθμησης δημιουργήθηκε από ";
      if (single) result += "τo αρχείo"; else result += "τα αρχεία";
      result+=":";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    virtual QCString trEnumReference(const QCString &name) override
    { return QCString("Αναφορά Απαρίθμησης ") + name; }

    /*! Used for a section containing inherited members */
    virtual QCString trInheritedFrom(const QCString &members,const QCString &what) override
    { return QCString(members)+" κληρονόμησαν από "+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    virtual QCString trAdditionalInheritedMembers() override
    { return "Επιπρόσθετες Κληρονομημένες Μέθοδοι"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    virtual QCString trPanelSynchronisationTooltip(bool enable) override
    {
      QCString opt = enable ? "ενεργοποιήσετε" : "απενεργοποιήσετε";
      return "Κάντε κλικ για να "+opt+" το συγχρονισμό του παραθύρου";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    virtual QCString trProvidedByCategory() override
    {
      return "Παρέχεται από την κατηγορία @0.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    virtual QCString trExtendsClass() override
    {
      return "Κληρονομει την κλάση @0.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    virtual QCString trClassMethods() override
    {
      return "Μέθοδοι Κλάσης";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    virtual QCString trInstanceMethods() override
    {
      return "Μέθοδοι Υπόστασης";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    virtual QCString trMethodDocumentation() override
    {
      return "Τεκμηρίωση Μεθόδου";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    virtual QCString trInterfaces() override
    { return "Εξαγόμενες Διεπαφές"; }

    /** old style UNO IDL services: inherited services */
    virtual QCString trServices() override
    { return "Συμπεριλαμβανόμενες Υπηρεσίες"; }

    /** UNO IDL constant groups */
    virtual QCString trConstantGroups() override
    { return "Ομάδες Σταθερών"; }

    /** UNO IDL constant groups */
    virtual QCString trConstantGroupReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" Τεκμηρίωση Ομάδας Σταθερών";
      return result;
    }
    /** UNO IDL service page title */
    virtual QCString trServiceReference(const QCString &sName) override
    {
      QCString result=sName;
      result+=" Τεκμηρίωση Υπηρεσίας";
      return result;
    }
    /** UNO IDL singleton page title */
    virtual QCString trSingletonReference(const QCString &sName) override
    {
      QCString result=sName;
      result+=" Τεκμηρίωση Μονοσύνολου";
      return result;
    }
    /** UNO IDL service page */
    virtual QCString trServiceGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      QCString result="Η τεκμηρίωση για την υπηρεσία αυτή "
                                "δημιουργήθηκε από ";
      if (single) result+="το ακόλουθο αρχείο:"; else result+="τα ακόλουθα αρχεία:";
      return result;
    }
    /** UNO IDL singleton page */
    virtual QCString trSingletonGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      QCString result="Η τεκμηρίωση για αυτό το μονοσύνολο "
                                "δημιουργήθηκε από ";
      if (single) result+="το ακόλουθο αρχείο:"; else result+="τα ακόλουθα αρχεία:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.15
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit hierarchy */
    virtual QCString trDesignUnitHierarchy() override
    { return "Ιεραρχία Μονάδας Σχεδιασμού"; }
    /** VHDL design unit list */
    virtual QCString trDesignUnitList() override
    { return "Λίστα Μονάδας Σχεδιασμού"; }
    /** VHDL design unit members */
    virtual QCString trDesignUnitMembers() override
    { return "Μέλη Μονάδας Σχεδιασμού"; }
    /** VHDL design unit list description */
    virtual QCString trDesignUnitListDescription() override
    {
        return "Ακολουθεί μια λίστα από τα μέλη της μονάδας σχεδιασμού με συνδέσμους στις Οντότητες στις οποίες ανήκουν:";
    }
    /** VHDL design unit index */
    virtual QCString trDesignUnitIndex() override
    { return "Ευρετήριο Μονάδων Σχεδιασμού"; }
    /** VHDL design units */
    virtual QCString trDesignUnits() override
    { return "Μονάδες Σχεδιασμού"; }
    /** VHDL functions/procedures/processes */
    virtual QCString trFunctionAndProc() override
    { return "Συναρτήσεις/Διαδικασίες/Διεργασίες"; }
    /** VHDL type */
    virtual QCString trVhdlType(VhdlSpecifier type,bool single) override
    {
      switch(type)
      {
        case VhdlSpecifier::LIBRARY:
          if (single) return "Βιβλιοθήκη";
          else        return "Βιβλιοθήκες";
        case VhdlSpecifier::PACKAGE:
          if (single) return "Πακέτο";
          else        return "Πακέτα";
        case VhdlSpecifier::SIGNAL:
          if (single) return "Σήμα";
          else        return "Σήματα";
        case VhdlSpecifier::COMPONENT:
          if (single) return "Εξάρτημα";
          else        return "Εξαρτήματα";
        case VhdlSpecifier::CONSTANT:
          if (single) return "Σταθερά";
          else        return "Σταθερές";
        case VhdlSpecifier::ENTITY:
          if (single) return "Οντότητα";
          else        return "Οντότητες";
        case VhdlSpecifier::TYPE:
          if (single) return "Τύπος";
          else        return "Τύποι";
        case VhdlSpecifier::SUBTYPE:
          if (single) return "Υποτύπος";
          else        return "Υποτύποι";
        case VhdlSpecifier::FUNCTION:
          if (single) return "Συνάρτηση";
          else        return "Συναρτήσεις";
        case VhdlSpecifier::RECORD:
          if (single) return "Εγγραφή";
          else        return "Εγγραφές";
        case VhdlSpecifier::PROCEDURE:
          if (single) return "Διαδικασία";
          else        return "Διαδικασίες";
        case VhdlSpecifier::ARCHITECTURE:
          if (single) return "Αρχιτεκτονική";
          else        return "Αρχιτεκτονικές";
        case VhdlSpecifier::ATTRIBUTE:
          if (single) return "Ιδιότητα";
          else        return "Ιδιότητες";
        case VhdlSpecifier::PROCESS:
          if (single) return "Διεργασία";
          else        return "Διεργασίες";
        case VhdlSpecifier::PORT:
          if (single) return "Πόρτα";
          else        return "Πόρτες";
        case VhdlSpecifier::USE:
          if (single) return "χρήση διάταξης";
          else        return "Χρήση Διατάξεων";
        case VhdlSpecifier::GENERIC:
          if (single) return "Γενίκευση";
          else        return "Γενικεύσεις";
        case VhdlSpecifier::PACKAGE_BODY:
          return "Σώμα Πακέτου";
        case VhdlSpecifier::UNITS:
          return "Μονάδες";
        case VhdlSpecifier::SHAREDVARIABLE:
          if (single) return "Κοινόχρηστη Μεταβλητή";
          else        return "Κοινόχρηστες Μεταβλητές";
        case VhdlSpecifier::VFILE:
          if (single) return "Αρχείο";
          else        return "Αρχεία";
        case VhdlSpecifier::GROUP:
          if (single) return "Ομάδα";
          else        return "Ομάδες";
        case VhdlSpecifier::INSTANTIATION:
          if (single) return "Ενσάρκωση";
          else        return "Ενσαρκώσεις";
        case VhdlSpecifier::ALIAS:
          if (single) return "Συνώνυμο";
          else        return "Συνώνυμα";
        case VhdlSpecifier::CONFIG:
          if (single) return "Ρύθμιση";
          else        return "Ρυθμίσεις";
        case VhdlSpecifier::MISCELLANEOUS:
          return "Διάφορα";
        case VhdlSpecifier::UCF_CONST:
          return "Εξαναγκασμοί";
        default:
          return "Κλάση";
      }
    }
    virtual QCString trCustomReference(const QCString &name) override
    { return QCString("Τεκμηρίωση ")+name; }

    /* Slice */
    virtual QCString trConstants() override
    {
        return "Σταθερές";
    }
    virtual QCString trConstantDocumentation() override
    {
        return "Τεκμηρίωση Σταθεράς";
    }
    virtual QCString trSequences() override
    {
        return "Ακολουθίες";
    }
    virtual QCString trSequenceDocumentation() override
    {
        return "Τεκμηρίωση Ακολουθίας";
    }
    virtual QCString trDictionaries() override
    {
        return "Λεξικά";
    }
    virtual QCString trDictionaryDocumentation() override
    {
        return "Τεκμηρίωση Λεξικού";
    }
    virtual QCString trSliceInterfaces() override
    {
        return "Διεπαφές";
    }
    virtual QCString trInterfaceIndex() override
    {
        return "Ευρετήριο Διεπαφής";
    }
    virtual QCString trInterfaceList() override
    {
        return "Λίστα Διεπαφής";
    }
    virtual QCString trInterfaceListDescription() override
    {
        return "Ακολουθούν οι διεπαφές με σύντομες περιγραφές:";
    }
    virtual QCString trInterfaceHierarchy() override
    {
        return "Ιεραρχία Διεπαφής";
    }
    virtual QCString trInterfaceHierarchyDescription() override
    {
        return "Αυτή η λίστα ιεραρχίας είναι ταξινομημένη χονδροειδώς και όχι αυστηρά αλφαβητικά:";
    }
    virtual QCString trInterfaceDocumentation() override
    {
        return "Τεκμηρίωση Διεπαφής";
    }
    virtual QCString trStructs() override
    {
        return "Δομές";
    }
    virtual QCString trStructIndex() override
    {
        return "Ευρετήριο Δομής";
    }
    virtual QCString trStructList() override
    {
        return "Λίστα Δομής";
    }
    virtual QCString trStructListDescription() override
    {
        return "Ακολουθούν οι δομές με σύντομες περιγραφές:";
    }
    virtual QCString trStructDocumentation() override
    {
        return "Τεκμηρίωση Δομής";
    }
    virtual QCString trExceptionIndex() override
    {
        return "Ευρετήριο Εξαιρέσεων";
    }
    virtual QCString trExceptionList() override
    {
        return "Λίστα Εξαίρεσης";
    }
    virtual QCString trExceptionListDescription() override
    {
        return "Ακολουθούν οι εξαιρέσεις με σύντομες περιγραφές:";
    }
    virtual QCString trExceptionHierarchy() override
    {
        return "Ιεραρχία Εξαίρεσης";
    }
    virtual QCString trExceptionHierarchyDescription() override
    {
        return "Αυτή η λίστα ιεραρχίας είναι ταξινομημένη χονδροειδώς και όχι αυστηρά αλφαβητικά:";
    }
    virtual QCString trExceptionDocumentation() override
    {
        return "Τεκμηρίωση Εξαίρεσης";
    }
    virtual QCString trCompoundReferenceSlice(const QCString &clName, ClassDef::CompoundType compType, bool isLocal) override
    {
      QCString result="Τεκμηρίωση ";
      if (isLocal)
		result+=compType == ClassDef::Protocol ? "Τοπικού " : "Τοπικής ";
      switch(compType)
      {
        case ClassDef::Class:      result+="Κλάσης "; break;
        case ClassDef::Struct:     result+="Δομής "; break;
        case ClassDef::Union:      result+="Ένωσης "; break;
        case ClassDef::Interface:  result+="Διεπαφής "; break;
        case ClassDef::Protocol:   result+="Πρωτοκόλλου "; break;
        case ClassDef::Category:   result+="Κατηγορίας "; break;
        case ClassDef::Exception:  result+="Εξαίρεσης "; break;
        default: break;
      }
      result+=clName;
      return result;
    }
    virtual QCString trOperations() override
    {
        return "Πράξεις";
    }
    virtual QCString trOperationDocumentation() override
    {
        return "Τεκμηρίωση Πράξης";
    }
    virtual QCString trDataMembers() override
    {
        return "Μέλη Δεδομένων";
    }
    virtual QCString trDataMemberDocumentation() override
    {
        return "Τεκμηρίωση Μέλους Δεδομένων";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.19
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit documentation */
    virtual QCString trDesignUnitDocumentation() override
    { return "Τεκμηρίωση Μονάδας Σχεδιασμού"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.2
//////////////////////////////////////////////////////////////////////////

    /** C++20 concept */
    virtual QCString trConcept(bool first_capital, bool singular) override
    {
      QCString result((first_capital ? "Έννοι" : "έννοι"));
      result+=singular ? "α" : "ες";
      return result;
    }
    /*! used as the title of the HTML page of a C++20 concept page */
    virtual QCString trConceptReference(const QCString &conceptName) override
    {
      QCString result("Αναφορά Έννοιας ");
      result+=conceptName;
      return result;
    }

    /*! used as the title of page containing all the index of all concepts. */
    virtual QCString trConceptList() override
    { return "Λίστα Έννοιας"; }

    /*! used as the title of chapter containing the index listing all concepts. */
    virtual QCString trConceptIndex() override
    { return "Ευρετήριο Έννοιας"; }

    /*! used as the title of chapter containing all information about concepts. */
    virtual QCString trConceptDocumentation() override
    { return "Τεκμηρίωση Έννοιας"; }

    /*! used as an introduction to the concept list */
    virtual QCString trConceptListDescription(bool extractAll) override
    {
      QCString result="Ακολουθεί μια λίστα από όλες τις ";
      if (!extractAll) result+="τεκμηριωμένες ";
      result+="έννοιες με σύντομες περιγραφές:";
      return result;
    }

    /*! used to introduce the definition of the C++20 concept */
    virtual QCString trConceptDefinition() override
    {
      return "Ορισμός Έννοιας";
    }

    /*! the compound type as used for the xrefitems */
    virtual QCString trCompoundType(ClassDef::CompoundType compType, SrcLangExt lang) override
    {
      QCString result;
      switch(compType)
      {
        case ClassDef::Class:
          if (lang == SrcLangExt_Fortran) trType(true,true);
          else result=trClass(true,true);
          break;
        case ClassDef::Struct:     result="Δομής"; break;
        case ClassDef::Union:      result="Ένωσης"; break;
        case ClassDef::Interface:  result="Διεπαφής"; break;
        case ClassDef::Protocol:   result="Πρωτοκόλλου"; break;
        case ClassDef::Category:   result="Κατηγορίας"; break;
        case ClassDef::Exception:  result="Εξαίρεσης"; break;
        case ClassDef::Service:    result="Υπηρεσίας"; break;
        case ClassDef::Singleton:  result="Μονοσύνολου"; break;
        default: break;
      }
      return result;
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.9.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trPackageList() override
    { return "Λίστα πακέτου"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used for translation of the word that will be
     *  followed by a single name of the VHDL process flowchart.
     */
    virtual QCString trFlowchart() override
    { return "Διάγραμμα ροής: "; }

    /*! Please translate also updated body of the method
     *  trMemberFunctionDocumentation(), now better adapted for
     *  VHDL sources documentation.
     */

//////////////////////////////////////////////////////////////////////////
// new since 1.9.7
//////////////////////////////////////////////////////////////////////////
    /*! used in the compound documentation before a list of related symbols.
     *
     *  Supersedes trRelatedFunctions
     */
    virtual QCString trRelatedSymbols() override
    { return "Σχετικά Σύμβολα"; }

    /*! subscript for the related symbols
     *
     *  Supersedes trRelatedSubscript
     */
    virtual QCString trRelatedSymbolsSubscript() override
    { return "(Προσέξτε ότι αυτά δεν είναι σύμβολα μέλη.)"; }

    /*! used in the class documentation as a header before the list of all
     * related classes.
     *
     * Supersedes trRelatedFunctionDocumentation
     */
    virtual QCString trRelatedSymbolDocumentation() override
    { return "Τεκμηρίωσης Φίλιων και Σχετικών Συμβόλων"; }

    virtual QCString trFileMembersDescriptionTotal(FileMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      QCString result="Αυτή είναι μια λίστα με ";

      switch (hl)
      {
        case FileMemberHighlight::All:
          if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
          {
            result+="όλες τις ";
            if (!extractAll) result+="τεκμηριωμένες ";
            result+="συναρτήσεις, μεταβλητές, ορισμούς προεπεξεργαστή, απαριθμήσεις και ορισμούς τύπων δεδομένων";
          }
          else
          {
            result+="όλα τα ";
            if (!extractAll) result+="τεκμηριωμένα ";
            result+="μέλη αρχείων";
          }
          break;
        case FileMemberHighlight::Functions:
          result+="όλες τις ";
          if (!extractAll) result+="τεκμηριωμένες ";
          result+="συναρτήσεις";
          break;
        case FileMemberHighlight::Variables:
          result+="όλες τις ";
          if (!extractAll) result+="τεκμηριωμένες ";
          result+="μεταβλητές";
          break;
        case FileMemberHighlight::Typedefs:
          result+="όλους τους ";
          if (!extractAll) result+="τεκμηριωμένους ";
          result+="ορισμούς τύπων δεδομένων";
          break;
        case FileMemberHighlight::Sequences:
          result+="όλες τις ";
          if (!extractAll) result+="τεκμηριωμένες ";
          result+="ακολουθίες";
          break;
        case FileMemberHighlight::Dictionaries:
          result+="όλα τα ";
          if (!extractAll) result+="τεκμηριωμένα ";
          result+="λεξικά";
          break;
        case FileMemberHighlight::Enums:
          result+="όλες τις ";
          if (!extractAll) result+="τεκμηριωμένες ";
          result+="απαριθμήσεις";
          break;
        case FileMemberHighlight::EnumValues:
          result+="όλες τις ";
          if (!extractAll) result+="τεκμηριωμένες ";
          result+="τιμές απαριθμήσεων";
          break;
        case FileMemberHighlight::Defines:
          result+="όλες τις ";
          if (!extractAll) result+="τεκμηριωμένες ";
          result+="μακροεντολές";
          break;
        case FileMemberHighlight::Total: // for completeness
          break;
      }
      result+=" με συνδέσμους ";
      if (extractAll)
        result+="στα αρχεία που ανήκουν:";
      else
        result+="στην τεκμηρίωση:";
      return result;
    }
    virtual QCString trCompoundMembersDescriptionTotal(ClassMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      QCString result="Αυτή είναι μια λίστα με ";

      switch (hl)
      {
        case ClassMemberHighlight::All:
          if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
          {
            result+="όλα τα ";
            if (!extractAll) result+="τεκμηριωμένα ";
            result+="πεδία δομών και ενώσεων";
          }
          else
          {
            result+="όλα τα ";
            if (!extractAll) result+="τεκμηριωμένα ";
            result+="μέλη κλάσεων";
          }
          break;
        case ClassMemberHighlight::Functions:
          result+="όλες τις ";
          if (!extractAll) result+="τεκμηριωμένες ";
          result+="συναρτήσεις";
          break;
        case ClassMemberHighlight::Variables:
          result+="όλες τις ";
          if (!extractAll) result+="τεκμηριωμένες ";
          result+="μεταβλητές";
          break;
        case ClassMemberHighlight::Typedefs:
          result+="όλους τους ";
          if (!extractAll) result+="τεκμηριωμένους ";
          result+="ορισμούς τύπων δεδομένων";
          break;
        case ClassMemberHighlight::Enums:
          result+="όλες τις ";
          if (!extractAll) result+="τεκμηριωμένες ";
          result+="απαριθμήσεις";
          break;
        case ClassMemberHighlight::EnumValues:
          result+="όλες τις ";
          if (!extractAll) result+="τεκμηριωμένες ";
          result+="τιμές απαριθμήσεων";
          break;
        case ClassMemberHighlight::Properties:
          result+="όλες τις ";
          if (!extractAll) result+="τεκμηριωμένες ";
          result+="ιδιότητες";
          break;
        case ClassMemberHighlight::Events:
          result+="όλα τα ";
          if (!extractAll) result+="τεκμηριωμένα ";
          result+="συμβάντα";
          break;
        case ClassMemberHighlight::Related:
          result+="όλα τα ";
          if (!extractAll) result+="τεκμηριωμένα ";
          result+="σχετικά σύμβολα";
          break;
        case ClassMemberHighlight::Total: // for completeness
          break;
      }
      result+=" με συνδέσμους ";
      if (!extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="στην τεκμηρίωση κάθε πεδίου της δομής/ένωσης:";
        }
        else
        {
          result+="στην τεκμηρίωση κάθε μέλους της κλάσης:";
        }
      }
      else
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="στις δομές/ενώσεις που ανήκουν:";
        }
        else
        {
          result+="στις κλάσεις που ανήκουν:";
        }
      }
      return result;
    }
    virtual QCString trNamespaceMembersDescriptionTotal(NamespaceMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      QCString result="Αυτή είναι μια λίστα με ";
      QCString singularResult = "";
      switch (hl)
      {
        case NamespaceMemberHighlight::All:
          result+="όλα τα ";
          if (!extractAll) result+="τεκμηριωμένα ";
          result+="μέλη ";
          singularResult="μέλους";
          break;
        case NamespaceMemberHighlight::Functions:
          result+="όλες τις ";
          if (!extractAll) result+="τεκμηριωμένες ";
          result+="συναρτήσεις ";
          singularResult="συνάρτησης";
          break;
        case NamespaceMemberHighlight::Variables:
          result+="όλες τις ";
          if (!extractAll) result+="τεκμηριωμένες ";
          result+="μεταβλητές ";
          singularResult="μεταβλητής";
          break;
        case NamespaceMemberHighlight::Typedefs:
          result+="όλους τους ";
          if (!extractAll) result+="τεκμηριωμένους ";
          result+="ορισμούς τύπων δεδομένων ";
          singularResult="ορισμού τύπου δεδομένων";
          break;
        case NamespaceMemberHighlight::Sequences:
          result+="όλες τις ";
          if (!extractAll) result+="τεκμηριωμένες ";
          result+="ακολουθίες ";
          singularResult="ακολουθίας";
          break;
        case NamespaceMemberHighlight::Dictionaries:
          result+="όλα τα ";
          if (!extractAll) result+="τεκμηριωμένα ";
          result+="λεξικά ";
          singularResult="λεξικού";
          break;
        case NamespaceMemberHighlight::Enums:
          result+="όλες τις ";
          if (!extractAll) result+="τεκμηριωμένες ";
          result+="απαριθμήσεις ";
          singularResult="απαρίθμησης";
          break;
        case NamespaceMemberHighlight::EnumValues:
          result+="όλες τις ";
          if (!extractAll) result+="τεκμηριωμένες ";
          result+="τιμές απαριθμήσεων ";
          singularResult="τιμής απαρίθμησης";
          break;
        case NamespaceMemberHighlight::Total: // for completeness
          break;
      }
      result+="του χώρου ονομάτων, με συνδέσμους ";
      if (extractAll)
        result+="στην τεκμηρίωση κάθε " + singularResult + " του χώρου ονομάτων:";
      else
        result+="στους χώρους ονομάτων που ανήκουν:";
      return result;
    }
};

#endif
