/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-1999 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * All output generated with Doxygen is not covered by this license.
 *
 */

#ifndef TRANSLATOR_SE_H
#define TRANSLATOR_SE_H

#include <qstring.h>

class TranslatorSwedish : public Translator
{
  public:
    QString latexBabelPackage()
    { return "swedish"; }
    QString trInherits()
    { return "Ärver"; }
    QString trAnd()
    { return "och"; }
    QString trInheritedBy()
    { return "Ärvd av"; }
    QString trRelatedFunctions()
    { return "Besläktade funktioner"; }
    QString trRelatedSubscript()
    { return "(Observera att dessa inte är medlemsfunktioner)"; }
    QString trDetailedDescription()
    { return "Utökad beskrivning"; }
    QString trMemberTypedefDocumentation()
    { return "Har inte en aning..."; }
    QString trMemberEnumerationDocumentation()
    { return "Uppräknad dokumentation???"; }
    QString trEnumerationValueDocumentation()
    { return "Documentatie van enumeratie waarden"; }
    QString trMemberFunctionDocumentation()
    { return "Dokumentation av medlemsfunktioner"; }
    QString trMemberDataDocumentation()
    { return "Dokumentation av datamedlemmar"; }
    QString trGeneratedFrom(const char *s,bool single)
    { 
      QString result=(QString)"Dokumentationen för detta"+s+
                     " är skapad ur följande fil";
      if (single) result+=":";   else result+="s:";
      return result;
    }
    QString trMore()
    { return "Fler..."; }
    QString trReference()
    { return "Hänvisning?"; }
    QString trListOfAllMembers()
    { return "Lista över alla medlemmar."; }
    QString trMemberList()
    { return "Medlemslista"; }
    QString trThisIsTheListOfAllMembers()
    { return "Det här är en fullständig lista av medlemmar för?"; }
    QString trIncludingInheritedMembers()
    { return "med alla ärvda medlemmar."; }
    QString trGeneratedAutomatically(const char *s)
    { QString result="Automatiskt skapad av Doxygen";
      if (s) result+=(QString)" för "+s;
      result+=" från källkoden."; 
      return result;
    }
    QString trEnumName()
    { return "enum namn?"; }
    QString trEnumValue()
    { return "enum värde?"; }
    QString trDefinedIn()
    { return "deklarerad i"; }
    QString trIncludeFile()
    { return "Include Fil"; }
    QString trVerbatimText(const char *f)
    { return (QString)"Detta är den ordagranna texten av "+f+" include fil."; }
    QString trModules()
    { return "Moduler?"; }
    QString trClassHierarchy()
    { return "Klasshierarki?"; }
    QString trCompoundList()
    { return "Inhägnad lista???"; }
    QString trFileList()
    { return "Fillista"; }
    QString trHeaderFiles()
    { return "Headerfiler"; }
    QString trCompoundMembers()
    { return "Inhägnade medlemmar??"; }
    QString trFileMembers()
    { return "Filmedlemmar???"; }
    QString trRelatedPages()
    { return "Besläktade sidor"; }
    QString trExamples()
    { return "Exempel"; }
    QString trSearch()
    { return "Sök"; }
    QString trClassHierarchyDescription()
    { return "Denna lista över arv är grovsorterad, men inte helt "
             "i alfabetisk ordning:";
    }
    QString trFileListDescription(bool extractAll)
    {
      QString result="Här följer en lista över alla ";
      if (!extractAll) result+="dokumenterade ";
      result+="filer, med en kort beskrivning:";
      return result;
    }
    QString trCompoundListDescription()
     { return "Här följer klasserna, structs och "
             "unions? med en kort beskrivning:"; 
    }
    QString trCompoundMembersDescription(bool extractAll)
    {
      QString result="Här följer en lista över alla ";
      if (!extractAll) result+="dokumenterade ";
      result+="klassmedlemmar med länkar till ";
      if (extractAll) result+="klassdokumentationen för varje medlem:";
      else result+="klasserna som de tillhör:";
      return result;
    }
    QString trFileMembersDescription(bool extractAll)
    {
      QString result="Här följer en lista över alla ";
      if (!extractAll) result+="dokumenterade ";
      result+="filmedlemmar? med länkar till ";
      if (extractAll) result+="dokumentationsfilen för varje medlem:";
      else result+="filerna som de tillhör:";
      return result;
    }
    QString trHeaderFilesDescription()
    { return "Här följer headerfilerna som API:n består av???:"; }
    QString trExamplesDescription()
    { return "Här följer en lista med alla exempel:"; }
    QString trRelatedPagesDescription()
    { return "Här följer en lista med alla relevanta? dokumentationssidor:"; }
    QString trModulesDescription()
    { return "Här följer en lista över alla moduler:"; }
    QString trNoDescriptionAvailable()
    { return "Beskrivning saknas"; }

