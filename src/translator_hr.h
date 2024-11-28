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
// translation by Boris Bralo <boris.bralo@gmail.com>
// Updates:
// --------
// 2000/08/20
//  - Better handling of ISO-8859-2/ WIN 1250 stuff based on (actually stolen from :-)) Czech translations
//    implemented by Petr Prikryl (prikrylp@skil.cz).
//    As opposed to Czech translation this one assumes that Doxygen strings are written in Linux ( it's true,
//    I don't have QT pro licence ) , and uses ISOToWin function when built in WIN32
//
// 2000/09/18
// - Added strings from 1.2.1
// - Removed unneeeded decode() calls
// - Changed some CS terminology
//
// 2001/01/22
// - Added strings from 1.2.4
//
// 2001/05/25
// - Added strings and up to and including 1.2.7_20010524
// - Removed obsolete method trFiles()
// - Removed obsolete method trAuthor()
// - Removed obsolete method trVerbatimHeadert()
// - Method latexBabelPackage() removed, ude latexLanguageSupportCommand
//
// 2001/11/13
// - inherits from Translator
// - Added strings for 1.2.11
// - better output for C documentation (trCompoundMembersDescription(), trClassDocumentation())
//
// 2001/11/13
// - Added strings for 1.2.13
//
// 2003/02/26
// - Added strings for 1.2.18
//
// 2003/04/29
// - Added strings for 1.3.0
//
// 2004/06/21
// - Added strings for 1.3.8
//
// 2004/09/15
// - Added strings for 1.3.9
//
// 2005/02/28
// - Removed obsolete (unused) methods
//
// 2005/03/21
// - Added strings for 1.4.1
//
// 2006/06/11
// - Added strings for 1.4.6
//
// 2009/01/09
// - Updated trLegendDocs
//
// 2010/03/04
// - Updates for "new since 1.6.0 (mainly for the new search engine)".
// - UTF-8
// - removed decode()
//
// 2010/05/27
// - Updates for 1.6.3
// 2012/04/12
// - Updates for 1.8.0
#ifndef TRANSLATOR_HR_H
#define TRANSLATOR_HR_H

class TranslatorCroatian : public TranslatorAdapter_1_8_2
{
  private:

