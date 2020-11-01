
#ifndef TOKENPARSER_H
#define TOKENPARSER_H

#include "VhdlParser.h"
// super class for VhdlParserTokenManager 
// is generated in vhdlparser.jj
// -option TOKEN_MANAGER_SUPER_CLASS = "TokenParser"
// sets the Vhdlparser in vhdljjparser.cpp
// tokenManager->setLexParser(vhdlParser);

//class VhdlParser;

namespace vhdl {
  namespace parser {
     class TokenParser {
    public:
      VhdlParser* parser = nullptr;
      void   setLexParser(VhdlParser* p)
      {
        parser = p;
      }
    };
  }
}
#endif
