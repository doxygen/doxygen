/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2012 by Dimitri van Heesch.
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
//  - Better handling of ISO-8859-2/ WIN 1250 stuff based on (actualy stolen from :-)) Czech translations
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
    QCString idLanguage()
    { return "croatian"; }
    QCString idLanguageCharset()
    {
        return "utf-8";
    }
    QCString latexLanguageSupportCommand()
    { return "\\usepackage[croatian]{babel}\n"; }
    QCString trRelatedFunctions()
    { return "Povezane funkcije"; }
    QCString trRelatedSubscript()
    { return "(To nisu member funkcije.)"; }
    QCString trDetailedDescription()
    { return "Detaljno objašnjenje"; }
    QCString trMemberTypedefDocumentation()
    { return "Dokumentacija typedef članova"; }
    QCString trMemberEnumerationDocumentation()
    { return "Dokumentacija enumeracijskih članova"; }
    QCString trMemberFunctionDocumentation()
    { return "Dokumentacija funkcija"; }
    QCString trMemberDataDocumentation()
    { return "Documentacija varijabli"; }
    QCString trMore()
    { return "Opširnije..."; }
    QCString trListOfAllMembers()
    { return "Popis svih članova"; }
    QCString trMemberList()
    { return "Popis članova."; }
    QCString trThisIsTheListOfAllMembers()
    { return "Ovo je popis svih članova"; }
    QCString trIncludingInheritedMembers()
    { return ", uključujući naslijeđene članove."; }
    QCString trGeneratedAutomatically(const char *s)
    { QCString result="napravljeno automatski Doxygen-om"; 
      if (s) result+=(QCString)" za "+s;
      result+=" iz programskog koda."; 
      return result;
    }
    QCString trEnumName()
    { return "enum ime"; }
    QCString trEnumValue()
    { return "enum vrijednost"; }
    QCString trDefinedIn()
    { return "definirano u"; }
    QCString trModules()
    { return "Moduli"; }
    QCString trClassHierarchy()
    { return "Stablo klasa"; }
    QCString trCompoundList()
	{
		if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
		{
			return "Sve strukture";
		}
		else
		{
			return "Sve klase"; 
		}
	}
    QCString trFileList()
    { return "Popis datoteka"; }
    QCString trCompoundMembers()
	{
		if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
			return "Svi članovi struktura"; 
		else
			return "Svi članovi klasa"; 
	}
    QCString trFileMembers()
    { return "članovi klasa u datoteci"; }
    QCString trRelatedPages()
    { return "Stranice povezane s ovom"; }
    QCString trExamples()
    { return "Primjeri"; }
    QCString trSearch()
    { return "Traži"; }
    QCString trClassHierarchyDescription()
    { return "Stablo naslijeđivanja je složeno "
             "približno po abecedi:";
    }
    QCString trFileListDescription(bool extractAll)
    {
      QCString result="Popis svih ";
      if (!extractAll) result+="dokumentiranih ";
      result+="datoteka, s kratkim opisom:";
      return result;
    }
    QCString trCompoundListDescription()
    { return "Popis svih klasa, unija i struktura "
		  "s kratkim opisom :"; 
    }
    QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="Popis svih ";
      if (!extractAll)
		  result+="dokumentiranih ";

	  if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
		  result+="članova klasa s linkovima na ";
	  else
		  result+="članova struktura s linkovima na ";
		  
      if (!extractAll)
	  {
		  result+="dokumentaciju svakog člana:";
      }
	  else
	  {
		  if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
			  result+="dokumentaciju klase :";
		  else
			  result +="dokumentaciju strukture";
	  }
      return result;
    }
    QCString trFileMembersDescription(bool extractAll)
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
    QCString trExamplesDescription()
    { return "Popis primjera :"; }
    QCString trRelatedPagesDescription()
    { return "Popis povezanih stranica:"; }
    QCString trModulesDescription()
    { return "Popis svih modula:"; }

    QCString trDocumentation()
    { return "Dokumentacija"; }
    QCString trModuleIndex()
    { return "Kazalo modula"; }
    QCString trHierarchicalIndex()
    { return "Hijerarhijsko kazalo"; }
    QCString trCompoundIndex()
    {
      if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
      { 
        return "Kazalo struktura podataka";
      }
      else
      {
		return "Skupno kazalo ";
      }
	}
    QCString trFileIndex() 
    { return "Kazalo datoteka"; }
    QCString trModuleDocumentation()
    { return "Dokumentacija modula"; }
    QCString trClassDocumentation()
    {
		if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
		{
			return "Dokumentacija struktura podataka"; 
		}
		else
		{
			return "Dokumentacija klasa";
		}
	}
    QCString trFileDocumentation()
    { return "Dokumentacija datoteka"; }
    QCString trExampleDocumentation()
    { return "Dokumentacija primjera"; }
    QCString trPageDocumentation()
    { return "Dokumentacija vezane stranice"; }
    QCString trReferenceManual()
    { return "Priručnik"; }

    QCString trDefines()
    { return "Definicije"; }
    QCString trFuncProtos()
    { return "Prototipi funkcija"; }
    QCString trTypedefs()
    { return "Typedef-ovi"; }
    QCString trEnumerations()
    { return "Enumeracije"; }
    QCString trFunctions()
    { return "Funkcije"; }
    QCString trVariables()
    { return "Varijable"; }
    QCString trEnumerationValues()
    { return "Vrijednosti enumeracija"; }
    QCString trDefineDocumentation()
    { return "Dokumentacija definicija"; }
    QCString trFunctionPrototypeDocumentation()
    { return "Dokumentacija prototipa funkcije"; }
    QCString trTypedefDocumentation()
    { return "Dokumentacija typedef-a"; }
    QCString trEnumerationTypeDocumentation()
    { return "Dokumentacija enumeracijskog tipa"; }
    QCString trFunctionDocumentation()
    { return "Dokumentacije funkcija"; }
    QCString trVariableDocumentation()
    { return "Dokumentacija varijable"; }
    QCString trCompounds()
    { return "Strukture"; }
    QCString trGeneratedAt(const char *date,const char *projName)
    { 
      QCString result=(QCString)"Napravljeno "+date;
      if (projName) result+=(QCString)" projekt: "+projName;
      result+=" generator: ";
      return result;
    }
    QCString trWrittenBy()
    {
      return "napisao ";
    }
    QCString trClassDiagram(const char *clName)
    {
      return QCString("Dijagram klasa za ")+clName;
    }
    QCString trForInternalUseOnly()
    { return "Isključivo za internu uporabu."; }
    QCString trWarning()
    { return "Upozorenje"; }
    QCString trVersion()
    { return "Verzija"; }
    QCString trDate()
    { return "Datum"; }
    QCString trReturns()
    { return "Povratne vrijednosti"; }
    QCString trSeeAlso()
    { return "Vidi također"; }
    QCString trParameters()
    { return "Parametri"; }
    QCString trExceptions()
    { return "Iznimke"; }
    QCString trGeneratedBy()
    { return "Generirao"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307 
//////////////////////////////////////////////////////////////////////////
    
    QCString trNamespaceList()
    { return "Popis imenika"; }
    QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Popis svih ";
      if (!extractAll) result+="dokumentiranih ";
      result+="imenika s kratkim opisom:";
      return result;
    }
    QCString trFriends()
    { return "Friend-ovi "; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
    QCString trRelatedFunctionDocumentation()
    { return "Dokumentacija povezanih funkcija"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    QCString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool /*isTemplate*/)
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
      }
          result += clName;
      
      return result;
    }
    QCString trFileReference(const char *fileName)
      // used as the title of the HTML page of a file
    {
      QCString result="Opis datoteke "; 
          result+=fileName;
          return result;
    }
    QCString trNamespaceReference(const char *namespaceName)
      // used as the title of the HTML page of a namespace
    {
      QCString result ="Opis imenika ";
          result+=namespaceName;
      
      return result;
    }
    
    // these are for the member sections of a class, struct or union 
    QCString trPublicMembers()
    { return "Public članovi"; }
    QCString trPublicSlots()
    { return "Public slotovi"; }
    QCString trSignals()
    { return "Signali"; }
    QCString trStaticPublicMembers()
    { return "Static public članovi"; }
    QCString trProtectedMembers()
    { return "Protected članovi"; }
    QCString trProtectedSlots()
    { return "Protected slotovi"; }
    QCString trStaticProtectedMembers()
    { return "Static protected članovi"; }
    QCString trPrivateMembers()
    { return "Privatni članovi"; }
    QCString trPrivateSlots()
    { return "Privatni slotovi"; }
    QCString trStaticPrivateMembers()
    { return "Statični privatni članovi"; }
    // end of member sections 
    
    QCString trWriteList(int numEntries)
    {
      // this function is used to produce a comma-separated list of items.
      // use generateMarker(i) to indicate where item i should be put.
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
            result+="";
        }
      }
      return result; 
    }
    
    QCString trInheritsList(int numEntries)
      // used in class documentation to produce a list of base classes,
      // if class diagrams are disabled.
    {
      return "Naslijeđuje od "+trWriteList(numEntries)+".";
    }
    QCString trInheritedByList(int numEntries)
      // used in class documentation to produce a list of super classes,
      // if class diagrams are disabled.
    {
      return "Naslijeđena u "+trWriteList(numEntries)+".";
    }
    QCString trReimplementedFromList(int numEntries)
      // used in member documentation blocks to produce a list of 
      // members that are hidden by this one.
    {
      return "Reimplementirano od  "+trWriteList(numEntries)+".";
    }
    QCString trReimplementedInList(int numEntries)
    {
      // used in member documentation blocks to produce a list of
      // all member that overwrite the implementation of this member.
      return "Reimplementacija u "+trWriteList(numEntries)+".";
    }

    QCString trNamespaceMembers()
      // This is put above each page as a link to all members of namespaces.
    { return "članovi imenika"; }
    QCString trNamespaceMemberDescription(bool extractAll)
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
    QCString trNamespaceIndex()
      // This is used in LaTeX as the title of the chapter with the 
      // index of all namespaces.
    { return "Kazalo imenika"; }
    QCString trNamespaceDocumentation()
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all namespaces.
    { return "Dokumentacija namespace-a"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces()
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
        bool single)
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
      }
      result+=" je napravljena iz " + trFile(FALSE, single) + ": ";
	  return result;
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    QCString trAlphabeticalList()
    { return "Abecedni popis"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    QCString trReturnValues()
    { return "Povratna vrijednost"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage()
    { return "Glavna stranica"; }

    /*! This is used in references to page that are put in the LaTeX 
     *  documentation. It should be an abbreviation of the word page.
     */
    QCString trPageAbbreviation()
    { return "str."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991106
//////////////////////////////////////////////////////////////////////////

    QCString trDefinedAtLineInSourceFile()
    {
      return "Definirano u liniji @0 datoteke @1.";
    }
    QCString trDefinedInSourceFile()
    {
      return "Definirano u datoteci @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated()
    {
      return "Zastarjelo";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)"Kolaboracijski dijagram za "+clName+ ":";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const char *fName)
    {
      return (QCString)"Graf include međuovisnosti za "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation()
    {
      return "Dokumentacija konstruktora i destruktora "; 
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode()
    {
      return "Izvorni kod";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation()
    {
      return "Dokumenacija za ovu datoteku.";
    }
    /*! Text for the \\pre command */
    QCString trPrecondition()
    {
      return "Preduvjeti";
    }
    /*! Text for the \\post command */
    QCString trPostcondition()
    {
      return "Postuvjeti";
    }
    /*! Text for the \\invariant command */
    QCString trInvariant()
    {
      return "Invarijanta";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue()
    {
      return "Početna vrijednost:";
    }
    /*! Text used the source code in the file index */
    QCString trCode()
    {
      return "kod";
    }
    QCString trGraphicalHierarchy()
    {
      return "Grafičko stablo klasa";
    }
    QCString trGotoGraphicalHierarchy()
    {
      return "Grafičko stablo klasa";
    }
    QCString trGotoTextualHierarchy()
    {
      return "Tekstualno stablo klasa";
    }
    QCString trPageIndex()
    {
      return "Indeks stranice";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////
    
    QCString trNote()
    {
      return "Primjedba";
    }
    QCString trPublicTypes()
    {
      return "Public tipovi";
    }
    QCString trPublicAttribs()
    {
      return "Public atributi";
    }
    QCString trStaticPublicAttribs()
    {
      return "Static public atributi";
    }
    QCString trProtectedTypes()
    {
      return "Protected tipovi";
    }
    QCString trProtectedAttribs()
    {
      return "Protected atributi";
    }
    QCString trStaticProtectedAttribs()
    {
      return "Static protected atributi";
    }
    QCString trPrivateTypes()
    {
      return "Private tipovi";
    }
    QCString trPrivateAttribs()
    {
      return "Private atributi";
    }
    QCString trStaticPrivateAttribs()
    {
      return "Static private atributi";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    virtual QCString trTodo()
    {
      return "Za uraditi";
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    {
      return "Ostalo za uraditi";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
      return "Referencirano od";
    }
    virtual QCString trRemarks()
    {
      return "Napomene";
    }
    virtual QCString trAttention()
    {
        return "Pažnja";
    }
    virtual QCString trInclByDepGraph()
    {
      return "Ovaj graf pokazuje koje datoteke izravno "
             "ili neizravno uključuju ovu datoteku:";
    }
    virtual QCString trSince()
    {
      return "Od";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    {
      return "Legenda";
    }
    /*! page explaining how the dot graph's should be interpreted */
    virtual QCString trLegendDocs()
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
        "<p><center><img src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
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
      return "Test";
    }
    /*! Used as the header of the test list */
    virtual QCString trTestList()
    {
      return "Test lista";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.1
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for KDE-2 IDL methods */
    virtual QCString trDCOPMethods()
    {
      return "DCOP metode";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    virtual QCString trProperties()
    {
      return "Svojstva (property)";
    }
    /*! Used as a section header for IDL property documentation */
    virtual QCString trPropertyDocumentation()
    {
      return "Dokumentacija svojstava";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      return "Klase";
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const char *name)
    {
      return (QCString)"Paket "+name;
    }
    /*! Title of the package index page */
    virtual QCString trPackageList()
    {
      return "Lista paketa";
    }
    /*! The description of the package index page */
    virtual QCString trPackageListDescription()
    {
      return "Paketi s kratkim opisom (ukoliko postoji):";
    }
    /*! The link name in the Quick links header for each page */
    virtual QCString trPackages()
    {
      return "Paketi";
    }
    /*! Text shown before a multi-line define */
    virtual QCString trDefineValue()
    {
      return "Vrijednost:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////
    
    /*! Used as a marker that is put before a \\bug item */
    virtual QCString trBug()
    {
      return "Greška";
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList()
    {
      return "Popis grešaka";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.6
//////////////////////////////////////////////////////////////////////////
    /*! Used as ansicpg for RTF file */
    virtual QCString trRTFansicp()
    {
      return "1252";
    }
    /*! Used as ansicpg for RTF fcharset */
    virtual QCString trRTFCharSet()
    {
      return "238";
    }
    /*! Used as header RTF general index */
    virtual QCString trRTFGeneralIndex()
    {
      return "Sadržaj";
    }
   
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Klas" : "klas"));
      result+= (singular ? "a" : "e");
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trFile(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Datotek" : "datotek"));
      result+= (singular ? "a" : "e");
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trNamespace(bool first_capital, bool singular)
    { 
		QCString result;
		if (singular)
			result = ((first_capital ? "Imenik" : "imenik"));
		else
			result = ((first_capital ? "Imenici" : "imenici"));
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGroup(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Grup" : "grup"));
      result+= (singular ? "a" : "e");
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Stranic" : "stranic"));
      result+= (singular ? "a" : "e");
      return result; 
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "član" : "član"));
      if (!singular)  result+="ovi";
      return result; 
    }
   
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trGlobal(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "G" : "g"));
	  if( singular )
		  result += "lobalna varijabla";
	  else
		  result += "lobalne varijable";

	  return result; 
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    virtual QCString trAuthor(bool first_capital, bool singular)
    {                                                                         
      QCString result((first_capital ? "Autor" : "autor"));
      if (!singular)  result+="i";
      return result; 
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences()
    {
      return "Reference";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of 
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    {
      return "Implementira "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implementation this member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      return "Implementirano u "+trWriteList(numEntries)+".";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    virtual QCString trRTFTableOfContents()
    {
      return "Sadržaj";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been 
     *  flagged deprecated 
     */
    virtual QCString trDeprecatedList()
    {
      return "Popis zastarjelih metoda";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in 
     * a C# program
     */
    virtual QCString trEvents()
    {
      return "Događaji";
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      return "Dokumentacija događaja";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    { 
      return "Tipovi u paketu";
    }
    /*! Used as a heading for a list of Java class functions with package 
     * scope. 
     */
    virtual QCString trPackageMembers()
    { 
      return "Funkcije u paketu";
    }
    /*! Used as a heading for a list of static Java class functions with 
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    { 
      return "Statičke funkcije u paketu";
    }
    /*! Used as a heading for a list of Java class variables with package 
     * scope.
     */
    virtual QCString trPackageAttribs()
    { 
      return "Atributi u paketu";
    }
    /*! Used as a heading for a list of static Java class variables with 
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    { 
      return "Statički atributi u paketu";
    }
	//////////////////////////////////////////////////////////////////////////
	// new since 1.3.1
	//////////////////////////////////////////////////////////////////////////

	/*! Used in the quick index of a class/file/namespace member list page 
	*  to link to the unfiltered list of all members.
	*/
	virtual QCString trAll()
	{
		return "Sve";
	}
	/*! Put in front of the call graph for a function. */
	virtual QCString trCallGraph()
	{
		return "Ovo je dijagram poziva za ovu funkciju:";
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
		return "Traži";
	}
	/*! This string is used as the title for the page listing the search
	*  results.
	*/
	virtual QCString trSearchResultsTitle()
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
	virtual QCString trSearchResults(int numDocuments)
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
	virtual QCString trSearchMatches()
	{
		return "Pronađeno:";
	}

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename)
    {
      return "Izvorni kod datoteke " + filename;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex()
    { return "Stablo direktorija"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation()
    { return "Dokumentacija direktorija"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories()
    { return "Direktoriji"; }

    /*! This returns a sentences that introduces the directory hierarchy. 
     *  and the fact that it is sorted alphabetically per level
     */
    virtual QCString trDirDescription()
    { return "Stablo direktorija sortirano abecednim redom:"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const char *dirName)
    { QCString result= "Opis direktorija "; result += dirName; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Direktorij" : "direktorij"));
      if (!singular) result+="i"; 
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
       return "Ovo je preopterećena funkcija (overload). "
              "Razlikuje se od navedene metode "
              "samo po vrsti argumenata koje prihvaća.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    virtual QCString trCallerGraph()
    {
      return "Ovo je graf funkcija koje pozivaju ovu funkciju:";
    }

    /*! This is used in the documentation of a file/namespace before the list 
     *  of documentation blocks for enumeration values
     */
    virtual QCString trEnumerationValueDocumentation()
    { return "Dokumentacija enumeracija"; }
//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran()
    { return "Dokumentacija member funkcija/subrutina"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */    
    virtual QCString trCompoundListFortran()
    { return "Lista tipova podataka"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran()
    { return "Polja"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran()
    { return "Tipovi podataka s kratkim opisom:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll)
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
    virtual QCString trCompoundIndexFortran()
    { return "Kazalo data tipova"; }

    /*! This is used in LaTeX as the title of the chapter containing
    *  the documentation of all data types (Fortran).
    */
    virtual QCString trTypeDocumentation()
    { return "Dokumentacija tipova podataka"; }

    /*! This is used in the documentation of a file as a header before the 
    *  list of (global) subprograms (Fortran).
    */
    virtual QCString trSubprograms()
    { return "Funkcije/Subrutine"; }

    /*! This is used in the documentation of a file/namespace before the list 
    *  of documentation blocks for subprograms (Fortran)
    */
    virtual QCString trSubprogramDocumentation()
    { return "Documentacija funkcija/subrutina"; }

    /*! This is used in the documentation of a file/namespace/group before 
    *  the list of links to documented compounds (Fortran)
    */
    virtual QCString trDataTypes()
    { return "Tipovi podataka"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList()
    { return "Popis modula"; }

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll)
    {
      QCString result="Lista svih ";
      if (!extractAll) result+="dokumentiranih ";
      result+="modula s kratkim opisom:";
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
      case ClassDef::Class:      result+=" Modul"; break;
      case ClassDef::Struct:     result+=" Tip"; break;
      case ClassDef::Union:      result+=" Unija"; break;
      case ClassDef::Interface:  result+=" Sučelje"; break;
      case ClassDef::Protocol:   result+=" Protokol"; break;
      case ClassDef::Category:   result+=" Kategorija"; break;
      case ClassDef::Exception:  result+=" Iznimka"; break;
      }
      if (isTemplate) result+=" Predložak";
      result+=" Referenca";
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    virtual QCString trModuleReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" - Sadržaj modula";        
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers()
    { return "članovi modula"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll)
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
    virtual QCString trModulesIndex()
    { return "Kazalo modula"; }

    /*! This is used for translation of the word that will possibly
    *  be followed by a single name or by a list of names 
    *  of the category.
    */
    virtual QCString trModule(bool first_capital, bool singular)
    {       
      QCString result((first_capital ? "Modul" : "modul"));
      if (!singular)  result+="i";
      return result; 
    }
    /*! This is put at the bottom of a module documentation page and is
    *  followed by a list of files that were used to generate the page.
    */
    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
      bool)
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      QCString result=(QCString)"Dokumentacija ovog ";
      switch(compType)
      {
      case ClassDef::Class:      result+="modula"; break;
      case ClassDef::Struct:     result+="tipa"; break;
      case ClassDef::Union:      result+="unije"; break;
      case ClassDef::Interface:  result+="sučelja"; break;
      case ClassDef::Protocol:   result+="protokola"; break;
      case ClassDef::Category:   result+="kategorije"; break;
      case ClassDef::Exception:  result+="iznimke"; break;
      }
      result+=" je napravljena iz :";
      return result;
    }
    /*! This is used for translation of the word that will possibly
    *  be followed by a single name or by a list of names 
    *  of the category.
    */
    virtual QCString trType(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Tip" : "tip"));
      if (!singular)  result+="ovi";
      return result; 
    }
    /*! This is used for translation of the word that will possibly
    *  be followed by a single name or by a list of names 
    *  of the category.
    */
    virtual QCString trSubprogram(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Subprogram" : "subprogram"));
      if (!singular)  result+="i";
      return result; 
    }

    /*! C# Type Constraint list */
    virtual QCString trTypeConstraints()
    {
      return "Ograničenja tipova (Type Constraints)";
    }
    //////////////////////////////////////////////////////////////////////////
    // new since 1.6.0 (mainly for the new search engine)
    //////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    virtual QCString trDirRelation(const char *name)
    {
        return QCString("Relacije ") + QCString(name);
    }

    /*! Loading message shown when loading search results */
    virtual QCString trLoading()
    {
        return "Učitavam...";
    }

    /*! Label used for search results in the global namespace */
    virtual QCString trGlobalNamespace()
    {
        return "Globalni namespace";
    }

    /*! Message shown while searching */
    virtual QCString trSearching()
    {
        return "Tražim...";
    }

    /*! Text shown when no search results are found */
    virtual QCString trNoMatches()
    {
        return "Nema traženih podataka";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.3 (missing items for the directory pages)
//////////////////////////////////////////////////////////////////////////

    /*! introduction text for the directory dependency graph */
    virtual QCString trDirDependency(const char *name)
    {
      return (QCString)"Graf povezanih direktorija za "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the first column mentions the
     *  source file that has a relation to another file.
     */
    virtual QCString trFileIn(const char *name)
    {
      return (QCString)"Datoteka u "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    virtual QCString trIncludesFileIn(const char *name)
    {
      return (QCString)"Uključuje datotake u "+name;
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
      static const char *days[]   = { "Pon","Uto","Sri","Čet","Pet","Sub","Ned" };
      static const char *months[] = { "Sje","Velj","Ožu","Tra","Svi","Lip","Srp","Kol","Ruj","Lis","Stu","Pro" };
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
//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    virtual QCString trCiteReferences()
    { return "Bibliografija"; }

    /*! Text for copyright paragraph */
    virtual QCString trCopyright()
    { return "Copyright"; }

    /*! Header for the graph showing the directory dependencies */
    virtual QCString trDirDepGraph(const char *name)
    { return QCString("Direktoriji o kojima ovisi ")+name+":"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    virtual QCString trDetailLevel()
    { return "razina detalja"; }

    /*! Section header for list of template parameters */
    virtual QCString trTemplateParameters()
    { return "Parametri predloška"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    virtual QCString trAndMore(const QCString &number)
    { return "i još "+number+" ..."; }

    /*! Used file list for a Java enum */
    virtual QCString trEnumGeneratedFromFiles(bool single)
    { QCString result = "Dokumatacija za ovu enumeraciju je generirana iz ";
      if (!single) result += "datoteka:";
      else result += "datoteke:";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    virtual QCString trEnumReference(const char *name)
    { return "Opis enumeracije " + QCString(name); }

    /*! Used for a section containing inherited members */
    virtual QCString trInheritedFrom(const char *members,const char *what)
    { return QCString(members)+" naslijeđeni od "+what; }

    /*! Header of the sections with inherited members specific for the 
     *  base class(es) 
     */
    virtual QCString trAdditionalInheritedMembers()
    { return "Dodatni naslijeđeni članovi"; }

//////////////////////////////////////////////////////////////////////////

};

#endif


