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

#include "qtbc.h"
#include "classdef.h"
#include "util.h"

class Translator
{
  public:
    //--------------------------------------------------------------------
    // NOTICE:
    // the following functions are now obsolete: these are no longer used and 
    // will disappear in future versions. You do not have to translate them!
    virtual QCString trInherits() 
    { return "Inherits"; }
    virtual QCString trAnd() 
    { return "and"; }
    virtual QCString trInheritedBy() 
    { return "Inherited By"; }
    virtual QCString trReference() 
    { return "Reference"; }
    virtual QCString trReimplementedFrom()
    { return "Reimplemented from"; }
    virtual QCString trReimplementedIn()
    { return "Reimplemented in"; }
    virtual QCString trIncludeFile()
    { return "Include File"; }
    // end of obsolete functions 
    //--------------------------------------------------------------------

    virtual QCString latexBabelPackage() 
      // returns the name of the package that is included by LaTeX
    { return ""; }

    virtual QCString trRelatedFunctions()
      // used in the compound documentation before a list of related functions.
    { return "Related Functions"; }

    virtual QCString trRelatedSubscript()
      // subscript for the related functions.
    { return "(Note that these are not member functions.)"; }

    virtual QCString trDetailedDescription()
      // header that is put before the detailed description of files, classes and namespaces.
    { return "Detailed Description"; }

    virtual QCString trMemberTypedefDocumentation()
      // header that is put before the list of typedefs.
    { return "Member Typedef Documentation"; }
    
    virtual QCString trMemberEnumerationDocumentation()
      // header that is put before the list of enumerations.
    { return "Member Enumeration Documentation"; }
    
    virtual QCString trMemberFunctionDocumentation()
      // header that is put before the list of member functions.
    { return "Member Function Documentation"; }
    
    virtual QCString trMemberDataDocumentation()
      // header that is put before the list of member attributes.
    { return "Member Data Documentation"; }
    
    virtual QCString trGeneratedFrom(const char *s,bool single)
    { // here s is one of " Class", " Struct" or " Union"
      // single is true implies a single file
      QCString result=(QCString)"The documentation for this"+s+
                     " was generated from the following file";
      if (single) result+=":"; else result+="s:";
      return result;
    }

    virtual QCString trMore() 
      // this is the text of a link put after brief descriptions.
    { return "More..."; }

    virtual QCString trListOfAllMembers()
      // put in the class documentation
    { return "List of all members."; }

    virtual QCString trMemberList()
      // used as the title of the "list of all members" page of a class
    { return "Member List"; }

    virtual QCString trThisIsTheListOfAllMembers()
      // this is the first part of a sentence that is followed by a class name
    { return "This is the complete list of members for "; }
    virtual QCString trIncludingInheritedMembers()
      // this is the remainder of the sentence after the class name
    { return ", including all inherited members."; }
    
    virtual QCString trGeneratedAutomatically(const char *s)
      // this is put at the author sections at the bottom of man pages.
      // parameter s is name of the project name.
    { QCString result="Generated automatically by Doxygen";
      if (s) result+=(QCString)" for "+s;
      result+=" from the source code."; 
      return result;
    }

    virtual QCString trEnumName()
      // put after an enum name in the list of all members
    { return "enum name"; }
    
    virtual QCString trEnumValue()
      // put after an enum value in the list of all members
    { return "enum value"; }
    
    virtual QCString trDefinedIn()
      // put after an undocumented member in the list of all members
    { return "defined in"; }

    virtual QCString trVerbatimText(const char *f)
      // put as in introduction in the verbatim header file of a class.
      // parameter f is the name of the include file.
    { return (QCString)"This is the verbatim text of the "+f+" include file."; }
    
    // quick reference sections
    virtual QCString trModules()
      // This is put above each page as a link to the list of all groups of 
      // compounds or files (see the \group command).
    { return "Modules"; }
    virtual QCString trClassHierarchy()
      // This is put above each page as a link to the class hierarchy 
    { return "Class Hierarchy"; }
    virtual QCString trCompoundList()
      // This is put above each page as a link to the list of annotated classes
    { return "Compound List"; }
    virtual QCString trFileList()
      // This is put above each page as a link to the list of documented files
    { return "File List"; }
    virtual QCString trHeaderFiles()
      // This is put above each page as a link to the list of all verbatim headers
    { return "Header Files"; }
    virtual QCString trCompoundMembers()
      // This is put above each page as a link to all members of compounds.
    { return "Compound Members"; }
    virtual QCString trFileMembers()
      // This is put above each page as a link to all members of files.
    { return "File Members"; }
    virtual QCString trRelatedPages()
      // This is put above each page as a link to all related pages.
    { return "Related Pages"; }
    virtual QCString trExamples()
      // This is put above each page as a link to all examples.
    { return "Examples"; }
    virtual QCString trSearch()
      // This is put above each page as a link to the search engine.
    { return "Search"; }

