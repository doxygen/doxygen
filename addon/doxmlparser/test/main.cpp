/******************************************************************************
 *
 * $Id$
 *
 *
 * Copyright (C) 1997-2002 by Dimitri van Heesch.
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
    case IDoc::MarkupModifier:
      {
        IDocMarkupModifier *md = dynamic_cast<IDocMarkupModifier*>(doc);
        ASSERT(md!=0);
        printf("     --- markup modifier enabled=%d markup=%d\n",md->enabled(),md->markup());
      }
      break;
    case IDoc::ItemizedList:
      {
        printf("     --- itemized list --- \n");
        IDocItemizedList *list = dynamic_cast<IDocItemizedList*>(doc);
        ASSERT(list!=0);
        IDocIterator *di = list->elements();
        IDoc *pdoc;
        for (di->toFirst();(pdoc=di->current());di->toNext())
        {
          DumpDoc(pdoc);
        }
        di->release();
        printf("     --- end itemized list --- \n");
      }
      break;
    case IDoc::OrderedList:
      {
        printf("     --- ordered list --- \n");
        IDocOrderedList *list = dynamic_cast<IDocOrderedList*>(doc);
        ASSERT(list!=0);
        IDocIterator *di = list->elements();
        IDoc *pdoc;
        for (di->toFirst();(pdoc=di->current());di->toNext())
        {
          DumpDoc(pdoc);
        }
        di->release();
        printf("     --- end ordered list --- \n");
      }
      break;
    case IDoc::ListItem:
      {
        printf("     --- list item --- \n");
        IDocListItem *li = dynamic_cast<IDocListItem*>(doc);
        ASSERT(li!=0);
        IDocIterator *di = li->contents();
        IDoc *pdoc;
        for (di->toFirst();(pdoc=di->current());di->toNext())
        {
          DumpDoc(pdoc);
        }
        di->release();
        printf("     --- end list item --- \n");
      }
      break;
    case IDoc::ParameterList:
      {
        IDocParameterList *list = dynamic_cast<IDocParameterList*>(doc);
        printf("     --- parameter list type=%d --- \n",list->listType());
        IDocIterator *di = list->params();
        IDoc *pdoc;
        for (di->toFirst();(pdoc=di->current());di->toNext())
        {
          DumpDoc(pdoc);
        }
        di->release();
        printf("     --- parameter list item --- \n");
        ASSERT(list!=0);
      }
      break;
    case IDoc::Parameter:
      {
        IDocParameter *par = dynamic_cast<IDocParameter*>(doc);
        ASSERT(par!=0);
        printf("     --- parameter name=%s --- \n",par->name().data());
        DumpDoc(par->description());
        printf("     --- end parameter --- \n");
      }
      break;
    case IDoc::SimpleSect:
      {
        IDocSimpleSect *ss = dynamic_cast<IDocSimpleSect*>(ss);
        ASSERT(ss!=0);
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
    case IDoc::Ref:
      {
        IDocRef *ref = dynamic_cast<IDocRef*>(ref);
        ASSERT(ref!=0);
      }
      break;
    case IDoc::VariableList:
      {
        printf("     --- start variablelist --- \n");
        IDocVariableList *vl = dynamic_cast<IDocVariableList*>(doc);
        ASSERT(vl!=0);
        IDocIterator *di = vl->entries();
        IDoc *pdoc;
        for (di->toFirst();(pdoc=di->current());di->toNext())
        {
          DumpDoc(pdoc);
        }
        di->release();
        printf("     --- end variablelist --- \n");
      }
      break;
    case IDoc::VariableListEntry:
      {
        IDocVariableListEntry *vle = dynamic_cast<IDocVariableListEntry*>(doc);
        ASSERT(vle!=0);
        printf("     --- start variablelistentry term=%s --- \n",vle->term().data());
        DumpDoc(vle->description());
        printf("     --- end variablelistentry --- \n");
      }
      break;
    case IDoc::HRuler:
      {
        IDocHRuler *hr = dynamic_cast<IDocHRuler*>(hr);
        ASSERT(hr!=0);
      }
      break;
    case IDoc::LineBreak:
      {
        IDocLineBreak *lb = dynamic_cast<IDocLineBreak*>(lb);
        ASSERT(lb!=0);
      }
      break;
    case IDoc::ULink:
      {
        IDocULink *ul = dynamic_cast<IDocULink*>(ul);
        ASSERT(ul!=0);
      }
      break;
    case IDoc::EMail:
      {
        IDocEMail *em = dynamic_cast<IDocEMail*>(em);
        ASSERT(em!=0);
      }
      break;
    case IDoc::Link:
      {
        IDocLink *lk = dynamic_cast<IDocLink*>(lk);
        ASSERT(lk!=0);
      }
      break;
    case IDoc::ProgramListing:
      {
        IDocProgramListing *pl = dynamic_cast<IDocProgramListing*>(pl);
        ASSERT(pl!=0);
      }
      break;
    case IDoc::CodeLine:
      {
        IDocCodeLine *cl = dynamic_cast<IDocCodeLine*>(cl);
        ASSERT(cl!=0);
      }
      break;
    case IDoc::Highlight:
      {
        IDocHighlight *hl = dynamic_cast<IDocHighlight*>(hl);
        ASSERT(hl!=0);
      }
      break;
    case IDoc::Anchor:
      {
        IDocAnchor *anc = dynamic_cast<IDocAnchor*>(anc);
        ASSERT(anc!=0);
      }
      break;
    case IDoc::Formula:
      {
        IDocFormula *fm = dynamic_cast<IDocFormula*>(fm);
        ASSERT(fm!=0);
      }
      break;
    case IDoc::Image:
      {
        IDocImage *img = dynamic_cast<IDocImage*>(img);
        ASSERT(img!=0);
      }
      break;
    case IDoc::DotFile:
      {
        IDocDotFile *df = dynamic_cast<IDocDotFile*>(df);
        ASSERT(df!=0);
      }
      break;
    case IDoc::IndexEntry:
      {
        IDocIndexEntry *ie = dynamic_cast<IDocIndexEntry*>(ie);
        ASSERT(ie!=0);
      }
      break;
    case IDoc::Table:
      {
        IDocTable *tbl = dynamic_cast<IDocTable*>(tbl);
        ASSERT(tbl!=0);
      }
      break;
    case IDoc::Row:
      {
        IDocRow *row = dynamic_cast<IDocRow*>(row);
        ASSERT(row!=0);
      }
      break;
    case IDoc::Entry:
      {
        IDocEntry *ent = dynamic_cast<IDocEntry*>(ent);
        ASSERT(ent!=0);
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
    printf("Usage: %s xmldir\n",argv[0]);
    exit(1);
  }

  IDoxygen *dox = createObjectModel();

  if (!dox->readXMLDir(argv[1]))
  {
    printf("Error reading %s/index.xml\n",argv[1]);
    exit(1);
  }

  ICompoundIterator *cli = dox->compounds();
  ICompound *comp;
  printf("--- compound list ---------\n");
  for (cli->toFirst();(comp=cli->current());cli->toNext())
  {
    printf("Compound name=%s id=%s kind=%s\n",
        comp->name().data(),comp->id().data(),comp->kindString().data());
    ISectionIterator *sli = comp->sections();
    ISection *sec;
    for (sli->toFirst();(sec=sli->current());sli->toNext())
    {
      printf("  Section kind=%s\n",sec->kindString().data());
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
    comp->release();
  }
  cli->release();
  printf("---------------------------\n");

  dox->release();

  return 0;
}

