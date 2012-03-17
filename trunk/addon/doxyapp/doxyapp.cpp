/******************************************************************************
 *
 * Copyright (C) 1997-2012 by Dimitri van Heesch.
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

/** @file
 *  @brief Example of how to use doxygen as part of another GPL applications
 *
 *  This example shows how to configure and run doxygen programmatically from
 *  within an application without generating the usual output.
 *  The example should work on any Unix like OS (including Linux and Mac OS X).
 *  
 *  This example shows how to use to code parser to get cross-references information
 *  and it also shows how to look up symbols in a program parsed by doxygen and
 *  show some information about them.
 */

#include <stdlib.h>
#include <unistd.h>
#include "doxygen.h"
#include "outputgen.h"
#include "parserintf.h"

class XRefDummyCodeGenerator : public CodeOutputInterface
{
  public:
    XRefDummyCodeGenerator(FileDef *fd) : m_fd(fd) {}
   ~XRefDummyCodeGenerator() {}

    // these are just null functions, they can be used to produce a syntax highlighted
    // and cross-linked version of the source code, but who needs that anyway ;-)
    void codify(const char *) {}
    void writeCodeLink(const char *,const char *,const char *,const char *,const char *)  {}
    void startCodeLine() {}
    void endCodeLine() {}
    void startCodeAnchor(const char *) {}
    void endCodeAnchor() {}
    void startFontClass(const char *) {}
    void endFontClass() {}
    void writeCodeAnchor(const char *) {}
    void writeLineNumber(const char *,const char *,const char *,int) {}

    // here we are presented with the symbols found by the code parser
    void linkableSymbol(int l, const char *sym,Definition *symDef,Definition *context) 
    {
      QCString ctx;
      if (context) // the context of the symbol is known
      {
        if (context->definitionType()==Definition::TypeMember) // it is inside a member
        {
          Definition *parentContext = context->getOuterScope();
          if (parentContext && parentContext->definitionType()==Definition::TypeClass)
             // it is inside a member of a class
          {
            ctx.sprintf("inside %s %s of %s %s",
              ((MemberDef *)context)->memberTypeName().data(),
              context->name().data(),
              ((ClassDef*)parentContext)->compoundTypeString().data(),
              parentContext->name().data());
          }
          else if (parentContext==Doxygen::globalScope) // it is inside a global member
          {
            ctx.sprintf("inside %s %s",
              ((MemberDef *)context)->memberTypeName().data(),
              context->name().data());
          }
        }
        if (ctx.isEmpty()) // it is something else (class, or namespace member, ...)
        {
          ctx.sprintf("in %s",context->name().data());
        }
      }
      printf("Found symbol %s at line %d of %s %s\n",
          sym,l,m_fd->getDefFileName().data(),ctx.data());
      if (symDef && context) // in this case the definition of the symbol is
        // known to doxygen.
      {
        printf("-> defined at line %d of %s\n",
            symDef->getDefLine(),symDef->getDefFileName().data());
      }
    }
  private:
    FileDef *m_fd;
};

static void findXRefSymbols(FileDef *fd)
{
  // get the interface to a parser that matches the file extension
  ParserInterface *pIntf=Doxygen::parserManager->getParser(fd->getDefFileExtension());

  // reset the parsers state
  pIntf->resetCodeParserState();

  // create a new backend object 
  XRefDummyCodeGenerator *xrefGen = new XRefDummyCodeGenerator(fd);

  // parse the source code
  pIntf->parseCode(*xrefGen,
                0,
                fileToString(fd->absFilePath()),
                FALSE,
                0,
                fd);

  // dismiss the object.
  delete xrefGen;
}

static void listSymbol(Definition *d)
{
  if (d!=Doxygen::globalScope && // skip the global namespace symbol
      d->name().at(0)!='@'       // skip anonymous stuff
     )      
  {
    printf("%s\n",
        d->name().data());
  }
}

static void listSymbols()
{
  QDictIterator<DefinitionIntf> sli(*Doxygen::symbolMap);
  DefinitionIntf *di;
  for (sli.toFirst();(di=sli.current());++sli)
  {
    if (di->definitionType()==DefinitionIntf::TypeSymbolList) // list of symbols
      // with same name
    {
      DefinitionListIterator dli(*(DefinitionList*)di);
      Definition *d;
      // for each symbol
      for (dli.toFirst();(d=dli.current());++dli)
      {
        listSymbol(d);
      }
    }
    else // single symbol
    {
      listSymbol((Definition*)di);
    }
  }
}

