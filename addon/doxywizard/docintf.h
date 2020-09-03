/******************************************************************************
 *
 * Copyright (C) 1997-2019 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#ifndef DOCINTF_H
#define DOCINTF_H

class DocIntf
{
  public:
    virtual ~DocIntf() {}
    virtual void setHeader(const char *header) = 0;
    virtual void add(const char *name,const char *docs) = 0;
};

#endif
