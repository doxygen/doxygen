/******************************************************************************
 *
 *
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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
    QCString idLanguage()
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
    QCString latexLanguageSupportCommand()
    {
      return "\\usepackage[dutch]{babel}\n";
    }
    QCString trRelatedFunctions()
    { return "Gerelateerde functies"; }
    QCString trRelatedSubscript()
    { return "(Merk op dat dit geen member functies zijn.)"; }
    QCString trDetailedDescription()
    { return "Gedetailleerde Beschrijving"; }
    QCString trMemberTypedefDocumentation()
    { return "Documentatie van type definitie members"; }
    QCString trMemberEnumerationDocumentation()
    { return "Documentatie van enumeratie members"; }
    QCString trMemberFunctionDocumentation()
    { return "Documentatie van functie members"; }
    QCString trMemberDataDocumentation()
    { return "Documentatie van data members"; }
    QCString trMore()
    { return "Meer..."; }
    QCString trListOfAllMembers()
    { return "Lijst van alle members"; }
    QCString trMemberList()
    { return "Member Lijst"; }
    QCString trThisIsTheListOfAllMembers()
    { return "Dit is de complete lijst van alle members voor"; }
    QCString trIncludingInheritedMembers()
    { return ", inclusief alle overge&euml;rfde members."; }
    QCString trGeneratedAutomatically(const char *s)
    { QCString result="Automatisch gegenereerd door Doxygen";
      if (s) result+=(QCString)" voor "+s;
      result+=" uit de programmatekst.";
      return result;
    }
    QCString trEnumName()
    { return "enum naam"; }
    QCString trEnumValue()
    { return "enum waarde"; }
    QCString trDefinedIn()
    { return "gedefinieerd in"; }
    QCString trModules()
    { return "Modules"; }
    QCString trClassHierarchy()
    { return "Klasse Hi&euml;rarchie"; }
    QCString trCompoundList()
    { return "Klasse Lijst"; }
    QCString trFileList()
    { return "Bestandslijst"; }
    QCString trCompoundMembers()
    { return "Klasse Members"; }
    QCString trFileMembers()
    { return "Bestand members"; }
    QCString trRelatedPages()
    { return "Gerelateerde pagina's"; }
    QCString trExamples()
    { return "Voorbeelden"; }
    QCString trSearch()
    { return "Zoeken"; }
    QCString trClassHierarchyDescription()
    { return "Deze inheritance lijst is min of meer alfabetisch "
             "gesorteerd:";
    }
    QCString trFileListDescription(bool extractAll)
    {
      QCString result="Hieronder volgt de lijst met alle ";
      if (!extractAll) result+="gedocumenteerde ";
      result+="bestanden, elk met een korte beschrijving:";
      return result;
    }
    QCString trCompoundListDescription()
    { return "Hieronder volgen de klassen, structs en "
             "unions met voor elk een korte beschrijving:";
    }
    QCString trCompoundMembersDescription(bool extractAll)
    {
      QCString result="Hieronder volgt de lijst met alle ";
      if (!extractAll) result+="gedocumenteerde ";
      result+="klasse members met links naar ";
      if (!extractAll) result+="de klasse documentatie voor elke member:";
      else result+="de klassen waartoe ze behoren:";
      return result;
    }
    QCString trFileMembersDescription(bool extractAll)
    {
      QCString result="Hieronder volgt de lijst met alle ";
      if (!extractAll) result+="gedocumenteerde ";
      result+="bestand members met links naar ";
      if (extractAll) result+="de bestand's documentatie voor elke member:";
      else result+="de bestanden waartoe ze behoren:";
      return result;
    }
    QCString trExamplesDescription()
    { return "Hieronder volgt de lijst met alle voorbeelden:"; }
    QCString trRelatedPagesDescription()
    { return "Hieronder volgt de lijst met alle pagina's die gerelateerde documentatie bevatten:"; }
    QCString trModulesDescription()
    { return "Hieronder volgt de lijst met alle modules:"; }

    QCString trDocumentation()
    { return "Documentatie"; }
    QCString trModuleIndex()
    { return "Module Index"; }
    QCString trHierarchicalIndex()
    { return "Hi&euml;rarchische Index"; }
    QCString trCompoundIndex()
    { return "Klasse Index"; }
    QCString trFileIndex()
    { return "Bestand Index"; }
    QCString trModuleDocumentation()
    { return "Module Documentatie"; }
    QCString trClassDocumentation()
    { return "Klassen Documentatie"; }
    QCString trFileDocumentation()
    { return "Bestand Documentatie"; }
    QCString trExampleDocumentation()
    { return "Documentatie van voorbeelden"; }
    QCString trPageDocumentation()
    { return "Documentatie van gerelateerde pagina's"; }
    QCString trReferenceManual()
    { return "Naslagwerk"; }

    QCString trDefines()
    { return "Macros"; }
    QCString trTypedefs()
    { return "Typedefs"; }
    QCString trEnumerations()
    { return "Enumeraties"; }
    QCString trFunctions()
    { return "Functies"; }
    QCString trVariables()
    { return "Variabelen"; }
    QCString trEnumerationValues()
    { return "Enumeratie waarden"; }
    QCString trDefineDocumentation()
    { return "Documentatie van macro's"; }
    QCString trTypedefDocumentation()
    { return "Documentatie van typedefs"; }
    QCString trEnumerationTypeDocumentation()
    { return "Documentatie van enumeratie types"; }
    QCString trEnumerationValueDocumentation()
    { return "Documentatie van enumeratie waarden"; }
    QCString trFunctionDocumentation()
    { return "Documentatie van functies"; }
    QCString trVariableDocumentation()
    { return "Documentatie van variabelen"; }
    QCString trCompounds()
    { return "Klassen"; }
    QCString trGeneratedAt(const char *date,const char *projName)
    {
      QCString result=(QCString)"Gegenereerd op "+date;
      if (projName) result+=(QCString)" voor "+projName;
      result+=(QCString)" door";
      return result;
    }
    QCString trClassDiagram(const char *clName)
    {
      return (QCString)"Klasse diagram voor "+clName;
    }
    QCString trForInternalUseOnly()
    { return "Alleen voor intern gebruik."; }
    QCString trWarning()
    { return "Waarschuwing"; }
    QCString trVersion()
    { return "Versie"; }
    QCString trDate()
    { return "Datum"; }
    QCString trReturns()
    { return "Retourneert"; }
    QCString trSeeAlso()
    { return "Zie ook"; }
    QCString trParameters()
    { return "Parameters"; }
    QCString trExceptions()
    { return "Excepties"; }
    QCString trGeneratedBy()
    { return "Gegenereerd door"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    QCString trNamespaceList()
    { return "Namespace Lijst"; }
    QCString trNamespaceListDescription(bool extractAll)
    {
      QCString result="Hier is een lijst met alle ";
      if (!extractAll) result+="gedocumenteerde ";
      result+="namespaces met voor elk een korte beschrijving:";
      return result;
    }
    QCString trFriends()
    { return "Friends"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    QCString trRelatedFunctionDocumentation()
    { return "Documentatie van friends en gerelateerde functies"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    QCString trCompoundReference(const char *clName,
                                 ClassDef::CompoundType compType,
                                 bool isTemplate)
      // used as the title of the HTML page of a class/struct/union
    {
      QCString result=(QCString)clName;
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
    QCString trFileReference(const char *fileName)
      // used as the title of the HTML page of a file
    {
      QCString result=fileName;
      result+=" Bestand Referentie";
      return result;
    }
    QCString trNamespaceReference(const char *namespaceName)
      // used as the title of the HTML page of a namespace
    {
      QCString result=namespaceName;
      result+=" Namespace Referentie";
      return result;
    }

    // these are for the member sections of a class, struct or union
    QCString trPublicMembers()
    { return "Public Members"; }
    QCString trPublicSlots()
    { return "Public Slots"; }
    QCString trSignals()
    { return "Signals"; }
    QCString trStaticPublicMembers()
    { return "Static Public Members"; }
    QCString trProtectedMembers()
    { return "Protected Members"; }
    QCString trProtectedSlots()
    { return "Protected Slots"; }
    QCString trStaticProtectedMembers()
    { return "Static Protected Members"; }
    QCString trPrivateMembers()
    { return "Private Members"; }
    QCString trPrivateSlots()
    { return "Private Slots"; }
    QCString trStaticPrivateMembers()
    { return "Static Private Members"; }
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
            result+=" en ";
        }
      }
      return result;
    }

    QCString trInheritsList(int numEntries)
      // used in class documentation to produce a list of base classes,
      // if class diagrams are disabled.
    {
      return "Erft over van "+trWriteList(numEntries)+".";
    }
    QCString trInheritedByList(int numEntries)
      // used in class documentation to produce a list of super classes,
      // if class diagrams are disabled.
    {
      return "Wordt overge&euml;rfd door "+trWriteList(numEntries)+".";
    }
    QCString trReimplementedFromList(int numEntries)
      // used in member documentation blocks to produce a list of
      // members that are hidden by this one.
    {
      return "Nieuwe implementatie van "+trWriteList(numEntries)+".";
    }
    QCString trReimplementedInList(int numEntries)
    {
      // used in member documentation blocks to produce a list of
      // all member that overwrite the implementation of this member.
      return "Opnieuw ge&iuml;mplementeerd in "+trWriteList(numEntries)+".";
    }

    QCString trNamespaceMembers()
      // This is put above each page as a link to all members of namespaces.
    { return "Namespace Members"; }
    QCString trNamespaceMemberDescription(bool extractAll)
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
    QCString trNamespaceIndex()
      // This is used in LaTeX as the title of the chapter with the
      // index of all namespaces.
    { return "Namespace Index"; }
    QCString trNamespaceDocumentation()
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all namespaces.
    { return "Namespace Documentatie"; }

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
      QCString result=(QCString)"De documentatie voor ";
      switch(compType)
      {
        case ClassDef::Class:      result+="deze klasse"; break;
        case ClassDef::Struct:     result+="deze struct"; break;
        case ClassDef::Union:      result+="deze union"; break;
        case ClassDef::Interface:  result+="dit interface"; break;
        case ClassDef::Protocol:   result+="dit protocol"; break;
        case ClassDef::Category:   result+="deze categorie"; break;
        case ClassDef::Exception:  result+="deze exceptie"; break;
        default: break;
      }
      result+=" is gegenereerd op grond van ";
      if (single) result+="het"; else result+="de";
      result+=" volgende bestand";
      if (single) result+=":"; else result+="en:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    QCString trReturnValues()
    { return "Retour waarden"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage()
    { return "Hoofd Pagina"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    QCString trPageAbbreviation()
    { return "p."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991106
//////////////////////////////////////////////////////////////////////////

    QCString trDefinedAtLineInSourceFile()
    {
      return "De definitie bevindt zich op regel @0 in het bestand @1.";
    }
    QCString trDefinedInSourceFile()
    {
      return "De definitie bevindt zich in het bestand @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated()
    {
      return "Verouderd";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const char *clName)
    {
      return (QCString)"Collaboratie diagram voor "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const char *fName)
    {
      return (QCString)"Include afhankelijkheidsgraaf voor "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation()
    {
      return "Constructor & Destructor Documentatie";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode()
    {
      return "Ga naar de broncode van dit bestand.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation()
    {
      return "Ga naar de documentatie van dit bestand.";
    }
    /*! Text for the \\pre command */
    QCString trPrecondition()
    {
      return "Preconditie";
    }
    /*! Text for the \\post command */
    QCString trPostcondition()
    {
      return "Postconditie";
    }
    /*! Text for the \\invariant command */
    QCString trInvariant()
    {
      return "Invariant";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue()
    {
      return "Initi&euml;le waarde:";
    }
    /*! Text used the source code in the file index */
    QCString trCode()
    {
      return "code";
    }
    QCString trGraphicalHierarchy()
    {
      return "Grafische Klasse Hi&euml;rarchie";
    }
    QCString trGotoGraphicalHierarchy()
    {
      return "Ga naar de grafische klasse hi&euml;rarchie";
    }
    QCString trGotoTextualHierarchy()
    {
      return "Ga naar de tekstuele klasse hi&euml;rarchie";
    }
    QCString trPageIndex()
    {
      return "Pagina Index";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    QCString trNote()
    {
      return "Noot";
    }
    QCString trPublicTypes()
    {
      return "Public Typen";
    }
    QCString trPublicAttribs()
    {
      return "Public Attributen";
    }
    QCString trStaticPublicAttribs()
    {
      return "Static Public Attributen";
    }
    QCString trProtectedTypes()
    {
      return "Protected Typen";
    }
    QCString trProtectedAttribs()
    {
      return "Protected Attributen";
    }
    QCString trStaticProtectedAttribs()
    {
      return "Static Protected Attributen";
    }
    QCString trPrivateTypes()
    {
      return "Private Typen";
    }
    QCString trPrivateAttribs()
    {
      return "Private Attributen";
    }
    QCString trStaticPrivateAttribs()
    {
      return "Static Private Attributen";
    }


//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a todo item */
    QCString trTodo()
    {
      return "Todo";
    }
    /*! Used as the header of the todo list */
    QCString trTodoList()
    {
      return "Todo Lijst";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy()
    {
      return "Wordt aangeroepen door";
    }
    QCString trRemarks()
    {
      return "Opmerkingen";
    }
    QCString trAttention()
    {
      return "Attentie";
    }
    QCString trInclByDepGraph()
    {
      return "Deze graaf geeft aan welke bestanden direct of "
             "indirect afhankelijk zijn van dit bestand:";
    }
    QCString trSince()
    {
      return "Sinds";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    QCString trLegendTitle()
    {
      return "Graaf Legenda";
    }
    /*! page explaining how the dot graph's should be interpreted */
    QCString trLegendDocs()
    {
      return
        "Deze pagina legt uit hoe de grafen die gegenereerd worden door doxygen "
        "ge&iuml;nterpreteerd dienen te worden.<p>\n"
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
        "<p><center><img src=\"graph_legend."+Config_getEnum("DOT_IMAGE_FORMAT")+"\"></center>\n"
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
        "<li>Een donkerblauwe pijl visualizeert een public inheritance "
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
    QCString trLegend()
    {
      return "legenda";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    QCString trTest()
    {
      return "Test";
    }
    /*! Used as the header of the test list */
    QCString trTestList()
    {
      return "Test Lijst";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    QCString trProperties()
    {
      return "Properties";
    }
    /*! Used as a section header for IDL property documentation */
    QCString trPropertyDocumentation()
    {
      return "Property Documentatie";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    QCString trClasses()
    {
      return "Klassen";
    }
    /*! Used as the title of a Java package */
    QCString trPackage(const char *name)
    {
      return (QCString)"Package "+name;
    }
    /*! Title of the package index page */
    QCString trPackageList()
    {
      return "Package Lijst";
    }
    /*! The description of the package index page */
    QCString trPackageListDescription()
    {
      return "Hier volgen de packages, elk met een korte beschrijving (indien aanwezig):";
    }
    /*! The link name in the Quick links header for each page */
    QCString trPackages()
    {
      return "Packages";
    }
    /*! Text shown before a multi-line define */
    QCString trDefineValue()
    {
      return "Waarde:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    QCString trBug()
    {
      return "Bug";
    }
    /*! Used as the header of the bug list */
    QCString trBugList()
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
    virtual QCString trRTFansicp()
    {
      return "1252";
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
      return "Index";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trClass(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Klasse" : "klass"));
      if (!singular)  result+="n";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trFile(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Bestand" : "bestand"));
      if (!singular)  result+="en";
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
      QCString result((first_capital ? "Groep" : "groep"));
      if (!singular)  result+="en";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trPage(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Pagina" : "pagina"));
      if (!singular)  result+="s";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trMember(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Member" : "member"));
      if (!singular)  result+="s";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trGlobal(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Globale member" : "globale member"));
      if (!singular)  result+="s";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.7
//////////////////////////////////////////////////////////////////////////

    /*! This text is generated when the \\author command is used and
     *  for the author section in man pages. */
    virtual QCString trAuthor(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Auteur" : "auteur"));
      if (!singular)  result+="s";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    virtual QCString trReferences()
    {
      return "Gebruikt";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    virtual QCString trImplementedFromList(int numEntries)
    {
      return "Implementeert "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implementation this member.
     */
    virtual QCString trImplementedInList(int numEntries)
    {
      return "Wordt ge&iuml;mplementeerd door "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    virtual QCString trRTFTableOfContents()
    { return "Inhoudsopgave"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    virtual QCString trDeprecatedList()
    {
      return "Deprecated Lijst";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for a section of events found in a C# program
     */
    virtual QCString trEvents()
    {
      return "Events";
    }
    /*! Header used for the documentation section of a class' events. */
    virtual QCString trEventDocumentation()
    {
      return "Event Documentatie";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    virtual QCString trPackageTypes()
    {
      return "Package Types";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    virtual QCString trPackageMembers()
    {
      return "Package Functies";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    virtual QCString trStaticPackageMembers()
    {
      return "Statische Package Functies";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    virtual QCString trPackageAttribs()
    {
      return "Package Attributen";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    virtual QCString trStaticPackageAttribs()
    {
      return "Statische Package Attributen";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    virtual QCString trAll()
    {
      return "Alle";
    }
    /*! Put in front of the call graph for a function. */
    virtual QCString trCallGraph()
    {
      return "Hier is de call graaf voor deze functie:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    virtual QCString trSearchResultsTitle()
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
    virtual QCString trSearchResults(int numDocuments)
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
    virtual QCString trSearchMatches()
    {
      return "Gevonden:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    virtual QCString trSourceFile(QCString& filename)
    {
      return filename + " Bron Bestand";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    virtual QCString trDirIndex()
    { return "Folder Hi&euml;rarchie"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    virtual QCString trDirDocumentation()
    { return "Folder Documentatie"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    virtual QCString trDirectories()
    { return "Folders"; }

    /*! This returns a sentences that introduces the directory hierarchy.
     *  and the fact that it is sorted alphabetically per level
     */
    virtual QCString trDirDescription()
    { return "Deze folder hi&euml;rarchie is min of meer alfabetisch "
             "gesorteerd:";
    }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    virtual QCString trDirReference(const char *dirName)
    { QCString result=dirName; result+=" Folder Referentie"; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    virtual QCString trDir(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Folder" : "folder"));
      if (!singular) result+="s";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a function.
     */
    virtual QCString trOverloadText()
    {
      return "Deze functie is overloaded en is beschikbaar gemaakt om het "
             "gebruik te vergemakkelijken. Ze verschilt alleen van de "
             "bovenstaande functie wat betreft de parameterlijst.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! Put in front of the call graph for a function. */
    virtual QCString trCallerGraph()
    {
      return "Hier is de caller graaf voor deze functie:";
    }

    /*! header that is put before the list of member subprograms (Fortran). */
    virtual QCString trMemberFunctionDocumentationFortran()
    { return "Member Functie/Subroutine Documentatie"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    virtual QCString trCompoundListFortran()
    { return "Lijst met data types"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    virtual QCString trCompoundMembersFortran()
    { return "Data velden"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    virtual QCString trCompoundListDescriptionFortran()
    { return "Hieronder volgen de data types elk een korte beschrijving:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    virtual QCString trCompoundMembersDescriptionFortran(bool extractAll)
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
    virtual QCString trCompoundIndexFortran()
    { return "Data Type Index"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    virtual QCString trTypeDocumentation()
    { return "Data Type Documentatie"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    virtual QCString trSubprograms()
    { return "Functies/Subroutines"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    virtual QCString trSubprogramDocumentation()
    { return "Functie/Subroutine Documentatie"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     virtual QCString trDataTypes()
    { return "Data Types"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    virtual QCString trModulesList()
    { return "Module Lijst"; }

    /*! used as an introduction to the modules list (Fortran) */
    virtual QCString trModulesListDescription(bool extractAll)
    {
      QCString result="Hieronder volgt de lijst met alle ";
      if (!extractAll) result+="gedocumenteerde ";
      result+="modulen, elk met een korte beschrijving:";
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
    virtual QCString trModuleReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" Module Referentie";
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    virtual QCString trModulesMembers()
    { return "Module Members"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    virtual QCString trModulesMemberDescription(bool extractAll)
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
    virtual QCString trModulesIndex()
    { return "Module Index"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trModule(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Module" : "module"));
      if (!singular)  result+="n";
      return result;
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    virtual QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single)
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      QCString result=(QCString)"De documentatie voor ";
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
      result+=" is gegenereerd op grond van ";
      if (single) result+="het"; else result+="de";
      result+=" volgende bestand";
      if (single) result+=":"; else result+="en:";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trType(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Type" : "type"));
      if (!singular)  result+="s";
      return result;
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    virtual QCString trSubprogram(bool first_capital, bool singular)
    {
      QCString result((first_capital ? "Subprogramma" : "subprogramma"));
      if (!singular)  result+="s";
      return result;
    }

    /*! C# Type Contraint list */
    virtual QCString trTypeConstraints()
    {
      return "Type Beperkingen";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    virtual QCString trDirRelation(const char *name)
    {
      return QCString(name)+" Relatie";
    }

    /*! Loading message shown when loading search results */
    virtual QCString trLoading()
    {
      return "Laden...";
    }

    /*! Label used for search results in the global namespace */
    virtual QCString trGlobalNamespace()
    {
      return "Globale Namespace";
    }

    /*! Message shown while searching */
    virtual QCString trSearching()
    {
      return "Zoeken...";
    }

    /*! Text shown when no search results are found */
    virtual QCString trNoMatches()
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
    virtual QCString trFileIn(const char *name)
    {
      return (QCString)"Bestand in "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    virtual QCString trIncludesFileIn(const char *name)
    {
      return (QCString)"Includeert bestand in "+name;
    }
    virtual QCString trDateTime(int year,int month,int day,int dayOfWeek,
                                int hour,int minutes,int seconds,
                                bool includeTime)
    {
      static const char *days[]   = { "Ma","Di","Wo","Do","Vr","Za","Zo" };
      static const char *months[] = { "Jan","Feb","Maa","Apr","Mei","Jun","Jul","Aug","Sep","Okt","Nov","Dec" };
      QCString sdate;
      sdate.sprintf("%s %d %s %d",days[dayOfWeek-1],day,months[month-1],year);
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
    { return "Bibliografie"; }

    /*! Text for copyright paragraph */
    virtual QCString trCopyright()
    { return "Copyright"; }

    /*! Header for the graph showing the directory dependencies */
    virtual QCString trDirDepGraph(const char *name)
    { return QCString("Folder afhankelijkheidsgraaf voor ")+name+":"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.0
//////////////////////////////////////////////////////////////////////////

    /*! Detail level selector shown for hierarchical indices */
    virtual QCString trDetailLevel()
    { return "detail niveau"; }

    /*! Section header for list of template parameters */
    virtual QCString trTemplateParameters()
    { return "Template Parameters"; }

    /*! Used in dot graph when UML_LOOK is enabled and there are many fields */
    virtual QCString trAndMore(const QCString &number)
    { return "en "+number+ " anderen..."; }

    /*! Used file list for a Java enum */
    virtual QCString trEnumGeneratedFromFiles(bool single)
    { QCString result = "De documentatie voor deze enum is gegenereerd op grond van ";
      if (single) result+="het"; else result+="de";
      result+=" volgende bestand";
      if (single) result+=":"; else result+="en:";
      return result;
    }

    /*! Header of a Java enum page (Java enums are represented as classes). */
    virtual QCString trEnumReference(const char *name)
    { return QCString(name)+" Enum Referentie"; }

    /*! Used for a section containing inherited members */
    virtual QCString trInheritedFrom(const char *members,const char *what)
    { return QCString(members)+" overge&euml;rfd van "+what; }

    /*! Header of the sections with inherited members specific for the
     *  base class(es)
     */
    virtual QCString trAdditionalInheritedMembers()
    { return "Additionele Overge&euml;rfde Members"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a tooltip for the toggle button that appears in the
     *  navigation tree in the HTML output when GENERATE_TREEVIEW is
     *  enabled. This tooltip explains the meaning of the button.
     */
    virtual QCString trPanelSynchronisationTooltip(bool enable)
    {
      QCString opt = enable ? "in" : "uit";
      return "klik hier de paneel synchronisatie "+opt+" the schakelen";
    }

    /*! Used in a method of an Objective-C class that is declared in a
     *  a category. Note that the @1 marker is required and is replaced
     *  by a link.
     */
    virtual QCString trProvidedByCategory()
    {
      return "Wordt aangeboden door category @1.";
    }

    /*! Used in a method of an Objective-C category that extends a class.
     *  Note that the @1 marker is required and is replaced by a link to
     *  the class method.
     */
    virtual QCString trExtendsClass()
    {
      return "Uitbereiding van klasse @1.";
    }

    /*! Used as the header of a list of class methods in Objective-C.
     *  These are similar to static public member functions in C++.
     */
    virtual QCString trClassMethods()
    {
      return "Klasse Methoden";
    }

    /*! Used as the header of a list of instance methods in Objective-C.
     *  These are similar to public member functions in C++.
     */
    virtual QCString trInstanceMethods()
    {
      return "Instantie Methoden";
    }

    /*! Used as the header of the member functions of an Objective-C class.
     */
    virtual QCString trMethodDocumentation()
    {
      return "Methode Documentatie";
    }

    /*! Used as the title of the design overview picture created for the
     *  VHDL output.
     */
    virtual QCString trDesignOverview()
    {
      return "Ontwerp Overzicht";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.8.4
//////////////////////////////////////////////////////////////////////////

    /** old style UNO IDL services: implemented interfaces */
    virtual QCString trInterfaces()
    { return "Ge&euml;porteerde Interfaces"; }

    /** old style UNO IDL services: inherited services */
    virtual QCString trServices()
    { return "Ge&iuml;ncludeerde Services"; }

    /** UNO IDL constant groups */
    virtual QCString trConstantGroups()
    { return "Konstanten Groepen"; }

    /** UNO IDL constant groups */
    virtual QCString trConstantGroupReference(const char *namespaceName)
    {
      QCString result=namespaceName;
      result+=" Konstanten Groepen Referentie";
      return result;
    }
    /** UNO IDL service page title */
    virtual QCString trServiceReference(const char *sName)
    {
      QCString result=(QCString)sName;
      result+=" Service Referentie";
      return result;
    }
    /** UNO IDL singleton page title */
    virtual QCString trSingletonReference(const char *sName)
    {
      QCString result=(QCString)sName;
      result+=" Singleton Referentie";
      return result;
    }
    /** UNO IDL service page */
    virtual QCString trServiceGeneratedFromFiles(bool single)
    { QCString result = "De documentatie voor deze service is gegenereerd op grond van ";
      if (single) result+="het"; else result+="de";
      result+=" volgende bestand";
      if (single) result+=":"; else result+="en:";
      return result;
    }
    /** UNO IDL singleton page */
    virtual QCString trSingletonGeneratedFromFiles(bool single)
    { QCString result = "De documentatie voor deze singleton is gegenereerd op grond van ";
      if (single) result+="het"; else result+="de";
      result+=" volgende bestand";
      if (single) result+=":"; else result+="en:";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
};

#endif
