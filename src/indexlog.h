/*
 * Copyright (C) 2008 by Sebastian Pipping.
 * Copyright (C) 2008 Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * Documents produced by Doxygen are derivative works derived from the
 * input used in their production; they are not affected by this license.
 *
 * Sebastian Pipping <sebastian@pipping.org>
 */

#ifndef INDEXLOG_H
#define INDEXLOG_H

#include "index.h"
#include "qhpxmlwriter.h"

class IndexLog : public IndexIntf
{
  public:  
    IndexLog();
   ~IndexLog();

    // BEGIN IndexIntf
    void initialize();
    void finalize();
    void incContentsDepth();
    void decContentsDepth();
    void addContentsItem(bool isDir, const char *name, const char *ref,
                         const char *file, const char *anchor);
    void addIndexItem(const char *level1, const char *level2,
                      const char *contRef, const char *memRef, 
                      const char *anchor, const MemberDef * md);
    void addIndexFile(const char *name);
    // END IndexIntf

  private:
    void openMethodCall(char const * methodName);
    void addPrimitiveParameter(char const * parameterName,
                               char const * value);
    void addBoolParameter(char const * parameterName, bool value);
    void addStringParameter(char const * parameterName,
                            char const * value);
    void addMemberDefParameter(char const * parameterName,
                               const MemberDef * value);
    void closeMethodCall();

    QhpXmlWriter m_out;
};

#endif // INDEXLOG_H 

