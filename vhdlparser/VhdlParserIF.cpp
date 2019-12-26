
#include "VhdlParserTokenManager.h"
#include "VhdlParserErrorHandler.hpp"
#include "VhdlParser.h"
#include "VhdlParserIF.h"
#include "CharStream.h"

using namespace vhdl::parser;

static VhdlParser * myParser=0;

void  VhdlParserIF::parseVhdlfile(const char* inputBuffer,bool inLine) 
{
  JAVACC_STRING_TYPE s =inputBuffer;
  CharStream *stream = new CharStream(s.c_str(), (int)s.size(), 1, 1);
  VhdlParserTokenManager *tokenManager = new VhdlParserTokenManager(stream);
  VhdlTokenManagerErrorHandler *myTokErr=new VhdlTokenManagerErrorHandler();
  tokenManager->setErrorHandler(myTokErr);
  myParser=new VhdlParser(tokenManager);
  VhdlErrorHandler *myErr=new VhdlErrorHandler();
  myParser->setErrorHandler(myErr);
  try
  {
    if(inLine)
    {
      myParser->parseInline();
    }
    else
    {
      myParser->design_file();
    }
  }
  catch( std::exception &){ /* fprintf(stderr,"\n[%s]",e.what()); */ }
  //  fprintf(stderr,"\n\nparsed lines: %d\n",yyLineNr);
  //  fprintf(stderr,"\n\nerrors : %d\n\n",myErr->getErrorCount());
  delete myParser;
}

void VhdlParser::error_skipto(int kind)
{
  Token *op;
  do
  {
     Token *t = myParser->getNextToken();// step to next token
	 op=myParser->getToken(1);           // get first token
     if (op==0) break;
    //fprintf(stderr,"\n %s",t->image.data());
  } while (op->kind != kind);
  myParser->hasError=false;
  // The above loop consumes tokens all the way up to a token of
  // "kind".  We use a do-while loop rather than a while because the
  // current token is the one immediately before the erroneous token
  // (in our case the token immediately before what should have been
  // "if"/"while".

}
