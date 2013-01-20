/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2013 by Dimitri van Heesch.
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
 */

#ifndef _DEBUG_H
#define _DEBUG_H

/** Class containing a print function for diagnostics. */
class Debug
{
  public:
    enum DebugMask { Quiet        = 0x00000000,
                     FindMembers  = 0x00000001, 
                     Functions    = 0x00000002, 
                     Variables    = 0x00000004,
                     Preprocessor = 0x00000008,
                     Classes      = 0x00000010,
                     CommentCnv   = 0x00000020,
                     CommentScan  = 0x00000040,
                     Validate     = 0x00000080,
                     PrintTree    = 0x00000100,
                     Time         = 0x00000200,
                     ExtCmd       = 0x00000400,
                     Markdown     = 0x00000800,
                     FilterOutput = 0x00001000
                   };
    static void print(DebugMask mask,int prio,const char *fmt,...);
    static void setFlag(const char *label);
    static void clearFlag(const char *label);
    static bool isFlagSet(DebugMask mask);
    static void setPriority(int p);
    
  private:
    static DebugMask curMask;
    static int curPrio;
};

#endif
