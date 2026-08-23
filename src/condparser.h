/**
 * Copyright (C) 1997-2026 by Dimitri van Heesch.
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
 * C++ Expression parser for ENABLED_SECTIONS in Doxygen
 *
 * Features used:
 *     Operators:
 *         &&    AND operator
 *         ||    OR  operator
 *         !     NOT operator
 */

#ifndef CONDPARSER_H
#define CONDPARSER_H


#include "dstring.h"

class CondParser
{
  // public functions
  public:
    CondParser() : m_e(nullptr), m_tokenType(NOTHING) {}
    bool parse(const DString &fileName,int lineNr,const DString &expr);

    // enumerations
  private:
    enum TOKENTYPE
    {
      NOTHING = -1,
      DELIMITER,
      VARIABLE,
      UNKNOWN
    };
    enum OPERATOR_ID
    {
      UNKNOWN_OP = -1,
      AND = 1,
      OR,
      NOT
    };

    // data
  private:

    DString m_err;                  //!< error state
    DString m_expr;                 //!< holds the expression
    const char *m_e;                //!< points to a character in expr

    DString m_token;                //!< holds the token
    TOKENTYPE m_tokenType;          //!< type of the token

    // private functions
  private:
    void getToken();

    bool parseLevel1();
    bool parseLevel2();
    bool parseLevel3();
    bool parseVar();

    bool evalOperator(const int opId, bool lhs, bool rhs);
    bool evalVariable(const DString &varName);
    int getOperatorId(const DString &opName);
};

#endif