  public:
    QCString idLanguage() override
    { return "croatian"; }
    QCString latexLanguageSupportCommand() override
    { return "\\usepackage[croatian]{babel}\n"; }
    QCString trISOLang() override
    { return "hr"; }
    QCString getLanguageString() override
    {
      return "0x41A Croatian";
    }
    QCString trRelatedFunctions() override
    { return "Povezane funkcije"; }
    QCString trRelatedSubscript() override
    { return "(To nisu member funkcije.)"; }
    QCString trDetailedDescription() override
    { return "Detaljno objašnjenje"; }
    QCString trDetails() override
    { return "Detalji"; }
    QCString trMemberTypedefDocumentation() override
    { return "Dokumentacija typedef članova"; }
    QCString trMemberEnumerationDocumentation() override
    { return "Dokumentacija enumeracijskih članova"; }
    QCString trMemberFunctionDocumentation() override
    { return "Dokumentacija funkcija"; }
    QCString trMemberDataDocumentation() override
    { return "Documentacija varijabli"; }
    QCString trMore() override
    { return "Opširnije..."; }
    QCString trListOfAllMembers() override
    { return "Popis svih članova"; }
    QCString trMemberList() override
    { return "Popis članova."; }
    QCString trThisIsTheListOfAllMembers() override
    { return "Ovo je popis svih članova"; }
    QCString trIncludingInheritedMembers() override
    { return ", uključujući naslijeđene članove."; }
    QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="napravljeno automatski Doxygen-om";
      if (!s.isEmpty()) result+=" za "+s;
      result+=" iz programskog koda.";
      return result;
    }
    QCString trEnumName() override
    { return "enum ime"; }
    QCString trEnumValue() override
    { return "enum vrijednost"; }
    QCString trDefinedIn() override
    { return "definirano u"; }
    QCString trModules() override
    { return "Moduli"; }
    QCString trClassHierarchy() override
    { return "Stablo klasa"; }
    QCString trCompoundList() override
	{
		if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
		{
			return "Sve strukture";
		}
		else
		{
			return "Sve klase";
		}
	}
    QCString trFileList() override
    { return "Popis datoteka"; }
    QCString trCompoundMembers() override
	{
		if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
			return "Svi članovi struktura";
		else
			return "Svi članovi klasa";
	}
    QCString trFileMembers() override
    { return "članovi klasa u datoteci"; }
    QCString trRelatedPages() override
    { return "Stranice povezane s ovom"; }
    QCString trExamples() override
    { return "Primjeri"; }
    QCString trSearch() override
    { return "Traži"; }
    QCString trClassHierarchyDescription() override
    { return "Stablo naslijeđivanja je složeno "
             "približno po abecedi:";
    }
    QCString trFileListDescription(bool extractAll) override
    {
      QCString result="Popis svih ";
      if (!extractAll) result+="dokumentiranih ";
      result+="datoteka, s kratkim opisom:";
      return result;
    }
    QCString trCompoundListDescription() override
    { return "Popis svih klasa, unija i struktura "
		  "s kratkim opisom :";
    }
    QCString trCompoundMembersDescription(bool extractAll) override
    {
      QCString result="Popis svih ";
      if (!extractAll)
		  result+="dokumentiranih ";

	  if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
		  result+="članova klasa s linkovima na ";
	  else
		  result+="članova struktura s linkovima na ";

      if (!extractAll)
	  {
		  result+="dokumentaciju svakog člana:";
      }
	  else
	  {
		  if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
			  result+="dokumentaciju klase :";
		  else
			  result +="dokumentaciju strukture";
	  }
      return result;
    }
    QCString trFileMembersDescription(bool extractAll) override
    {
      QCString result="Popis svih ";
      if (!extractAll)
		  result+="dokumentiranih ";
      result+="članova s linkovima na ";
      if (extractAll)
		  result+="dokumentaciju datoteke u kojima se nalaze:";
      else
		  result+="datoteke u kojima se nalaze:";
      return result;
    }
    QCString trExamplesDescription() override
    { return "Popis primjera :"; }
    QCString trRelatedPagesDescription() override
    { return "Popis povezanih stranica:"; }
    QCString trModulesDescription() override
    { return "Popis svih modula:"; }

    QCString trDocumentation(const QCString &projName) override
    { return (!projName.isEmpty()?projName + " " : "") + "Dokumentacija"; }
    QCString trModuleIndex() override
    { return "Kazalo modula"; }
    QCString trHierarchicalIndex() override
    { return "Hijerarhijsko kazalo"; }
    QCString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Kazalo struktura podataka";
      }
      else
      {
	return "Skupno kazalo ";
      }
    }
    QCString trFileIndex() override
    { return "Kazalo datoteka"; }
    QCString trModuleDocumentation() override
    { return "Dokumentacija modula"; }
    QCString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Dokumentacija struktura podataka";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
        return trDesignUnitDocumentation();
      }
      else
      {
        return "Dokumentacija klasa";
      }
    }
    QCString trFileDocumentation() override
    { return "Dokumentacija datoteka"; }
    QCString trReferenceManual() override
    { return "Priručnik"; }

    QCString trDefines() override
    { return "Definicije"; }
    QCString trTypedefs() override
    { return "Typedef-ovi"; }
    QCString trEnumerations() override
    { return "Enumeracije"; }
    QCString trFunctions() override
    { return "Funkcije"; }
    QCString trVariables() override
    { return "Varijable"; }
    QCString trEnumerationValues() override
    { return "Vrijednosti enumeracija"; }
    QCString trDefineDocumentation() override
    { return "Dokumentacija definicija"; }
    QCString trTypedefDocumentation() override
    { return "Dokumentacija typedef-a"; }
    QCString trEnumerationTypeDocumentation() override
    { return "Dokumentacija enumeracijskog tipa"; }
    QCString trFunctionDocumentation() override
    { return "Dokumentacije funkcija"; }
    QCString trVariableDocumentation() override
    { return "Dokumentacija varijable"; }
    QCString trCompounds() override
    { return "Strukture"; }
    QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result="Napravljeno "+date;
      if (!projName.isEmpty()) result+=" projekt: "+projName;
      result+=" generator: ";
      return result;
    }
    QCString trClassDiagram(const QCString &clName) override
    {
      return QCString("Dijagram klasa za ")+clName;
    }
    QCString trWarning() override
    { return "Upozorenje"; }
    QCString trVersion() override
    { return "Verzija"; }
    QCString trDate() override
    { return "Datum"; }
    QCString trReturns() override
    { return "Povratne vrijednosti"; }
    QCString trSeeAlso() override
    { return "Vidi također"; }
    QCString trParameters() override
    { return "Parametri"; }
    QCString trExceptions() override
    { return "Iznimke"; }
    QCString trGeneratedBy() override
    { return "Generirao"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    QCString trNamespaceList() override
    { return "Popis imenika"; }
    QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="Popis svih ";
      if (!extractAll) result+="dokumentiranih ";
      result+="imenika s kratkim opisom:";
      return result;
    }
    QCString trFriends() override
    { return "Friend-ovi "; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    QCString trRelatedFunctionDocumentation() override
    { return "Dokumentacija povezanih funkcija"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    QCString trCompoundReference(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool /*isTemplate*/) override
      // used as the title of the HTML page of a class/struct/union
    {
      QCString result="Opis ";
      switch(compType)
      {
        case ClassDef::Class:  result+=" klase "; break;
        case ClassDef::Struct: result+=" strukture "; break;
        case ClassDef::Union:  result+=" unije "; break;
        case ClassDef::Interface:  result+=" sučelja (interface) "; break;
        case ClassDef::Protocol:   result+=" protokola "; break;
        case ClassDef::Category:   result+=" kategorije "; break;
        case ClassDef::Exception:  result+=" iznimke (exception) "; break;
        default: break;
      }
      result += clName;
      return result;
    }
    QCString trFileReference(const QCString &fileName) override
      // used as the title of the HTML page of a file
    {
      QCString result="Opis datoteke ";
          result+=fileName;
          return result;
    }
    QCString trNamespaceReference(const QCString &namespaceName) override
      // used as the title of the HTML page of a namespace
    {
      QCString result ="Opis imenika ";
          result+=namespaceName;

      return result;
    }

    // these are for the member sections of a class, struct or union
    QCString trPublicMembers() override
    { return "Public članovi"; }
    QCString trPublicSlots() override
    { return "Public slotovi"; }
    QCString trSignals() override
    { return "Signali"; }
    QCString trStaticPublicMembers() override
    { return "Static public članovi"; }
    QCString trProtectedMembers() override
    { return "Protected članovi"; }
    QCString trProtectedSlots() override
    { return "Protected slotovi"; }
    QCString trStaticProtectedMembers() override
    { return "Static protected članovi"; }
    QCString trPrivateMembers() override
    { return "Privatni članovi"; }
    QCString trPrivateSlots() override
    { return "Privatni slotovi"; }
    QCString trStaticPrivateMembers() override
    { return "Statični privatni članovi"; }
    // end of member sections

    QCString trWriteList(int numEntries) override
    {
      // this function is used to produce a comma-separated list of items.
      // use generateMarker(i) to indicate where item i should be put.
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
            result+="";
        }
      }
      return result;
    }

    QCString trInheritsList(int numEntries) override
      // used in class documentation to produce a list of base classes,
      // if class diagrams are disabled.
    {
      return "Naslijeđuje od "+trWriteList(numEntries)+".";
    }
    QCString trInheritedByList(int numEntries) override
      // used in class documentation to produce a list of super classes,
      // if class diagrams are disabled.
    {
      return "Naslijeđena u "+trWriteList(numEntries)+".";
    }
    QCString trReimplementedFromList(int numEntries) override
      // used in member documentation blocks to produce a list of
      // members that are hidden by this one.
    {
      return "Reimplementirano od  "+trWriteList(numEntries)+".";
    }
    QCString trReimplementedInList(int numEntries) override
    {
      // used in member documentation blocks to produce a list of
      // all member that overwrite the implementation of this member.
      return "Reimplementacija u "+trWriteList(numEntries)+".";
    }

    QCString trNamespaceMembers() override
      // This is put above each page as a link to all members of namespaces.
    { return "članovi imenika"; }
    QCString trNamespaceMemberDescription(bool extractAll) override
      // This is an introduction to the page with all namespace members
    {
      QCString result="Lista svih ";
      if (!extractAll) result+="dokumentiranih ";
      result+="članova imenika s linkovima na ";
      if (extractAll)
        result+="imeničku dokumentaciju svakog člana:";
      else
        result+="imenike kojima pripadaju:";
      return result;
    }
    QCString trNamespaceIndex() override
      // This is used in LaTeX as the title of the chapter with the
      // index of all namespaces.
    { return "Kazalo imenika"; }
    QCString trNamespaceDocumentation() override
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all namespaces.
    { return "Dokumentacija namespace-a"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces() override
    {
      return "Imenici";
    }

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
      QCString result="Dokumentacija ";
      switch(compType)
      {
        case ClassDef::Class:      result+="klase"; break;
        case ClassDef::Struct:     result+="strukture"; break;
        case ClassDef::Union:      result+="unije"; break;
        case ClassDef::Interface:  result+="sučelja (interface)"; break;
        case ClassDef::Protocol:   result+="protokola"; break;
        case ClassDef::Category:   result+="kategorije"; break;
        case ClassDef::Exception:  result+="iznimke (exception)"; break;
        default: break;
      }
      result+=" je napravljena iz " + trFile(FALSE, single) + ": ";
	  return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    QCString trReturnValues() override
    { return "Povratna vrijednost"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage() override
    { return "Glavna stranica"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    QCString trPageAbbreviation() override
    { return "str."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991106
//////////////////////////////////////////////////////////////////////////

    QCString trDefinedAtLineInSourceFile() override
    {
      return "Definirano u liniji @0 datoteke @1.";
    }
    QCString trDefinedInSourceFile() override
    {
      return "Definirano u datoteci @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated() override
    {
      return "Zastarjelo";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const QCString &clName) override
    {
      return "Kolaboracijski dijagram za "+clName+ ":";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const QCString &fName) override
    {
      return "Graf include međuovisnosti za "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation() override
    {
      return "Dokumentacija konstruktora i destruktora ";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode() override
    {
      return "Izvorni kod";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation() override
    {
      return "Dokumenacija za ovu datoteku.";
    }
    /*! Text for the \\pre command */
    QCString trPrecondition() override
    {
      return "Preduvjeti";
    }
    /*! Text for the \\post command */
    QCString trPostcondition() override
    {
      return "Postuvjeti";
    }
    /*! Text for the \\invariant command */
    QCString trInvariant() override
    {
      return "Invarijanta";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue() override
    {
      return "Početna vrijednost:";
    }
    /*! Text used the source code in the file index */
    QCString trCode() override
    {
      return "kod";
    }
    QCString trGraphicalHierarchy() override
    {
      return "Grafičko stablo klasa";
    }
    QCString trGotoGraphicalHierarchy() override
    {
      return "Grafičko stablo klasa";
    }
    QCString trGotoTextualHierarchy() override
    {
      return "Tekstualno stablo klasa";
    }
    QCString trPageIndex() override
    {
      return "Indeks stranice";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    QCString trNote() override
    {
      return "Primjedba";
    }
    QCString trPublicTypes() override
    {
      return "Public tipovi";
    }
    QCString trPublicAttribs() override
    {
      return "Public atributi";
    }
    QCString trStaticPublicAttribs() override
    {
      return "Static public atributi";
    }
    QCString trProtectedTypes() override
    {
      return "Protected tipovi";
    }
    QCString trProtectedAttribs() override
    {
      return "Protected atributi";
    }
    QCString trStaticProtectedAttribs() override
    {
      return "Static protected atributi";
    }
    QCString trPrivateTypes() override
    {
      return "Private tipovi";
    }
    QCString trPrivateAttribs() override
    {
      return "Private atributi";
    }
    QCString trStaticPrivateAttribs() override
    {
      return "Static private atributi";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    QCString trTodo() override
    {
      return "Za uraditi";
    }
    /*! Used as the header of the todo list */
    QCString trTodoList() override
    {
      return "Ostalo za uraditi";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy() override
    {
      return "Referencirano od";
    }
    QCString trRemarks() override
    {
      return "Napomene";
    }
    QCString trAttention() override
    {
        return "Pažnja";
    }
    QCString trInclByDepGraph() override
    {
      return "Ovaj graf pokazuje koje datoteke izravno "
             "ili neizravno uključuju ovu datoteku:";
    }
    QCString trSince() override
    {
      return "Od";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    QCString trLegendTitle() override
    {
      return "Legenda";
    }
    /*! page explaining how the dot graph's should be interpreted */
    QCString trLegendDocs() override
    {
      return
        "Ova stranica objašnjava kako interpretirati grafikone koje je generirao "
        "doxygen.<p>\n"
        "Na primjer:\n"
        "\\code\n"
        "/*! Nevidljiva klasa (neće stati na graf date visine) */\n"
        "class Invisible { };\n\n"
        "/*! Odrezana klasa, inheritance je skriven (klase se vidi na grafu, ali ne i sve bazne klase) */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Nedokumentirana klasa */\n"
        "class Undocumented { };\n\n"
        "/*! Klasa koja je naslijeđena public  */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! A template class */\n"
        "template<class T> class Templ { };\n\n"
        "/*! Klasa koje je naslijeđena protected */\n"
        "class ProtectedBase { };\n\n"
        "/*! Klasa koje je naslijeđena private */\n"
        "class PrivateBase { };\n\n"
        "/*! Klasa koja se koristi agregacijom  */\n"
        "class Used { };\n\n"
        "/*! Super klasa koja naslijeđuje/koristi ostale */\n"
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
        "To će rezultirati grafikonom:"
        "<p><center><img src=\"graph_legend."+getDotImageExtension()+"\"></center>\n"
        "<p>\n"
        "Pravokutnici imaju slijedeće značenje:\n"
        "<ul>\n"
        "<li>Puni crni predstavlja klasu za koji je napravljen graf.\n"
        "<li>Pravokutnik s crnim rubom predstavlja dokumentiranu klasu.\n"
        "<li>Pravokutnik s sivim rubom predstavlja nedokumentiranu klasu.\n"
        "<li>Pravokutnik s crvenim rubom predstavlja dokumentiranu klasu\n"
        "Za koju nije prikazan graf naslijeđivanja. Graf je odrezan "
        "ako ne stane unutar određenih granica."
        "</ul>\n"
        "Strelice imaju slijedeće značenje:\n"
        "<ul>\n"
        "<li>Tamnoplava strelica označava public naslijeđivanje.\n"
        "<li>Tamnozelena strelica označava protected naslijeđivanje.\n"
        "<li>Tamnocrvena strelica označava private naslijeđivanje.\n"
        "<li>Ljubičasta isprekidana strelica se koristi ako je klasa dio "
        "druge klase ili ako se klasa koristi u drugoj klasi. Natpis na "
        "strelici je ime varijable u drugoj klasi\n"
        "Strelica je označena imenom varijable.\n"
        "<li>Žuta isprekidana strelica označava relaciju između template instance "
        "i template klase. Označena je imenom template parametra\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    QCString trLegend() override
    {
      return "legenda";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    QCString trTest() override
    {
      return "Test";
    }
    /*! Used as the header of the test list */
    QCString trTestList() override
    {
      return "Test lista";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    QCString trProperties() override
    {
      return "Svojstva (property)";
    }
    /*! Used as a section header for IDL property documentation */
    QCString trPropertyDocumentation() override
    {
      return "Dokumentacija svojstava";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    QCString trClasses() override
    {
      return "Klase";
    }
    /*! Used as the title of a Java package */
    QCString trPackage(const QCString &name) override
    {
      return "Paket "+name;
    }
    /*! The description of the package index page */
    QCString trPackageListDescription() override
    {
      return "Paketi s kratkim opisom (ukoliko postoji):";
    }
    /*! The link name in the Quick links header for each page */
    QCString trPackages() override
    {
      return "Paketi";
    }
    /*! Text shown before a multi-line define */
    QCString trDefineValue() override
    {
      return "Vrijednost:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    QCString trBug() override
    {
      return "Greška";
    }
    /*! Used as the header of the bug list */
    QCString trBugList() override
    {
      return "Popis grešaka";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.6
//////////////////////////////////////////////////////////////////////////
    /*! Used as ansicpg for RTF file */
    QCString trRTFansicp() override
    {
      return "1252";
    }
    /*! Used as ansicpg for RTF fcharset */
    QCString trRTFCharSet() override
    {
      return "238";
    }
    /*! Used as header RTF general index */
    QCString trRTFGeneralIndex() override
    {
      return "Sadržaj";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trClass(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "klas", "e", "a");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trFile(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "datotek", "e", "a");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trNamespace(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "imeni", "ci", "k");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGroup(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "grup", "e", "a");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trPage(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "stranic", "e", "a");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trMember(bool, bool singular) override
    {
      return createNoun(false, singular, "član", "ovi");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGlobal(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "globaln", "e", "a") +
             createNoun(false, singular, " varijabl", "e", "a");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    QCString trAuthor(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "autor", "i");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    QCString trReferences() override
    {
      return "Reference";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    QCString trImplementedFromList(int numEntries) override
    {
      return "Implementira "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implementation this member.
     */
    QCString trImplementedInList(int numEntries) override
    {
      return "Implementirano u "+trWriteList(numEntries)+".";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    QCString trRTFTableOfContents() override
    {
      return "Sadržaj";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    QCString trDeprecatedList() override
    {
      return "Popis zastarjelih metoda";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    QCString trEvents() override
    {
      return "Događaji";
    }
    /*! Header used for the documentation section of a class' events. */
    QCString trEventDocumentation() override
    {
      return "Dokumentacija događaja";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    QCString trPackageTypes() override
    {
      return "Tipovi u paketu";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    QCString trPackageFunctions() override
    {
      return "Funkcije u paketu";
    }
    QCString trPackageMembers() override
    {
      return "članovi u paketu";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    QCString trStaticPackageFunctions() override
    {
      return "Statičke funkcije u paketu";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    QCString trPackageAttribs() override
    {
      return "Atributi u paketu";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    QCString trStaticPackageAttribs() override
    {
      return "Statički atributi u paketu";
    }
	//////////////////////////////////////////////////////////////////////////
	// new since 1.3.1
	//////////////////////////////////////////////////////////////////////////

	/*! Used in the quick index of a class/file/namespace member list page
	*  to link to the unfiltered list of all members.
	*/
	QCString trAll() override
	{
		return "Sve";
	}
	/*! Put in front of the call graph for a function. */
	QCString trCallGraph() override
	{
		return "Ovo je dijagram poziva za ovu funkciju:";
	}

	//////////////////////////////////////////////////////////////////////////
	// new since 1.3.3
	//////////////////////////////////////////////////////////////////////////

	/*! This string is used as the title for the page listing the search
	*  results.
	*/
	QCString trSearchResultsTitle() override
	{
		return "Rezultati pretrage";
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
			return "Nema dokumenta koji odgovaraju vašem upitu";
		}
		else if (numDocuments==1)
		{
			return "Nađen <b>1</b> dokument koji odgovara vašem upitu.";
		}
		else if (numDocuments<5)
		{
			// Croatian (AFAIK all Slavic languages except Macedonian and Bulgarian)
            // have different plural form for 2,3,4.
			return "Nađena <b>$num</b> dokumenta koji odgovaraju vašem upitu."
				"Najbolji su prikazani prvi.";
		}
		else
		{
			return "Nađeno <b>$num</b> dokumenata koji odgovaraju vašem upitu."
				"Najbolji su prikazani prvi.";
		}
	}
	/*! This string is put before the list of matched words, for each search
	*  result. What follows is the list of words that matched the query.
	*/
	QCString trSearchMatches() override
	{
		return "Pronađeno:";
	}

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    QCString trSourceFile(const QCString& filename) override
    {
      return "Izvorni kod datoteke " + filename;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    QCString trDirIndex() override
    { return "Stablo direktorija"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    QCString trDirDocumentation() override
    { return "Dokumentacija direktorija"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    QCString trDirectories() override
    { return "Direktoriji"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    QCString trDirReference(const QCString &dirName) override
    { QCString result= "Opis direktorija "; result += dirName; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    QCString trDir(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "direktorij", "i");
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    QCString trOverloadText() override
    {
       return "Ovo je preopterećena funkcija (overload). "
              "Razlikuje se od navedene metode "
              "samo po vrsti argumenata koje prihvaća.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    QCString trCallerGraph() override
    {
      return "Ovo je graf funkcija koje pozivaju ovu funkciju:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    QCString trEnumerationValueDocumentation() override
    { return "Dokumentacija enumeracija"; }
//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    QCString trMemberFunctionDocumentationFortran() override
    { return "Dokumentacija member funkcija/subrutina"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    QCString trCompoundListFortran() override
    { return "Lista tipova podataka"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    QCString trCompoundMembersFortran() override
    { return "Polja"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    QCString trCompoundListDescriptionFortran() override
    { return "Tipovi podataka s kratkim opisom:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    QCString trCompoundMembersDescriptionFortran(bool extractAll) override
    {
      QCString result="Lista svih ";
      if (!extractAll)
      {
        result+="dokumentiranih ";
      }
      result+="polja";
      result+=" s linkovima na ";
      if (!extractAll)
      {
        result+="dokumentaciju struktura podataka za svako polje";
      }
      else
      {
        result+="strukture kojima pripadaju:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
    * annotated compound index (Fortran).
    */
    QCString trCompoundIndexFortran() override
    { return "Kazalo data tipova"; }

    /*! This is used in LaTeX as the title of the chapter containing
    *  the documentation of all data types (Fortran).
    */
    QCString trTypeDocumentation() override
    { return "Dokumentacija tipova podataka"; }

    /*! This is used in the documentation of a file as a header before the
    *  list of (global) subprograms (Fortran).
    */
    QCString trSubprograms() override
    { return "Funkcije/Subrutine"; }

    /*! This is used in the documentation of a file/namespace before the list
    *  of documentation blocks for subprograms (Fortran)
    */
    QCString trSubprogramDocumentation() override
    { return "Documentacija funkcija/subrutina"; }

    /*! This is used in the documentation of a file/namespace/group before
    *  the list of links to documented compounds (Fortran)
    */
    QCString trDataTypes() override
    { return "Tipovi podataka"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    QCString trModulesList() override
    { return "Popis modula"; }

    /*! used as an introduction to the modules list (Fortran) */
    QCString trModulesListDescription(bool extractAll) override
    {
      QCString result="Lista svih ";
      if (!extractAll) result+="dokumentiranih ";
      result+="modula s kratkim opisom:";
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
        case ClassDef::Class:      result+=" Modul"; break;
        case ClassDef::Struct:     result+=" Tip"; break;
        case ClassDef::Union:      result+=" Unija"; break;
        case ClassDef::Interface:  result+=" Sučelje"; break;
        case ClassDef::Protocol:   result+=" Protokol"; break;
        case ClassDef::Category:   result+=" Kategorija"; break;
        case ClassDef::Exception:  result+=" Iznimka"; break;
        default: break;
      }
      if (isTemplate) result+=" Predložak";
      result+=" Referenca";
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    QCString trModuleReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" - Sadržaj modula";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    QCString trModulesMembers() override
    { return "članovi modula"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    QCString trModulesMemberDescription(bool extractAll) override
    {
      QCString result="Lista svih ";
      if (!extractAll) result+="dokumentiranih ";
      result+="članova modula s linkovima na ";
      if (extractAll)
      {
        result+="dokumentaciju modula za svaki član:";
      }
      else
      {
        result+="modul kojem pripadaju:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
    *  index of all modules (Fortran).
    */
    QCString trModulesIndex() override
    { return "Kazalo modula"; }

    /*! This is used for translation of the word that will possibly
    *  be followed by a single name or by a list of names
    *  of the category.
    */
    QCString trModule(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "modul", "i");
    }
    /*! This is put at the bottom of a module documentation page and is
    *  followed by a list of files that were used to generate the page.
    */
    QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
      bool) override
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      QCString result="Dokumentacija ovog ";
      switch(compType)
      {
        case ClassDef::Class:      result+="modula"; break;
        case ClassDef::Struct:     result+="tipa"; break;
        case ClassDef::Union:      result+="unije"; break;
        case ClassDef::Interface:  result+="sučelja"; break;
        case ClassDef::Protocol:   result+="protokola"; break;
        case ClassDef::Category:   result+="kategorije"; break;
        case ClassDef::Exception:  result+="iznimke"; break;
        default: break;
      }
      result+=" je napravljena iz :";
      return result;
    }
    /*! This is used for translation of the word that will possibly
    *  be followed by a single name or by a list of names
    *  of the category.
    */
    QCString trType(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "tip", "ovi");
    }
    /*! This is used for translation of the word that will possibly
    *  be followed by a single name or by a list of names
    *  of the category.
    */
    QCString trSubprogram(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "subprogram", "i");
    }

    /*! C# Type Constraint list */
    QCString trTypeConstraints() override
    {
      return "Ograničenja tipova (Type Constraints)";
    }
    //////////////////////////////////////////////////////////////////////////
    // new since 1.6.0 (mainly for the new search engine)
    //////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    QCString trDirRelation(const QCString &name) override
    {
        return QCString("Relacije ") + QCString(name);
    }

    /*! Loading message shown when loading search results */
    QCString trLoading() override
    {
        return "Učitavam...";
    }

    /*! Label used for search results in the global namespace */
    QCString trGlobalNamespace() override
    {
        return "Globalni namespace";
    }

    /*! Message shown while searching */
    QCString trSearching() override
    {
        return "Tražim...";
    }

    /*! Text shown when no search results are found */
    QCString trNoMatches() override
    {
        return "Nema traženih podataka";
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
      return "Datoteka u "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    QCString trIncludesFileIn(const QCString &name) override
    {
      return "Uključuje datotake u "+name;
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
      static const char *days[]   = { "Pon","Uto","Sri","Čet","Pet","Sub","Ned" };
      static const char *months[] = { "Sje","Velj","Ožu","Tra","Svi","Lip","Srp","Kol","Ruj","Lis","Stu","Pro" };
      QCString sdate;
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Date)
      {
        sdate.sprintf("%s %s %d %d",days[dayOfWeek-1],months[month-1],day,year);
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
      static const char *days_short[]   = { "pon", "uto", "sri", "čet", "pet", "sub", "ned" };
      static const char *days_full[]    = { "ponedjeljak", "utorak", "srijeda", "četvrtak", "petak", "subota", "nedjelja" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      return text;
    }
    QCString trMonth(int month, bool, bool full) override
    {
      static const char *months_short[] = { "sij", "vlj", "ožu", "tra", "svi", "lip", "srp", "kol", "ruj", "lis", "stu", "pro" };
      static const char *months_full[]  = { "siječanj", "veljača", "ožujak", "travanj", "svibanj", "lipanj", "srpanj", "kolovoz", "rujan", "listopad", "studeni", "prosinac" };
      QCString text  = full? months_full[month-1] : months_short[month-1];
      return text;
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
    { return "Bibliografija"; }

    /*! Text for copyright paragraph */
    QCString trCopyright() override
    { return "Copyright"; }

    /*! Header for the graph showing the directory dependencies */
    QCString trDirDepGraph(const QCString &name) override
    { return QCString("Direktoriji o kojima ovisi ")+name+":"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    QCString trDetailLevel() override
    { return "razina detalja"; }

    /*! Section header for list of template parameters */
    QCString trTemplateParameters() override
    { return "Parametri predloška"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    QCString trAndMore(const QCString &number) override
    { return "i još "+number+" ..."; }

    /*! Used file list for a Java enum */
    QCString trEnumGeneratedFromFiles(bool single) override
    { QCString result = "Dokumatacija za ovu enumeraciju je generirana iz ";
      if (!single) result += "datoteka:";
      else result += "datoteke:";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    QCString trEnumReference(const QCString &name) override
    { return "Opis enumeracije " + QCString(name); }

    /*! Used for a section containing inherited members */
    QCString trInheritedFrom(const QCString &members,const QCString &what) override
    { return QCString(members)+" naslijeđeni od "+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    QCString trAdditionalInheritedMembers() override
    { return "Dodatni naslijeđeni članovi"; }

};

#endif


