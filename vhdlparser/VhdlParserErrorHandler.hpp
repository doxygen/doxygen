#ifndef VHDLPARSERERRORHANDLER_H
#define VHDLPARSERERRORHANDLER_H

#include <stdio.h>
#include <stdlib.h>
#include <exception>
#include "VhdlParser.h"
#include "ErrorHandler.h"
#include "vhdlstring.h"

namespace vhdl { namespace parser {

class VhdlErrorHandler: public ErrorHandler 
  {
    virtual void handleUnexpectedToken(int expectedKind, JAVACC_STRING_TYPE expectedToken, Token *actual, VhdlParser *parser) 
    {
      fprintf(stderr,"\n\n syntax error at line: %d : %s\n", actual->beginLine,actual->image.data());
      error_count++;
      throw std::exception();
    }

    virtual void handleParseError(Token *last, Token *unexpected, JAVACC_SIMPLE_STRING production, VhdlParser *parser) 
    {
      fprintf(stderr,"\n\n unexpected token at line: %d %s\n", last->beginLine,unexpected->image.data());
      error_count++;
      throw std::exception();
    }

    virtual void handleOtherError(JAVACC_STRING_TYPE message, VhdlParser *parser)
    {
      fprintf(stderr, "\n\n unexpected error: %s\n", (char*)message.c_str());
      error_count++;
      throw std::exception();
    }
  };
}
}

#endif

