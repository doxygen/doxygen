/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
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

// translation by Matjaz Ostroversnik <matjaz.ostroversnik@zrs-tk.si>

#ifndef TRANSLATOR_SI_H
#define TRANSLATOR_SI_H

#include "translator_adapter.h"

class TranslatorSlovene : public TranslatorAdapter_1_1_5
{
  public:
    QCString idLanguage()
    { return "slovene"; }
    /*! Used to get the command(s) for the language support. This method
     *  was designed for languages which do not prefer babel package.
     *  If this methods returns empty string, then the latexBabelPackage()
     *  method is used to generate the command for using the babel package.
     */
    QCString latexLanguageSupportCommand()
    {
      return "\\usepackage[slovene]{babel} \n\\usepackage[latin2]{inputenc} \n\\usepackage[T1]{fontenc}\n";
    }
    QCString latexBabelPackage()
    { return "slovene"; }
    QCString idLanguageCharset()
    { 
#ifdef _WIN32
      return "windows-1250"; 
#else
      return "iso-8859-2"; 
#endif
    }
    QCString trRelatedFunctions()
    { return "Povezane funkcije"; }
    QCString trRelatedSubscript()
    { return "(To niso metode.)"; }
    QCString trDetailedDescription()
    { return "Podroben opis"; }
    QCString trMemberTypedefDocumentation()
    { return "Opis <code> uporabni¹ko definiranih tipov (typedef) </code>"; }
    QCString trMemberEnumerationDocumentation()
    { return "Opis <code>  na¹tevnih tipov </code> èlanova"; }
    QCString trEnumerationValueDocumentation()
    { return "Opis <code> na¹tevnih vednosti (enum) </code> "; }
    QCString trMemberFunctionDocumentation()
    { return "Opis metod"; }
    QCString trMemberDataDocumentation()
    { return "Opis atributov"; }
    QCString trMore()
    { return "..."; }
    QCString trListOfAllMembers()
    { return "Seznam vseh metod / atributov."; }
    QCString trMemberList()
    { return " - Seznam metod in atributov."; }
    QCString trThisIsTheListOfAllMembers()
    { return "To je seznam metod razreda "; }
    QCString trIncludingInheritedMembers()
    { return ", vkljuèujoè dedovane metode in atribute."; }
    QCString trGeneratedAutomatically(const char *s)
    { QCString result="zgenerirano avtomatièno z Doxygen-om"; 
      if (s) result+=(QCString)" za "+s;
      result+=" iz programskog koda."; 
      return result;
    }
    QCString trEnumName()
    { return "enum ime"; }
    QCString trEnumValue()
    { return "enum vrednost"; }
    QCString trDefinedIn()
    { return "definirano v"; }
    QCString trVerbatimText(const char *f)
    { return (QCString)"Ovo je citirani tekst iz include datoteke "+f+"."; }
    QCString trModules()
    { return "Moduli"; }
    QCString trClassHierarchy()
    { return "dedovalna hierarhija"; }
    QCString trCompoundList()
    { return "kratek opis razredov"; }
    QCString trFileList()
    { return "seznam datotek"; }
    QCString trHeaderFiles()
    { return "\"Header\" datoteka"; }
    QCString trCompoundMembers()
    { return "metode in atributi"; }
    QCString trFileMembers()
    { return "komponente v datotekah"; }
    QCString trRelatedPages()
    { return "Povezane strani"; }
    QCString trExamples()
    { return "Primeri"; }
    QCString trSearch()
    { return "I¹èi"; }
    QCString trClassHierarchyDescription()
      { return "Hierarhièno drevo je (okvirno) sortirano po abecedi. ";
    }
    QCString trFileListDescription(bool extractAll)
    {
      QCString result="Seznam vseh ";
      if (!extractAll) result+="dokumentiranih ";
      result+="datotek, s kratkim opisom:";
      return result;
    }
    QCString trCompoundListDescription()
    { return "Seznam razredov, mno¾ic in struktur "
             "s kratkim opisom :"; 
    }
    QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="Seznam vseh ";
      if (!extractAll) result+="dokumentiranih ";
      result+="metod in atributov s povezavami na ";
      if (extractAll) result+="opis posamezne metode in/ali atributa:";
      else result+="opis razreda :";
      return result;
    }
    QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Seznam ";
      if (!extractAll) result+="dokumentiranih ";
      result+="entite v datotekah ";
      if (extractAll) result+="skupaj z opisom datoteke v kateri se nahajajo:";
      else result+="z linki na datoteke v katerih se nahajajo:";
      return result;
    }
    QCString trHeaderFilesDescription()
    { return "Seznam header datotek, ki tvoriju aplikacijski vmesnik (API) :"; }
    QCString trExamplesDescription()
    { return "Seznam primerov :"; }
    QCString trRelatedPagesDescription()
    { return "Seznam povezanih strani:"; }
    QCString trModulesDescription()
    { return "Seznam modulov:"; }
    QCString trNoDescriptionAvailable()
    { return "Opis ni dostopen"; }

    QCString trDocumentation()
    { return "Dokumentacija"; }
    QCString trModuleIndex()
    { return "seznam modulov"; }
    QCString trHierarchicalIndex()
    { return "Hierarhièni indeks"; }
    QCString trCompoundIndex()
    { return "abecedni seznam"; }
    QCString trFileIndex() 
    { return "seznam datotek"; }
    QCString trModuleDocumentation()
    { return "Dokumentacija modulov"; }
    QCString trClassDocumentation()
    { return "Opis razreda"; }
    QCString trFileDocumentation()
    { return "Opis datoteke"; }
    QCString trExampleDocumentation()
    { return "Opis primera"; }
    QCString trPageDocumentation()
    { return "Dokumentacija povezanih strani"; }
    QCString trReferenceManual()
    { return "Priroènik"; }

    QCString trDefines()
    { return "Makro deklaracije"; }
    QCString trFuncProtos()
    { return "Prototipi funkcij"; }
    QCString trTypedefs()
    { return "Uporabni¹ko definirani tipi"; }
    QCString trEnumerations()
    { return "Na¹tevni tipi"; }
    QCString trFunctions()
    { return "Funkcije"; }
    QCString trVariables()
    { return "Spremenljivke"; }
    QCString trEnumerationValues()
    { return "Vrednosti na¹tevnih tipov"; }
    QCString trAuthor()
    { return "avtor"; }
    QCString trDefineDocumentation()
    { return "Opis makro definicije"; }
    QCString trFunctionPrototypeDocumentation()
    { return "Opis prototipa funkcije"; }
    QCString trTypedefDocumentation()
    { return "Opis uporabni¹ko definiranega tipa"; }
    QCString trEnumerationTypeDocumentation()
    { return "Opis na¹tevnega (enum) tipa"; }
    QCString trFunctionDocumentation()
    { return "Opis funkcije"; }
    QCString trVariableDocumentation()
    { return "Opis spremenljivke"; }
    QCString trCompounds()
    { return "Strukture"; }
    QCString trFiles()
    { return "Datoteke"; }
    QCString trGeneratedAt(const char *date,const char *projName)
    { 
      QCString result=(QCString)"Generirano "+date;
      if (projName) result+=(QCString)" projekt: "+projName;
      result+=(QCString)" generator: ";
      return result;
    }
    QCString trWrittenBy()
    {
      return "napisal ";
    }
    QCString trClassDiagram(const char *clName)
    {
      return (QCString)"Diagram razredov za "+clName;
    }
    QCString trForInternalUseOnly()
    { return "Samo za interno uporabo."; }
    QCString trReimplementedForInternalReasons()
    { return "Reimplementirano zbog internih razloga; "
             "Nema utjecaja na API."; 
    }
    QCString trWarning()
    { return "Opozorilo"; }
    QCString trBugsAndLimitations()
    { return "Napake in omejtive"; }
    QCString trVersion()
    { return "Verzija"; }
    QCString trDate()
    { return "Datum"; }
    QCString trAuthors()
    { return "Avtor(ji)"; }
    QCString trReturns()
    { return "Rezultat(i)"; }
    QCString trSeeAlso()
    { return "Glej"; }
    QCString trParameters()
    { return "Parametri"; }
    QCString trExceptions()
    { return "Izjeme (Exceptions)"; }
    QCString trGeneratedBy()
    { return "Izdelano s pomoèjo"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307 
//////////////////////////////////////////////////////////////////////////
    
    QCString trNamespaceList()
    { return "Seznam imenskih prostorov"; }
    QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Seznam ";
      if (!extractAll) result+="dokumentiranih ";
      result+="imenskih prostorov z opisom:";
      return result;
    }
    QCString trFriends()
    { return "Prijatelji (Friends) "; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
    QCString trRelatedFunctionDocumentation()
    { return "Podatki o poveznih funkcijah"; }
    
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
        case ClassDef::Class:  result+=" razreda "; break;
        case ClassDef::Struct: result+=" strukture "; break;
        case ClassDef::Union:  result+=" unije "; break;
        case ClassDef::Interface:  result+=" vmesnika (interface) "; break;
        case ClassDef::Exception:  result+=" izjeme (exception) "; break;
      }
	  result += (QCString)clName;
      
      return result;
    }
    QCString trFileReference(const char *fileName)
      // used as the title of the HTML page of a file
    {
      QCString result="Vsebina datoteke "; 
	  result+=fileName;
	  return result;
    }
    QCString trNamespaceReference(const char *namespaceName)
      // used as the title of the HTML page of a namespace
    {
      QCString result ="Opis imenskega prostora ";
	  result+=namespaceName;
      
      return result;
    }
    
    // these are for the member sections of a class, struct or union 
    QCString trPublicMembers()
    { return "Javne metode"; }
    QCString trPublicSlots()
    { return "Public slotovi"; }
    QCString trSignals()
    { return "Programske prekinitve"; }
    QCString trStaticPublicMembers()
    { return "Statiène javne metode in atributi"; }
    QCString trProtectedMembers()
    { return "Za¹èitene metode in atributi"; }
    QCString trProtectedSlots()
    { return "Za¹èiteni sloti"; }
    QCString trStaticProtectedMembers()
    { return "Statiène za¹èitene metode in atributi"; }
    QCString trPrivateMembers()
    { return "Skrite metode in atributi"; }
    QCString trPrivateSlots()
    { return "Skriti slotovi"; }
    QCString trStaticPrivateMembers()
    { return "Statiène skrite metode in atributi"; }
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
            result+=" in ";
        }
      }
      return result; 
    }
    
    QCString trInheritsList(int numEntries)
      // used in class documentation to produce a list of base classes,
      // if class diagrams are disabled.
    {
      return "Deduje od "+trWriteList(numEntries)+".";
    }
    QCString trInheritedByList(int numEntries)
      // used in class documentation to produce a list of super classes,
      // if class diagrams are disabled.
    {
      return "Naslijeðena u "+trWriteList(numEntries)+".";
    }
    QCString trReimplementedFromList(int numEntries)
      // used in member documentation blocks to produce a list of 
      // members that are hidden by this one.
    {
      return "Redefinirano v "+trWriteList(numEntries)+".";
    }
    QCString trReimplementedInList(int numEntries)
    {
      // used in member documentation blocks to produce a list of
      // all member that overwrite the implementation of this member.
      return "Reimplementacija u "+trWriteList(numEntries)+".";
    }

    QCString trNamespaceMembers()
      // This is put above each page as a link to all members of namespaces.
    { return "Namespace èlanovi"; }
    QCString trNamespaceMemberDescription(bool extractAll)
      // This is an introduction to the page with all namespace members
    { 
      QCString result="Lista svih ";
      if (!extractAll) result+="dokumentiranih ";
      result+="namespace èlanova s linkovima na ";
      if (extractAll) 
        result+="namespace dokumentaciju svakog èlana:";
      else 
        result+="namespace kojima pripadaju:";
      return result;
    }
    QCString trNamespaceIndex()
      // This is used in LaTeX as the title of the chapter with the 
      // index of all namespaces.
    { return "Indeks namespace-a"; }
    QCString trNamespaceDocumentation()
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all namespaces.
    { return "Podati o imenskih prostorih"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces()
    {
      return "Namespaces";
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
      QCString result=(QCString)"Opis ";
      switch(compType)
      {
        case ClassDef::Class:      result+="razreda"; break;
        case ClassDef::Struct:     result+="strukture"; break;
        case ClassDef::Union:      result+="unije"; break;
        case ClassDef::Interface:  result+="vmesnika (interface)"; break;
        case ClassDef::Exception:  result+="izjeme (exception)"; break;
      }
      result+=" je zgrajen na podlagi naslednj";
      if (single) result+="e "; else result+="ih";
      result+=" datotek";
      if (single) result+="e :"; else result+=" :";
      return result;
    }

    /*! This is in the (quick) index as a link to the alphabetical compound
     * list.
     */
    QCString trAlphabeticalList()
    { return "abecedni seznam"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    QCString trReturnValues()
    { return "Povratna vrednost"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage()
    { return "prva stran"; }

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
      return "Definirano v @0 vrstici datoteke @1.";
    }
    QCString trDefinedInSourceFile()
    {
      return "Definirano v datoteki @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated()
    {
      return "Zastarelo";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)"Kolaboracijski diagram razreda "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const char *fName)
    {
      return (QCString)"Graf prikazuje seznam datotek, "
          "ki jih datoteka \""+fName+"\" "
          "direktno ali indirektno vkljuèuje. Pravokotniki ponazarjajo datoteke, pu¹èice "
	  "predstavljajo relacije med njimi. "
	  "Èrn pravokotnik ponazarja datoteko "+fName+". Pu¹èice A->B ponazarjajo "
	  "usmerjeno relacijo \"A vkljuèuje B\"."
;
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation()
    {
      return "Opis konstruktorjev in destruktorjev "; 
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode()
    {
      return "izvorna koda";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation()
    {
      return "dokumenacija tekoèe datoteke.";
    }
    /*! Text for the \pre command */
    QCString trPrecondition()
    {
      return "Predpogoji (preconditions)";
    }
    /*! Text for the \post command */
    QCString trPostcondition()
    {
      return "Naknadni pogoji (posconditions)";
    }
    /*! Text for the \invariant command */
    QCString trInvariant()
    {
      return "Invarianta";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue()
    {
      return "Zaèetna vrednost / definicija :";
    }
    /*! Text used the source code in the file index */
    QCString trCode()
    {
      return "koda";
    }
    QCString trGraphicalHierarchy()
    {
      return "Hierarhija razredov v grafièni obliki";
    }
    QCString trGotoGraphicalHierarchy()
    {
      return "Dedovalna hierarhija v grafièni obliki";
    }
    QCString trGotoTextualHierarchy()
    {
      return "Dedovalna hierarhija v tekstovni obliki";
    }
    QCString trPageIndex()
    {
      return "Indeks strani";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////
    
    QCString trNote()
    {
      return "Opomba";
    }
    QCString trPublicTypes()
    {
      return "Javni tipi";
    }
    QCString trPublicAttribs()
    {
      return "Javni atributi";
    }
    QCString trStaticPublicAttribs()
    {
      return "Statièni javni atributi";
    }
    QCString trProtectedTypes()
    {
      return "Za¹èiteni tipi";
    }
    QCString trProtectedAttribs()
    {
      return "Za¹èiteni atributi";
    }
    QCString trStaticProtectedAttribs()
    {
      return "Statièni za¹èiteni tipi";
    }
    QCString trPrivateTypes()
    {
      return "Skriti tipi";
    }
    QCString trPrivateAttribs()
    {
      return "Skriti atributi";
    }
    QCString trStaticPrivateAttribs()
    {
      return "Statièni skriti atributi";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    QCString trTodo()
    {
      return "TODO";
    }
    /*! Used as the header of the todo list */
    QCString trTodoList()
    {
      return "Seznam nedokonèanih opravil";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy()
    {
      return "Uporabniki entitete:  ";
    }
    QCString trRemarks()
    {
      return "Opomba";
    }
    QCString trAttention()
    {
      return "Pozor";
    }
    QCString trInclByDepGraph()
    {
      return "Graf prikazuje datoteke, ki posredno ali neposredno "
             "vkljuèujejo tekoèo datoteko. Pravokotniki simbolizirajo datoteke, "
             "pu¹èice pa relacije med datotekami. Tekoèa datoteka je prikazana "
	     "kot pravokotnik s èrno podlago, ostale pa kot pravokotnik brez podlage. "
	     "Smer pu¹èice A->B definira relacijo \"A vkljuèuje B\". "
	     "Vse datoteke, ki torej mejijo na tekoèo (t.j. obstaja povezava med èrnim in "
	     "praznim pravokotnikom), jo direktno vkljuèujejo, medtem, ko jo ostale vkljuèujejo "
	     "le posredno. "
	;
    }
    QCString trSince()
    {
      return "Od";
    }


};

#endif








