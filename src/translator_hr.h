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

#ifndef TRANSLATOR_HR_H
#define TRANSLATOR_HR_H

#include "translator.h"

class TranslatorCroatian : public Translator
{
  public:
    QCString latexBabelPackage()
    { return "croatian"; }
    QCString trRelatedFunctions()
    { return "Povezane funkcije"; }
    QCString trRelatedSubscript()
    { return "(To nisu member funkcije.)"; }
    QCString trDetailedDescription()
    { return "Detaljno obja¹njenje"; }
    QCString trMemberTypedefDocumentation()
    { return "Dokumentacija <code> typedef </code> èlanova"; }
    QCString trMemberEnumerationDocumentation()
    { return "Dokumentacija <code> enum </code> èlanova"; }
    QCString trEnumerationValueDocumentation()
    { return "Dokumentacija <code> enum </code> vrijednosti"; }
    QCString trMemberFunctionDocumentation()
    { return "Dokumentacija funkcija"; }
    QCString trMemberDataDocumentation()
    { return "Documentacija varijabli"; }
    QCString trGeneratedFrom(const char *s,bool single)
    { 
      QCString result=(QCString)"Dokumentacija za "+s+
                     "je generirana iz ";
      if (single) result+="datoteke :";   else result+="slijedeæih datoteka:";
      return result;
    }
    QCString trMore()
    { return "Op¹irnije..."; }
    QCString trListOfAllMembers()
    { return "Popis svih èlanova."; }
    QCString trMemberList()
    { return "Popis èlanova."; }
    QCString trThisIsTheListOfAllMembers()
    { return "Ovo je popis svih èlanova"; }
    QCString trIncludingInheritedMembers()
    { return ", ukljuèujuæi naslijeðene èlanove."; }
    QCString trGeneratedAutomatically(const char *s)
    { QCString result="generirano automatski Doxygen-om"; 
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
    QCString trVerbatimText(const char *f)
    { return (QCString)"Ovo je citirani tekst iz include datoteke "+f+"."; }
    QCString trModules()
    { return "Moduli"; }
    QCString trClassHierarchy()
    { return "Stablo klasa"; }
    QCString trCompoundList()
    { return "Sve klase"; }
    QCString trFileList()
    { return "Popis datoteka"; }
    QCString trHeaderFiles()
    { return "Header datoteke"; }
    QCString trCompoundMembers()
    { return "Svi èlanovi"; }
    QCString trFileMembers()
    { return "Èlanovi(memberi) u datoteci"; }
    QCString trRelatedPages()
    { return "Povezane stranice"; }
    QCString trExamples()
    { return "Primjeri"; }
    QCString trSearch()
    { return "Tra¾i"; }
    QCString trClassHierarchyDescription()
    { return "Stablo naslijeðivanja je poslo¾eno "
             "pribli¾no po abecedi:";
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
      if (!extractAll) result+="dokumentiranih ";
      result+="èlanova klasa s linkovima na ";
      if (extractAll) result+="dokumentaciju svakog èlana:";
      else result+="dokumentaciju klase :";
      return result;
    }
    QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Popis svih ";
      if (!extractAll) result+="dokumentiranih ";
      result+="èlanova s linkovima na ";
      if (extractAll) result+="dokumentaciju datoteke u kojima se nalaze:";
      else result+="datoteke u kojima se nalaze:";
      return result;
    }
    QCString trHeaderFilesDescription()
    { return "Popis header datoteka koje èine API :"; }
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
    { return "Indeks modula"; }
    QCString trHierarchicalIndex()
    { return "Hijerarhijski indeks"; }
    QCString trCompoundIndex()
    { return "Skupni indeks"; }
    QCString trFileIndex() 
    { return "Indeks datoteka"; }
    QCString trModuleDocumentation()
    { return "Dokumentacija modula"; }
    QCString trClassDocumentation()
    { return "Dokumentacija klasa"; }
    QCString trFileDocumentation()
    { return "Dokumentacija datoteka"; }
    QCString trExampleDocumentation()
    { return "Dokumentacija primjera"; }
    QCString trPageDocumentation()
    { return "Dokumentacija za povezane stranice"; }
    QCString trReferenceManual()
    { return "Priruènik"; }

