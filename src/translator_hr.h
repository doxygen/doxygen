/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
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
// translation by Boris Bralo <boris.bralo@zg.tel.hr>
// Updates:
// --------
// 2000/08/20 
//  - Better handling of ISO-8859-2/ WIN 1250 stuff based on (actualy stolen from :-)) Czech translations
//    implemented by Petr Prikryl (prikrylp@skil.cz).
//    As opposed to Czech translation this one assumes that Doxygen strings are written in Linux ( it's true,
//    I don't have QT pro licence ) , and uses ISOToWin function when built in WIN32
//
#ifndef TRANSLATOR_HR_H
#define TRANSLATOR_HR_H

#include "translator.h"

class TranslatorCroatian : public Translator
{
  private:
        /*! to avoid macro redefinition from translator_cz.h */
        /*  Speed loss is actualy less than in Czech version due to
                fewer QCString copying. 
         */
        inline QCString decode(const QCString& sInput)
        { 
#ifdef _WIN32
                return ISO88592ToWin1250(sInput);
#else
                return sInput;
#endif
        }
  
  public:
    QCString idLanguage()
    { return "croatian"; }
    QCString idLanguageCharset()
#ifdef _WIN32
        { return "windows-1250"; }
#else
        { return "iso-8859-2"; }
#endif
    QCString latexBabelPackage()
    { return "croatian"; }
    QCString trRelatedFunctions()
    { return decode("Povezane funkcije"); }
    QCString trRelatedSubscript()
    { return decode("(To nisu member funkcije.)"); }
    QCString trDetailedDescription()
    { return decode("Detaljno obja¹njenje"); }
    QCString trMemberTypedefDocumentation()
    { return decode("Dokumentacija typedef èlanova"); }
    QCString trMemberEnumerationDocumentation()
    { return decode("Dokumentacija enum èlanova"); }
    QCString trEnumerationValueDocumentation()
    { return decode("Dokumentacija enum vrijednosti"); }
    QCString trMemberFunctionDocumentation()
        { return decode("Dokumentacija funkcija"); }
    QCString trMemberDataDocumentation()
    { return decode("Documentacija varijabli"); }
    QCString trGeneratedFrom(const char *s,bool single)
    { 
      QCString result= decode("Dokumentacija za ")+s+
                       decode("je generirana iz ");
      if (single)
                  result+= decode("datoteke :");
          else
                  result+= decode("slijedeæih datoteka:");
      return result;
    }
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
      if (s) result+=(QCString)decode(" za ")+s;
      result+=decode(" iz programskog koda."); 
      return result;
    }
    QCString trEnumName()
    { return decode("enum ime"); }
    QCString trEnumValue()
    { return decode("enum vrijednost"); }
    QCString trDefinedIn()
    { return decode("definirano u"); }
    QCString trVerbatimText(const char *f)
    { return (QCString)decode("Ovo je citirani tekst iz include datoteke ")+f+"."; }
    QCString trModules()
    { return decode("Moduli"); }
    QCString trClassHierarchy()
    { return decode("Stablo klasa"); }
    QCString trCompoundList()
    { return decode("Sve klase"); }
    QCString trFileList()
    { return decode("Popis datoteka"); }
    QCString trHeaderFiles()
    { return decode("Header datoteke"); }
    QCString trCompoundMembers()
    { return decode("Svi èlanovi"); }
    QCString trFileMembers()
    { return decode("Èlanovi(memberi) u datoteci"); }
    QCString trRelatedPages()
    { return decode("Povezane stranice"); }
    QCString trExamples()
    { return decode("Primjeri"); }
    QCString trSearch()
    { return decode("Tra¾i"); }
    QCString trClassHierarchyDescription()
    { return decode("Stablo naslijeðivanja je poslo¾eno "
                                        "pribli¾no po abecedi:");
    }
    QCString trFileListDescription(bool extractAll)
    {
      QCString result=decode("Popis svih ");
      if (!extractAll) result+=decode("dokumentiranih ");
      result+=decode("datoteka, s kratkim opisom:");
      return result;
    }
    QCString trCompoundListDescription()
    { return decode("Popis svih klasa, unija i struktura "
                                        "s kratkim opisom :"); 
    }
    QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result=decode("Popis svih ");
      if (!extractAll) result+=decode("dokumentiranih ");
      result+=decode("èlanova klasa s linkovima na ");
      if (extractAll) result+=decode("dokumentaciju svakog èlana:");
      else result+=decode("dokumentaciju klase :");
      return result;
    }
    QCString trFileMembersDescription(bool extractAll)
    {
      QCString result=decode("Popis svih ");
      if (!extractAll) result+=decode("dokumentiranih ");
      result+=decode("èlanova s linkovima na ");
      if (extractAll) result+=decode("dokumentaciju datoteke u kojima se nalaze:");
      else result+=decode("datoteke u kojima se nalaze:");
      return result;
    }
    QCString trHeaderFilesDescription()
    { return decode("Popis header datoteka koje èine API :"); }
    QCString trExamplesDescription()
    { return decode("Popis primjera :"); }
    QCString trRelatedPagesDescription()
    { return decode("Popis povezanih stranica:"); }
    QCString trModulesDescription()
    { return decode("Popis svih modula:"); }
    QCString trNoDescriptionAvailable()
    { return decode("Opis nije dostupan"); }

    QCString trDocumentation()
    { return decode("Dokumentacija"); }
    QCString trModuleIndex()
    { return decode("Indeks modula"); }
    QCString trHierarchicalIndex()
    { return decode("Hijerarhijski indeks"); }
    QCString trCompoundIndex()
    { return decode("Skupni indeks"); }
    QCString trFileIndex() 
    { return decode("Indeks datoteka"); }
    QCString trModuleDocumentation()
    { return decode("Dokumentacija modula"); }
    QCString trClassDocumentation()
    { return decode("Dokumentacija klasa"); }
    QCString trFileDocumentation()
    { return decode("Dokumentacija datoteka"); }
    QCString trExampleDocumentation()
    { return decode("Dokumentacija primjera"); }
    QCString trPageDocumentation()
    { return decode("Dokumentacija za povezane stranice"); }
    QCString trReferenceManual()
    { return decode("Priruènik"); }

    QCString trDefines()
    { return decode("Define-ovi"); }
    QCString trFuncProtos()
    { return decode("Prototipi funkcija"); }
    QCString trTypedefs()
    { return decode("Typedef-ovi"); }
    QCString trEnumerations()
    { return decode("Enumeracije"); }
    QCString trFunctions()
    { return decode("Funkcije"); }
    QCString trVariables()
    { return decode("Varijable"); }
    QCString trEnumerationValues()
    { return decode("Vrijednosti enum-ova"); }
    QCString trAuthor()
    { return decode("autor"); }
    QCString trDefineDocumentation()
    { return decode("Dokumentacija define-a"); }
    QCString trFunctionPrototypeDocumentation()
    { return decode("Dokumentacija prototipa funkcije"); }
    QCString trTypedefDocumentation()
    { return decode("Dokumentacija typedef-a"); }
    QCString trEnumerationTypeDocumentation()
    { return decode("Dokumentacija enum tipa"); }
    QCString trFunctionDocumentation()
    { return decode("Dokumentacije funkcijs"); }
    QCString trVariableDocumentation()
    { return decode("Dokumentacija varijable"); }
    QCString trCompounds()
    { return decode("Strukture"); }
    QCString trFiles()
    { return decode("Datoteke"); }
    QCString trGeneratedAt(const char *date,const char *projName)
    { 
      QCString result=(QCString)decode("Generirano ")+date;
      if (projName) result+=(QCString)decode(" projekt: ")+projName;
      result+=(QCString)decode(" generator: ");
      return result;
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
    { return decode("Upozorenje"); }
    QCString trBugsAndLimitations()
    { return decode("Gre¹ke i ogranièenja"); }
    QCString trVersion()
    { return decode("Verzija"); }
    QCString trDate()
    { return decode("Datum"); }
    QCString trAuthors()
    { return decode("Autor(i)"); }
    QCString trReturns()
    { return decode("Povratne vrijednosti"); }
    QCString trSeeAlso()
    { return decode("Vidi takoðer"); }
    QCString trParameters()
    { return decode("Parametri"); }
    QCString trExceptions()
    { return decode("Iznimke"); }
    QCString trGeneratedBy()
    { return decode("Generirao"); }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307 
//////////////////////////////////////////////////////////////////////////
    
    QCString trNamespaceList()
    { return decode("Popis namespace-ova"); }
    QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result=decode("Popis svih ");
      if (!extractAll) result+=decode("dokumentiranih ");
      result+=decode("namespace-ova s kratkim opisom:");
      return result;
    }
    QCString trFriends()
    { return decode("Friend-ovi "); }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
    QCString trRelatedFunctionDocumentation()
    { return decode("Dokumentacija povezanih funkcija"); }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    QCString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType,
                                    bool /*isTemplate*/)
      // used as the title of the HTML page of a class/struct/union
    {
      QCString result=decode("Opis ");
      switch(compType)
      {
        case ClassDef::Class:  result+=decode(" klase "); break;
        case ClassDef::Struct: result+=decode(" strukture "); break;
        case ClassDef::Union:  result+=decode(" unije "); break;
        case ClassDef::Interface:  result+=decode(" suèelja (interface) "); break;
        case ClassDef::Exception:  result+=decode(" iznimke (exception) "); break;
      }
          result += (QCString)clName;
      
      return result;
    }
    QCString trFileReference(const char *fileName)
      // used as the title of the HTML page of a file
    {
      QCString result=decode("Opis datoteke "); 
          result+=fileName;
          return result;
    }
    QCString trNamespaceReference(const char *namespaceName)
      // used as the title of the HTML page of a namespace
    {
      QCString result =decode("Opis namespace-a ");
          result+=namespaceName;
      
      return result;
    }
    
    // these are for the member sections of a class, struct or union 
    QCString trPublicMembers()
    { return decode("Public èlanovi"); }
    QCString trPublicSlots()
    { return decode("Public slotovi"); }
    QCString trSignals()
    { return decode("Signali"); }
    QCString trStaticPublicMembers()
    { return decode("Static public èlanovi"); }
    QCString trProtectedMembers()
    { return decode("Protected èlanovi"); }
    QCString trProtectedSlots()
    { return decode("Protected slotovi"); }
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
    { return decode("Namespace èlanovi"); }
    QCString trNamespaceMemberDescription(bool extractAll)
      // This is an introduction to the page with all namespace members
    { 
      QCString result=decode("Lista svih ");
      if (!extractAll) result+=decode("dokumentiranih ");
      result+=decode("namespace èlanova s linkovima na ");
      if (extractAll) 
        result+=decode("namespace dokumentaciju svakog èlana:");
      else 
        result+=decode("namespace kojima pripadaju:");
      return result;
    }
    QCString trNamespaceIndex()
      // This is used in LaTeX as the title of the chapter with the 
      // index of all namespaces.
    { return decode("Indeks namespace-a"); }
    QCString trNamespaceDocumentation()
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all namespaces.
    { return decode("Dokumentacija namespace-a"); }

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
      QCString result=(QCString)decode("Dokumentacija ove ");
      switch(compType)
      {
        case ClassDef::Class:      result+=decode("klase"); break;
        case ClassDef::Struct:     result+=decode("strukture"); break;
        case ClassDef::Union:      result+=decode("unije"); break;
        case ClassDef::Interface:  result+=decode("suèelja (interface)"); break;
        case ClassDef::Exception:  result+=decode("iznimke (exception)"); break;
      }
      result+=decode(" je generirana iz datotek");
      if (single) result+="e :"; else result+="a :";
      return result;
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    QCString trAlphabeticalList()
    { return decode("Abecedni popis"); }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    QCString trReturnValues()
    { return decode("Povratna vrijednost"); }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage()
    { return decode("Glavna stranica"); }

    /*! This is used in references to page that are put in the LaTeX 
     *  documentation. It should be an abbreviation of the word page.
     */
    QCString trPageAbbreviation()
    { return decode("str."); }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991106
