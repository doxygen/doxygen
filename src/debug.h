/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
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

#ifndef _DEBUG_H
#define _DEBUG_H

class Debug
{
  public:
    enum DebugMask { Quiet        = 0x00000000,
                     FindMembers  = 0x00000001, 
                     Functions    = 0x00000002, 
                     Variables    = 0x00000004,
                     Preprocessor = 0x00000008,
                     Classes      = 0x00000010
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