    virtual QCString trClassHierarchyDescription()
      // This is an introduction to the class hierarchy.
    { return "This inheritance list is sorted roughly, "
             "but not completely, alphabetically:";
    }
    virtual QCString trFileListDescription(bool extractAll)
      // This is an introduction to the list with all files.
    {
      QCString result="Here is a list of all ";
      if (!extractAll) result+="documented ";
      result+="files with brief descriptions:";
      return result;
    }
    virtual QCString trCompoundListDescription()
      // This is an introduction to the annotated compound list
    { return "Here are the classes, structs and "
             "unions with brief descriptions:"; 
    }
    virtual QCString trCompoundMembersDescription(bool extractAll)
      // This is an introduction to the page with all class members
    {
      QCString result="Here is a list of all ";
      if (!extractAll) result+="documented ";
      result+="class members with links to ";
      if (extractAll) 
        result+="the class documentation for each member:";
      else 
        result+="the classes they belong to:";
      return result;
    }
    virtual QCString trFileMembersDescription(bool extractAll)
      // This is an introduction to the page with all file members
    {
      QCString result="Here is a list of all ";
      if (!extractAll) result+="documented ";
      result+="file members with links to ";
      if (extractAll) 
        result+="the file documentation for each member:";
      else 
        result+="the files they belong to:";
      return result;
    }
    virtual QCString trHeaderFilesDescription()
      // This is an introduction to the page with the list of all header files
    { return "Here are the header files that make up the API:"; }
    virtual QCString trExamplesDescription()
      // This is an introduction to the page with the list of all examples
    { return "Here is a list of all examples:"; }
    virtual QCString trRelatedPagesDescription()
      // This is an introduction to the page with the list of related pages
    { return "Here is a list of all related documentation pages:"; }
    virtual QCString trModulesDescription()
      // This is an introduction to the page with the list of class/file groups
    { return "Here is a list of all modules:"; }
    virtual QCString trNoDescriptionAvailable()
      // This sentences is used in the annotated class/file lists if no brief
      // description is given.
    { return "No description available"; }
    
    // index titles (the project name is prepended for these) 
    virtual QCString trDocumentation()
      // This is used in HTML as the title of index.html. 
    { return "Documentation"; }
    virtual QCString trModuleIndex()
      // This is used in LaTeX as the title of the chapter with the 
      // index of all groups.
    { return "Module Index"; }
    virtual QCString trHierarchicalIndex()
      // This is used in LaTeX as the title of the chapter with the 
      // class hierarchy.
    { return "Hierarchical Index"; }
    virtual QCString trCompoundIndex()
      // This is used in LaTeX as the title of the chapter with the 
      // annotated compound index
    { return "Compound Index"; }
    virtual QCString trFileIndex() 
      // This is used in LaTeX as the title of the chapter with the
      // list of all files.
    { return "File Index"; }
    virtual QCString trModuleDocumentation()
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all groups.
    { return "Module Documentation"; }
    virtual QCString trClassDocumentation()
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all classes, structs and unions.
    { return "Class Documentation"; }
    virtual QCString trFileDocumentation()
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all files.
    { return "File Documentation"; }
    virtual QCString trExampleDocumentation()
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all examples.
    { return "Example Documentation"; }
    virtual QCString trPageDocumentation()
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all related pages.
    { return "Page Documentation"; }
    virtual QCString trReferenceManual()
      // This is used in LaTeX as the title of the document
    { return "Reference Manual"; }
    