    QCString trDefines()
    { return "Define-ovi"; }
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
    { return "Vrijednosti enum-ova"; }
    QCString trAuthor()
    { return "autor"; }
    QCString trDefineDocumentation()
    { return "Dokumentacija define-a"; }
    QCString trFunctionPrototypeDocumentation()
    { return "Dokumentacija prototipa funkcije"; }
    QCString trTypedefDocumentation()
    { return "Dokumentacija typedef-a"; }
    QCString trEnumerationTypeDocumentation()
    { return "Dokumentacija enum tipa"; }
    QCString trFunctionDocumentation()
    { return "Dokumentacije funkcijs"; }
    QCString trVariableDocumentation()
    { return "Dokumentacija varijable"; }
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
      return "napisao ";
    }
    QCString trClassDiagram(const char *clName)
    {
      return (QCString)"Dijagram klasa za "+clName;
    }
    QCString trForInternalUseOnly()
    { return "Iskljuèivo za internu uporabu."; }
    QCString trReimplementedForInternalReasons()
    { return "Reimplementirano zbog internih razloga; "
             "Nema utjecaja na API."; 
    }
    QCString trWarning()
    { return "Upozorenje"; }
    QCString trBugsAndLimitations()
    { return "Gre¹ke i ogranièenja"; }
    QCString trVersion()
    { return "Verzija"; }
    QCString trDate()
    { return "Datum"; }
    QCString trAuthors()
    { return "Autor(i)"; }
    QCString trReturns()
    { return "Povratne vrijednosti"; }
    QCString trSeeAlso()
    { return "Vidi takoðer"; }
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
    { return "Popis namespace-ova"; }
    QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Popis svih ";
      if (!extractAll) result+="dokumentiranih ";
      result+="namespace-ova s kratkim opisom:";
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
                                    ClassDef::CompoundType compType)
      // used as the title of the HTML page of a class/struct/union
    {
      QCString result="Opis ";
      switch(compType)
      {
        case ClassDef::Class:  result+=" klase "; break;
        case ClassDef::Struct: result+=" strukture "; break;
        case ClassDef::Union:  result+=" unije "; break;
        case ClassDef::Interface:  result+=" suèelja (interface) "; break;
        case ClassDef::Exception:  result+=" iznimke (exception) "; break;
      }
	  result += (QCString)clName;
      
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
      QCString result ="Opis namespace-a ";
	  result+=namespaceName;
      
      return result;
    }
    
    // these are for the member sections of a class, struct or union 
    QCString trPublicMembers()
    { return "Public èlanovi"; }
    QCString trPublicSlots()
    { return "Public slotovi"; }
    QCString trSignals()
    { return "Signali"; }
    QCString trStaticPublicMembers()
    { return "Static public èlanovi"; }
    QCString trProtectedMembers()
    { return "Protected èlanovi"; }
    QCString trProtectedSlots()
    { return "Protected slotovi"; }
    QCString trStaticProtectedMembers()
    { return "Static protected èlanovi"; }
    QCString trPrivateMembers()
    { return "Private èlanovi"; }
    QCString trPrivateSlots()
    { return "Private slotovi"; }
    QCString trStaticPrivateMembers()
    { return "Static private èlanovi"; }
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
      return "Naslijeðuje od "+trWriteList(numEntries)+".";
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
    { return "Dokumentacija namespace-a"; }

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
      QCString result=(QCString)"Dokumentacija ove ";
      switch(compType)
      {
        case ClassDef::Class:      result+="klase"; break;
        case ClassDef::Struct:     result+="strukture"; break;
        case ClassDef::Union:      result+="unije"; break;
        case ClassDef::Interface:  result+="suèelja (interface)"; break;
        case ClassDef::Exception:  result+="iznimke (exception)"; break;
      }
      result+=" je generirana iz datotek";
      if (single) result+="e :"; else result+="a :";
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
      return (QCString)"Kolaboracijski dijagram za "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const char *fName)
    {
      return (QCString)"Graf include meðuovisnosti za "+fName+":";
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
    /*! Text for the \pre command */
    QCString trPrecondition()
    {
      return "Preduvjeti";
    }
    /*! Text for the \post command */
    QCString trPostcondition()
    {
      return "Postuvjeti";
    }
    /*! Text for the \invariant command */
    QCString trInvariant()
    {
      return "Invarijanta";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue()
    {
      return "Poèetna vrijednost:";
    }
    /*! Text used the source code in the file index */
    QCString trCode()
    {
      return "kod";
    }
    QCString trGraphicalHierarchy()
    {
      return "Grafièko stablo klasa";
    }
    QCString trGotoGraphicalHierarchy()
    {
      return "Grafièko stablo klasa";
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
// new since 1.1.1
//////////////////////////////////////////////////////////////////////////

};

#endif
