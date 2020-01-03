#ifndef VHDLPARSERERRORHANDLER_H
#define VHDLPARSERERRORHANDLER_H

#include <stdio.h>
#include <stdlib.h>
#include <exception>
#include "VhdlParser.h"
#include "ErrorHandler.h"
#include "vhdlstring.h"
#include "message.h"

const char *getVhdlFileName(void);

namespace vhdl { namespace parser {

class VhdlErrorHandler: public ErrorHandler
{
  public:
    VhdlErrorHandler(const char *fileName) : m_fileName(fileName) {}

    virtual void handleUnexpectedToken(int expectedKind, JAVACC_STRING_TYPE expectedToken, Token *actual, VhdlParser *parser)
    {
      warn(m_fileName,actual->beginLine,"syntax error '%s'",actual->image.data());
      error_count++;
      throw std::exception();
    }

    virtual void handleParseError(Token *last, Token *unexpected, JAVACC_SIMPLE_STRING production, VhdlParser *parser)
    {
      warn(m_fileName,last->beginLine,"unexpected token: '%s'", unexpected->image.data());
      error_count++;
      throw std::exception();
    }

    virtual void handleOtherError(JAVACC_STRING_TYPE message, VhdlParser *parser)
    {
      warn(m_fileName, -1, "unexpected error: '%s'", (char*)message.c_str());
      error_count++;
      throw std::exception();
    }

  private:
    QCString m_fileName;
};

class VhdlTokenManagerErrorHandler: public TokenManagerErrorHandler
{
  public:
    VhdlTokenManagerErrorHandler(const char *fileName) : m_fileName(fileName) {}

    virtual void lexicalError(bool EOFSeen, int lexState, int errorLine, int errorColumn, JAVACC_STRING_TYPE errorAfter, JAVACC_CHAR_TYPE curChar, VhdlParserTokenManager* token_manager)
    {
      warn(m_fileName,errorLine,"Lexical error, Encountered: '%c' after: '%s'",curChar, (EOFSeen? "EOF" : (const char*)errorAfter.c_str()));
    }

    virtual void lexicalError(JAVACC_STRING_TYPE errorMessage, VhdlParserTokenManager* token_manager)
    {
      warn(m_fileName,-1,"Unknown error: '%s'", (char*)errorMessage.c_str());
    }

  private:
    QCString m_fileName;
};

} }

#endif

