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
#include "classdef.h"
#include "util.h"

class Translator
{
  public:
    //--------------------------------------------------------------------
    // NOTICE:
    // the following functions are now obsolete: these are no longer used and 
    // will disappear in future versions. You do not have to translate them!
    virtual QString trInherits() 
    { return "Inherits"; }
    virtual QString trAnd() 
    { return "and"; }
    virtual QString trInheritedBy() 
    { return "Inherited By"; }
    virtual QString trReference() 
    { return "Reference"; }
    virtual QString trReimplementedFrom()
    { return "Reimplemented from"; }
    virtual QString trReimplementedIn()
    { return "Reimplemented in"; }
    virtual QString trIncludeFile()
    { return "Include File"; }
    virtual QString trNamespaces()
    { return "Namespace List"; }
    // end of obsolete functions 
    //--------------------------------------------------------------------

    virtual QString latexBabelPackage() 
      // returns the name of the package that is included by LaTeX
    { return ""; }

    virtual QString trRelatedFunctions()
      // used in the compound documentation before a list of related functions.
    { return "Related Functions"; }

    virtual QString trRelatedSubscript()
      // subscript for the related functions.
    { return "(Note that these are not member functions.)"; }

    virtual QString trDetailedDescription()
      // header that is put before the detailed description of files, classes and namespaces.
    { return "Detailed Description"; }

    virtual QString trMemberTypedefDocumentation()
      // header that is put before the list of typedefs.
    { return "Member Typedef Documentation"; }
    
    virtual QString trMemberEnumerationDocumentation()
      // header that is put before the list of enumerations.
    { return "Member Enumeration Documentation"; }
    
    virtual QString trMemberFunctionDocumentation()
      // header that is put before the list of member functions.
    { return "Member Function Documentation"; }
    
    virtual QString trMemberDataDocumentation()
      // header that is put before the list of member attributes.
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
      // this is the text of a link put after brief descriptions.
    { return "More..."; }

    virtual QString trListOfAllMembers()
      // put in the class documentation
    { return "List of all members."; }

    virtual QString trMemberList()
      // used as the title of the "list of all members" page of a class
    { return "Member List"; }

    virtual QString trThisIsTheListOfAllMembers()
      // this is the first part of a sentence that is followed by a class name
    { return "This is the complete list of members for "; }
    virtual QString trIncludingInheritedMembers()
      // this is the remainder of the sentence after the class name
    { return ", including all inherited members."; }
    
    virtual QString trGeneratedAutomatically(const char *s)
      // this is put at the author sections at the bottom of man pages.
      // parameter s is name of the project name.
    { QString result="Generated automatically by Doxygen";
      if (s) result+=(QString)" for "+s;
      result+=" from the source code."; 
      return result;
    }

    virtual QString trEnumName()
      // put after an enum name in the list of all members
    { return "enum name"; }
    
    virtual QString trEnumValue()
      // put after an enum value in the list of all members
    { return "enum value"; }
    
    virtual QString trDefinedIn()
      // put after an undocumented member in the list of all members
    { return "defined in"; }

    virtual QString trVerbatimText(const char *f)
      // put as in introduction in the verbatim header file of a class.
      // parameter f is the name of the include file.
    { return (QString)"This is the verbatim text of the "+f+" include file."; }
    
    // quick reference sections
    virtual QString trModules()
      // This is put above each page as a link to the list of all groups of 
      // compounds or files (see the \group command).
    { return "Modules"; }
    virtual QString trClassHierarchy()
      // This is put above each page as a link to the class hierarchy 
    { return "Class Hierarchy"; }
    virtual QString trCompoundList()
      // This is put above each page as a link to the list of annotated classes
    { return "Compound List"; }
    virtual QString trFileList()
      // This is put above each page as a link to the list of documented files
    { return "File List"; }
    virtual QString trHeaderFiles()
      // This is put above each page as a link to the list of all verbatim headers
    { return "Header Files"; }
    virtual QString trCompoundMembers()
      // This is put above each page as a link to all members of compounds.
    { return "Compound Members"; }
    virtual QString trFileMembers()
      // This is put above each page as a link to all members of files.
    { return "File Members"; }
    virtual QString trRelatedPages()
      // This is put above each page as a link to all related pages.
    { return "Related Pages"; }
    virtual QString trExamples()
      // This is put above each page as a link to all examples.
    { return "Examples"; }
    virtual QString trSearch()
      // This is put above each page as a link to the search engine.
    { return "Search"; }

