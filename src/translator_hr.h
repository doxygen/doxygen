/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2004 by Dimitri van Heesch.
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
// translation by Boris Bralo <boris.bralo@zg.htnet.hr>
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

#ifndef TRANSLATOR_HR_H
#define TRANSLATOR_HR_H

class TranslatorCroatian : public Translator
{
  private:
        /*! to avoid macro redefinition from translator_cz.h */
        inline QCString decode(const QCString& sInput)
        { 
          if (Config_getBool("USE_WINDOWS_ENCODING"))
          {
            return ISO88592ToWin1250(sInput);
          }
          else
          {
            return sInput;
          }
        }
  
  public:
    QCString idLanguage()
    { return "croatian"; }
    QCString idLanguageCharset()
    {
      if (Config_getBool("USE_WINDOWS_ENCODING"))
      {
        return "windows-1250"; 
      }
      else
      {
        return "iso-8859-2";
      }
    }
    QCString latexLanguageSupportCommand()
    { return "\\usepackage[croatian]{babel}\n"; }
    QCString trRelatedFunctions()
    { return "Povezane funkcije"; }
    QCString trRelatedSubscript()
    { return "(To nisu member funkcije.)"; }
    QCString trDetailedDescription()
    { return decode("Detaljno obja¹njenje"); }
    QCString trMemberTypedefDocumentation()
    { return decode("Dokumentacija typedef èlanova"); }
    QCString trMemberEnumerationDocumentation()
    { return decode("Dokumentacija enumeracijskih èlanova"); }
    QCString trEnumerationValueDocumentation()
    { return "Dokumentacija enumeracijskih vrijednosti"; }
    QCString trMemberFunctionDocumentation()
        { return "Dokumentacija funkcija"; }
    QCString trMemberDataDocumentation()
    { return "Documentacija varijabli"; }
    QCString trMore()
    { return decode("Op¹irnije..."); }
    QCString trListOfAllMembers()
    { return decode("Popis svih èlanova."); }
    QCString trMemberList()
    { return decode("Popis èlanova."); }
    QCString trThisIsTheListOfAllMembers()
    { return decode("Ovo je popis svih èlanova"); }
    QCString trIncludingInheritedMembers()
    { return decode(", ukljuèujuæi naslijeðene èlanove."); }
    QCString trGeneratedAutomatically(const char *s)
    { QCString result=decode("generirano automatski Doxygen-om"); 
      if (s) result+=(QCString)" za "+s;
      result+=" iz programskog koda."; 
      return decode(result);
    }
    QCString trEnumName()
    { return decode("enum ime"); }
    QCString trEnumValue()
    { return decode("enum vrijednost"); }
    QCString trDefinedIn()
    { return decode("definirano u"); }
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
    QCString trHeaderFiles()
    { return "Header datoteke"; }
    QCString trCompoundMembers()
	{
		if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
			return decode("Svi èlanovi struktura"); 
		else
			return decode("Svi èlanovi klasa"); 
	}
    QCString trFileMembers()
    { return decode("Èlanovi klasa u datoteci"); }
    QCString trRelatedPages()
    { return decode("Stranice povezane s ovom"); }
    QCString trExamples()
    { return "Primjeri"; }
    QCString trSearch()
    { return decode("Tra¾i"); }
    QCString trClassHierarchyDescription()
    { return decode("Stablo naslijeðivanja je slo¾eno "
                                        "pribli¾no po abecedi:");
    }
    QCString trFileListDescription(bool extractAll)
    {
      QCString result="Popis svih ";
      if (!extractAll) result+="dokumentiranih ";
      result+="datoteka, s kratkim opisom:";
      return decode(result);
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
		  result+="èlanova klasa s linkovima na ";
	  else
		  result+="èlanova struktura s linkovima na ";
		  
      if (!extractAll)
	  {
		  result+="dokumentaciju svakog èlana:";
      }
	  else
	  {
		  if (Config_getBool("OPTIMIZE_OUTPUT_FOR_C"))
			  result+="dokumentaciju klase :";
		  else
			  result +="dokumentaciju strukture";
	  }
      return decode(result);
    }
    QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Popis svih ";
      if (!extractAll)
		  result+="dokumentiranih ";
      result+="èlanova s linkovima na ";
      if (extractAll)
		  result+="dokumentaciju datoteke u kojima se nalaze:";
      else
		  result+="datoteke u kojima se nalaze:";
      return decode(result);
    }
    QCString trHeaderFilesDescription()
    { return decode("Popis header datoteka koje èine API :"); }
    QCString trExamplesDescription()
    { return "Popis primjera :"; }
    QCString trRelatedPagesDescription()
    { return "Popis povezanih stranica:"; }
    QCString trModulesDescription()
    { return "Popis svih modula:"; }
    QCString trNoDescriptionAvailable()
    { return "Opis nije dostupan"; }

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
    { return decode("Priruènik"); }

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
      QCString result=(QCString)"Generirano "+date;
      if (projName) result+=(QCString)" projekt: "+projName;
      result+=" generator: ";
      return decode(result);
    }
    QCString trWrittenBy()
    {
      return decode("napisao ");
    }
    QCString trClassDiagram(const char *clName)
    {
      return decode("Dijagram klasa za ")+clName;
    }
    QCString trForInternalUseOnly()
    { return decode("Iskljuèivo za internu uporabu."); }
    QCString trReimplementedForInternalReasons()
        { return decode("Reimplementirano zbog internih razloga; "
                                        "Nema utjecaja na API."); 
    }
    QCString trWarning()
    { return "Upozorenje"; }
    QCString trBugsAndLimitations()
    { return decode("Gre¹ke i ogranièenja"); }
    QCString trVersion()
    { return "Verzija"; }
    QCString trDate()
    { return "Datum"; }
    QCString trReturns()
    { return "Povratne vrijednosti"; }
    QCString trSeeAlso()
    { return decode("Vidi takoðer"); }
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
      return decode(result);
    }
    QCString trFriends()
    { return decode("Friend-ovi "); }

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
        case ClassDef::Interface:  result+=" suèelja (interface) "; break;
        case ClassDef::Protocol:   result+=" protocol "; break; // translate me!
        case ClassDef::Category:   result+=" category "; break; // translate me!
        case ClassDef::Exception:  result+=" iznimke (exception) "; break;
      }
          result += clName;
      
      return decode(result);
    }
    QCString trFileReference(const char *fileName)
      // used as the title of the HTML page of a file
    {
      QCString result="Opis datoteke "; 
          result+=fileName;
          return decode(result);
    }
    QCString trNamespaceReference(const char *namespaceName)
      // used as the title of the HTML page of a namespace
    {
      QCString result ="Opis imenika ";
          result+=namespaceName;
      
      return decode(result);
    }
    
    // these are for the member sections of a class, struct or union 
    QCString trPublicMembers()
    { return decode("Public èlanovi"); }
    QCString trPublicSlots()
    { return "Public slotovi"; }
    QCString trSignals()
    { return "Signali"; }
    QCString trStaticPublicMembers()
    { return decode("Static public èlanovi"); }
    QCString trProtectedMembers()
    { return decode("Protected èlanovi"); }
    QCString trProtectedSlots()
    { return "Protected slotovi"; }
    QCString trStaticProtectedMembers()
    { return decode("Static protected èlanovi"); }
    QCString trPrivateMembers()
    { return decode("Private èlanovi"); }
    QCString trPrivateSlots()
    { return decode("Private slotovi"); }
    QCString trStaticPrivateMembers()
    { return decode("Static private èlanovi"); }
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
      return decode("Naslijeðuje od "+trWriteList(numEntries)+".");
    }
    QCString trInheritedByList(int numEntries)
      // used in class documentation to produce a list of super classes,
      // if class diagrams are disabled.
    {
      return decode("Naslijeðena u "+trWriteList(numEntries)+".");
    }
    QCString trReimplementedFromList(int numEntries)
      // used in member documentation blocks to produce a list of 
      // members that are hidden by this one.
    {
      return decode("Reimplementirano od  "+trWriteList(numEntries)+".");
    }
    QCString trReimplementedInList(int numEntries)
    {
      // used in member documentation blocks to produce a list of
      // all member that overwrite the implementation of this member.
      return decode("Reimplementacija u "+trWriteList(numEntries)+".");
    }

    QCString trNamespaceMembers()
      // This is put above each page as a link to all members of namespaces.
    { return decode("Èlanovi imenika"); }
    QCString trNamespaceMemberDescription(bool extractAll)
      // This is an introduction to the page with all namespace members
    { 
      QCString result="Lista svih ";
      if (!extractAll) result+="dokumentiranih ";
      result+="èlanova imenika s linkovima na ";
      if (extractAll) 
        result+="imenièku dokumentaciju svakog èlana:";
      else 
        result+="imenike kojima pripadaju:";
      return decode(result);
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
      QCString result="Dokumentacija ove ";
      switch(compType)
      {
        case ClassDef::Class:      result+="klase"; break;
        case ClassDef::Struct:     result+="strukture"; break;
        case ClassDef::Union:      result+="unije"; break;
        case ClassDef::Interface:  result+="suèelja (interface)"; break;
        case ClassDef::Protocol:   result+="protocol"; break; // translate me!
        case ClassDef::Category:   result+="category"; break; // translate me!
        case ClassDef::Exception:  result+="iznimke (exception)"; break;
      }
      result+=" je generirana iz " + trFile(FALSE, single) + ": ";
	  return decode(result);
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

    QCString trSources()
    {
      return "Izvorne datoteke";
    }
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
      return decode( (QCString)"Kolaboracijski dijagram za "+clName+ ":");
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const char *fName)
    {
      return decode((QCString)"Graf include meðuovisnosti za "+fName+":");
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
      return decode("Poèetna vrijednost:");
    }
    /*! Text used the source code in the file index */
    QCString trCode()
    {
      return "kod";
    }
    QCString trGraphicalHierarchy()
    {
      return decode("Grafièko stablo klasa");
    }
    QCString trGotoGraphicalHierarchy()
    {
      return decode("Grafièko stablo klasa");
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
        return decode("Pa¾nja");
    }
    virtual QCString trInclByDepGraph()
    {
      return decode("Ovaj graf pokazuje koje datoteke izravno "
                            "ili neizravno ukljuèuju ovu datoteku:");
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
        decode( "Ova stranica obja¹njava kako interpretirati grafikone koje je generirao "
        "doxygen.<p>\n"
        "Na primjer:\n"
        "\\code\n"
        "/*! Nevidljiva klasa (neæe stati na graf date visine) */\n"
        "class Invisible { };\n\n"
        "/*! Odrezana klasa, inheritance je skriven (klase se vidi na grafu, ali ne i sve bazne klase) */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Nedokumentirana klasa */\n"
        "class Undocumented { };\n\n"
        "/*! Klasa koja je naslijeðena public  */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! Klasa koje je naslijeðena protected */\n"
        "class ProtectedBase { };\n\n"
        "/*! Klasa koje je naslijeðena private */\n"
        "class PrivateBase { };\n\n"
        "/*! Klasa koja se koristi agregacijom  */\n"
        "class Used { };\n\n"
        "/*! Super klasa koja naslijeðuje/koristi ostale */\n"
        "class Inherited : public PublicBase,\n"
        "                  protected ProtectedBase,\n"
        "                  private PrivateBase,\n"
        "                  public Undocumented\n"
        "{\n"
        "  private:\n"
        "    Used *m_usedClass;\n"
        "};\n"
        "\\endcode\n"
        "Ako je \\c MAX_DOT_GRAPH_HEIGHT tag u konfiguracijskoj datoteci "
        "postavljen na  200 gornje veze æe rezultirati grafikonom:"
        "<p><center><img src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
        "<p>\n"
        "Pravokutnici imaju slijedeæe znaèenje:\n"
        "<ul>\n"
        "<li>Puni crni predstavlja klasu za koji je generiran graf.\n"
        "<li>Pravokutnik s crnim rubom predstavlja dokumentiranu klasu.\n"
        "<li>Pravokutnik s sivim rubom predstavlja nedokumentiranu klasu.\n"
        "<li>Pravokutnik s crvenim rubom predstavlja dokumentiranu klasu\n"
        "Za koju nije prikazan graf naslijeðivanja. Graf je odrezan "
        "ako ne stane unutar odreðenih granica."
        "</ul>\n"
        "Strelice imaju slijedeæe znaèenje:\n"
        "<ul>\n"
        "<li>Tamnoplava strelica oznaèava public naslijeðivanje.\n"
        "<li>Tamnozelena strelica oznaèava protected naslijeðivanje.\n"
        "<li>Tamnocrvena strelica oznaèava private naslijeðivanje.\n"
        "<li>Ljubièasta isprekidana strelica se koristi za agregate vezu. "
        "Strelica je oznaèena imenom varijable.\n"
        "</ul>\n");
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

    /*! Used for Java interfaces in the summary section of Java packages */
    virtual QCString trInterfaces()
    {
      return decode("Suèelja (interfaces)");
    }
    /*! Used for Java classes in the summary section of Java packages */
    virtual QCString trClasses()
    {
      return "Klase";
    }
    /*! Used as the title of a Java package */
    virtual QCString trPackage(const char *name)
    {
      return decode((QCString)"Paket "+name);
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
    /*! Used as a chapter title for Latex & RTF output */
    virtual QCString trPackageDocumentation()
    {
      return "Dokumentacija paketa";
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
      return decode("Greška");
    }
    /*! Used as the header of the bug list */
    virtual QCString trBugList()
    {
      return decode("Popis grešaka");
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
      return decode("Sadr¾aj");
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
      QCString result((first_capital ? "Èlan" : "èlan"));
      if (!singular)  result+="ovi";
      return decode(result); 
    }
   
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names 
     *  of the category.
     */
    virtual QCString trField(bool first_capital, bool singular)
    { 
      QCString result((first_capital ? "Polj" : "polj"));
      result+= (singular ? "e" : "a");
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
      return decode("Sadr¾aj");
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
      return decode("Dogaðaji");
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      return decode("Dokumentacija dogaðaja");
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
      return decode("Statièke funkcije u paketu");
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
      return decode("Statièki atributi u paketu");
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
		return decode("Traži");
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
			return decode("Nema dokumenta koji odgovaraju vašem upitu");
		}
		else if (numDocuments==1)
		{
			return decode("Naðen <b>1</b> dokument koji odgovara vašem upitu.");
		}
		else if (numDocuments<5) 
		{
			// croatian handle numbers up to 5 differently 
			// For those of you who like to read comments :
			// There are only 4 men in the world in croatian.
			// We say 1 èovjek (man), 2 èovjeka, 3 èovjeka , 4 èovjeka , but 5, 6, 7 ... ljudi (people)
			// That's deep, isn't it :-)?
			return decode("Naðena <b>$num</b> dokumenta koji odgovaraju vašem upitu."
				"Najbolji su prikazani prvi.");
		}
		else 
		{
			return decode("Naðeno <b>$num</b> dokumenata koji odgovaraju vašem upitu."
				"Najbolji su prikazani prvi.");
		}
	}
	/*! This string is put before the list of matched words, for each search 
	*  result. What follows is the list of words that matched the query.
	*/
	virtual QCString trSearchMatches()
	{
		return decode("Pronaðeno:");
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

};

#endif
