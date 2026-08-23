/******************************************************************************
 *
 * Copyright (C) 1997-2026 by Dimitri van Heesch.
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

#ifndef TRANSLATOR_NL_H
#define TRANSLATOR_NL_H

class TranslatorDutch : public Translator
{
  public:
    DString idLanguage() override
    { return "dutch"; }
    /*! Used to get the LaTeX command(s) for the language support.
     *  This method should return string with commands that switch
     *  LaTeX to the desired language.  For example
     *  <pre>"\\usepackage[german]{babel}\n"
     *  </pre>
     *  or
     *  <pre>"\\usepackage{polski}\n"
     *  "\\usepackage[latin2]{inputenc}\n"
     *  "\\usepackage[T1]{fontenc}\n"
     *  </pre>
     */
    DString latexLanguageSupportCommand() override
    { return "\\usepackage[dutch]{babel}\n"; }
    DString trISOLang() override
    { return "nl"; }
    DString getLanguageString() override
    {
      return "0x413 Dutch";
    }
    DString trRelatedFunctions() override
    { return "Gerelateerde functies"; }
    DString trRelatedSubscript() override
    { return "(Merk op dat dit geen member functies zijn.)"; }
    DString trDetailedDescription() override
    { return "Gedetailleerde Beschrijving"; }
    DString trDetails() override
    { return "Details"; }

    DString trMemberTypedefDocumentation() override
    { return "Documentatie van type definitie members"; }
    DString trMemberEnumerationDocumentation() override
    { return "Documentatie van enumeratie members"; }
    DString trMemberFunctionDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
        return "Documentatie van functie/procedure/process members";
      }
      else
      {
        return "Documentatie van functie members";
      }
    }
    DString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Documentatie van data velden";
      }
      else
      {
        return "Documentatie van data members";
      }
    }
    DString trMore() override
    { return "Meer..."; }
    DString trListOfAllMembers() override
    { return "Lijst van alle members"; }
    DString trMemberList() override
    { return "Member Lijst"; }
    DString trThisIsTheListOfAllMembers() override
    { return "Dit is de complete lijst van alle members voor"; }
    DString trIncludingInheritedMembers() override
    { return ", inclusief alle overgeërfde members."; }
    DString trGeneratedAutomatically(const DString &s) override
    { DString result="Automatisch gegenereerd door Doxygen";
      if (!s.empty()) result+=" voor "+s;
      result+=" uit de programmatekst.";
      return result;
    }
    DString trEnumName() override
    { return "enum naam"; }
    DString trEnumValue() override
    { return "enum waarde"; }
    DString trDefinedIn() override
    { return "gedefinieerd in"; }
    DString trModules() override
    { return "Modules"; }
    DString trClassHierarchy() override
    { return "Klasse Hiërarchie"; }
    DString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Data Structuren Lijst";
      }
      else
      {
        return "Klasse Lijst";
      }
    }
    DString trFileList() override
    { return "Bestandslijst"; }
    DString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Data Velden";
      }
      else
      {
        return "Klasse Members";
      }
    }
    DString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Globale members";
      }
      else
      {
        return "Bestand members";
      }
    }
    DString trRelatedPages() override
    { return "Gerelateerde pagina's"; }
    DString trExamples() override
    { return "Voorbeelden"; }
    DString trSearch() override
    { return "Zoeken"; }
    DString trClassHierarchyDescription() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
        return "Hieronder is een hiërarchische lijst met alle entiteiten:";
      }
      else
      {
        return "Deze inheritance lijst is min of meer alfabetisch gesorteerd:";
      }
    }
    DString trFileListDescription(bool extractAll) override
    {
      DString result="Hieronder volgt de lijst met alle ";
      if (!extractAll) result+="gedocumenteerde ";
      result+="bestanden, elk met een korte beschrijving:";
      return result;
    }
    DString trCompoundListDescription() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Hieronder volgen de structs "
               "met voor elk een korte beschrijving:";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
      {
        return "Hieronder volgen de klassen "
               "met voor elk een korte beschrijving:";
      }
      else
      {
        return "Hieronder volgen de klassen, structs en "
               "unions met voor elk een korte beschrijving:";
      }
    }
    DString trCompoundMembersDescription(bool extractAll) override
    {
      DString result="Hieronder volgt de lijst met alle ";
      if (!extractAll) result+="gedocumenteerde ";
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="struct en union velden";
      }
      else
      {
        result+="klasse members";
      }
      result+=" met links naar ";

      if (!extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="de struct/union documentatie voor elke veld:";
        }
        else
        {
           result+="de klasse documentatie voor elke member:";
        }
      }
      else
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="de structures/unions waartoe ze behoren:";
        }
        else
        {
          result+="de klassen waartoe ze behoren:";
        }
      }
      return result;
    }
    DString trFileMembersDescription(bool extractAll) override
    {
      DString result="Hieronder volgt de lijst met alle ";
      if (!extractAll) result+="gedocumenteerde ";
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="functies, variabelen, macros, enumeraties, en typedefs";
      }
      else
      {
        result+="bestand members";
      }
      result+=" met links naar ";
      if (extractAll) result+="de bestand's documentatie voor elke member:";
      else result+="de bestanden waartoe ze behoren:";
      return result;
    }
    DString trExamplesDescription() override
    { return "Hieronder volgt de lijst met alle voorbeelden:"; }
    DString trRelatedPagesDescription() override
    { return "Hieronder volgt de lijst met alle pagina's die gerelateerde documentatie bevatten:"; }
    DString trModulesDescription() override
    { return "Hieronder volgt de lijst met alle modules:"; }

    DString trDocumentation(const DString &projName) override
    { return (!projName.empty()?projName + " " : "") + "Documentatie"; }
    DString trModuleIndex() override
    { return "Module Index"; }
    DString trHierarchicalIndex() override
    { return "Hiërarchische Index"; }
    DString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Data Structuren Index";
      }
      else
      {
        return "Klasse Index";
      }
    }
    DString trFileIndex() override
    { return "Bestand Index"; }
    DString trModuleDocumentation() override
    { return "Module Documentatie"; }
    DString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Klassen Documentatie";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return "Klassen Documentatie";
      }
    }
    DString trFileDocumentation() override
    { return "Bestand Documentatie"; }
    DString trReferenceManual() override
    { return "Naslagwerk"; }

    DString trDefines() override
    { return "Macros"; }
    DString trTypedefs() override
    { return "Typedefs"; }
    DString trEnumerations() override
    { return "Enumeraties"; }
    DString trFunctions() override
    { return "Functies"; }
    DString trVariables() override
    { return "Variabelen"; }
    DString trEnumerationValues() override
    { return "Enumeratie waarden"; }
    DString trDefineDocumentation() override
    { return "Documentatie van macro's"; }
    DString trTypedefDocumentation() override
    { return "Documentatie van typedefs"; }
    DString trEnumerationTypeDocumentation() override
    { return "Documentatie van enumeratie types"; }
    DString trEnumerationValueDocumentation() override
    { return "Documentatie van enumeratie waarden"; }
    DString trFunctionDocumentation() override
    { return "Documentatie van functies"; }
    DString trVariableDocumentation() override
    { return "Documentatie van variabelen"; }
    DString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Data Structuren";
      }
      else
      {
        return "Klassen";
      }
    }
    DString trGeneratedAt(const DString &date,const DString &projName) override
    {
      DString result="Gegenereerd op "+date;
      if (!projName.empty()) result+=" voor "+projName;
      result+=" door";
      return result;
    }
    DString trClassDiagram(const DString &clName) override
    {
      return "Klasse diagram voor "+clName;
    }
    DString trWarning() override
    { return "Waarschuwing"; }
    DString trVersion() override
    { return "Versie"; }
    DString trDate() override
    { return "Datum"; }
    DString trReturns() override
    { return "Retourneert"; }
    DString trSeeAlso() override
    { return "Zie ook"; }
    DString trParameters() override
    { return "Parameters"; }
    DString trExceptions() override
    { return "Excepties"; }
    DString trGeneratedBy() override
    { return "Gegenereerd door"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    DString trNamespaceList() override
    { return "Namespace Lijst"; }
    DString trNamespaceListDescription(bool extractAll) override
    {
      DString result="Hier is een lijst met alle ";
      if (!extractAll) result+="gedocumenteerde ";
      result+="namespaces met voor elk een korte beschrijving:";
      return result;
    }
    DString trFriends() override
    { return "Friends"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    DString trRelatedFunctionDocumentation() override
    { return "Documentatie van friends en gerelateerde functies"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    DString trCompoundReference(const DString &clName,
                                 ClassDef::CompoundType compType,
                                 bool isTemplate) override
      // used as the title of the HTML page of a class/struct/union
    {
      DString result=clName;
      if (isTemplate) result+=" Template";
      switch(compType)
      {
        case ClassDef::Class:  result+=" Class"; break;
        case ClassDef::Struct: result+=" Struct"; break;
        case ClassDef::Union:  result+=" Union"; break;
        case ClassDef::Interface:  result+=" Interface"; break;
        case ClassDef::Protocol:  result+=" Protocol"; break;
        case ClassDef::Category:  result+=" Category"; break;
        case ClassDef::Exception:  result+=" Exception"; break;
        default: break;
      }
      result+=" Referentie";
      return result;
    }
    DString trFileReference(const DString &fileName) override
      // used as the title of the HTML page of a file
    {
      DString result=fileName;
      result+=" Bestand Referentie";
      return result;
    }
    DString trNamespaceReference(const DString &namespaceName) override
      // used as the title of the HTML page of a namespace
    {
      DString result=namespaceName;
      result+=" Namespace Referentie";
      return result;
    }

    // these are for the member sections of a class, struct or union
    DString trPublicMembers() override
    { return "Public Members"; }
    DString trPublicSlots() override
    { return "Public Slots"; }
    DString trSignals() override
    { return "Signals"; }
    DString trStaticPublicMembers() override
    { return "Static Public Members"; }
    DString trProtectedMembers() override
    { return "Protected Members"; }
    DString trProtectedSlots() override
    { return "Protected Slots"; }
    DString trStaticProtectedMembers() override
    { return "Static Protected Members"; }
    DString trPrivateMembers() override
    { return "Private Members"; }
    DString trPrivateSlots() override
    { return "Private Slots"; }
    DString trStaticPrivateMembers() override
    { return "Static Private Members"; }
    // end of member sections

    DString trWriteList(int numEntries) override
    {
      // this function is used to produce a comma-separated list of items.
      // use generateMarker(i) to indicate where item i should be put.
      DString result;
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
            result+=" en ";
        }
      }
      return result;
    }

    DString trInheritsList(int numEntries) override
      // used in class documentation to produce a list of base classes,
      // if class diagrams are disabled.
    {
      return "Erft over van "+trWriteList(numEntries)+".";
    }
    DString trInheritedByList(int numEntries) override
      // used in class documentation to produce a list of super classes,
      // if class diagrams are disabled.
    {
      return "Wordt overgeërfd door "+trWriteList(numEntries)+".";
    }
    DString trReimplementedFromList(int numEntries) override
      // used in member documentation blocks to produce a list of
      // members that are hidden by this one.
    {
      return "Nieuwe implementatie van "+trWriteList(numEntries)+".";
    }
    DString trReimplementedInList(int numEntries) override
    {
      // used in member documentation blocks to produce a list of
      // all member that overwrite the implementation of this member.
      return "Opnieuw geïmplementeerd in "+trWriteList(numEntries)+".";
    }

    DString trNamespaceMembers() override
      // This is put above each page as a link to all members of namespaces.
    { return "Namespace Members"; }
    DString trNamespaceMemberDescription(bool extractAll) override
      // This is an introduction to the page with all namespace members
    {
      DString result="Hier is een lijst van alle ";
      if (!extractAll) result+="gedocumenteerde ";
      result+="namespace members met links naar ";
      if (extractAll)
        result+="de namespace documentatie voor iedere member:";
      else
        result+="de namespaces waartoe ze behoren:";
      return result;
    }
    DString trNamespaceIndex() override
      // This is used in LaTeX as the title of the chapter with the
      // index of all namespaces.
    { return "Namespace Index"; }
    DString trNamespaceDocumentation() override
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all namespaces.
    { return "Namespace Documentatie"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    DString trNamespaces() override
    {
      return "Namespaces";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990728
//////////////////////////////////////////////////////////////////////////

    /*! This is put at the bottom of a class documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    DString trGeneratedFromFiles(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      bool vhdlOpt = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
      DString result="De documentatie voor ";
      switch(compType)
      {
        case ClassDef::Class:      result+=vhdlOpt?"deze ontwerp eenheid":"deze klasse"; break;
        case ClassDef::Struct:     result+="deze struct"; break;
        case ClassDef::Union:      result+="deze union"; break;
        case ClassDef::Interface:  result+="dit interface"; break;
        case ClassDef::Protocol:   result+="dit protocol"; break;
        case ClassDef::Category:   result+="deze categorie"; break;
        case ClassDef::Exception:  result+="deze exceptie"; break;
        default: break;
      }
      result+=" is gegenereerd op basis van ";
      if (single) result+="het"; else result+="de";
      result+=" volgende bestand";
      if (single) result+=":"; else result+="en:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    DString trReturnValues() override
    { return "Retour waarden"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    DString trMainPage() override
    { return "Hoofd Pagina"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    DString trPageAbbreviation() override
    { return "p."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991106
//////////////////////////////////////////////////////////////////////////

    DString trDefinedAtLineInSourceFile() override
    {
      return "De definitie bevindt zich op regel @0 in het bestand @1.";
    }
    DString trDefinedInSourceFile() override
    {
      return "De definitie bevindt zich in het bestand @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    DString trDeprecated() override
    {
      return "Verouderd";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    DString trCollaborationDiagram(const DString &clName) override
    {
      return "Collaboratie diagram voor "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    DString trInclDepGraph(const DString &fName) override
    {
      return "Include afhankelijkheidsgraaf voor "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    DString trConstructorDocumentation() override
    {
      return "Constructor & Destructor Documentatie";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    DString trGotoSourceCode() override
    {
      return "Ga naar de broncode van dit bestand.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    DString trGotoDocumentation() override
    {
      return "Ga naar de documentatie van dit bestand.";
    }
    /*! Text for the \\pre command */
    DString trPrecondition() override
    {
      return "Preconditie";
    }
    /*! Text for the \\post command */
    DString trPostcondition() override
    {
      return "Postconditie";
    }
    /*! Text for the \\invariant command */
    DString trInvariant() override
    {
      return "Invariant";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    DString trInitialValue() override
    {
      return "Initiële waarde:";
    }
    /*! Text used the source code in the file index */
    DString trCode() override
    {
      return "code";
    }
    DString trGraphicalHierarchy() override
    {
      return "Grafische Klasse Hiërarchie";
    }
    DString trGotoGraphicalHierarchy() override
    {
      return "Ga naar de grafische klasse hiërarchie";
    }
    DString trGotoTextualHierarchy() override
    {
      return "Ga naar de tekstuele klasse hiërarchie";
    }
    DString trPageIndex() override
    {
      return "Pagina Index";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    DString trNote() override
    {
      return "Noot";
    }
    DString trPublicTypes() override
    {
      return "Public Typen";
    }
    DString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Data Velden";
      }
      else
      {
        return "Public Attributen";
      }
    }
    DString trStaticPublicAttribs() override
    {
      return "Static Public Attributen";
    }
    DString trProtectedTypes() override
    {
      return "Protected Typen";
    }
    DString trProtectedAttribs() override
    {
      return "Protected Attributen";
    }
    DString trStaticProtectedAttribs() override
    {
      return "Static Protected Attributen";
    }
    DString trPrivateTypes() override
    {
      return "Private Typen";
    }
    DString trPrivateAttribs() override
    {
      return "Private Attributen";
    }
    DString trStaticPrivateAttribs() override
    {
      return "Static Private Attributen";
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    DString trTodo() override
    {
      return "Todo";
    }
    /*! Used as the header of the todo list */
    DString trTodoList() override
    {
      return "Todo Lijst";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    DString trReferencedBy() override
    {
      return "Wordt aangeroepen door";
    }
    DString trRemarks() override
    {
      return "Opmerkingen";
    }
    DString trAttention() override
    {
      return "Attentie";
    }
    DString trInclByDepGraph() override
    {
      return "Deze graaf geeft aan welke bestanden direct of "
             "indirect afhankelijk zijn van dit bestand:";
    }
    DString trSince() override
    {
      return "Sinds";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    DString trLegendTitle() override
    {
      return "Graaf Legenda";
    }
    /*! page explaining how the dot graph's should be interpreted */
    DString trLegendDocs() override
    {
      return
        "Deze pagina legt uit hoe de grafen die gegenereerd worden door doxygen "
        "geïnterpreteerd dienen te worden.<p>\n"
        "Beschouw het volgende voorbeeld:\n"
        "\\code\n"
        "/*! Onzichtbare klasse vanwege afkappen van de graaf */\n"
        "class Invisible { };\n\n"
        "/*! Afgekapte klasse, overervingsrelatie is verborgen */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Klasse is niet gedocumenteerd met doxygen commentaar */\n"
        "class Undocumented { };\n\n"
        "/*! Klasse met public inheritance */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! A template class */\n"
        "template<class T> class Templ { };\n\n"
        "/*! Klasse met protected inheritance */\n"
        "class ProtectedBase { };\n\n"
        "/*! Klasse met private inheritance */\n"
        "class PrivateBase { };\n\n"
        "/*! Klasse die wordt gebruikt door de klasse Inherited */\n"
        "class Used { };\n\n"
        "/*! Super klasse die overerft van een aantal andere klassen */\n"
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
        "Dit voorbeeld zal resulteren in de volgende graaf:"
        "<p><center><img src=\"graph_legend."+getDotImageExtension()+"\"></center>\n"
        "<p>\n"
        "De rechthoeken in the bovenstaande graaf hebben de volgende betekenis:\n"
        "<ul>\n"
        "<li>Een gevulde grijze rechthoek representeert de structure of klasse waarvoor "
        "de graaf is gegenereerd.\n"
        "<li>Een rechthoek met een zwarte rand representeert een gedocumenteerde structure of klasse.\n"
        "<li>Een rechthoek met een grijze rand representeert een ongedocumenteerde structure of klasse.\n"
        "<li>Een rechthoek met een rode rand representeert een gedocumenteerde structure or klasse waarvoor\n"
        "niet alle overervings- of gebruiksrelaties konden worden getoond. Een graaf wordt "
        "afgekapt als hij niet past binnen de gespecificeerde grenzen."
        "</ul>\n"
        "De pijlen hebben de volgende betekenis:\n"
        "<ul>\n"
        "<li>Een blauwe pijl visualizeert een public inheritance "
        "relatie tussen twee klassen.\n"
        "<li>Een donkergroene pijl wordt gebruikt voor protected inheritance.\n"
        "<li>Een donkerrode pijl wordt gebruikt voor private inheritance.\n"
        "<li>Een paars gestippelde pijl wordt gebruikt indien een klasse bevat is of gebruikt wordt "
        "door een andere klasse. De pijl is gelabeled met de variable(n) "
        "die toegang geven tot de aangewezen klasse of structure. \n"
        "<li>Een geel gestippelde pijl wordt gebruikt om een relatie tussen een \n"
        "template instantie en een template klasse aan te geven. De pijl is gelabeld met \n"
        "template parameters van de instantie.\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    DString trLegend() override
    {
      return "legenda";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    DString trTest() override
    {
      return "Test";
    }
    /*! Used as the header of the test list */
    DString trTestList() override
    {
      return "Test Lijst";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    DString trProperties() override
    {
      return "Properties";
    }
    /*! Used as a section header for IDL property documentation */
    DString trPropertyDocumentation() override
    {
      return "Property Documentatie";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    DString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Data Structuren";
      }
      else
      {
        return "Klassen";
      }
    }
    /*! Used as the title of a Java package */
    DString trPackage(const DString &name) override
    {
      return "Package "+name;
    }
    /*! The description of the package index page */
    DString trPackageListDescription() override
    {
      return "Hier volgen de packages, elk met een korte beschrijving (indien aanwezig):";
    }
    /*! The link name in the Quick links header for each page */
    DString trPackages() override
    {
      return "Packages";
    }
    /*! Text shown before a multi-line define */
    DString trDefineValue() override
    {
      return "Waarde:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    DString trBug() override
    {
      return "Bug";
    }
    /*! Used as the header of the bug list */
    DString trBugList() override
    {
      return "Bug Lijst";
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
    DString trRTFansicp() override
    {
      return "1252";
    }


    /*! Used as ansicpg for RTF fcharset
     *  \see trRTFansicp() for a table of possible values.
     */
    DString trRTFCharSet() override
    {
      return "0";
    }

    /*! Used as header RTF general index */
    DString trRTFGeneralIndex() override
    {
      return "Index";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trClass(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "klasse", "n");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trFile(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "bestand", "en");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trNamespace(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "namespace", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trGroup(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "groep", "en");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trPage(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "pagina", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trMember(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "member", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trGlobal(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "globale member", "s");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    DString trAuthor(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "auteur", "s");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    DString trReferences() override
    {
      return "Gebruikt";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    DString trImplementedFromList(int numEntries) override
    {
      return "Implementeert "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implementation this member.
     */
    DString trImplementedInList(int numEntries) override
    {
      return "Wordt geïmplementeerd door "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    DString trRTFTableOfContents() override
    { return "Inhoudsopgave"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    DString trDeprecatedList() override
    {
      return "Deprecated Lijst";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for a section of events found in a C# program
     */
    DString trEvents() override
    {
      return "Events";
    }
    /*! Header used for the documentation section of a class' events. */
    DString trEventDocumentation() override
    {
      return "Event Documentatie";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    DString trPackageTypes() override
    {
      return "Package Types";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    DString trPackageFunctions() override
    {
      return "Package Functies";
    }
    DString trPackageMembers() override
    {
      return "Package Members";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    DString trStaticPackageFunctions() override
    {
      return "Statische Package Functies";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    DString trPackageAttribs() override
    {
      return "Package Attributen";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    DString trStaticPackageAttribs() override
    {
      return "Statische Package Attributen";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    DString trAll() override
    {
      return "Alle";
    }
    /*! Put in front of the call graph for a function. */
    DString trCallGraph() override
    {
      return "Hier is de call graaf voor deze functie:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    DString trSearchResultsTitle() override
    {
      return "Zoek Resultaten";
    }
    /*! This string is put just before listing the search results. The
     *  text can be different depending on the number of documents found.
     *  Inside the text you can put the special marker $num to insert
     *  the number representing the actual number of search results.
     *  The @a numDocuments parameter can be either 0, 1 or 2, where the
     *  value 2 represents 2 or more matches. HTML markup is allowed inside
     *  the returned string.
     */
    DString trSearchResults(int numDocuments) override
    {
      if (numDocuments==0)
      {
        return "Helaas, er zijn geen documenten gevonden die aan de zoekopdracht voldoen.";
      }
      else if (numDocuments==1)
      {
        return "Er is <b>1</b> document gevonden dat aan de zoekopdracht voldoet.";
      }
      else
      {
        return "Er zijn <b>$num</b> documenten gevonden die aan de zoekopdracht voldoen. "
               "De beste resultaten worden eerst getoond.";
      }
    }
    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    DString trSearchMatches() override
    {
      return "Gevonden:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    DString trSourceFile(const DString& filename) override
    {
      return filename + " Bron Bestand";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    DString trDirIndex() override
    { return "Folder Hiërarchie"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    DString trDirDocumentation() override
    { return "Folder Documentatie"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    DString trDirectories() override
    { return "Folders"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    DString trDirReference(const DString &dirName) override
    { DString result=dirName; result+=" Folder Referentie"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in singular or plural form (\a singular).
     */
    DString trDir(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "folder", "s");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a function.
     */
    DString trOverloadText() override
    {
      return "Deze functie is overloaded en is beschikbaar gemaakt om het "
             "gebruik te vergemakkelijken. Ze verschilt alleen van de "
             "bovenstaande functie wat betreft de parameterlijst.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! Put in front of the call graph for a function. */
    DString trCallerGraph() override
    {
      return "Hier is de caller graaf voor deze functie:";
    }

    /*! header that is put before the list of member subprograms (Fortran). */
    DString trMemberFunctionDocumentationFortran() override
    { return "Member Functie/Subroutine Documentatie"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    DString trCompoundListFortran() override
    { return "Lijst met data types"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    DString trCompoundMembersFortran() override
    { return "Data velden"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    DString trCompoundListDescriptionFortran() override
    { return "Hieronder volgen de data types elk een korte beschrijving:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    DString trCompoundMembersDescriptionFortran(bool extractAll) override
    {
      DString result="Hieronder volgt de lijst met alle ";
      if (!extractAll) result+="gedocumenteerde ";
      result+="data types met links naar ";
      if (!extractAll) result+="de klasse documentatie voor elke member:";
      else result+="de klassen waartoe ze behoren:";
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index (Fortran).
     */
    DString trCompoundIndexFortran() override
    { return "Data Type Index"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    DString trTypeDocumentation() override
    { return "Data Type Documentatie"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    DString trSubprograms() override
    { return "Functies/Subroutines"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    DString trSubprogramDocumentation() override
    { return "Functie/Subroutine Documentatie"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     DString trDataTypes() override
    { return "Data Types"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    DString trModulesList() override
    { return "Module Lijst"; }

    /*! used as an introduction to the modules list (Fortran) */
    DString trModulesListDescription(bool extractAll) override
    {
      DString result="Hieronder volgt de lijst met alle ";
      if (!extractAll) result+="gedocumenteerde ";
      result+="modulen, elk met een korte beschrijving:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    DString trCompoundReferenceFortran(const DString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      DString result=clName;
      switch(compType)
      {
        case ClassDef::Class:      result+=" Module"; break;
        case ClassDef::Struct:     result+=" Type"; break;
        case ClassDef::Union:      result+=" Union"; break;
        case ClassDef::Interface:  result+=" Interface"; break;
        case ClassDef::Protocol:   result+=" Protocol"; break;
        case ClassDef::Category:   result+=" Category"; break;
        case ClassDef::Exception:  result+=" Exception"; break;
        default: break;
      }
      if (isTemplate) result+=" Template";
      result+=" Referentie";
      return result;
    }
    /*! used as the title of the HTML page of a module (Fortran) */
    DString trModuleReference(const DString &namespaceName) override
    {
      DString result=namespaceName;
      result+=" Module Referentie";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    DString trModulesMembers() override
    { return "Module Members"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    DString trModulesMemberDescription(bool extractAll) override
    {
      DString result="Hier is een lijst van alle ";
      if (!extractAll) result+="gedocumenteerde ";
      result+="module members met links naar ";
      if (extractAll)
        result+="de module documentatie voor iedere member:";
      else
        result+="de module waartoe ze behoren:";
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all modules (Fortran).
     */
    DString trModulesIndex() override
    { return "Module Index"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trModule(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "module", "n");
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    DString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      DString result="De documentatie voor ";
      switch(compType)
      {
        case ClassDef::Class:      result+="deze module"; break;
        case ClassDef::Struct:     result+="dit type"; break;
        case ClassDef::Union:      result+="deze union"; break;
        case ClassDef::Interface:  result+="dit interface"; break;
        case ClassDef::Protocol:   result+="dit protocol"; break;
        case ClassDef::Category:   result+="deze category"; break;
        case ClassDef::Exception:  result+="deze exception"; break;
        default: break;
      }
      result+=" is gegenereerd op basis van ";
      if (single) result+="het"; else result+="de";
      result+=" volgende bestand";
      if (single) result+=":"; else result+="en:";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trType(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "type", "s");
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    DString trSubprogram(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "subprogramma", "s");
    }

    /*! C# Type Constraint list */
    DString trTypeConstraints() override
    {
      return "Type Beperkingen";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    DString trDirRelation(const DString &name) override
    {
      return name+" Relatie";
    }

    /*! Loading message shown when loading search results */
    DString trLoading() override
    {
      return "Laden...";
    }

    /*! Label used for search results in the global namespace */
    DString trGlobalNamespace() override
    {
      return "Globale Namespace";
    }

    /*! Message shown while searching */
    DString trSearching() override
    {
      return "Zoeken...";
    }

    /*! Text shown when no search results are found */
    DString trNoMatches() override
    {
      return "Niets gevonden";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.3 (missing items for the directory pages)
//////////////////////////////////////////////////////////////////////////

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the first column mentions the
     *  source file that has a relation to another file.
     */
    DString trFileIn(const DString &name) override
    {
      return "Bestand in "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    DString trIncludesFileIn(const DString &name) override
    {
      return "Includeert bestand in "+name;
    }
    DString trDateTime(int year,int month,int day,int dayOfWeek,
                                int hour,int minutes,int seconds,
                                DateTimeType includeTime) override
    {
      static const char *days[]   = { "Ma","Di","Wo","Do","Vr","Za","Zo" };
      static const char *months[] = { "Jan","Feb","Maa","Apr","Mei","Jun","Jul","Aug","Sep","Okt","Nov","Dec" };
      DString sdate;
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Date)
      {
        sdate.sprintf("%s %d %s %d",days[dayOfWeek-1],day,months[month-1],year);
      }
      if (includeTime == DateTimeType::DateTime) sdate += " ";
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Time)
      {
        DString stime;
        stime.sprintf("%.2d:%.2d:%.2d",hour,minutes,seconds);
        sdate+=stime;
      }
      return sdate;
    }
    DString trDayOfWeek(int dayOfWeek, bool first_capital, bool full) override
    {
      static const char *days_short[]   = { "ma", "di", "wo", "do", "vr", "za", "zo" };
      static const char *days_full[]    = { "maandag", "dinsdag", "woensdag", "donderdag", "vrijdag", "zaterdag", "zondag" };
      DString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    DString trMonth(int month, bool first_capital, bool full) override
    {
      static const char *months_short[] = { "jan", "feb", "mrt", "apr", "mei", "jun", "jul", "aug", "sep", "okt", "nov", "dec" };
      static const char *months_full[]  = { "januari", "februari", "maart", "april", "mei", "juni", "juli", "augustus", "september", "oktober", "november", "december" };
      DString text  = full? months_full[month-1] : months_short[month-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    DString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "a.m.", "p.m." };
      return dayPeriod[period?1:0];
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    DString trCiteReferences() override
    { return "Bibliografie"; }

    /*! Text for copyright paragraph */
    DString trCopyright() override
    { return "Copyright"; }

    /*! Header for the graph showing the directory dependencies */
    DString trDirDepGraph(const DString &name) override
    { return "Folder afhankelijkheidsgraaf voor "+name+":"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    DString trDetailLevel() override
    { return "detail niveau"; }

    /*! Section header for list of template parameters */
    DString trTemplateParameters() override
    { return "Template Parameters"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    DString trAndMore(const DString &number) override
    { return "en "+number+ " anderen..."; }

    /*! Used file list for a Java enum */
    DString trEnumGeneratedFromFiles(bool single) override
    { DString result = "De documentatie voor deze enum is gegenereerd op basis van ";
      if (single) result+="het"; else result+="de";
      result+=" volgende bestand";
      if (single) result+=":"; else result+="en:";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    DString trEnumReference(const DString &name) override
    { return name+" Enum Referentie"; }

    /*! Used for a section containing inherited members */
    DString trInheritedFrom(const DString &members,const DString &what) override
    { return members+" overgeërfd van "+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    DString trAdditionalInheritedMembers() override
    { return "Additionele Overgeërfde Members"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    DString trPanelSynchronisationTooltip(bool enable) override
    {
      DString opt = enable ? "in" : "uit";
      return "klik hier om de paneel synchronisatie "+opt+" te schakelen";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    DString trProvidedByCategory() override
    {
      return "Wordt aangeboden door category @0.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    DString trExtendsClass() override
    {
      return "Uitbereiding van klasse @0.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    DString trClassMethods() override
    {
      return "Klasse Methoden";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    DString trInstanceMethods() override
    {
      return "Instantie Methoden";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    DString trMethodDocumentation() override
    {
      return "Methode Documentatie";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    DString trInterfaces() override
    { return "Geëporteerde Interfaces"; }

    /** old style UNO IDL services: inherited services */
    DString trServices() override
    { return "Geïncludeerde Services"; }

    /** UNO IDL constant groups */
    DString trConstantGroups() override
    { return "Konstanten Groepen"; }

    /** UNO IDL constant groups */
    DString trConstantGroupReference(const DString &namespaceName) override
    {
      DString result=namespaceName;
      result+=" Konstanten Groepen Referentie";
      return result;
    }
    /** UNO IDL service page title */
    DString trServiceReference(const DString &sName) override
    {
      DString result=sName;
      result+=" Service Referentie";
      return result;
    }
    /** UNO IDL singleton page title */
    DString trSingletonReference(const DString &sName) override
    {
      DString result=sName;
      result+=" Singleton Referentie";
      return result;
    }
    /** UNO IDL service page */
    DString trServiceGeneratedFromFiles(bool single) override
    { DString result = "De documentatie voor deze service is gegenereerd op basis van ";
      if (single) result+="het"; else result+="de";
      result+=" volgende bestand";
      if (single) result+=":"; else result+="en:";
      return result;
    }
    /** UNO IDL singleton page */
    DString trSingletonGeneratedFromFiles(bool single) override
    { DString result = "De documentatie voor deze singleton is gegenereerd op basis van ";
      if (single) result+="het"; else result+="de";
      result+=" volgende bestand";
      if (single) result+=":"; else result+="en:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.15
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit hierarchy */
    DString trDesignUnitHierarchy() override
    { return "Ontwerp Eenheid Hiërarchie"; }
    /** VHDL design unit list */
    DString trDesignUnitList() override
    { return "Ontwerp Eenheid Lijst"; }
    /** VHDL design unit members */
    DString trDesignUnitMembers() override
    { return "Ontwerp Eenheid Members"; }
    /** VHDL design unit list description */
    DString trDesignUnitListDescription() override
    {
      return "hieronder volgt de lijst met all ontwerp eenheden met links "
             "naar de entiteiten waar ze bij behoren:";
    }
    /** VHDL design unit index */
    DString trDesignUnitIndex() override
    { return "Ontwerp Eenheid Index"; }
    /** VHDL design units */
    DString trDesignUnits() override
    { return "Ontwerp Eenheden"; }
    /** VHDL functions/procedures/processes */
    DString trFunctionAndProc() override
    { return "Functies/Procedures/Processen"; }
    /** VHDL type */
    DString trVhdlType(VhdlSpecifier type,bool single) override
    {
      switch(type)
      {
        case VhdlSpecifier::LIBRARY:
          if (single) return "Bibliotheek";
          else        return "Bibliotheken";
        case VhdlSpecifier::PACKAGE:
          if (single) return "Package";
          else        return "Packages";
        case VhdlSpecifier::SIGNAL:
          if (single) return "Signal";
          else        return "Signals";
        case VhdlSpecifier::COMPONENT:
          if (single) return "Bestanddeel";
          else        return "Bestanddelen";
        case VhdlSpecifier::CONSTANT:
          if (single) return "Konstante";
          else        return "Konstanten";
        case VhdlSpecifier::ENTITY:
          if (single) return "Entiteit";
          else        return "Entiteiten";
        case VhdlSpecifier::TYPE:
          if (single) return "Type";
          else        return "Types";
        case VhdlSpecifier::SUBTYPE:
          if (single) return "Ondertype";
          else        return "Ondertypes";
        case VhdlSpecifier::FUNCTION:
          if (single) return "Funktie";
          else        return "Funkties";
        case VhdlSpecifier::RECORD:
          if (single) return "Record";
          else        return "Records";
        case VhdlSpecifier::PROCEDURE:
          if (single) return "Procedure";
          else        return "Procedures";
        case VhdlSpecifier::ARCHITECTURE:
          if (single) return "Architectuur";
          else        return "Architecturen";
        case VhdlSpecifier::ATTRIBUTE:
          if (single) return "Attribuut";
          else        return "Attributen";
        case VhdlSpecifier::PROCESS:
          if (single) return "Proces";
          else        return "Processen";
        case VhdlSpecifier::PORT:
          if (single) return "Poort";
          else        return "Porten";
        case VhdlSpecifier::USE:
          if (single) return "gebruiks clausule";
          else        return "Gebruiks Clausules";
        case VhdlSpecifier::GENERIC:
          if (single) return "Algemeen";
          else        return "Algemene";
        case VhdlSpecifier::PACKAGE_BODY:
          return "Package Body";
        case VhdlSpecifier::UNITS:
          return "Eenheden";
        case VhdlSpecifier::SHAREDVARIABLE:
          if (single) return "Gedeelde Variable";
          else        return "Gedeelde Variablen";
        case VhdlSpecifier::VFILE:
          if (single) return "Bestand";
          else        return "Bestanden";
        case VhdlSpecifier::GROUP:
          if (single) return "Groep";
          else        return "Groepen";
        case VhdlSpecifier::INSTANTIATION:
          if (single) return "Instantiëring";
          else        return "Instantiëringen";
        case VhdlSpecifier::ALIAS:
          if (single) return "Alias";
          else        return "Aliases";
        case VhdlSpecifier::CONFIG:
          if (single) return "Configuratie";
          else        return "Configuraties";
        case VhdlSpecifier::MISCELLANEOUS:
          return "Diverse";
        case VhdlSpecifier::UCF_CONST:
          return "Limiteringen";
        default:
          return "Klasse";
      }
    }
    DString trCustomReference(const DString &name) override
    { return name+" Referentie"; }

    /* Slice */
    DString trConstants() override
    { return "Konstanten"; }
    DString trConstantDocumentation() override
    { return "Documentatie van konstanten"; }
    DString trSequences() override
    { return "Reeksen"; }
    DString trSequenceDocumentation() override
    { return "Documentatie van reeksen"; }
    DString trDictionaries() override
    { return "Vertalingslijsten"; }
    DString trDictionaryDocumentation() override
    { return "Documentatie van vertalingslijsten"; }
    DString trSliceInterfaces() override
    { return "Interfaces"; }
    DString trInterfaceIndex() override
    { return "Index van interfaces"; }
    DString trInterfaceList() override
    { return "Lijst van interfaces"; }
    DString trInterfaceListDescription() override
    { return "Hieronder volgt de lijst met alle interfaces, elk met een korte beschrijving:"; }
    DString trInterfaceHierarchy() override
    { return "Interface Hiërarchie"; }
    DString trInterfaceHierarchyDescription() override
    { return "Deze inheritance lijst is min of meer alfabetisch gesorteerd:"; }
    DString trInterfaceDocumentation() override
    { return "Documentatie van interfaces"; }
    DString trStructs() override
    { return "Structs"; }
    DString trStructIndex() override
    { return "Index van struct"; }
    DString trStructList() override
    { return "Lijst van struct"; }
    DString trStructListDescription() override
    { return "Hieronder volgt de lijst met alle structs, elk met een korte beschrijving:"; }
    DString trStructDocumentation() override
    { return "Documentatie van structs"; }
    DString trExceptionIndex() override
    { return "Index van exceptions"; }
    DString trExceptionList() override
    { return "Lijst van exceptions"; }
    DString trExceptionListDescription() override
    { return "Hieronder volgt de lijst met alle exeptions, elk met een korte beschrijving:"; }
    DString trExceptionHierarchy() override
    { return "Exception Hiërarchie"; }
    DString trExceptionHierarchyDescription() override
    { return "Deze inheritance lijst is min of meer alfabetisch gesorteerd:"; }
    DString trExceptionDocumentation() override
    { return "Documentatie van exceptions"; }
    DString trCompoundReferenceSlice(const DString &clName, ClassDef::CompoundType compType, bool isLocal) override
    {
      DString result=clName;
      if (isLocal) result+=" Lokale";
      switch(compType)
      {
        case ClassDef::Class:  result+=" Class"; break;
        case ClassDef::Struct: result+=" Struct"; break;
        case ClassDef::Union:  result+=" Union"; break;
        case ClassDef::Interface:  result+=" Interface"; break;
        case ClassDef::Protocol:  result+=" Protocol"; break;
        case ClassDef::Category:  result+=" Category"; break;
        case ClassDef::Exception:  result+=" Exception"; break;
        default: break;
      }
      result+=" Referentie";
      return result;
    }
    DString trOperations() override
    { return "Bewerkingen"; }
    DString trOperationDocumentation() override
    { return "Documentatie van bewerkingen"; }
    DString trDataMembers() override
    { return "Data members"; }
    DString trDataMemberDocumentation() override
    { return "Documentatie van data members"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.19
//////////////////////////////////////////////////////////////////////////
    DString trDesignUnitDocumentation() override
    { return "Ontwerp Eenheid Documentatie"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.2
//////////////////////////////////////////////////////////////////////////
    DString trConcept(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "concept", "en");
    }

    DString trConceptReference(const DString &conceptName) override
    {
      DString result=conceptName;
      result+=" Concept Referentie";
      return result;
    }

    DString trConceptList() override
    { return "Concept Lijst"; }

    DString trConceptIndex() override
    { return "Concept Index"; }

    DString trConceptDocumentation() override
    { return "Concept Documentatie"; }

    DString trConceptListDescription(bool extractAll) override
    {
      DString result="Hieronder volgt de lijst met alle ";
      if (!extractAll) result+="gedocumenteerde ";
      result+="concepten, elk met een korte beschrijving:";
      return result;
    }

    DString trConceptDefinition() override
    {
      return "Concept definitie";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.4
//////////////////////////////////////////////////////////////////////////

    DString trPackageList() override
    { return "Package Lijst"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used for translation of the word that will be
     *  followed by a single name of the VHDL process flowchart.
     */
    DString trFlowchart() override
    { return "Stroomschema:"; }

    /*! Please translate also updated body of the method
     *  trMemberFunctionDocumentation(), now better adapted for
     *  VHDL sources documentation.
     *  Done.
     */

//////////////////////////////////////////////////////////////////////////
// new since 1.9.7
//////////////////////////////////////////////////////////////////////////
    /*! used in the compound documentation before a list of related symbols.
     *
     *  Supersedes trRelatedFunctions
     */
    DString trRelatedSymbols() override
    { return "Gerelateerde symbolen"; }

    /*! subscript for the related symbols
     *
     *  Supersedes trRelatedSubscript
     */
    DString trRelatedSymbolsSubscript() override
    { return "(Merk op dat dit geen member symbolen zijn.)"; }

    /*! used in the class documentation as a header before the list of all
     * related classes.
     *
     * Supersedes trRelatedFunctionDocumentation
     */
    DString trRelatedSymbolDocumentation() override
    { return "Documentatie van friends en gerelateerde symbolen"; }

    DString trCompoundType(ClassDef::CompoundType compType, SrcLangExt lang) override
    {
      DString result;
      switch(compType)
      {
        case ClassDef::Class:
          if (lang == SrcLangExt::Fortran) result=trType(true,true);
          else result=trClass(true,true);
          break;
        case ClassDef::Struct:     result="Struct"; break;
        case ClassDef::Union:      result="Union"; break;
        case ClassDef::Interface:  result="Interface"; break;
        case ClassDef::Protocol:   result="Protocol"; break;
        case ClassDef::Category:   result="Category"; break;
        case ClassDef::Exception:  result="Exception"; break;
        case ClassDef::Service:    result="Service"; break;
        case ClassDef::Singleton:  result="Singleton"; break;
        default: break;
      }
      return result;
    }

    DString trFileMembersDescriptionTotal(FileMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      DString result="Hieronder volgt de lijst met alle ";
      if (!extractAll) result+="gedocumenteerde ";

      switch (hl)
      {
        case FileMemberHighlight::All:
          if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
          {
            result+="functies, variabelen, macros, enumeraties, en typedefs";
          }
          else
          {
            result+="bestand members";
          }
          break;
        case FileMemberHighlight::Functions:
          result+="functies";
          break;
        case FileMemberHighlight::Variables:
          result+="variabelen";
          break;
        case FileMemberHighlight::Typedefs:
          result+="typedefs";
          break;
        case FileMemberHighlight::Sequences:
          result+="reeksen";
          break;
        case FileMemberHighlight::Dictionaries:
          result+="vertalingslijsten";
          break;
        case FileMemberHighlight::Enums:
          result+="enumeratie";
          break;
        case FileMemberHighlight::EnumValues:
          result+="enumeratie waarden";
          break;
        case FileMemberHighlight::Defines:
          result+="macros";
          break;
        case FileMemberHighlight::Total: // for completeness
          break;
      }
      result+=" met links naar ";
      if (extractAll) result+="de bestand's documentatie voor elke member:";
      else result+="de bestanden waartoe ze behoren:";
      return result;
    }
        //return trCompoundMembersDescription(Config_getBool(EXTRACT_ALL));

    DString trCompoundMembersDescriptionTotal(ClassMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      DString result="Hieronder volgt de lijst met alle ";
      if (!extractAll) result+="gedocumenteerde ";

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="struct en union velden";
      }
      else
      {
        result+="klasse members";
      }

      switch (hl)
      {
        case ClassMemberHighlight::All:
          if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
          {
            result+="struct en union velden";
          }
          else
          {
            result+="klasse members";
          }
          break;
        case ClassMemberHighlight::Functions:
          result+="functies";
          break;
        case ClassMemberHighlight::Variables:
          result+="variabelen";
          break;
        case ClassMemberHighlight::Typedefs:
          result+="typedefs";
          break;
        case ClassMemberHighlight::Enums:
          result+="enumeraties";
          break;
        case ClassMemberHighlight::EnumValues:
          result+="enumeratie waarden";
          break;
        case ClassMemberHighlight::Properties:
          result+="properties";
          break;
        case ClassMemberHighlight::Events:
          result+="events";
          break;
        case ClassMemberHighlight::Related:
          result+="gerelateerde symbolen";
          break;
        case ClassMemberHighlight::Total: // for completeness
          break;
      }
      result+=" met links naar ";
      if (!extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="de struct/union documentatie voor elke veld:";
        }
        else
        {
           result+="de klasse documentatie voor elke symbool:";
        }
      }
      else
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="de structures/unions waartoe ze behoren:";
        }
        else
        {
          result+="de klassen waartoe ze behoren:";
        }
      }
      return result;
    }

    DString trNamespaceMembersDescriptionTotal(NamespaceMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      DString result="Hier is een lijst van alle ";
      if (!extractAll) result+="gedocumenteerde ";
      result+="namespace ";
      DString singularResult = "";
      DString pluralResult = "";
      switch (hl)
      {
        case NamespaceMemberHighlight::All:
          singularResult="symbool";
          pluralResult="symbolen";
          break;
        case NamespaceMemberHighlight::Functions:
          singularResult="e functie";
          pluralResult="functies";
          break;
        case NamespaceMemberHighlight::Variables:
          singularResult="e variabele";
          pluralResult="variabelen";
          break;
        case NamespaceMemberHighlight::Typedefs:
          singularResult="e typedef";
          pluralResult="typedefs";
          break;
        case NamespaceMemberHighlight::Sequences:
          singularResult="e reeks";
          pluralResult="reeksen";
          break;
        case NamespaceMemberHighlight::Dictionaries:
          singularResult="e vertalingslijst";
          pluralResult="vertalingslijsten";
          break;
        case NamespaceMemberHighlight::Enums:
          singularResult="e enumeratie";
          pluralResult="enumeraties";
          break;
        case NamespaceMemberHighlight::EnumValues:
          singularResult="e enumeratie waarde";
          pluralResult="enumeratie waarden";
          break;
        case NamespaceMemberHighlight::Total: // for completeness
          break;
      }
      result+=pluralResult;
      result+=" met links naar ";
      if (extractAll)
        result+="de namespace documentatie voor ieder" + singularResult + ":";
      else
        result+="de namespaces waartoe ze behoren:";
      return result;
    }

    DString trDefinition() override  { return "Definitie";}
    DString trDeclaration() override { return "Declaratie";}

//////////////////////////////////////////////////////////////////////////
// new since 1.9.8
//////////////////////////////////////////////////////////////////////////

    DString trTopics() override
    { return "Onderwerpen"; }
    DString trTopicDocumentation() override
    { return "Documentatie voor dit onderwerp"; }
    DString trTopicIndex() override
    { return "Overwerpen"; }
    DString trTopicList() override
    { return "Lijst met onderwerpen"; }
    DString trTopicListDescription() override
    { return "Hieronder volgt de lijst met alle onderwerpen, elk met een korte beschrijving:"; }
    DString trModuleMembersDescriptionTotal(ModuleMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      DString result="Hier is een lijst van alle ";
      if (!extractAll) result+="gedocumenteerde ";
      result+="module ";
      DString singularResult = "";
      DString pluralResult = "";
      switch (hl)
      {
        case ModuleMemberHighlight::All:
          singularResult="symbool";
          pluralResult="symbolen";
          break;
        case ModuleMemberHighlight::Functions:
          singularResult="e functie";
          pluralResult="functies";
          break;
        case ModuleMemberHighlight::Variables:
          singularResult="e variabele";
          pluralResult="variabelen";
          break;
        case ModuleMemberHighlight::Typedefs:
          singularResult="e typedef";
          pluralResult="typedefs";
          break;
        case ModuleMemberHighlight::Enums:
          singularResult="e enumeratie";
          pluralResult="enumeraties";
          break;
        case ModuleMemberHighlight::EnumValues:
          singularResult="e enumeratie waarde";
          pluralResult="enumeratie waarden";
          break;
        case ModuleMemberHighlight::Total: // for completeness
          break;
      }
      result+=pluralResult;
      result+=" met links naar ";
      if (extractAll)
        result+="de module documentatie voor ieder" + singularResult + ":";
      else
        result+="de modules waartoe ze behoren:";
      return result;
    }
    DString trExportedModules() override
    {
      return "Geëxporteerde Modules";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.10.0
//////////////////////////////////////////////////////////////////////////

    DString trCopyToClipboard() override
    {
      return "Kopiëren naar het klembord";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.11.0
//////////////////////////////////////////////////////////////////////////

    DString trImportant() override
    {
      return "Belangrijk";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.16.0
//////////////////////////////////////////////////////////////////////////

    // the title of the requirements overview page
    DString trRequirements() override
    {
      return "Vereisten";
    }
    // table header for the column with the requirements IDs
    DString trRequirementID() override
    {
      return "ID";
    }
    // indicates a symbol implements (satisfies) a requirement
    DString trSatisfies(bool singular) override
    {
      return createNoun(true, singular, "Voldoet aan vereiste", "n");
    }
    // indicates a requirement is satisfied (implemented) by one or more symbols
    DString trSatisfiedBy(const DString &list) override
    {
      return "Wordt aan voldaan door "+list+".";
    }
    DString trUnsatisfiedRequirements() override
    {
      return "Onvoldane Vereisten";
    }
    DString trUnsatisfiedRequirementsText(bool singular,const DString &list) override
    {
      return singular ?
        "De vereiste "+list+" mist de 'voldaan' relatie." :
        "De vereisten "+list+" missen de 'voldaan' relatie.";
    }
    // indicates a symbol verifies (tests) a requirement
    DString trVerifies(bool singular) override
    {
      return createNoun(true, singular, "Verifieert vereiste", "n");
    }
    // indicates a requirement is verified (tested) by one or more symbols
    DString trVerifiedBy(const DString &list) override
    {
      return "Wordt geverifieerd door "+list+".";
    }
    DString trUnverifiedRequirements() override
    {
      return "Ongeverifieerde vereisten";
    }
    DString trUnverifiedRequirementsText(bool singular,const DString &list) override
    {
      return singular ?
        "De vereiste "+list+" mist de 'verifieer' relatie." :
        "De vereisten "+list+" missen de 'verifieer' relatie.";
    }

};

#endif