static void lookupSymbol(Definition *d)
{
  if (d!=Doxygen::globalScope && // skip the global namespace symbol
      d->name().at(0)!='@'       // skip anonymous stuff
     )      
  {
    printf("Symbol info\n");
    printf("-----------\n");
    printf("Name: %s\n",d->name().data());
    printf("File: %s\n",d->getDefFileName().data());
    printf("Line: %d\n",d->getDefLine());
    // depending on the definition type we can case to the appropriate
    // derived to get additional information
    switch (d->definitionType())
    {
      case Definition::TypeClass:
        {
          ClassDef *cd = (ClassDef *)d;
          printf("Kind: %s\n",cd->compoundTypeString().data());
        }
        break;
      case Definition::TypeFile:
        {
          FileDef *fd = (FileDef *)d;
          printf("Kind: File: #includes %d other files\n",
              fd->includeFileList() ? fd->includeFileList()->count() : 0);
        }
        break;
      case Definition::TypeNamespace:
        {
          NamespaceDef *nd = (NamespaceDef *)d;
          printf("Kind: Namespace: contains %d classes and %d namespaces\n",
              nd->getClassSDict() ? nd->getClassSDict()->count() : 0,
              nd->getNamespaceSDict() ? nd->getNamespaceSDict()->count() : 0);
        }
        break;
      case Definition::TypeMember:
        {
          MemberDef *md = (MemberDef *)d;
          printf("Kind: %s\n",md->memberTypeName().data());
        }
        break;
      default:
        // ignore groups/pages/packages/dirs for now
        break;
    }
  }
}

static void lookupSymbols(const QCString &sym)
{
  if (!sym.isEmpty())
  {
    DefinitionIntf *di = Doxygen::symbolMap->find(sym);
    if (di)
    {
      if (di->definitionType()==DefinitionIntf::TypeSymbolList)
      {
        DefinitionListIterator dli(*(DefinitionList*)di);
        Definition *d;
        // for each symbol with the given name
        for (dli.toFirst();(d=dli.current());++dli)
        {
          lookupSymbol(d);
        }
      }
      else
      {
        lookupSymbol((Definition*)di);
      }
    }
    else
    {
      printf("Unknown symbol\n");
    }
  }
}

int main(int argc,char **argv)
{
  char cmd[256];

  if (argc<2)
  {
    printf("Usage: %s [source_file | source_dir]\n",argv[0]);
    exit(1);
  }

  // initialize data structures 
  initDoxygen();

  // setup the non-default configuration options

  // we need a place to put intermediate files
  Config_getString("OUTPUT_DIRECTORY")="/tmp/doxygen"; 
  // disable html output
  Config_getBool("GENERATE_HTML")=FALSE;
  // disable latex output
  Config_getBool("GENERATE_LATEX")=FALSE;
  // be quiet
  Config_getBool("QUIET")=TRUE;
  // turn off warnings
  Config_getBool("WARNINGS")=FALSE;
  Config_getBool("WARN_IF_UNDOCUMENTED")=FALSE;
  Config_getBool("WARN_IF_DOC_ERROR")=FALSE;
  // Extract as much as possible
  Config_getBool("EXTRACT_ALL")=TRUE;
  Config_getBool("EXTRACT_STATIC")=TRUE;
  Config_getBool("EXTRACT_PRIVATE")=TRUE;
  Config_getBool("EXTRACT_LOCAL_METHODS")=TRUE;
  // Extract source browse information, needed 
  // to make doxygen gather the cross reference info
  Config_getBool("SOURCE_BROWSER")=TRUE;

  // set the input
  Config_getList("INPUT").append(argv[1]);

  // check and finialize the configuration
  checkConfiguration();
  adjustConfiguration();

  // parse the files
  parseInput();

  // iterate over the input files
  FileNameListIterator fnli(*Doxygen::inputNameList); 
  FileName *fn;
  // foreach file with a certain name
  for (fnli.toFirst();(fn=fnli.current());++fnli)
  {
    FileNameIterator fni(*fn);
    FileDef *fd;
    // for each file definition
    for (;(fd=fni.current());++fni)
    {
      // get the references (linked and unlinked) found in this file
      findXRefSymbols(fd);
    }
  }

  // remove temporary files
  if (!Doxygen::objDBFileName.isEmpty()) unlink(Doxygen::objDBFileName);
  if (!Doxygen::entryDBFileName.isEmpty()) unlink(Doxygen::entryDBFileName);
  // clean up after us
  rmdir("/tmp/doxygen");

  while (1)
  {
    printf("> Type a symbol name or\n> .list for a list of symbols or\n> .quit to exit\n> ");
    fgets(cmd,256,stdin);
    QCString s(cmd);
    if (s.at(s.length()-1)=='\n') s=s.left(s.length()-1); // strip trailing \n
    if (s==".list") 
      listSymbols();
    else if (s==".quit") 
      exit(0);
    else 
      lookupSymbols(s);
  }
}

