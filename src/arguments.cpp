#include "arguments.h"

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

