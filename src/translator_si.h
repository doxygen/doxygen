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

// translation by Matjaz Ostroversnik <matjaz.ostroversnik@zrs-tk.si>

#ifndef TRANSLATOR_SI_H
#define TRANSLATOR_SI_H


class TranslatorSlovene : public TranslatorAdapter_1_4_6
{
  public:
    QCString idLanguage() override
    { return "slovene"; }
    /*! Used to get the command(s) for the language support. This method
     *  was designed for languages which do not prefer babel package.
     *  If this methods returns empty string, then the latexBabelPackage()
     *  method is used to generate the command for using the babel package.
     */
    QCString latexLanguageSupportCommand() override
    { return "\\usepackage[slovene]{babel}\n"; }
    QCString trISOLang() override
    { return "sl"; }
    QCString getLanguageString() override
    {
      return "0x424 Slovenian";
    }
    QCString trRelatedFunctions() override
    { return "Povezane funkcije"; }
    QCString trRelatedSubscript() override
    { return "(To niso metode.)"; }
    QCString trDetailedDescription() override
    { return "Podroben opis"; }
    QCString trDetails() override
    { return "Podrobnosti"; }

    QCString trMemberTypedefDocumentation() override
    { return "Opis uporabniško definiranih tipov"; }
    QCString trMemberEnumerationDocumentation() override
    { return "Opis komponent  naštevnih tipov"; }
/*     QCString trEnumerationValueDocumentation() */
/*     { return "Opis vrednosti naštevnih tipov (enum) "; } */
    QCString trMemberFunctionDocumentation() override
    { return "Opis metod"; }
    QCString trMemberDataDocumentation() override
    { return "Opis atributov"; }
    QCString trMore() override
    { return "..."; }
    QCString trListOfAllMembers() override
    { return "Seznam vseh metod / atributov"; }
    QCString trMemberList() override
    { return " - seznam metod in atributov."; }
    QCString trThisIsTheListOfAllMembers() override
    { return "Seznam metod razreda"; }
    QCString trIncludingInheritedMembers() override
    { return ", vključujoč dedovane metode in atribute."; }
    QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="zgenerirano z Doxygen-om";
      if (!s.isEmpty()) result+=" za "+s;
      result+=" iz izvorne kode.";
      return result;
    }
    QCString trEnumName() override
    { return "naštevno ime"; }
    QCString trEnumValue() override
    { return "naštevna vrednost"; }
    QCString trDefinedIn() override
    { return "definirano v"; }
    QCString trModules() override
    { return "moduli"; }
    QCString trClassHierarchy() override
    { return "dedovalna hierarhija"; }
    QCString trCompoundList() override
    { return "kratek opis razredov"; }
    QCString trFileList() override
    { return "seznam datotek"; }
    QCString trCompoundMembers() override
    { return "metode in atributi"; }
    QCString trFileMembers() override
    { return "komponente v datotekah"; }
    QCString trRelatedPages() override
    { return "dodatni opisi"; }
    QCString trExamples() override
    { return "Primeri"; }
    QCString trSearch() override
    { return "Išči"; }
    QCString trClassHierarchyDescription() override
      { return "Hierarhično drevo je (okvirno) sortirano po abecedi. ";
    }
    QCString trFileListDescription(bool extractAll) override
    {
      QCString result="Seznam vseh ";
      if (!extractAll) result+="dokumentiranih ";
      result+="datotek s kratkim opisom:";
      return result;
    }
    QCString trCompoundListDescription() override
    { return "Seznam razredov, množic in struktur "
             "s kratkim opisom :";
    }
    QCString trCompoundMembersDescription(bool extractAll) override
    {
      QCString result="Seznam vseh ";
      if (!extractAll) result+="dokumentiranih ";
      result+="metod in atributov s povezavami na ";
      if (extractAll) result+="opis posamezne metode in/ali atributa:";
      else result+="opis razreda :";
      return result;
    }
    QCString trFileMembersDescription(bool extractAll) override
    {
      QCString result="Seznam ";
      if (!extractAll) result+="dokumentiranih ";
      result+="entitet v datotekah ";
      if (extractAll) result+="skupaj z opisom datoteke v kateri se nahajajo:";
      else result+="s povezavami na datoteke v katerih se nahajajo:";
      return result;
    }
    QCString trExamplesDescription() override
    { return "Seznam primerov :"; }
    QCString trRelatedPagesDescription() override
    { return "Seznam strani z dodatnimi opisi:"; }
    QCString trModulesDescription() override
    { return "Seznam modulov:"; }
    QCString trDocumentation(const QCString &projName) override
    { return (!projName.isEmpty()?projName + " " : "") + "Dokumentacija"; }
    QCString trModuleIndex() override
    { return "seznam modulov"; }
    QCString trHierarchicalIndex() override
    { return "Hierarhični indeks"; }
    QCString trCompoundIndex() override
    { return "abecedni seznam"; }
    QCString trFileIndex() override
    { return "seznam datotek"; }
    QCString trModuleDocumentation() override
    { return "Dokumentacija modulov"; }
    QCString trClassDocumentation() override
    { return "Opis razreda"; }
    QCString trFileDocumentation() override
    { return "Opis datoteke"; }
    QCString trReferenceManual() override
    { return "Priročnik"; }

    QCString trDefines() override
    { return "Makro deklaracije"; }
    QCString trTypedefs() override
    { return "Uporabniško definirani tipi"; }
    QCString trEnumerations() override
    { return "Naštevni tipi"; }
    QCString trFunctions() override
    { return "Funkcije"; }
    QCString trVariables() override
    { return "Spremenljivke"; }
    QCString trEnumerationValues() override
    { return "Vrednosti naštevnih tipov"; }
    QCString trDefineDocumentation() override
    { return "Opis makro definicije"; }
    QCString trTypedefDocumentation() override
    { return "Opis uporabniško definiranega tipa"; }
    QCString trEnumerationTypeDocumentation() override
    { return "Opis naštevnega (enum) tipa"; }
    QCString trFunctionDocumentation() override
    { return "Opis funkcije"; }
    QCString trVariableDocumentation() override
    { return "Opis spremenljivke"; }
    QCString trCompounds() override
    { return "Strukture"; }
    QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result="Generirano "+date;
      if (!projName.isEmpty()) result+=" projekt: "+projName;
      result+=" generator: ";
      return result;
    }
    QCString trClassDiagram(const QCString &clName) override
    {
      return "Diagram razredov za "+clName;
    }
    QCString trWarning() override
    { return "Opozorilo"; }
    QCString trVersion() override
    { return "Verzija"; }
    QCString trDate() override
    { return "Datum"; }
    QCString trReturns() override
    { return "Rezultat(i)"; }
    QCString trSeeAlso() override
    { return "Glej"; }
    QCString trParameters() override
    { return "Parametri"; }
    QCString trExceptions() override
    { return "Prekinitve"; }
    QCString trGeneratedBy() override
    { return "Izdelano s pomočjo"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    QCString trNamespaceList() override
    { return "imenski prostori"; }
    QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="Seznam ";
      if (!extractAll) result+="dokumentiranih ";
      result+="imenskih prostorov z opisom:";
      return result;
    }
    QCString trFriends() override
    { return "Prijatelji (Friends) "; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    QCString trRelatedFunctionDocumentation() override
    { return "Podatki o poveznih funkcijah"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    QCString trCompoundReference(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool /*isTemplate*/) override
      // used as the title of the HTML page of a class/struct/union
    {
      QCString result="";
      switch(compType)
      {
        case ClassDef::Class:  result+=" Razred "; break;
        case ClassDef::Struct: result+=" Struktura "; break;
        case ClassDef::Union:  result+=" Množica "; break;
        case ClassDef::Interface:  result+=" IDL vmesnik "; break;
        case ClassDef::Protocol:   result+=" protocol "; break; // translate me!
        case ClassDef::Category:   result+=" category "; break; // translate me!
        case ClassDef::Exception:  result+=" IDL prekinitev "; break;
        default: break;
      }
      result += clName;

      return result;
    }
    QCString trFileReference(const QCString &fileName) override
      // used as the title of the HTML page of a file
    {
      QCString result="Datoteka ";
	  result+=fileName;
	  return result;
    }
    QCString trNamespaceReference(const QCString &namespaceName) override
      // used as the title of the HTML page of a namespace
    {
      QCString result ="Imenski prostor ";
	  result+=namespaceName;

      return result;
    }

    // these are for the member sections of a class, struct or union
    QCString trPublicMembers() override
    { return "Javne metode"; }
    QCString trPublicSlots() override
    { return "Public slotovi"; }
    QCString trSignals() override
    { return "Programske prekinitve"; }
    QCString trStaticPublicMembers() override
    { return "Statične javne metode in atributi"; }
    QCString trProtectedMembers() override
    { return "Zaščitene metode in atributi"; }
    QCString trProtectedSlots() override
    { return "Zaščiteni sloti"; }
    QCString trStaticProtectedMembers() override
    { return "Statične zaščitene metode in atributi"; }
    QCString trPrivateMembers() override
    { return "Skrite metode in atributi"; }
    QCString trPrivateSlots() override
    { return "Skriti slotovi"; }
    QCString trStaticPrivateMembers() override
    { return "Statične skrite metode in atributi"; }
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
            result+=" in ";
        }
      }
      return result;
    }

    QCString trInheritsList(int numEntries) override
      // used in class documentation to produce a list of base classes,
      // if class diagrams are disabled.
    {
      return "Deduje od "+trWriteList(numEntries)+".";
    }
    QCString trInheritedByList(int numEntries) override
      // used in class documentation to produce a list of super classes,
      // if class diagrams are disabled.
    {
      return "Naslijeðena u "+trWriteList(numEntries)+".";
    }
    QCString trReimplementedFromList(int numEntries) override
      // used in member documentation blocks to produce a list of
      // members that are hidden by this one.
    {
      return "Skrije implementacijo iz "+trWriteList(numEntries)+".";
    }
    QCString trReimplementedInList(int numEntries) override
    {
      // used in member documentation blocks to produce a list of
      // all member that overwrite the implementation of this member.
      return "Metodo skrijejo implementacije v razredih "+trWriteList(numEntries)+".";
    }

    QCString trNamespaceMembers() override
      // This is put above each page as a link to all members of namespaces.
    { return "elementi imenskega prostora"; }
    QCString trNamespaceMemberDescription(bool extractAll) override
      // This is an introduction to the page with all namespace members
    {
      QCString result="Seznam vseh ";
      if (!extractAll) result+="dokumentiranih ";
      result+="elementov imenskega prostora s povezavami na ";
      if (extractAll)
        result+="opis vsakega elementa:";
      else
        result+="imenski prostor, ki mu pripadajo:";
      return result;
    }
    QCString trNamespaceIndex() override
      // This is used in LaTeX as the title of the chapter with the
      // index of all namespaces.
    { return "Indeks imenskih prostorov"; }
    QCString trNamespaceDocumentation() override
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all namespaces.
    { return "Podatki o imenskih prostorih"; }
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces() override
    { return "Imenski prostori"; }


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
      QCString result="Opis ";
      switch(compType)
      {
        case ClassDef::Class:      result+="razreda"; break;
        case ClassDef::Struct:     result+="strukture"; break;
        case ClassDef::Union:      result+="unije"; break;
        case ClassDef::Interface:  result+="vmesnika (interface)"; break;
        case ClassDef::Protocol:   result+="protocol"; break; // translate me!
        case ClassDef::Category:   result+="category"; break; // translate me!
        case ClassDef::Exception:  result+="prekinitve (exception)"; break;
        default: break;
      }
      result+=" je zgrajen na podlagi naslednj";
      if (single) result+="e "; else result+="ih";
      result+=" datotek";
      if (single) result+="e :"; else result+=" :";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    QCString trReturnValues() override
    { return "Povratna vrednost"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage() override
    { return "prva stran"; }

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
      return "Definirano v @0 vrstici datoteke @1.";
    }
    QCString trDefinedInSourceFile() override
    {
      return "Definirano v datoteki @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated() override
    {
      return "Zastarelo";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const QCString &clName) override
    {
      return "Kolaboracijski diagram razreda "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const QCString &fName) override
    {
      return "Graf prikazuje seznam datotek, "
          "ki jih datoteka \""+fName+"\" "
          "direktno ali indirektno vključuje. Pravokotniki ponazarjajo datoteke, puščice "
	  "predstavljajo relacije med njimi. "
	  "črn pravokotnik ponazarja datoteko "+fName+". Puščice A->B ponazarjajo "
	  "usmerjeno relacijo \"A vključuje B\"."
;
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation() override
    {
      return "Opis konstruktorjev in destruktorjev ";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode() override
    {
      return "izvorna koda";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation() override
    {
      return "dokumenacija tekoče datoteke.";
    }
    /*! Text for the \pre command */
    QCString trPrecondition() override
    {
      return "Predpogoji (preconditions)";
    }
    /*! Text for the \post command */
    QCString trPostcondition() override
    {
      return "Naknadni pogoji (posconditions)";
    }
    /*! Text for the \invariant command */
    QCString trInvariant() override
    {
      return "Invarianta";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue() override
    {
      return "Začetna vrednost / definicija :";
    }
    /*! Text used the source code in the file index */
    QCString trCode() override
    {
      return "koda";
    }
    QCString trGraphicalHierarchy() override
    {
      return "Hierarhija razredov v grafični obliki";
    }
    QCString trGotoGraphicalHierarchy() override
    {
      return "Dedovalna hierarhija v grafični obliki";
    }
    QCString trGotoTextualHierarchy() override
    {
      return "Dedovalna hierarhija v tekstovni obliki";
    }
    QCString trPageIndex() override
    {
      return "Indeks strani";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    QCString trNote() override
    {
      return "Opomba";
    }
    QCString trPublicTypes() override
    {
      return "Javni tipi";
    }
    QCString trPublicAttribs() override
    {
      return "Javni atributi";
    }
    QCString trStaticPublicAttribs() override
    {
      return "Statični javni atributi";
    }
    QCString trProtectedTypes() override
    {
      return "Zaščiteni tipi";
    }
    QCString trProtectedAttribs() override
    {
      return "Zaščiteni atributi";
    }
    QCString trStaticProtectedAttribs() override
    {
      return "Statični zaščiteni tipi";
    }
    QCString trPrivateTypes() override
    {
      return "Skriti tipi";
    }
    QCString trPrivateAttribs() override
    {
      return "Skriti atributi";
    }
    QCString trStaticPrivateAttribs() override
    {
      return "Statični skriti atributi";
    }
//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    QCString trTodo() override
    {
      return "TODO";
    }
    /*! Used as the header of the todo list */
    QCString trTodoList() override
    {
      return "Seznam nedokončanih opravil";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy() override
    {
      return "Uporabniki entitete:  ";
    }
    QCString trRemarks() override
    {
      return "Opomba";
    }
    QCString trAttention() override
    {
      return "Pozor";
    }
    QCString trInclByDepGraph() override
    {
      return "Graf prikazuje datoteke, ki posredno ali neposredno "
             "vključujejo tekočo datoteko. Pravokotniki simbolizirajo datoteke, "
             "puščice pa relacije med datotekami. Tekoča datoteka je prikazana "
	     "kot pravokotnik s črno podlago, ostale pa kot pravokotnik brez podlage. "
	     "Smer puščice A->B definira relacijo \"A vključuje B\". "
	     "Vse datoteke, ki torej mejijo na tekočo (t.j. obstaja povezava med črnim in "
	     "praznim pravokotnikom), jo direktno vključujejo, medtem, ko jo ostale vključujejo "
	     "le posredno. "
	;
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
      return "Legenda grafa";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    QCString trLegendDocs() override
    {
      return
        "Tekoča stran pojasnjuje način interpretacije grafov, ki jih izriše "
        "doxygen.<p>\n"
        "Poglejmo si naslednji primer:\n"
        "\\code\n"
        "/*! Nevide razred zaradi rezanja */\n"
        "class Invisible { };\n\n"
        "/*! Odrezan razred, dedovalna relacija je skrita */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* razred, ki ni opisan z doxygen komentarji */\n"
        "class Undocumented { };\n\n"
        "/*! Razred, ki ga dedujemo s pomočjo javnega dedovanja */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! A template class */\n"
        "template<class T> class Templ { };\n\n"
        "/*! Razred, ki ga dedujemo s pomočjo zaščitenega dedovanja */\n"
        "class ProtectedBase { };\n\n"
        "/*! Razred, ki ga dedujemo s pomočjo skritega dedovanja */\n"
        "class PrivateBase { };\n\n"
        "/*! Razred, ki ga uporablja dedovani razred */\n"
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
        "If the \\c MAX_DOT_GRAPH_HEIGHT tag in the configuration file "
        "is set to 200 this will result in the following graph:"
        "<p><center><img src=\"graph_legend."+getDotImageExtension()+"\"></center>\n"
        "<p>\n"
        "The boxes in the above graph have the following meaning:\n"
        "<ul>\n"
        "<li>%A filled black box represents the struct or class for which the "
        "graph is generated.\n"
        "<li>%A box with a black border denotes a documented struct or class.\n"
        "<li>%A box with a gray border denotes an undocumented struct or class.\n"
        "<li>%A box with a red border denotes a documented struct or class for\n"
        "which not all inheritance/containment relations are shown. %A graph is "
        "truncated if it does not fit within the specified boundaries."
        "</ul>\n"
        "The arrows have the following meaning:\n"
        "<ul>\n"
        "<li>%A dark blue arrow is used to visualize a public inheritance "
        "relation between two classes.\n"
        "<li>%A dark green arrow is used for protected inheritance.\n"
        "<li>%A dark red arrow is used for private inheritance.\n"
        "<li>%A purple dashed arrow is used if a class is contained or used "
        "by another class. The arrow is labeled with the variable(s) "
        "through which the pointed class or struct is accessible. \n"
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
      return "Test List";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    QCString trProperties() override
    {
      return "IDL Lastnosti";
    }
    /*! Used as a section header for IDL property documentation */
    QCString trPropertyDocumentation() override
    {
      return "Opis IDL lastnosti";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java interfaces in the summary section of Java packages */
/*     QCString trInterfaces() override */
/*     { */
/*       return "Vmesniki"; */
/*     } */
    /*! Used for Java classes in the summary section of Java packages */
    QCString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Podatkovne strukture";
      }
      else
      {
        return "Razredi";
      }
    }
    /*! Used as the title of a Java package */
    QCString trPackage(const QCString &name) override
    {
      return "JAVA paket "+name;
    }
    /*! The description of the package index page */
    QCString trPackageListDescription() override
    {
      return "Seznam JAVA paketov in njihovih kratkih opisov v primeru, da obstajajo:";
    }
    /*! The link name in the Quick links header for each page */
    QCString trPackages() override
    {
      return "JAVA paketi";
    }
    /*! Text shown before a multi-line define */
    QCString trDefineValue() override
    {
      return "Vrednost:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    QCString trBug() override
    {
      return "Programska napaka";
    }
    /*! Used as the header of the bug list */
    QCString trBugList() override
    {
      return "Seznam programskih napak";
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
      return "1250";
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
      return createNoun(first_capital, singular, "razred", "i");
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
      return createNoun(first_capital, singular, "imenski prostor", "i");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGroup(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "skupina", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trPage(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "stran", "i");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trMember(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "element", "i");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGlobal(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "global", "s");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    QCString trAuthor(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "avtor", "ji");
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
     *  all members that implement this abstract member.
     */
    QCString trImplementedInList(int numEntries) override
    {
      return "Implementirano v "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    QCString trRTFTableOfContents() override
    {
      return "Vsebina";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    QCString trDeprecatedList() override
    {
      return "Seznam opuščenih";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    QCString trEvents() override
    {
      return "Dogodki";
    }
    /*! Header used for the documentation section of a class' events. */
    QCString trEventDocumentation() override
    {
      return "Opisi dogodkov";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    QCString trPackageTypes() override
    {
      return "Tipi paketov";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    QCString trPackageFunctions() override
    {
      return "Funkcije paketa";   /* don't know the context */
    }
    QCString trPackageMembers() override
    {
      return "Elemente paketa";   /* don't know the context */
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    QCString trStaticPackageFunctions() override
    {
      return "Statične funkcije paketa";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    QCString trPackageAttribs() override
    {
      return "Atributi paketa";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    QCString trStaticPackageAttribs() override
    {
      return "Statični atributi paketa";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    QCString trAll() override
    {
      return "Vse";
    }
    /*! Put in front of the call graph for a function. */
    QCString trCallGraph() override
    {
      return "Graf klicev tekoče funkcije:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    QCString trSearchResultsTitle() override
    {
      return "Rezultat(i) iskanja";
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
        return "Oprostite, noben dokument ne ustreza vašemu povpraševanju.";
      }
      else if (numDocuments==1)
      {
        return "Našel sem <b>1</b> dokument, ki ustreza vašemu povpraševanju.";
      }
      else if (numDocuments==2)
      {
        return "Našel sem <b>2</b> dokumenta, ki ustrezata vašemu povpraševanju.";
      }
      else
      {
        return "Našel sem <b>$num</b> dokumentov, ki ustrezajo vašemu povpraševanju. "
               "Dokumenti z najboljšo stopnjo ujemanja se nahajajo na začetku.";
      }
    }
    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    QCString trSearchMatches() override
    {
      return "Zadetki:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    QCString trSourceFile(const QCString& filename) override
    {
      return filename + " izvorna koda";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    QCString trDirIndex() override
    { return "Hierarhija imenikov"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    QCString trDirDocumentation() override
    { return "Opisi imenikov"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of an HTML page, to link to the directory hierarchy.
     */
    QCString trDirectories() override
    { return "Imeniki"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    QCString trDirReference(const QCString &dirName) override
    { QCString result=dirName;
      result+=" imeniške reference"; /* not sure for context */
      return result;
    }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    QCString trDir(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "imenik", "i");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    QCString trOverloadText() override
    {
      return "To je ponovno definirana metoda, " /* don't know Slovene expresion for overloaded */
             "podana je zaradi priročnosti. Metoda se od predhodnje razlikuje "
             "samo v številu in/ali tipu formalnih argumentov.";
    }

    QCString trDayOfWeek(int dayOfWeek, bool first_capital, bool full) override
    {
      static const char *days_short[]   = { "pon.", "tor.", "sre.", "čet.", "pet.", "sob.", "ned." };
      static const char *days_full[]    = { "ponedeljek", "torek", "sreda", "četrtek", "petek", "sobota", "nedelja" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    QCString trMonth(int month, bool first_capital, bool full) override
    {
      static const char *months_short[] = { "jan.", "feb.", "mar.", "apr.", "maj", "jun.", "jul.", "avg.", "sep.", "okt.", "nov.", "dec." };
      static const char *months_full[]  = { "januar", "februar", "marec", "april", "maj", "junij", "julij", "avgust", "september", "oktober", "november", "december" };
      QCString text  = full? months_full[month-1] : months_short[month-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    QCString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "dop.", "pop." };
      return dayPeriod[period?1:0];
    }
};

#endif
