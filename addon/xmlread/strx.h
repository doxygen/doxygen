#ifndef _STRX_H
#define _STRX_H

// ---------------------------------------------------------------------------
//  Includes for all the program files to see
// ---------------------------------------------------------------------------
#include "qtbc.h"
#include <iostream.h>

/*!
 *  This is a simple wrapper class around QCString that lets us do easy 
 *  trancoding of XMLCh data to local code page for display.
 */
class StrX : public QCString
{
  public:
    /*! Constructor */
    StrX(const XMLCh* const toTranscode) : 
         QCString(XMLString::transcode(toTranscode)) {}
    /*! Destructor */
    ~StrX() {}
};

#endif