    virtual QCString trDefines()
      // This is used in the documentation of a file as a header before the 
      // list of defines
    { return "Defines"; }
    virtual QCString trFuncProtos()
      // This is used in the documentation of a file as a header before the 
      // list of function prototypes
    { return "Function Prototypes"; }
    virtual QCString trTypedefs()
      // This is used in the documentation of a file as a header before the 
      // list of typedefs
    { return "Typedefs"; }
    virtual QCString trEnumerations()
      // This is used in the documentation of a file as a header before the 
      // list of enumerations
    { return "Enumerations"; }
    virtual QCString trFunctions()
      // This is used in the documentation of a file as a header before the 
      // list of (global) functions
    { return "Functions"; }
    virtual QCString trVariables()
      // This is used in the documentation of a file as a header before the 
      // list of (global) variables
    { return "Variables"; }
    virtual QCString trEnumerationValues()
      // This is used in the documentation of a file as a header before the 
      // list of (global) variables
    { return "Enumeration values"; }
    
    virtual QCString trAuthor()
      // This is used in man pages as the author section.
    { return "Author"; }

    virtual QCString trDefineDocumentation()
      // This is used in the documentation of a file before the list of
      // documentation blocks for defines
    { return "Define Documentation"; }
    virtual QCString trFunctionPrototypeDocumentation()
      // This is used in the documentation of a file/namespace before the list 
      // of documentation blocks for function prototypes
    { return "Function Prototype Documentation"; }
    virtual QCString trTypedefDocumentation()
      // This is used in the documentation of a file/namespace before the list 
      // of documentation blocks for typedefs
    { return "Typedef Documentation"; }
    virtual QCString trEnumerationTypeDocumentation()
      // This is used in the documentation of a file/namespace before the list 
      // of documentation blocks for enumeration types
    { return "Enumeration Type Documentation"; }
    virtual QCString trEnumerationValueDocumentation()
      // This is used in the documentation of a file/namespace before the list 
      // of documentation blocks for enumeration values
    { return "Enumeration Value Documentation"; }
    virtual QCString trFunctionDocumentation()
      // This is used in the documentation of a file/namespace before the list 
      // of documentation blocks for functions
    { return "Function Documentation"; }
    virtual QCString trVariableDocumentation()
      // This is used in the documentation of a file/namespace before the list 
      // of documentation blocks for variables
    { return "Variable Documentation"; }
    virtual QCString trCompounds()
      // This is used in the documentation of a file/namespace/group before 
      // the list of links to documented compounds
    { return "Compounds"; }
    virtual QCString trFiles()
      // This is used in the documentation of a group before the list of 
      // links to documented files
    { return "Files"; }

    virtual QCString trGeneratedAt(const char *date,const char *projName)
    { 
      QCString result=(QCString)"Generated at "+date;
      if (projName) result+=(QCString)" for "+projName;
      result+=(QCString)" by";
      return result;
    }
    virtual QCString trWrittenBy()
    {
      return "written by";
    }

    virtual QCString trClassDiagram(const char *clName)
      // this text is put before a class diagram
    {
      return (QCString)"Class diagram for "+clName;
    }
    
    virtual QCString trForInternalUseOnly()
      // this text is generated when the \internal command is used.
    { return "For internal use only."; }
    virtual QCString trReimplementedForInternalReasons()
      // this text is generated when the \reimp command is used.
    { return "Reimplemented for internal reasons; the API is not affected."; }
    virtual QCString trWarning()
      // this text is generated when the \warning command is used.
    { return "Warning"; }
    virtual QCString trBugsAndLimitations()
      // this text is generated when the \bug command is used.
    { return "Bugs and limitations"; }
    virtual QCString trVersion()
      // this text is generated when the \version command is used.
    { return "Version"; }
    virtual QCString trDate()
      // this text is generated when the \date command is used.
    { return "Date"; }
    virtual QCString trAuthors()
      // this text is generated when the \author command is used.
    { return "Author(s)"; }
    virtual QCString trReturns()
      // this text is generated when the \return command is used.
    { return "Returns"; }
    virtual QCString trSeeAlso()
      // this text is generated when the \sa command is used.
    { return "See also"; }
    virtual QCString trParameters()
      // this text is generated when the \param command is used.
    { return "Parameters"; }
    virtual QCString trExceptions()
      // this text is generated when the \exception command is used.
    { return "Exceptions"; }
    
    virtual QCString trGeneratedBy()
      // this text is used in the title page of a LaTeX document.
    { return "Generated by"; }

    // new since 0.49-990307
    
