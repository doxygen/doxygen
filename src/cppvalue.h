/******************************************************************************
 *
 * Copyright (C) 1997-2022 by Dimitri van Heesch.
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

#ifndef CPPVALUE_H
#define CPPVALUE_H

#include <cstdio>
#include <string>

/** A class representing a C-preprocessor value. */
class CPPValue
{
  public:
    enum Type { Int, Float };

    explicit CPPValue(char c) : type(Int) { v.l = c; }
    explicit CPPValue(long val=0) : type(Int) { v.l = val; }
    explicit CPPValue(double val) : type(Float) { v.d = val; }

    operator double () const { return type==Int ? static_cast<double>(v.l) : v.d; }
    operator long ()   const { return type==Int ? v.l : static_cast<long>(v.d);   }

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

extern CPPValue parseOctal(const std::string& token);
extern CPPValue parseDecimal(const std::string& token);
extern CPPValue parseHexadecimal(const std::string& token);
extern CPPValue parseBinary(const std::string& token);
extern CPPValue parseCharacter(const std::string& token);
extern CPPValue parseFloat(const std::string& token);

#endif