    QString trDocumentation()
    { return "Dokumentation"; }
    QString trModuleIndex()
    { return "Module Index"; }
    QString trHierarchicalIndex()
    { return "Hierarkiskt Index"; }
    QString trCompoundIndex()
    { return "Compound Index"; }
    QString trFileIndex() 
    { return "Fil Index"; }
    QString trModuleDocumentation()
    { return "Dokumentation över moduler"; }
    QString trClassDocumentation()
    { return "Dokumentation över klasser"; }
    QString trFileDocumentation()
    { return "Dokumentation över filer"; }
    QString trExampleDocumentation()
    { return "Dokumentation över exempel"; }
    QString trPageDocumentation()
    { return "Dokumentation av sidor"; }
    QString trReferenceManual()
    { return "Uppslagsbok"; }

    QString trDefines()
    { return "Definerar"; }
    QString trFuncProtos()
    { return "Funktionsprototyper"; }
    QString trTypedefs()
    { return "Typedefs"; }
    QString trEnumerations()
    { return "Uppräkning???"; }
    QString trFunctions()
    { return "Funktioner"; }
    QString trVariables()
    { return "Variabler"; }
    QString trEnumerationValues()
    { return "Enum värden??"; }
    QString trReimplementedFrom()
    { return "Återanvänd från???"; }
    QString trReimplementedIn()
    { return "Återanvänd i??"; }
    QString trAuthor()
    { return "Författare"; }
    QString trDefineDocumentation()
    { return "Definiera dokumentation"; }
    QString trFunctionPrototypeDocumentation()
    { return "Dokumentation över funktionsprototyper"; }
    QString trTypedefDocumentation()
    { return "Dokumentation över typedefs"; }
    QString trEnumerationTypeDocumentation()
    { return "Dokumentation över enum typer"; }
    QString trFunctionDocumentation()
    { return "Dokumentation över funktioner"; }
    QString trVariableDocumentation()
    { return "Dokumentation över variabler"; }
    QString trCompounds()
    { return "Compounds??"; }
    QString trFiles()
    { return "Filer"; }
    QString trGeneratedAt(const char *date,const char *projName)
    { 
      QString result=(QString)"Skapad "+date;
      if (projName) result+=(QString)" för "+projName;
      result+=(QString)" av";
      return result;
    }
    QString trWrittenBy()
    {
      return "skriven av";
    }
    QString trClassDiagram(const char *clName)
    {
      return (QString)"Klass diagram för "+clName;
    }
    QString trForInternalUseOnly()
    { return "Endast för internt bruk."; }
    QString trReimplementedForInternalReasons()
    { return "Omskriven av interna orsaker???; "
             "API:n påverkas inte."; 
    }
    QString trWarning()
    { return "Varning"; }
    QString trBugsAndLimitations()
    { return "Fel och begränsningar"; }
    QString trVersion()
    { return "Version"; }
    QString trDate()
    { return "Datum"; }
    QString trAuthors()
    { return "Författare"; }
    QString trReturns()
    { return "Returnerar"; }
    QString trSeeAlso()
    { return "Se även"; }
    QString trParameters()
    { return "Parametrar"; }
    QString trExceptions()
    { return "Undantag"; }
    QString trGeneratedBy()
    { return "Skapad av"; }
};

#endif
