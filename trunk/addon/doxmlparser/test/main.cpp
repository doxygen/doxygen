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

#include <stdio.h>
#include <stdlib.h>
#include <doxmlintf.h>
#include <qstring.h>

/*! Dumps the contents of a hyperlinked text fragment as plain text to the
 *  output.
 */
QString linkedTextToString(ILinkedTextIterator *ti)
{
  QString result;
  ILinkedText *lt=0;
  for (ti->toFirst();(lt=ti->current());ti->toNext())
  {
    switch (lt->kind())
    {
      case ILinkedText::Kind_Text: // plain text
        result+=dynamic_cast<ILT_Text*>(lt)->text()->latin1(); break;
      case ILinkedText::Kind_Ref:  // a link
        result+=dynamic_cast<ILT_Ref *>(lt)->text()->latin1(); break;
    }
  }
  return result;
}

/*! Macro for printing an indented  message. */
#define InPrint(x) printf("%s",indent.latin1()), printf x;

/*! Dumps the contents of a documentation block to stdout.
 *  @note This function will call itself recursively.
 *  @param doc The root of the documentation tree.
 *  @param level The indent level.
 */
void DumpDoc(IDoc *doc,int level)
{
  if (doc==0) return;
  QString indent;
  indent.fill(' ',level);
  //printf("      doc node kind=`%d'\n",doc->kind());
  switch (doc->kind())
  {
    case IDoc::Para: 
      {
        InPrint(("<para>\n"));
        IDocPara *par = dynamic_cast<IDocPara*>(doc);
        ASSERT(par!=0);
        IDocIterator *di = par->contents();
        IDoc *pdoc;
        for (di->toFirst();(pdoc=di->current());di->toNext())
        {
          DumpDoc(pdoc,level+1);
        }
        di->release();
        InPrint(("</para>\n"));
      }
      break;
    case IDoc::Text:
      {
        IDocText *txt = dynamic_cast<IDocText*>(doc);
        ASSERT(txt!=0);
        InPrint(("<text value=`%s' markup=%d headingLevel=%d/>\n",
              txt->text()->latin1(),txt->markup(),txt->headingLevel()));
      }
      break;
    case IDoc::MarkupModifier:
      {
        IDocMarkupModifier *md = dynamic_cast<IDocMarkupModifier*>(doc);
        ASSERT(md!=0);
        InPrint(("<markup modifier enabled=%d markup=%d headingLevel=%d/>\n",
              md->enabled(),md->markup(),md->headingLevel()));
      }
      break;
    case IDoc::ItemizedList:
      {
        InPrint(("<itemized list>\n"));
        IDocItemizedList *list = dynamic_cast<IDocItemizedList*>(doc);
        ASSERT(list!=0);
        IDocIterator *di = list->elements();
        IDoc *pdoc;
        for (di->toFirst();(pdoc=di->current());di->toNext())
        {
          DumpDoc(pdoc,level+1);
        }
        di->release();
        InPrint(("</itemized list>\n"));
      }
      break;
    case IDoc::OrderedList:
      {
        InPrint(("<ordered list>\n"));
        IDocOrderedList *list = dynamic_cast<IDocOrderedList*>(doc);
        ASSERT(list!=0);
        IDocIterator *di = list->elements();
        IDoc *pdoc;
        for (di->toFirst();(pdoc=di->current());di->toNext())
        {
          DumpDoc(pdoc,level+1);
        }
        di->release();
        InPrint(("</ordered list>\n"));
      }
      break;
    case IDoc::ListItem:
      {
        InPrint(("<list item>\n"));
        IDocListItem *li = dynamic_cast<IDocListItem*>(doc);
        ASSERT(li!=0);
        IDocIterator *di = li->contents();
        IDoc *pdoc;
        for (di->toFirst();(pdoc=di->current());di->toNext())
        {
          DumpDoc(pdoc,level+1);
        }
        di->release();
        InPrint(("</list item>\n"));
      }
      break;
    case IDoc::ParameterItem:
      {
        IDocParameterItem *item = dynamic_cast<IDocParameterItem*>(doc);
        InPrint(("<parameter item>\n"));
        IDocIterator *di = item->paramNames();
        IDoc *pdoc;
        for (di->toFirst();(pdoc=di->current());di->toNext())
        {
          DumpDoc(pdoc,level+1);
        }
        di->release();
        DumpDoc(item->description(),level+1);
        InPrint(("</parameter item>\n"));
      }
      break;
    case IDoc::ParameterList:
      {
        IDocParameterList *list = dynamic_cast<IDocParameterList*>(doc);
        InPrint(("<parameter list type=%d>\n",list->sectType()));
        IDocIterator *di = list->params();
        IDoc *pdoc;
        for (di->toFirst();(pdoc=di->current());di->toNext())
        {
          DumpDoc(pdoc,level+1);
        }
        di->release();
        InPrint(("</parameter list>\n"));
        ASSERT(list!=0);
      }
      break;
    case IDoc::Parameter:
      {
        IDocParameter *par = dynamic_cast<IDocParameter*>(doc);
        ASSERT(par!=0);
        InPrint(("<parameter name=%s/>\n",par->name()->latin1()));
      }
      break;
    case IDoc::SimpleSect:
      {
        IDocSimpleSect *ss = dynamic_cast<IDocSimpleSect*>(doc);
        ASSERT(ss!=0);
        InPrint(("<simplesect type=%s>\n",ss->typeString()->latin1()));
        DumpDoc(ss->title(),level+1);
        DumpDoc(ss->description(),level+1);
        InPrint(("<simplesect/>\n"));
      }
      break;
    case IDoc::Title:
      {
        InPrint(("<title>\n"));
        IDocTitle *t = dynamic_cast<IDocTitle*>(doc);
        ASSERT(t!=0);
        IDocIterator *di = t->title();
        IDoc *pdoc;
        for (di->toFirst();(pdoc=di->current());di->toNext())
        {
          DumpDoc(pdoc,level+1);
        }
        InPrint(("<title/>\n"));
      }
      break;
    case IDoc::Ref:
      {
        IDocRef *ref = dynamic_cast<IDocRef*>(doc);
        ASSERT(ref!=0);
        InPrint(("<ref id=%s text=%s/>\n",
            ref->refId()->latin1(),ref->text()->latin1()));
      }
      break;
    case IDoc::VariableList:
      {
        InPrint(("<variablelist>\n"));
        IDocVariableList *vl = dynamic_cast<IDocVariableList*>(doc);
        ASSERT(vl!=0);
        IDocIterator *di = vl->entries();
        IDoc *pdoc;
        for (di->toFirst();(pdoc=di->current());di->toNext())
        {
          DumpDoc(pdoc,level+1);
        }
        di->release();
        InPrint(("<variablelist/>\n"));
      }
      break;
    case IDoc::VariableListEntry:
      {
        IDocVariableListEntry *vle = dynamic_cast<IDocVariableListEntry*>(doc);
        ASSERT(vle!=0);
        ILinkedTextIterator *lti = vle->term();
        QString term = linkedTextToString(lti);
        lti->release();
        InPrint(("<variablelistentry term=%s>\n",term.latin1()));
        DumpDoc(vle->description(),level+1);
        InPrint(("<variablelistentry/>\n"));
      }
      break;
    case IDoc::HRuler:
      {
        IDocHRuler *hr = dynamic_cast<IDocHRuler*>(doc);
        ASSERT(hr!=0);
        InPrint(("<hruler/>\n"));
      }
      break;
    case IDoc::LineBreak:
      {
        IDocLineBreak *lb = dynamic_cast<IDocLineBreak*>(doc);
        ASSERT(lb!=0);
        InPrint(("<linebreak/>\n"));
      }
      break;
    case IDoc::ULink:
      {
        IDocULink *ul = dynamic_cast<IDocULink*>(doc);
        ASSERT(ul!=0);
        InPrint(("<ulink url=`%s' text=`%s'/>\n",ul->url()->latin1(),ul->text()->latin1()));
      }
      break;
    case IDoc::EMail:
      {
        IDocEMail *em = dynamic_cast<IDocEMail*>(doc);
        ASSERT(em!=0);
        InPrint(("<email address=`%s'/>\n",em->address()->latin1()));
      }
      break;
    case IDoc::Link:
      {
        IDocLink *lk = dynamic_cast<IDocLink*>(doc);
        ASSERT(lk!=0);
        InPrint(("<link refid=`%s' text=`%s'/>\n",lk->refId()->latin1(),lk->text()->latin1()));
      }
      break;
    case IDoc::ProgramListing:
      {
        IDocProgramListing *pl = dynamic_cast<IDocProgramListing*>(doc);
        ASSERT(pl!=0);
        InPrint(("<programlisting>\n"));
        IDocIterator *cli = pl->codeLines();
        IDoc *cl;
        for (cli->toFirst();(cl=cli->current());cli->toNext())
        {
          DumpDoc(cl,level+1);
        }
        cli->release();
        InPrint(("</programlisting>\n"));
      }
      break;
    case IDoc::CodeLine:
      {
        IDocCodeLine *cl = dynamic_cast<IDocCodeLine*>(doc);
        ASSERT(cl!=0);
        InPrint(("<codeline lineNumber=%d refId=`%s'>\n",cl->lineNumber(),cl->refId()->latin1()));
        IDocIterator *cei = cl->codeElements();
        IDoc *ce;
        for (cei->toFirst();(ce=cei->current());cei->toNext())
        {
          DumpDoc(ce,level+1);
        }
        cei->release();
        InPrint(("</codeline>\n"));
      }
      break;
    case IDoc::Highlight:
      {
        IDocHighlight *hl = dynamic_cast<IDocHighlight*>(doc);
        ASSERT(hl!=0);
        InPrint(("<highlight kind=%d>\n",hl->kind()));
        IDocIterator *cei = hl->codeElements();
        IDoc *ce;
        for (cei->toFirst();(ce=cei->current());cei->toNext())
        {
          DumpDoc(ce,level+1);
        }
        cei->release();
        InPrint(("</highlight>\n"));
      }
      break;
    case IDoc::Formula:
      {
        IDocFormula *fm = dynamic_cast<IDocFormula*>(doc);
        ASSERT(fm!=0);
        InPrint(("<formula id=`%s' text=`%s'/>\n",fm->id()->latin1(),fm->text()->latin1()));
      }
      break;
    case IDoc::Image:
      {
        IDocImage *img = dynamic_cast<IDocImage*>(doc);
        ASSERT(img!=0);
        InPrint(("<image name=`%s' caption=`%s'/>\n",img->name()->latin1(),img->caption()->latin1()));
      }
      break;
    case IDoc::DotFile:
      {
        IDocDotFile *df = dynamic_cast<IDocDotFile*>(doc);
        ASSERT(df!=0);
        InPrint(("<dotfile name=`%s' caption=`%s'/>\n",df->name()->latin1(),df->caption()->latin1()));
      }
      break;
    case IDoc::IndexEntry:
      {
        IDocIndexEntry *ie = dynamic_cast<IDocIndexEntry*>(doc);
        ASSERT(ie!=0);
        InPrint(("<indexentry primary=`%s' secondary=`%s'/>\n",ie->primary()->latin1(),ie->secondary()->latin1()));
      }
      break;
    case IDoc::Table:
      {
        IDocTable *tbl = dynamic_cast<IDocTable*>(doc);
        ASSERT(tbl!=0);
        InPrint(("<table numcols=%d caption=`%s'>\n",tbl->numColumns(),tbl->caption()->latin1()));
        IDocIterator *ri = tbl->rows();
        IDoc *row;
        for (ri->toFirst();(row=ri->current());ri->toNext())
        {
          DumpDoc(row,level+1);
        }
        ri->release();
        InPrint(("</table>\n"));
      }
      break;
    case IDoc::Row:
      {
        IDocRow *row = dynamic_cast<IDocRow*>(doc);
        ASSERT(row!=0);
        InPrint(("<row>\n"));
        IDocIterator *ei = row->entries();
        IDoc *e;
        for (ei->toFirst();(e=ei->current());ei->toNext())
        {
          DumpDoc(e,level+1);
        }
        ei->release();
        InPrint(("</row>\n"));
      }
      break;
    case IDoc::Entry:
      {
        IDocEntry *ent = dynamic_cast<IDocEntry*>(doc);
        ASSERT(ent!=0);
        InPrint(("<entry>\n"));
        IDocIterator *di = ent->contents();
        IDoc *pdoc;
        for (di->toFirst();(pdoc=di->current());di->toNext())
        {
          DumpDoc(pdoc,level+1);
        }
        di->release();
        InPrint(("</entry>\n"));
      }
      break;
    case IDoc::Section:
      {
        IDocSection *sec = dynamic_cast<IDocSection*>(doc);
        ASSERT(sec!=0);
        InPrint(("<section id=`%s' level=%d>\n",
            sec->id()->latin1(),sec->level()));
        DumpDoc(sec->title(),level+1);
        IDocIterator *di = sec->paragraphs();
        IDoc *pdoc;
        for (di->toFirst();(pdoc=di->current());di->toNext())
        {
          DumpDoc(pdoc,level+1);
        }
        di=sec->subSections();
        for (di->toFirst();(pdoc=di->current());di->toNext())
        {
          DumpDoc(pdoc,level+1);
        }
        IDocInternal *intern = sec->internal();
        if (intern)
        {
          DumpDoc(intern,level+1);
        }
        InPrint(("</section>\n"));
      }
      break;
    case IDoc::Internal:
      {
        IDocInternal *intern = dynamic_cast<IDocInternal*>(doc);
        ASSERT(intern!=0);
        InPrint(("<internal>\n"));
        IDocIterator *di = intern->paragraphs();
        IDoc *pdoc;
        for (di->toFirst();(pdoc=di->current());di->toNext())
        {
          DumpDoc(pdoc,level+1);
        }
        di=intern->subSections();
        for (di->toFirst();(pdoc=di->current());di->toNext())
        {
          DumpDoc(pdoc,level+1);
        }
        InPrint(("</internal>\n"));
      }
      break;
    case IDoc::Copy:
      {
        IDocCopy *cpy = dynamic_cast<IDocCopy*>(doc);
        ASSERT(cpy!=0);
        InPrint(("<copydoc>\n"));
        IDocIterator *di = cpy->contents();
        IDoc *pdoc;
        for (di->toFirst();(pdoc=di->current());di->toNext())
        {
          DumpDoc(pdoc,level+1);
        }
        di->release();
        InPrint(("<copydoc/>\n"));
      }
      break;
    case IDoc::TocItem:
      {
        IDocTocItem *ti = dynamic_cast<IDocTocItem*>(doc);
        ASSERT(ti!=0);
        InPrint(("<tocitem id=\"%s\" title=\"%s\"/>\n",
                 ti->id()->latin1(),ti->title()->latin1()));
      }
      break;
    case IDoc::TocList:
      {
        IDocTocList *tl = dynamic_cast<IDocTocList*>(doc);
        ASSERT(tl!=0);
        InPrint(("<toclist>\n"));
        IDocIterator *di = tl->elements();
        IDoc *pdoc;
        for (di->toFirst();(pdoc=di->current());di->toNext())
        {
          DumpDoc(pdoc,level+1);
        }
        di->release();
        InPrint(("<toclist/>\n"));
      }
      break;
    case IDoc::Verbatim:
      {
        IDocVerbatim *vt = dynamic_cast<IDocVerbatim*>(doc);
        ASSERT(vt!=0);
        const char *s=0;
        switch (vt->type())
        {
          case IDocVerbatim::Verbatim:  s="verbatim"; break;
          case IDocVerbatim::HtmlOnly:  s="htmlonly"; break;
          case IDocVerbatim::LatexOnly: s="latexonly"; break;
          default:
            printf("Invalid verbatim type!\n");
        }
        InPrint(("<verbatim %s>\n",s));
        InPrint(("%s",vt->text()->latin1()));
        InPrint(("</verbatim>\n"));
      }
      break;
    case IDoc::Anchor:
      {
        IDocAnchor *anc = dynamic_cast<IDocAnchor*>(doc);
        ASSERT(anc!=0);
        InPrint(("<anchor id='%s'/>\n",anc->id()->latin1()));
      }
      break;
    case IDoc::Symbol:
      {
        IDocSymbol *sym = dynamic_cast<IDocSymbol*>(doc);
        ASSERT(sym!=0);
        InPrint(("<symbol type=%s letter=%c/>\n",
              sym->typeString()->latin1(),sym->letter()));
      }
      break;
    case IDoc::Root:
      {
        InPrint(("<root>\n"));
        IDocRoot *root = dynamic_cast<IDocRoot*>(doc);
        ASSERT(root!=0);
        IDocIterator *di = root->contents();
        IDoc *pdoc;
        for (di->toFirst();(pdoc=di->current());di->toNext())
        {
          DumpDoc(pdoc,level+1);
        }
        di->release();
        InPrint(("</root>\n"));
      }
      break;

    default:
      printf("Found unsupported node type %d!\n",doc->kind());
      break;
  }
}

