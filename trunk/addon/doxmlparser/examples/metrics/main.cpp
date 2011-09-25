/******************************************************************************
 *
 * $Id$
 *
 *
 * Copyright (C) 1997-2006 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

/*! \mainpage Metrics
 *  This is a small example that shows how to use doxygen's XML output and
 *  the doxmlparser library. The example shows some very basic code metrics.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <doxmlintf.h>

bool isDocumented(IDocRoot *brief,IDocRoot *detailed)
{
  bool found=false;
  if (brief)
  {
    IDocIterator *docIt = brief->contents();
    if (docIt->current()) // method has brief description
    {
      found=true;
    }
    docIt->release();
  }
  if (detailed && !found)
  {
    IDocIterator *docIt = detailed->contents();
    if (docIt->current())
    {
      found=true;
    }
    docIt->release();
  }
  return found;
}

int main(int argc,char **argv)
{
  if (argc!=2)
  {
    printf("Usage: %s xml_output_dir\n",argv[0]);
    exit(1);
  }

  int numClasses=0;
  int numDocClasses=0;
  int numStructs=0;
  int numUnions=0;
  int numInterfaces=0;
  int numExceptions=0;
  int numNamespaces=0;
  int numFiles=0;
  int numGroups=0;
  int numPages=0;
  int numPackages=0;
  int numPubMethods=0;
  int numProMethods=0;
  int numPriMethods=0;
  int numDocPubMethods=0;
  int numDocProMethods=0;
  int numDocPriMethods=0;
  int numFunctions=0;
  int numAttributes=0;
  int numVariables=0;
  int numDocFunctions=0;
  int numDocAttributes=0;
  int numDocVariables=0;
  int numParams=0;
  
  IDoxygen *dox = createObjectModel();

  dox->setDebugLevel(0);

  if (!dox->readXMLDir(argv[1]))
  {
    printf("Error reading %s/index.xml\n",argv[1]);
    exit(1);
  }

  ICompoundIterator *cli = dox->compounds();
  ICompound *comp;
  for (cli->toFirst();(comp=cli->current());cli->toNext())
  {
    printf("Processing %s...\n",comp->name()->latin1());
    bool hasDocs = isDocumented(comp->briefDescription(),comp->detailedDescription());
    switch (comp->kind())
    {
      case ICompound::Class:      
        numClasses++;    
        if (hasDocs) numDocClasses++;
        break;
      case ICompound::Struct:     numStructs++;    break;
      case ICompound::Union:      numUnions++;     break;
      case ICompound::Interface:  numInterfaces++; break;
      case ICompound::Exception:  numExceptions++; break;
      case ICompound::Namespace:  numNamespaces++; break;
      case ICompound::File:       numFiles++;      break;
      case ICompound::Group:      numGroups++;     break;
      case ICompound::Page:       numPages++;      break;
      default: break;
    }
    
    ISectionIterator *sli = comp->sections();
    ISection *sec;
    for (sli->toFirst();(sec=sli->current());sli->toNext())
    {
      IMemberIterator *mli = sec->members();
      IMember *mem;
      for (mli->toFirst();(mem=mli->current());mli->toNext())
      {
        IParamIterator *pli = mem->parameters();
        IParam *par;
        if (comp->kind()==ICompound::Class || 
            comp->kind()==ICompound::Struct ||
            comp->kind()==ICompound::Interface
           )
        {
          if (mem->kind()==IMember::Function ||
              mem->kind()==IMember::Prototype ||
              mem->kind()==IMember::Signal ||
              mem->kind()==IMember::Slot ||
              mem->kind()==IMember::DCOP
             ) // is a "method"
          {
            if (mem->section()->isPublic())
            {
              numPubMethods++;
              if (isDocumented(mem->briefDescription(),mem->detailedDescription()))
              {
                numDocPubMethods++;
              }
            }
            else if (mem->section()->isProtected())
            {
              numProMethods++;
              if (isDocumented(mem->briefDescription(),mem->detailedDescription()))
              {
                numDocProMethods++;
              }
            }
            else if (mem->section()->isPrivate())
            {
              numPriMethods++;
              if (isDocumented(mem->briefDescription(),mem->detailedDescription()))
              {
                numDocPriMethods++;
              }
            }
          }
          else if (mem->kind()==IMember::Variable || 
                   mem->kind()==IMember::Property
                  ) // is an "attribute"
          {
            numAttributes++;
            if (isDocumented(mem->briefDescription(),mem->detailedDescription()))
            {
              numDocAttributes++;
            }
          }
        }
        else if (comp->kind()==ICompound::File ||
                 comp->kind()==ICompound::Namespace
                )
        {
          if (mem->kind()==IMember::Function ||
              mem->kind()==IMember::Prototype ||
              mem->kind()==IMember::Signal ||
              mem->kind()==IMember::Slot ||
              mem->kind()==IMember::DCOP
             ) // is a "method"
          {
            numFunctions++;
            if (isDocumented(mem->briefDescription(),mem->detailedDescription()))
            {
              numDocFunctions++;
            }
          }
          else if (mem->kind()==IMember::Variable || 
                   mem->kind()==IMember::Property
                  ) // is an "attribute"
          {
            numVariables++;
            if (isDocumented(mem->briefDescription(),mem->detailedDescription()))
            {
              numDocVariables++;
            }
          }
        }
        
        for (pli->toFirst();(par=pli->current());pli->toNext())
        {
          numParams++;
        }
        const char *type = mem->typeString()->latin1();
        if (type && strcmp(type, "void"))
        {
          numParams++; // count non-void return types as well
        }
        pli->release();
      }
      mli->release();
    }
    sli->release();

    comp->release();
  }
  cli->release();

  dox->release();

  int numMethods    = numPubMethods+numProMethods+numPriMethods;
  int numDocMethods = numDocPubMethods+numDocProMethods+numDocPriMethods;
  
  printf("Metrics:\n");
  printf("-----------------------------------\n");
  if (numClasses>0)    printf("Classes:     %10d (%d documented)\n",numClasses,numDocClasses);
  if (numStructs>0)    printf("Structs:     %10d\n",numStructs);
  if (numUnions>0)     printf("Unions:      %10d\n",numUnions);
  if (numInterfaces>0) printf("Interfaces:  %10d\n",numInterfaces);
  if (numExceptions>0) printf("Exceptions:  %10d\n",numExceptions);
  if (numNamespaces>0) printf("Namespaces:  %10d\n",numNamespaces);
  if (numFiles>0)      printf("Files:       %10d\n",numFiles);
  if (numGroups>0)     printf("Groups:      %10d\n",numGroups);
  if (numPages>0)      printf("Pages:       %10d\n",numPages);
  if (numPackages>0)   printf("Packages:    %10d\n",numPackages);
  if (numMethods>0)    printf("Methods:     %10d (%d documented)\n",numMethods,numDocMethods);
  if (numPubMethods>0) printf("  Public:    %10d (%d documented)\n",numPubMethods,numDocPubMethods);
  if (numProMethods>0) printf("  Protected: %10d (%d documented)\n",numProMethods,numDocProMethods);
  if (numPriMethods>0) printf("  Private:   %10d (%d documented)\n",numPriMethods,numDocPriMethods);
  if (numFunctions>0)  printf("Functions:   %10d (%d documented)\n",numFunctions,numDocFunctions);
  if (numAttributes>0) printf("Attributes:  %10d (%d documented)\n",numAttributes,numDocAttributes);
  if (numVariables>0)  printf("Variables:   %10d (%d documented)\n",numVariables,numDocVariables);
  if (numParams>0)     printf("Params:      %10d\n",numParams);
  printf("-----------------------------------\n");
  if (numClasses>0)    printf("Avg. #methods/compound:  %10f\n",(double)numMethods/(double)numClasses);
  if (numMethods>0)    printf("Avg. #params/method:     %10f\n",(double)numParams/(double)numMethods);
  printf("-----------------------------------\n");

  return 0;
}