    virtual QString trClassHierarchyDescription()
      // This is an introduction to the class hierarchy.
    { return "This inheritance list is sorted roughly, "
             "but not completely, alphabetically:";
    }
    virtual QString trFileListDescription(bool extractAll)
      // This is an introduction to the list with all files.
    {
      QString result="Here is a list of all ";
      if (!extractAll) result+="documented ";
      result+="files with brief descriptions:";
      return result;
    }
    virtual QString trCompoundListDescription()
      // This is an introduction to the annotated compound list
    { return "Here are the classes, structs and "
             "unions with brief descriptions:"; 
    }
    virtual QString trCompoundMembersDescription(bool extractAll)
      // This is an introduction to the page with all class members
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
      // This is an introduction to the page with all file members
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
      // This is an introduction to the page with the list of all header files
    { return "Here are the header files that make up the API:"; }
    virtual QString trExamplesDescription()
      // This is an introduction to the page with the list of all examples
    { return "Here is a list of all examples:"; }
    virtual QString trRelatedPagesDescription()
      // This is an introduction to the page with the list of related pages
    { return "Here is a list of all related documentation pages:"; }
    virtual QString trModulesDescription()
      // This is an introduction to the page with the list of class/file groups
    { return "Here is a list of all modules:"; }
    virtual QString trNoDescriptionAvailable()
      // This sentences is used in the annotated class/file lists if no brief
      // description is given.
    { return "No description available"; }
    
    // index titles (the project name is prepended for these) 
    virtual QString trDocumentation()
      // This is used in HTML as the title of index.html. 
    { return "Documentation"; }
    virtual QString trModuleIndex()
      // This is used in LaTeX as the title of the chapter with the 
      // index of all groups.
    { return "Module Index"; }
    virtual QString trHierarchicalIndex()
      // This is used in LaTeX as the title of the chapter with the 
      // class hierarchy.
    { return "Hierarchical Index"; }
    virtual QString trCompoundIndex()
      // This is used in LaTeX as the title of the chapter with the 
      // annotated compound index
    { return "Compound Index"; }
    virtual QString trFileIndex() 
      // This is used in LaTeX as the title of the chapter with the
      // list of all files.
    { return "File Index"; }
    virtual QString trModuleDocumentation()
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all groups.
    { return "Module Documentation"; }
    virtual QString trClassDocumentation()
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all classes, structs and unions.
    { return "Class Documentation"; }
    virtual QString trFileDocumentation()
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all files.
    { return "File Documentation"; }
    virtual QString trExampleDocumentation()
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all examples.
    { return "Example Documentation"; }
    virtual QString trPageDocumentation()
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all related pages.
    { return "Page Documentation"; }
    virtual QString trReferenceManual()
      // This is used in LaTeX as the title of the document
    { return "Reference Manual"; }
    