void DumpGraph(IGraph *graph)
{
  if (graph==0) { printf(" --- no graph ---\n"); return; }
  printf(" --- graph ----\n");
  INodeIterator *ni = graph->nodes();
  INode *node;
  for (ni->toFirst();(node=ni->current());ni->toNext())
  {
    printf("   --- node id=%s label=%s linkId=%s\n",
           node->id()->latin1(),
           node->label()->latin1(),
           node->linkId()->latin1()
          );
    IChildNodeIterator *cni = node->children();
    IChildNode *cn;
    for (cni->toFirst();(cn=cni->current());cni->toNext())
    {
      printf("    + child id=%s label=%s relation=%s\n",
          cn->node()->id()->latin1(),
          cn->node()->label()->latin1(),
          cn->relationString()->latin1()
      );
      IEdgeLabelIterator *eli = cn->edgeLabels();
      IEdgeLabel *el;
      for (eli->toFirst();(el=eli->current());eli->toNext())
      {
        printf("      edgeLabel=%s\n",el->label()->latin1());
      }
      eli->release();
    }
    cni->release();
  }
  ni->release();
  printf(" --- end graph ----\n");

}

void DumpParamList(IParamIterator *pli,int indent)
{
  QString indentStr;
  indentStr.fill(' ',indent);
  IParam *par;
  for (pli->toFirst();(par=pli->current());pli->toNext())
  {
    ILinkedTextIterator *lti = par->type();
    QString parType = linkedTextToString(lti);
    lti->release();
    lti = par->defaultValue();
    QString defVal =  linkedTextToString(lti);
    lti->release();
    printf("%sParam type=%s decl_name=%s def_name=%s defvalue=%s\n",
        indentStr.data(), parType.latin1(), 
        par->declarationName()->latin1(),
        par->definitionName()->latin1(),
        defVal.latin1());
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

  dox->setDebugLevel(4);

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
        comp->name()->latin1(),comp->id()->latin1(),comp->kindString()->latin1());

    ISectionIterator *sli = comp->sections();
    ISection *sec;
    for (sli->toFirst();(sec=sli->current());sli->toNext())
    {
      printf("  Section kind=%s\n",sec->kindString()->latin1());
      IMemberIterator *mli = sec->members();
      IMember *mem;
      if( sec->kind() == ISection::UserDefined )
      {
	IUserDefined *group = dynamic_cast<IUserDefined*>(sec);
	printf("    Title=%s\n", group->header()->latin1() );
      }
      for (mli->toFirst();(mem=mli->current());mli->toNext())
      {
        ILinkedTextIterator *lti = mem->type();
        printf("    Member type=%s name=%s\n",
            linkedTextToString(lti).latin1(),mem->name()->latin1());
        lti->release();

        IParamIterator *pli = mem->parameters();
        DumpParamList(pli,6);
        pli->release();
        IMemberReferenceIterator *mri = mem->references();
        IMemberReference *mr;
        for (mri->toFirst();(mr=mri->current());mri->toNext())
        {
          IMember *memr = mr->member();
          printf("      References %s at line %d\n",
              mr->name()->latin1(),memr->bodyStart());
        }

        mri->release();
        mri = mem->referencedBy();
        for (mri->toFirst();(mr=mri->current());mri->toNext())
        {
          IMember *memr = mr->member();
          printf("      ReferencedBy %s at line %d\n",
              mr->name()->latin1(),memr->bodyStart());
        }
        mri->release();

        if (mem->kind()==IMember::Enum) // we have found an enum
        {
          IEnum *e = dynamic_cast<IEnum*>(mem);
          IMemberIterator *evi = e->enumValues(); // get the enum values
          IMember *mev;
          for (evi->toFirst();(mev=evi->current());evi->toNext())
          {
            IEnumValue *ev = dynamic_cast<IEnumValue*>(mev);
            ILinkedTextIterator *lti = ev->initializer();
            QString init = linkedTextToString(lti);
            lti->release();
            printf("      Enum value `%s' init=`%s'\n",
                ev->name()->latin1(),init.latin1());
          }
          evi->release();
        }

        pli = mem->templateParameters();
        if (pli)
        {
          printf("      Template parameters\n");
          DumpParamList(pli,8);
          pli->release();
        }

        IDoc *doc = mem->briefDescription();
        if (doc)
        {
          printf("===== brief description ==== \n");
          DumpDoc(doc,0);
        }

        doc = mem->detailedDescription();
        if (doc)
        {
          printf("===== detailed description ==== \n");
          DumpDoc(doc,0);
        }
      }
      mli->release();
    }
    sli->release();

    IDoc *doc = comp->briefDescription();
    if (doc)
    {
      printf("===== brief description ==== \n");
      DumpDoc(doc,0);
    }

    doc = comp->detailedDescription();
    if (doc)
    {
      printf("===== detailed description ==== \n");
      DumpDoc(doc,0);
    }

    if (comp->kind()==ICompound::Class)
    {
      IClass *cls = dynamic_cast<IClass*>(comp);
      ASSERT(cls!=0);

      printf("==== inheritance graph ==== \n");
      DumpGraph(cls->inheritanceGraph());

      printf("==== collabration graph ==== \n");
      DumpGraph(cls->collaborationGraph());

      printf("==== base classes ==== \n");
      IRelatedCompoundIterator *bcli = cls->baseCompounds();
      IRelatedCompound *bClass;
      for (bcli->toFirst();(bClass=bcli->current());bcli->toNext())
      {
        ICompound *bc = bClass->compound();
        printf("    + class %s\n",bc->name()->latin1());
        bc->release();
      }
      bcli->release();

      printf("==== derived classes ==== \n");
      IRelatedCompoundIterator *dcli = cls->derivedCompounds();
      IRelatedCompound *dClass;
      for (dcli->toFirst();(dClass=dcli->current());dcli->toNext())
      {
        ICompound *dc = dClass->compound();
        printf("    + class %s\n",dc->name()->latin1());
        dc->release();
      }
      dcli->release();
    }
    else if (comp->kind()==ICompound::File)
    {
      IFile *file = dynamic_cast<IFile*>(comp);
      ASSERT(file!=0);

      printf("==== include dependency graph ==== \n");
      DumpGraph(file->includeDependencyGraph());

      printf("==== included by dependency graph ==== \n");
      DumpGraph(file->includedByDependencyGraph());

      printf("==== source ====\n");
      DumpDoc(file->source(),0);
    }
    
    comp->release();
  }
  cli->release();
  printf("---------------------------\n");

  dox->release();

  return 0;
}

