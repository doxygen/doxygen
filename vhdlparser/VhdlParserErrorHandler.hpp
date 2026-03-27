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

    virtual void handleUnexpectedToken(int /* expectedKind */, const JJString& /* expectedToken */, Token *actual, VhdlParser * /* parser */)
    {
      warn(m_fileName,actual->beginLine,"syntax error '{}'",(const char*)actual->image.c_str());
      error_count++;
      throw std::exception();
    }

    virtual void handleParseError(Token *last, Token *unexpected, const JJSimpleString& /* production */, VhdlParser * /* parser */)
    {
      warn(m_fileName,last->beginLine,"unexpected token: '{}'", (const char*)unexpected->image.c_str());
      error_count++;
      throw std::exception();
    }

    virtual void handleOtherError(const JJString& message, VhdlParser * /* parser */)
    {
      warn(m_fileName, -1, "unexpected error: '{}'", (const char*)message.c_str());
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

    virtual void lexicalError(bool EOFSeen, int /* lexState */, int errorLine, int /* errorColumn */, const JJString& errorAfter, JJChar curChar, VhdlParserTokenManager* /* token_manager */)
    {
      warn(m_fileName,errorLine,"Lexical error, Encountered: '{:c}' after: '{}'",(char)curChar, (EOFSeen? "EOF" : (const char*)errorAfter.c_str()));
    }

    virtual void lexicalError(const JJString& errorMessage, VhdlParserTokenManager* /* token_manager */)
    {
      warn(m_fileName,-1,"Unknown error: '{}'", (const char*)errorMessage.c_str());
    }

  private:
    QCString m_fileName;
};

} }

#endif

