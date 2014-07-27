#ifndef VHDLPARSERIF
#define VHDLPARSERIF

#include "VhdlParser.h"

class VhdlParserIF
{
  public:
  static void  parseVhdlfile(const char* inputBuffer,bool inLine);

};
#endif
