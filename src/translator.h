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

#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <qstring.h>

class Translator
{
  public:
    virtual QString latexBabelPackage()
    { return ""; }
    virtual QString trInherits()
    { return "Inherits"; }
    virtual QString trAnd()
    { return "and"; }
    virtual QString trInheritedBy()
    { return "Inherited By"; }
    virtual QString trRelatedFunctions()
    { return "Related Functions"; }
    virtual QString trRelatedSubscript()
    { return "(Note that these are not member functions.)"; }
    virtual QString trDetailedDescription()
    { return "Detailed Description"; }
    virtual QString trMemberTypedefDocumentation()
    { return "Member Typedef Documentation"; }
    virtual QString trMemberEnumerationDocumentation()
    { return "Member Enumeration Documentation"; }
    virtual QString trMemberFunctionDocumentation()
    { return "Member Function Documentation"; }
    virtual QString trMemberDataDocumentation()
    { return "Member Data Documentation"; }
    virtual QString trGeneratedFrom(const char *s,bool single)
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      QString result=(QString)"The documentation for this"+s+
                     " was generated from the following file";
      if (single) result+=":"; else result+="s:";
      return result;
    }
    virtual QString trMore()
    { return "More..."; }
    virtual QString trReference()
    { return "Reference"; }
    virtual QString trListOfAllMembers()
    { return "List of all members."; }
    virtual QString trMemberList()
    { return "Member List"; }
    virtual QString trThisIsTheListOfAllMembers()
    { return "This is the complete list of members for"; }
    virtual QString trIncludingInheritedMembers()
    { return "including all inherited members."; }
    virtual QString trGeneratedAutomatically(const char *s)
    { QString result="Generated automatically by Doxygen";
      if (s) result+=(QString)" for "+s;
      result+=" from the source code."; 
      return result;
    }
    virtual QString trEnumName()
    { return "enum name"; }
    virtual QString trEnumValue()
    { return "enum value"; }
    virtual QString trDefinedIn()
    { return "defined in"; }
    virtual QString trIncludeFile()
    { return "Include File"; }
    virtual QString trVerbatimText(const char *f)
    { return (QString)"This is the verbatim text of the "+f+" include file."; }
    
    
    virtual QString trModules()
    { return "Modules"; }
    virtual QString trClassHierarchy()
    { return "Class Hierarchy"; }
    virtual QString trCompoundList()
    { return "Compound List"; }
    virtual QString trFileList()
    { return "File List"; }
    virtual QString trHeaderFiles()
    { return "Header Files"; }
    virtual QString trCompoundMembers()
    { return "Compound Members"; }
    virtual QString trFileMembers()
    { return "File Members"; }
    virtual QString trRelatedPages()
    { return "Related Pages"; }
    virtual QString trExamples()
    { return "Examples"; }
    virtual QString trSearch()
    { return "Search"; }
    
    virtual QString trClassHierarchyDescription()
    { return "This inheritance list is sorted roughly, "
             "but not completely, alphabetically:";
    }
    virtual QString trFileListDescription(bool extractAll)
    {
      QString result="Here is a list of all ";
      if (!extractAll) result+="documented ";
      result+="files with brief descriptions:";
      return result;
    }
    virtual QString trCompoundListDescription()
    { return "Here are the classes, structs and "
             "unions with brief descriptions:"; 
    }
    virtual QString trCompoundMembersDescription(bool extractAll)
    {
      QString result="Here is a list of all ";
      if (!extractAll) result+="documented ";
      result+="class members with links to ";
      if (extractAll) 
        result+="the class documentation for each member:";
      else 
        result+="the classes they belong to:";
      return result;
    }
    virtual QString trFileMembersDescription(bool extractAll)
    {
      QString result="Here is a list of all ";
      if (!extractAll) result+="documented ";
      result+="file members with links to ";
      if (extractAll) 
        result+="the file documentation for each member:";
      else 
        result+="the files they belong to:";
      return result;
    }
    virtual QString trHeaderFilesDescription()
    { return "Here are the header files that make up the API:"; }
    virtual QString trExamplesDescription()
    { return "Here is a list of all examples:"; }
    virtual QString trRelatedPagesDescription()
    { return "Here is a list of all related documentation pages:"; }
    virtual QString trModulesDescription()
    { return "Here is a list of all modules:"; }
    virtual QString trNoDescriptionAvailable()
    { return "No description available"; }
    
    virtual QString trDocumentation()
    { return "Documentation"; }
    virtual QString trModuleIndex()
    { return "Module Index"; }
    virtual QString trHierarchicalIndex()
    { return "Hierarchical Index"; }
    virtual QString trCompoundIndex()
    { return "Compound Index"; }
    virtual QString trFileIndex() 
    { return "File Index"; }
    virtual QString trModuleDocumentation()
    { return "Module Documentation"; }
    virtual QString trClassDocumentation()
    { return "Class Documentation"; }
    virtual QString trFileDocumentation()
    { return "File Documentation"; }
    virtual QString trExampleDocumentation()
    { return "Example Documentation"; }
    virtual QString trPageDocumentation()
    { return "Page Documentation"; }
    virtual QString trReferenceManual()
    { return "Reference Manual"; }
    
    virtual QString trDefines()
    { return "Defines"; }
    virtual QString trFuncProtos()
    { return "Function Prototypes"; }
    virtual QString trTypedefs()
    { return "Typedefs"; }
    virtual QString trEnumerations()
    { return "Enumerations"; }
    virtual QString trFunctions()
    { return "Functions"; }
    virtual QString trVariables()
    { return "Variables"; }
    virtual QString trEnumerationValues()
    { return "Enumeration values"; }
    virtual QString trReimplementedFrom()
    { return "Reimplemented from"; }
    virtual QString trReimplementedIn()
    { return "Reimplemented in"; }
    virtual QString trAuthor()
    { return "Author"; }
    virtual QString trDefineDocumentation()
    { return "Define Documentation"; }
    virtual QString trFunctionPrototypeDocumentation()
    { return "Function Prototype Documentation"; }
    virtual QString trTypedefDocumentation()
    { return "Typedef Documentation"; }
    virtual QString trEnumerationTypeDocumentation()
    { return "Enumeration Type Documentation"; }
    virtual QString trEnumerationValueDocumentation()
    { return "Enumeration Value Documentation"; }
    virtual QString trFunctionDocumentation()
    { return "Function Documentation"; }
    virtual QString trVariableDocumentation()
    { return "Variable Documentation"; }
    virtual QString trCompounds()
    { return "Compounds"; }
    virtual QString trFiles()
    { return "Files"; }

    virtual QString trGeneratedAt(const char *date,const char *projName)
    { 
      QString result=(QString)"Generated at "+date;
      if (projName) result+=(QString)" for "+projName;
      result+=(QString)" by";
      return result;
    }
    virtual QString trWrittenBy()
    {
      return "written by";
    }

    virtual QString trClassDiagram(const char *clName)
    {
      return (QString)"Class diagram for "+clName;
    }
    
    virtual QString trForInternalUseOnly()
    { return "For internal use only."; }
    virtual QString trReimplementedForInternalReasons()
    { return "Reimplemented for internal reasons; the API is not affected."; }
    virtual QString trWarning()
    { return "Warning"; }
    virtual QString trBugsAndLimitations()
    { return "Bugs and limitations"; }
    virtual QString trVersion()
    { return "Version"; }
    virtual QString trDate()
    { return "Date"; }
    virtual QString trAuthors()
    { return "Author(s)"; }
    virtual QString trReturns()
    { return "Returns"; }
    virtual QString trSeeAlso()
    { return "See also"; }
    virtual QString trParameters()
    { return "Parameters"; }
    virtual QString trExceptions()
    { return "Exceptions"; }
    virtual QString trGeneratedBy()
    { return "Generated by"; }

    // new since 0.49-990307
    
    virtual QString trNamespaces()
    { return "Namespaces"; }
    virtual QString trNamespaceList()
    { return "Namespace List"; }
    virtual QString trNamespaceListDescription(bool extractAll)
    {
      QString result="Here is a list of all ";
      if (!extractAll) result+="documented ";
      result+="namespaces with brief descriptions:";
      return result;
    }
    virtual QString trFriends()
    { return "Friends"; }
    
    // new since 0.49-990405
    
    virtual QString trRelatedFunctionDocumentation()
    { return "Friends And Related Function Documentation"; }
};

#endif