    virtual QCString trNamespaceList()
      // used as the title of page containing all the index of all namespaces.
    { return "Namespace List"; }
    virtual QCString trNamespaceListDescription(bool extractAll)
      // used as an introduction to the namespace list
    {
      QCString result="Here is a list of all ";
      if (!extractAll) result+="documented ";
      result+="namespaces with brief descriptions:";
      return result;
    }
    virtual QCString trFriends()
      // used in the class documentation as a header before the list of all
      // friends of a class
    { return "Friends"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990405
//////////////////////////////////////////////////////////////////////////
    
    virtual QCString trRelatedFunctionDocumentation()
      // used in the class documentation as a header before the list of all
      // related classes
    { return "Friends And Related Function Documentation"; }
    
//////////////////////////////////////////////////////////////////////////
// new since 0.49-990425
//////////////////////////////////////////////////////////////////////////

    virtual QCString trCompoundReference(const char *clName,
                                    ClassDef::CompoundType compType)
      // used as the title of the HTML page of a class/struct/union
    {
      QCString result=(QCString)clName+" ";
      switch(compType)
      {
        case ClassDef::Class:  result+=" Class"; break;
        case ClassDef::Struct: result+=" Struct"; break;
        case ClassDef::Union:  result+=" Union"; break;
      }
      result+=" Reference";
      return result;
    }
    virtual QCString trFileReference(const char *fileName)
      // used as the title of the HTML page of a file
    {
      QCString result=fileName;
      result+=" File Reference"; 
      return result;
    }
    virtual QCString trNamespaceReference(const char *namespaceName)
      // used as the title of the HTML page of a namespace
    {
      QCString result=namespaceName;
      result+=" Namespace Reference";
      return result;
    }
    
    // these are for the member sections of a class, struct or union 
    virtual QCString trPublicMembers()
    { return "Public Members"; }
    virtual QCString trPublicSlots()
    { return "Public Slots"; }
    virtual QCString trSignals()
    { return "Signals"; }
    virtual QCString trStaticPublicMembers()
    { return "Static Public Members"; }
    virtual QCString trProtectedMembers()
    { return "Protected Members"; }
    virtual QCString trProtectedSlots()
    { return "Protected Slots"; }
    virtual QCString trStaticProtectedMembers()
    { return "Static Protected Members"; }
    virtual QCString trPrivateMembers()
    { return "Private Members"; }
    virtual QCString trPrivateSlots()
    { return "Private Slots"; }
    virtual QCString trStaticPrivateMembers()
    { return "Static Private Members"; }
    // end of member sections 
    
    virtual QCString trWriteList(int numEntries)
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
            result+=", and ";
        }
      }
      return result; 
    }
    
    virtual QCString trInheritsList(int numEntries)
      // used in class documentation to produce a list of base classes,
      // if class diagrams are disabled.
    {
      return "Inherits "+trWriteList(numEntries)+".";
    }
    virtual QCString trInheritedByList(int numEntries)
      // used in class documentation to produce a list of super classes,
      // if class diagrams are disabled.
    {
      return "Inherited by "+trWriteList(numEntries)+".";
    }
    virtual QCString trReimplementedFromList(int numEntries)
      // used in member documentation blocks to produce a list of 
      // members that are hidden by this one.
    {
      return "Reimplemented from "+trWriteList(numEntries)+".";
    }
    virtual QCString trReimplementedInList(int numEntries)
    {
      // used in member documentation blocks to produce a list of
      // all member that overwrite the implementation of this member.
      return "Reimplemented in "+trWriteList(numEntries)+".";
    }

    virtual QCString trNamespaceMembers()
      // This is put above each page as a link to all members of namespaces.
    { return "Namespace Members"; }
    virtual QCString trNamespaceMemberDescription(bool extractAll)
      // This is an introduction to the page with all namespace members
    { 
      QCString result="Here is a list of all ";
      if (!extractAll) result+="documented ";
      result+="namespace members with links to ";
      if (extractAll) 
        result+="the namespace documentation for each member:";
      else 
        result+="the namespaces they belong to:";
      return result;
    }
    virtual QCString trNamespaceIndex()
      // This is used in LaTeX as the title of the chapter with the 
      // index of all namespaces.
    { return "Namespace Index"; }
    virtual QCString trNamespaceDocumentation()
      // This is used in LaTeX as the title of the chapter containing
      // the documentation of all namespaces.
    { return "Namespace Documentation"; }

//////////////////////////////////////////////////////////////////////////
// new since 0.49-990522
//////////////////////////////////////////////////////////////////////////

      // This is used in the documentation before the list of all
      // namespaces in a file.
    virtual QCString trNamespaces()
    { return "Namespaces"; }
};

#endif
