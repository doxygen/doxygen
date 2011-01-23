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

/*
 * 15 Dec 2001 : Translation to greek by
 *               Harry Kalogirou <no email>
 *
 * 01 Jan 2009 : Greek maintainance by
 *               Paul Gessos <gessos.paul@yahoo.gr>
 */

#ifndef TRANSLATOR_GR_H
#define TRANSLATOR_GR_H

class TranslatorGreek : public Translator
{
  protected:
    friend class TranslatorAdapterBase;

  public:

    // --- Language control methods -------------------

    /*! Used for identification of the language. The identification
     * should not be translated. It should be replaced by the name
     * of the language in English using lower-case characters only
     * (e.g. "czech", "japanese", "russian", etc.). It should be equal to
     * the identification used in language.cpp.
     */
    virtual QCString idLanguage()
    { return "greek"; }

    virtual QCString latexLanguageSupportCommand()
    {
      //return "\\usepackage[greek,english]{babel}\n\\usepackage[iso-8859-7]{inputenc}\n";
      return "\\usepackage[greek,english]{babel}\n";
    }

    /*! return the language charset. This will be used for the HTML output */
    virtual QCString idLanguageCharset()
    {
        return "utf-8";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    virtual QCString trRelatedFunctions()
    { return "Σχετικές συναρτήσεις"; }

    /*! subscript for the related functions. */
    virtual QCString trRelatedSubscript()
    { return "(Σημειώστε ότι αυτές δεν είναι συναρτήσεις μέλη.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    virtual QCString trDetailedDescription()
    { return "Λεπτομερής Περιγραφή"; }

    /*! header that is put before the list of typedefs. */
    virtual QCString trMemberTypedefDocumentation()
    { return "Τεκμηρίωση Μελών Typedef"; }

    /*! header that is put before the list of enumerations. */
    virtual QCString trMemberEnumerationDocumentation()
    { return "Τεκμηρίωση Απαριθμήσεων Μελών"; }

    /*! header that is put before the list of member functions. */
    virtual QCString trMemberFunctionDocumentation()
    { return "Τεκμηρίωση Συναρτήσεων Μελών"; }

    /*! header that is put before the list of member attributes. */
    virtual QCString trMemberDataDocumentation()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Τεκμηρίωση Πεδίων";
      }
      else
      {
        return "Τεκμηρίωση Δεδομένων Μελών";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    virtual QCString trMore()
    { return "Περισσότερα..."; }

    /*! put in the class documentation */
    virtual QCString trListOfAllMembers()
    { return "Λίστα όλων των μελών."; }

    /*! used as the title of the "list of all members" page of a class */
    virtual QCString trMemberList()
    { return "Λίστα μελών"; }

    /*! this is the first part of a sentence that is followed by a class name */
    virtual QCString trThisIsTheListOfAllMembers()
    { return "Ακολουθεί η πλήρης λίστα των μελών της"; }

    /*! this is the remainder of the sentence after the class name */
    virtual QCString trIncludingInheritedMembers()
    { return ", περιλαμβανομένων όλων των κληρονομημένων μελών."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    virtual QCString trGeneratedAutomatically(const char *s)
    { QCString result="Δημιουργήθηκε αυτόματα από το Doxygen";
      if (s) result+=(QCString)" για "+s;
      result+=" από τον πηγαίο κώδικα.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    virtual QCString trEnumName()
    { return "όνομα απαρύθμισης"; }

    /*! put after an enum value in the list of all members */
    virtual QCString trEnumValue()
    { return "τιμή απαρίθμησης"; }

    /*! put after an undocumented member in the list of all members */
    virtual QCString trDefinedIn()
    { return "ορισμένο στο "; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    virtual QCString trModules()
    { return "Κομμάτια"; }

    /*! This is put above each page as a link to the class hierarchy */
    virtual QCString trClassHierarchy()
    { return "Ιεραρχία Κλάσεων"; }

    /*! This is put above each page as a link to the list of annotated classes */
    virtual QCString trCompoundList()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Δομές Δεδομένων";
      }
      else
      {
        return "Λίστα Συσσωματωμάτων";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    virtual QCString trFileList()
    { return "Λίστα Αρχείων"; }

    /*! This is put above each page as a link to all members of compounds. */
    virtual QCString trCompoundMembers()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Πεδία Δεδομένων";
      }
      else
      {
        return "Μέλη Συσσωματώματα";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    virtual QCString trFileMembers()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Καθολικοί ορισμοί";
      }
      else
      {
        return "Μέλη Αρχείων";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    virtual QCString trRelatedPages()
    { return "Σχετικές Σελίδες"; }

    /*! This is put above each page as a link to all examples. */
    virtual QCString trExamples()
    { return "Παραδείγματα"; }

    /*! This is put above each page as a link to the search engine. */
    virtual QCString trSearch()
    { return "Αναζήτηση"; }

    /*! This is an introduction to the class hierarchy. */
    virtual QCString trClassHierarchyDescription()
    { return "Αυτή η λίστα κληρονομηκότητας είναι μερικώς ταξινομημένη, "
             "αλλά όχι πλήρως, αλφαβητικά:";
    }

    /*! This is an introduction to the list with all files. */
    virtual QCString trFileListDescription(bool extractAll)
    {
      QCString result="Ακολουθεί μια λίστα όλων των ";
      if (!extractAll) result+="τεκμηριωμένων ";
      result+="αρχείων με σύντομες περιγραφές:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    virtual QCString trCompoundListDescription()
    {

      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Ακολουθούνε οι δομές δεδομένων με σύντομες περιγραφές:";
      }
      else
      {
        return "Ακολουθούνε οι κλάσεις, οι δομές, "
               "τα σώματα και οι διαπροσωπίες με σύντομες περιγραφές:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    virtual QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="Ακολουθεί η λίστα όλων των ";
      if (!extractAll)
      {
        result+="τεκμηριωμένων ";
      }
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        result+="πεδίων δομών και σωμάτων ";
      }
      else
      {
        result+="κλάσεων";
      }
      result+=" με συνδέσμους ";
      if (!extractAll)
      {
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
        if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Ακολουθεί η λίστα όλων των ";
      if (!extractAll) result+="τεκμηριωμένων ";

      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trExamplesDescription()
    { return "Ακολουθεί η λίστα με τα παραδείγματα:"; }

    /*! This is an introduction to the page with the list of related pages */
    virtual QCString trRelatedPagesDescription()
    { return "Ακολουθεί η λίστα όλων των σχετικών σελίδων τεκμηρίωσης:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    virtual QCString trModulesDescription()
    { return "Ακολουθεί η λίστα όλων των μονάδων:"; }

    // index titles (the project name is prepended for these)


    /*! This is used in HTML as the title of index.html. */
    virtual QCString trDocumentation()
    { return "Τεκμηρίωση"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    virtual QCString trModuleIndex()
    { return "Ευρετήριο μονάδων"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    virtual QCString trHierarchicalIndex()
    { return "Ιεραρχικό Ευρετήριο"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    virtual QCString trCompoundIndex()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
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
    virtual QCString trFileIndex()
    { return "Ευρετήτιο Αρχείων"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    virtual QCString trModuleDocumentation()
    { return "Τεκμηρίωση Μονάδων"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    virtual QCString trClassDocumentation()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Τεκμηρίωση Δομών Δεδομένων";
      }
      else
      {
        return "Τεκμηρίωση Κλάσεων";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    virtual QCString trFileDocumentation()
    { return "Τεκμηρίωση Αρχείων"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all examples.
     */
    virtual QCString trExampleDocumentation()
    { return "Τεκμηρίωση Παραδειγμάτων"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all related pages.
     */
    virtual QCString trPageDocumentation()
    { return "Τεκμηρίωση Σελίδων"; }

    /*! This is used in LaTeX as the title of the document */
    virtual QCString trReferenceManual()
    { return "Ενχειρίδιο Αναφοράς"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    virtual QCString trDefines()
    { return "Ορισμοί"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of function prototypes
     */
    virtual QCString trFuncProtos()
    { return "Πρωτοτυπήσεις Συναρτήσεων"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    virtual QCString trTypedefs()
    { return "Ορισμοί Τύπων"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    virtual QCString trEnumerations()
    { return "Απαριθμήσεις"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    virtual QCString trFunctions()
    { return "Συναρτήσεις"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trVariables()
    { return "Μεταβλητές"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    virtual QCString trEnumerationValues()
    { return "Τιμές Απαριθμήσεων"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    virtual QCString trDefineDocumentation()
    { return "Τεκμηρίωση Ορισμών"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for function prototypes
     */
    virtual QCString trFunctionPrototypeDocumentation()
    { return "Τεκμηρίωση Πρωτοτυπήσεων των Συναρτήσεων"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    virtual QCString trTypedefDocumentation()
    { return "Τεκμηρίωση Ορισμών Τύπων"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    virtual QCString trEnumerationTypeDocumentation()
    { return "Τεκμηρίωση Απαριθμήσεων"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    virtual QCString trFunctionDocumentation()
    { return "Τεκμηρίωση Συναρτήσεων"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    virtual QCString trVariableDocumentation()
    { return "Τεκμηρίωση Μεταβλητών"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    virtual QCString trCompounds()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Δομές Δεδομένων";
      }
      else
      {
        return "Συσσωματώματα";
      }
    }

    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    virtual QCString trGeneratedAt(const char *date,const char *projName)
    {
      QCString result=(QCString)"Δημιουργήθηκε στις "+date;
      if (projName) result+=(QCString)" για "+projName;
      result+=(QCString)" από";
      return result;
    }
    /*! This is part of the sentence used in the standard footer of each page.
     */
    virtual QCString trWrittenBy()
    {
      return "γραμμένο από τον ";
    }

    /*! this text is put before a class diagram */
    virtual QCString trClassDiagram(const char *clName)
    {
      return (QCString)"Διάγραμμα κληρονομικότητας για την "+clName+":";
    }

    /*! this text is generated when the \\internal command is used. */
    virtual QCString trForInternalUseOnly()
    { return "Μόνο για εσωτερική χρήση."; }

    /*! this text is generated when the \\warning command is used. */
    virtual QCString trWarning()
    { return "Προειδοποίηση"; }

    /*! this text is generated when the \\version command is used. */
    virtual QCString trVersion()
    { return "Έκδοση"; }

    /*! this text is generated when the \\date command is used. */
    virtual QCString trDate()
    { return "Ημ/νια"; }

    /*! this text is generated when the \\return command is used. */
    virtual QCString trReturns()
    { return "Επιστρέφει"; }

    /*! this text is generated when the \\sa command is used. */
    virtual QCString trSeeAlso()
    { return "Κοιτάξτε επίσης "; }

    /*! this text is generated when the \\param command is used. */
    virtual QCString trParameters()
    { return "Παράμετροι"; }

    /*! this text is generated when the \\exception command is used. */
    virtual QCString trExceptions()
    { return "Εξαίρεση"; }

    /*! this text is used in the title page of a LaTeX document. */
    virtual QCString trGeneratedBy()
    { return "Δημιουργήθηκε από "; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    virtual QCString trNamespaceList()
    { return "Λίστα Namespace"; }

    /*! used as an introduction to the namespace list */
    virtual QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Ακολουθέι η λίστα όλων των ";
      if (!extractAll) result+="τεκμηριωμένων ";
      result+="Namespace με σύντομες περιγραφές:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    virtual QCString trFriends()
    { return "Φίλοι"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    virtual QCString trRelatedFunctionDocumentation()
    { return "Τεκμηρίωση Φιλικών και Συσχετιζόμενων Συναρτήσεων"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    virtual QCString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate)
    {
      QCString result=(QCString)clName+" ";
      result+=" Αναφορά";
      switch(compType)
      {
        case ClassDef::Class:      result+=" Κλάσης"; break;
        case ClassDef::Struct:     result+=" Δομής"; break;
        case ClassDef::Union:      result+=" Ένωσης"; break;
        case ClassDef::Interface:  result+=" Διαπροσωπίας"; break;
        case ClassDef::Protocol:   result+=" Πρωτοκόλλου"; break;
        case ClassDef::Category:   result+=" Κατηγορίας"; break;
        case ClassDef::Exception:  result+=" Εξαίρεσης"; break;
      }
      if (isTemplate) result+=" Template";

      return result;
    }

    /*! used as the title of the HTML page of a file */
    virtual QCString trFileReference(const char *fileName)
    {
      QCString result=fileName;
      result+=" Αναφορά Αρχείου";
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    virtual QCString trNamespaceReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" Αναφορά Namespace";
      return result;
    }

    virtual QCString trPublicMembers()
    { return "Δημόσιες Μέθοδοι"; }
    virtual QCString trPublicSlots()
    { return "Δημόσια Slots"; }
    virtual QCString trSignals()
    { return "Σήματα"; }
    virtual QCString trStaticPublicMembers()
    { return "Στατικές Δημόσιες Μέδοδοι"; }
    virtual QCString trProtectedMembers()
    { return "Προστατευμένες Μέδοδοι"; }
    virtual QCString trProtectedSlots()
    { return "Προστατευμένα Slots"; }
    virtual QCString trStaticProtectedMembers()
    { return "Στατικές Προστατευμένες Μέδοδοι"; }
    virtual QCString trPrivateMembers()
    { return "Ιδιοτικές Μέδοδοι"; }
    virtual QCString trPrivateSlots()
    { return "Ιδιοτικά Slots"; }
    virtual QCString trStaticPrivateMembers()
    { return "Στατικές Ιδιοτικές Μέδοδοι"; }

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
            result+=", και ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritsList(int numEntries)
    {
      return "Κληρονομεί "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    virtual QCString trInheritedByList(int numEntries)
    {
      return "Κληρονομείται από "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    virtual QCString trReimplementedFromList(int numEntries)
    {
      return "Επαναϋλοποιείται από "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    virtual QCString trReimplementedInList(int numEntries)
    {
      return "Επαναϋλοποιείται στην "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    virtual QCString trNamespaceMembers()
    { return "Μέλη Namespace"; }

    /*! This is an introduction to the page with all namespace members */
    virtual QCString trNamespaceMemberDescription(bool extractAll)
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
    virtual QCString trNamespaceIndex()
    { return "Ευρετήριο Χώρων Ονομάτων"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    virtual QCString trNamespaceDocumentation()
    { return "Τεκμηρίωση Χώρων Ονομάτων"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    virtual QCString trNamespaces()
    { return "Χώροι Ονομάτων"; }

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
      QCString result=(QCString)"Η τεκμηρίωση για ";
      switch(compType)
      {
        case ClassDef::Class:      result+="αυτή την κλάση"; break;
        case ClassDef::Struct:     result+="αυτή η δομή"; break;
        case ClassDef::Union:      result+="αυτή η ένωση"; break;
        case ClassDef::Interface:  result+="αυτή η διαπροσωπία"; break;
        case ClassDef::Protocol:   result+="Πρωτόκολλο"; break;
        case ClassDef::Category:   result+="Κατηγορία"; break;
        case ClassDef::Exception:  result+="αυτή η εξαίρεση"; break;
      }
      result+=" δημιουργήθηκε απο τ";
      if (single) result+="ο ακόλουθο αρχείο:"; else result+="α ακόλουθα αρχεία:";
      return result;
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    virtual QCString trAlphabeticalList()
    { return "Αλφαβητική Λίστα"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    virtual QCString trReturnValues()
    { return "Τιμές Επιστροφής"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    virtual QCString trMainPage()
    { return "Κύρια Σελίδα"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    virtual QCString trPageAbbreviation()
    { return "σελ."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDefinedAtLineInSourceFile()
    {
      return "Ορισμός στη γραμμή @0 του αρχείου @1.";
    }
    virtual QCString trDefinedInSourceFile()
    {
      return "Ορισμός στο αρχείο @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    virtual QCString trDeprecated()
    {
      return "Αποσυρμένο";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    virtual QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)"Δίαγραμμα Συνεργασίας για την κλάση "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    virtual QCString trInclDepGraph(const char *fName)
    {
      return (QCString)"Διάγραμμα εξάρτησης Include για το αρχείο "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    virtual QCString trConstructorDocumentation()
    {
      return "Τεκμηρίωση Constructor & Destructor";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    virtual QCString trGotoSourceCode()
    {
      return "Πήγαινε στον πηγαίο κώδικα του αρχείου.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    virtual QCString trGotoDocumentation()
    {
      return "Πήγαινε στην τεκμηρίωση του αρχείου.";
    }
    /*! Text for the \\pre command */
    virtual QCString trPrecondition()
    {
      return "Προϋποθέσεις";
    }
    /*! Text for the \\post command */
    virtual QCString trPostcondition()
    {
      return "Μεταϋποθέσεις";
    }
    /*! Text for the \\invariant command */
    virtual QCString trInvariant()
    {
      return "Αμετάβλητα";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    virtual QCString trInitialValue()
    {
      return "Αρχική τιμή:";
    }
    /*! Text used the source code in the file index */
    virtual QCString trCode()
    {
      return "κώδικας";
    }
    virtual QCString trGraphicalHierarchy()
    {
      return "Γραφική Αναπαράσταση Της Ιεραρχίας Των Κλάσεων";
    }
    virtual QCString trGotoGraphicalHierarchy()
    {
      return "Πήγαινε στην γραφική αναπαράσταση της ιεραρχίας των κλάσεων";
    }
    virtual QCString trGotoTextualHierarchy()
    {
      return "Πήγαινε στην με κείμενο αναπαράσταση της ιεραρχίας των κλάσεων";
    }
    virtual QCString trPageIndex()
    {
      return "Ευρετήριο Σελίδων";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    virtual QCString trNote()
    {
      return "Σημείωση";
    }
    virtual QCString trPublicTypes()
    {
      return "Δημόσιοι Τυποι";
    }
    virtual QCString trPublicAttribs()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Πεδία Δεδομένων";
      }
      else
      {
        return "Δημόσια Χαρακτηριστικά";
      }
    }
    virtual QCString trStaticPublicAttribs()
    {
      return "Στατικά Δημόσια Χαρακτηριστικά";
    }
    virtual QCString trProtectedTypes()
    {
      return "Προστατευμένοι Τύποι";
    }
    virtual QCString trProtectedAttribs()
    {
      return "Προστατευμένα Χαρακτηριστικά";
    }
    virtual QCString trStaticProtectedAttribs()
    {
      return "Στατικά Προστατευμένα Χαρακτηριστικά";
    }
    virtual QCString trPrivateTypes()
    {
      return "Ιδιωτικοί Τύποι";
    }
    virtual QCString trPrivateAttribs()
    {
      return "Ιδιωτικα Χαρακτηριστικά";
    }
    virtual QCString trStaticPrivateAttribs()
    {
      return "Στατικά Ιδιωτικα Χαρακτηριστικά";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    virtual QCString trTodo()
    {
      return "Προς Υλοποίηση";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    {
      return "Λίστα των Προς Υλοποίηση";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
      return "Αναφορά από";
    }
    virtual QCString trRemarks()
    {
      return "Παρατήρηση";
    }
    virtual QCString trAttention()
    {
      return "Προσοχή";
    }
    virtual QCString trInclByDepGraph()
    {
      return "Το γράφημα αυτό παρουσιάζει ποιά αρχεία άμεσα ή "
             "έμεσα περιλαμβάνουν αυτό το αρχείο:";
    }
    virtual QCString trSince()
    {
      return "Από";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    {
      return "Υπόμνημα Διαγραμμάτων";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    virtual QCString trLegendDocs()
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
        "Αυτό οδηγεί στο επόμενο γράφημα:"
        "<p><center><img alt=\"\" src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center></p>\n"
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
        "<li>Ένα κίτρινο διακεκομμένο βέλος χρησιμοποιείται για μια σχέση μεταξύ ενός template αντικειμένου και "
        "της template κλάσης από την οποία δημιουργήθηκε. Το βέλος ονομάζεται με τις παραμέτρους του template "
        "του αντικειμένου.</li>\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    virtual QCString trLegend()
    {
      return "υπόμνημα";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    virtual QCString trTest()
    {
      return "Δοκιμαστικό";
    }
    /*! Used as the header of the test list */
    virtual QCString trTestList()
    {
      return "Λίστα Δοκιμαστικών";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for KDE-2 IDL methods */
    virtual QCString trDCOPMethods()
    {
      return "Μέδοδοι DCOP";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties()
    {
      return "Ιδιότητες";
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
      return "Τεκμηρίωση Ιδιοτήτων";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      {
        return "Δομές Δεδομένων";
      }
      else
      {
        return "Κλάσεις";
      }
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const char *name)
    {
      return (QCString)"Πακέτο "+name;
    }
    /*! Title of the package index page */
    virtual QCString trPackageList()
    {
      return "Λίστα Πακέτων";
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return "Ακολουθεί η λίστα των πακέτων με σύντομη περιγραφή (αν υπάρχει):";
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return "Πακέτα";
    }
    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue()
    {
      return "Τιμή:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug()
    {
      return "Πρόβλημα";
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList()
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
    virtual QCString trRTFansicp()
    {
      return "1253";
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
      return "Ευρετήριο";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Κλάση" : "κλάση"));
      if (!singular)  result+="";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trFile(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Αρχεί" : "αρχεί"));
      if (!singular)  result+="α"; else result+="ο";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Namespace" : "namespace"));
      if (!singular)  result+="s";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Ομάδ" : "ομάδ"));
      if (!singular)  result+="ες"; else result+="α";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Σελίδ" : "σελίδ"));
      if (!singular)  result+="ες"; else result+="α";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Μέλ" : "μέλ"));
      if (!singular)  result+="η"; else result+="ος";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGlobal(bool first_capital, bool singular)
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
    virtual QCString trAuthor(bool first_capital, bool singular)
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
    virtual QCString trReferences()
    {
      return "Αναφορές";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    {
      return "Υλοποιεί "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      return "Υλοποιείται από "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      return "Πίνακας Περιεχομένων";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    virtual QCString trDeprecatedList()
    {
      return "Λίστα Καταργημένων";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    virtual QCString trEvents()
    {
      return "Συμβάντα";
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      return "Τεκμηρίωση Συμβάντων";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    {
      return "Τύποι Πακέτων";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    virtual QCString trPackageMembers()
    {
      return "Συναρτήσεις Πακέτου";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    {
      return "Στατικές Συναρτήσεις Πακέτου";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    virtual QCString trPackageAttribs()
    {
      return "Μεταβλητές Πακέτου";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    {
      return "Στατικές Μεταβλητές Πακέτου";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll()
    {
      return "Όλα";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph()
    {
      return "Το γράφημα δείχνει ποιές συναρτήσεις καλούνται από αυτή:";
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
      return "Αναζήτηση για";
    }
    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle()
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
    virtual QCString trSearchResults(int numDocuments)
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
    virtual QCString trSearchMatches()
    {
      return "Ταίριαξαν:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename)
    {
      return "Αρχείο κώδικα " + filename;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex()
    { return "Ιεραρχία Καταλόγου"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation()
    { return "Τεκμηρίωση Καταλόγου"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories()
    { return "Κατάλογοι"; }

    /*! This returns a sentences that introduces the directory hierarchy.
     *  and the fact that it is sorted alphabetically per level
     */
    virtual QCString trDirDescription()
    {
			return "Η ιεραρχία καταλόγων ταξινομήθηκε αλφαβητικά, αλλά όχι πολύ αυστηρά:";
    }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const char *dirName)
    { QCString result=QCString("Αναφορά του Καταλόγου ") + dirName; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool first_capital, bool singular)
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
    virtual QCString trOverloadText()
    {
       return "Αυτή είναι μια υπερφορτωμένη συνάρτηση-μέλος, "
              "που παρέχεται για ευκολία. Διαφέρει από την παραπάνω "
              "συνάρτηση μόνο στον τύπο των παραμέτρων που δέχεται.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    virtual QCString trCallerGraph()
    {
      return "Το γράφημα δείχνει από ποιές συναρτήσεις καλείται αυτή η συνάρτηση:";
    }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return "Enumerator Documentation"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran()
    { return "Τεκμηρίωση Συνάρτησης/Υπορουτίνας Μέλους"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    virtual QCString trCompoundListFortran()
    { return "Λίστα Τύπων Δεδομένων"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran()
    { return "Πεδία Δεδομένων"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran()
    { return "Εδώ είναι οι τύποι δεδομένων με σύντομη περιγραφή:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll)
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
    virtual QCString trCompoundIndexFortran()
    { return "Περιεχόμενα Τύπων Δεδομένων"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation()
    { return "Τεκμηρίωση Τύπων Δεδομένων"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms()
    { return "Συναρτήσεις/Υπορουτίνες"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation()
    { return "Τεκμηρίωση Συνάρτησης/Υπορουτίνας"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes()
    { return "Τύποι Δεδομένων"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList()
    { return "Λίστα Υπομονάδων"; }

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll)
    {
      QCString result="Εδώ είναι μια λίστα με όλες τις ";
      if (!extractAll) result+="τεκμηριωμένες ";
      result+="υπομονάδες με σύντομή περιγραφή:";
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
        case ClassDef::Class:      result+=" Υπομονάδα"; break;
        case ClassDef::Struct:     result+=" Τύπος"; break;
        case ClassDef::Union:      result+=" Ένωση"; break;
        case ClassDef::Interface:  result+=" Interface"; break;
        case ClassDef::Protocol:   result+=" Πρωτόκολλο"; break;
        case ClassDef::Category:   result+=" Κατηγορία"; break;
        case ClassDef::Exception:  result+=" Εξαίρεση"; break;
      }
      if (isTemplate) result+=" Πρότυπο";
      result+=" Δήλωση";
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    virtual QCString trModuleReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" Δηλώσεις Υπομονάδων";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers()
    { return "Υπομονάδες Μέλη"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll)
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
    virtual QCString trModulesIndex()
    { return "Περιεχόμενα Υπομονάδων"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trModule(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Υπομονάδ" : "υπομονάδ"));
      if (!singular)  result+="ες"; else result+="α";
      return result;
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single)
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      QCString result=(QCString)"Η τεκμηρίωση για ";
      switch(compType)
      {
        case ClassDef::Class:      result+="αυτή την υπομονάδα"; break;
        case ClassDef::Struct:     result+="αυτό τον τύπο δεδομένων"; break;
        case ClassDef::Union:      result+="αυτή την ένωση"; break;
        case ClassDef::Interface:  result+="αυτό το interface"; break;
        case ClassDef::Protocol:   result+="αυτό το πρωτόκολλο"; break;
        case ClassDef::Category:   result+="αυτή την κατηγορία"; break;
        case ClassDef::Exception:  result+="αυτή την εξαίρεση"; break;
      }
      result+=" δημιουργήθηκε από ";
      if (single) result+="το παρακάτω αρχείο:"; else result+="τα παρακάτω αρχεία:";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trType(bool first_capital, bool singular)
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
    virtual QCString trSubprogram(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Υποπρ" : "υποπρ"));
      if (!singular)  result+="ογράμματα"; else result+="όγραμμα";
      return result;
    }

    /*! C# Type Constraint list */
    virtual QCString trTypeConstraints()
    {
      return "Περιορισμοί Τύπων Δεδομένων";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    virtual QCString trDirRelation(const char *name)
    {
      return "Σχέση του "+QCString(name);
    }

    /*! Loading message shown when loading search results */
    virtual QCString trLoading()
    {
      return "Φόρτωση...";
    }

    /*! Label used for search results in the global namespace */
    virtual QCString trGlobalNamespace()
    {
      return "Γενικός χώρος ονομάτων";
    }

    /*! Message shown while searching */
    virtual QCString trSearching()
    {
      return "Αναζήτηση...";
    }

    /*! Text shown when no search results are found */
    virtual QCString trNoMatches()
    {
      return "Δεν βρέθηκαν αποτελέσματα αναζήτησης";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.3 (missing items for the directory pages)
//////////////////////////////////////////////////////////////////////////

    /*! introduction text for the directory dependency graph */
    virtual QCString trDirDependency(const char *name)
    {
      return (QCString)"Γράφημα εξάρτησης φακέλου για το "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the first column mentions the
     *  source file that has a relation to another file.
     */
    virtual QCString trFileIn(const char *name)
    {
      return (QCString)"Αρχείο σε "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    virtual QCString trIncludesFileIn(const char *name)
    {
      return (QCString)"Εσωκλείει το αρχείο στο "+name;
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
      static const char *days[]   = { "Δευ","Τρι","Τετ","Πεμ","Παρ","Σαβ","Κυρ" };
      static const char *months[] = { "Ιαν","Φεβ","Μαρ","Απρ","Μαι","Ιουν","Ιουλ","Αυγ","Σεπ","Οκτ","Νοε","Δεκ" };
      QCString sdate;
      sdate.sprintf("%s %.2d %s %d",days[dayOfWeek-1],day,months[month-1],year);
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