//////////////////////////////////////////////////////////////////////////

    QCString trSources()
    {
      return decode("Izvorne datoteke");
    }
    QCString trDefinedAtLineInSourceFile()
    {
      return decode("Definirano u liniji @0 datoteke @1.");
    }
    QCString trDefinedInSourceFile()
    {
      return decode("Definirano u datoteci @0.");
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated()
    {
      return decode("Zastarjelo");
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
      return decode("Dokumentacija konstruktora i destruktora "); 
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode()
    {
      return decode("Izvorni kod");
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation()
    {
      return decode("Dokumenacija za ovu datoteku.");
    }
    /*! Text for the \pre command */
    QCString trPrecondition()
    {
      return decode("Preduvjeti");
    }
    /*! Text for the \post command */
    QCString trPostcondition()
    {
      return decode("Postuvjeti");
    }
    /*! Text for the \invariant command */
    QCString trInvariant()
    {
      return decode("Invarijanta");
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue()
    {
      return decode("Poèetna vrijednost:");
    }
    /*! Text used the source code in the file index */
    QCString trCode()
    {
      return decode("kod");
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
      return decode("Tekstualno stablo klasa");
    }
    QCString trPageIndex()
    {
      return decode("Indeks stranice");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////
    
    QCString trNote()
    {
      return decode("Primjedba");
    }
    QCString trPublicTypes()
    {
      return decode("Public tipovi");
    }
    QCString trPublicAttribs()
    {
      return decode("Public atributi");
    }
    QCString trStaticPublicAttribs()
    {
      return decode("Static public atributi");
    }
    QCString trProtectedTypes()
    {
      return decode("Protected tipovi");
    }
    QCString trProtectedAttribs()
    {
      return decode("Protected atributi");
    }
    QCString trStaticProtectedAttribs()
    {
      return decode("Static protected atributi");
    }
    QCString trPrivateTypes()
    {
      return decode("Private tipovi");
    }
    QCString trPrivateAttribs()
    {
      return decode("Private atributi");
    }
    QCString trStaticPrivateAttribs()
    {
      return decode("Static private atributi");
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    virtual QCString trTodo()
    {
      return decode("Za uraditi");
    }
    /*! Used as the header of the todo list */
    virtual QCString trTodoList()
    {
      return decode("Ostalo za uraditi");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    virtual QCString trReferencedBy()
    {
      return decode("Referencirano od");
    }
    virtual QCString trRemarks()
    {
      return decode("Napomene");
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
      return decode("Od");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    virtual QCString trLegendTitle()
    {
      return decode("Legenda");
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
        "<p><center><img src=\"graph_legend.gif\"></center>\n"
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
      return decode("legenda");
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
};

#endif
