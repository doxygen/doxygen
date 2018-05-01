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
    virtual void handleUnexpectedToken(int expectedKind, JAVACC_STRING_TYPE expectedToken, Token *actual, VhdlParser *parser)
    {
      warn(getVhdlFileName(),actual->beginLine,"syntax error '%s'",actual->image.data());
      error_count++;
      throw std::exception();
    }

    virtual void handleParseError(Token *last, Token *unexpected, JAVACC_SIMPLE_STRING production, VhdlParser *parser)
    {
      warn(getVhdlFileName(),last->beginLine,"unexpected token: '%s'", unexpected->image.data());
      error_count++;
      throw std::exception();
    }

    virtual void handleOtherError(JAVACC_STRING_TYPE message, VhdlParser *parser)
    {
      warn(getVhdlFileName(), -1, "unexpected error: '%s'", (char*)message.c_str());
      error_count++;
      throw std::exception();
    }
  };

class VhdlTokenManagerErrorHandler: public TokenManagerErrorHandler {
    virtual void lexicalError(bool EOFSeen, int lexState, int errorLine, int errorColumn, JAVACC_STRING_TYPE errorAfter, JAVACC_CHAR_TYPE curChar, VhdlParserTokenManager* token_manager)
    {
      warn(getVhdlFileName(),errorLine,"Lexical error, Encountered: '%c' after: '%s'",curChar, (EOFSeen? "EOF" : (const char*)errorAfter.c_str()));
    }

    virtual void lexicalError(JAVACC_STRING_TYPE errorMessage, VhdlParserTokenManager* token_manager)
    {
      warn(getVhdlFileName(),-1,"Unknown error: '%s'", (char*)errorMessage.c_str());
    }
  };
}
}

#endif

