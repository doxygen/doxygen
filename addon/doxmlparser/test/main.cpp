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
#include <doxmlintf.h>

QString linkedTextToString(ILinkedTextIterator *ti)
{
  QString result;
  ILinkedText *lt=0;
  for (ti->toFirst();(lt=ti->current());ti->toNext())
  {
    switch (lt->kind())
    {
      case ILinkedText::Kind_Text: 
        result+=dynamic_cast<ILT_Text*>(lt)->text(); break;
      case ILinkedText::Kind_Ref:  
        result+=dynamic_cast<ILT_Ref *>(lt)->text(); break;
    }
  }
  return result;
}

void DumpDoc(IDoc *doc)
{
  printf("      doc node kind=`%d'\n",doc->kind());
  switch (doc->kind())
  {
    case IDoc::Para: 
      {
        printf("     --- par --- \n");
        IDocPara *par = dynamic_cast<IDocPara*>(doc);
        ASSERT(par!=0);
        IDocIterator *di = par->contents();
        IDoc *pdoc;
        for (di->toFirst();(pdoc=di->current());di->toNext())
        {
          DumpDoc(pdoc);
        }
        di->release();
        printf("     --- end par --- \n");
      }
      break;
    case IDoc::Text:
      {
        IDocText *txt = dynamic_cast<IDocText*>(doc);
        ASSERT(txt!=0);
        printf("     --- text `%s' markup=%d --- \n",txt->text().data(),txt->markup());
      }
      break;
    case IDoc::Title:
      {
        printf("     --- title --- \n");
        IDocTitle *t = dynamic_cast<IDocTitle*>(doc);
        ASSERT(t!=0);
        IDocIterator *di = t->title();
        IDoc *pdoc;
        for (di->toFirst();(pdoc=di->current());di->toNext())
        {
          DumpDoc(pdoc);
        }
        printf("     --- end title --- \n");
      }
      break;
    case IDoc::Section:
      {
        IDocSection *sec = dynamic_cast<IDocSection*>(doc);
        ASSERT(sec!=0);
        printf("     --- section id=`%s' level=%d --- \n",
            sec->id().data(),sec->level());
        IDocIterator *di = sec->title();
        IDoc *pdoc;
        for (di->toFirst();(pdoc=di->current());di->toNext())
        {
          DumpDoc(pdoc);
        }
        printf("     --- end section --- \n");
      }
      break;
    case IDoc::Root:
      {
        printf("     --- root --- \n");
        IDocRoot *root = dynamic_cast<IDocRoot*>(doc);
        ASSERT(root!=0);
        IDocIterator *di = root->contents();
        IDoc *pdoc;
        for (di->toFirst();(pdoc=di->current());di->toNext())
        {
          DumpDoc(pdoc);
        }
        di->release();
        printf("     --- end root --- \n");
      }
      break;

    default:
      break;
  }
}

int main(int argc,char **argv)
{
  if (argc!=2)
  {
    printf("Usage: %s file.xml\n",argv[0]);
    exit(1);
  }

  IDoxygen *dox = createObjectModelFromXML(argv[1]);

  ICompoundIterator *cli = dox->compounds();
  ICompound *comp;
  printf("--- compound list ---------\n");
  for (cli->toFirst();(comp=cli->current());cli->toNext())
  {
    printf("Compound name=%s id=%s kind=%s\n",
        comp->name().data(),comp->id().data(),comp->kind().data());
    ISectionIterator *sli = comp->sections();
    ISection *sec;
    for (sli->toFirst();(sec=sli->current());sli->toNext())
    {
      printf("  Section kind=%s\n",sec->kind().data());
      IMemberIterator *mli = sec->members();
      IMember *mem;
      for (mli->toFirst();(mem=mli->current());mli->toNext())
      {
        ILinkedTextIterator *lti = mem->type();
        printf("    Member type=%s name=%s\n",
            linkedTextToString(lti).data(),mem->name().data());
        lti->release();

        IParamIterator *pli = mem->params();
        IParam *par;
        for (pli->toFirst();(par=pli->current());pli->toNext())
        {
          lti = par->type();
          QString parType = linkedTextToString(lti);
          lti->release();
          lti = par->defaultValue();
          QString defVal =  linkedTextToString(lti);
          lti->release();
          printf("      Param type=%s name=%s defvalue=%s\n",
              parType.data(), par->definitionName().data(),defVal.data());
        }
        pli->release();
        IMemberReferenceIterator *mri = mem->references();
        IMemberReference *mr;
        for (mri->toFirst();(mr=mri->current());mri->toNext())
        {
          IMember *memr = mr->member();
          printf("      References %s at line %d\n",
              mr->memberName().data(),memr->bodyStart());
        }

        mri->release();
        mri = mem->referencedBy();
        for (mri->toFirst();(mr=mri->current());mri->toNext())
        {
          IMember *memr = mr->member();
          printf("      ReferencedBy %s at line %d\n",
              mr->memberName().data(),memr->bodyStart());
        }
        mri->release();

        IEnumValueIterator *evi = mem->enumValues();
        IEnumValue *ev;
        for (evi->toFirst();(ev=evi->current());evi->toNext())
        {
          printf("      Enum value `%s' init=`%s'\n",
              ev->name().data(),ev->initializer().data());
        }
        evi->release();

        IDoc *doc = mem->briefDescription();
        if (doc)
        {
          printf("===== brief description ==== \n");
          DumpDoc(doc);
        }

        doc = mem->detailedDescription();
        if (doc)
        {
          printf("===== detailed description ==== \n");
          DumpDoc(doc);
        }
      }
      mli->release();
    }
    sli->release();

    IDoc *doc = comp->briefDescription();
    if (doc)
    {
      printf("===== brief description ==== \n");
      DumpDoc(doc);
    }

    doc = comp->detailedDescription();
    if (doc)
    {
      printf("===== detailed description ==== \n");
      DumpDoc(doc);
    }
  }
  cli->release();
  printf("---------------------------\n");

  dox->release();

  return 0;
}