    virtual QString trDefines()
      // This is used in the documentation of a file as a header before the 
      // list of defines
    { return "Defines"; }
    virtual QString trFuncProtos()
      // This is used in the documentation of a file as a header before the 
      // list of function prototypes
    { return "Function Prototypes"; }
    virtual QString trTypedefs()
      // This is used in the documentation of a file as a header before the 
      // list of typedefs
    { return "Typedefs"; }
    virtual QString trEnumerations()
      // This is used in the documentation of a file as a header before the 
      // list of enumerations
    { return "Enumerations"; }
    virtual QString trFunctions()
      // This is used in the documentation of a file as a header before the 
      // list of (global) functions
    { return "Functions"; }
    virtual QString trVariables()
      // This is used in the documentation of a file as a header before the 
      // list of (global) variables
    { return "Variables"; }
    virtual QString trEnumerationValues()
      // This is used in the documentation of a file as a header before the 
      // list of (global) variables
    { return "Enumeration values"; }
    
    virtual QString trAuthor()
      // This is used in man pages as the author section.
    { return "Author"; }

    virtual QString trDefineDocumentation()
      // This is used in the documentation of a file before the list of
      // documentation blocks for defines
    { return "Define Documentation"; }
    virtual QString trFunctionPrototypeDocumentation()
      // This is used in the documentation of a file/namespace before the list 
      // of documentation blocks for function prototypes
    { return "Function Prototype Documentation"; }
    virtual QString trTypedefDocumentation()
      // This is used in the documentation of a file/namespace before the list 
      // of documentation blocks for typedefs
    { return "Typedef Documentation"; }
    virtual QString trEnumerationTypeDocumentation()
      // This is used in the documentation of a file/namespace before the list 
      // of documentation blocks for enumeration types
    { return "Enumeration Type Documentation"; }
    virtual QString trEnumerationValueDocumentation()
      // This is used in the documentation of a file/namespace before the list 
      // of documentation blocks for enumeration values
    { return "Enumeration Value Documentation"; }
    virtual QString trFunctionDocumentation()
      // This is used in the documentation of a file/namespace before the list 
      // of documentation blocks for functions
    { return "Function Documentation"; }
    virtual QString trVariableDocumentation()
      // This is used in the documentation of a file/namespace before the list 
      // of documentation blocks for variables
    { return "Variable Documentation"; }
    virtual QString trCompounds()
      // This is used in the documentation of a file/namespace/group before 
      // the list of links to documented compounds
    { return "Compounds"; }
    virtual QString trFiles()
      // This is used in the documentation of a group before the list of 
      // links to documented files
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
      // this text is put before a class diagram
    {
      return (QString)"Class diagram for "+clName;
    }
    
    virtual QString trForInternalUseOnly()
      // this text is generated when the \internal command is used.
    { return "For internal use only."; }
    virtual QString trReimplementedForInternalReasons()
      // this text is generated when the \reimp command is used.
    { return "Reimplemented for internal reasons; the API is not affected."; }
    virtual QString trWarning()
      // this text is generated when the \warning command is used.
    { return "Warning"; }
    virtual QString trBugsAndLimitations()
      // this text is generated when the \bug command is used.
    { return "Bugs and limitations"; }
    virtual QString trVersion()
      // this text is generated when the \version command is used.
    { return "Version"; }
    virtual QString trDate()
      // this text is generated when the \date command is used.
    { return "Date"; }
    virtual QString trAuthors()
      // this text is generated when the \author command is used.
    { return "Author(s)"; }
    virtual QString trReturns()
      // this text is generated when the \return command is used.
    { return "Returns"; }
    virtual QString trSeeAlso()
      // this text is generated when the \sa command is used.
    { return "See also"; }
    virtual QString trParameters()
      // this text is generated when the \param command is used.
    { return "Parameters"; }
    virtual QString trExceptions()
      // this text is generated when the \exception command is used.
    { return "Exceptions"; }
    
    virtual QString trGeneratedBy()
      // this text is used in the title page of a LaTeX document.
    { return "Generated by"; }

    // new since 0.49-990307
    
