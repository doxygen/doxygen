/**
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
 * C++ Expression parser for ENABLED_SECTIONS in Doxygen
 *
 * Features used:
 *     Operators:
 *         &&    AND operator
 *         ||    OR  operator
 *         !     NOT operator
 */

#include <algorithm>

#include "condparser.h"
#include "config.h"
#include "message.h"

// declarations

/**
 * parses and evaluates the given expression.
 * @returns
 * - On error, an error message is returned.
 * - On success, the result of the expression is either "1" or "0".
 */
bool CondParser::parse(const QCString &fileName,int lineNr,const QCString &expr)
{
  if (expr.isEmpty()) return false;
  m_expr      = expr;
  m_tokenType = NOTHING;

  // initialize all variables
  m_e = m_expr.data();    // let m_e point to the start of the expression

  bool answer=false;
  getToken();
  if (m_tokenType==DELIMITER && m_token.isEmpty())
  {
    // empty expression: answer==FALSE
  }
  else if (m_err.isEmpty())
  {
    answer = parseLevel1();
  }
  if (!m_err.isEmpty())
  {
    warn(fileName,lineNr,"problem evaluating expression '{}': {}", expr, m_err);
  }
  //printf("expr='%s' answer=%d\n",expr,answer);
  return answer;
}


/**
 * checks if the given char c is a delimiter
 * minus is checked apart, can be unary minus
 */
static bool isDelimiter(const char c)
{
  return c=='&' || c=='|' || c=='!';
}

/**
 * checks if the given char c is a letter or underscore
 */
static bool isAlpha(const char c)
{
  return (c>='A' && c<='Z') || (c>='a' && c<='z') || c=='_';
}

static bool isAlphaNumSpec(const char c)
{
  return isAlpha(c) || (c>='0' && c<='9') || c=='-' || c=='.' || (static_cast<unsigned char>(c)>=0x80);
}

/**
 * returns the id of the given operator
 * returns -1 if the operator is not recognized
 */
int CondParser::getOperatorId(const QCString &opName)
{
  // level 2
  if (opName=="&&") { return AND; }
  if (opName=="||") { return OR;  }

  // not operator
  if (opName=="!")  { return NOT; }

  return UNKNOWN_OP;
}

/**
 * Get next token in the current string expr.
 * Uses the data in m_expr pointed to by m_e to
 * produce m_tokenType and m_token, set m_err in case of an error
 */
void CondParser::getToken()
{
  m_tokenType = NOTHING;
  m_token.clear();

  //printf("\tgetToken e:{%c}, ascii=%i, col=%i\n", *e, *e, e-expr);

  // skip over whitespaces
  while (*m_e == ' ' || *m_e == '\t' || *m_e == '\n')     // space or tab or newline
  {
    m_e++;
  }

  // check for end of expression
  if (*m_e=='\0')
  {
    // token is still empty
    m_tokenType = DELIMITER;
    return;
  }

  // check for parentheses
  if (*m_e == '(' || *m_e == ')')
  {
    m_tokenType = DELIMITER;
    m_token += *m_e++;
    return;
  }

  // check for operators (delimiters)
  if (isDelimiter(*m_e))
  {
    m_tokenType = DELIMITER;
    while (isDelimiter(*m_e))
    {
      m_token += *m_e++;
    }
    return;
  }

  // check for variables
  if (isAlpha(*m_e))
  {
    m_tokenType = VARIABLE;
    while (isAlphaNumSpec(*m_e))
    {
      m_token += *m_e++;
    }
    return;
  }

  // something unknown is found, wrong characters -> a syntax error
  m_tokenType = UNKNOWN;
  while (*m_e)
  {
    m_token += *m_e++;
  }
  m_err = QCString("Syntax error in part '")+m_token+"'";
  return;
}


/**
 * conditional operators AND and OR
 */
bool CondParser::parseLevel1()
{
  bool ans = parseLevel2();
  int opId = getOperatorId(m_token);

  while (opId==AND || opId==OR)
  {
    getToken();
    ans = evalOperator(opId, ans, parseLevel2());
    opId = getOperatorId(m_token);
  }

  return ans;
}

/**
 * NOT
 */
bool CondParser::parseLevel2()
{
  int opId = getOperatorId(m_token);
  if (opId == NOT)
  {
    getToken();
    return !parseLevel3();
  }
  else
  {
    return parseLevel3();
  }
}


/**
 * parenthesized expression or variable
 */
bool CondParser::parseLevel3()
{
  // check if it is a parenthesized expression
  if (m_tokenType == DELIMITER)
  {
    if (m_token=="(")
    {
      getToken();
      bool ans = parseLevel1();
      if (m_tokenType!=DELIMITER || m_token!=")")
      {
        m_err="Parenthesis ) missing";
        return FALSE;
      }
      getToken();
      return ans;
    }
  }

  // if not parenthesized then the expression is a variable
  return parseVar();
}


bool CondParser::parseVar()
{
  bool ans = false;
  switch (m_tokenType)
  {
    case VARIABLE:
      // this is a variable
      ans = evalVariable(m_token);
      getToken();
      break;

    default:
      // syntax error or unexpected end of expression
      if (m_token.isEmpty())
      {
        m_err="Unexpected end of expression";
        return FALSE;
      }
      else
      {
        m_err="Value expected";
        return FALSE;
      }
      break;
  }
  return ans;
}

/**
 * evaluate an operator for given values
 */
bool CondParser::evalOperator(int opId, bool lhs, bool rhs)
{
  switch (opId)
  {
    // level 2
    case AND: return lhs && rhs;
    case OR:  return lhs || rhs;
  }

  m_err = "Internal error unknown operator: id="+QCString().setNum(opId);
  return FALSE;
}

/**
 * evaluate a variable
 */
bool CondParser::evalVariable(const QCString &varName)
{
  const StringVector &list = Config_getList(ENABLED_SECTIONS);
  return std::find(list.begin(),list.end(),varName.str())!=list.end();
}

