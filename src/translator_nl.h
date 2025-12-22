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

#ifndef TRANSLATOR_NL_H
#define TRANSLATOR_NL_H

class TranslatorDutch : public Translator
{
  public:
    QCString idLanguage() override
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
    QCString latexLanguageSupportCommand() override
    { return "\\usepackage[dutch]{babel}\n"; }
    QCString trISOLang() override
    { return "nl"; }
    QCString getLanguageString() override
    {
      return "0x413 Dutch";
    }
    QCString trRelatedFunctions() override
    { return "Gerelateerde functies"; }
    QCString trRelatedSubscript() override
    { return "(Merk op dat dit geen member functies zijn.)"; }
    QCString trDetailedDescription() override
    { return "Gedetailleerde Beschrijving"; }
    QCString trDetails() override
    { return "Details"; }

    QCString trMemberTypedefDocumentation() override
    { return "Documentatie van type definitie members"; }
    QCString trMemberEnumerationDocumentation() override
    { return "Documentatie van enumeratie members"; }
    QCString trMemberFunctionDocumentation() override
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
    QCString trMemberDataDocumentation() override
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
    QCString trMore() override
    { return "Meer..."; }
    QCString trListOfAllMembers() override
    { return "Lijst van alle members"; }
    QCString trMemberList() override
    { return "Member Lijst"; }
    QCString trThisIsTheListOfAllMembers() override
    { return "Dit is de complete lijst van alle members voor"; }
    QCString trIncludingInheritedMembers() override
    { return ", inclusief alle overgeërfde members."; }
    QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="Automatisch gegenereerd door Doxygen";
      if (!s.isEmpty()) result+=" voor "+s;
      result+=" uit de programmatekst.";
      return result;
    }
    QCString trEnumName() override
    { return "enum naam"; }
    QCString trEnumValue() override
    { return "enum waarde"; }
    QCString trDefinedIn() override
    { return "gedefinieerd in"; }
    QCString trModules() override
    { return "Modules"; }
    QCString trClassHierarchy() override
    { return "Klasse Hiërarchie"; }
    QCString trCompoundList() override
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
    QCString trFileList() override
    { return "Bestandslijst"; }
    QCString trCompoundMembers() override
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
    QCString trFileMembers() override
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
    QCString trRelatedPages() override
    { return "Gerelateerde pagina's"; }
    QCString trExamples() override
    { return "Voorbeelden"; }
    QCString trSearch() override
    { return "Zoeken"; }
    QCString trClassHierarchyDescription() override
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
    QCString trFileListDescription(bool extractAll) override
    {
      QCString result="Hieronder volgt de lijst met alle ";
      if (!extractAll) result+="gedocumenteerde ";
      result+="bestanden, elk met een korte beschrijving:";
      return result;
    }
    QCString trCompoundListDescription() override
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
    QCString trCompoundMembersDescription(bool extractAll) override
    {
      QCString result="Hieronder volgt de lijst met alle ";
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
    QCString trFileMembersDescription(bool extractAll) override
    {
      QCString result="Hieronder volgt de lijst met alle ";
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
    QCString trExamplesDescription() override
    { return "Hieronder volgt de lijst met alle voorbeelden:"; }
    QCString trRelatedPagesDescription() override
    { return "Hieronder volgt de lijst met alle pagina's die gerelateerde documentatie bevatten:"; }
    QCString trModulesDescription() override
    { return "Hieronder volgt de lijst met alle modules:"; }

    QCString trDocumentation(const QCString &projName) override
    { return (!projName.isEmpty()?projName + " " : "") + "Documentatie"; }
    QCString trModuleIndex() override
    { return "Module Index"; }
    QCString trHierarchicalIndex() override
    { return "Hiërarchische Index"; }
    QCString trCompoundIndex() override
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
    QCString trFileIndex() override
    { return "Bestand Index"; }
    QCString trModuleDocumentation() override
    { return "Module Documentatie"; }
    QCString trClassDocumentation() override
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
    QCString trFileDocumentation() override
    { return "Bestand Documentatie"; }
    QCString trReferenceManual() override
    { return "Naslagwerk"; }

    QCString trDefines() override
    { return "Macros"; }
    QCString trTypedefs() override
    { return "Typedefs"; }
    QCString trEnumerations() override
    { return "Enumeraties"; }
    QCString trFunctions() override
    { return "Functies"; }
    QCString trVariables() override
    { return "Variabelen"; }
    QCString trEnumerationValues() override
    { return "Enumeratie waarden"; }
    QCString trDefineDocumentation() override
    { return "Documentatie van macro's"; }
    QCString trTypedefDocumentation() override
    { return "Documentatie van typedefs"; }
    QCString trEnumerationTypeDocumentation() override
    { return "Documentatie van enumeratie types"; }
    QCString trEnumerationValueDocumentation() override
    { return "Documentatie van enumeratie waarden"; }
    QCString trFunctionDocumentation() override
    { return "Documentatie van functies"; }
    QCString trVariableDocumentation() override
    { return "Documentatie van variabelen"; }
    QCString trCompounds() override
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
    QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result="Gegenereerd op "+date;
      if (!projName.isEmpty()) result+=" voor "+projName;
      result+=" door";
      return result;
    }
    QCString trClassDiagram(const QCString &clName) override
    {
      return "Klasse diagram voor "+clName;
    }
    QCString trWarning() override
    { return "Waarschuwing"; }
    QCString trVersion() override
    { return "Versie"; }
    QCString trDate() override
    { return "Datum"; }
    QCString trReturns() override
    { return "Retourneert"; }
    QCString trSeeAlso() override
    { return "Zie ook"; }
    QCString trParameters() override
    { return "Parameters"; }
    QCString trExceptions() override
    { return "Excepties"; }
    QCString trGeneratedBy() override
    { return "Gegenereerd door"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    QCString trNamespaceList() override
    { return "Namespace Lijst"; }
    QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="Hier is een lijst met alle ";
      if (!extractAll) result+="gedocumenteerde ";
      result+="namespaces met voor elk een korte beschrijving:";
      return result;
    }
    QCString trFriends() override
    { return "Friends"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    QCString trRelatedFunctionDocumentation() override
    { return "Documentatie van friends en gerelateerde functies"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    QCString trCompoundReference(const QCString &clName,
                                 ClassDef::CompoundType compType,
                                 bool isTemplate) override
      // used as the title of the HTML page of a class/struct/union
    {
      QCString result=clName;
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
    QCString trFileReference(const QCString &fileName) override
      // used as the title of the HTML page of a file
    {
      QCString result=fileName;
      result+=" Bestand Referentie";
      return result;
    }
    QCString trNamespaceReference(const QCString &namespaceName) override
      // used as the title of the HTML page of a namespace
    {
      QCString result=namespaceName;
      result+=" Namespace Referentie";
      return result;
    }

    // these are for the member sections of a class, struct or union
    QCString trPublicMembers() override
    { return "Public Members"; }
    QCString trPublicSlots() override
    { return "Public Slots"; }
    QCString trSignals() override
    { return "Signals"; }
    QCString trStaticPublicMembers() override
    { return "Static Public Members"; }
    QCString trProtectedMembers() override
    { return "Protected Members"; }
    QCString trProtectedSlots() override
    { return "Protected Slots"; }
    QCString trStaticProtectedMembers() override
    { return "Static Protected Members"; }
    QCString trPrivateMembers() override
    { return "Private Members"; }
    QCString trPrivateSlots() override
    { return "Private Slots"; }
    QCString trStaticPrivateMembers() override
    { return "Static Private Members"; }
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
            result+=" en ";
        }
      }
      return result;
    }

    QCString trInheritsList(int numEntries) override
      // used in class documentation to produce a list of base classes,
      // if class diagrams are disabled.
    {
      return "Erft over van "+trWriteList(numEntries)+".";
    }
    QCString trInheritedByList(int numEntries) override
      // used in class documentation to produce a list of super classes,
      // if class diagrams are disabled.
    {
      return "Wordt overgeërfd door "+trWriteList(numEntries)+".";
    }
    QCString trReimplementedFromList(int numEntries) override
      // used in member documentation blocks to produce a list of
      // members that are hidden by this one.
    {
      return "Nieuwe implementatie van "+trWriteList(numEntries)+".";
    }
    QCString trReimplementedInList(int numEntries) override
    {
      // used in member documentation blocks to produce a list of
      // all member that overwrite the implementation of this member.
      return "Opnieuw geïmplementeerd in "+trWriteList(numEntries)+".";
    }

    QCString trNamespaceMembers() override
      // This is put above each page as a link to all members of namespaces.
    { return "Namespace Members"; }
    QCString trNamespaceMemberDescription(bool extractAll) override
      // This is an introduction to the page with all namespace members
    {
      QCString result="Hier is een lijst van alle ";
      if (!extractAll) result+="gedocumenteerde ";
      result+="namespace members met links naar ";
      if (extractAll)
        result+="de namespace documentatie voor iedere member:";
      else
        result+="de namespaces waartoe ze behoren:";
      return result;
    }
    QCString trNamespaceIndex() override
      // This is used in LaTeX as the title of the chapter with the
      // index of all namespaces.
    { return "Namespace Index"; }
    QCString trNamespaceDocumentation() override
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all namespaces.
    { return "Namespace Documentatie"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces() override
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
        bool single) override
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      bool vhdlOpt = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
      QCString result="De documentatie voor ";
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
    QCString trReturnValues() override
    { return "Retour waarden"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage() override
    { return "Hoofd Pagina"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    QCString trPageAbbreviation() override
    { return "p."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991106
//////////////////////////////////////////////////////////////////////////

    QCString trDefinedAtLineInSourceFile() override
    {
      return "De definitie bevindt zich op regel @0 in het bestand @1.";
    }
    QCString trDefinedInSourceFile() override
    {
      return "De definitie bevindt zich in het bestand @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated() override
    {
      return "Verouderd";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const QCString &clName) override
    {
      return "Collaboratie diagram voor "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const QCString &fName) override
    {
      return "Include afhankelijkheidsgraaf voor "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation() override
    {
      return "Constructor & Destructor Documentatie";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode() override
    {
      return "Ga naar de broncode van dit bestand.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation() override
    {
      return "Ga naar de documentatie van dit bestand.";
    }
    /*! Text for the \\pre command */
    QCString trPrecondition() override
    {
      return "Preconditie";
    }
    /*! Text for the \\post command */
    QCString trPostcondition() override
    {
      return "Postconditie";
    }
    /*! Text for the \\invariant command */
    QCString trInvariant() override
    {
      return "Invariant";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue() override
    {
      return "Initiële waarde:";
    }
    /*! Text used the source code in the file index */
    QCString trCode() override
    {
      return "code";
    }
    QCString trGraphicalHierarchy() override
    {
      return "Grafische Klasse Hiërarchie";
    }
    QCString trGotoGraphicalHierarchy() override
    {
      return "Ga naar de grafische klasse hiërarchie";
    }
    QCString trGotoTextualHierarchy() override
    {
      return "Ga naar de tekstuele klasse hiërarchie";
    }
    QCString trPageIndex() override
    {
      return "Pagina Index";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    QCString trNote() override
    {
      return "Noot";
    }
    QCString trPublicTypes() override
    {
      return "Public Typen";
    }
    QCString trPublicAttribs() override
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
    QCString trStaticPublicAttribs() override
    {
      return "Static Public Attributen";
    }
    QCString trProtectedTypes() override
    {
      return "Protected Typen";
    }
    QCString trProtectedAttribs() override
    {
      return "Protected Attributen";
    }
    QCString trStaticProtectedAttribs() override
    {
      return "Static Protected Attributen";
    }
    QCString trPrivateTypes() override
    {
      return "Private Typen";
    }
    QCString trPrivateAttribs() override
    {
      return "Private Attributen";
    }
    QCString trStaticPrivateAttribs() override
    {
      return "Static Private Attributen";
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    QCString trTodo() override
    {
      return "Todo";
    }
    /*! Used as the header of the todo list */
    QCString trTodoList() override
    {
      return "Todo Lijst";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy() override
    {
      return "Wordt aangeroepen door";
    }
    QCString trRemarks() override
    {
      return "Opmerkingen";
    }
    QCString trAttention() override
    {
      return "Attentie";
    }
    QCString trInclByDepGraph() override
    {
      return "Deze graaf geeft aan welke bestanden direct of "
             "indirect afhankelijk zijn van dit bestand:";
    }
    QCString trSince() override
    {
      return "Sinds";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    QCString trLegendTitle() override
    {
      return "Graaf Legenda";
    }
    /*! page explaining how the dot graph's should be interpreted */
    QCString trLegendDocs() override
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
      return "Test Lijst";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    QCString trProperties() override
    {
      return "Properties";
    }
    /*! Used as a section header for IDL property documentation */
    QCString trPropertyDocumentation() override
    {
      return "Property Documentatie";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    QCString trClasses() override
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
    QCString trPackage(const QCString &name) override
    {
      return "Package "+name;
    }
    /*! The description of the package index page */
    QCString trPackageListDescription() override
    {
      return "Hier volgen de packages, elk met een korte beschrijving (indien aanwezig):";
    }
    /*! The link name in the Quick links header for each page */
    QCString trPackages() override
    {
      return "Packages";
    }
    /*! Text shown before a multi-line define */
    QCString trDefineValue() override
    {
      return "Waarde:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    QCString trBug() override
    {
      return "Bug";
    }
    /*! Used as the header of the bug list */
    QCString trBugList() override
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
    QCString trRTFansicp() override
    {
      return "1252";
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
      return "Index";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trClass(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "klasse", "n");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trFile(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "bestand", "en");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trNamespace(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "namespace", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGroup(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "groep", "en");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trPage(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "pagina", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trMember(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "member", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trGlobal(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "globale member", "s");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    QCString trAuthor(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "auteur", "s");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    QCString trReferences() override
    {
      return "Gebruikt";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    QCString trImplementedFromList(int numEntries) override
    {
      return "Implementeert "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implementation this member.
     */
    QCString trImplementedInList(int numEntries) override
    {
      return "Wordt geïmplementeerd door "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    QCString trRTFTableOfContents() override
    { return "Inhoudsopgave"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    QCString trDeprecatedList() override
    {
      return "Deprecated Lijst";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for a section of events found in a C# program
     */
    QCString trEvents() override
    {
      return "Events";
    }
    /*! Header used for the documentation section of a class' events. */
    QCString trEventDocumentation() override
    {
      return "Event Documentatie";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    QCString trPackageTypes() override
    {
      return "Package Types";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    QCString trPackageFunctions() override
    {
      return "Package Functies";
    }
    QCString trPackageMembers() override
    {
      return "Package Members";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    QCString trStaticPackageFunctions() override
    {
      return "Statische Package Functies";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    QCString trPackageAttribs() override
    {
      return "Package Attributen";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    QCString trStaticPackageAttribs() override
    {
      return "Statische Package Attributen";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    QCString trAll() override
    {
      return "Alle";
    }
    /*! Put in front of the call graph for a function. */
    QCString trCallGraph() override
    {
      return "Hier is de call graaf voor deze functie:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    QCString trSearchResultsTitle() override
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
    QCString trSearchResults(int numDocuments) override
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
    QCString trSearchMatches() override
    {
      return "Gevonden:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    QCString trSourceFile(const QCString& filename) override
    {
      return filename + " Bron Bestand";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    QCString trDirIndex() override
    { return "Folder Hiërarchie"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    QCString trDirDocumentation() override
    { return "Folder Documentatie"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    QCString trDirectories() override
    { return "Folders"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    QCString trDirReference(const QCString &dirName) override
    { QCString result=dirName; result+=" Folder Referentie"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    QCString trDir(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "folder", "s");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a function.
     */
    QCString trOverloadText() override
    {
      return "Deze functie is overloaded en is beschikbaar gemaakt om het "
             "gebruik te vergemakkelijken. Ze verschilt alleen van de "
             "bovenstaande functie wat betreft de parameterlijst.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! Put in front of the call graph for a function. */
    QCString trCallerGraph() override
    {
      return "Hier is de caller graaf voor deze functie:";
    }

    /*! header that is put before the list of member subprograms (Fortran). */
    QCString trMemberFunctionDocumentationFortran() override
    { return "Member Functie/Subroutine Documentatie"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    QCString trCompoundListFortran() override
    { return "Lijst met data types"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    QCString trCompoundMembersFortran() override
    { return "Data velden"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    QCString trCompoundListDescriptionFortran() override
    { return "Hieronder volgen de data types elk een korte beschrijving:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    QCString trCompoundMembersDescriptionFortran(bool extractAll) override
    {
      QCString result="Hieronder volgt de lijst met alle ";
      if (!extractAll) result+="gedocumenteerde ";
      result+="data types met links naar ";
      if (!extractAll) result+="de klasse documentatie voor elke member:";
      else result+="de klassen waartoe ze behoren:";
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index (Fortran).
     */
    QCString trCompoundIndexFortran() override
    { return "Data Type Index"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    QCString trTypeDocumentation() override
    { return "Data Type Documentatie"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    QCString trSubprograms() override
    { return "Functies/Subroutines"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    QCString trSubprogramDocumentation() override
    { return "Functie/Subroutine Documentatie"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     QCString trDataTypes() override
    { return "Data Types"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    QCString trModulesList() override
    { return "Module Lijst"; }

    /*! used as an introduction to the modules list (Fortran) */
    QCString trModulesListDescription(bool extractAll) override
    {
      QCString result="Hieronder volgt de lijst met alle ";
      if (!extractAll) result+="gedocumenteerde ";
      result+="modulen, elk met een korte beschrijving:";
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
    QCString trModuleReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" Module Referentie";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    QCString trModulesMembers() override
    { return "Module Members"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    QCString trModulesMemberDescription(bool extractAll) override
    {
      QCString result="Hier is een lijst van alle ";
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
    QCString trModulesIndex() override
    { return "Module Index"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trModule(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "module", "n");
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      QCString result="De documentatie voor ";
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
    QCString trType(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "type", "s");
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trSubprogram(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "subprogramma", "s");
    }

    /*! C# Type Contraint list */
    QCString trTypeConstraints() override
    {
      return "Type Beperkingen";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    QCString trDirRelation(const QCString &name) override
    {
      return name+" Relatie";
    }

    /*! Loading message shown when loading search results */
    QCString trLoading() override
    {
      return "Laden...";
    }

    /*! Label used for search results in the global namespace */
    QCString trGlobalNamespace() override
    {
      return "Globale Namespace";
    }

    /*! Message shown while searching */
    QCString trSearching() override
    {
      return "Zoeken...";
    }

    /*! Text shown when no search results are found */
    QCString trNoMatches() override
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
    QCString trFileIn(const QCString &name) override
    {
      return "Bestand in "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    QCString trIncludesFileIn(const QCString &name) override
    {
      return "Includeert bestand in "+name;
    }
    QCString trDateTime(int year,int month,int day,int dayOfWeek,
                                int hour,int minutes,int seconds,
                                DateTimeType includeTime) override
    {
      static const char *days[]   = { "Ma","Di","Wo","Do","Vr","Za","Zo" };
      static const char *months[] = { "Jan","Feb","Maa","Apr","Mei","Jun","Jul","Aug","Sep","Okt","Nov","Dec" };
      QCString sdate;
      if (includeTime == DateTimeType::DateTime || includeTime == DateTimeType::Date)
      {
        sdate.sprintf("%s %d %s %d",days[dayOfWeek-1],day,months[month-1],year);
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
    QCString trDayOfWeek(int dayOfWeek, bool first_capital, bool full) override
    {
      static const char *days_short[]   = { "ma", "di", "wo", "do", "vr", "za", "zo" };
      static const char *days_full[]    = { "maandag", "dinsdag", "woensdag", "donderdag", "vrijdag", "zaterdag", "zondag" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    QCString trMonth(int month, bool first_capital, bool full) override
    {
      static const char *months_short[] = { "jan", "feb", "mrt", "apr", "mei", "jun", "jul", "aug", "sep", "okt", "nov", "dec" };
      static const char *months_full[]  = { "januari", "februari", "maart", "april", "mei", "juni", "juli", "augustus", "september", "oktober", "november", "december" };
      QCString text  = full? months_full[month-1] : months_short[month-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    QCString trDayPeriod(bool period) override
    {
      static const char *dayPeriod[] = { "a.m.", "p.m." };
      return dayPeriod[period?1:0];
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.7.5
//////////////////////////////////////////////////////////////////////////

    /*! Header for the page with bibliographic citations */
    QCString trCiteReferences() override
    { return "Bibliografie"; }

    /*! Text for copyright paragraph */
    QCString trCopyright() override
    { return "Copyright"; }

    /*! Header for the graph showing the directory dependencies */
    QCString trDirDepGraph(const QCString &name) override
    { return "Folder afhankelijkheidsgraaf voor "+name+":"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    QCString trDetailLevel() override
    { return "detail niveau"; }

    /*! Section header for list of template parameters */
    QCString trTemplateParameters() override
    { return "Template Parameters"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    QCString trAndMore(const QCString &number) override
    { return "en "+number+ " anderen..."; }

    /*! Used file list for a Java enum */
    QCString trEnumGeneratedFromFiles(bool single) override
    { QCString result = "De documentatie voor deze enum is gegenereerd op basis van ";
      if (single) result+="het"; else result+="de";
      result+=" volgende bestand";
      if (single) result+=":"; else result+="en:";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    QCString trEnumReference(const QCString &name) override
    { return name+" Enum Referentie"; }

    /*! Used for a section containing inherited members */
    QCString trInheritedFrom(const QCString &members,const QCString &what) override
    { return members+" overgeërfd van "+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    QCString trAdditionalInheritedMembers() override
    { return "Additionele Overgeërfde Members"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    QCString trPanelSynchronisationTooltip(bool enable) override
    {
      QCString opt = enable ? "in" : "uit";
      return "klik hier om de paneel synchronisatie "+opt+" te schakelen";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    QCString trProvidedByCategory() override
    {
      return "Wordt aangeboden door category @0.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    QCString trExtendsClass() override
    {
      return "Uitbereiding van klasse @0.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    QCString trClassMethods() override
    {
      return "Klasse Methoden";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    QCString trInstanceMethods() override
    {
      return "Instantie Methoden";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    QCString trMethodDocumentation() override
    {
      return "Methode Documentatie";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    QCString trInterfaces() override
    { return "Geëporteerde Interfaces"; }

    /** old style UNO IDL services: inherited services */
    QCString trServices() override
    { return "Geïncludeerde Services"; }

    /** UNO IDL constant groups */
    QCString trConstantGroups() override
    { return "Konstanten Groepen"; }

    /** UNO IDL constant groups */
    QCString trConstantGroupReference(const QCString &namespaceName) override
    {
      QCString result=namespaceName;
      result+=" Konstanten Groepen Referentie";
      return result;
    }
    /** UNO IDL service page title */
    QCString trServiceReference(const QCString &sName) override
    {
      QCString result=sName;
      result+=" Service Referentie";
      return result;
    }
    /** UNO IDL singleton page title */
    QCString trSingletonReference(const QCString &sName) override
    {
      QCString result=sName;
      result+=" Singleton Referentie";
      return result;
    }
    /** UNO IDL service page */
    QCString trServiceGeneratedFromFiles(bool single) override
    { QCString result = "De documentatie voor deze service is gegenereerd op basis van ";
      if (single) result+="het"; else result+="de";
      result+=" volgende bestand";
      if (single) result+=":"; else result+="en:";
      return result;
    }
    /** UNO IDL singleton page */
    QCString trSingletonGeneratedFromFiles(bool single) override
    { QCString result = "De documentatie voor deze singleton is gegenereerd op basis van ";
      if (single) result+="het"; else result+="de";
      result+=" volgende bestand";
      if (single) result+=":"; else result+="en:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.15
//////////////////////////////////////////////////////////////////////////

    /** VHDL design unit hierarchy */
    QCString trDesignUnitHierarchy() override
    { return "Ontwerp Eenheid Hiërarchie"; }
    /** VHDL design unit list */
    QCString trDesignUnitList() override
    { return "Ontwerp Eenheid Lijst"; }
    /** VHDL design unit members */
    QCString trDesignUnitMembers() override
    { return "Ontwerp Eenheid Members"; }
    /** VHDL design unit list description */
    QCString trDesignUnitListDescription() override
    {
      return "hieronder volgt de lijst met all ontwerp eenheden met links "
             "naar de entiteiten waar ze bij behoren:";
    }
    /** VHDL design unit index */
    QCString trDesignUnitIndex() override
    { return "Ontwerp Eenheid Index"; }
    /** VHDL design units */
    QCString trDesignUnits() override
    { return "Ontwerp Eenheden"; }
    /** VHDL functions/procedures/processes */
    QCString trFunctionAndProc() override
    { return "Functies/Procedures/Processen"; }
    /** VHDL type */
    QCString trVhdlType(VhdlSpecifier type,bool single) override
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
    QCString trCustomReference(const QCString &name) override
    { return name+" Referentie"; }

    /* Slice */
    QCString trConstants() override
    { return "Konstanten"; }
    QCString trConstantDocumentation() override
    { return "Documentatie van konstanten"; }
    QCString trSequences() override
    { return "Reeksen"; }
    QCString trSequenceDocumentation() override
    { return "Documentatie van reeksen"; }
    QCString trDictionaries() override
    { return "Vertalingslijsten"; }
    QCString trDictionaryDocumentation() override
    { return "Documentatie van vertalingslijsten"; }
    QCString trSliceInterfaces() override
    { return "Interfaces"; }
    QCString trInterfaceIndex() override
    { return "Index van interfaces"; }
    QCString trInterfaceList() override
    { return "Lijst van interfaces"; }
    QCString trInterfaceListDescription() override
    { return "Hieronder volgt de lijst met alle interfaces, elk met een korte beschrijving:"; }
    QCString trInterfaceHierarchy() override
    { return "Interface Hiërarchie"; }
    QCString trInterfaceHierarchyDescription() override
    { return "Deze inheritance lijst is min of meer alfabetisch gesorteerd:"; }
    QCString trInterfaceDocumentation() override
    { return "Documentatie van interfaces"; }
    QCString trStructs() override
    { return "Structs"; }
    QCString trStructIndex() override
    { return "Index van struct"; }
    QCString trStructList() override
    { return "Lijst van struct"; }
    QCString trStructListDescription() override
    { return "Hieronder volgt de lijst met alle structs, elk met een korte beschrijving:"; }
    QCString trStructDocumentation() override
    { return "Documentatie van structs"; }
    QCString trExceptionIndex() override
    { return "Index van exceptions"; }
    QCString trExceptionList() override
    { return "Lijst van exceptions"; }
    QCString trExceptionListDescription() override
    { return "Hieronder volgt de lijst met alle exeptions, elk met een korte beschrijving:"; }
    QCString trExceptionHierarchy() override
    { return "Exception Hiërarchie"; }
    QCString trExceptionHierarchyDescription() override
    { return "Deze inheritance lijst is min of meer alfabetisch gesorteerd:"; }
    QCString trExceptionDocumentation() override
    { return "Documentatie van exceptions"; }
    QCString trCompoundReferenceSlice(const QCString &clName, ClassDef::CompoundType compType, bool isLocal) override
    {
      QCString result=clName;
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
    QCString trOperations() override
    { return "Bewerkingen"; }
    QCString trOperationDocumentation() override
    { return "Documentatie van bewerkingen"; }
    QCString trDataMembers() override
    { return "Data members"; }
    QCString trDataMemberDocumentation() override
    { return "Documentatie van data members"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.19
//////////////////////////////////////////////////////////////////////////
    QCString trDesignUnitDocumentation() override
    { return "Ontwerp Eenheid Documentatie"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.2
//////////////////////////////////////////////////////////////////////////
    QCString trConcept(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "concept", "en");
    }

    QCString trConceptReference(const QCString &conceptName) override
    {
      QCString result=conceptName;
      result+=" Concept Referentie";
      return result;
    }

    QCString trConceptList() override
    { return "Concept Lijst"; }

    QCString trConceptIndex() override
    { return "Concept Index"; }

    QCString trConceptDocumentation() override
    { return "Concept Documentatie"; }

    QCString trConceptListDescription(bool extractAll) override
    {
      QCString result="Hieronder volgt de lijst met alle ";
      if (!extractAll) result+="gedocumenteerde ";
      result+="concepten, elk met een korte beschrijving:";
      return result;
    }

    QCString trConceptDefinition() override
    {
      return "Concept definitie";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.4
//////////////////////////////////////////////////////////////////////////

    QCString trPackageList() override
    { return "Package Lijst"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.9.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used for translation of the word that will be
     *  followed by a single name of the VHDL process flowchart.
     */
    QCString trFlowchart() override
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
    QCString trRelatedSymbols() override
    { return "Gerelateerde symbolen"; }

    /*! subscript for the related symbols
     *
     *  Supersedes trRelatedSubscript
     */
    QCString trRelatedSymbolsSubscript() override
    { return "(Merk op dat dit geen member symbolen zijn.)"; }

    /*! used in the class documentation as a header before the list of all
     * related classes.
     *
     * Supersedes trRelatedFunctionDocumentation
     */
    QCString trRelatedSymbolDocumentation() override
    { return "Documentatie van friends en gerelateerde symbolen"; }

    QCString trCompoundType(ClassDef::CompoundType compType, SrcLangExt lang) override
    {
      QCString result;
      switch(compType)
      {
        case ClassDef::Class:
          if (lang == SrcLangExt::Fortran) trType(true,true);
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

    QCString trFileMembersDescriptionTotal(FileMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      QCString result="Hieronder volgt de lijst met alle ";
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

    QCString trCompoundMembersDescriptionTotal(ClassMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      QCString result="Hieronder volgt de lijst met alle ";
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

    QCString trNamespaceMembersDescriptionTotal(NamespaceMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      QCString result="Hier is een lijst van alle ";
      if (!extractAll) result+="gedocumenteerde ";
      result+="namespace ";
      QCString singularResult = "";
      QCString pluralResult = "";
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

    QCString trDefinition() override  { return "Definitie";}
    QCString trDeclaration() override { return "Declaratie";}

//////////////////////////////////////////////////////////////////////////
// new since 1.9.8
//////////////////////////////////////////////////////////////////////////

    QCString trTopics() override
    { return "Onderwerpen"; }
    QCString trTopicDocumentation() override
    { return "Documentatie voor dit onderwerp"; }
    QCString trTopicIndex() override
    { return "Overwerpen"; }
    QCString trTopicList() override
    { return "Lijst met onderwerpen"; }
    QCString trTopicListDescription() override
    { return "Hieronder volgt de lijst met alle onderwerpen, elk met een korte beschrijving:"; }
    QCString trModuleMembersDescriptionTotal(ModuleMemberHighlight::Enum hl) override
    {
      bool extractAll = Config_getBool(EXTRACT_ALL);
      QCString result="Hier is een lijst van alle ";
      if (!extractAll) result+="gedocumenteerde ";
      result+="module ";
      QCString singularResult = "";
      QCString pluralResult = "";
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
    QCString trExportedModules() override
    {
      return "Geëxporteerde Modules";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.10.0
//////////////////////////////////////////////////////////////////////////

    QCString trCopyToClipboard() override
    {
      return "Kopiëren naar het klembord";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.11.0
//////////////////////////////////////////////////////////////////////////

    QCString trImportant() override
    {
      return "Belangrijk";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.16.0
//////////////////////////////////////////////////////////////////////////

    // the title of the requirements overview page
    QCString trRequirements() override
    {
      return "Vereisten";
    }
    // table header for the column with the requirements IDs
    QCString trRequirementID() override
    {
      return "ID";
    }
    // indicates a symbol implements (satisfies) a requirement
    QCString trSatisfies(bool singular) override
    {
      return createNoun(true, singular, "Voldoet aan vereiste", "n");
    }
    // indicates a requirement is satisfied (implemented) by one or more symbols
    QCString trSatisfiedBy(const QCString &list) override
    {
      return "Wordt aan voldaan door "+list+".";
    }
    QCString trUnsatisfiedRequirements() override
    {
      return "Onvoldane Vereisten";
    }
    QCString trUnsatisfiedRequirementsText(bool singular,const QCString &list) override
    {
      return singular ?
        "De vereiste "+list+" mist de 'voldaan' relatie." :
        "De vereisten "+list+" missen de 'voldaan' relatie.";
    }
    // indicates a symbol verifies (tests) a requirement
    QCString trVerifies(bool singular) override
    {
      return createNoun(true, singular, "Verifieert vereiste", "n");
    }
    // indicates a requirement is verified (tested) by one or more symbols
    QCString trVerifiedBy(const QCString &list) override
    {
      return "Wordt geverifieerd door "+list+".";
    }
    QCString trUnverifiedRequirements() override
    {
      return "Ongeverifieerde vereisten";
    }
    QCString trUnverifiedRequirementsText(bool singular,const QCString &list) override
    {
      return singular ?
        "De vereiste "+list+" mist de 'verifieer' relatie." :
        "De vereisten "+list+" missen de 'verifieer' relatie.";
    }

};

#endif
