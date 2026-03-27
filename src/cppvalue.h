/******************************************************************************
 *
 * Copyright (C) 1997-2021 by Dimitri van Heesch.
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
    enum class Type { Int, Float };

    explicit constexpr CPPValue(char c)     noexcept : m_type(Type::Int),   m_d(0.0), m_l(c)   {}
    explicit constexpr CPPValue(long val=0) noexcept : m_type(Type::Int),   m_d(0.0), m_l(val) {}
    explicit constexpr CPPValue(double val) noexcept : m_type(Type::Float), m_d(val), m_l(0)   {}

    constexpr operator double () const noexcept { return m_type==Type::Int ? static_cast<double>(m_l) : m_d; }
    constexpr operator long ()   const noexcept { return m_type==Type::Int ? m_l : static_cast<long>(m_d);   }

    constexpr Type type() const noexcept { return m_type; }

    void print() const
    {
      if (m_type==Type::Int)
        printf("(%ld)\n",m_l);
      else
        printf("(%f)\n",m_d);
    }
    static CPPValue parseOctal      (const std::string &token);
    static CPPValue parseDecimal    (const std::string &token);
    static CPPValue parseHexadecimal(const std::string &token);
    static CPPValue parseBinary     (const std::string &token);
    static CPPValue parseCharacter  (const std::string &token);
    static CPPValue parseFloat      (const std::string &token);

  private:
    Type m_type;
    double m_d;
    long m_l;
};


#endif