    virtual QString trNamespaceList()
      // used as the title of page containing all the index of all namespaces.
    { return "Namespace List"; }
    virtual QString trNamespaceListDescription(bool extractAll)
      // used as an introduction to the namespace list
    {
      QString result="Here is a list of all ";
      if (!extractAll) result+="documented ";
      result+="namespaces with brief descriptions:";
      return result;
    }
    virtual QString trFriends()
      // used in the class documentation as a header before the list of all
      // friends of a class
    { return "Friends"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
    virtual QString trRelatedFunctionDocumentation()
      // used in the class documentation as a header before the list of all
      // related classes
    { return "Friends And Related Function Documentation"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    virtual QString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType)
      // used as the title of the HTML page of a class/struct/union
    {
      QString result=(QString)clName+" ";
      switch(compType)
      {
        case ClassDef::Class:  result+=" Class"; break;
        case ClassDef::Struct: result+=" Struct"; break;
        case ClassDef::Union:  result+=" Union"; break;
      }
      result+=" Reference";
      return result;
    }
    virtual QString trFileReference(const char *fileName)
      // used as the title of the HTML page of a file
    {
      QString result=fileName;
      result+=" File Reference"; 
      return result;
    }
    virtual QString trNamespaceReference(const char *namespaceName)
      // used as the title of the HTML page of a namespace
    {
      QString result=namespaceName;
      result+=" Namespace Reference";
      return result;
    }
    
    // these are for the member sections of a class, struct or union 
    virtual QString trPublicMembers()
    { return "Public Members"; }
    virtual QString trPublicSlots()
    { return "Public Slots"; }
    virtual QString trSignals()
    { return "Signals"; }
    virtual QString trStaticPublicMembers()
    { return "Static Public Members"; }
    virtual QString trProtectedMembers()
    { return "Protected Members"; }
    virtual QString trProtectedSlots()
    { return "Protected Slots"; }
    virtual QString trStaticProtectedMembers()
    { return "Static Protected Members"; }
    virtual QString trPrivateMembers()
    { return "Private Members"; }
    virtual QString trPrivateSlots()
    { return "Private Slots"; }
    virtual QString trStaticPrivateMembers()
    { return "Static Private Members"; }
    // end of member sections 
    
    virtual QString trWriteList(int numEntries)
    {
      // this function is used to produce a comma-separated list of items.
      // use generateMarker(i) to indicate where item i should be put.
      QString result;
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
            result+=", and ";
        }
      }
      return result; 
    }
    
    virtual QString trInheritsList(int numEntries)
      // used in class documentation to produce a list of base classes,
      // if class diagrams are disabled.
    {
      return "Inherits "+trWriteList(numEntries)+".";
    }
    virtual QString trInheritedByList(int numEntries)
      // used in class documentation to produce a list of super classes,
      // if class diagrams are disabled.
    {
      return "Inherited by "+trWriteList(numEntries)+".";
    }
    virtual QString trReimplementedFromList(int numEntries)
      // used in member documentation blocks to produce a list of 
      // members that are hidden by this one.
    {
      return "Reimplemented from "+trWriteList(numEntries)+".";
    }
    virtual QString trReimplementedInList(int numEntries)
    {
      // used in member documentation blocks to produce a list of
      // all member that overwrite the implementation of this member.
      return "Reimplemented in "+trWriteList(numEntries)+".";
    }

    virtual QString trNamespaceMembers()
      // This is put above each page as a link to all members of namespaces.
    { return "Namespace Members"; }
    virtual QString trNamespaceMemberDescription(bool extractAll)
      // This is an introduction to the page with all namespace members
    { 
      QString result="Here is a list of all ";
      if (!extractAll) result+="documented ";
      result+="namespace members with links to ";
      if (extractAll) 
        result+="the namespace documentation for each member:";
      else 
        result+="the namespaces they belong to:";
      return result;
    }
    virtual QString trNamespaceIndex()
      // This is used in LaTeX as the title of the chapter with the 
      // index of all namespaces.
    { return "Namespace Index"; }
    virtual QString trNamespaceDocumentation()
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all namespaces.
    { return "Namespace Documentation"; }
};

#endif
