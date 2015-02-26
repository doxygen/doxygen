/******************************************************************************
 *
 * 
 *
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
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

#ifndef _CPPVALUE_H
#define _CPPVALUE_H

#include <stdio.h>
#include <qglobal.h> 

/** A class representing a C-preprocessor value. */
class CPPValue
{
  public:
    enum Type { Int, Float };
  
    CPPValue(long val=0) : type(Int) { v.l = val; }
    CPPValue(double val) : type(Float) { v.d = val; }

    operator double () const { return type==Int ? (double)v.l : v.d; }
    operator long ()   const { return type==Int ? v.l : (long)v.d;   }

    bool isInt() const { return type == Int; }
     
    void print() const
    {
      if (type==Int) 
        printf("(%ld)\n",v.l);
      else
        printf("(%f)\n",v.d);
    }

  private:
    Type type;
    union {
      double d;
      long l;
    } v;
};

extern CPPValue parseOctal();
extern CPPValue parseDecimal();
extern CPPValue parseHexadecimal();
extern CPPValue parseCharacter();
extern CPPValue parseFloat();

#endif
