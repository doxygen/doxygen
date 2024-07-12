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

#ifndef TRANSLATOR_CA_H
#define TRANSLATOR_CA_H

/*!
 When defining a translator class for the new language, follow
 the description in the documentation.  One of the steps says
 that you should copy the translator_en.h (this) file to your
 translator_xx.h new file.  Your new language should use the
 Translator class as the base class.  This means that you need to
 implement exactly the same (pure virtual) override methods as the
 TranslatorEnglish does.  Because of this, it is a good idea to
 start with the copy of TranslatorEnglish and replace the strings
 one by one.

 It is not necessary to include "translator.h" or
 "translator_adapter.h" here.  The files are included in the
 language.cpp correctly.  Not including any of the mentioned
 files frees the maintainer from thinking about whether the
 first, the second, or both files should be included or not, and
 why.  This holds namely for localized translators because their
 base class is changed occasionally to adapter classes when the
 Translator class changes the interface, or back to the
 Translator class (by the local maintainer) when the localized
 translator is made up-to-date again.
*/
class TranslatorCatalan : public TranslatorAdapter_1_8_0
{
  public:

    // --- Language control methods -------------------

    /*! Used for identification of the language. The identification
     * should not be translated. It should be replaced by the name
     * of the language in English using lower-case characters only
     * (e.g. "czech", "japanese", "russian", etc.). It should be equal to
     * the identification used in language.cpp.
     */
    QCString idLanguage() override
    { return "catalan"; }

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
     *
     * The English LaTeX does not use such commands.  Because of this
     * the empty string is returned in this implementation.
     */
    QCString latexLanguageSupportCommand() override
    {
      //return "\\usepackage[catalan]{babel}\n\\usepackage[latin1]{inputenc}";
      return "\\usepackage[catalan]{babel}\n";
    }

    QCString trISOLang() override
    {
      return "ca";
    }
    QCString getLanguageString() override
    {
      return "0x403 Catalan";
    }

    // --- Language translation methods -------------------

    /*! used in the compound documentation before a list of related functions. */
    QCString trRelatedFunctions() override
    { return "Funcions Associades"; }

    /*! subscript for the related functions. */
    QCString trRelatedSubscript() override
    { return "(Remarcar que aquestes funcions no són funcions membre.)"; }

    /*! header that is put before the detailed description of files, classes and namespaces. */
    QCString trDetailedDescription() override
    { return "Descripció Detallada"; }

    /*! header that is used when the summary tag is missing inside the details tag */
    QCString trDetails() override
    { return "Detalls"; }

    /*! header that is put before the list of typedefs. */
    QCString trMemberTypedefDocumentation() override
    { return "Documentació de les Definicions de Tipus Membre"; }

    /*! header that is put before the list of enumerations. */
    QCString trMemberEnumerationDocumentation() override
    { return "Documentació de les Enumeracions Membre"; }

    /*! header that is put before the list of member functions. */
    QCString trMemberFunctionDocumentation() override
    { return "Documentació de les Funcions Membre"; }

