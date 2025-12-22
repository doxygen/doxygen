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
 *               Pavlos Gkesos <gessos.paul@gmail.com>
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
	definition -> ορισμός
 	declaration -> δήλωση
*/


#ifndef TRANSLATOR_GR_H
#define TRANSLATOR_GR_H

class TranslatorGreek : public TranslatorAdapter_1_16_0
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
    { return "greek"; }

    QCString latexLanguageSupportCommand() override
    {
      return "\\usepackage{fontspec}\n"
             "\\usepackage[greek]{babel}\n";
    }

    QCString latexCommandName() override
    {
      return p_latexCommandName("xelatex");
    }

    QCString trISOLang() override
    {
      return "el";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    QCString trRelatedFunctions() override
    { return "Σχετικές συναρτήσεις"; }

    /*! subscript for the related functions. */
    QCString trRelatedSubscript() override
    { return "(Σημειώστε ότι αυτές δεν είναι συναρτήσεις μέλη.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    QCString trDetailedDescription() override
    { return "Λεπτομερής Περιγραφή"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    QCString trDetails() override
    { return "Λεπτομέρειες"; }

    /*! header that is put before the list of typedefs. */
    QCString trMemberTypedefDocumentation() override
    { return "Τεκμηρίωση Μελών Ορισμών Τύπων"; }

    /*! header that is put before the list of enumerations. */
    QCString trMemberEnumerationDocumentation() override
    { return "Τεκμηρίωση Απαριθμήσεων Μελών"; }

    /*! header that is put before the list of member functions. */
    QCString trMemberFunctionDocumentation() override
    { return "Τεκμηρίωση Συναρτήσεων Μελών"; }

    /*! header that is put before the list of member attributes. */
    QCString trMemberDataDocumentation() override
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
    QCString trMore() override
    { return "Περισσότερα..."; }

    /*! put in the class documentation */
    QCString trListOfAllMembers() override
    { return "Λίστα όλων των μελών"; }

    /*! used as the title of the "list of all members" page of a class */
    QCString trMemberList() override
    { return "Λίστα μελών"; }

    /*! this is the first part of a sentence that is followed by a class name */
    QCString trThisIsTheListOfAllMembers() override
    { return "Ακολουθεί η πλήρης λίστα των μελών της"; }

    /*! this is the remainder of the sentence after the class name */
    QCString trIncludingInheritedMembers() override
    { return ", περιλαμβανομένων όλων των κληρονομημένων μελών."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="Δημιουργήθηκε αυτόματα από το Doxygen";
      if (!s.isEmpty()) result+=" για "+s;
      result+=" από τον πηγαίο κώδικα.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    QCString trEnumName() override
    { return "όνομα απαρίθμησης"; }

    /*! put after an enum value in the list of all members */
    QCString trEnumValue() override
    { return "τιμή απαρίθμησης"; }

    /*! put after an undocumented member in the list of all members */
    QCString trDefinedIn() override
    { return "ορισμένο στο "; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    QCString trModules() override
    { return "Υπομονάδες"; }

    /*! This is put above each page as a link to the class hierarchy */
    QCString trClassHierarchy() override
    { return "Ιεραρχία Κλάσεων"; }

    /*! This is put above each page as a link to the list of annotated classes */
    QCString trCompoundList() override
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
    QCString trFileList() override
    { return "Λίστα Αρχείων"; }

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers() override
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
    QCString trFileMembers() override
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
    QCString trRelatedPages() override
    { return "Σχετικές Σελίδες"; }

    /*! This is put above each page as a link to all examples. */
    QCString trExamples() override
    { return "Παραδείγματα"; }

    /*! This is put above each page as a link to the search engine. */
    QCString trSearch() override
    { return "Αναζήτηση"; }

    /*! This is an introduction to the class hierarchy. */
    QCString trClassHierarchyDescription() override
    { return "Αυτή η λίστα κληρονομικότητας είναι μερικώς ταξινομημένη αλφαβητικά:"; }

    /*! This is an introduction to the list with all files. */
    QCString trFileListDescription(bool extractAll) override
    {
      QCString result="Ακολουθεί μια λίστα όλων των ";
      if (!extractAll) result+="τεκμηριωμένων ";
      result+="αρχείων με σύντομες περιγραφές:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    QCString trCompoundListDescription() override
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
    QCString trCompoundMembersDescription(bool extractAll) override
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
    QCString trFileMembersDescription(bool extractAll) override
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
    QCString trExamplesDescription() override
    { return "Ακολουθεί η λίστα με τα παραδείγματα:"; }

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription() override
    { return "Ακολουθεί η λίστα όλων των σχετικών σελίδων τεκμηρίωσης:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    QCString trModulesDescription() override
    { return "Ακολουθεί η λίστα όλων των υπομονάδων:"; }

    // index titles (the project name is prepended for these)


    /*! This is used in HTML as the title of index.html. */
    QCString trDocumentation(const QCString &projName) override
    { return (!projName.isEmpty()?projName + " " : "") + "Τεκμηρίωση"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    QCString trModuleIndex() override
    { return "Ευρετήριο υπομονάδων"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    QCString trHierarchicalIndex() override
    { return "Ιεραρχικό Ευρετήριο"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    QCString trCompoundIndex() override
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
    QCString trFileIndex() override
    { return "Ευρετήριο Αρχείων"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    QCString trModuleDocumentation() override
    { return "Τεκμηρίωση Υπομονάδων"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trClassDocumentation() override
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
    QCString trFileDocumentation() override
    { return "Τεκμηρίωση Αρχείων"; }

    /*! This is used in LaTeX as the title of the document */
    QCString trReferenceManual() override
    { return "Εγχειρίδιο Τεκμηρίωσης"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    QCString trDefines() override
    { return "Ορισμοί"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    QCString trTypedefs() override
    { return "Ορισμοί Τύπων"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    QCString trEnumerations() override
    { return "Απαριθμήσεις"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    QCString trFunctions() override
    { return "Συναρτήσεις"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trVariables() override
    { return "Μεταβλητές"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trEnumerationValues() override
    { return "Τιμές Απαριθμήσεων"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    QCString trDefineDocumentation() override
    { return "Τεκμηρίωση Ορισμών"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    QCString trTypedefDocumentation() override
    { return "Τεκμηρίωση Ορισμών Τύπων"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation() override
    { return "Τεκμηρίωση Απαριθμήσεων"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    QCString trFunctionDocumentation() override
    { return "Τεκμηρίωση Συναρτήσεων"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    QCString trVariableDocumentation() override
    { return "Τεκμηρίωση Μεταβλητών"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    QCString trCompounds() override
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
    QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result="Δημιουργήθηκε στις "+date;
      if (!projName.isEmpty()) result+=" για "+projName;
      result+=" από";
      return result;
    }

    /*! this text is put before a class diagram */
    QCString trClassDiagram(const QCString &clName) override
    {
      return "Διάγραμμα κληρονομικότητας για την "+clName+":";
    }

    /*! this text is generated when the \\warning command is used. */
    QCString trWarning() override
    { return "Προειδοποίηση"; }

    /*! this text is generated when the \\version command is used. */
    QCString trVersion() override
    { return "Έκδοση"; }

    /*! this text is generated when the \\date command is used. */
    QCString trDate() override
    { return "Ημερομηνία"; }

    /*! this text is generated when the \\return command is used. */
    QCString trReturns() override
    { return "Επιστρέφει"; }

    /*! this text is generated when the \\sa command is used. */
    QCString trSeeAlso() override
    { return "Κοιτάξτε επίσης "; }

    /*! this text is generated when the \\param command is used. */
    QCString trParameters() override
    { return "Παράμετροι"; }

    /*! this text is generated when the \\exception command is used. */
    QCString trExceptions() override
    { return "Εξαίρεση"; }

    /*! this text is used in the title page of a LaTeX document. */
    QCString trGeneratedBy() override
    { return "Δημιουργήθηκε από "; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    QCString trNamespaceList() override
    { return "Λίστα Χώρων Ονομάτων"; }

    /*! used as an introduction to the namespace list */
    QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="Ακολουθέι η λίστα όλων των ";
      if (!extractAll) result+="τεκμηριωμένων ";
      result+="Χώρων Ονομάτων με σύντομες περιγραφές:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    QCString trFriends() override
    { return "Φίλοι"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    QCString trRelatedFunctionDocumentation() override
    { return "Τεκμηρίωση Φιλικών και Συσχετιζόμενων Συναρτήσεων"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    QCString trCompoundReference(const QCString &clName,
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
    QCString trFileReference(const QCString &fileName) override
    {
      QCString result="Τεκμηρίωση Αρχείου ";
      result+=fileName;
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    QCString trNamespaceReference(const QCString &namespaceName) override
    {
      QCString result="Τεκμηρίωση Χώρου Ονομάτων ";
      result+=namespaceName;
      return result;
    }

    QCString trPublicMembers() override
    { return "Δημόσιες Μέθοδοι"; }
    QCString trPublicSlots() override
    { return "Δημόσιες Υποδοχές"; }
    QCString trSignals() override
    { return "Σήματα"; }
    QCString trStaticPublicMembers() override
    { return "Στατικές Δημόσιες Μέθοδοι"; }
    QCString trProtectedMembers() override
    { return "Προστατευμένες Μέθοδοι"; }
    QCString trProtectedSlots() override
    { return "Προστατευμένες Υποδοχές"; }
    QCString trStaticProtectedMembers() override
    { return "Στατικές Προστατευμένες Μέθοδοι"; }
    QCString trPrivateMembers() override
    { return "Ιδιωτικές Μέθοδοι"; }
    QCString trPrivateSlots() override
    { return "Ιδιωτικές Υποδοχές"; }
    QCString trStaticPrivateMembers() override
    { return "Στατικές Ιδιωτικές Μέθοδοι"; }

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
            result+=", και ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritsList(int numEntries) override
    {
      return QCString("Κληρονομεί ")+(numEntries==1?"την ":"τις ")+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritedByList(int numEntries) override
    {
      return QCString("Κληρονομείται από ")+(numEntries==1?"την ":"τις ")+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    QCString trReimplementedFromList(int numEntries) override
    {
      return QCString("Επαναϋλοποιείται από ")+(numEntries==1?"την ":"τις ")+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    QCString trReimplementedInList(int numEntries) override
    {
      return QCString("Επαναϋλοποιείται ")+(numEntries==1?"στην ":"στις ")+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    QCString trNamespaceMembers() override
    { return "Μέλη Χώρου Ονομάτων"; }

    /*! This is an introduction to the page with all namespace members */
    QCString trNamespaceMemberDescription(bool extractAll) override
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
    QCString trNamespaceIndex() override
    { return "Ευρετήριο Χώρων Ονομάτων"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation() override
    { return "Τεκμηρίωση Χώρων Ονομάτων"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces() override
    { return "Χώροι Ονομάτων"; }

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
    QCString trReturnValues() override
    { return "Τιμές Επιστροφής"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage() override
    { return "Κύρια Σελίδα"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    QCString trPageAbbreviation() override
    { return "σελ."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    QCString trDefinedAtLineInSourceFile() override
    {
      return "Ορισμός στη γραμμή @0 του αρχείου @1.";
    }
    QCString trDefinedInSourceFile() override
    {
      return "Ορισμός στο αρχείο @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated() override
    {
      return "Αποσυρμένο";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const QCString &clName) override
    {
      return "Διάγραμμα Συνεργασίας για την κλάση "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const QCString &fName) override
    {
      return "Διάγραμμα εξάρτησης αρχείου συμπερίληψης για το "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation() override
    {
      return "Τεκμηρίωση Constructor & Destructor";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode() override
    {
      return "Πήγαινε στον πηγαίο κώδικα του αρχείου.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation() override
    {
      return "Πήγαινε στην τεκμηρίωση του αρχείου.";
    }
    /*! Text for the \\pre command */
    QCString trPrecondition() override
    {
      return "Προϋποθέσεις";
    }
    /*! Text for the \\post command */
    QCString trPostcondition() override
    {
      return "Μεταϋποθέσεις";
    }
    /*! Text for the \\invariant command */
    QCString trInvariant() override
    {
      return "Αμετάβλητα";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue() override
    {
      return "Αρχική τιμή:";
    }
    /*! Text used the source code in the file index */
    QCString trCode() override
    {
      return "κώδικας";
    }
    QCString trGraphicalHierarchy() override
    {
      return "Γραφική Αναπαράσταση Της Ιεραρχίας Των Κλάσεων";
    }
    QCString trGotoGraphicalHierarchy() override
    {
      return "Πήγαινε στην γραφική αναπαράσταση της ιεραρχίας των κλάσεων";
    }
    QCString trGotoTextualHierarchy() override
    {
      return "Πήγαινε στην με κείμενο αναπαράσταση της ιεραρχίας των κλάσεων";
    }
    QCString trPageIndex() override
    {
      return "Ευρετήριο Σελίδων";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    QCString trNote() override
    {
      return "Σημείωση";
    }
    QCString trPublicTypes() override
    {
      return "Δημόσιοι Τυποι";
    }
    QCString trPublicAttribs() override
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
    QCString trStaticPublicAttribs() override
    {
      return "Στατικά Δημόσια Χαρακτηριστικά";
    }
    QCString trProtectedTypes() override
    {
      return "Προστατευμένοι Τύποι";
    }
    QCString trProtectedAttribs() override
    {
      return "Προστατευμένα Χαρακτηριστικά";
    }
    QCString trStaticProtectedAttribs() override
    {
      return "Στατικά Προστατευμένα Χαρακτηριστικά";
    }
    QCString trPrivateTypes() override
    {
      return "Ιδιωτικοί Τύποι";
    }
    QCString trPrivateAttribs() override
    {
      return "Ιδιωτικα Χαρακτηριστικά";
    }
    QCString trStaticPrivateAttribs() override
    {
      return "Στατικά Ιδιωτικα Χαρακτηριστικά";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    QCString trTodo() override
    {
      return "Προς Υλοποίηση";
    }
    /*! Used as the header of the todo list */
    QCString trTodoList() override
    {
      return "Λίστα των Προς Υλοποίηση";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy() override
    {
      return "Αναφορά από";
    }
    QCString trRemarks() override
    {
      return "Παρατήρηση";
    }
    QCString trAttention() override
    {
      return "Προσοχή";
    }
    QCString trInclByDepGraph() override
    {
      return "Το διάγραμμα αυτό παρουσιάζει ποιά αρχεία άμεσα ή "
             "έμμεσα περιλαμβάνουν αυτό το αρχείο:";
    }
    QCString trSince() override
    {
      return "Από";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    QCString trLegendTitle() override
    {
      return "Υπόμνημα Διαγραμμάτων";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    QCString trLegendDocs() override
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
    QCString trLegend() override
    {
      return "υπόμνημα";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    QCString trTest() override
    {
      return "Δοκιμαστικό";
    }
    /*! Used as the header of the test list */
    QCString trTestList() override
    {
      return "Λίστα Δοκιμαστικών";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    QCString trProperties() override
    {
      return "Ιδιότητες";
    }
    /*! Used as a section header for IDL property documentation */
    QCString trPropertyDocumentation() override
    {
      return "Τεκμηρίωση Ιδιοτήτων";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    QCString trClasses() override
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
    QCString trPackage(const QCString &name) override
    {
      return "Πακέτο "+name;
    }
    /*! The description of the package index page */
    QCString trPackageListDescription() override
    {
      return "Ακολουθεί η λίστα των πακέτων με σύντομη περιγραφή (αν υπάρχει):";
    }
    /*! The link name in the Quick links header for each page */
    QCString trPackages() override
    {
      return "Πακέτα";
    }
    /*! Text shown before a multi-line define */
    QCString trDefineValue() override
    {
      return "Τιμή:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    QCString trBug() override
    {
      return "Πρόβλημα";
    }
    /*! Used as the header of the bug list */
    QCString trBugList() override
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
    QCString trRTFansicp() override
    {
      return "1253";
    }

    /*! Used as ansicpg for RTF fcharset
     */
    QCString trRTFCharSet() override
    {
      return "161";
    }

    /*! Used as header RTF general index */
    QCString trRTFGeneralIndex() override
    {
      return "Ευρετήριο";
    }
    QCString getLanguageString() override
    {
      return "0x408 Greece";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trClass(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "κλάση", "");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trFile(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "αρχεί", "α", "ο");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trNamespace(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "χ", "ώροι", "ώρος") +
	     " ονομάτων";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGroup(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "ομάδ", "ες", "α");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trPage(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "σελίδ", "ες", "α");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trMember(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "μέλ", "η", "ος");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGlobal(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "καθολικ", "ές", "ή");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    QCString trAuthor(bool first_capital, bool singular) override
    {
      return createNoun(first_capital,singular,"συγραφ","","έας");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    QCString trReferences() override
    {
      return "Αναφορές";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    QCString trImplementedFromList(int numEntries) override
    {
      return "Υλοποιεί "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    QCString trImplementedInList(int numEntries) override
    {
      return "Υλοποιείται από "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    QCString trRTFTableOfContents() override
    {
      return "Πίνακας Περιεχομένων";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    QCString trDeprecatedList() override
    {
      return "Λίστα Καταργημένων";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    QCString trEvents() override
    {
      return "Συμβάντα";
    }
    /*! Header used for the documentation section of a class' events. */
    QCString trEventDocumentation() override
    {
      return "Τεκμηρίωση Συμβάντων";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    QCString trPackageTypes() override
    {
      return "Τύποι Πακέτων";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    QCString trPackageFunctions() override
    {
      return "Συναρτήσεις Πακέτου";
    }
    QCString trPackageMembers() override
    {
      return "Μέλη Πακέτου";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    QCString trStaticPackageFunctions() override
    {
      return "Στατικές Συναρτήσεις Πακέτου";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    QCString trPackageAttribs() override
    {
      return "Μεταβλητές Πακέτου";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    QCString trStaticPackageAttribs() override
    {
      return "Στατικές Μεταβλητές Πακέτου";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    QCString trAll() override
    {
      return "Όλα";
    }
    /*! Put in front of the call graph for a function. */
    QCString trCallGraph() override
    {
      return "Το διάγραμμα δείχνει ποιές συναρτήσεις καλούνται από αυτή:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    QCString trSearchResultsTitle() override
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
    QCString trSearchResults(int numDocuments) override
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
    QCString trSearchMatches() override
    {
      return "Ταίριαξαν:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    QCString trSourceFile(const QCString& filename) override
    {
      return "Αρχείο κώδικα " + filename;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    QCString trDirIndex() override
    { return "Ιεραρχία Καταλόγου"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    QCString trDirDocumentation() override
    { return "Τεκμηρίωση Καταλόγου"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    QCString trDirectories() override
    { return "Κατάλογοι"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    QCString trDirReference(const QCString &dirName) override
    { QCString result=QCString("Αναφορά του Καταλόγου ") + dirName; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    QCString trDir(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "κατάλογο", "ι", "ς");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    QCString trOverloadText() override
    {
       return "Αυτή είναι μια υπερφορτωμένη συνάρτηση-μέλος, "
              "που παρέχεται για ευκολία. Διαφέρει από την παραπάνω "
              "συνάρτηση μόνο στον τύπο των παραμέτρων που δέχεται.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    QCString trCallerGraph() override
    {
      return "Το διάγραμμα δείχνει από ποιές συναρτήσεις καλείται αυτή η συνάρτηση:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    QCString trEnumerationValueDocumentation() override
    { return "Τεκμηρίωση Απαρίθμησης"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    QCString trMemberFunctionDocumentationFortran() override
    { return "Τεκμηρίωση Συνάρτησης/Υπορουτίνας Μέλους"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    QCString trCompoundListFortran() override
    { return "Λίστα Τύπων Δεδομένων"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    QCString trCompoundMembersFortran() override
    { return "Πεδία Δεδομένων"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    QCString trCompoundListDescriptionFortran() override
    { return "Εδώ είναι οι τύποι δεδομένων με σύντομη περιγραφή:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    QCString trCompoundMembersDescriptionFortran(bool extractAll) override
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
    QCString trCompoundIndexFortran() override
    { return "Περιεχόμενα Τύπων Δεδομένων"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    QCString trTypeDocumentation() override
    { return "Τεκμηρίωση Τύπων Δεδομένων"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    QCString trSubprograms() override
    { return "Συναρτήσεις/Υπορουτίνες"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    QCString trSubprogramDocumentation() override
    { return "Τεκμηρίωση Συνάρτησης/Υπορουτίνας"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     QCString trDataTypes() override
    { return "Τύποι Δεδομένων"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    QCString trModulesList() override
    { return "Λίστα Υπομονάδων"; }

    /*! used as an introduction to the modules list (Fortran) */
    QCString trModulesListDescription(bool extractAll) override
    {
      QCString result="Εδώ είναι μια λίστα με όλες τις ";
      if (!extractAll) result+="τεκμηριωμένες ";
      result+="υπομονάδες με σύντομή περιγραφή:";
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
    QCString trModuleReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" Δηλώσεις Υπομονάδων";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    QCString trModulesMembers() override
    { return "Υπομονάδες Μέλη"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    QCString trModulesMemberDescription(bool extractAll) override
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
    QCString trModulesIndex() override
    { return "Περιεχόμενα Υπομονάδων"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trModule(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "υπομονάδ", "ες", "α");
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
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
    QCString trType(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "τύπο", "ι", "ος") + " " +
             createNoun(first_capital, false, "δεδομένων", "");
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trSubprogram(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "υποπρ", "ογράμματα", "όγραμμα");
    }

    /*! C# Type Constraint list */
    QCString trTypeConstraints() override
    {
      return "Περιορισμοί Τύπων Δεδομένων";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    QCString trDirRelation(const QCString &name) override
    {
      return "Σχέση του "+QCString(name);
    }

    /*! Loading message shown when loading search results */
    QCString trLoading() override
    {
      return "Φόρτωση...";
    }

    /*! Label used for search results in the global namespace */
    QCString trGlobalNamespace() override
    {
      return "Γενικός χώρος ονομάτων";
    }

    /*! Message shown while searching */
    QCString trSearching() override
    {
      return "Αναζήτηση...";
    }

    /*! Text shown when no search results are found */
    QCString trNoMatches() override
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
    QCString trFileIn(const QCString &name) override
    {
      return "Αρχείο σε "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    QCString trIncludesFileIn(const QCString &name) override
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
    QCString trDateTime(int year,int month,int day,int dayOfWeek,
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
    QCString trDayOfWeek(int dayOfWeek, bool, bool full) override
    {
      static const char *days_short[]   = { "Δευ", "Τρι", "Τετ", "Πεμ", "Παρ", "Σαβ", "Κυρ" };
      static const char *days_full[]    = { "Δευτέρα", "Τρίτη", "Τετάρτη", "Πέμπτη", "Παρασκευή", "Σάββατο", "Κυριακή" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      return text;
    }
    QCString trMonth(int month, bool, bool full) override
    {
      static const char *months_short[] = { "Ιαν", "Φεβ", "Μαρ", "Απρ", "Μαϊ", "Ιουν", "Ιουλ", "Αυγ", "Σεπ", "Οκτ", "Νοε", "Δεκ" };
      static const char *months_full[]  = { "Ιανουάριος", "Φεβρουάριος", "Μάρτιος", "Απρίλιος", "Μάιος", "Ιούνιος", "Ιούλιος", "Αύγουστος", "Σεπτέμβριος", "Οκτώβριος", "Νοέμβριος", "Δεκέμβριος" };
      QCString text  = full? months_full[month-1] : months_short[month-1];
      return text;
    }
    QCString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "π.μ.", "μ.μ." };
      return dayPeriod[period?1:0];
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    QCString trCiteReferences() override
    { return "Αναφορές Βιβλιογραφίας"; }

    /*! Text for copyright paragraph */
    QCString trCopyright() override
    { return "Copyright"; }

    /*! Header for the graph showing the directory dependencies */
    QCString trDirDepGraph(const QCString &name) override
    { return QCString("Διάγραμμα εξάρτησης φακέλων για ")+name+":"; }


//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    QCString trDetailLevel() override
    { return "Επίπεδο λεπτομέρειας"; }

    /*! Section header for list of template parameters */
    QCString trTemplateParameters() override
    { return "Παράμετροι Προτύπου"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    QCString trAndMore(const QCString &number) override
    { return "και "+number+" ακόμη..."; }

    /*! Used file list for a Java enum */
    QCString trEnumGeneratedFromFiles(bool single) override
    { QCString result = "Η τεκμηρίωση αυτής της απαρίθμησης δημιουργήθηκε από ";
      if (single) result += "τo αρχείo"; else result += "τα αρχεία";
      result+=":";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    QCString trEnumReference(const QCString &name) override
    { return QCString("Αναφορά Απαρίθμησης ") + name; }

    /*! Used for a section containing inherited members */
    QCString trInheritedFrom(const QCString &members,const QCString &what) override
    { return QCString(members)+" κληρονόμησαν από "+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    QCString trAdditionalInheritedMembers() override
    { return "Επιπρόσθετες Κληρονομημένες Μέθοδοι"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    QCString trPanelSynchronisationTooltip(bool enable) override
    {
      QCString opt = enable ? "ενεργοποιήσετε" : "απενεργοποιήσετε";
      return "Κάντε κλικ για να "+opt+" το συγχρονισμό του παραθύρου";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    QCString trProvidedByCategory() override
    {
      return "Παρέχεται από την κατηγορία @0.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    QCString trExtendsClass() override
    {
      return "Κληρονομει την κλάση @0.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    QCString trClassMethods() override
    {
      return "Μέθοδοι Κλάσης";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    QCString trInstanceMethods() override
    {
      return "Μέθοδοι Υπόστασης";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    QCString trMethodDocumentation() override
    {
      return "Τεκμηρίωση Μεθόδου";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    QCString trInterfaces() override
    { return "Εξαγόμενες Διεπαφές"; }

    /** old style UNO IDL services: inherited services */
    QCString trServices() override
    { return "Συμπεριλαμβανόμενες Υπηρεσίες"; }

    /** UNO IDL constant groups */
    QCString trConstantGroups() override
    { return "Ομάδες Σταθερών"; }

    /** UNO IDL constant groups */
    QCString trConstantGroupReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" Τεκμηρίωση Ομάδας Σταθερών";
      return result;
    }
    /** UNO IDL service page title */
    QCString trServiceReference(const QCString &sName) override
    {
      QCString result=sName;
      result+=" Τεκμηρίωση Υπηρεσίας";
      return result;
    }
    /** UNO IDL singleton page title */
    QCString trSingletonReference(const QCString &sName) override
    {
      QCString result=sName;
      result+=" Τεκμηρίωση Μονοσύνολου";
      return result;
    }
    /** UNO IDL service page */
    QCString trServiceGeneratedFromFiles(bool single) override
    {
      // single is true implies a single file
      QCString result="Η τεκμηρίωση για την υπηρεσία αυτή "
                                "δημιουργήθηκε από ";
      if (single) result+="το ακόλουθο αρχείο:"; else result+="τα ακόλουθα αρχεία:";
      return result;
    }
    /** UNO IDL singleton page */
    QCString trSingletonGeneratedFromFiles(bool single) override
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
    QCString trDesignUnitHierarchy() override
    { return "Ιεραρχία Μονάδας Σχεδιασμού"; }
    /** VHDL design unit list */
    QCString trDesignUnitList() override
    { return "Λίστα Μονάδας Σχεδιασμού"; }
    /** VHDL design unit members */
    QCString trDesignUnitMembers() override
    { return "Μέλη Μονάδας Σχεδιασμού"; }
    /** VHDL design unit list description */
    QCString trDesignUnitListDescription() override
    {
        return "Ακολουθεί μια λίστα από τα μέλη της μονάδας σχεδιασμού με συνδέσμους στις Οντότητες στις οποίες ανήκουν:";
    }
    /** VHDL design unit index */
    QCString trDesignUnitIndex() override
    { return "Ευρετήριο Μονάδων Σχεδιασμού"; }
    /** VHDL design units */
    QCString trDesignUnits() override
    { return "Μονάδες Σχεδιασμού"; }
    /** VHDL functions/procedures/processes */
    QCString trFunctionAndProc() override
    { return "Συναρτήσεις/Διαδικασίες/Διεργασίες"; }
    /** VHDL type */
    QCString trVhdlType(VhdlSpecifier type,bool single) override
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
    QCString trCustomReference(const QCString &name) override
    { return QCString("Τεκμηρίωση ")+name; }

    /* Slice */
    QCString trConstants() override
    {
        return "Σταθερές";
    }
    QCString trConstantDocumentation() override
    {
        return "Τεκμηρίωση Σταθεράς";
    }
    QCString trSequences() override
    {
        return "Ακολουθίες";
    }
    QCString trSequenceDocumentation() override
    {
        return "Τεκμηρίωση Ακολουθίας";
    }
    QCString trDictionaries() override
    {
        return "Λεξικά";
    }
    QCString trDictionaryDocumentation() override
    {
        return "Τεκμηρίωση Λεξικού";
    }
    QCString trSliceInterfaces() override
    {
        return "Διεπαφές";
    }
    QCString trInterfaceIndex() override
    {
        return "Ευρετήριο Διεπαφής";
    }
    QCString trInterfaceList() override
    {
        return "Λίστα Διεπαφής";
    }
    QCString trInterfaceListDescription() override
    {
        return "Ακολουθούν οι διεπαφές με σύντομες περιγραφές:";
    }
    QCString trInterfaceHierarchy() override
    {
        return "Ιεραρχία Διεπαφής";
    }
    QCString trInterfaceHierarchyDescription() override
    {
        return "Αυτή η λίστα ιεραρχίας είναι ταξινομημένη χονδροειδώς και όχι αυστηρά αλφαβητικά:";
    }
    QCString trInterfaceDocumentation() override
    {
        return "Τεκμηρίωση Διεπαφής";
    }
    QCString trStructs() override
    {
        return "Δομές";
    }
    QCString trStructIndex() override
    {
        return "Ευρετήριο Δομής";
    }
    QCString trStructList() override
    {
        return "Λίστα Δομής";
    }
    QCString trStructListDescription() override
    {
        return "Ακολουθούν οι δομές με σύντομες περιγραφές:";
    }
    QCString trStructDocumentation() override
    {
        return "Τεκμηρίωση Δομής";
    }
    QCString trExceptionIndex() override
    {
        return "Ευρετήριο Εξαιρέσεων";
    }
    QCString trExceptionList() override
    {
        return "Λίστα Εξαίρεσης";
    }
    QCString trExceptionListDescription() override
    {
        return "Ακολουθούν οι εξαιρέσεις με σύντομες περιγραφές:";
    }
    QCString trExceptionHierarchy() override
    {
        return "Ιεραρχία Εξαίρεσης";
    }
    QCString trExceptionHierarchyDescription() override
    {
        return "Αυτή η λίστα ιεραρχίας είναι ταξινομημένη χονδροειδώς και όχι αυστηρά αλφαβητικά:";
    }
    QCString trExceptionDocumentation() override
    {
        return "Τεκμηρίωση Εξαίρεσης";
    }
    QCString trCompoundReferenceSlice(const QCString &clName, ClassDef::CompoundType compType, bool isLocal) override
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
    QCString trOperations() override
    {
        return "Πράξεις";
    }
    QCString trOperationDocumentation() override
    {
        return "Τεκμηρίωση Πράξης";
    }
    QCString trDataMembers() override
    {
        return "Μέλη Δεδομένων";
    }
    QCString trDataMemberDocumentation() override
    {
        return "Τεκμηρίωση Μέλους Δεδομένων";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.19
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit documentation */
    QCString trDesignUnitDocumentation() override
    { return "Τεκμηρίωση Μονάδας Σχεδιασμού"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.2
//////////////////////////////////////////////////////////////////////////

    /** C++20 concept */
    QCString trConcept(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "έννοι", "ες", "α");
    }
    /*! used as the title of the HTML page of a C++20 concept page */
    QCString trConceptReference(const QCString &conceptName) override
    {
      QCString result("Αναφορά Έννοιας ");
      result+=conceptName;
      return result;
    }

    /*! used as the title of page containing all the index of all concepts. */
    QCString trConceptList() override
    { return "Λίστα Έννοιας"; }

    /*! used as the title of chapter containing the index listing all concepts. */
    QCString trConceptIndex() override
    { return "Ευρετήριο Έννοιας"; }

    /*! used as the title of chapter containing all information about concepts. */
    QCString trConceptDocumentation() override
    { return "Τεκμηρίωση Έννοιας"; }

    /*! used as an introduction to the concept list */
    QCString trConceptListDescription(bool extractAll) override
    {
      QCString result="Ακολουθεί μια λίστα από όλες τις ";
      if (!extractAll) result+="τεκμηριωμένες ";
      result+="έννοιες με σύντομες περιγραφές:";
      return result;
    }

    /*! used to introduce the definition of the C++20 concept */
    QCString trConceptDefinition() override
    {
      return "Ορισμός Έννοιας";
    }

    /*! the compound type as used for the xrefitems */
    QCString trCompoundType(ClassDef::CompoundType compType, SrcLangExt lang) override
    {
      QCString result;
      switch(compType)
      {
        case ClassDef::Class:
          if (lang == SrcLangExt::Fortran) trType(true,true);
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

    QCString trPackageList() override
    { return "Λίστα πακέτου"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used for translation of the word that will be
     *  followed by a single name of the VHDL process flowchart.
     */
    QCString trFlowchart() override
    { return "Διάγραμμα ροής:"; }

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
    QCString trRelatedSymbols() override
    { return "Σχετικά Σύμβολα"; }

    /*! subscript for the related symbols
     *
     *  Supersedes trRelatedSubscript
     */
    QCString trRelatedSymbolsSubscript() override
    { return "(Προσέξτε ότι αυτά δεν είναι σύμβολα μέλη.)"; }

    /*! used in the class documentation as a header before the list of all
     * related classes.
     *
     * Supersedes trRelatedFunctionDocumentation
     */
    QCString trRelatedSymbolDocumentation() override
    { return "Τεκμηρίωσης Φίλιων και Σχετικών Συμβόλων"; }

    QCString trFileMembersDescriptionTotal(FileMemberHighlight::Enum hl) override
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
    QCString trCompoundMembersDescriptionTotal(ClassMemberHighlight::Enum hl) override
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
    QCString trNamespaceMembersDescriptionTotal(NamespaceMemberHighlight::Enum hl) override
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
    QCString trDefinition() override  { return "Ορισμός";}
    QCString trDeclaration() override { return "Δήλωση";}

//////////////////////////////////////////////////////////////////////////
// new since 1.9.8
//////////////////////////////////////////////////////////////////////////

    QCString trTopics() override
    { return "Θέματα"; }
    QCString trTopicDocumentation() override
    { return "Τεκμηρίωση Θέματος"; }
    QCString trTopicList() override
    { return "Λίστα Θεμάτων"; }
    QCString trTopicIndex() override
    { return "Ευρετήριο Θεμάτων"; }
    QCString trTopicListDescription() override
    { return "Μια λίστα με όλα τα θέματα με σύντομες περιγραφές:"; }
    QCString trModuleMembersDescriptionTotal(ModuleMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      QCString result="Μια λίστα με ";
      QCString singularResult = "";
      switch (hl)
      {
        case ModuleMemberHighlight::All:
          singularResult="μέλος";
          result+="όλα ";
          if (!extractAll) result+="τα τεκμηριωμένα ";
          result+="μέλη";
          break;
        case ModuleMemberHighlight::Functions:
          singularResult="συνάρτηση";
          result+="όλες ";
          if (!extractAll) result+="τις τεκμηριωμένες ";
          result+="συναρτήσεις";
          break;
        case ModuleMemberHighlight::Variables:
          singularResult="μεταβλητή";
          result+="όλες ";
          if (!extractAll) result+="τις τεκμηριωμένες ";
          result+="μεταβλητές";
          break;
        case ModuleMemberHighlight::Typedefs:
          singularResult="ορισμό τύπου";
          result+="όλους ";
          if (!extractAll) result+="τους τεκμηριωμένους ";
          result+="ορισμούς τύπων";
          break;
        case ModuleMemberHighlight::Enums:
          singularResult="απαρίθμηση";
          result+="όλες ";
          if (!extractAll) result+="τις τεκμηριωμένες ";
          result+="απαριθμήσεις";
          break;
        case ModuleMemberHighlight::EnumValues:
          singularResult="τιμή απαρίθμησης";
          result+="όλες ";
          if (!extractAll) result+="τις τεκμηριωμένες ";
          result+="τιμές απαριθμήσεων";
          break;
        case ModuleMemberHighlight::Total: // for completeness
          break;
      }
      result+=" υπομονάδων, με συνδέσμους στην ";
      if (extractAll)
        result+="τεκμηρίωση της υπομονάδας για κάθε " + singularResult + ":";
      else
        result+="υπομονάδα στην οποία ανήκουν:";
      return result;
    }
    QCString trExportedModules() override
    {
      return "Εξαγόμενες Υπομονάδες";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.10.0
//////////////////////////////////////////////////////////////////////////

    QCString trCopyToClipboard() override
    {
      return "Αντιγραφή στο πρόχειρο";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.11.0
//////////////////////////////////////////////////////////////////////////

    QCString trImportant() override
    {
      return "Σημαντικό";
    }
};

#endif
