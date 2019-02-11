#include "arguments.h"
#include <assert.h>

/*! the argument list is documented if one of its
 *  arguments is documented 
 */
bool ArgumentList::hasDocumentation() const
{
  bool hasDocs=FALSE;
  ArgumentListIterator ali(*this);
  Argument *a;
  for (ali.toFirst();!hasDocs && (a=ali.current());++ali)
  {
    hasDocs = a->hasDocumentation(); 
  }
  return hasDocs;
}

ArgumentList *ArgumentList::deepCopy() const
{
  ArgumentList *argList = new ArgumentList;
  argList->setAutoDelete(TRUE);

  QListIterator<Argument> ali(*this);
  Argument *a;
  for (;(a=ali.current());++ali)
  {
    argList->append(new Argument(*a));
  }
  argList->constSpecifier     = constSpecifier;
  argList->volatileSpecifier  = volatileSpecifier;
  argList->pureSpecifier      = pureSpecifier;
  argList->trailingReturnType = trailingReturnType;
  argList->isDeleted          = isDeleted;
  argList->refQualifier       = refQualifier;

  return argList;
}