    /*! header that is put before the list of member attributes. */
    QCString trMemberDataDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Documentació dels Camps";
      }
      else
      {
        return "Documentació de les Dades Membre";
      }
    }

    /*! this is the text of a link put after brief descriptions. */
    QCString trMore() override
    { return "Més..."; }

    /*! put in the class documentation */
    QCString trListOfAllMembers() override
    { return "Llista de tots els membres"; }

    /*! used as the title of the "list of all members" page of a class */
    QCString trMemberList() override
    { return "Llista dels Membres"; }

    /*! this is the first part of a sentence that is followed by a class name */
    QCString trThisIsTheListOfAllMembers() override
    { return "Aquesta és la llista complerta dels membres de"; }

    /*! this is the remainder of the sentence after the class name */
    QCString trIncludingInheritedMembers() override
    { return ", incloent tots els membres heretats."; }

    /*! this is put at the author sections at the bottom of man pages.
     *  parameter s is name of the project name.
     */
    QCString trGeneratedAutomatically(const QCString &s) override
    { QCString result="Generat automàticament per Doxygen";
      if (!s.isEmpty()) result+=" per a "+s;
      result+=" a partir del codi font.";
      return result;
    }

    /*! put after an enum name in the list of all members */
    QCString trEnumName() override
    { return "nom de la enum"; }

    /*! put after an enum value in the list of all members */
    QCString trEnumValue() override
    { return "valors de la enum"; }

    /*! put after an undocumented member in the list of all members */
    QCString trDefinedIn() override
    { return "definit a"; }

    // quick reference sections

    /*! This is put above each page as a link to the list of all groups of
     *  compounds or files (see the \\group command).
     */
    QCString trModules() override
    { return "Mòduls"; }

    /*! This is put above each page as a link to the class hierarchy */
    QCString trClassHierarchy() override
    { return "Jerarquia de Classes"; }

    /*! This is put above each page as a link to the list of annotated classes */
    QCString trCompoundList() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Estructures de Dades";
      }
      else
      {
        return "Llista de Classes";
      }
    }

    /*! This is put above each page as a link to the list of documented files */
    QCString trFileList() override
    { return "Llista dels Fitxers"; }

    /*! This is put above each page as a link to all members of compounds. */
    QCString trCompoundMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Camps de Dades";
      }
      else
      {
        return "Membres de Classes";
      }
    }

    /*! This is put above each page as a link to all members of files. */
    QCString trFileMembers() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Globals";
      }
      else
      {
        return "Membres de Fitxers";
      }
    }

    /*! This is put above each page as a link to all related pages. */
    QCString trRelatedPages() override
    { return "Pàgines Relacionades"; }

    /*! This is put above each page as a link to all examples. */
    QCString trExamples() override
    { return "Exemples"; }

    /*! This is put above each page as a link to the search engine. */
    QCString trSearch() override
    { return "Cerca"; }

    /*! This is an introduction to the class hierarchy. */
    QCString trClassHierarchyDescription() override
    { return "Aquesta llista d'herència està ordenada toscament, "
             "però no completa, de forma alfabètica:";
    }

    /*! This is an introduction to the list with all files. */
    QCString trFileListDescription(bool extractAll) override
    {
      QCString result="Aquesta és la llista de tots els fitxers ";
      if (!extractAll) result+="documentats ";
      result+="acompanyats amb breus descripcions:";
      return result;
    }

    /*! This is an introduction to the annotated compound list. */
    QCString trCompoundListDescription() override
    {

      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Aquestes són les estructures de dades acompanyades amb breus descripcions:";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_SLICE))
      {
        return "Aquestes són les classes acompanyades amb breus descripcions:";
      }
      else
      {
        return "Aquestes són les classes, estructures, "
               "unions i interfícies acompanyades amb breus descripcions:";
      }
    }

    /*! This is an introduction to the page with all class members. */
    QCString trCompoundMembersDescription(bool extractAll) override
    {
      QCString result="Aquesta és la llista de tots els ";
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="camps d'estructures i unions";
      }
      else
      {
        result+="membres de classe";
      }
      if (!extractAll)
      {
        result+=" documentats";
      }
      result+=" amb enllaços a ";
      if (!extractAll)
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="la documentació de l'estructura/unió per a cada camp:";
        }
        else
        {
          result+="la documentació de la classe per a cada membre:";
        }
      }
      else
      {
        if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
        {
          result+="les estructures/unions a que pertanyen:";
        }
        else
        {
          result+="les classes a que pertanyen:";
        }
      }
      return result;
    }
    /*! This is an introduction to the page with all file members. */
    QCString trFileMembersDescription(bool extractAll) override
    {
      QCString result="Aquesta és la llista de ";
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        result+="totes les funcions, variables, definicions, enumeracions, i definicions de tipus";
        if (!extractAll) result+=" documentades";
      }
      else
      {
        result+="tots els membres de fitxers";
        if (!extractAll) result+=" documentats";
      }
      result+=" amb enllaços ";
      if (extractAll)
        result+="als fitxers als quals corresponen:";
      else
        result+="a la documentació:";
      return result;
    }

    /*! This is an introduction to the page with the list of all examples */
    QCString trExamplesDescription() override
    { return "Aquesta és la llista de tots els exemples:"; }

    /*! This is an introduction to the page with the list of related pages */
    QCString trRelatedPagesDescription() override
    { return "Aquesta és la llista de totes les pàgines de documentació associades:"; }

    /*! This is an introduction to the page with the list of class/file groups */
    QCString trModulesDescription() override
    { return "Aquesta és la llista de mòduls:"; }

    // index titles (the project name is prepended for these)


    /*! This is used in HTML as the title of index.html. */
    QCString trDocumentation(const QCString &projName) override
    { return (!projName.isEmpty()?projName + " " : "") + ": Documentació"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * index of all groups.
     */
    QCString trModuleIndex() override
    { return "Índex de Mòduls"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * class hierarchy.
     */
    QCString trHierarchicalIndex() override
    { return "Índex Jeràrquic"; }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index.
     */
    QCString trCompoundIndex() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Índex d'Estructures de Dades";
      }
      else
      {
        return "Índex de Classes";
      }
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * list of all files.
     */
    QCString trFileIndex() override
    { return "Índex de Fitxers"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all groups.
     */
    QCString trModuleDocumentation() override
    { return "Documentació dels Mòduls"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all classes, structs and unions.
     */
    QCString trClassDocumentation() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Documentació de les Estructures de Dades";
      }
      else if (Config_getBool(OPTIMIZE_OUTPUT_VHDL))
      {
          return trDesignUnitDocumentation();
      }
      else
      {
        return "Documentació de les Classes";
      }
    }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all files.
     */
    QCString trFileDocumentation() override
    { return "Documentació dels Fitxers"; }

    /*! This is used in LaTeX as the title of the document */
    QCString trReferenceManual() override
    { return "Manual de Referència"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of defines
     */
    QCString trDefines() override
    { return "Definicions"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of typedefs
     */
    QCString trTypedefs() override
    { return "Definicions de Tipus"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of enumerations
     */
    QCString trEnumerations() override
    { return "Enumeracions"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) functions
     */
    QCString trFunctions() override
    { return "Funcions"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trVariables() override
    { return "Variables"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) variables
     */
    QCString trEnumerationValues() override
    { return "Valors de les Enumeracions"; }

    /*! This is used in the documentation of a file before the list of
     *  documentation blocks for defines
     */
    QCString trDefineDocumentation() override
    { return "Documentació de les Definicions"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for typedefs
     */
    QCString trTypedefDocumentation() override
    { return "Documentació de les Definicions de Tipus"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration types
     */
    QCString trEnumerationTypeDocumentation() override
    { return "Documentació dels Tipus de les Enumeracions"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for functions
     */
    QCString trFunctionDocumentation() override
    { return "Documentació de les Funcions"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for variables
     */
    QCString trVariableDocumentation() override
    { return "Documentació de les Variables"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds
     */
    QCString trCompounds() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Estructures de Dades";
      }
      else
      {
        return "Classes";
      }
    }

    /*! This is used in the standard footer of each page and indicates when
     *  the page was generated
     */
    QCString trGeneratedAt(const QCString &date,const QCString &projName) override
    {
      QCString result="Generat a "+date;
      if (!projName.isEmpty()) result+=" per a "+projName;
      result+=" per";
      return result;
    }

    /*! this text is put before a class diagram */
    QCString trClassDiagram(const QCString &clName) override
    {
      return "Diagrama d'Herència per a "+clName+":";
    }

    /*! this text is generated when the \\warning command is used. */
    QCString trWarning() override
    { return "Atenció"; }

    /*! this text is generated when the \\version command is used. */
    QCString trVersion() override
    { return "Versió"; }

    /*! this text is generated when the \\date command is used. */
    QCString trDate() override
    { return "Data"; }

    /*! this text is generated when the \\return command is used. */
    QCString trReturns() override
    { return "Retorna"; }

    /*! this text is generated when the \\sa command is used. */
    QCString trSeeAlso() override
    { return "Mireu també"; }

    /*! this text is generated when the \\param command is used. */
    QCString trParameters() override
    { return "Paràmetres"; }

    /*! this text is generated when the \\exception command is used. */
    QCString trExceptions() override
    { return "Excepcions"; }

    /*! this text is used in the title page of a LaTeX document. */
    QCString trGeneratedBy() override
    { return "Generat per"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990307
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of page containing all the index of all namespaces. */
    QCString trNamespaceList() override
    { return "Llista dels Espais de Noms"; }

    /*! used as an introduction to the namespace list */
    QCString trNamespaceListDescription(bool extractAll) override
    {
      QCString result="Aquests són tots els espais de noms ";
      if (!extractAll) result+="documentats ";
      result+="amb breus descripcions:";
      return result;
    }

    /*! used in the class documentation as a header before the list of all
     *  friends of a class
     */
    QCString trFriends() override
    { return "Classes Amigues"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////

    /*! used in the class documentation as a header before the list of all
     * related classes
     */
    QCString trRelatedFunctionDocumentation() override
    { return "Documentació de funcions amigues i relacionades"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    /*! used as the title of the HTML page of a class/struct/union */
    QCString trCompoundReference(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result="Referència de";
      switch(compType)
      {
        case ClassDef::Class:      result+=" la Classe "; break;
        case ClassDef::Struct:     result+=" l'Estructura "; break;
        case ClassDef::Union:      result+=" la Unió "; break;
        case ClassDef::Interface:  result+=" la Interfície "; break;
        case ClassDef::Protocol:   result+="l Protocol "; break;
        case ClassDef::Category:   result+=" la Categoria "; break;
        case ClassDef::Exception:  result+=" l'Excepció "; break;
        default: break;
      }
      if (isTemplate) result+="Template ";
      result+=clName;
      return result;
    }

    /*! used as the title of the HTML page of a file */
    QCString trFileReference(const QCString &fileName) override
    {
      QCString result="Referència del Fitxer ";
      result+=fileName;
      return result;
    }

    /*! used as the title of the HTML page of a namespace */
    QCString trNamespaceReference(const QCString &namespaceName) override
    {
      QCString result="Referència de l'Espai de Noms ";
      result+=namespaceName;
      return result;
    }

    QCString trPublicMembers() override
    { return "Mètodes públics"; }
    QCString trPublicSlots() override
    { return "Slots públics"; }
    QCString trSignals() override
    { return "Senyals"; }
    QCString trStaticPublicMembers() override
    { return "Mètodes Públics Estàtics"; }
    QCString trProtectedMembers() override
    { return "Mètodes Protegits"; }
    QCString trProtectedSlots() override
    { return "Slots Protegits"; }
    QCString trStaticProtectedMembers() override
    { return "Mètodes Protegits Estàtics"; }
    QCString trPrivateMembers() override
    { return "Mètodes Privats"; }
    QCString trPrivateSlots() override
    { return "Slots Privats"; }
    QCString trStaticPrivateMembers() override
    { return "Mètodes Privats Estàtics"; }

    /*! this function is used to produce a comma-separated list of items.
     *  use generateMarker(i) to indicate where item i should be put.
     */
    QCString trWriteList(int numEntries) override
    {
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
            result+=" i ";
        }
      }
      return result;
    }

    /*! used in class documentation to produce a list of base classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritsList(int numEntries) override
    {
      return "Hereta de "+trWriteList(numEntries)+".";
    }

    /*! used in class documentation to produce a list of super classes,
     *  if class diagrams are disabled.
     */
    QCString trInheritedByList(int numEntries) override
    {
      return "Heretat per "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  members that are hidden by this one.
     */
    QCString trReimplementedFromList(int numEntries) override
    {
      return "Reimplementat de "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all member that overwrite the implementation of this member.
     */
    QCString trReimplementedInList(int numEntries) override
    {
      return "Reimplementat a "+trWriteList(numEntries)+".";
    }

    /*! This is put above each page as a link to all members of namespaces. */
    QCString trNamespaceMembers() override
    { return "Membres de l'Espai de Noms"; }

    /*! This is an introduction to the page with all namespace members */
    QCString trNamespaceMemberDescription(bool extractAll) override
    {
      QCString result="Aquesta és la llista de tots els membres de l'espai de noms ";
      if (!extractAll) result+="documentats ";
      result+="amb enllaços a ";
      if (extractAll)
        result+="la documentació de l'espai de noms de cada membre:";
      else
        result+="l'espai de noms al qual corresponen:";
      return result;
    }
    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all namespaces.
     */
    QCString trNamespaceIndex() override
    { return "Índex d'Espais de Noms"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all namespaces.
     */
    QCString trNamespaceDocumentation() override
    { return "Documentació de l'Espai de Noms"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

    /*! This is used in the documentation before the list of all
     *  namespaces in a file.
     */
    QCString trNamespaces() override
    { return "Espais de Noms"; }

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
      QCString result="La documentació d'aquest";
      switch(compType)
      {
        case ClassDef::Class:      result+="a classe"; break;
        case ClassDef::Struct:     result+="a estructura"; break;
        case ClassDef::Union:      result+="a unió"; break;
        case ClassDef::Interface:  result+="a interfície"; break;
        case ClassDef::Protocol:   result+=" protocol"; break;
        case ClassDef::Category:   result+="a categoria"; break;
        case ClassDef::Exception:  result+="a excepció"; break;
        default: break;
      }
      result+=" es va generar a partir del";
      if (!single) result+="s";
      result+=" següent";
      if (!single) result+="s";
      result+=" fitxer";
      if (!single) result+="s:"; else result+=":";
      return result;
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990901
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the heading text for the retval command. */
    QCString trReturnValues() override
    { return "Valors de retorn"; }

    /*! This is in the (quick) index as a link to the main page (index.html)
     */
    QCString trMainPage() override
    { return "Pàgina principal"; }

    /*! This is used in references to page that are put in the LaTeX
     *  documentation. It should be an abbreviation of the word page.
     */
    QCString trPageAbbreviation() override
    { return "p."; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991003
//////////////////////////////////////////////////////////////////////////

    QCString trDefinedAtLineInSourceFile() override
    {
      return "Definició a la línia @0 del fitxer @1.";
    }
    QCString trDefinedInSourceFile() override
    {
      return "Definició al fitxer @0.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-991205
//////////////////////////////////////////////////////////////////////////

    QCString trDeprecated() override
    {
      return "Antiquat";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.0.0
//////////////////////////////////////////////////////////////////////////

    /*! this text is put before a collaboration diagram */
    QCString trCollaborationDiagram(const QCString &clName) override
    {
      return "Diagrama de col·laboració per a "+clName+":";
    }
    /*! this text is put before an include dependency graph */
    QCString trInclDepGraph(const QCString &fName) override
    {
      return "Inclou el graf de dependències per a "+fName+":";
    }
    /*! header that is put before the list of constructor/destructors. */
    QCString trConstructorDocumentation() override
    {
      return "Documentació del Constructor i el Destructor";
    }
    /*! Used in the file documentation to point to the corresponding sources. */
    QCString trGotoSourceCode() override
    {
      return "Veure el codi d'aquest fitxer.";
    }
    /*! Used in the file sources to point to the corresponding documentation. */
    QCString trGotoDocumentation() override
    {
      return "Veure la documentació d'aquest fitxer.";
    }
    /*! Text for the \\pre command */
    QCString trPrecondition() override
    {
      return "Precondició";
    }
    /*! Text for the \\post command */
    QCString trPostcondition() override
    {
      return "Postcondició";
    }
    /*! Text for the \\invariant command */
    QCString trInvariant() override
    {
      return "Invariant";
    }
    /*! Text shown before a multi-line variable/enum initialization */
    QCString trInitialValue() override
    {
      return "Valor inicial:";
    }
    /*! Text used the source code in the file index */
    QCString trCode() override
    {
      return "codi";
    }
    QCString trGraphicalHierarchy() override
    {
      return "Jerarquia Gràfica de la Classe";
    }
    QCString trGotoGraphicalHierarchy() override
    {
      return "Veure la jerarquia gràfica de la classe";
    }
    QCString trGotoTextualHierarchy() override
    {
      return "Veure la jerarquia textual de la classe";
    }
    QCString trPageIndex() override
    {
      return "Índex de Pàgines";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.0
//////////////////////////////////////////////////////////////////////////

    QCString trNote() override
    {
      return "Nota";
    }
    QCString trPublicTypes() override
    {
      return "Tipus Públics";
    }
    QCString trPublicAttribs() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Camps de Dades";
      }
      else
      {
        return "Atributs Públics";
      }
    }
    QCString trStaticPublicAttribs() override
    {
      return "Atributs Públics Estàtics";
    }
    QCString trProtectedTypes() override
    {
      return "Tipus Protegits";
    }
    QCString trProtectedAttribs() override
    {
      return "Atributs Protegits";
    }
    QCString trStaticProtectedAttribs() override
    {
      return "Atributs Protegits Estàtics";
    }
    QCString trPrivateTypes() override
    {
      return "Tipus Privats";
    }
    QCString trPrivateAttribs() override
    {
      return "Atributs Privats";
    }
    QCString trStaticPrivateAttribs() override
    {
      return "Atributs Privats Estàtics";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\todo item */
    QCString trTodo() override
    {
      return "Per fer";
    }
    /*! Used as the header of the todo list */
    QCString trTodoList() override
    {
      return "Llista de coses per fer";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.4
//////////////////////////////////////////////////////////////////////////

    QCString trReferencedBy() override
    {
      return "Referenciat a";
    }
    QCString trRemarks() override
    {
      return "Remarca";
    }
    QCString trAttention() override
    {
      return "Atenció";
    }
    QCString trInclByDepGraph() override
    {
      return "Aquest gràfic mostra quins fitxers inclouen, "
             "de forma directa o indirecta, aquest fitxer:";
    }
    QCString trSince() override
    {
      return "Des de";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.1.5
//////////////////////////////////////////////////////////////////////////

    /*! title of the graph legend page */
    QCString trLegendTitle() override
    {
      return "Llegenda del Gràfic";
    }
    /*! page explaining how the dot graph's should be interpreted
     *  The %A in the text below are to prevent link to classes called "A".
     */
    QCString trLegendDocs() override
    {
      return
        "Aquesta pàgina explica com s'interpreten els gràfics generats per doxygen.<p>\n"
        "Considera aquest exemple:\n"
        "\\code\n"
        "/*! Classe invisible per culpa del retall */\n"
        "class Invisible { };\n\n"
        "/*! Classe truncada, l'herència està amagada */\n"
        "class Truncated : public Invisible { };\n\n"
        "/* Classe no documentada amb comentaris doxygen */\n"
        "class Undocumented { };\n\n"
        "/*! Classe heredada amb herència pública */\n"
        "class PublicBase : public Truncated { };\n\n"
        "/*! Una classe Template */\n"
        "template<class T> class Templ { };\n\n"
        "/*! Classe heredada utilitzant herència protegida */\n"
        "class ProtectedBase { };\n\n"
        "/*! Classe heredada utiltzant herència privada */\n"
        "class PrivateBase { };\n\n"
        "/*! Classe usada per la classe heretada */\n"
        "class Used { };\n\n"
        "/*! Super classe que hereda una quantitat de classes */\n"
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
        "Resultarà el gràfic següent:"
        "<p><center><img alt=\"\" src=\"graph_legend."+getDotImageExtension()+"\"></center>\n"
        "<p>\n"
        "Les caixes del gràfic superior tenen aquesta interpretació:\n"
        "<ul>\n"
        "<li>Una caixa negra plena represent l'estructura o classe per la qual el gràfic s'ha generat.\n"
        "<li>Una caixa de vora negra representa una estructura o classe documentada.\n"
        "<li>Una caixa de vora verda representa una estructura o classe indocumentada.\n"
        "<li>Una caixa de vora vermalla representa una estructura o classe documentada de la qual "
        "no es mostren totes les relacions d'herència/inclusió. Un gràfic és truncat si no s'ajusta als límits.\n"
        "</ul>\n"
        "Les sagetes tenen aquest significat:\n"
        "<ul>\n"
        "<li>Una sageta blau fosc remarca una relació d'herència de tipus pública entre dues classes.\n"
        "<li>Una sageta verd fosc remarca una relació d'herència de tipus protegida entre dues classes.\n"
        "<li>Una sageta roig fosc remarca una relació d'herència de tipus privada entre dues classes.\n"
        "<li>Una sageta puntejada de color porpra indica que una classe és continguda o usada per una altra classe."
        " La sageta s'etiqueta amb la variable o variables a través de les quals la classe o estructura apuntada és accessible.\n"
        "<li>Una sageta puntejada de color groc indica la relació entre una instància template i la classe template de què ha set instanciada."
        " La sageta s'etiqueta amb els paràmetres template de la instància.\n"
        "</ul>\n";
    }
    /*! text for the link to the legend page */
    QCString trLegend() override
    {
      return "llegenda";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.0
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a test item */
    QCString trTest() override
    {
      return "Prova";
    }
    /*! Used as the header of the test list */
    QCString trTestList() override
    {
      return "Llista de proves";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.2
//////////////////////////////////////////////////////////////////////////

    /*! Used as a section header for IDL properties */
    QCString trProperties() override
    {
      return "Propietats";
    }
    /*! Used as a section header for IDL property documentation */
    QCString trPropertyDocumentation() override
    {
      return "Documentació de les Propietats";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.4
//////////////////////////////////////////////////////////////////////////

    /*! Used for Java classes in the summary section of Java packages */
    QCString trClasses() override
    {
      if (Config_getBool(OPTIMIZE_OUTPUT_FOR_C))
      {
        return "Estructures de Dades";
      }
      else
      {
        return "Classes";
      }
    }
    /*! Used as the title of a Java package */
    QCString trPackage(const QCString &name) override
    {
      return "Paquet "+name;
    }
    /*! The description of the package index page */
    QCString trPackageListDescription() override
    {
      return "Aquesta és la llista de paquets, amb una breu descripció (si se'n disposa):";
    }
    /*! The link name in the Quick links header for each page */
    QCString trPackages() override
    {
      return "Paquets";
    }
    /*! Text shown before a multi-line define */
    QCString trDefineValue() override
    {
      return "Valor:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.5
//////////////////////////////////////////////////////////////////////////

    /*! Used as a marker that is put before a \\bug item */
    QCString trBug() override
    {
      return "Error";
    }
    /*! Used as the header of the bug list */
    QCString trBugList() override
    {
      return "Llista d'Errors";
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
      return "Índex";
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trClass(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "classe", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trFile(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "fitxer", "s");
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
      return createNoun(first_capital, singular, "grup", "s");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trPage(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "pàgin", "es", "a");
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trMember(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "membre", "s");
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
      return createNoun(first_capital, singular, "autor", "s");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.11
//////////////////////////////////////////////////////////////////////////

    /*! This text is put before the list of members referenced by a member
     */
    QCString trReferences() override
    {
      return "Referències";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.13
//////////////////////////////////////////////////////////////////////////

    /*! used in member documentation blocks to produce a list of
     *  members that are implemented by this one.
     */
    QCString trImplementedFromList(int numEntries) override
    {
      return "Implementa "+trWriteList(numEntries)+".";
    }

    /*! used in member documentation blocks to produce a list of
     *  all members that implement this abstract member.
     */
    QCString trImplementedInList(int numEntries) override
    {
      return "Implementat a "+trWriteList(numEntries)+".";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.16
//////////////////////////////////////////////////////////////////////////

    /*! used in RTF documentation as a heading for the Table
     *  of Contents.
     */
    QCString trRTFTableOfContents() override
    {
      return "Taula de Continguts";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.17
//////////////////////////////////////////////////////////////////////////

    /*! Used as the header of the list of item that have been
     *  flagged deprecated
     */
    QCString trDeprecatedList() override
    {
      return "Llista d'Antiquats";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.2.18
//////////////////////////////////////////////////////////////////////////

    /*! Used as a header for declaration section of the events found in
     * a C# program
     */
    QCString trEvents() override
    {
      return "Esdeveniments";
    }
    /*! Header used for the documentation section of a class' events. */
    QCString trEventDocumentation() override
    {
      return "Documentació dels Esdeveniments";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3
//////////////////////////////////////////////////////////////////////////

    /*! Used as a heading for a list of Java class types with package scope.
     */
    QCString trPackageTypes() override
    {
      return "Tipus de paquets";
    }
    /*! Used as a heading for a list of Java class functions with package
     * scope.
     */
    QCString trPackageFunctions() override
    {
      return "Funcions de Paquet";
    }
    QCString trPackageMembers() override
    {
      return "Membres de Paquet";
    }
    /*! Used as a heading for a list of static Java class functions with
     *  package scope.
     */
    QCString trStaticPackageFunctions() override
    {
      return "Funcions Estàtiques de Paquet";
    }
    /*! Used as a heading for a list of Java class variables with package
     * scope.
     */
    QCString trPackageAttribs() override
    {
      return "Atributs de Paquet";
    }
    /*! Used as a heading for a list of static Java class variables with
     * package scope.
     */
    QCString trStaticPackageAttribs() override
    {
      return "Atributs Estàtics de Paquet";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.1
//////////////////////////////////////////////////////////////////////////

    /*! Used in the quick index of a class/file/namespace member list page
     *  to link to the unfiltered list of all members.
     */
    QCString trAll() override
    {
      return "Tot";
    }
    /*! Put in front of the call graph for a function. */
    QCString trCallGraph() override
    {
      return "Gràfic de crides d'aquesta funció:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.3
//////////////////////////////////////////////////////////////////////////

    /*! This string is used as the title for the page listing the search
     *  results.
     */
    QCString trSearchResultsTitle() override
    {
      return "Resultats de la Búsqueda";
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
        return "No s'ha trobat cap document.";
      }
      else if (numDocuments==1)
      {
        return "Trobat <b>1</b> document.";
      }
      else
      {
        return "Trobats <b>$num</b> documents. "
               "Mostrant els millors resultats primer.";
      }
    }
    /*! This string is put before the list of matched words, for each search
     *  result. What follows is the list of words that matched the query.
     */
    QCString trSearchMatches() override
    {
      return "Resultats:";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.8
//////////////////////////////////////////////////////////////////////////

    /*! This is used in HTML as the title of page with source code for file filename
     */
    QCString trSourceFile(const QCString& filename) override
    {
      return "Fitxer de Codi " + filename;
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.3.9
//////////////////////////////////////////////////////////////////////////

    /*! This is used as the name of the chapter containing the directory
     *  hierarchy.
     */
    QCString trDirIndex() override
    { return "Jerarquia de Directoris"; }

    /*! This is used as the name of the chapter containing the documentation
     *  of the directories.
     */
    QCString trDirDocumentation() override
    { return "Documentació dels Directoris"; }

    /*! This is used as the title of the directory index and also in the
     *  Quick links of a HTML page, to link to the directory hierarchy.
     */
    QCString trDirectories() override
    { return "Directoris"; }

    /*! This returns the title of a directory page. The name of the
     *  directory is passed via \a dirName.
     */
    QCString trDirReference(const QCString &dirName) override
    { QCString result="Referència del Directori "; result+=dirName; return result; }

    /*! This returns the word directory with or without starting capital
     *  (\a first_capital) and in sigular or plural form (\a singular).
     */
    QCString trDir(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "directori", "s");
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.1
//////////////////////////////////////////////////////////////////////////

    /*! This text is added to the documentation when the \\overload command
     *  is used for a overloaded function.
     */
    QCString trOverloadText() override
    {
       return "Aquesta és una funció membre sobrecarregada, "
              "proveïda per conveniència. Es diferencia de la funció "
              "anterior només en els arguments que accepta.";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.4.6
//////////////////////////////////////////////////////////////////////////

    /*! This is used to introduce a caller (or called-by) graph */
    QCString trCallerGraph() override
    {
      return "Gràfic de crides a aquesta funció:";
    }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for enumeration values
     */
    QCString trEnumerationValueDocumentation() override
    { return "Documentació de les Enumeracions"; }

//////////////////////////////////////////////////////////////////////////
// new since 1.5.4 (mainly for Fortran)
//////////////////////////////////////////////////////////////////////////

    /*! header that is put before the list of member subprograms (Fortran). */
    QCString trMemberFunctionDocumentationFortran() override
    { return "Documentació de les Funcions/Subrutines Membre"; }

    /*! This is put above each page as a link to the list of annotated data types (Fortran). */
    QCString trCompoundListFortran() override
    { return "Llista de Tipus de Dades"; }

    /*! This is put above each page as a link to all members of compounds (Fortran). */
    QCString trCompoundMembersFortran() override
    { return "Camps de Dades"; }

    /*! This is an introduction to the annotated compound list (Fortran). */
    QCString trCompoundListDescriptionFortran() override
    { return "Aquests són els tipus de dades acompanyats amb breus descripcions:"; }

    /*! This is an introduction to the page with all data types (Fortran). */
    QCString trCompoundMembersDescriptionFortran(bool extractAll) override
    {
      QCString result="Aquesta és la llista de tots els membres de tipus de dades";
      if (!extractAll)
      {
        result+=" documentats";
      }
      result+=" amb enllaços a ";
      if (!extractAll)
      {
        result+="la documentació del tipus de dades per a cada membre:";
      }
      else
      {
        result+="els tipus de dades a que pertanyen:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     * annotated compound index (Fortran).
     */
    QCString trCompoundIndexFortran() override
    { return "Índex de Tipus de Dades"; }

    /*! This is used in LaTeX as the title of the chapter containing
     *  the documentation of all data types (Fortran).
     */
    QCString trTypeDocumentation() override
    { return "Documentació dels Tipus de Dades"; }

    /*! This is used in the documentation of a file as a header before the
     *  list of (global) subprograms (Fortran).
     */
    QCString trSubprograms() override
    { return "Funcions/Subrutines"; }

    /*! This is used in the documentation of a file/namespace before the list
     *  of documentation blocks for subprograms (Fortran)
     */
    QCString trSubprogramDocumentation() override
    { return "Documentació de les Funcions/Subrutines"; }

    /*! This is used in the documentation of a file/namespace/group before
     *  the list of links to documented compounds (Fortran)
     */
     QCString trDataTypes() override
    { return "Tipus de Dades"; }

    /*! used as the title of page containing all the index of all modules (Fortran). */
    QCString trModulesList() override
    { return "Llista de Mòduls"; }

    /*! used as an introduction to the modules list (Fortran) */
    QCString trModulesListDescription(bool extractAll) override
    {
      QCString result="Aquesta és la llista de tots els mòduls ";
      if (!extractAll) result+="documentats ";
      result+="amb breus descripcions:";
      return result;
    }

    /*! used as the title of the HTML page of a module/type (Fortran) */
    QCString trCompoundReferenceFortran(const QCString &clName,
                                    ClassDef::CompoundType compType,
                                    bool isTemplate) override
    {
      QCString result="Referència de";
      switch(compType)
      {
        case ClassDef::Class:      result+=" el Mòdul "; break;
        case ClassDef::Struct:     result+=" el Tipus "; break;
        case ClassDef::Union:      result+=" la Unió "; break;
        case ClassDef::Interface:  result+=" la Interfície "; break;
        case ClassDef::Protocol:   result+="l Protocol "; break;
        case ClassDef::Category:   result+=" la Categoria "; break;
        case ClassDef::Exception:  result+=" l'Excepció "; break;
        default: break;
      }
      if (isTemplate) result+="Template ";
      result+=clName;
      return result;
    }

    /*! used as the title of the HTML page of a module (Fortran) */
    QCString trModuleReference(const QCString &namespaceName) override
    {
      QCString result="Referència del Mòdul ";
      result+=namespaceName;
      return result;
    }

    /*! This is put above each page as a link to all members of modules. (Fortran) */
    QCString trModulesMembers() override
    { return "Membres del Mòdul"; }

    /*! This is an introduction to the page with all modules members (Fortran) */
    QCString trModulesMemberDescription(bool extractAll) override
    {
      QCString result="Aquesta és la llista de tots els membres del mòdul";
      if (!extractAll)
      {
        result+=" documentats";
      }
      result+=" amb enllaços a ";
      if (!extractAll)
      {
        result+="la documentació del mòdul per a cada membre:";
      }
      else
      {
        result+="els mòduls a que pertanyen:";
      }
      return result;
    }

    /*! This is used in LaTeX as the title of the chapter with the
     *  index of all modules (Fortran).
     */
    QCString trModulesIndex() override
    { return "Índex de Mòduls"; }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trModule(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "mòdul", "s");
    }
    /*! This is put at the bottom of a module documentation page and is
     *  followed by a list of files that were used to generate the page.
     */
    QCString trGeneratedFromFilesFortran(ClassDef::CompoundType compType,
        bool single) override
    { // here s is one of " Module", " Struct" or " Union"
      // single is true implies a single file
      QCString result="La documentació d'aquest";
      switch(compType)
      {
        case ClassDef::Class:      result+=" mòdul"; break;
        case ClassDef::Struct:     result+=" tipus"; break;
        case ClassDef::Union:      result+="a unió"; break;
        case ClassDef::Interface:  result+="a interfície"; break;
        case ClassDef::Protocol:   result+=" protocol"; break;
        case ClassDef::Category:   result+="a categoria"; break;
        case ClassDef::Exception:  result+="a excepció"; break;
        default: break;
      }
      result+=" es va generar a partir del";
      if (!single) result+="s";
      result+=" següent";
      if (!single) result+="s";
      result+=" fitxer";
      if (!single) result+="s:"; else result+=":";
      return result;
    }

    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trType(bool first_capital, bool) override
    {
      return createNoun(first_capital, false, "tipus", "");
    }
    /*! This is used for translation of the word that will possibly
     *  be followed by a single name or by a list of names
     *  of the category.
     */
    QCString trSubprogram(bool first_capital, bool singular) override
    {
      return createNoun(first_capital, singular, "subprogram", "es", "a");
    }

    /*! C# Type Constraint list */
    QCString trTypeConstraints() override
    {
      return "Restriccions de Tipus";
    }

//////////////////////////////////////////////////////////////////////////
// new since 1.6.0 (mainly for the new search engine)
//////////////////////////////////////////////////////////////////////////

    /*! directory relation for \a name */
    QCString trDirRelation(const QCString &name) override
    {
      return QCString(name)+" Relació";
    }

    /*! Loading message shown when loading search results */
    QCString trLoading() override
    {
      return "Carregant...";
    }

    /*! Label used for search results in the global namespace */
    QCString trGlobalNamespace() override
    {
      return "Espai de Noms Global";
    }

    /*! Message shown while searching */
    QCString trSearching() override
    {
      return "Cercant...";
    }

    /*! Text shown when no search results are found */
    QCString trNoMatches() override
    {
      return "Cap coincidència";
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
      return "Fitxer a "+name;
    }

    /*! when clicking a directory dependency label, a page with a
     *  table is shown. The heading for the second column mentions the
     *  destination file that is included.
     */
    QCString trIncludesFileIn(const QCString &name) override
    {
      return "Inclou fitxer a "+name;
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
      static const char *days[]   = { "Dl","Dt","Dc","Dj","Dv","Ds","Dg" };
      static const char *months[] = { "Gen","Feb","Mar","Abr","Mai","Jun","Jul","Ago","Sep","Oct","Nov","Dec" };
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
    QCString trDayOfWeek(int dayOfWeek, bool first_capital, bool full) override
    {
      static const char *days_short[]   = { "dl.", "dt.", "dc.", "dj.", "dv.", "ds.", "dg." };
      static const char *days_full[]    = { "dilluns", "dimarts", "dimecres", "dijous", "divendres", "dissabte", "diumenge" };
      QCString text  = full? days_full[dayOfWeek-1] : days_short[dayOfWeek-1];
      if (first_capital) return text.mid(0,1).upper()+text.mid(1);
      else return text;
    }
    QCString trMonth(int month, bool first_capital, bool full) override
    {
      static const char *months_short[] = { "gen.", "febr.", "març", "abr.", "maig", "juny", "jul.", "ag.", "set.", "oct.", "nov.", "des." };
      static const char *months_full[]  = { "gener", "febrer", "març", "abril", "maig", "juny", "juliol", "agost", "setembre", "octubre", "novembre", "desembre" };
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
    { return "Referències Bibliogràfiques"; }

    /*! Text for copyright paragraph */
    QCString trCopyright() override
    { return "Copyright"; }

    /*! Header for the graph showing the directory dependencies */
    QCString trDirDepGraph(const QCString &name) override
    { return QCString("Graf de dependència de directoris per a ")+name+":"; }


};

#endif
