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

#ifndef DEFINITION_H
#define DEFINITION_H

#include <qstring.h>
#include "config.h"

/*! The common base class of all definitions. */
class Definition
{
  public:
    //! create a new definition
    Definition(const char *name,const char *b=0,const char *d=0)
      { n=name; brief=b; doc=d; }
    //! destroys the definition
    virtual ~Definition() {}
    //! returns the name of the definition
    QString name() const { return n; }
    //! returns the detailed description of this definition
    QString documentation() const { return doc; }
    //! returns the brief description of this definition
    QString briefDescription() const { return brief; }
    //! sets a new name for the definition
    void setName(const char *name) { n=name; }
    //! sets the documentation of this definition
    void setDocumentation(const char *d) 
      { 
        doc=d; doc=doc.stripWhiteSpace();
      }
    //! sets the brief description of this definition
    void setBriefDescription(const char *b) 
      { 
        brief=b; brief=brief.stripWhiteSpace(); 
        int bl=brief.length(); 
        if (bl>0 && brief.at(bl-1)!='.' && brief.at(bl-1)!='!' && 
            brief.at(bl-1)!='?') brief+='.'; 
      }
    //! returns TRUE iff the definition is documented
    bool hasDocumentation() 
      { return !doc.isNull() || !brief.isNull() || extractAllFlag; }
    QString nameToFile(const char *name);

  private: 
    QString n;     // name of the definition
    QString brief; // brief description
    QString doc;   // detailed description
};

#endif
