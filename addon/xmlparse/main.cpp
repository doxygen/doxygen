/******************************************************************************
 *
 * $Id$
 *
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#include <stdio.h>
#include "doxmlintf.h"


int main(int argc,char **argv)
{
  if (argc!=2)
  {
    printf("Usage: %s file.xml\n",argv[0]);
    exit(1);
  }

  IDoxygen *dox = createObjectModelFromXML(argv[1]);

  {
    QListIterator<ICompound> cli(dox->getCompoundIterator());
    ICompound *comp;
    printf("--- compound list ---------\n");
    for (cli.toFirst();(comp=cli.current());++cli)
    {
      printf("Compound name=%s id=%s kind=%s\n",
          comp->name().data(),comp->id().data(),comp->kind().data());
      QListIterator<ISection> sli(comp->getSectionIterator());
      ISection *sec;
      for (sli.toFirst();(sec=sli.current());++sli)
      {
        printf("  Section kind=%s\n",sec->kind().data());
        QListIterator<IMember> mli(sec->getMemberIterator());
        IMember *mem;
        for (mli.toFirst();(mem=mli.current());++mli)
        {
          printf("    Member type=%s name=%s\n",mem->type().data(),mem->name().data());
          QListIterator<IParam> pli(mem->getParamIterator());
          IParam *par;
          for (pli.toFirst();(par=pli.current());++pli)
          {
            printf("      Param type=%s name=%s defvalue=%s\n",
                par->type().data(),par->definitionName().data(),par->defaultValue().data());
          }
        }
      }
    }
    printf("---------------------------\n");

  }

  delete dox;

  return 0;
}

