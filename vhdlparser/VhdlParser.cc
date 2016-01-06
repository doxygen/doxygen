/* VhdlParser.cc */
#include "VhdlParser.h"
#include "TokenMgrError.h"
namespace vhdl {
namespace parser {
  unsigned int jj_la1_0[] = {
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x20000,0x0,0x90404000,0x20080000,0x40000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x90004000,0x400000,0x0,0x90404000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x20080000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x40000,0x0,0x0,0x0,0x0,0x20000000,0x0,0x0,0x40000000,0x0,0x400000,0x400000,0x0,0x0,0x0,0x0,0x0,0x0,0x40020000,0x40020001,0x0,0x0,0x0,0x40000000,0xd0020000,0x0,0x0,0x800000,0x0,0x0,0x80004000,0x400000,0x0,0x0,0x80404000,0x0,0x0,0x0,0x0,0x8000,0x0,0x80000,0x0,0x0,0x0,0x0,0x0,0x0,0x10000,0x10000,0x0,0x800,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x800,0x10000000,0x0,0x50000000,0x8000,0x0,0x80000000,0x0,0x80000000,0x80000000,0x4000000,0x8000000,0x0,0x0,0x20000,0x0,0x0,0x0,0x0,0x800,0x4000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x80000000,0x0,0x0,0x0,0x0,0x80004000,0x0,0x80004000,0x0,0x0,0x0,0x0,0x90004000,0x400000,0x0,0x0,0x90404000,0x0,0x0,0x0,0x40000000,0x0,0x0,0x80004000,0x400000,0x0,0x80404000,0x0,0x0,0x8000,0x0,0x0,0x0,0x0,0x20080000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x800,0x0,0x20000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x8000000,0x0,0x8000000,0x0,0x8000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x80004000,0x400000,0x0,0x80404000,0x0,0x0,0x0,0x0,0x0,0x0,0x20080000,0x8000,0x0,0x0,0x41000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x800,0x2000,0x0,0x80004000,0x400000,0x0,0x80404000,0x0,0x400000,0x400000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x90c04000,0x20080000,0x80000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x800,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,};
  unsigned int jj_la1_1[] = {
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x125808,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x10000,0x0,0x0,0x1000,0x104800,0x1008,0x20000,0x125808,0x10000,0x0,0x0,0x0,0x0,0x1000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x10000,0x1000,0x1000000,0x10000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x20000,0x20000,0x1000,0x0,0x4000000,0x4000000,0x0,0x400000,0x4000101,0x4000101,0x0,0x10,0x0,0x100,0x12024900,0x0,0x0,0x0,0x100,0x0,0x104800,0x8,0x20000,0x0,0x124808,0x0,0x10000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1000000,0x0,0x0,0x0,0x0,0x81000,0x0,0x0,0x0,0x0,0x0,0x40,0x20,0x0,0x0,0x0,0x0,0x0,0x100,0x0,0x0,0x800,0x0,0x0,0x0,0x8a00000,0x0,0x0,0x1000,0x1,0x0,0x0,0x1000,0x0,0x0,0x8a00000,0x80000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x800,0x40000,0x400000,0x0,0x0,0x104800,0x20000,0x124800,0x0,0x0,0x10000,0x10000,0x104800,0x8,0x20000,0x0,0x124808,0x0,0x0,0x0,0x100,0x0,0x0,0x104800,0x0,0x20000,0x124800,0x0,0x0,0x0,0x0,0x1000000,0x0,0x0,0x20081200,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x400000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x4000,0x0,0x104000,0x1000000,0x1104800,0x0,0x20000,0x1124800,0x4000,0x0,0x100000,0x100000,0x0,0x104000,0x20081200,0x0,0x0,0x0,0x800,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1000,0x0,0x0,0x0,0x0,0x1104800,0x0,0x20000,0x1124800,0x0,0x104000,0x104000,0x4000001,0x1,0x0,0x4000001,0x10000,0x10000,0x10000,0x0,0x0,0x125808,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x400000,0x20,0x0,0x200000,0x0,0x0,0x0,0x0,0x0,0x400000,0x0,0x200000,0x0,0x0,0x0,0x200000,0x0,0x20,0x104000,0x100000,0x100000,0x0,0x1000000,0x0,0x0,0x104000,};
  unsigned int jj_la1_2[] = {
0x0,0x100,0x0,0x0,0x0,0x0,0x0,0x0,0x2,0x0,0x0,0x110000,0x28000,0x0,0x20000000,0x0,0x0,0x0,0x200000,0x0,0x0,0x0,0x4000,0x0,0x0,0x110000,0x0,0x0,0x110000,0x0,0x4000,0x4000,0x0,0x0,0x0,0x0,0x28000,0x0,0x0,0x0,0x400,0x0,0x4000,0x0,0x0,0x0,0x4000,0x0,0x4000,0x400000,0x8000,0x8000,0x8000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1000000,0x1000000,0x1000,0x1000,0x0,0x0,0x0,0x100,0x11000,0x0,0x0,0x0,0x0,0x0,0x110000,0x0,0x0,0x1000,0x111000,0x0,0x0,0x4000,0x0,0x400,0x8000,0x28000,0x0,0x0,0x0,0x0,0x0,0x0,0x209,0x209,0x0,0x32,0x100,0x100,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x32,0x0,0x0,0x0,0x400,0x1000,0x0,0x0,0x0,0x0,0x800,0x0,0x0,0x0,0x1000,0x20,0x0,0x0,0x0,0x10,0x800,0x10000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1000000,0x1000,0x0,0x110000,0x0,0x110000,0x1000,0x0,0x0,0x0,0x110000,0x0,0x0,0x1000,0x111000,0x0,0x0,0x2,0x0,0x1000,0x0,0x110000,0x0,0x0,0x110000,0x0,0x8000,0x0,0x0,0x0,0x8000,0x0,0x20000024,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x32,0x200000,0x1000,0x0,0x0,0x20,0xc0000000,0xc0000000,0x0,0x1000000,0x0,0x800000,0x0,0x800000,0x0,0x400,0x0,0x0,0x0,0x0,0x10000,0x0,0x110000,0x0,0x110000,0x0,0x0,0x110000,0x10000,0x0,0x100000,0x100000,0x0,0x110000,0x20000024,0x0,0x0,0x0,0x600000,0x80000,0x0,0x0,0x0,0x0,0x0,0x0,0x80,0x0,0x0,0x0,0x32,0x0,0x0,0x110000,0x0,0x0,0x110000,0x0,0x110000,0x110000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x110000,0x28000,0x0,0x0,0x0,0x0,0x0,0x0,0x1000000,0x0,0x0,0x800,0x32,0x0,0x0,0x0,0x0,0x1000000,0x0,0x800,0x0,0x0,0x0,0x800,0x0,0x0,0x110000,0x100000,0x100000,0x0,0x0,0x2000,0x0,0x138000,};
  unsigned int jj_la1_3[] = {
0x0,0x0,0x0,0x40000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x188830,0x8000000,0x0,0x0,0x8,0x0,0x0,0x0,0x0,0x80000,0x0,0x0,0x80000,0x0,0x108830,0x80000,0x0,0x188830,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x8000000,0x0,0x2000000,0x0,0x0,0x0,0x80000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x8000000,0x0,0x0,0x2000000,0x0,0x0,0x80000,0x80000,0x0,0x0,0x80000,0x80000,0x0,0x4000,0x80000,0x80000,0x0,0x2000,0x0,0x0,0x128810,0x0,0x0,0x0,0x0,0x0,0x108830,0x80000,0x0,0x0,0x188830,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2000000,0x30000000,0x30000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x108030,0x0,0x100030,0x100030,0x0,0x0,0x0,0x4000000,0x0,0x0,0x0,0x4000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2000000,0x0,0x0,0x108030,0x0,0x4000,0x0,0x0,0x188820,0x0,0x188820,0x0,0x0,0x0,0x0,0x108830,0x80000,0x0,0x0,0x188830,0x0,0x0,0x0,0x0,0x0,0x0,0x108820,0x80000,0x0,0x188820,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xd000001,0x0,0x0,0x0,0x0,0x0,0x0,0x8,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x3c0,0x3c0,0x0,0x4000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x0,0x0,0x0,0x108820,0x80000,0x0,0x188820,0x0,0x0,0x0,0x0,0x0,0x0,0xd000001,0x0,0x0,0x8000,0x0,0x0,0x0,0x0,0x8000000,0x20,0x0,0x0,0x0,0x40000,0x0,0x0,0x10000,0x0,0x0,0x108820,0x80000,0x0,0x188820,0x0,0x80000,0x80000,0x80000,0x0,0x0,0x80000,0x0,0x0,0x0,0x2000000,0x0,0x188830,0x8000000,0x100010,0x0,0x0,0x0,0x0,0x0,0x4000,0x0,0x2000000,0x0,0x0,0x8000000,0x0,0x0,0x0,0x4000,0x0,0x0,0x0,0x0,0x0,0x0,0x2000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,};
  unsigned int jj_la1_4[] = {
0x0,0x0,0x60000,0x30,0x40,0x100,0x0,0x0,0x0,0x0,0x40000,0x0,0x40001,0x0,0x0,0x0,0x0,0x40,0x0,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x0,0x0,0x40001,0x0,0x0,0x0,0x0,0x10000000,0x200,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x40000,0x0,0x0,0x0,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x20000,0x40,0x0,0x0,0x40000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x40000,0x40000,0x40000,0x40000,0x40000,0x0,0x40,0x0,0x0,0x0,0x0,0x0,0x4,0x40001,0x0,0x0,0x40000,0x1,0x0,0x0,0x40000,0x40,0x0,0x40,0x0,0x0,0x0,0x0,0x40,0x40031,0x0,0x1,0x0,0x0,0x0,0x0,0x200,0x0,0x0,0x0,0x0,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x4,0x0,0x40000008,0x40000,0x40,0x1,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x40000,0x0,0x0,0x0,0x0,0x40000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x40000,0x1,0x0,0x0,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x40000,0x1,0x0,0x0,0x0,0x40001,0x1,0x0,0x40000,0xfc00,0xfc00,0x0,0x0,0x0,0x40031,0x1,0x0,0x40,0x40,0x0,0x0,0x0,0x30,0x0,0x40001,0x0,0x80,0x0,0x40,0x40000,0x40,0x40000,0x0,0x30,0x0,0x0,0x0,0x200,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x1,0x0,0x40001,0x0,0x40001,0x0,0x1,0x0,0x40,0x40,0x40001,0x0,0x80,0x0,0x0,0x0,0x0,0x40,0x40031,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x40001,0x0,0xa0000000,0x20000000,0x0,0x1,0x40001,0x0,0x0,0x0,0x0,0x40031,0x0,0x8000000,0x40000,0x8000000,0x0,0x8000000,0x0,0x240,0x240,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x20000,0x0,0x0,0x1,0x0,};
  unsigned int jj_la1_5[] = {
0x1420,0x0,0x1c0,0x0,0x0,0x0,0x2,0x3c0,0x0,0x0,0x1c0,0x0,0x80001c0,0x0,0x0,0x0,0x180,0x0,0x180,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x180,0x80001c0,0x180,0x0,0x180,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x180,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x8000000,0x0,0x0,0x1c0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1c0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1c0,0x0,0x0,0x0,0x8000000,0x8000000,0x2,0x0,0x0,0x3c0,0x3c0,0x1c0,0x1c0,0x3c0,0x380,0x0,0x180,0x180,0x0,0x0,0x0,0x0,0x817e0,0x0,0x0,0x1e0,0x0,0x180,0x0,0x3c0,0x0,0x180,0x0,0x180,0x0,0x0,0x180,0x0,0x817e0,0x0,0x0,0x180,0x180,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x40,0x180,0x0,0x180,0x0,0x0,0x0,0x1c0,0x0,0x0,0x0,0x180,0x180,0x0,0x180,0x1420,0x0,0x0,0x0,0x0,0x1c0,0x0,0x0,0x0,0x0,0x1c0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x180,0x1c0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x180,0x0,0x1c0,0x0,0x0,0x0,0x180,0x1c0,0x0,0x180,0x1c0,0x0,0x0,0x180,0x0,0x180,0x817e0,0x0,0x0,0x0,0x0,0x180,0x0,0x0,0x0,0x0,0x1c0,0x0,0x0,0x0,0x0,0x1c0,0x0,0x1c0,0x0,0x0,0x0,0x1c0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1c0,0x240,0x1c0,0x0,0x0,0x0,0x0,0x0,0x1c0,0x0,0x0,0x180,0x0,0x0,0x0,0x0,0x817e0,0x0,0x180,0x0,0x0,0x0,0x0,0x180,0x0,0x0,0x0,0x0,0x180,0x0,0x0,0x0,0x0,0x0,0x180,0x0,0x80001c0,0x0,0x1,0x0,0x1,0x0,0x1c0,0x0,0x0,0x0,0x0,0x817e0,0x0,0x0,0x1c0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x180,0x0,0x0,0x0,0x180,};

  /** Constructor with user supplied TokenManager. */




QCString VhdlParser::abstract_literal() {Token *tok;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case DECIMAL_LITERAL:{if (!hasError) {
      
      tok = jj_consume_token(DECIMAL_LITERAL);
      }
      if (!hasError) {
      
return tok->image.c_str();
      }
      
      break;
      }
    case INTEGER:{if (!hasError) {
      
      tok = jj_consume_token(INTEGER);
      }
      if (!hasError) {
      
return tok->image.c_str();
      }
      
      break;
      }
    case BASED_LITERAL:{if (!hasError) {
      
      tok = jj_consume_token(BASED_LITERAL);
      }
      if (!hasError) {
      
return tok->image.c_str();
      }
      
      break;
      }
    default:
      jj_la1[0] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


QCString VhdlParser::access_type_definition() {Token *tok=0;QCString str,str1;if (!hasError) {
    
    tok = jj_consume_token(ACCESS_T);
    }
    if (!hasError) {
    
    str1 = subtype_indication();
    }
    
str=tok->image.c_str(); return str+str1;
assert(false);
}


QCString VhdlParser::actual_designator() {QCString str;Token *t=0;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case OPEN_T:{if (!hasError) {
      
      t = jj_consume_token(OPEN_T);
      }
      if (!hasError) {
      
return t->image.c_str();
      }
      
      break;
      }
    default:
      jj_la1[1] = jj_gen;
      if (jj_2_1(2147483647)) {if (!hasError) {
        
        str = expression();
        }
        if (!hasError) {
        
return str;
        }
        
      } else if (jj_2_2(2147483647)) {if (!hasError) {
        
        str = name();
        }
        if (!hasError) {
        
return str;
        }
        
      } else {
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
assert(false);
return QCString();
}


QCString VhdlParser::actual_parameter_part() {QCString s;if (!hasError) {
    
    s = association_list();
    }
    
return s;
assert(false);
}


QCString VhdlParser::actual_part() {QCString s,s1;
    if (jj_2_3(2147483647)) {if (!hasError) {
      
      s = actual_designator();
      }
      if (!hasError) {
      
return s;
      }
      
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case BOX_T:{if (!hasError) {
        
        jj_consume_token(BOX_T);
        }
        if (!hasError) {
        
return "<>";
        }
        
        break;
        }
      case SLSL_T:
      case STRINGLITERAL:
      case BASIC_IDENTIFIER:
      case EXTENDED_CHARACTER:{if (!hasError) {
        
        s = name();
        }
        if (!hasError) {
        
        jj_consume_token(LPAREN_T);
        }
        if (!hasError) {
        
        s1 = actual_designator();
        }
        if (!hasError) {
        
        jj_consume_token(RPAREN_T);
        }
        if (!hasError) {
        
s+="(";s+=s1+")";return s;
        }
        
        break;
        }
      default:
        jj_la1[2] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
assert(false);
return QCString();
}


QCString VhdlParser::adding_operator() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case PLUS_T:{if (!hasError) {
      
      jj_consume_token(PLUS_T);
      }
      if (!hasError) {
      
return "+";
      }
      
      break;
      }
    case MINUS_T:{if (!hasError) {
      
      jj_consume_token(MINUS_T);
      }
      if (!hasError) {
      
return "-";
      }
      
      break;
      }
    case AMPERSAND_T:{if (!hasError) {
      
      jj_consume_token(AMPERSAND_T);
      }
      if (!hasError) {
      
return "&";
      }
      
      break;
      }
    default:
      jj_la1[3] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


QCString VhdlParser::aggregate() {QCString s,s1,s2;if (!hasError) {
    
    jj_consume_token(LPAREN_T);
    }
    if (!hasError) {
    
    s = element_association();
    }
    if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case COMMA_T:{
        ;
        break;
        }
      default:
        jj_la1[4] = jj_gen;
        goto end_label_1;
      }if (!hasError) {
      
      jj_consume_token(COMMA_T);
      }
      if (!hasError) {
      
      s1 = element_association();
      }
      if (!hasError) {
      
s+=","+s1;
      }
      
    }
    end_label_1: ;
    }
    if (!hasError) {
    
    jj_consume_token(RPAREN_T);
    }
    
return "("+s+")";
assert(false);
}


QCString VhdlParser::alias_declaration() {QCString s,s1,s2;if (!hasError) {
    
    jj_consume_token(ALIAS_T);
    }
    if (!hasError) {
    
    s2 = alias_designator();
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case COLON_T:{if (!hasError) {
      
      jj_consume_token(COLON_T);
      }
      if (!hasError) {
      
s+=":";
      }
      if (!hasError) {
      
      s1 = subtype_indication();
      }
      if (!hasError) {
      
s+=s1;
      }
      
      break;
      }
    default:
      jj_la1[5] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(IS_T);
    }
    if (!hasError) {
    
s+=" is ";
    }
    if (!hasError) {
    
    s1 = name();
    }
    if (!hasError) {
    
s+=s1;
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LBRACKET_T:{if (!hasError) {
      
      s1 = signature();
      }
      if (!hasError) {
      
s+=s1;
      }
      
      break;
      }
    default:
      jj_la1[6] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
addVhdlType(s2.data(),getLine(ALIAS_T),Entry::VARIABLE_SEC,VhdlDocGen::ALIAS,0,s.data(),Public);

 return s2+" "+s+";";
assert(false);
}


QCString VhdlParser::alias_designator() {Token *tok=0;QCString s;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      s = identifier();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    case CHARACTER_LITERAL:{if (!hasError) {
      
      tok = jj_consume_token(CHARACTER_LITERAL);
      }
      if (!hasError) {
      
return tok->image.c_str();
      }
      
      break;
      }
    case STRINGLITERAL:{if (!hasError) {
      
      s = operator_symbol();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    default:
      jj_la1[7] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


void VhdlParser::allocator() {
    if (jj_2_4(3)) {if (!hasError) {
      
      jj_consume_token(NEW_T);
      }
      if (!hasError) {
      
      qualified_expression();
      }
      
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case NEW_T:{if (!hasError) {
        
        jj_consume_token(NEW_T);
        }
        if (!hasError) {
        
        subtype_indication();
        }
        
        break;
        }
      default:
        jj_la1[8] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
}


void VhdlParser::architecture_body() {QCString s,s1;if (!hasError) {
    
    jj_consume_token(ARCHITECTURE_T);
    }
    if (!hasError) {
    
    s = identifier();
    }
    if (!hasError) {
    
    jj_consume_token(OF_T);
    }
    if (!hasError) {
    
    s1 = name();
    }
    if (!hasError) {
    
    jj_consume_token(IS_T);
    }
    if (!hasError) {
    
QCString t=s1+"::"+s;
                  genLabels.resize(0);
                  pushLabel(genLabels,s1);
                  lastCompound=current;
                  addVhdlType(t,getLine(ARCHITECTURE_T),Entry::CLASS_SEC,VhdlDocGen::ARCHITECTURE,0,0,Private);
    }
    if (!hasError) {
    
    try {if (!hasError) {
      
      architecture_declarative_part();
      }
      
    } catch ( ...) {
error_skipto(BEGIN_T);
    }
    }
    if (!hasError) {
    
    jj_consume_token(BEGIN_T);
    }
    if (!hasError) {
    
    architecture_statement_part();
    }
    if (!hasError) {
    
    jj_consume_token(END_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case ARCHITECTURE_T:{if (!hasError) {
      
      jj_consume_token(ARCHITECTURE_T);
      }
      
      break;
      }
    default:
      jj_la1[9] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      name();
      }
      
      break;
      }
    default:
      jj_la1[10] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
lastEntity=0;lastCompound=0; genLabels.resize(0);
}


void VhdlParser::architecture_declarative_part() {if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ALIAS_T:
      case ATTRIBUTE_T:
      case COMPONENT_T:
      case CONSTANT_T:
      case DISCONNECT_T:
      case FILE_T:
      case FOR_T:
      case FUNCTION_T:
      case GROUP_T:
      case IMPURE_T:
      case PROCEDURE_T:
      case PURE_T:
      case SIGNAL_T:
      case SHARED_T:
      case SUBTYPE_T:
      case TYPE_T:
      case USE_T:
      case VARIABLE_T:{
        ;
        break;
        }
      default:
        jj_la1[11] = jj_gen;
        goto end_label_2;
      }if (!hasError) {
      
      block_declarative_item();
      }
      
    }
    end_label_2: ;
    }
    
}


void VhdlParser::architecture_statement_part() {if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ASSERT_T:
      case CASE_T:
      case POSTPONED_T:
      case PROCESS_T:
      case WITH_T:
      case LPAREN_T:
      case SLSL_T:
      case STRINGLITERAL:
      case BASIC_IDENTIFIER:
      case EXTENDED_CHARACTER:
      case VHDL2008TOOLDIR:{
        ;
        break;
        }
      default:
        jj_la1[12] = jj_gen;
        goto end_label_3;
      }if (!hasError) {
      
      concurrent_statement();
      }
      
    }
    end_label_3: ;
    }
    
}


QCString VhdlParser::array_type_definition() {QCString s;
    if (jj_2_5(2147483647)) {if (!hasError) {
      
      s = unconstraint_array_definition();
      }
      if (!hasError) {
      
return s;
      }
      
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ARRAY_T:{if (!hasError) {
        
        s = constraint_array_definition();
        }
        if (!hasError) {
        
return s;
        }
        
        break;
        }
      default:
        jj_la1[13] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
assert(false);
return QCString();
}


QCString VhdlParser::assertion() {QCString s,s1,s2;Token *t=0;Token *t1=0;if (!hasError) {
    
    jj_consume_token(ASSERT_T);
    }
    if (!hasError) {
    
    s = condition();
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case REPORT_T:{if (!hasError) {
      
      t = jj_consume_token(REPORT_T);
      }
      if (!hasError) {
      
      s1 = expression();
      }
      
      break;
      }
    default:
      jj_la1[14] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SEVERITY_T:{if (!hasError) {
      
      t1 = jj_consume_token(SEVERITY_T);
      }
      if (!hasError) {
      
      s2 = expression();
      }
      
      break;
      }
    default:
      jj_la1[15] = jj_gen;
      ;
    }
    }
    
s.prepend("assert ");
   if(t) s1.prepend(" report ");
   if(t1) s2.prepend(" report ");
    return s+s1+s2;
assert(false);
}


QCString VhdlParser::assertion_statement() {QCString s,s1,s2;Token *t=0;if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      s = label();
      }
      if (!hasError) {
      
      t = jj_consume_token(COLON_T);
      }
      
      break;
      }
    default:
      jj_la1[16] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    s1 = assertion();
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
if(t) s+=":";
    return s+s1+";";
assert(false);
}


QCString VhdlParser::association_element() {QCString s,s1;if (!hasError) {
    
    if (jj_2_6(2147483647)) {if (!hasError) {
      
      s = formal_part();
      }
      if (!hasError) {
      
      jj_consume_token(ARROW_T);
      }
      
    } else {
      ;
    }
    }
    if (!hasError) {
    
    s1 = actual_part();
    }
    
return s+" => "+s1;
assert(false);
}


QCString VhdlParser::association_list() {QCString s,s1;if (!hasError) {
    
    s = association_element();
    }
    if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case COMMA_T:{
        ;
        break;
        }
      default:
        jj_la1[17] = jj_gen;
        goto end_label_4;
      }if (!hasError) {
      
      jj_consume_token(COMMA_T);
      }
      if (!hasError) {
      
      s1 = association_element();
      }
      if (!hasError) {
      
s+=","+s1;
      }
      
    }
    end_label_4: ;
    }
    
return s;
assert(false);
}


QCString VhdlParser::attribute_declaration() {QCString s,s1;if (!hasError) {
    
    jj_consume_token(ATTRIBUTE_T);
    }
    if (!hasError) {
    
    s = identifier();
    }
    if (!hasError) {
    
    jj_consume_token(COLON_T);
    }
    if (!hasError) {
    
    s1 = type_mark();
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
addVhdlType(s.data(),getLine(ATTRIBUTE_T),Entry::VARIABLE_SEC,VhdlDocGen::ATTRIBUTE,0,s1.data(),Public);
    return " attribute "+s+":"+s1+";";
assert(false);
}


QCString VhdlParser::attribute_designator() {QCString s;Token *tok=0;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      s = identifier();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    case RANGE_T:{if (!hasError) {
      
      tok = jj_consume_token(RANGE_T);
      }
      if (!hasError) {
      
return tok->image.c_str();
      }
      
      break;
      }
    default:
      jj_la1[18] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


QCString VhdlParser::attribute_name() {QCString s,s1;if (!hasError) {
    
    s = identifier();
    }
    if (!hasError) {
    
    jj_consume_token(APOSTROPHE_T);
    }
    if (!hasError) {
    
    s1 = name();
    }
    if (!hasError) {
    
s+="'"+s1;
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LPAREN_T:{if (!hasError) {
      
      jj_consume_token(LPAREN_T);
      }
      if (!hasError) {
      
      s1 = expression();
      }
      if (!hasError) {
      
      jj_consume_token(RPAREN_T);
      }
      if (!hasError) {
      
s+"("+s1+")";
      }
      
      break;
      }
    default:
      jj_la1[19] = jj_gen;
      ;
    }
    }
    
return s;
assert(false);
}


QCString VhdlParser::attribute_specification() {QCString s,s1,s2;if (!hasError) {
    
    jj_consume_token(ATTRIBUTE_T);
    }
    if (!hasError) {
    
    s = attribute_designator();
    }
    if (!hasError) {
    
    jj_consume_token(OF_T);
    }
    if (!hasError) {
    
    s1 = entity_specification();
    }
    if (!hasError) {
    
    jj_consume_token(IS_T);
    }
    if (!hasError) {
    
    s2 = expression();
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
QCString t= s1+" is "+s2;
   addVhdlType(s.data(),getLine(ATTRIBUTE_T),Entry::VARIABLE_SEC,VhdlDocGen::ATTRIBUTE,0,t.data(),Public);
   return " attribute "+s+" of "+s1+ " is "+s2+";";
assert(false);
}


QCString VhdlParser::base() {Token *tok=0;if (!hasError) {
    
    tok = jj_consume_token(INTEGER);
    }
    
return tok->image.c_str();
assert(false);
}


QCString VhdlParser::base_specifier() {Token *tok=0;if (!hasError) {
    
    tok = jj_consume_token(BASIC_IDENTIFIER);
    }
    
return tok->image.c_str();
assert(false);
}


QCString VhdlParser::base_unit_declaration() {QCString s;if (!hasError) {
    
    s = identifier();
    }
    
return s;
assert(false);
}


QCString VhdlParser::based_integer() {Token *tok=0;if (!hasError) {
    
    tok = jj_consume_token(BASIC_IDENTIFIER);
    }
    
return tok->image.c_str();
assert(false);
}


QCString VhdlParser::based_literal() {Token *tok=0;if (!hasError) {
    
    tok = jj_consume_token(BASED_LITERAL);
    }
    
return tok->image.c_str();
assert(false);
}


QCString VhdlParser::basic_identifier() {Token *tok=0;if (!hasError) {
    
    tok = jj_consume_token(BASIC_IDENTIFIER);
    }
    
return tok->image.c_str();
assert(false);
}


void VhdlParser::binding_indication() {if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case USE_T:{if (!hasError) {
      
      jj_consume_token(USE_T);
      }
      if (!hasError) {
      
      entity_aspect();
      }
      
      break;
      }
    default:
      jj_la1[20] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case GENERIC_T:{if (!hasError) {
      
      generic_map_aspect();
      }
      
      break;
      }
    default:
      jj_la1[21] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case PORT_T:{if (!hasError) {
      
      port_map_aspect();
      }
      
      break;
      }
    default:
      jj_la1[22] = jj_gen;
      ;
    }
    }
    
}


QCString VhdlParser::bit_string_literal() {Token *tok=0;if (!hasError) {
    
    tok = jj_consume_token(BIT_STRING_LITERAL);
    }
    
return tok->image.c_str();
assert(false);
}


QCString VhdlParser::bit_value() {Token *tok=0;if (!hasError) {
    
    tok = jj_consume_token(BASIC_IDENTIFIER);
    }
    
return tok->image.c_str();
assert(false);
}


void VhdlParser::block_configuration() {if (!hasError) {
    
    jj_consume_token(FOR_T);
    }
    if (!hasError) {
    
    block_specification();
    }
    if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case USE_T:{
        ;
        break;
        }
      default:
        jj_la1[23] = jj_gen;
        goto end_label_5;
      }if (!hasError) {
      
      use_clause();
      }
      
    }
    end_label_5: ;
    }
    if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case FOR_T:{
        ;
        break;
        }
      default:
        jj_la1[24] = jj_gen;
        goto end_label_6;
      }if (!hasError) {
      
      configuration_item();
      }
      
    }
    end_label_6: ;
    }
    if (!hasError) {
    
    jj_consume_token(END_T);
    }
    if (!hasError) {
    
    jj_consume_token(FOR_T);
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
}


void VhdlParser::block_declarative_item() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case FUNCTION_T:
    case IMPURE_T:
    case PROCEDURE_T:
    case PURE_T:{if (!hasError) {
      
      subprogram_declaration();
      }
      
      break;
      }
    case TYPE_T:{if (!hasError) {
      
      type_declaration();
      }
      
      break;
      }
    case SUBTYPE_T:{if (!hasError) {
      
      subtype_declaration();
      }
      
      break;
      }
    case CONSTANT_T:{if (!hasError) {
      
      constant_declaration();
      }
      
      break;
      }
    case SIGNAL_T:{if (!hasError) {
      
      signal_declaration();
      }
      
      break;
      }
    case SHARED_T:
    case VARIABLE_T:{if (!hasError) {
      
      variable_declaration();
      }
      
      break;
      }
    case FILE_T:{if (!hasError) {
      
      file_declaration();
      }
      
      break;
      }
    case ALIAS_T:{if (!hasError) {
      
      alias_declaration();
      }
      
      break;
      }
    case COMPONENT_T:{if (!hasError) {
      
      component_declaration();
      }
      
      break;
      }
    default:
      jj_la1[25] = jj_gen;
      if (jj_2_7(2147483647)) {if (!hasError) {
        
        attribute_declaration();
        }
        
      } else {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case ATTRIBUTE_T:{if (!hasError) {
          
          attribute_specification();
          }
          
          break;
          }
        case FOR_T:{if (!hasError) {
          
          configuration_specification();
          }
          
          break;
          }
        case DISCONNECT_T:{if (!hasError) {
          
          disconnection_specification();
          }
          
          break;
          }
        case USE_T:{if (!hasError) {
          
          use_clause();
          }
          
          break;
          }
        default:
          jj_la1[26] = jj_gen;
          if (jj_2_8(3)) {if (!hasError) {
            
            group_template_declaration();
            }
            
          } else {
            switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
            case GROUP_T:{if (!hasError) {
              
              group_declaration();
              }
              
              break;
              }
            default:
              jj_la1[27] = jj_gen;
              jj_consume_token(-1);
              errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
            }
          }
        }
      }
    }
}


void VhdlParser::block_declarative_part() {if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ALIAS_T:
      case ATTRIBUTE_T:
      case COMPONENT_T:
      case CONSTANT_T:
      case DISCONNECT_T:
      case FILE_T:
      case FOR_T:
      case FUNCTION_T:
      case GROUP_T:
      case IMPURE_T:
      case PROCEDURE_T:
      case PURE_T:
      case SIGNAL_T:
      case SHARED_T:
      case SUBTYPE_T:
      case TYPE_T:
      case USE_T:
      case VARIABLE_T:{
        ;
        break;
        }
      default:
        jj_la1[28] = jj_gen;
        goto end_label_7;
      }if (!hasError) {
      
      block_declarative_item();
      }
      
    }
    end_label_7: ;
    }
    
}


void VhdlParser::block_header() {if (!hasError) {
    
    if (jj_2_9(2147483647)) {if (!hasError) {
      
      generic_clause();
      }
      if (!hasError) {
      
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case GENERIC_T:{if (!hasError) {
        
        generic_map_aspect();
        }
        if (!hasError) {
        
        jj_consume_token(SEMI_T);
        }
        
        break;
        }
      default:
        jj_la1[29] = jj_gen;
        ;
      }
      }
      
    } else {
      ;
    }
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case PORT_T:{if (!hasError) {
      
      port_clause();
      }
      if (!hasError) {
      
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case PORT_T:{if (!hasError) {
        
        port_map_aspect();
        }
        if (!hasError) {
        
        jj_consume_token(SEMI_T);
        }
        
        break;
        }
      default:
        jj_la1[30] = jj_gen;
        ;
      }
      }
      
      break;
      }
    default:
      jj_la1[31] = jj_gen;
      ;
    }
    }
    
}


void VhdlParser::block_specification() {if (!hasError) {
    
    name();
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LPAREN_T:{if (!hasError) {
      
      jj_consume_token(LPAREN_T);
      }
      if (!hasError) {
      
      index_specification();
      }
      if (!hasError) {
      
      jj_consume_token(RPAREN_T);
      }
      
      break;
      }
    default:
      jj_la1[32] = jj_gen;
      ;
    }
    }
    
}


void VhdlParser::block_statement() {QCString s;if (!hasError) {
    
    s = identifier();
    }
    if (!hasError) {
    
    jj_consume_token(COLON_T);
    }
    if (!hasError) {
    
    jj_consume_token(BLOCK_T);
    }
    if (!hasError) {
    
pushLabel(genLabels,s);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LPAREN_T:{if (!hasError) {
      
      jj_consume_token(LPAREN_T);
      }
      if (!hasError) {
      
      expression();
      }
      if (!hasError) {
      
      jj_consume_token(RPAREN_T);
      }
      
      break;
      }
    default:
      jj_la1[33] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case IS_T:{if (!hasError) {
      
      jj_consume_token(IS_T);
      }
      
      break;
      }
    default:
      jj_la1[34] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    block_header();
    }
    if (!hasError) {
    
    block_declarative_part();
    }
    if (!hasError) {
    
    jj_consume_token(BEGIN_T);
    }
    if (!hasError) {
    
    block_statement_part();
    }
    if (!hasError) {
    
    jj_consume_token(END_T);
    }
    if (!hasError) {
    
    jj_consume_token(BLOCK_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      identifier();
      }
      
      break;
      }
    default:
      jj_la1[35] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
genLabels=popLabel(genLabels);
}


void VhdlParser::block_statement_part() {if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ASSERT_T:
      case CASE_T:
      case POSTPONED_T:
      case PROCESS_T:
      case WITH_T:
      case LPAREN_T:
      case SLSL_T:
      case STRINGLITERAL:
      case BASIC_IDENTIFIER:
      case EXTENDED_CHARACTER:
      case VHDL2008TOOLDIR:{
        ;
        break;
        }
      default:
        jj_la1[36] = jj_gen;
        goto end_label_8;
      }if (!hasError) {
      
      concurrent_statement();
      }
      
    }
    end_label_8: ;
    }
    
}


void VhdlParser::case_statement() {QCString s;if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      identifier();
      }
      if (!hasError) {
      
      jj_consume_token(COLON_T);
      }
      
      break;
      }
    default:
      jj_la1[37] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(CASE_T);
    }
    if (!hasError) {
    
    s = expression();
    }
    if (!hasError) {
    
QCString ca="case "+s;
     FlowChart::addFlowChart(FlowChart::CASE_NO,0,ca);
    }
    if (!hasError) {
    
    jj_consume_token(IS_T);
    }
    if (!hasError) {
    
    case_statement_alternative();
    }
    if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case WHEN_T:{
        ;
        break;
        }
      default:
        jj_la1[38] = jj_gen;
        goto end_label_9;
      }if (!hasError) {
      
      case_statement_alternative();
      }
      
    }
    end_label_9: ;
    }
    if (!hasError) {
    
    jj_consume_token(END_T);
    }
    if (!hasError) {
    
    jj_consume_token(CASE_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      identifier();
      }
      
      break;
      }
    default:
      jj_la1[39] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
FlowChart::moveToPrevLevel();
         FlowChart::addFlowChart(FlowChart::END_CASE,"end case",0);
}


void VhdlParser::case_statement_alternative() {QCString s;if (!hasError) {
    
    jj_consume_token(WHEN_T);
    }
    if (!hasError) {
    
    s = choices();
    }
    if (!hasError) {
    
    jj_consume_token(ARROW_T);
    }
    if (!hasError) {
    
QCString t="when ";
    t+=s+"=> ";
    FlowChart::addFlowChart(FlowChart::WHEN_NO,s.data(),t);
    }
    if (!hasError) {
    
    sequence_of_statement();
    }
    
FlowChart::moveToPrevLevel();
}


QCString VhdlParser::character_literal() {Token *tok=0;if (!hasError) {
    
    tok = jj_consume_token(CHARACTER_LITERAL);
    }
    
return tok->image.c_str();
assert(false);
}


QCString VhdlParser::choice() {QCString s;
    if (jj_2_10(2147483647)) {if (!hasError) {
      
      s = discrete_range();
      }
      if (!hasError) {
      
return s;
      }
      
    } else if (jj_2_11(2147483647)) {if (!hasError) {
      
      s = simple_expression();
      }
      if (!hasError) {
      
return s;
      }
      
    } else if (jj_2_12(2147483647)) {if (!hasError) {
      
      s = identifier();
      }
      if (!hasError) {
      
return s;
      }
      
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case OTHER_T:{if (!hasError) {
        
        jj_consume_token(OTHER_T);
        }
        if (!hasError) {
        
return " others ";
        }
        
        break;
        }
      default:
        jj_la1[40] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
assert(false);
return QCString();
}


QCString VhdlParser::choices() {QCString s,s1;if (!hasError) {
    
    s = choice();
    }
    if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case BAR_T:{
        ;
        break;
        }
      default:
        jj_la1[41] = jj_gen;
        goto end_label_10;
      }if (!hasError) {
      
      jj_consume_token(BAR_T);
      }
      if (!hasError) {
      
      s1 = choice();
      }
      if (!hasError) {
      
s+="|";s+=s1;
      }
      
    }
    end_label_10: ;
    }
    
return s;
assert(false);
}


void VhdlParser::component_configuration() {if (!hasError) {
    
    jj_consume_token(FOR_T);
    }
    if (!hasError) {
    
    component_specification();
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case GENERIC_T:
    case PORT_T:
    case USE_T:
    case SEMI_T:{if (!hasError) {
      
      binding_indication();
      }
      if (!hasError) {
      
      jj_consume_token(SEMI_T);
      }
      
      break;
      }
    default:
      jj_la1[42] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case FOR_T:{if (!hasError) {
      
      block_configuration();
      }
      
      break;
      }
    default:
      jj_la1[43] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(END_T);
    }
    if (!hasError) {
    
    jj_consume_token(FOR_T);
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
}


void VhdlParser::component_declaration() {QCString s;if (!hasError) {
    
    jj_consume_token(COMPONENT_T);
    }
    if (!hasError) {
    
    s = identifier();
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case IS_T:{if (!hasError) {
      
      jj_consume_token(IS_T);
      }
      
      break;
      }
    default:
      jj_la1[44] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
currP=VhdlDocGen::COMPONENT;
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case GENERIC_T:{if (!hasError) {
      
      generic_clause();
      }
      
      break;
      }
    default:
      jj_la1[45] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case PORT_T:{if (!hasError) {
      
      port_clause();
      }
      
      break;
      }
    default:
      jj_la1[46] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
addVhdlType(s.data(),getLine(COMPONENT_T),Entry::VARIABLE_SEC,VhdlDocGen::COMPONENT,0,0,Public);
     currP=0;
    }
    if (!hasError) {
    
    jj_consume_token(END_T);
    }
    if (!hasError) {
    
    jj_consume_token(COMPONENT_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      identifier();
      }
      
      break;
      }
    default:
      jj_la1[47] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
}


void VhdlParser::component_instantiation_statement() {QCString s,s1;if (!hasError) {
    
    s = identifier();
    }
    if (!hasError) {
    
    jj_consume_token(COLON_T);
    }
    if (!hasError) {
    
    s1 = instantiation_unit();
    }
    if (!hasError) {
    
addCompInst(s.lower().data(),s1.lower().data(),0,getLine());
    }
    if (!hasError) {
    
    if (jj_2_13(2147483647)) {if (!hasError) {
      
      generic_map_aspect();
      }
      
    } else {
      ;
    }
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case PORT_T:{if (!hasError) {
      
      port_map_aspect();
      }
      
      break;
      }
    default:
      jj_la1[48] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
}


void VhdlParser::component_specification() {if (!hasError) {
    
    instantiation_list();
    }
    if (!hasError) {
    
    jj_consume_token(COLON_T);
    }
    if (!hasError) {
    
    name();
    }
    
}


QCString VhdlParser::composite_type_definition() {QCString s,s1;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case ARRAY_T:{if (!hasError) {
      
      s = array_type_definition();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    case RECORD_T:{if (!hasError) {
      
      record_type_definition();
      }
      if (!hasError) {
      
return s+"#";
      }
      
      break;
      }
    default:
      jj_la1[49] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


void VhdlParser::concurrent_assertion_statement() {if (!hasError) {
    
    if (jj_2_14(2)) {if (!hasError) {
      
      identifier();
      }
      if (!hasError) {
      
      jj_consume_token(COLON_T);
      }
      
    } else {
      ;
    }
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case POSTPONED_T:{if (!hasError) {
      
      jj_consume_token(POSTPONED_T);
      }
      
      break;
      }
    default:
      jj_la1[50] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    assertion();
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
}


void VhdlParser::concurrent_procedure_call_statement() {if (!hasError) {
    
    if (jj_2_15(2)) {if (!hasError) {
      
      identifier();
      }
      if (!hasError) {
      
      jj_consume_token(COLON_T);
      }
      
    } else {
      ;
    }
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case POSTPONED_T:{if (!hasError) {
      
      jj_consume_token(POSTPONED_T);
      }
      
      break;
      }
    default:
      jj_la1[51] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    procedure_call();
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
}


void VhdlParser::concurrent_signal_assignment_statement() {if (!hasError) {
    
    if (jj_2_16(2)) {if (!hasError) {
      
      identifier();
      }
      if (!hasError) {
      
      jj_consume_token(COLON_T);
      }
      
    } else {
      ;
    }
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case POSTPONED_T:{if (!hasError) {
      
      jj_consume_token(POSTPONED_T);
      }
      
      break;
      }
    default:
      jj_la1[52] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    if (jj_2_17(2147483647)) {if (!hasError) {
      
      conditional_signal_assignment();
      }
      
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case WITH_T:{if (!hasError) {
        
        selected_signal_assignment();
        }
        
        break;
        }
      default:
        jj_la1[53] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
    }
    
}


void VhdlParser::concurrent_statement() {
    if (jj_2_18(2147483647)) {if (!hasError) {
      
      block_statement();
      }
      
    } else if (jj_2_19(2147483647)) {if (!hasError) {
      
      process_statement();
      }
      
    } else if (jj_2_20(2147483647)) {if (!hasError) {
      
      generate_statement();
      }
      
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case CASE_T:{if (!hasError) {
        
        case_scheme();
        }
        
        break;
        }
      default:
        jj_la1[54] = jj_gen;
        if (jj_2_21(2147483647)) {if (!hasError) {
          
          concurrent_assertion_statement();
          }
          
        } else if (jj_2_22(2147483647)) {if (!hasError) {
          
          concurrent_signal_assignment_statement();
          }
          
        } else if (jj_2_23(2147483647)) {if (!hasError) {
          
          component_instantiation_statement();
          }
          
        } else if (jj_2_24(2147483647)) {if (!hasError) {
          
          concurrent_procedure_call_statement();
          }
          
        } else {
          switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
          case VHDL2008TOOLDIR:{if (!hasError) {
            
            jj_consume_token(VHDL2008TOOLDIR);
            }
            
            break;
            }
          default:
            jj_la1[55] = jj_gen;
            jj_consume_token(-1);
            errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
          }
        }
      }
    }
}


QCString VhdlParser::condition() {QCString s;if (!hasError) {
    
    s = expression();
    }
    
return s;
assert(false);
}


QCString VhdlParser::condition_clause() {QCString s;if (!hasError) {
    
    jj_consume_token(UNTIL_T);
    }
    if (!hasError) {
    
    s = condition();
    }
    
return " until "+s;
assert(false);
}


void VhdlParser::conditional_signal_assignment() {if (!hasError) {
    
    target();
    }
    if (!hasError) {
    
    jj_consume_token(LESSTHAN_T);
    }
    if (!hasError) {
    
    options();
    }
    if (!hasError) {
    
    conditional_waveforms();
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
}


void VhdlParser::conditional_waveforms() {if (!hasError) {
    
    waveform();
    }
    if (!hasError) {
    
    while (!hasError) {
      if (jj_2_25(2147483647)) {
        ;
      } else {
        goto end_label_11;
      }if (!hasError) {
      
      jj_consume_token(WHEN_T);
      }
      if (!hasError) {
      
      condition();
      }
      if (!hasError) {
      
      jj_consume_token(ELSE_T);
      }
      if (!hasError) {
      
      waveform();
      }
      
    }
    end_label_11: ;
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case WHEN_T:{if (!hasError) {
      
      jj_consume_token(WHEN_T);
      }
      if (!hasError) {
      
      condition();
      }
      
      break;
      }
    default:
      jj_la1[56] = jj_gen;
      ;
    }
    }
    
}


void VhdlParser::configuration_declaration() {QCString s,s1;if (!hasError) {
    
    jj_consume_token(CONFIGURATION_T);
    }
    if (!hasError) {
    
    s = identifier();
    }
    if (!hasError) {
    
    jj_consume_token(OF_T);
    }
    if (!hasError) {
    
    s1 = name();
    }
    if (!hasError) {
    
    jj_consume_token(IS_T);
    }
    if (!hasError) {
    
confName=s+"::"+s1;
  addVhdlType(s.data(),getLine(CONFIGURATION_T),Entry::VARIABLE_SEC,VhdlDocGen::CONFIG,"configuration",s1.data(),Public);
    }
    if (!hasError) {
    
    configuration_declarative_part();
    }
    if (!hasError) {
    
    block_configuration();
    }
    if (!hasError) {
    
    jj_consume_token(END_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case CONFIGURATION_T:{if (!hasError) {
      
      jj_consume_token(CONFIGURATION_T);
      }
      
      break;
      }
    default:
      jj_la1[57] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      name();
      }
      
      break;
      }
    default:
      jj_la1[58] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
genLabels.resize(0); confName="";
}


void VhdlParser::configuration_declarative_item() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case USE_T:{if (!hasError) {
      
      use_clause();
      }
      
      break;
      }
    case ATTRIBUTE_T:{if (!hasError) {
      
      attribute_specification();
      }
      
      break;
      }
    case GROUP_T:{if (!hasError) {
      
      group_declaration();
      }
      
      break;
      }
    default:
      jj_la1[59] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
}


void VhdlParser::configuration_declarative_part() {if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ATTRIBUTE_T:
      case GROUP_T:
      case USE_T:{
        ;
        break;
        }
      default:
        jj_la1[60] = jj_gen;
        goto end_label_12;
      }if (!hasError) {
      
      configuration_declarative_item();
      }
      
    }
    end_label_12: ;
    }
    
}


void VhdlParser::configuration_item() {
    if (jj_2_26(2147483647)) {if (!hasError) {
      
      component_configuration();
      }
      
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case FOR_T:{if (!hasError) {
        
        block_configuration();
        }
        
        break;
        }
      default:
        jj_la1[61] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
}


void VhdlParser::configuration_specification() {if (!hasError) {
    
    jj_consume_token(FOR_T);
    }
    if (!hasError) {
    
    component_specification();
    }
    if (!hasError) {
    
    binding_indication();
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
}


QCString VhdlParser::constant_declaration() {QCString s,s1,s2;Token *t=0;if (!hasError) {
    
    jj_consume_token(CONSTANT_T);
    }
    if (!hasError) {
    
    s = identifier_list();
    }
    if (!hasError) {
    
    jj_consume_token(COLON_T);
    }
    if (!hasError) {
    
    s1 = subtype_indication();
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case VARASSIGN_T:{if (!hasError) {
      
      t = jj_consume_token(VARASSIGN_T);
      }
      if (!hasError) {
      
      s2 = expression();
      }
      
      break;
      }
    default:
      jj_la1[62] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
if(t)
      s2.prepend(":=");
     QCString it=s1+s2;
     addVhdlType(s.data(),getLine(CONSTANT_T),Entry::VARIABLE_SEC,VhdlDocGen::CONSTANT,0,it.data(),Public);
     it.prepend("constant ");
     return it;
assert(false);
}


QCString VhdlParser::constraint_array_definition() {QCString s,s1;if (!hasError) {
    
    jj_consume_token(ARRAY_T);
    }
    if (!hasError) {
    
    s = index_constraint();
    }
    if (!hasError) {
    
    jj_consume_token(OF_T);
    }
    if (!hasError) {
    
    s1 = subtype_indication();
    }
    
return s+" "+s1;
assert(false);
}


void VhdlParser::context_clause() {if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case LIBRARY_T:
      case USE_T:{
        ;
        break;
        }
      default:
        jj_la1[63] = jj_gen;
        goto end_label_13;
      }if (!hasError) {
      
      context_item();
      }
      
    }
    end_label_13: ;
    }
    
}


QCString VhdlParser::constraint() {QCString s;
    if (jj_2_27(2147483647)) {if (!hasError) {
      
      s = range_constraint();
      }
      if (!hasError) {
      
return s;
      }
      
    } else if (jj_2_28(2147483647)) {if (!hasError) {
      
      s = index_constraint();
      }
      if (!hasError) {
      
return s;
      }
      
    } else {
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


void VhdlParser::context_item() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LIBRARY_T:{if (!hasError) {
      
      library_clause();
      }
      
      break;
      }
    case USE_T:{if (!hasError) {
      
      use_clause();
      }
      
      break;
      }
    default:
      jj_la1[64] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
}


QCString VhdlParser::decimal_literal() {Token *tok=0;if (!hasError) {
    
    tok = jj_consume_token(DECIMAL_LITERAL);
    }
    
return tok->image.c_str();
assert(false);
}


QCString VhdlParser::delay_mechanism() {QCString s;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case TRANSPORT_T:{if (!hasError) {
      
      jj_consume_token(TRANSPORT_T);
      }
      if (!hasError) {
      
return " transport ";
      }
      
      break;
      }
    case INERTIAL_T:
    case REJECT_T:{if (!hasError) {
      
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case REJECT_T:{if (!hasError) {
        
        jj_consume_token(REJECT_T);
        }
        if (!hasError) {
        
        s = expression();
        }
        if (!hasError) {
        
s.prepend(" reject ");
        }
        
        break;
        }
      default:
        jj_la1[65] = jj_gen;
        ;
      }
      }
      if (!hasError) {
      
      jj_consume_token(INERTIAL_T);
      }
      if (!hasError) {
      
return s+" inertial ";
      }
      
      break;
      }
    default:
      jj_la1[66] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


void VhdlParser::design_file() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case ARCHITECTURE_T:
    case CONFIGURATION_T:
    case CONTEXT_T:
    case ENTITY_T:
    case LIBRARY_T:
    case PACKAGE_T:
    case USE_T:{if (!hasError) {
      
      while (!hasError) {if (!hasError) {
        
        design_unit();
        }
        
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case ARCHITECTURE_T:
        case CONFIGURATION_T:
        case CONTEXT_T:
        case ENTITY_T:
        case LIBRARY_T:
        case PACKAGE_T:
        case USE_T:{
          ;
          break;
          }
        default:
          jj_la1[67] = jj_gen;
          goto end_label_14;
        }
      }
      end_label_14: ;
      }
      if (!hasError) {
      

      }
      
      break;
      }
    case 0:{if (!hasError) {
      
      jj_consume_token(0);
      }
      
      break;
      }
    default:
      jj_la1[68] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
}


void VhdlParser::design_unit() {if (!hasError) {
    
    context_clause();
    }
    if (!hasError) {
    
    library_unit();
    }
    
}


QCString VhdlParser::designator() {QCString s;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      s = identifier();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    case STRINGLITERAL:{if (!hasError) {
      
      s = operator_symbol();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    default:
      jj_la1[69] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


QCString VhdlParser::direction() {Token *tok=0;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case TO_T:{if (!hasError) {
      
      tok = jj_consume_token(TO_T);
      }
      if (!hasError) {
      
return tok->image.c_str();
      }
      
      break;
      }
    case DOWNTO_T:{if (!hasError) {
      
      tok = jj_consume_token(DOWNTO_T);
      }
      if (!hasError) {
      
return tok->image.c_str();
      }
      
      break;
      }
    default:
      jj_la1[70] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


void VhdlParser::disconnection_specification() {if (!hasError) {
    
    jj_consume_token(DISCONNECT_T);
    }
    if (!hasError) {
    
    guarded_signal_specificatio();
    }
    if (!hasError) {
    
    jj_consume_token(AFTER_T);
    }
    if (!hasError) {
    
    expression();
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
}


void VhdlParser::guarded_signal_specificatio() {if (!hasError) {
    
    signal_list();
    }
    if (!hasError) {
    
    jj_consume_token(COLON_T);
    }
    if (!hasError) {
    
    name();
    }
    
}


QCString VhdlParser::discrete_range() {QCString s;
    if (jj_2_29(2147483647)) {if (!hasError) {
      
      s = range();
      }
      if (!hasError) {
      
return s;
      }
      
    } else if (jj_2_30(2147483647)) {if (!hasError) {
      
      s = subtype_indication();
      }
      if (!hasError) {
      
return s;
      }
      
    } else {
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


QCString VhdlParser::element_association() {QCString s,s1;if (!hasError) {
    
    if (jj_2_31(2147483647)) {if (!hasError) {
      
      s = choices();
      }
      if (!hasError) {
      
      jj_consume_token(ARROW_T);
      }
      
    } else {
      ;
    }
    }
    if (!hasError) {
    
    s1 = expression();
    }
    
if(!s.isEmpty())
 return s+"=>"+s1;
return s1;
assert(false);
}


QCString VhdlParser::element_declaration() {QCString s,s1;if (!hasError) {
    
    s = identifier_list();
    }
    if (!hasError) {
    
    jj_consume_token(COLON_T);
    }
    if (!hasError) {
    
    s1 = subtype_indication();
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
addVhdlType(s.data(),getLine(),Entry::VARIABLE_SEC,VhdlDocGen::RECORD,0,s1.data(),Public);
  //addVhdlType(s.data(),getLine(),Entry::VARIABLE_SEC,VhdlDocGen::VFILE,0,s1.data(),Public);
  return s+":"+s1;
assert(false);
}


QCString VhdlParser::entity_aspect() {Token *tok=0;QCString s,s1;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case ENTITY_T:{if (!hasError) {
      
      tok = jj_consume_token(ENTITY_T);
      }
      if (!hasError) {
      
      s = name();
      }
      if (!hasError) {
      
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case LPAREN_T:{if (!hasError) {
        
        jj_consume_token(LPAREN_T);
        }
        if (!hasError) {
        
        s1 = identifier();
        }
        if (!hasError) {
        
        jj_consume_token(RPAREN_T);
        }
        if (!hasError) {
        
s+="("+s1+")";
        }
        
        break;
        }
      default:
        jj_la1[71] = jj_gen;
        ;
      }
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    case CONFIGURATION_T:{if (!hasError) {
      
      tok = jj_consume_token(CONFIGURATION_T);
      }
      if (!hasError) {
      
      s = name();
      }
      if (!hasError) {
      
return tok->image.c_str()+s;
      }
      
      break;
      }
    case OPEN_T:{if (!hasError) {
      
      tok = jj_consume_token(OPEN_T);
      }
      if (!hasError) {
      
return tok->image.c_str();
      }
      
      break;
      }
    default:
      jj_la1[72] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


QCString VhdlParser::entity_class() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case ENTITY_T:{if (!hasError) {
      
      jj_consume_token(ENTITY_T);
      }
      if (!hasError) {
      
return "entity";
      }
      
      break;
      }
    case ARCHITECTURE_T:{if (!hasError) {
      
      jj_consume_token(ARCHITECTURE_T);
      }
      if (!hasError) {
      
return "architecture";
      }
      
      break;
      }
    case CONFIGURATION_T:{if (!hasError) {
      
      jj_consume_token(CONFIGURATION_T);
      }
      if (!hasError) {
      
return "configuration";
      }
      
      break;
      }
    case PROCEDURE_T:{if (!hasError) {
      
      jj_consume_token(PROCEDURE_T);
      }
      if (!hasError) {
      
return "procedure";
      }
      
      break;
      }
    case FUNCTION_T:{if (!hasError) {
      
      jj_consume_token(FUNCTION_T);
      }
      if (!hasError) {
      
return "function";
      }
      
      break;
      }
    case PACKAGE_T:{if (!hasError) {
      
      jj_consume_token(PACKAGE_T);
      }
      if (!hasError) {
      
return "package";
      }
      
      break;
      }
    case TYPE_T:{if (!hasError) {
      
      jj_consume_token(TYPE_T);
      }
      if (!hasError) {
      
return "type";
      }
      
      break;
      }
    case SUBTYPE_T:{if (!hasError) {
      
      jj_consume_token(SUBTYPE_T);
      }
      if (!hasError) {
      
return "subtype";
      }
      
      break;
      }
    case CONSTANT_T:{if (!hasError) {
      
      jj_consume_token(CONSTANT_T);
      }
      if (!hasError) {
      
return "constant";
      }
      
      break;
      }
    case SIGNAL_T:{if (!hasError) {
      
      jj_consume_token(SIGNAL_T);
      }
      if (!hasError) {
      
return "signal";
      }
      
      break;
      }
    case VARIABLE_T:{if (!hasError) {
      
      jj_consume_token(VARIABLE_T);
      }
      if (!hasError) {
      
return "variable";
      }
      
      break;
      }
    case COMPONENT_T:{if (!hasError) {
      
      jj_consume_token(COMPONENT_T);
      }
      if (!hasError) {
      
return "component";
      }
      
      break;
      }
    case LABEL_T:{if (!hasError) {
      
      jj_consume_token(LABEL_T);
      }
      if (!hasError) {
      
return "label";
      }
      
      break;
      }
    case LITERAL_T:{if (!hasError) {
      
      jj_consume_token(LITERAL_T);
      }
      if (!hasError) {
      
return "literal";
      }
      
      break;
      }
    case UNITS_T:{if (!hasError) {
      
      jj_consume_token(UNITS_T);
      }
      if (!hasError) {
      
return "units";
      }
      
      break;
      }
    case GROUP_T:{if (!hasError) {
      
      jj_consume_token(GROUP_T);
      }
      if (!hasError) {
      
return "group";
      }
      
      break;
      }
    case FILE_T:{if (!hasError) {
      
      jj_consume_token(FILE_T);
      }
      if (!hasError) {
      
return "file";
      }
      
      break;
      }
    default:
      jj_la1[73] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


QCString VhdlParser::entity_class_entry() {QCString s;if (!hasError) {
    
    s = entity_class();
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BOX_T:{if (!hasError) {
      
      jj_consume_token(BOX_T);
      }
      if (!hasError) {
      
s+="<>";
      }
      
      break;
      }
    default:
      jj_la1[74] = jj_gen;
      ;
    }
    }
    
return s;
assert(false);
}


QCString VhdlParser::entity_class_entry_list() {QCString s,s1,s2;if (!hasError) {
    if (!hasError) {
    
    s1 = entity_class_entry();
    }
    if (!hasError) {
    
s+=s1;
    }
    
    }
    if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case COMMA_T:{
        ;
        break;
        }
      default:
        jj_la1[75] = jj_gen;
        goto end_label_15;
      }if (!hasError) {
      
      jj_consume_token(COMMA_T);
      }
      if (!hasError) {
      
      s = entity_class_entry();
      }
      if (!hasError) {
      
s2+=",";s2+=s;
      }
      
    }
    end_label_15: ;
    }
    
return s1+s2;
assert(false);
}


void VhdlParser::entity_declaration() {QCString s;if (!hasError) {
    
    jj_consume_token(ENTITY_T);
    }
    if (!hasError) {
    
    s = identifier();
    }
    if (!hasError) {
    
    jj_consume_token(IS_T);
    }
    if (!hasError) {
    
lastEntity=current;
                lastCompound=0;
                addVhdlType(s.data(),getLine(ENTITY_T),Entry::CLASS_SEC,VhdlDocGen::ENTITY,0,0,Public);
    }
    if (!hasError) {
    
    entity_header();
    }
    if (!hasError) {
    
    entity_declarative_part();
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BEGIN_T:{if (!hasError) {
      
      jj_consume_token(BEGIN_T);
      }
      if (!hasError) {
      
      entity_statement_part();
      }
      
      break;
      }
    default:
      jj_la1[76] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(END_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case ENTITY_T:{if (!hasError) {
      
      jj_consume_token(ENTITY_T);
      }
      
      break;
      }
    default:
      jj_la1[77] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      name();
      }
      
      break;
      }
    default:
      jj_la1[78] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
lastEntity=0;lastCompound=0; genLabels.resize(0);
}


void VhdlParser::entity_declarative_item() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case FUNCTION_T:
    case IMPURE_T:
    case PROCEDURE_T:
    case PURE_T:{if (!hasError) {
      
      subprogram_declaration();
      }
      
      break;
      }
    case TYPE_T:{if (!hasError) {
      
      type_declaration();
      }
      
      break;
      }
    case SUBTYPE_T:{if (!hasError) {
      
      subtype_declaration();
      }
      
      break;
      }
    case CONSTANT_T:{if (!hasError) {
      
      constant_declaration();
      }
      
      break;
      }
    case SIGNAL_T:{if (!hasError) {
      
      signal_declaration();
      }
      
      break;
      }
    case SHARED_T:
    case VARIABLE_T:{if (!hasError) {
      
      variable_declaration();
      }
      
      break;
      }
    case FILE_T:{if (!hasError) {
      
      file_declaration();
      }
      
      break;
      }
    case ALIAS_T:{if (!hasError) {
      
      alias_declaration();
      }
      
      break;
      }
    default:
      jj_la1[79] = jj_gen;
      if (jj_2_32(2147483647)) {if (!hasError) {
        
        attribute_declaration();
        }
        
      } else {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case ATTRIBUTE_T:{if (!hasError) {
          
          attribute_specification();
          }
          
          break;
          }
        case DISCONNECT_T:{if (!hasError) {
          
          disconnection_specification();
          }
          
          break;
          }
        case USE_T:{if (!hasError) {
          
          use_clause();
          }
          
          break;
          }
        default:
          jj_la1[80] = jj_gen;
          if (jj_2_33(3)) {if (!hasError) {
            
            group_template_declaration();
            }
            
          } else {
            switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
            case GROUP_T:{if (!hasError) {
              
              group_declaration();
              }
              
              break;
              }
            default:
              jj_la1[81] = jj_gen;
              if (jj_2_34(5)) {if (!hasError) {
                
                package_instantiation_declaration();
                }
                
              } else {
                switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
                case PACKAGE_T:{if (!hasError) {
                  
                  package_declaration();
                  }
                  
                  break;
                  }
                case VHDL2008TOOLDIR:{if (!hasError) {
                  
                  jj_consume_token(VHDL2008TOOLDIR);
                  }
                  
                  break;
                  }
                default:
                  jj_la1[82] = jj_gen;
                  jj_consume_token(-1);
                  errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
                }
              }
            }
          }
        }
      }
    }
}


void VhdlParser::entity_declarative_part() {if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ALIAS_T:
      case ATTRIBUTE_T:
      case CONSTANT_T:
      case DISCONNECT_T:
      case FILE_T:
      case FUNCTION_T:
      case GROUP_T:
      case IMPURE_T:
      case PACKAGE_T:
      case PROCEDURE_T:
      case PURE_T:
      case SIGNAL_T:
      case SHARED_T:
      case SUBTYPE_T:
      case TYPE_T:
      case USE_T:
      case VARIABLE_T:
      case VHDL2008TOOLDIR:{
        ;
        break;
        }
      default:
        jj_la1[83] = jj_gen;
        goto end_label_16;
      }if (!hasError) {
      
      entity_declarative_item();
      }
      
    }
    end_label_16: ;
    }
    
}


QCString VhdlParser::entity_designator() {QCString s,s1;if (!hasError) {
    
    s = entity_tag();
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LBRACKET_T:{if (!hasError) {
      
      s1 = signature();
      }
      
      break;
      }
    default:
      jj_la1[84] = jj_gen;
      ;
    }
    }
    
return s+s1;
assert(false);
}


void VhdlParser::entity_header() {if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case GENERIC_T:{if (!hasError) {
      
currP=VhdlDocGen::GENERIC;parse_sec=GEN_SEC;
      }
      if (!hasError) {
      
      generic_clause();
      }
      
      break;
      }
    default:
      jj_la1[85] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case PORT_T:{if (!hasError) {
      
currP=VhdlDocGen::PORT;
      }
      if (!hasError) {
      
      port_clause();
      }
      
      break;
      }
    default:
      jj_la1[86] = jj_gen;
      ;
    }
    }
    
}


QCString VhdlParser::entity_name_list() {QCString s,s1;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:
    case CHARACTER_LITERAL:{if (!hasError) {
      
      while (!hasError) {if (!hasError) {
        
        s1 = entity_designator();
        }
        if (!hasError) {
        
s+=s1;
        }
        
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case SLSL_T:
        case STRINGLITERAL:
        case BASIC_IDENTIFIER:
        case EXTENDED_CHARACTER:
        case CHARACTER_LITERAL:{
          ;
          break;
          }
        default:
          jj_la1[87] = jj_gen;
          goto end_label_17;
        }
      }
      end_label_17: ;
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    case OTHER_T:{if (!hasError) {
      
      jj_consume_token(OTHER_T);
      }
      if (!hasError) {
      
return "other";
      }
      
      break;
      }
    case ALL_T:{if (!hasError) {
      
      jj_consume_token(ALL_T);
      }
      if (!hasError) {
      
return "all";
      }
      
      break;
      }
    default:
      jj_la1[88] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


QCString VhdlParser::entity_specification() {QCString s,s1;if (!hasError) {
    
    s = entity_name_list();
    }
    if (!hasError) {
    
    jj_consume_token(COLON_T);
    }
    if (!hasError) {
    
    s1 = entity_class();
    }
    
return s+":"+s1;
assert(false);
}


void VhdlParser::entity_statement() {
    if (jj_2_35(2147483647)) {if (!hasError) {
      
      concurrent_assertion_statement();
      }
      
    } else if (jj_2_36(2147483647)) {if (!hasError) {
      
      process_statement();
      }
      
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case POSTPONED_T:
      case SLSL_T:
      case STRINGLITERAL:
      case BASIC_IDENTIFIER:
      case EXTENDED_CHARACTER:{if (!hasError) {
        
        concurrent_procedure_call_statement();
        }
        
        break;
        }
      default:
        jj_la1[89] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
}


void VhdlParser::entity_statement_part() {if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ASSERT_T:
      case POSTPONED_T:
      case PROCESS_T:
      case SLSL_T:
      case STRINGLITERAL:
      case BASIC_IDENTIFIER:
      case EXTENDED_CHARACTER:{
        ;
        break;
        }
      default:
        jj_la1[90] = jj_gen;
        goto end_label_18;
      }if (!hasError) {
      
      entity_statement();
      }
      
    }
    end_label_18: ;
    }
    
}


QCString VhdlParser::entity_tag() {QCString s;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      s = name();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    case CHARACTER_LITERAL:{if (!hasError) {
      
      s = character_literal();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    default:
      jj_la1[91] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


QCString VhdlParser::enumeration_literal() {QCString s;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      s = identifier();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    case CHARACTER_LITERAL:{if (!hasError) {
      
      s = character_literal();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    default:
      jj_la1[92] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


QCString VhdlParser::enumeration_type_definition() {QCString s,s1;if (!hasError) {
    
    jj_consume_token(LPAREN_T);
    }
    if (!hasError) {
    
    s = enumeration_literal();
    }
    if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case COMMA_T:{
        ;
        break;
        }
      default:
        jj_la1[93] = jj_gen;
        goto end_label_19;
      }if (!hasError) {
      
      jj_consume_token(COMMA_T);
      }
      if (!hasError) {
      
      s1 = enumeration_literal();
      }
      if (!hasError) {
      
s+=",";s+=s1;
      }
      
    }
    end_label_19: ;
    }
    if (!hasError) {
    
    jj_consume_token(RPAREN_T);
    }
    
return "("+s+")";
assert(false);
}


QCString VhdlParser::exit_statement() {QCString s,s1,s2;Token *t=0;Token *t1=0;if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      s = identifier();
      }
      if (!hasError) {
      
      t = jj_consume_token(COLON_T);
      }
      
      break;
      }
    default:
      jj_la1[94] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(EXIT_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      s1 = identifier();
      }
      
      break;
      }
    default:
      jj_la1[95] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case WHEN_T:{if (!hasError) {
      
      t1 = jj_consume_token(WHEN_T);
      }
      if (!hasError) {
      
      s2 = condition();
      }
      
      break;
      }
    default:
      jj_la1[96] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
lab.resize(0);
  if(t) s+=":";
  if(t1) s2.prepend(" when ");
   FlowChart::addFlowChart(FlowChart::EXIT_NO,"exit",s2.data(),s1.data());

  return s+s1+s2+";";
assert(false);
}


QCString VhdlParser::expression() {QCString s,s1,s2;if (!hasError) {
    
    s = relation();
    }
    if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case AND_T:
      case NAND_T:
      case NOR_T:
      case OR_T:
      case XOR_T:
      case XNOR_T:{
        ;
        break;
        }
      default:
        jj_la1[97] = jj_gen;
        goto end_label_20;
      }if (!hasError) {
      
      s1 = logop();
      }
      if (!hasError) {
      
      s2 = relation();
      }
      if (!hasError) {
      
s+=s1;s+=s2;
      }
      
    }
    end_label_20: ;
    }
    
return s;
assert(false);
}


QCString VhdlParser::logop() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case AND_T:{if (!hasError) {
      
      jj_consume_token(AND_T);
      }
      if (!hasError) {
      
return "and" ;
      }
      
      break;
      }
    case NAND_T:{if (!hasError) {
      
      jj_consume_token(NAND_T);
      }
      if (!hasError) {
      
return "nand" ;
      }
      
      break;
      }
    case NOR_T:{if (!hasError) {
      
      jj_consume_token(NOR_T);
      }
      if (!hasError) {
      
return "nor" ;
      }
      
      break;
      }
    case XNOR_T:{if (!hasError) {
      
      jj_consume_token(XNOR_T);
      }
      if (!hasError) {
      
return "xnor" ;
      }
      
      break;
      }
    case XOR_T:{if (!hasError) {
      
      jj_consume_token(XOR_T);
      }
      if (!hasError) {
      
return "xor" ;
      }
      
      break;
      }
    case OR_T:{if (!hasError) {
      
      jj_consume_token(OR_T);
      }
      if (!hasError) {
      
return "or" ;
      }
      
      break;
      }
    default:
      jj_la1[98] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


QCString VhdlParser::extended_identifier() {Token *t;if (!hasError) {
    
    t = jj_consume_token(EXTENDED_CHARACTER);
    }
    
return t->image.c_str();
assert(false);
}


QCString VhdlParser::factor() {QCString s,s1;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case NEW_T:
    case NULL_T:
    case LPAREN_T:
    case SLSL_T:
    case INTEGER:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:
    case CHARACTER_LITERAL:
    case DECIMAL_LITERAL:
    case BASED_LITERAL:
    case BIT_STRING_LITERAL:{if (!hasError) {
      
      s = primary();
      }
      if (!hasError) {
      
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case DOUBLEMULT_T:{if (!hasError) {
        
        jj_consume_token(DOUBLEMULT_T);
        }
        if (!hasError) {
        
        s1 = primary();
        }
        if (!hasError) {
        
s+="**";s+=s1;
        }
        
        break;
        }
      default:
        jj_la1[99] = jj_gen;
        ;
      }
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    case ABS_T:{if (!hasError) {
      
      jj_consume_token(ABS_T);
      }
      if (!hasError) {
      
      s = primary();
      }
      if (!hasError) {
      
s1 = "abs "; return s1+s;
      }
      
      break;
      }
    case NOT_T:{if (!hasError) {
      
      jj_consume_token(NOT_T);
      }
      if (!hasError) {
      
      s = primary();
      }
      if (!hasError) {
      
s1="not ";return s1+s;
      }
      
      break;
      }
    default:
      jj_la1[100] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


QCString VhdlParser::file_declaration() {QCString s,s1,s2,s3;if (!hasError) {
    
    jj_consume_token(FILE_T);
    }
    if (!hasError) {
    
    s = identifier_list();
    }
    if (!hasError) {
    
    jj_consume_token(COLON_T);
    }
    if (!hasError) {
    
    s2 = subtype_indication();
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case IS_T:
    case OPEN_T:{if (!hasError) {
      
      s3 = file_open_information();
      }
      
      break;
      }
    default:
      jj_la1[101] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
QCString t1=s2+" "+s3;
   addVhdlType(s.data(),getLine(),Entry::VARIABLE_SEC,VhdlDocGen::VFILE,0,t1.data(),Public);
   return " file "+s+":"+s2+" "+s3+";";
assert(false);
}


QCString VhdlParser::file_logical_name() {QCString s;if (!hasError) {
    
    s = expression();
    }
    
return s;
assert(false);
}


QCString VhdlParser::file_open_information() {QCString s,s1,s2;if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case OPEN_T:{if (!hasError) {
      
      jj_consume_token(OPEN_T);
      }
      if (!hasError) {
      
      s = expression();
      }
      
      break;
      }
    default:
      jj_la1[102] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(IS_T);
    }
    if (!hasError) {
    
    s1 = file_logical_name();
    }
    
s2="open "+s+" is "+s1;  return s2;
assert(false);
}


QCString VhdlParser::file_type_definition() {QCString s,s1;if (!hasError) {
    
    jj_consume_token(FILE_T);
    }
    if (!hasError) {
    
    jj_consume_token(OF_T);
    }
    if (!hasError) {
    
    s = type_mark();
    }
    
s1=" file of "+s; return s1;
assert(false);
}


QCString VhdlParser::floating_type_definition() {QCString s;if (!hasError) {
    
    s = range_constraint();
    }
    
return s;
assert(false);
}


QCString VhdlParser::formal_designator() {QCString s;Token *tok=0;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      s = name();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    case INTEGER:{if (!hasError) {
      
      tok = jj_consume_token(INTEGER);
      }
      if (!hasError) {
      
return tok->image.c_str();
      }
      
      break;
      }
    default:
      jj_la1[103] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


QCString VhdlParser::formal_parameter_list() {QCString s;if (!hasError) {
    
    s = interface_list();
    }
    
return s;
assert(false);
}


QCString VhdlParser::formal_part() {QCString s,s1;if (!hasError) {
    
    s = name();
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LPAREN_T:{if (!hasError) {
      
      jj_consume_token(LPAREN_T);
      }
      if (!hasError) {
      
      formal_designator();
      }
      if (!hasError) {
      
      jj_consume_token(RPAREN_T);
      }
      if (!hasError) {
      
s+"("+s1+")";
      }
      
      break;
      }
    default:
      jj_la1[104] = jj_gen;
      ;
    }
    }
    
return s;
assert(false);
}


QCString VhdlParser::full_type_declaration() {Entry *tmpEntry;QCString s,s1,s2;if (!hasError) {
    
    jj_consume_token(TYPE_T);
    }
    if (!hasError) {
    
    s = identifier();
    }
    if (!hasError) {
    
    jj_consume_token(IS_T);
    }
    if (!hasError) {
    
tmpEntry=current;
  addVhdlType(s.data(),getLine(),Entry::VARIABLE_SEC,VhdlDocGen::RECORD,0,0,Public);
    }
    if (!hasError) {
    
    try {if (!hasError) {
      
      s2 = type_definition();
      }
      
    } catch ( ...) {
error_skipto(SEMI_T);
    }
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
if (s2.contains("#")) {
        VhdlDocGen::deleteAllChars(s2,'#');
        tmpEntry->spec=VhdlDocGen::RECORD;
        tmpEntry->type=s2.data();
        //addVhdlType(s.data(),getLine(TYPE_T),Entry::VARIABLE_SEC,VhdlDocGen::RECORD,0,s2.data(),Public);
      }
      else if (s2.contains("%")) {
        VhdlDocGen::deleteAllChars(s2,'%');
        tmpEntry->spec=VhdlDocGen::UNITS;
        tmpEntry->type=s2.data();
        //addVhdlType(s.data(),getLine(TYPE_T),Entry::VARIABLE_SEC,VhdlDocGen::UNITS,s2.data(),s2.data(),Public);
      }
      else {
        tmpEntry->spec=VhdlDocGen::TYPE;
        tmpEntry->type=s2.data();
        //addVhdlType(s.data(),getLine(TYPE_T),Entry::VARIABLE_SEC,VhdlDocGen::TYPE,0,s2.data(),Public);
      }
      tmpEntry=0;
      return "type "+s+" is "+s2+";";
assert(false);
}


QCString VhdlParser::function_call() {QCString s,s1;if (!hasError) {
    
    s = name();
    }
    if (!hasError) {
    
    jj_consume_token(LPAREN_T);
    }
    if (!hasError) {
    
    s1 = actual_parameter_part();
    }
    if (!hasError) {
    
    jj_consume_token(RPAREN_T);
    }
    
return s+"("+s1+")";
assert(false);
}


void VhdlParser::generate_statement() {QCString s;if (!hasError) {
    
    s = identifier();
    }
    if (!hasError) {
    
    jj_consume_token(COLON_T);
    }
    if (!hasError) {
    
    try {if (!hasError) {
      
      generate_scheme();
      }
      if (!hasError) {
      
      jj_consume_token(GENERATE_T);
      }
      if (!hasError) {
      
pushLabel(genLabels,s);
      }
      if (!hasError) {
      
      generate_statement_body1();
      }
      if (!hasError) {
      
      jj_consume_token(END_T);
      }
      
    } catch ( ...) {
error_skipto(GENERATE_T);
    }
    }
    if (!hasError) {
    
    jj_consume_token(GENERATE_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      identifier();
      }
      
      break;
      }
    default:
      jj_la1[105] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
genLabels=popLabel(genLabels);
}


void VhdlParser::generate_scheme() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case FOR_T:{if (!hasError) {
      
      jj_consume_token(FOR_T);
      }
      if (!hasError) {
      
      parameter_specification();
      }
      
      break;
      }
    case IF_T:{if (!hasError) {
      
      jj_consume_token(IF_T);
      }
      if (!hasError) {
      
      condition();
      }
      
      break;
      }
    default:
      jj_la1[106] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
}


void VhdlParser::generic_clause() {QCString s;if (!hasError) {
    
    jj_consume_token(GENERIC_T);
    }
    if (!hasError) {
    
    jj_consume_token(LPAREN_T);
    }
    if (!hasError) {
    
parse_sec=GEN_SEC;
    }
    if (!hasError) {
    
    s = generic_list();
    }
    if (!hasError) {
    
    jj_consume_token(RPAREN_T);
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
parse_sec=0;
}


QCString VhdlParser::generic_list() {QCString s;if (!hasError) {
    
    s = interface_list();
    }
    
return s;
assert(false);
}


void VhdlParser::generic_map_aspect() {if (!hasError) {
    
    jj_consume_token(GENERIC_T);
    }
    if (!hasError) {
    
    jj_consume_token(MAP_T);
    }
    if (!hasError) {
    
    jj_consume_token(LPAREN_T);
    }
    if (!hasError) {
    
    association_list();
    }
    if (!hasError) {
    
    jj_consume_token(RPAREN_T);
    }
    
}


QCString VhdlParser::group_constituent() {QCString s;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      s = name();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    case CHARACTER_LITERAL:{if (!hasError) {
      
      s = character_literal();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    default:
      jj_la1[107] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


QCString VhdlParser::group_constituent_list() {QCString s,s1,s2;if (!hasError) {
    if (!hasError) {
    
    s1 = group_constituent();
    }
    
    }
    if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case COMMA_T:{
        ;
        break;
        }
      default:
        jj_la1[108] = jj_gen;
        goto end_label_21;
      }if (!hasError) {
      
      jj_consume_token(COMMA_T);
      }
      if (!hasError) {
      
      s = group_constituent();
      }
      if (!hasError) {
      
s2+=",";s2+=s1;
      }
      
    }
    end_label_21: ;
    }
    
return s+s2;
assert(false);
}


QCString VhdlParser::group_declaration() {QCString s,s1,s2;if (!hasError) {
    
    jj_consume_token(GROUP_T);
    }
    if (!hasError) {
    
    s = identifier();
    }
    if (!hasError) {
    
    jj_consume_token(COLON_T);
    }
    if (!hasError) {
    
    s1 = identifier();
    }
    if (!hasError) {
    
    jj_consume_token(LPAREN_T);
    }
    if (!hasError) {
    
    s2 = group_constituent_list();
    }
    if (!hasError) {
    
    jj_consume_token(RPAREN_T);
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
return "group "+s+":"+s1+"("+s2+");";
assert(false);
}


QCString VhdlParser::group_template_declaration() {QCString s,s1;if (!hasError) {
    
    jj_consume_token(GROUP_T);
    }
    if (!hasError) {
    
    s = identifier();
    }
    if (!hasError) {
    
    jj_consume_token(IS_T);
    }
    if (!hasError) {
    
    jj_consume_token(LPAREN_T);
    }
    if (!hasError) {
    
    s1 = entity_class_entry_list();
    }
    if (!hasError) {
    
    jj_consume_token(RPAREN_T);
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
return "group "+s+ "is ("+s1+");";
assert(false);
}


void VhdlParser::guarded_signal_specification() {if (!hasError) {
    
    signal_list();
    }
    if (!hasError) {
    
    jj_consume_token(COLON_T);
    }
    if (!hasError) {
    
    type_mark();
    }
    
}


QCString VhdlParser::identifier() {Token *tok=0;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      tok = jj_consume_token(EXTENDED_CHARACTER);
      }
      if (!hasError) {
      
return tok->image.c_str();
      }
      
      break;
      }
    case BASIC_IDENTIFIER:{if (!hasError) {
      
      tok = jj_consume_token(BASIC_IDENTIFIER);
      }
      if (!hasError) {
      
return tok->image.c_str();
      }
      
      break;
      }
    default:
      jj_la1[109] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


QCString VhdlParser::identifier_list() {QCString str,str1;if (!hasError) {
    
    str = identifier();
    }
    if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case COMMA_T:{
        ;
        break;
        }
      default:
        jj_la1[110] = jj_gen;
        goto end_label_22;
      }if (!hasError) {
      
      jj_consume_token(COMMA_T);
      }
      if (!hasError) {
      
      str1 = identifier();
      }
      if (!hasError) {
      
str+=",";str+=str1;
      }
      
    }
    end_label_22: ;
    }
    
return str;
assert(false);
}


void VhdlParser::if_statement() {QCString s,s1;if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      identifier();
      }
      if (!hasError) {
      
      jj_consume_token(COLON_T);
      }
      
      break;
      }
    default:
      jj_la1[111] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(IF_T);
    }
    if (!hasError) {
    
    s = condition();
    }
    if (!hasError) {
    
    jj_consume_token(THEN_T);
    }
    if (!hasError) {
    
s.prepend("if ");
    FlowChart::addFlowChart(FlowChart::IF_NO,0,s);
    }
    if (!hasError) {
    
    sequence_of_statement();
    }
    if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ELSIF_T:{
        ;
        break;
        }
      default:
        jj_la1[112] = jj_gen;
        goto end_label_23;
      }if (!hasError) {
      
      jj_consume_token(ELSIF_T);
      }
      if (!hasError) {
      
      s1 = condition();
      }
      if (!hasError) {
      
      jj_consume_token(THEN_T);
      }
      if (!hasError) {
      
s1.prepend("elsif ");
           FlowChart::addFlowChart(FlowChart::ELSIF_NO,0,s1.data());
      }
      if (!hasError) {
      
      sequence_of_statement();
      }
      
    }
    end_label_23: ;
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case ELSE_T:{if (!hasError) {
      
      jj_consume_token(ELSE_T);
      }
      if (!hasError) {
      
FlowChart::addFlowChart(FlowChart::ELSE_NO,0,0);
      }
      if (!hasError) {
      
      sequence_of_statement();
      }
      
      break;
      }
    default:
      jj_la1[113] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(END_T);
    }
    if (!hasError) {
    
    jj_consume_token(IF_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      identifier();
      }
      
      break;
      }
    default:
      jj_la1[114] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
FlowChart::moveToPrevLevel();
          FlowChart::addFlowChart(FlowChart::ENDIF_NO,0,0);
}


QCString VhdlParser::incomplete_type_declaration() {QCString s;if (!hasError) {
    
    jj_consume_token(TYPE_T);
    }
    if (!hasError) {
    
    s = identifier();
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
return "type "+s+";";
assert(false);
}


QCString VhdlParser::index_constraint() {QCString s="("; QCString s1,s2;if (!hasError) {
    
    jj_consume_token(LPAREN_T);
    }
    if (!hasError) {
    
    s2 = discrete_range();
    }
    if (!hasError) {
    
s+=s2;
    }
    if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case COMMA_T:{
        ;
        break;
        }
      default:
        jj_la1[115] = jj_gen;
        goto end_label_24;
      }if (!hasError) {
      
      jj_consume_token(COMMA_T);
      }
      if (!hasError) {
      
      s1 = discrete_range();
      }
      if (!hasError) {
      
s+=",";s+=s1;
      }
      
    }
    end_label_24: ;
    }
    if (!hasError) {
    
    jj_consume_token(RPAREN_T);
    }
    
return s+")";
assert(false);
}


QCString VhdlParser::index_specification() {QCString s;
    if (jj_2_37(2147483647)) {if (!hasError) {
      
      s = discrete_range();
      }
      if (!hasError) {
      
return s;
      }
      
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ABS_T:
      case NEW_T:
      case NOT_T:
      case NULL_T:
      case LPAREN_T:
      case PLUS_T:
      case MINUS_T:
      case SLSL_T:
      case INTEGER:
      case STRINGLITERAL:
      case BASIC_IDENTIFIER:
      case EXTENDED_CHARACTER:
      case CHARACTER_LITERAL:
      case DECIMAL_LITERAL:
      case BASED_LITERAL:
      case BIT_STRING_LITERAL:{if (!hasError) {
        
        s = expression();
        }
        if (!hasError) {
        
return s;
        }
        
        break;
        }
      default:
        jj_la1[116] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
assert(false);
return QCString();
}


QCString VhdlParser::index_subtype_definition() {QCString s;if (!hasError) {
    
    s = type_mark();
    }
    if (!hasError) {
    
    jj_consume_token(RANGE_T);
    }
    if (!hasError) {
    
    jj_consume_token(BOX_T);
    }
    
return s+" range <> ";
assert(false);
}


QCString VhdlParser::instantiation_unit() {QCString s,s1,s2;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case COMPONENT_T:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case COMPONENT_T:{if (!hasError) {
        
        jj_consume_token(COMPONENT_T);
        }
        
        break;
        }
      default:
        jj_la1[117] = jj_gen;
        ;
      }
      }
      if (!hasError) {
      
      s = identifier();
      }
      if (!hasError) {
      
s1="component "; return s;
      }
      
      break;
      }
    case ENTITY_T:{if (!hasError) {
      
      jj_consume_token(ENTITY_T);
      }
      if (!hasError) {
      
      if (jj_2_38(2)) {if (!hasError) {
        
        jj_consume_token(BASIC_IDENTIFIER);
        }
        if (!hasError) {
        
        jj_consume_token(DOT_T);
        }
        
      } else {
        ;
      }
      }
      if (!hasError) {
      
      s2 = name();
      }
      if (!hasError) {
      
s="entity "+s2;
      }
      if (!hasError) {
      
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case LPAREN_T:{if (!hasError) {
        
        jj_consume_token(LPAREN_T);
        }
        if (!hasError) {
        
        s1 = identifier();
        }
        if (!hasError) {
        
        jj_consume_token(RPAREN_T);
        }
        if (!hasError) {
        
s+="(";s+=s1;s+=")" ;
        }
        
        break;
        }
      default:
        jj_la1[118] = jj_gen;
        ;
      }
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    case CONFIGURATION_T:{if (!hasError) {
      
      jj_consume_token(CONFIGURATION_T);
      }
      if (!hasError) {
      
      s = name();
      }
      if (!hasError) {
      
s1="configuration ";return s;
      }
      
      break;
      }
    default:
      jj_la1[119] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


QCString VhdlParser::instantiation_list() {QCString s;Token *tok=0;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      s = identifier_list();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    case OTHER_T:{if (!hasError) {
      
      tok = jj_consume_token(OTHER_T);
      }
      if (!hasError) {
      
return tok->image.c_str();
      }
      
      break;
      }
    case ALL_T:{if (!hasError) {
      
      tok = jj_consume_token(ALL_T);
      }
      if (!hasError) {
      
return tok->image.c_str();
      }
      
      break;
      }
    default:
      jj_la1[120] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


QCString VhdlParser::integer() {Token *t;if (!hasError) {
    
    t = jj_consume_token(INTEGER);
    }
    
return t->image.c_str();
assert(false);
}


QCString VhdlParser::integer_type_definition() {QCString s;if (!hasError) {
    
    s = range_constraint();
    }
    
return s;
assert(false);
}


QCString VhdlParser::interface_declaration() {QCString s,s1;
    if (jj_2_39(5)) {if (!hasError) {
      
      s = interface_subprogram_declaration();
      }
      if (!hasError) {
      
return s;
      }
      
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case PACKAGE_T:{if (!hasError) {
        
        interface_package_declaration();
        }
        if (!hasError) {
        
return s;
        }
        
        break;
        }
      default:
        jj_la1[121] = jj_gen;
        if (jj_2_40(5)) {if (!hasError) {
          
          s = interface_variable_declaration();
          }
          if (!hasError) {
          
return s;
          }
          
        } else if (jj_2_41(5)) {if (!hasError) {
          
          interface_file_declaration();
          }
          if (!hasError) {
          
return s;
          }
          
        } else if (jj_2_42(2147483647)) {if (!hasError) {
          
          subprogram_declaration();
          }
          if (!hasError) {
          
return s;
          }
          
        } else {
          switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
          case CONSTANT_T:
          case FILE_T:
          case SIGNAL_T:
          case SHARED_T:
          case TYPE_T:
          case VARIABLE_T:{if (!hasError) {
            
            s = object_class();
            }
            if (!hasError) {
            
            s1 = identifier();
            }
            if (!hasError) {
            
if (parse_sec==GEN_SEC)
    addVhdlType(s.data(),getLine(),Entry::VARIABLE_SEC,currP,s1.data(),0,Public);
    return s;
            }
            
            break;
            }
          default:
            jj_la1[122] = jj_gen;
            jj_consume_token(-1);
            errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
          }
        }
      }
    }
assert(false);
return QCString();
}


QCString VhdlParser::interface_element() {QCString s;if (!hasError) {
    
    s = interface_declaration();
    }
    
return s;
assert(false);
}


QCString VhdlParser::interface_file_declaration() {QCString s,s1;if (!hasError) {
    
    jj_consume_token(FILE_T);
    }
    if (!hasError) {
    
    s = identifier_list();
    }
    if (!hasError) {
    
    jj_consume_token(COLON_T);
    }
    if (!hasError) {
    
    s1 = subtype_indication();
    }
    
addVhdlType(s.data(),getLine(),Entry::VARIABLE_SEC,VhdlDocGen::VFILE,0,s1.data(),Public);
   return " file "+s+":"+s1;
assert(false);
}


QCString VhdlParser::interface_list() {QCString s,s1,s2;if (!hasError) {
    
    s = interface_element();
    }
    if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case SEMI_T:{
        ;
        break;
        }
      default:
        jj_la1[123] = jj_gen;
        goto end_label_25;
      }if (!hasError) {
      
      jj_consume_token(SEMI_T);
      }
      if (!hasError) {
      
      s1 = interface_element();
      }
      if (!hasError) {
      
s2+=";";s2+=s1;
      }
      
    }
    end_label_25: ;
    }
    
return s+s2;
assert(false);
}


QCString VhdlParser::interface_variable_declaration() {Token *tok=0;Token *tok1=0;Token *tok2=0;QCString s,s1,s2,s3,s4,s5;if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case CONSTANT_T:
    case SIGNAL_T:
    case SHARED_T:
    case VARIABLE_T:{if (!hasError) {
      
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case VARIABLE_T:{if (!hasError) {
        
        tok = jj_consume_token(VARIABLE_T);
        }
        
        break;
        }
      case SIGNAL_T:{if (!hasError) {
        
        tok = jj_consume_token(SIGNAL_T);
        }
        
        break;
        }
      case CONSTANT_T:{if (!hasError) {
        
        tok = jj_consume_token(CONSTANT_T);
        }
        
        break;
        }
      case SHARED_T:{if (!hasError) {
        
        tok = jj_consume_token(SHARED_T);
        }
        
        break;
        }
      default:
        jj_la1[124] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
      }
      
      break;
      }
    default:
      jj_la1[125] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    s = identifier_list();
    }
    if (!hasError) {
    
    jj_consume_token(COLON_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BUFFER_T:
    case IN_T:
    case INOUT_T:
    case LINKAGE_T:
    case OUT_T:{if (!hasError) {
      
      s1 = mode();
      }
      
      break;
      }
    default:
      jj_la1[126] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    s2 = subtype_indication();
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BUS_T:{if (!hasError) {
      
      tok1 = jj_consume_token(BUS_T);
      }
      
      break;
      }
    default:
      jj_la1[127] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case VARASSIGN_T:{if (!hasError) {
      
      tok2 = jj_consume_token(VARASSIGN_T);
      }
      if (!hasError) {
      
      s4 = expression();
      }
      
      break;
      }
    default:
      jj_la1[128] = jj_gen;
      ;
    }
    }
    
if(tok)
                    s5=tok->image.c_str();

                    if(tok1)
                     s3=tok1->image.data();

                    if(tok2)
                     s3+=":=";

                    QCString it=s+":"+s1+" "+s2+" "+s3+" "+s4;
                    if (currP!=VhdlDocGen::COMPONENT)
                    {
                      if (currP==VhdlDocGen::FUNCTION || currP==VhdlDocGen::PROCEDURE)
                      {
                        addProto(s5.data(),s.data(),s1.data(),s2.data(),s3.data(),s4.data());
                      }
                      else
                      {
                        QCString i=s2+s3+s4;
                        if (currP==VhdlDocGen::GENERIC && param_sec==0)
                          addVhdlType(s.data(),getLine(),Entry::VARIABLE_SEC,currP,i.data(),s1.data(),Public);
                        else if(parse_sec != GEN_SEC)
                          addVhdlType(s.data(),getLine(),Entry::VARIABLE_SEC,currP,i.data(),s1.data(),Public);
                      }
                      //   fprintf(stderr,"\n\n <<port  %s  >>\n",$$.data());
                    } // if component
              return it;
assert(false);
}


QCString VhdlParser::iteration_scheme() {QCString s;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case WHILE_T:{if (!hasError) {
      
      jj_consume_token(WHILE_T);
      }
      if (!hasError) {
      
      s = condition();
      }
      if (!hasError) {
      
s.prepend("while ");
   FlowChart::addFlowChart(FlowChart::WHILE_NO,0,s.data(),lab.data());
   lab="";
  return s;
      }
      
      break;
      }
    case FOR_T:{if (!hasError) {
      
      jj_consume_token(FOR_T);
      }
      if (!hasError) {
      
      s = parameter_specification();
      }
      if (!hasError) {
      
QCString q=lab+" for "+s;
    FlowChart::addFlowChart(FlowChart::FOR_NO,0,q.data(),lab.data());
    lab="";
    return q;
      }
      
      break;
      }
    default:
      jj_la1[129] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


QCString VhdlParser::label() {QCString s;if (!hasError) {
    
    s = identifier();
    }
    
return s;
assert(false);
}


QCString VhdlParser::library_clause() {QCString s;if (!hasError) {
    if (!hasError) {
    
    jj_consume_token(LIBRARY_T);
    }
    if (!hasError) {
    
    s = identifier_list();
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
    }
    
if ( parse_sec==0 && Config_getBool(SHOW_INCLUDE_FILES) )
                   {
                           addVhdlType(s.data(),getLine(),Entry::VARIABLE_SEC,VhdlDocGen::LIBRARY,s.data(),"_library_",Public);
                   }
                   QCString s1="library "+s;
                   return s1;
assert(false);
}


QCString VhdlParser::library_unit() {QCString s;
    if (jj_2_43(2)) {if (!hasError) {
      
      primary_unit();
      }
      if (!hasError) {
      
return s;
      }
      
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ARCHITECTURE_T:
      case PACKAGE_T:{if (!hasError) {
        
        secondary_unit();
        }
        if (!hasError) {
        
return s;
        }
        
        break;
        }
      case CONTEXT_T:{if (!hasError) {
        
        context_declaration();
        }
        
        break;
        }
      default:
        jj_la1[130] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
assert(false);
return QCString();
}


QCString VhdlParser::literal() {QCString s;
    if (jj_2_44(2147483647)) {if (!hasError) {
      
      s = bit_string_literal();
      }
      if (!hasError) {
      
return s;
      }
      
    } else if (jj_2_45(2147483647)) {if (!hasError) {
      
      s = numeric_literal();
      }
      if (!hasError) {
      
return s;
      }
      
    } else if (jj_2_46(2147483647)) {if (!hasError) {
      
      s = enumeration_literal();
      }
      if (!hasError) {
      
return s;
      }
      
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case STRINGLITERAL:{if (!hasError) {
        
        s = string_literal();
        }
        if (!hasError) {
        
return s;
        }
        
        break;
        }
      case NULL_T:{if (!hasError) {
        
        jj_consume_token(NULL_T);
        }
        if (!hasError) {
        
return "null";
        }
        
        break;
        }
      default:
        jj_la1[131] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
assert(false);
return QCString();
}


QCString VhdlParser::logical_operator() {QCString s;if (!hasError) {
    
    s = logop();
    }
    
return s;
assert(false);
}


QCString VhdlParser::loop_statement() {QCString s,s1,s2,s3;if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      s = identifier();
      }
      if (!hasError) {
      
      jj_consume_token(COLON_T);
      }
      if (!hasError) {
      
s+=":";
      }
      
      break;
      }
    default:
      jj_la1[132] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case FOR_T:
    case WHILE_T:{if (!hasError) {
      
      s1 = iteration_scheme();
      }
      
      break;
      }
    default:
      jj_la1[133] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
if(s1.isEmpty())
        FlowChart::addFlowChart(FlowChart::LOOP_NO,0,"infinite");
    }
    if (!hasError) {
    
    jj_consume_token(LOOP_T);
    }
    if (!hasError) {
    
    s2 = sequence_of_statement();
    }
    if (!hasError) {
    
    jj_consume_token(END_T);
    }
    if (!hasError) {
    
    jj_consume_token(LOOP_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      s3 = identifier();
      }
      
      break;
      }
    default:
      jj_la1[134] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
QCString q = s+" loop "+s2+" end loop" +s3;
         QCString endLoop="end loop" + s3;
         FlowChart::moveToPrevLevel();
         FlowChart::addFlowChart(FlowChart::END_LOOP,endLoop.data(),0);
        return q;
assert(false);
}


QCString VhdlParser::miscellaneous_operator() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case DOUBLEMULT_T:{if (!hasError) {
      
      jj_consume_token(DOUBLEMULT_T);
      }
      if (!hasError) {
      
return "**";
      }
      
      break;
      }
    case ABS_T:{if (!hasError) {
      
      jj_consume_token(ABS_T);
      }
      if (!hasError) {
      
return "abs";
      }
      
      break;
      }
    case NOT_T:{if (!hasError) {
      
      jj_consume_token(NOT_T);
      }
      if (!hasError) {
      
return "not";
      }
      
      break;
      }
    default:
      jj_la1[135] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


QCString VhdlParser::mode() {Token *tok=0;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case IN_T:{if (!hasError) {
      
      tok = jj_consume_token(IN_T);
      }
      if (!hasError) {
      
return "in";
      }
      
      break;
      }
    case OUT_T:{if (!hasError) {
      
      tok = jj_consume_token(OUT_T);
      }
      if (!hasError) {
      
return "out";
      }
      
      break;
      }
    case INOUT_T:{if (!hasError) {
      
      tok = jj_consume_token(INOUT_T);
      }
      if (!hasError) {
      
return "inout";
      }
      
      break;
      }
    case BUFFER_T:{if (!hasError) {
      
      tok = jj_consume_token(BUFFER_T);
      }
      if (!hasError) {
      
return "buffer";
      }
      
      break;
      }
    case LINKAGE_T:{if (!hasError) {
      
      tok = jj_consume_token(LINKAGE_T);
      }
      if (!hasError) {
      
return "linkage";
      }
      
      break;
      }
    default:
      jj_la1[136] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


QCString VhdlParser::multiplying_operation() {Token *tok=0;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case MULT_T:{if (!hasError) {
      
      tok = jj_consume_token(MULT_T);
      }
      if (!hasError) {
      
return tok->image.c_str();
      }
      
      break;
      }
    case SLASH_T:{if (!hasError) {
      
      tok = jj_consume_token(SLASH_T);
      }
      if (!hasError) {
      
return tok->image.c_str();
      }
      
      break;
      }
    case MOD_T:{if (!hasError) {
      
      tok = jj_consume_token(MOD_T);
      }
      if (!hasError) {
      
return tok->image.c_str();
      }
      
      break;
      }
    case REM_T:{if (!hasError) {
      
      tok = jj_consume_token(REM_T);
      }
      if (!hasError) {
      
return tok->image.c_str();
      }
      
      break;
      }
    default:
      jj_la1[137] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


QCString VhdlParser::name() {QCString s,s1;if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case STRINGLITERAL:{if (!hasError) {
      
      s = operator_symbol();
      }
      
      break;
      }
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      s = identifier();
      }
      
      break;
      }
    case SLSL_T:{if (!hasError) {
      
      s = external_name();
      }
      
      break;
      }
    default:
      jj_la1[138] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
    }
    if (!hasError) {
    
    if (jj_2_47(2147483647)) {if (!hasError) {
      
      s1 = name_ext1();
      }
      if (!hasError) {
      
s+=s1;
      }
      
    } else {
      ;
    }
    }
    
return s;
assert(false);
}


QCString VhdlParser::name_ext1() {QCString s,s1,s2;if (!hasError) {
    
    s = name_ext();
    }
    if (!hasError) {
    
    while (!hasError) {
      if (jj_2_48(2147483647)) {
        ;
      } else {
        goto end_label_26;
      }if (!hasError) {
      
      s1 = name_ext();
      }
      if (!hasError) {
      
s+=s1;
      }
      
    }
    end_label_26: ;
    }
    
return s;
assert(false);
}


QCString VhdlParser::name_ext() {QCString s,s1,s2;if (!hasError) {
    
    if (jj_2_49(2147483647)) {if (!hasError) {
      
      jj_consume_token(DOT_T);
      }
      if (!hasError) {
      
      s1 = suffix();
      }
      if (!hasError) {
      
s+=".";s+=s1;
      }
      
    } else if (jj_2_50(2147483647)) {if (!hasError) {
      
      s1 = test_att_name();
      }
      if (!hasError) {
      
s+=s1;
      }
      
    } else if (jj_2_51(2147483647)) {if (!hasError) {
      
      jj_consume_token(LPAREN_T);
      }
      if (!hasError) {
      
      s1 = discrete_range();
      }
      if (!hasError) {
      
      jj_consume_token(RPAREN_T);
      }
      if (!hasError) {
      
s+="(";s+=s1;s+=")";
      }
      
    } else if (jj_2_52(2147483647)) {if (!hasError) {
      
      jj_consume_token(LPAREN_T);
      }
      if (!hasError) {
      
      s1 = expression();
      }
      if (!hasError) {
      
s+="(";s+=s1;
      }
      if (!hasError) {
      
      while (!hasError) {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case COMMA_T:{
          ;
          break;
          }
        default:
          jj_la1[139] = jj_gen;
          goto end_label_27;
        }if (!hasError) {
        
        jj_consume_token(COMMA_T);
        }
        if (!hasError) {
        
        s1 = expression();
        }
        if (!hasError) {
        
s+=",";s+=s1;
        }
        
      }
      end_label_27: ;
      }
      if (!hasError) {
      
      jj_consume_token(RPAREN_T);
      }
      if (!hasError) {
      
s+=")";
      }
      
    } else {
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
    }
    
return s;
assert(false);
}


QCString VhdlParser::test_att_name() {QCString s,s1;if (!hasError) {
    
    if (jj_2_53(2147483647)) {if (!hasError) {
      
      s1 = signature();
      }
      if (!hasError) {
      
s=s1;
      }
      
    } else {
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(APOSTROPHE_T);
    }
    if (!hasError) {
    
    s1 = attribute_designator();
    }
    if (!hasError) {
    
s+="'";s+=s1;
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LPAREN_T:{if (!hasError) {
      
      jj_consume_token(LPAREN_T);
      }
      if (!hasError) {
      
      s1 = expression();
      }
      if (!hasError) {
      
      jj_consume_token(RPAREN_T);
      }
      if (!hasError) {
      
s+="(";s+=s1;s+=")";
      }
      
      break;
      }
    default:
      jj_la1[140] = jj_gen;
      ;
    }
    }
    
return s;
assert(false);
}


QCString VhdlParser::indexed_name() {QCString s,s1,s2;if (!hasError) {
    
    s2 = identifier();
    }
    if (!hasError) {
    
    jj_consume_token(LPAREN_T);
    }
    if (!hasError) {
    
    s1 = expression();
    }
    if (!hasError) {
    
s=s2+"("+s1;
    }
    if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case COMMA_T:{
        ;
        break;
        }
      default:
        jj_la1[141] = jj_gen;
        goto end_label_28;
      }if (!hasError) {
      
      jj_consume_token(COMMA_T);
      }
      if (!hasError) {
      
      s1 = expression();
      }
      if (!hasError) {
      
s+=",";s+=s1;
      }
      
    }
    end_label_28: ;
    }
    if (!hasError) {
    
    jj_consume_token(RPAREN_T);
    }
    
return s+")";
assert(false);
}


QCString VhdlParser::next_statement() {QCString s,s1,s2;Token *t=0;Token *t1=0;if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      s = identifier();
      }
      if (!hasError) {
      
      t = jj_consume_token(COLON_T);
      }
      
      break;
      }
    default:
      jj_la1[142] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(NEXT_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      s1 = identifier();
      }
      
      break;
      }
    default:
      jj_la1[143] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case WHEN_T:{if (!hasError) {
      
      t1 = jj_consume_token(WHEN_T);
      }
      if (!hasError) {
      
      s2 = condition();
      }
      
      break;
      }
    default:
      jj_la1[144] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
if(t) s+=":";
   FlowChart::addFlowChart(FlowChart::NEXT_NO,"next ",s2.data(),s1.data());
    lab.resize(0);
  if(t1) s2.prepend("when ");
  return s+s1+s2+";";
assert(false);
}


QCString VhdlParser::null_statement() {QCString s;if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      s = identifier();
      }
      if (!hasError) {
      
      jj_consume_token(COLON_T);
      }
      if (!hasError) {
      
s+=":";
      }
      
      break;
      }
    default:
      jj_la1[145] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(NULL_T);
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
return s+="null";
assert(false);
}


QCString VhdlParser::numeric_literal() {QCString s;
    if (jj_2_54(2147483647)) {if (!hasError) {
      
      s = physical_literal();
      }
      if (!hasError) {
      
return s;
      }
      
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case INTEGER:
      case DECIMAL_LITERAL:
      case BASED_LITERAL:{if (!hasError) {
        
        s = abstract_literal();
        }
        if (!hasError) {
        
return s;
        }
        
        break;
        }
      default:
        jj_la1[146] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
assert(false);
return QCString();
}


QCString VhdlParser::object_class() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case CONSTANT_T:{if (!hasError) {
      
      jj_consume_token(CONSTANT_T);
      }
      if (!hasError) {
      
return "constant";
      }
      
      break;
      }
    case SIGNAL_T:{if (!hasError) {
      
      jj_consume_token(SIGNAL_T);
      }
      if (!hasError) {
      
return "signal";
      }
      
      break;
      }
    case VARIABLE_T:{if (!hasError) {
      
      jj_consume_token(VARIABLE_T);
      }
      if (!hasError) {
      
return "variable";
      }
      
      break;
      }
    case SHARED_T:{if (!hasError) {
      
      jj_consume_token(SHARED_T);
      }
      if (!hasError) {
      
      jj_consume_token(VARIABLE_T);
      }
      if (!hasError) {
      
return "shared variable";
      }
      
      break;
      }
    case FILE_T:{if (!hasError) {
      
      jj_consume_token(FILE_T);
      }
      if (!hasError) {
      
return "file";
      }
      
      break;
      }
    case TYPE_T:{if (!hasError) {
      
      jj_consume_token(TYPE_T);
      }
      if (!hasError) {
      
return "type";
      }
      
      break;
      }
    default:
      jj_la1[147] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


QCString VhdlParser::operator_symbol() {Token *tok=0;if (!hasError) {
    
    tok = jj_consume_token(STRINGLITERAL);
    }
    
return tok->image.c_str();
assert(false);
}


void VhdlParser::options() {if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case GUARDED_T:{if (!hasError) {
      
      jj_consume_token(GUARDED_T);
      }
      
      break;
      }
    default:
      jj_la1[148] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case INERTIAL_T:
    case REJECT_T:
    case TRANSPORT_T:{if (!hasError) {
      
      delay_mechanism();
      }
      
      break;
      }
    default:
      jj_la1[149] = jj_gen;
      ;
    }
    }
    
}


void VhdlParser::package_body() {QCString s;if (!hasError) {
    
    jj_consume_token(PACKAGE_T);
    }
    if (!hasError) {
    
    jj_consume_token(BODY_T);
    }
    if (!hasError) {
    
    s = name();
    }
    if (!hasError) {
    
    jj_consume_token(IS_T);
    }
    if (!hasError) {
    
lastCompound=current;
                        s.prepend("_");
                        addVhdlType(s,getLine(),Entry::CLASS_SEC,VhdlDocGen::PACKAGE_BODY,0,0,Protected);
    }
    if (!hasError) {
    
    package_body_declarative_part();
    }
    if (!hasError) {
    
    jj_consume_token(END_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case PACKAGE_T:{if (!hasError) {
      
      jj_consume_token(PACKAGE_T);
      }
      if (!hasError) {
      
      jj_consume_token(BODY_T);
      }
      
      break;
      }
    default:
      jj_la1[150] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      name();
      }
      
      break;
      }
    default:
      jj_la1[151] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
lastCompound=0; genLabels.resize(0);
}


void VhdlParser::package_body_declarative_item() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case FUNCTION_T:
    case IMPURE_T:
    case PROCEDURE_T:
    case PURE_T:{if (!hasError) {
      
      subprogram_declaration();
      }
      
      break;
      }
    case TYPE_T:{if (!hasError) {
      
      type_declaration();
      }
      
      break;
      }
    case SUBTYPE_T:{if (!hasError) {
      
      subtype_declaration();
      }
      
      break;
      }
    case CONSTANT_T:{if (!hasError) {
      
      constant_declaration();
      }
      
      break;
      }
    case SHARED_T:
    case VARIABLE_T:{if (!hasError) {
      
      variable_declaration();
      }
      
      break;
      }
    case FILE_T:{if (!hasError) {
      
      file_declaration();
      }
      
      break;
      }
    case ALIAS_T:{if (!hasError) {
      
      alias_declaration();
      }
      
      break;
      }
    case USE_T:{if (!hasError) {
      
      use_clause();
      }
      
      break;
      }
    default:
      jj_la1[152] = jj_gen;
      if (jj_2_55(3)) {if (!hasError) {
        
        group_template_declaration();
        }
        
      } else {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case GROUP_T:{if (!hasError) {
          
          group_declaration();
          }
          
          break;
          }
        default:
          jj_la1[153] = jj_gen;
          jj_consume_token(-1);
          errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
        }
      }
    }
}


void VhdlParser::package_body_declarative_part() {if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ALIAS_T:
      case CONSTANT_T:
      case FILE_T:
      case FUNCTION_T:
      case GROUP_T:
      case IMPURE_T:
      case PROCEDURE_T:
      case PURE_T:
      case SHARED_T:
      case SUBTYPE_T:
      case TYPE_T:
      case USE_T:
      case VARIABLE_T:{
        ;
        break;
        }
      default:
        jj_la1[154] = jj_gen;
        goto end_label_29;
      }if (!hasError) {
      
      package_body_declarative_item();
      }
      
    }
    end_label_29: ;
    }
    
}


void VhdlParser::package_declaration() {QCString s;if (!hasError) {
    
    jj_consume_token(PACKAGE_T);
    }
    if (!hasError) {
    
    s = identifier();
    }
    if (!hasError) {
    
    jj_consume_token(IS_T);
    }
    if (!hasError) {
    
lastCompound=current;
                          Entry *clone=new Entry(*current);
                          clone->section=Entry::NAMESPACE_SEC;
                          clone->spec=VhdlDocGen::PACKAGE;
                          clone->name=s;
                          clone->startLine=getLine(PACKAGE_T);
                          clone->bodyLine=getLine(PACKAGE_T);
                          clone->protection=Package;
                          current_root->addSubEntry(clone);
                          addVhdlType(s,getLine(PACKAGE_T),Entry::CLASS_SEC,VhdlDocGen::PACKAGE,0,0,Package);
    }
    if (!hasError) {
    
    package_declarative_part();
    }
    if (!hasError) {
    
    jj_consume_token(END_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case PACKAGE_T:{if (!hasError) {
      
      jj_consume_token(PACKAGE_T);
      }
      
      break;
      }
    default:
      jj_la1[155] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      name();
      }
      
      break;
      }
    default:
      jj_la1[156] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
lastEntity=0;lastCompound=0; genLabels.resize(0);
}


void VhdlParser::geninter() {if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case GENERIC_T:{if (!hasError) {
      
      gen_interface_list();
      }
      if (!hasError) {
      
      jj_consume_token(SEMI_T);
      }
      if (!hasError) {
      
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case GENERIC_T:{if (!hasError) {
        
        gen_assoc_list();
        }
        if (!hasError) {
        
        jj_consume_token(SEMI_T);
        }
        
        break;
        }
      default:
        jj_la1[157] = jj_gen;
        ;
      }
      }
      
      break;
      }
    default:
      jj_la1[158] = jj_gen;
      ;
    }
    }
    
}


void VhdlParser::package_declarative_item() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case FUNCTION_T:
    case IMPURE_T:
    case PROCEDURE_T:
    case PURE_T:{if (!hasError) {
      
      subprogram_declaration();
      }
      
      break;
      }
    case TYPE_T:{if (!hasError) {
      
      type_declaration();
      }
      
      break;
      }
    case SUBTYPE_T:{if (!hasError) {
      
      subtype_declaration();
      }
      
      break;
      }
    case CONSTANT_T:{if (!hasError) {
      
      constant_declaration();
      }
      
      break;
      }
    case SIGNAL_T:{if (!hasError) {
      
      signal_declaration();
      }
      
      break;
      }
    case SHARED_T:
    case VARIABLE_T:{if (!hasError) {
      
      variable_declaration();
      }
      
      break;
      }
    case FILE_T:{if (!hasError) {
      
      file_declaration();
      }
      
      break;
      }
    case ALIAS_T:{if (!hasError) {
      
      alias_declaration();
      }
      
      break;
      }
    case COMPONENT_T:{if (!hasError) {
      
      component_declaration();
      }
      
      break;
      }
    default:
      jj_la1[159] = jj_gen;
      if (jj_2_56(2147483647)) {if (!hasError) {
        
        attribute_declaration();
        }
        
      } else {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case ATTRIBUTE_T:{if (!hasError) {
          
          attribute_specification();
          }
          
          break;
          }
        case DISCONNECT_T:{if (!hasError) {
          
          disconnection_specification();
          }
          
          break;
          }
        case USE_T:{if (!hasError) {
          
          use_clause();
          }
          
          break;
          }
        default:
          jj_la1[160] = jj_gen;
          if (jj_2_57(3)) {if (!hasError) {
            
            group_template_declaration();
            }
            
          } else {
            switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
            case GROUP_T:{if (!hasError) {
              
              group_declaration();
              }
              
              break;
              }
            default:
              jj_la1[161] = jj_gen;
              if (jj_2_58(5)) {if (!hasError) {
                
                package_instantiation_declaration();
                }
                
              } else {
                switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
                case PACKAGE_T:{if (!hasError) {
                  
                  package_declaration();
                  }
                  
                  break;
                  }
                default:
                  jj_la1[162] = jj_gen;
                  jj_consume_token(-1);
                  errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
                }
              }
            }
          }
        }
      }
    }
}


void VhdlParser::package_declarative_part() {if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ALIAS_T:
      case ATTRIBUTE_T:
      case COMPONENT_T:
      case CONSTANT_T:
      case DISCONNECT_T:
      case FILE_T:
      case FUNCTION_T:
      case GROUP_T:
      case IMPURE_T:
      case PACKAGE_T:
      case PROCEDURE_T:
      case PURE_T:
      case SIGNAL_T:
      case SHARED_T:
      case SUBTYPE_T:
      case TYPE_T:
      case USE_T:
      case VARIABLE_T:{
        ;
        break;
        }
      default:
        jj_la1[163] = jj_gen;
        goto end_label_30;
      }if (!hasError) {
      
      package_declarative_item();
      }
      
    }
    end_label_30: ;
    }
    
}


QCString VhdlParser::parameter_specification() {QCString s,s1;if (!hasError) {
    
    s = identifier();
    }
    if (!hasError) {
    
    jj_consume_token(IN_T);
    }
    if (!hasError) {
    
    s1 = discrete_range();
    }
    
return s+" in "+s1;
assert(false);
}


QCString VhdlParser::physical_literal() {QCString s,s1;if (!hasError) {
    
    if (jj_2_59(2147483647)) {if (!hasError) {
      
      s = abstract_literal();
      }
      
    } else {
      ;
    }
    }
    if (!hasError) {
    
    s1 = name();
    }
    
s+=" ";s+=s1;s.prepend(" "); return s;
assert(false);
}


QCString VhdlParser::physical_type_definition() {QCString s,s1,s2;if (!hasError) {
    
    jj_consume_token(UNITS_T);
    }
    if (!hasError) {
    
    s = identifier();
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    if (!hasError) {
    
addVhdlType(s.data(),getLine(),Entry::VARIABLE_SEC,VhdlDocGen::UNITS,0,0,Public);
    }
    if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case BASIC_IDENTIFIER:
      case EXTENDED_CHARACTER:{
        ;
        break;
        }
      default:
        jj_la1[164] = jj_gen;
        goto end_label_31;
      }if (!hasError) {
      
      s1 = secondary_unit_declaration();
      }
      
    }
    end_label_31: ;
    }
    if (!hasError) {
    
    jj_consume_token(END_T);
    }
    if (!hasError) {
    
    jj_consume_token(UNITS_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      name();
      }
      
      break;
      }
    default:
      jj_la1[165] = jj_gen;
      ;
    }
    }
    
return s+"%";
assert(false);
}


void VhdlParser::port_clause() {if (!hasError) {
    
    jj_consume_token(PORT_T);
    }
    if (!hasError) {
    
    jj_consume_token(LPAREN_T);
    }
    if (!hasError) {
    
    port_list();
    }
    if (!hasError) {
    
    jj_consume_token(RPAREN_T);
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
currP=0;
}


QCString VhdlParser::port_list() {QCString s;if (!hasError) {
    
    s = interface_list();
    }
    
return s;
assert(false);
}


void VhdlParser::port_map_aspect() {if (!hasError) {
    
    jj_consume_token(PORT_T);
    }
    if (!hasError) {
    
    jj_consume_token(MAP_T);
    }
    if (!hasError) {
    
    jj_consume_token(LPAREN_T);
    }
    if (!hasError) {
    
    association_list();
    }
    if (!hasError) {
    
    jj_consume_token(RPAREN_T);
    }
    
}


QCString VhdlParser::primary() {QCString s,s1;
    if (jj_2_60(2147483647)) {if (!hasError) {
      
      s = function_call();
      }
      if (!hasError) {
      
return s;
      }
      
    } else if (jj_2_61(2147483647)) {if (!hasError) {
      
      jj_consume_token(LPAREN_T);
      }
      if (!hasError) {
      
      s1 = expression();
      }
      if (!hasError) {
      
      jj_consume_token(RPAREN_T);
      }
      if (!hasError) {
      
s="("+s1+")"; return s;
      }
      
    } else if (jj_2_62(2147483647)) {if (!hasError) {
      
      s = qualified_expression();
      }
      if (!hasError) {
      
return s;
      }
      
    } else if (jj_2_63(2147483647)) {if (!hasError) {
      
      s = type_conversion();
      }
      if (!hasError) {
      
return s;
      }
      
    } else if (jj_2_64(2147483647)) {if (!hasError) {
      
      s = literal();
      }
      if (!hasError) {
      
s.prepend(" ");return s;
      }
      
    } else if (jj_2_65(2147483647)) {if (!hasError) {
      
      s = name();
      }
      if (!hasError) {
      
return s;
      }
      
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case NEW_T:{if (!hasError) {
        
        allocator();
        }
        if (!hasError) {
        
return "";
        }
        
        break;
        }
      case LPAREN_T:{if (!hasError) {
        
        s = aggregate();
        }
        if (!hasError) {
        
return s;
        }
        
        break;
        }
      default:
        jj_la1[166] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
assert(false);
return QCString();
}


void VhdlParser::primary_unit() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case ENTITY_T:{if (!hasError) {
      
      entity_declaration();
      }
      
      break;
      }
    case CONFIGURATION_T:{if (!hasError) {
      
      configuration_declaration();
      }
      
      break;
      }
    default:
      jj_la1[167] = jj_gen;
      if (jj_2_66(2147483647)) {if (!hasError) {
        
        package_instantiation_declaration();
        }
        
      } else if (jj_2_67(4)) {if (!hasError) {
        
        interface_package_declaration();
        }
        
      } else {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case PACKAGE_T:{if (!hasError) {
          
          package_declaration();
          }
          
          break;
          }
        default:
          jj_la1[168] = jj_gen;
          jj_consume_token(-1);
          errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
        }
      }
    }
}


QCString VhdlParser::procedure_call() {QCString s,s1;if (!hasError) {
    
    s = name();
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LPAREN_T:{if (!hasError) {
      
      jj_consume_token(LPAREN_T);
      }
      if (!hasError) {
      
      s1 = actual_parameter_part();
      }
      if (!hasError) {
      
      jj_consume_token(RPAREN_T);
      }
      if (!hasError) {
      
s1.prepend("("); s1.append(")");
      }
      
      break;
      }
    default:
      jj_la1[169] = jj_gen;
      ;
    }
    }
    
return s+s1;
assert(false);
}


QCString VhdlParser::procedure_call_statement() {QCString s,s1;if (!hasError) {
    
    if (jj_2_68(2)) {if (!hasError) {
      
      s = identifier();
      }
      if (!hasError) {
      
      jj_consume_token(COLON_T);
      }
      if (!hasError) {
      
s+=":";
      }
      
    } else {
      ;
    }
    }
    if (!hasError) {
    
    s1 = procedure_call();
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
return s+s1+";";
assert(false);
}


QCString VhdlParser::process_declarative_item() {QCString s;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case FUNCTION_T:
    case IMPURE_T:
    case PROCEDURE_T:
    case PURE_T:{if (!hasError) {
      
      subprogram_declaration();
      }
      if (!hasError) {
      
return "";
      }
      
      break;
      }
    case TYPE_T:{if (!hasError) {
      
      s = type_declaration();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    case SUBTYPE_T:{if (!hasError) {
      
      s = subtype_declaration();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    case CONSTANT_T:{if (!hasError) {
      
      s = constant_declaration();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    case SHARED_T:
    case VARIABLE_T:{if (!hasError) {
      
      s = variable_declaration();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    case FILE_T:{if (!hasError) {
      
      s = file_declaration();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    case ALIAS_T:{if (!hasError) {
      
      s = alias_declaration();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    default:
      jj_la1[170] = jj_gen;
      if (jj_2_69(3)) {if (!hasError) {
        
        s = attribute_declaration();
        }
        if (!hasError) {
        
return s;
        }
        
      } else {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case ATTRIBUTE_T:{if (!hasError) {
          
          s = attribute_specification();
          }
          if (!hasError) {
          
return s;
          }
          
          break;
          }
        case USE_T:{if (!hasError) {
          
          s = use_clause();
          }
          if (!hasError) {
          
return s;
          }
          
          break;
          }
        default:
          jj_la1[171] = jj_gen;
          if (jj_2_70(3)) {if (!hasError) {
            
            s = group_template_declaration();
            }
            if (!hasError) {
            
return s;
            }
            
          } else {
            switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
            case GROUP_T:{if (!hasError) {
              
              s = group_declaration();
              }
              if (!hasError) {
              
return s;
              }
              
              break;
              }
            default:
              jj_la1[172] = jj_gen;
              jj_consume_token(-1);
              errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
            }
          }
        }
      }
    }
assert(false);
return QCString();
}


QCString VhdlParser::process_declarative_part() {QCString s,s1;if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ALIAS_T:
      case ATTRIBUTE_T:
      case CONSTANT_T:
      case FILE_T:
      case FUNCTION_T:
      case GROUP_T:
      case IMPURE_T:
      case PROCEDURE_T:
      case PURE_T:
      case SHARED_T:
      case SUBTYPE_T:
      case TYPE_T:
      case USE_T:
      case VARIABLE_T:{
        ;
        break;
        }
      default:
        jj_la1[173] = jj_gen;
        goto end_label_32;
      }if (!hasError) {
      
      s1 = process_declarative_item();
      }
      if (!hasError) {
      
s+=s1;
      }
      
    }
    end_label_32: ;
    }
    
return s;
assert(false);
}


void VhdlParser::process_statement() {QCString s,s1,s2;Token *tok=0;if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      s = identifier();
      }
      if (!hasError) {
      
      jj_consume_token(COLON_T);
      }
      
      break;
      }
    default:
      jj_la1[174] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case POSTPONED_T:{if (!hasError) {
      
      jj_consume_token(POSTPONED_T);
      }
      
      break;
      }
    default:
      jj_la1[175] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
currP=VhdlDocGen::PROCESS;
               current->startLine=getLine();
               current->bodyLine=getLine();
    }
    if (!hasError) {
    
    jj_consume_token(PROCESS_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LPAREN_T:{if (!hasError) {
      
      jj_consume_token(LPAREN_T);
      }
      if (!hasError) {
      
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ALL_T:{if (!hasError) {
        
        tok = jj_consume_token(ALL_T);
        }
        
        break;
        }
      case SLSL_T:
      case STRINGLITERAL:
      case BASIC_IDENTIFIER:
      case EXTENDED_CHARACTER:{if (!hasError) {
        
        s1 = sensitivity_list();
        }
        
        break;
        }
      default:
        jj_la1[176] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
      }
      if (!hasError) {
      
      jj_consume_token(RPAREN_T);
      }
      
      break;
      }
    default:
      jj_la1[177] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case IS_T:{if (!hasError) {
      
      jj_consume_token(IS_T);
      }
      
      break;
      }
    default:
      jj_la1[178] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    s2 = process_declarative_part();
    }
    if (!hasError) {
    
if (s2.data())
                  FlowChart::addFlowChart(FlowChart::VARIABLE_NO,s2.data(),0);
                FlowChart::addFlowChart(FlowChart::BEGIN_NO,"BEGIN",0);
    }
    if (!hasError) {
    
    jj_consume_token(BEGIN_T);
    }
    if (!hasError) {
    
    process_statement_part();
    }
    if (!hasError) {
    
    jj_consume_token(END_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case POSTPONED_T:{if (!hasError) {
      
      jj_consume_token(POSTPONED_T);
      }
      
      break;
      }
    default:
      jj_la1[179] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(PROCESS_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      identifier();
      }
      
      break;
      }
    default:
      jj_la1[180] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
if(s.isEmpty())
   currName=VhdlDocGen::getProcessNumber();
   else
   currName=s;

   current->name=currName;
   tempEntry=current;
   current->endBodyLine=getLine();
   currP=0;
 if(tok)
   s1=tok->image.data();
  createFunction(currName,VhdlDocGen::PROCESS,s1.data());
  createFlow();
   currName="";
   newEntry();
}


void VhdlParser::process_statement_part() {if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ASSERT_T:
      case CASE_T:
      case EXIT_T:
      case FOR_T:
      case IF_T:
      case LOOP_T:
      case NEXT_T:
      case NULL_T:
      case REPORT_T:
      case RETURN_T:
      case WAIT_T:
      case WHILE_T:
      case WITH_T:
      case LPAREN_T:
      case SLSL_T:
      case STRINGLITERAL:
      case BASIC_IDENTIFIER:
      case EXTENDED_CHARACTER:{
        ;
        break;
        }
      default:
        jj_la1[181] = jj_gen;
        goto end_label_33;
      }if (!hasError) {
      
      sequential_statement();
      }
      
    }
    end_label_33: ;
    }
    
}


QCString VhdlParser::qualified_expression() {QCString s,s1;if (!hasError) {
    
    s1 = identifier();
    }
    if (!hasError) {
    
    jj_consume_token(APOSTROPHE_T);
    }
    if (!hasError) {
    
s=s1+"'";
    }
    if (!hasError) {
    
    if (jj_2_71(2147483647)) {if (!hasError) {
      
      s1 = aggregate();
      }
      if (!hasError) {
      
s+=s1;
      }
      
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case LPAREN_T:{if (!hasError) {
        
        jj_consume_token(LPAREN_T);
        }
        if (!hasError) {
        
        s1 = expression();
        }
        if (!hasError) {
        
        jj_consume_token(RPAREN_T);
        }
        if (!hasError) {
        
s+="(";s+=s1;s+=")";
        }
        
        break;
        }
      default:
        jj_la1[182] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
    }
    
return s;
assert(false);
}


QCString VhdlParser::range() {QCString s,s1,s2;
    if (jj_2_72(2147483647)) {if (!hasError) {
      
      s = simple_expression();
      }
      if (!hasError) {
      
      s1 = direction();
      }
      if (!hasError) {
      
      s2 = simple_expression();
      }
      if (!hasError) {
      
return s+" "+s1+" "+s2;
      }
      
    } else if (jj_2_73(2147483647)) {if (!hasError) {
      
      s = attribute_name();
      }
      if (!hasError) {
      
return s;
      }
      
    } else {
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


QCString VhdlParser::range_constraint() {QCString s,s1;if (!hasError) {
    
    jj_consume_token(RANGE_T);
    }
    if (!hasError) {
    
    s = range();
    }
    
return " range "+s;
assert(false);
}


void VhdlParser::record_type_definition() {if (!hasError) {
    
    jj_consume_token(RECORD_T);
    }
    if (!hasError) {
    
    while (!hasError) {if (!hasError) {
      
      element_declaration();
      }
      
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case BASIC_IDENTIFIER:
      case EXTENDED_CHARACTER:{
        ;
        break;
        }
      default:
        jj_la1[183] = jj_gen;
        goto end_label_34;
      }
    }
    end_label_34: ;
    }
    if (!hasError) {
    
    jj_consume_token(END_T);
    }
    if (!hasError) {
    
    jj_consume_token(RECORD_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      name();
      }
      
      break;
      }
    default:
      jj_la1[184] = jj_gen;
      ;
    }
    }
    
}


QCString VhdlParser::relation() {QCString s,s1,s2;if (!hasError) {
    
    s = shift_expression();
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LESSTHAN_T:
    case GREATERTHAN_T:
    case LT_T:
    case GT_T:
    case EQU_T:
    case NOTEQU_T:{if (!hasError) {
      
      s1 = relation_operator();
      }
      if (!hasError) {
      
      s2 = shift_expression();
      }
      
      break;
      }
    default:
      jj_la1[185] = jj_gen;
      ;
    }
    }
    
return s+s1+s2;
assert(false);
}


QCString VhdlParser::relation_operator() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LT_T:{if (!hasError) {
      
      jj_consume_token(LT_T);
      }
      if (!hasError) {
      
return "<";
      }
      
      break;
      }
    case GT_T:{if (!hasError) {
      
      jj_consume_token(GT_T);
      }
      if (!hasError) {
      
return ">";
      }
      
      break;
      }
    case EQU_T:{if (!hasError) {
      
      jj_consume_token(EQU_T);
      }
      if (!hasError) {
      
return "=";
      }
      
      break;
      }
    case GREATERTHAN_T:{if (!hasError) {
      
      jj_consume_token(GREATERTHAN_T);
      }
      if (!hasError) {
      
return ">=";
      }
      
      break;
      }
    case LESSTHAN_T:{if (!hasError) {
      
      jj_consume_token(LESSTHAN_T);
      }
      if (!hasError) {
      
return "<=";
      }
      
      break;
      }
    case NOTEQU_T:{if (!hasError) {
      
      jj_consume_token(NOTEQU_T);
      }
      if (!hasError) {
      
return "/=";
      }
      
      break;
      }
    default:
      jj_la1[186] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


QCString VhdlParser::report_statement() {Token *t=0;Token *t1=0;QCString s,s1,s2;if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      s = identifier();
      }
      if (!hasError) {
      
      t = jj_consume_token(COLON_T);
      }
      
      break;
      }
    default:
      jj_la1[187] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(REPORT_T);
    }
    if (!hasError) {
    
    s1 = expression();
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SEVERITY_T:{if (!hasError) {
      
      t1 = jj_consume_token(SEVERITY_T);
      }
      if (!hasError) {
      
      s2 = expression();
      }
      
      break;
      }
    default:
      jj_la1[188] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
if(t) s.append(":");
        s1.prepend(" report ");
        if(t1) s2.prepend(" severity ");
        return s+s1+s2+";";
assert(false);
}


QCString VhdlParser::return_statement() {QCString s,s1;if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      s = identifier();
      }
      if (!hasError) {
      
      jj_consume_token(COLON_T);
      }
      if (!hasError) {
      
s+=":";
      }
      
      break;
      }
    default:
      jj_la1[189] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(RETURN_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case ABS_T:
    case NEW_T:
    case NOT_T:
    case NULL_T:
    case LPAREN_T:
    case PLUS_T:
    case MINUS_T:
    case SLSL_T:
    case INTEGER:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:
    case CHARACTER_LITERAL:
    case DECIMAL_LITERAL:
    case BASED_LITERAL:
    case BIT_STRING_LITERAL:{if (!hasError) {
      
      s1 = expression();
      }
      
      break;
      }
    default:
      jj_la1[190] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
return s+" return "+s1+";";
assert(false);
}


QCString VhdlParser::scalar_type_definition() {QCString s,s1;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LPAREN_T:{if (!hasError) {
      
      s = enumeration_type_definition();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    case RANGE_T:{if (!hasError) {
      
      s = range_constraint();
      }
      if (!hasError) {
      
      if (jj_2_74(2147483647)) {if (!hasError) {
        
        s1 = physical_type_definition();
        }
        
      } else {
        ;
      }
      }
      if (!hasError) {
      
return s+" "+s1+"%";
      }
      
      break;
      }
    default:
      jj_la1[191] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


void VhdlParser::secondary_unit() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case ARCHITECTURE_T:{if (!hasError) {
      
      architecture_body();
      }
      
      break;
      }
    case PACKAGE_T:{if (!hasError) {
      
      package_body();
      }
      
      break;
      }
    default:
      jj_la1[192] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
}


QCString VhdlParser::secondary_unit_declaration() {QCString s,s1;if (!hasError) {
    
    s = identifier();
    }
    if (!hasError) {
    
    jj_consume_token(EQU_T);
    }
    if (!hasError) {
    
    s1 = physical_literal();
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
//printf("\n %s %s [%d]",s.data(),s1.data(),getLine());
  addVhdlType(s.data(),getLine(),Entry::VARIABLE_SEC,VhdlDocGen::UNITS,0,s1.data(),Public);

  return s+"="+s1;
assert(false);
}


QCString VhdlParser::selected_name() {QCString s,s1;if (!hasError) {
    
    s = identifier();
    }
    if (!hasError) {
    
    jj_consume_token(DOT_T);
    }
    if (!hasError) {
    
    s1 = suffix();
    }
    
return s+"."+s1;
assert(false);
}


void VhdlParser::selected_signal_assignment() {if (!hasError) {
    
    jj_consume_token(WITH_T);
    }
    if (!hasError) {
    
    expression();
    }
    if (!hasError) {
    
    jj_consume_token(SELECT_T);
    }
    if (!hasError) {
    
    target();
    }
    if (!hasError) {
    
    jj_consume_token(LESSTHAN_T);
    }
    if (!hasError) {
    
    options();
    }
    if (!hasError) {
    
    selected_waveforms();
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
}


void VhdlParser::selected_waveforms() {if (!hasError) {
    
    waveform();
    }
    if (!hasError) {
    
    jj_consume_token(WHEN_T);
    }
    if (!hasError) {
    
    choices();
    }
    if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case COMMA_T:{
        ;
        break;
        }
      default:
        jj_la1[193] = jj_gen;
        goto end_label_35;
      }if (!hasError) {
      
      jj_consume_token(COMMA_T);
      }
      if (!hasError) {
      
      waveform();
      }
      if (!hasError) {
      
      jj_consume_token(WHEN_T);
      }
      if (!hasError) {
      
      choices();
      }
      
    }
    end_label_35: ;
    }
    
}


QCString VhdlParser::sensitivity_clause() {QCString s;if (!hasError) {
    
    jj_consume_token(ON_T);
    }
    if (!hasError) {
    
    s = sensitivity_list();
    }
    
s.prepend(" on ");
  return s;
assert(false);
}


QCString VhdlParser::sensitivity_list() {QCString s,s1;if (!hasError) {
    
    s = name();
    }
    if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case COMMA_T:{
        ;
        break;
        }
      default:
        jj_la1[194] = jj_gen;
        goto end_label_36;
      }if (!hasError) {
      
      jj_consume_token(COMMA_T);
      }
      if (!hasError) {
      
      s1 = name();
      }
      if (!hasError) {
      
s+=",";s+=s1;
      }
      
    }
    end_label_36: ;
    }
    
return s;
assert(false);
}


QCString VhdlParser::sequence_of_statement() {QCString s,s1;if (!hasError) {
    
    while (!hasError) {
      if (jj_2_75(3)) {
        ;
      } else {
        goto end_label_37;
      }if (!hasError) {
      
      s1 = sequential_statement();
      }
      if (!hasError) {
      
s+=s1;
      }
      
    }
    end_label_37: ;
    }
    
return s;
assert(false);
}


QCString VhdlParser::sequential_statement() {QCString s;
    if (jj_2_76(2147483647)) {if (!hasError) {
      
      s = signal_assignment_statement();
      }
      if (!hasError) {
      
FlowChart::addFlowChart(FlowChart::TEXT_NO,s.data(),0);return s;
      }
      
    } else if (jj_2_77(3)) {if (!hasError) {
      
      s = assertion_statement();
      }
      if (!hasError) {
      
FlowChart::addFlowChart(FlowChart::TEXT_NO,s.data(),0);return s;
      }
      
    } else if (jj_2_78(3)) {if (!hasError) {
      
      s = report_statement();
      }
      if (!hasError) {
      
FlowChart::addFlowChart(FlowChart::TEXT_NO,s.data(),0);return s;
      }
      
    } else if (jj_2_79(3)) {if (!hasError) {
      
      s = wait_statement();
      }
      if (!hasError) {
      
FlowChart::addFlowChart(FlowChart::TEXT_NO,s.data(),0);return s;
      }
      
    } else if (jj_2_80(2147483647)) {if (!hasError) {
      
      s = variable_assignment_statement();
      }
      if (!hasError) {
      
FlowChart::addFlowChart(FlowChart::TEXT_NO,s.data(),0);return s;
      }
      
    } else if (jj_2_81(3)) {if (!hasError) {
      
      s = procedure_call_statement();
      }
      if (!hasError) {
      
FlowChart::addFlowChart(FlowChart::TEXT_NO,s.data(),0);return s;
      }
      
    } else if (jj_2_82(3)) {if (!hasError) {
      
      if_statement();
      }
      if (!hasError) {
      
return s;
      }
      
    } else if (jj_2_83(3)) {if (!hasError) {
      
      case_statement();
      }
      if (!hasError) {
      
return s;
      }
      
    } else if (jj_2_84(3)) {if (!hasError) {
      
      loop_statement();
      }
      if (!hasError) {
      
return s;
      }
      
    } else if (jj_2_85(3)) {if (!hasError) {
      
      s = next_statement();
      }
      if (!hasError) {
      
return s;
      }
      
    } else if (jj_2_86(3)) {if (!hasError) {
      
      s = exit_statement();
      }
      if (!hasError) {
      
return s;
      }
      
    } else if (jj_2_87(3)) {if (!hasError) {
      
      s = return_statement();
      }
      if (!hasError) {
      
FlowChart::addFlowChart(FlowChart::RETURN_NO,s.data(),0);return s;
      }
      
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case NULL_T:
      case BASIC_IDENTIFIER:
      case EXTENDED_CHARACTER:{if (!hasError) {
        
        s = null_statement();
        }
        if (!hasError) {
        
FlowChart::addFlowChart(FlowChart::TEXT_NO,s.data(),0);return s;
        }
        
        break;
        }
      default:
        jj_la1[195] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
assert(false);
return QCString();
}


QCString VhdlParser::shift_expression() {QCString s,s1,s2;if (!hasError) {
    
    s = simple_expression();
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case ROL_T:
    case ROR_T:
    case SLA_T:
    case SLL_T:
    case SRA_T:
    case SRL_T:{if (!hasError) {
      
      s1 = shift_operator();
      }
      if (!hasError) {
      
      s2 = simple_expression();
      }
      
      break;
      }
    default:
      jj_la1[196] = jj_gen;
      ;
    }
    }
    
return s+s1+s2;
assert(false);
}


QCString VhdlParser::shift_operator() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SLL_T:{if (!hasError) {
      
      jj_consume_token(SLL_T);
      }
      if (!hasError) {
      
return "sll";
      }
      
      break;
      }
    case SRL_T:{if (!hasError) {
      
      jj_consume_token(SRL_T);
      }
      if (!hasError) {
      
return "srl";
      }
      
      break;
      }
    case SLA_T:{if (!hasError) {
      
      jj_consume_token(SLA_T);
      }
      if (!hasError) {
      
return "sla";
      }
      
      break;
      }
    case SRA_T:{if (!hasError) {
      
      jj_consume_token(SRA_T);
      }
      if (!hasError) {
      
return "sra";
      }
      
      break;
      }
    case ROL_T:{if (!hasError) {
      
      jj_consume_token(ROL_T);
      }
      if (!hasError) {
      
return "rol";
      }
      
      break;
      }
    case ROR_T:{if (!hasError) {
      
      jj_consume_token(ROR_T);
      }
      if (!hasError) {
      
return "ror";
      }
      
      break;
      }
    default:
      jj_la1[197] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


QCString VhdlParser::sign() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case PLUS_T:{if (!hasError) {
      
      jj_consume_token(PLUS_T);
      }
      if (!hasError) {
      
return "+";
      }
      
      break;
      }
    case MINUS_T:{if (!hasError) {
      
      jj_consume_token(MINUS_T);
      }
      if (!hasError) {
      
return "-";
      }
      
      break;
      }
    default:
      jj_la1[198] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


QCString VhdlParser::signal_assignment_statement() {QCString s,s1,s2,s3;
    if (jj_2_89(2147483647)) {if (!hasError) {
      
      conditional_signal_assignment_wave();
      }
      if (!hasError) {
      
return "";
      }
      
    } else if (jj_2_90(2147483647)) {if (!hasError) {
      
      selected_signal_assignment_wave();
      }
      if (!hasError) {
      
return "";
      }
      
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case LPAREN_T:
      case SLSL_T:
      case STRINGLITERAL:
      case BASIC_IDENTIFIER:
      case EXTENDED_CHARACTER:{if (!hasError) {
        
        if (jj_2_88(2)) {if (!hasError) {
          
          s = identifier();
          }
          if (!hasError) {
          
          jj_consume_token(COLON_T);
          }
          if (!hasError) {
          
s+=":";
          }
          
        } else {
          ;
        }
        }
        if (!hasError) {
        
        s1 = target();
        }
        if (!hasError) {
        
        jj_consume_token(LESSTHAN_T);
        }
        if (!hasError) {
        
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case INERTIAL_T:
        case REJECT_T:
        case TRANSPORT_T:{if (!hasError) {
          
          s2 = delay_mechanism();
          }
          
          break;
          }
        default:
          jj_la1[199] = jj_gen;
          ;
        }
        }
        if (!hasError) {
        
        s3 = waveform();
        }
        if (!hasError) {
        
        jj_consume_token(SEMI_T);
        }
        if (!hasError) {
        
return s+s1+"<="+s2+s3+";";
        }
        
        break;
        }
      default:
        jj_la1[200] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
assert(false);
return QCString();
}


void VhdlParser::semi() {if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
}


void VhdlParser::signal_declaration() {Token* tok=0;QCString s,s1,s2,s3,s4;if (!hasError) {
    
    jj_consume_token(SIGNAL_T);
    }
    if (!hasError) {
    
    s = identifier_list();
    }
    if (!hasError) {
    
    jj_consume_token(COLON_T);
    }
    if (!hasError) {
    
    s1 = subtype_indication();
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BUS_T:
    case REGISTER_T:{if (!hasError) {
      
      s2 = signal_kind();
      }
      
      break;
      }
    default:
      jj_la1[201] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case VARASSIGN_T:{if (!hasError) {
      
      tok = jj_consume_token(VARASSIGN_T);
      }
      if (!hasError) {
      
      s3 = expression();
      }
      
      break;
      }
    default:
      jj_la1[202] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
if(tok)
      s3.prepend(":=");
     s4=s1+s2+s3;
     addVhdlType(s.data(),getLine(),Entry::VARIABLE_SEC,VhdlDocGen::SIGNAL,0,s4.data(),Public);
}


QCString VhdlParser::signal_kind() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case REGISTER_T:{if (!hasError) {
      
      jj_consume_token(REGISTER_T);
      }
      if (!hasError) {
      
return "register";
      }
      
      break;
      }
    case BUS_T:{if (!hasError) {
      
      jj_consume_token(BUS_T);
      }
      if (!hasError) {
      
return "bus";
      }
      
      break;
      }
    default:
      jj_la1[203] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


QCString VhdlParser::signal_list() {QCString s,s1;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      s = name();
      }
      if (!hasError) {
      
      while (!hasError) {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case COMMA_T:{
          ;
          break;
          }
        default:
          jj_la1[204] = jj_gen;
          goto end_label_38;
        }if (!hasError) {
        
        jj_consume_token(COMMA_T);
        }
        if (!hasError) {
        
        s1 = name();
        }
        if (!hasError) {
        
s+=",";s+=s1;
        }
        
      }
      end_label_38: ;
      }
      
      break;
      }
    case OTHER_T:{if (!hasError) {
      
      jj_consume_token(OTHER_T);
      }
      if (!hasError) {
      
return "other";
      }
      
      break;
      }
    case ALL_T:{if (!hasError) {
      
      jj_consume_token(ALL_T);
      }
      if (!hasError) {
      
return "all";
      }
      
      break;
      }
    default:
      jj_la1[205] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


QCString VhdlParser::signature() {QCString s,s1,s2;if (!hasError) {
    
    jj_consume_token(LBRACKET_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      s = name();
      }
      if (!hasError) {
      
      while (!hasError) {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case COMMA_T:{
          ;
          break;
          }
        default:
          jj_la1[206] = jj_gen;
          goto end_label_39;
        }if (!hasError) {
        
        jj_consume_token(COMMA_T);
        }
        if (!hasError) {
        
        s1 = name();
        }
        if (!hasError) {
        
s+=",";s+=s1;
        }
        
      }
      end_label_39: ;
      }
      
      break;
      }
    default:
      jj_la1[207] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case RETURN_T:{if (!hasError) {
      
      jj_consume_token(RETURN_T);
      }
      if (!hasError) {
      
      s1 = name();
      }
      if (!hasError) {
      
s+="return ";s+=s1;
      }
      
      break;
      }
    default:
      jj_la1[208] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(RBRACKET_T);
    }
    
s1="["+s+"]";return s1;
assert(false);
}


QCString VhdlParser::simple_expression() {QCString s,s1,s2;if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case PLUS_T:
    case MINUS_T:{if (!hasError) {
      
      s = sign();
      }
      
      break;
      }
    default:
      jj_la1[209] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    s1 = term();
    }
    if (!hasError) {
    
s+=s1;
    }
    if (!hasError) {
    
    while (!hasError) {
      if (jj_2_91(2147483647)) {
        ;
      } else {
        goto end_label_40;
      }if (!hasError) {
      
      s1 = adding_operator();
      }
      if (!hasError) {
      
      s2 = term();
      }
      if (!hasError) {
      
s+=s1;s+=s2;
      }
      
    }
    end_label_40: ;
    }
    
return s;
assert(false);
}


void VhdlParser::simple_name() {if (!hasError) {
    
    name();
    }
    
}


QCString VhdlParser::slice_name() {QCString s,s1;if (!hasError) {
    
    s = identifier();
    }
    if (!hasError) {
    
    jj_consume_token(LPAREN_T);
    }
    if (!hasError) {
    
    s1 = discrete_range();
    }
    if (!hasError) {
    
    jj_consume_token(RPAREN_T);
    }
    
return s+"("+s1+")";
assert(false);
}


QCString VhdlParser::string_literal() {Token *tok=0;if (!hasError) {
    
    tok = jj_consume_token(STRINGLITERAL);
    }
    
return tok->image.c_str();
assert(false);
}


void VhdlParser::subprogram_body() {QCString s;if (!hasError) {
    
    jj_consume_token(IS_T);
    }
    if (!hasError) {
    
    //try{
     s = subprogram_declarative_part();
    }
    if (!hasError) {
    
if (s.data())
      {
        FlowChart::addFlowChart(FlowChart::VARIABLE_NO,s,0);
      }
      FlowChart::addFlowChart(FlowChart::BEGIN_NO,"BEGIN",0);
    }
    if (!hasError) {
    
    jj_consume_token(BEGIN_T);
    }
    if (!hasError) {
    
    subprogram_statement_part();
    }
    if (!hasError) {
    
    jj_consume_token(END_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case FUNCTION_T:
    case PROCEDURE_T:{if (!hasError) {
      
      subprogram_kind();
      }
      
      break;
      }
    default:
      jj_la1[210] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      designator();
      }
      
      break;
      }
    default:
      jj_la1[211] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
tempEntry->endBodyLine=getLine(END_T);
      createFlow();
      currP=0;
}


void VhdlParser::subprogram_declaration() {
    if (jj_2_92(2147483647)) {if (!hasError) {
      
      subprogram_instantiation_declaration();
      }
      
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case FUNCTION_T:
      case IMPURE_T:
      case PROCEDURE_T:
      case PURE_T:{if (!hasError) {
        
        subprogram_specification();
        }
        if (!hasError) {
        
        subprogram_1();
        }
        if (!hasError) {
        
currP=0;
        }
        
        break;
        }
      default:
        jj_la1[212] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
}


void VhdlParser::subprogram_1() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case IS_T:{if (!hasError) {
      
      subprogram_body();
      }
      
      break;
      }
    case SEMI_T:{if (!hasError) {
      
      jj_consume_token(SEMI_T);
      }
      
      break;
      }
    default:
      jj_la1[213] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
}


QCString VhdlParser::subprogram_declarative_item() {QCString s;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case FUNCTION_T:
    case IMPURE_T:
    case PROCEDURE_T:
    case PURE_T:{if (!hasError) {
      
      subprogram_declaration();
      }
      if (!hasError) {
      
return "";
      }
      
      break;
      }
    case TYPE_T:{if (!hasError) {
      
      s = type_declaration();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    case IS_T:{if (!hasError) {
      
      subprogram_body();
      }
      if (!hasError) {
      
return "";
      }
      
      break;
      }
    case SUBTYPE_T:{if (!hasError) {
      
      s = subtype_declaration();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    case CONSTANT_T:{if (!hasError) {
      
      s = constant_declaration();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    case SHARED_T:
    case VARIABLE_T:{if (!hasError) {
      
      s = variable_declaration();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    case FILE_T:{if (!hasError) {
      
      s = file_declaration();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    case ALIAS_T:{if (!hasError) {
      
      s = alias_declaration();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    default:
      jj_la1[214] = jj_gen;
      if (jj_2_93(2147483647)) {if (!hasError) {
        
        s = attribute_declaration();
        }
        if (!hasError) {
        
return s;
        }
        
      } else {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case ATTRIBUTE_T:{if (!hasError) {
          
          s = attribute_specification();
          }
          if (!hasError) {
          
return s;
          }
          
          break;
          }
        case USE_T:{if (!hasError) {
          
          s = use_clause();
          }
          if (!hasError) {
          
return s;
          }
          
          break;
          }
        default:
          jj_la1[215] = jj_gen;
          if (jj_2_94(3)) {if (!hasError) {
            
            s = group_template_declaration();
            }
            if (!hasError) {
            
return s;
            }
            
          } else {
            switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
            case GROUP_T:{if (!hasError) {
              
              s = group_declaration();
              }
              if (!hasError) {
              
return s;
              }
              
              break;
              }
            default:
              jj_la1[216] = jj_gen;
              jj_consume_token(-1);
              errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
            }
          }
        }
      }
    }
assert(false);
return QCString();
}


QCString VhdlParser::subprogram_declarative_part() {QCString s,s1;if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ALIAS_T:
      case ATTRIBUTE_T:
      case CONSTANT_T:
      case FILE_T:
      case FUNCTION_T:
      case GROUP_T:
      case IMPURE_T:
      case IS_T:
      case PROCEDURE_T:
      case PURE_T:
      case SHARED_T:
      case SUBTYPE_T:
      case TYPE_T:
      case USE_T:
      case VARIABLE_T:{
        ;
        break;
        }
      default:
        jj_la1[217] = jj_gen;
        goto end_label_41;
      }if (!hasError) {
      
      s1 = subprogram_declarative_item();
      }
      if (!hasError) {
      
s+=s1;
      }
      
    }
    end_label_41: ;
    }
    
return s;
assert(false);
}


void VhdlParser::subprogram_kind() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case FUNCTION_T:{if (!hasError) {
      
      jj_consume_token(FUNCTION_T);
      }
      
      break;
      }
    case PROCEDURE_T:{if (!hasError) {
      
      jj_consume_token(PROCEDURE_T);
      }
      
      break;
      }
    default:
      jj_la1[218] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
}


void VhdlParser::subprogram_specification() {QCString s;Token *tok=0;Token *t;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case PROCEDURE_T:{if (!hasError) {
      
      jj_consume_token(PROCEDURE_T);
      }
      if (!hasError) {
      
      s = designator();
      }
      if (!hasError) {
      
currP=VhdlDocGen::PROCEDURE;
              createFunction(s.data(),currP,0);
              tempEntry=current;
               current->startLine=getLine(PROCEDURE_T);
               current->bodyLine=getLine(PROCEDURE_T);
      }
      if (!hasError) {
      
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case LPAREN_T:{if (!hasError) {
        
        jj_consume_token(LPAREN_T);
        }
        if (!hasError) {
        
param_sec=PARAM_SEC;
        }
        if (!hasError) {
        
        interface_list();
        }
        if (!hasError) {
        
param_sec=0;
        }
        if (!hasError) {
        
        jj_consume_token(RPAREN_T);
        }
        
        break;
        }
      default:
        jj_la1[219] = jj_gen;
        ;
      }
      }
      if (!hasError) {
      
      if (jj_2_95(2)) {if (!hasError) {
        
        gen_interface_list();
        }
        
      } else {
        ;
      }
      }
      if (!hasError) {
      
      if (jj_2_96(2)) {if (!hasError) {
        
        gen_assoc_list();
        }
        
      } else {
        ;
      }
      }
      if (!hasError) {
      
      param();
      }
      if (!hasError) {
      
newEntry();
      }
      
      break;
      }
    case FUNCTION_T:
    case IMPURE_T:
    case PURE_T:{if (!hasError) {
      
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case IMPURE_T:
      case PURE_T:{if (!hasError) {
        
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case PURE_T:{if (!hasError) {
          
          tok = jj_consume_token(PURE_T);
          }
          
          break;
          }
        case IMPURE_T:{if (!hasError) {
          
          tok = jj_consume_token(IMPURE_T);
          }
          
          break;
          }
        default:
          jj_la1[220] = jj_gen;
          jj_consume_token(-1);
          errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
        }
        }
        
        break;
        }
      default:
        jj_la1[221] = jj_gen;
        ;
      }
      }
      if (!hasError) {
      
      t = jj_consume_token(FUNCTION_T);
      }
      if (!hasError) {
      
      s = designator();
      }
      if (!hasError) {
      
currP=VhdlDocGen::FUNCTION;
     if(tok)
     createFunction(tok->image.c_str(),currP,s.data());
     else
     createFunction(0,currP,s.data());
      tempEntry=current;
      current->startLine=getLine(FUNCTION_T);
      current->bodyLine=getLine(FUNCTION_T);
      }
      if (!hasError) {
      
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case LPAREN_T:{if (!hasError) {
        
param_sec=PARAM_SEC;
        }
        if (!hasError) {
        
        jj_consume_token(LPAREN_T);
        }
        if (!hasError) {
        
        formal_parameter_list();
        }
        if (!hasError) {
        
        jj_consume_token(RPAREN_T);
        }
        if (!hasError) {
        
param_sec=0;
        }
        
        break;
        }
      default:
        jj_la1[222] = jj_gen;
        ;
      }
      }
      if (!hasError) {
      
      jj_consume_token(RETURN_T);
      }
      if (!hasError) {
      
      s = type_mark();
      }
      if (!hasError) {
      
tempEntry=current;
                current->type=s;
                newEntry();
      }
      
      break;
      }
    default:
      jj_la1[223] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
}


void VhdlParser::subprogram_statement_part() {if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ASSERT_T:
      case CASE_T:
      case EXIT_T:
      case FOR_T:
      case IF_T:
      case LOOP_T:
      case NEXT_T:
      case NULL_T:
      case REPORT_T:
      case RETURN_T:
      case WAIT_T:
      case WHILE_T:
      case WITH_T:
      case LPAREN_T:
      case SLSL_T:
      case STRINGLITERAL:
      case BASIC_IDENTIFIER:
      case EXTENDED_CHARACTER:{
        ;
        break;
        }
      default:
        jj_la1[224] = jj_gen;
        goto end_label_42;
      }if (!hasError) {
      
      sequential_statement();
      }
      
    }
    end_label_42: ;
    }
    
}


QCString VhdlParser::subtype_declaration() {QCString s,s1;if (!hasError) {
    
    jj_consume_token(SUBTYPE_T);
    }
    if (!hasError) {
    
    s = identifier();
    }
    if (!hasError) {
    
    jj_consume_token(IS_T);
    }
    if (!hasError) {
    
    s1 = subtype_indication();
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
addVhdlType(s.data(),getLine(),Entry::VARIABLE_SEC,VhdlDocGen::SUBTYPE,0,s1.data(),Public);
  return " subtype "+s+" is "+s1+";";
assert(false);
}


QCString VhdlParser::subtype_indication() {QCString s,s1,s2;if (!hasError) {
    
    s = name();
    }
    if (!hasError) {
    
    if (jj_2_97(2147483647)) {if (!hasError) {
      
      s1 = name();
      }
      
    } else {
      ;
    }
    }
    if (!hasError) {
    
    if (jj_2_98(2147483647)) {if (!hasError) {
      
      s2 = constraint();
      }
      
    } else {
      ;
    }
    }
    
return s+" "+s1+" "+s2;
assert(false);
}


QCString VhdlParser::suffix() {QCString s;
    if (jj_2_99(2147483647)) {if (!hasError) {
      
      s = name();
      }
      if (!hasError) {
      
return s;
      }
      
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case CHARACTER_LITERAL:{if (!hasError) {
        
        s = character_literal();
        }
        if (!hasError) {
        
return s;
        }
        
        break;
        }
      case STRINGLITERAL:{if (!hasError) {
        
        s = operator_symbol();
        }
        if (!hasError) {
        
return s;
        }
        
        break;
        }
      case ALL_T:{if (!hasError) {
        
        jj_consume_token(ALL_T);
        }
        if (!hasError) {
        
return " all ";
        }
        
        break;
        }
      default:
        jj_la1[225] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
assert(false);
return QCString();
}


QCString VhdlParser::target() {QCString s;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      s = name();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    case LPAREN_T:{if (!hasError) {
      
      s = aggregate();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    default:
      jj_la1[226] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


QCString VhdlParser::term() {QCString s,s1,s2;if (!hasError) {
    
    s = factor();
    }
    if (!hasError) {
    
    while (!hasError) {
      if (jj_2_100(2)) {
        ;
      } else {
        goto end_label_43;
      }if (!hasError) {
      
      s1 = multiplying_operation();
      }
      if (!hasError) {
      
      s2 = factor();
      }
      if (!hasError) {
      
s+=s1;s+=s2;
      }
      
    }
    end_label_43: ;
    }
    
return s;
assert(false);
}


QCString VhdlParser::timeout_clause() {QCString s;if (!hasError) {
    
    jj_consume_token(FOR_T);
    }
    if (!hasError) {
    
    s = expression();
    }
    
return " for "+s;
assert(false);
}


QCString VhdlParser::type_conversion() {QCString s,s1;if (!hasError) {
    
    s = name();
    }
    if (!hasError) {
    
    jj_consume_token(LPAREN_T);
    }
    if (!hasError) {
    
    s1 = expression();
    }
    if (!hasError) {
    
    jj_consume_token(RPAREN_T);
    }
    
return s+"("+s1+")";
assert(false);
}


QCString VhdlParser::type_declaration() {QCString s;
    if (jj_2_101(3)) {if (!hasError) {
      
      s = full_type_declaration();
      }
      if (!hasError) {
      
return s;
      }
      
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case TYPE_T:{if (!hasError) {
        
        s = incomplete_type_declaration();
        }
        if (!hasError) {
        
return s;
        }
        
        break;
        }
      default:
        jj_la1[227] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
assert(false);
return QCString();
}


QCString VhdlParser::type_definition() {QCString s;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case RANGE_T:
    case LPAREN_T:{if (!hasError) {
      
      //try{
      s = scalar_type_definition();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    case ARRAY_T:
    case RECORD_T:{if (!hasError) {
      
      s = composite_type_definition();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    case ACCESS_T:{if (!hasError) {
      
      s = access_type_definition();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    case FILE_T:{if (!hasError) {
      
      s = file_type_definition();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    default:
      jj_la1[228] = jj_gen;
      if (jj_2_102(2)) {if (!hasError) {
        
        protected_type_body();
        }
        if (!hasError) {
        
return "";
        }
        
      } else {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case PROTECTED_T:{if (!hasError) {
          
          protected_type_declaration();
          }
          if (!hasError) {
          
return "";
          }
          
          break;
          }
        default:
          jj_la1[229] = jj_gen;
          jj_consume_token(-1);
          errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
        }
      }
    }
assert(false);
return QCString();
}


QCString VhdlParser::type_mark() {QCString s;if (!hasError) {
    
    s = name();
    }
    
return s;
assert(false);
}


QCString VhdlParser::unconstraint_array_definition() {QCString s,s1,s2,s3;if (!hasError) {
    
    jj_consume_token(ARRAY_T);
    }
    if (!hasError) {
    
    jj_consume_token(LPAREN_T);
    }
    if (!hasError) {
    
    s = index_subtype_definition();
    }
    if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case COMMA_T:{
        ;
        break;
        }
      default:
        jj_la1[230] = jj_gen;
        goto end_label_44;
      }if (!hasError) {
      
      jj_consume_token(COMMA_T);
      }
      if (!hasError) {
      
      s1 = index_subtype_definition();
      }
      if (!hasError) {
      
s3+=",";s3+=s1;
      }
      
    }
    end_label_44: ;
    }
    if (!hasError) {
    
    jj_consume_token(RPAREN_T);
    }
    if (!hasError) {
    
    jj_consume_token(OF_T);
    }
    if (!hasError) {
    
    s2 = subtype_indication();
    }
    
return "array("+s+s3+") of "+s2;
assert(false);
}


QCString VhdlParser::use_clause() {QCString s,s1;if (!hasError) {
    
    jj_consume_token(USE_T);
    }
    if (!hasError) {
    
    s = selected_name();
    }
    if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case COMMA_T:{
        ;
        break;
        }
      default:
        jj_la1[231] = jj_gen;
        goto end_label_45;
      }if (!hasError) {
      
      jj_consume_token(COMMA_T);
      }
      if (!hasError) {
      
      s1 = selected_name();
      }
      if (!hasError) {
      
s+=",";s+=s1;
      }
      
    }
    end_label_45: ;
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
QStringList ql1=QStringList::split(",",s,FALSE);
                   for (uint j=0;j<ql1.count();j++)
                   {
                    QStringList ql=QStringList::split(".",ql1[j],FALSE);
                    QCString it=ql[1].utf8();
                     if ( parse_sec==0 && Config_getBool(SHOW_INCLUDE_FILES) )
                     {
                       VhdlParser::addVhdlType(it.data(),getLine(),Entry::VARIABLE_SEC,VhdlDocGen::USE,it.data(),"_use_",Public);
                     }
                   }
                   s1="use "+s;
                   return s1;
assert(false);
}


QCString VhdlParser::variable_assignment_statement() {QCString s,s1,s2;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LPAREN_T:
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      if (jj_2_103(2)) {if (!hasError) {
        
        s = identifier();
        }
        if (!hasError) {
        
        jj_consume_token(COLON_T);
        }
        if (!hasError) {
        
s+=":";
        }
        
      } else {
        ;
      }
      }
      if (!hasError) {
      
      s1 = target();
      }
      if (!hasError) {
      
      jj_consume_token(VARASSIGN_T);
      }
      if (!hasError) {
      
      s2 = expression();
      }
      if (!hasError) {
      
      jj_consume_token(SEMI_T);
      }
      if (!hasError) {
      
return s+s1+":="+s2+";";
      }
      
      break;
      }
    case WITH_T:{if (!hasError) {
      
      selected_variable_assignment();
      }
      if (!hasError) {
      
return "";
      }
      
      break;
      }
    default:
      jj_la1[232] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


QCString VhdlParser::variable_declaration() {Token *tok=0;Token *t1=0;QCString s,s1,s2;if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SHARED_T:{if (!hasError) {
      
      tok = jj_consume_token(SHARED_T);
      }
      
      break;
      }
    default:
      jj_la1[233] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(VARIABLE_T);
    }
    if (!hasError) {
    
    s = identifier_list();
    }
    if (!hasError) {
    
    jj_consume_token(COLON_T);
    }
    if (!hasError) {
    
    s1 = subtype_indication();
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case VARASSIGN_T:{if (!hasError) {
      
      t1 = jj_consume_token(VARASSIGN_T);
      }
      if (!hasError) {
      
      s2 = expression();
      }
      
      break;
      }
    default:
      jj_la1[234] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
int spec;
    if(t1)
     s2.prepend(":=");
    QCString val=" variable "+s+":"+s1+s2+";";
    QCString it=s1;
    if(tok != 0)
    {
     it.prepend(" shared ");
     val.prepend(" shared");
     spec=VhdlDocGen::SHAREDVARIABLE;
     }
     else
     spec=VhdlDocGen::SHAREDVARIABLE;

    if(t1){
     it+=":=";
     it+=s2;
     }
    addVhdlType(s.data(),getLine(),Entry::VARIABLE_SEC,spec,0,it.data(),Public);
    return val;
assert(false);
}


QCString VhdlParser::wait_statement() {QCString s,s1,s2,s3;Token *t=0;if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      s = identifier();
      }
      if (!hasError) {
      
      t = jj_consume_token(COLON_T);
      }
      
      break;
      }
    default:
      jj_la1[235] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(WAIT_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case ON_T:{if (!hasError) {
      
      s1 = sensitivity_clause();
      }
      
      break;
      }
    default:
      jj_la1[236] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case UNTIL_T:{if (!hasError) {
      
      s2 = condition_clause();
      }
      
      break;
      }
    default:
      jj_la1[237] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case FOR_T:{if (!hasError) {
      
      s3 = timeout_clause();
      }
      
      break;
      }
    default:
      jj_la1[238] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
if(t) s.append(":");
  return s+" wait "+s1+s2+s3+";";
assert(false);
}


QCString VhdlParser::waveform() {QCString s,s1;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case ABS_T:
    case NEW_T:
    case NOT_T:
    case NULL_T:
    case LPAREN_T:
    case PLUS_T:
    case MINUS_T:
    case SLSL_T:
    case INTEGER:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:
    case CHARACTER_LITERAL:
    case DECIMAL_LITERAL:
    case BASED_LITERAL:
    case BIT_STRING_LITERAL:{if (!hasError) {
      
      s = waveform_element();
      }
      if (!hasError) {
      
      while (!hasError) {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case COMMA_T:{
          ;
          break;
          }
        default:
          jj_la1[239] = jj_gen;
          goto end_label_46;
        }if (!hasError) {
        
        jj_consume_token(COMMA_T);
        }
        if (!hasError) {
        
        s1 = waveform_element();
        }
        if (!hasError) {
        
s+=","; s+=s1;
        }
        
      }
      end_label_46: ;
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    case UNAFFECTED_T:{if (!hasError) {
      
      jj_consume_token(UNAFFECTED_T);
      }
      if (!hasError) {
      
return " unaffected ";
      }
      
      break;
      }
    default:
      jj_la1[240] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


QCString VhdlParser::waveform_element() {QCString s,s1;if (!hasError) {
    
    s = expression();
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case AFTER_T:{if (!hasError) {
      
      jj_consume_token(AFTER_T);
      }
      if (!hasError) {
      
      s1 = expression();
      }
      if (!hasError) {
      
s1.prepend(" after ");
      }
      
      break;
      }
    default:
      jj_la1[241] = jj_gen;
      ;
    }
    }
    
return s+s1;
assert(false);
}


QCString VhdlParser::protected_type_body() {if (!hasError) {
    
    jj_consume_token(PROTECTED_T);
    }
    if (!hasError) {
    
    jj_consume_token(BODY_T);
    }
    if (!hasError) {
    
    protected_type_body_declarative_part();
    }
    if (!hasError) {
    
    jj_consume_token(END_T);
    }
    if (!hasError) {
    
    jj_consume_token(PROTECTED_T);
    }
    if (!hasError) {
    
    jj_consume_token(BODY_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      identifier();
      }
      
      break;
      }
    default:
      jj_la1[242] = jj_gen;
      ;
    }
    }
    
return "";
assert(false);
}


void VhdlParser::protected_type_body_declarative_item() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case FUNCTION_T:
    case IMPURE_T:
    case PROCEDURE_T:
    case PURE_T:{if (!hasError) {
      
      subprogram_declaration();
      }
      
      break;
      }
    case IS_T:{if (!hasError) {
      
      subprogram_body();
      }
      
      break;
      }
    case TYPE_T:{if (!hasError) {
      
      type_declaration();
      }
      
      break;
      }
    case SUBTYPE_T:{if (!hasError) {
      
      subtype_declaration();
      }
      
      break;
      }
    case CONSTANT_T:{if (!hasError) {
      
      constant_declaration();
      }
      
      break;
      }
    case SHARED_T:
    case VARIABLE_T:{if (!hasError) {
      
      variable_declaration();
      }
      
      break;
      }
    case FILE_T:{if (!hasError) {
      
      file_declaration();
      }
      
      break;
      }
    case ALIAS_T:{if (!hasError) {
      
      alias_declaration();
      }
      
      break;
      }
    default:
      jj_la1[243] = jj_gen;
      if (jj_2_104(2147483647)) {if (!hasError) {
        
        attribute_declaration();
        }
        
      } else {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case ATTRIBUTE_T:{if (!hasError) {
          
          attribute_specification();
          }
          
          break;
          }
        case USE_T:{if (!hasError) {
          
          use_clause();
          }
          
          break;
          }
        default:
          jj_la1[244] = jj_gen;
          if (jj_2_105(3)) {if (!hasError) {
            
            group_template_declaration();
            }
            
          } else {
            switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
            case GROUP_T:{if (!hasError) {
              
              group_declaration();
              }
              
              break;
              }
            default:
              jj_la1[245] = jj_gen;
              jj_consume_token(-1);
              errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
            }
          }
        }
      }
    }
}


void VhdlParser::protected_type_body_declarative_part() {if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ALIAS_T:
      case ATTRIBUTE_T:
      case CONSTANT_T:
      case FILE_T:
      case FUNCTION_T:
      case GROUP_T:
      case IMPURE_T:
      case IS_T:
      case PROCEDURE_T:
      case PURE_T:
      case SHARED_T:
      case SUBTYPE_T:
      case TYPE_T:
      case USE_T:
      case VARIABLE_T:{
        ;
        break;
        }
      default:
        jj_la1[246] = jj_gen;
        goto end_label_47;
      }if (!hasError) {
      
      protected_type_body_declarative_item();
      }
      
    }
    end_label_47: ;
    }
    
}


QCString VhdlParser::protected_type_declaration() {if (!hasError) {
    
    jj_consume_token(PROTECTED_T);
    }
    if (!hasError) {
    
    try {if (!hasError) {
      
      protected_type_declarative_part();
      }
      
    } catch ( ...) {
error_skipto(END_T);
    }
    }
    if (!hasError) {
    
    jj_consume_token(END_T);
    }
    if (!hasError) {
    
    jj_consume_token(PROTECTED_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      identifier();
      }
      
      break;
      }
    default:
      jj_la1[247] = jj_gen;
      ;
    }
    }
    
return "";
assert(false);
}


void VhdlParser::protected_type_declarative_item() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case FUNCTION_T:
    case IMPURE_T:
    case PROCEDURE_T:
    case PURE_T:{if (!hasError) {
      
      subprogram_specification();
      }
      
      break;
      }
    case ATTRIBUTE_T:{if (!hasError) {
      
      attribute_specification();
      }
      
      break;
      }
    case USE_T:{if (!hasError) {
      
      use_clause();
      }
      
      break;
      }
    default:
      jj_la1[248] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
}


void VhdlParser::protected_type_declarative_part() {if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ATTRIBUTE_T:
      case FUNCTION_T:
      case IMPURE_T:
      case PROCEDURE_T:
      case PURE_T:
      case USE_T:{
        ;
        break;
        }
      default:
        jj_la1[249] = jj_gen;
        goto end_label_48;
      }if (!hasError) {
      
      protected_type_declarative_item();
      }
      if (!hasError) {
      
      jj_consume_token(SEMI_T);
      }
      
    }
    end_label_48: ;
    }
    
}


QCString VhdlParser::context_ref() {QCString s;if (!hasError) {
    
    jj_consume_token(CONTEXT_T);
    }
    if (!hasError) {
    
    s = identifier_list();
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
return "context "+s ;
assert(false);
}


void VhdlParser::context_declaration() {QCString s,s1;if (!hasError) {
    
    jj_consume_token(CONTEXT_T);
    }
    if (!hasError) {
    
    s = identifier();
    }
    if (!hasError) {
    
    jj_consume_token(IS_T);
    }
    if (!hasError) {
    
parse_sec=CONTEXT_SEC;
    }
    if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case CONTEXT_T:
      case LIBRARY_T:
      case USE_T:{
        ;
        break;
        }
      default:
        jj_la1[250] = jj_gen;
        goto end_label_49;
      }if (!hasError) {
      
      s1 = libustcont_stats();
      }
      
    }
    end_label_49: ;
    }
    if (!hasError) {
    
    jj_consume_token(END_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case CONTEXT_T:{if (!hasError) {
      
      jj_consume_token(CONTEXT_T);
      }
      
      break;
      }
    default:
      jj_la1[251] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      identifier();
      }
      
      break;
      }
    default:
      jj_la1[252] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
parse_sec=0;
                          addVhdlType(s.data(),getLine(LIBRARY_T),Entry::VARIABLE_SEC,VhdlDocGen::LIBRARY,"context",s1.data(),Public);
}


QCString VhdlParser::libustcont_stats() {QCString s;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case USE_T:{if (!hasError) {
      
      s = use_clause();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    case LIBRARY_T:{if (!hasError) {
      
      s = library_clause();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    case CONTEXT_T:{if (!hasError) {
      
      s = context_ref();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    default:
      jj_la1[253] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


void VhdlParser::package_instantiation_declaration() {QCString s,s1,s2;if (!hasError) {
    
    jj_consume_token(PACKAGE_T);
    }
    if (!hasError) {
    
    s = identifier();
    }
    if (!hasError) {
    
    jj_consume_token(IS_T);
    }
    if (!hasError) {
    
    jj_consume_token(NEW_T);
    }
    if (!hasError) {
    
    s1 = name();
    }
    if (!hasError) {
    
    s2 = signature();
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case GENERIC_T:{if (!hasError) {
      
      gen_assoc_list();
      }
      
      break;
      }
    default:
      jj_la1[254] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
QCString q=" is new "+s1+s2;
      addVhdlType(s.data(),getLine(PACKAGE_T),Entry::VARIABLE_SEC,VhdlDocGen::INSTANTIATION,"package",q.data(),Public);
}


QCString VhdlParser::interface_package_declaration() {QCString s,s1;if (!hasError) {
    
    jj_consume_token(PACKAGE_T);
    }
    if (!hasError) {
    
    s = identifier();
    }
    if (!hasError) {
    
    jj_consume_token(IS_T);
    }
    if (!hasError) {
    
    jj_consume_token(NEW_T);
    }
    if (!hasError) {
    
    s1 = name();
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case GENERIC_T:{if (!hasError) {
      
      gen_assoc_list();
      }
      
      break;
      }
    default:
      jj_la1[255] = jj_gen;
      ;
    }
    }
    
current->name=s;
                            return "package "+s+" is new "+s1;
assert(false);
}


QCString VhdlParser::subprogram_instantiation_declaration() {QCString s,s1,s2;if (!hasError) {
    
    jj_consume_token(FUNCTION_T);
    }
    if (!hasError) {
    
    s = identifier();
    }
    if (!hasError) {
    
    jj_consume_token(IS_T);
    }
    if (!hasError) {
    
    jj_consume_token(NEW_T);
    }
    if (!hasError) {
    
    s1 = name();
    }
    if (!hasError) {
    
    s2 = signature();
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case GENERIC_T:{if (!hasError) {
      
      gen_assoc_list();
      }
      
      break;
      }
    default:
      jj_la1[256] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
QCString q= " is new "+s1+s2;
      addVhdlType(s.data(),getLine(FUNCTION_T),Entry::VARIABLE_SEC,VhdlDocGen::INSTANTIATION,"function ",q.data(),Public);
    return q;
assert(false);
}


void VhdlParser::gen_assoc_list() {if (!hasError) {
    
    jj_consume_token(GENERIC_T);
    }
    if (!hasError) {
    
    jj_consume_token(MAP_T);
    }
    if (!hasError) {
    
    jj_consume_token(LPAREN_T);
    }
    if (!hasError) {
    
    association_list();
    }
    if (!hasError) {
    
    jj_consume_token(RPAREN_T);
    }
    
}


void VhdlParser::gen_interface_list() {if (!hasError) {
    
    jj_consume_token(GENERIC_T);
    }
    if (!hasError) {
    
    jj_consume_token(LPAREN_T);
    }
    if (!hasError) {
    
//int u=s_str.iLine;
                          parse_sec=GEN_SEC;
    }
    if (!hasError) {
    
    interface_list();
    }
    if (!hasError) {
    
//  QCString vo=$3;
                          parse_sec=0;
    }
    if (!hasError) {
    
    jj_consume_token(RPAREN_T);
    }
    
}


void VhdlParser::case_scheme() {if (!hasError) {
    
    jj_consume_token(CASE_T);
    }
    if (!hasError) {
    
    expression();
    }
    if (!hasError) {
    
    jj_consume_token(GENERATE_T);
    }
    if (!hasError) {
    
    when_stats();
    }
    if (!hasError) {
    
    if (jj_2_106(3)) {if (!hasError) {
      
      ttend();
      }
      
    } else {
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(END_T);
    }
    if (!hasError) {
    
    jj_consume_token(GENERATE_T);
    }
    if (!hasError) {
    
    generate_statement_body();
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
}


void VhdlParser::when_stats() {if (!hasError) {
    
    while (!hasError) {if (!hasError) {
      
      jj_consume_token(WHEN_T);
      }
      if (!hasError) {
      
      if (jj_2_107(2)) {if (!hasError) {
        
        label();
        }
        if (!hasError) {
        
        jj_consume_token(COLON_T);
        }
        
      } else {
        ;
      }
      }
      if (!hasError) {
      
      choices();
      }
      if (!hasError) {
      
      jj_consume_token(ARROW_T);
      }
      if (!hasError) {
      
      generate_statement_body();
      }
      
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case WHEN_T:{
        ;
        break;
        }
      default:
        jj_la1[257] = jj_gen;
        goto end_label_50;
      }
    }
    end_label_50: ;
    }
    
}


void VhdlParser::ttend() {if (!hasError) {
    
    jj_consume_token(END_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      identifier();
      }
      
      break;
      }
    default:
      jj_la1[258] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
}


void VhdlParser::generate_statement_body() {if (!hasError) {
    
    jj_consume_token(BEGIN_T);
    }
    if (!hasError) {
    
    generate_statement_body();
    }
    
}


void VhdlParser::generate_statement_body1() {if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case ALIAS_T:
    case ATTRIBUTE_T:
    case BEGIN_T:
    case COMPONENT_T:
    case CONSTANT_T:
    case DISCONNECT_T:
    case FILE_T:
    case FOR_T:
    case FUNCTION_T:
    case GROUP_T:
    case IMPURE_T:
    case PROCEDURE_T:
    case PURE_T:
    case SIGNAL_T:
    case SHARED_T:
    case SUBTYPE_T:
    case TYPE_T:
    case USE_T:
    case VARIABLE_T:{if (!hasError) {
      
      while (!hasError) {
        if (jj_2_108(2147483647)) {
          ;
        } else {
          goto end_label_51;
        }if (!hasError) {
        
        block_declarative_item();
        }
        
      }
      end_label_51: ;
      }
      if (!hasError) {
      
      jj_consume_token(BEGIN_T);
      }
      
      break;
      }
    default:
      jj_la1[259] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ASSERT_T:
      case CASE_T:
      case POSTPONED_T:
      case PROCESS_T:
      case WITH_T:
      case LPAREN_T:
      case SLSL_T:
      case STRINGLITERAL:
      case BASIC_IDENTIFIER:
      case EXTENDED_CHARACTER:
      case VHDL2008TOOLDIR:{
        ;
        break;
        }
      default:
        jj_la1[260] = jj_gen;
        goto end_label_52;
      }if (!hasError) {
      
      concurrent_statement();
      }
      
    }
    end_label_52: ;
    }
    
}


QCString VhdlParser::external_name() {QCString s,s1,s2;if (!hasError) {
    
    jj_consume_token(SLSL_T);
    }
    if (!hasError) {
    
    s = sig_stat();
    }
    if (!hasError) {
    
    s1 = external_pathname();
    }
    if (!hasError) {
    
    jj_consume_token(COLON_T);
    }
    if (!hasError) {
    
    s2 = subtype_indication();
    }
    if (!hasError) {
    
    jj_consume_token(RSRS_T);
    }
    
QCString t="<<"+s;
                         QCString t1=s1+":"+s2+">>";
                         return   s+s1;
assert(false);
}


QCString VhdlParser::sig_stat() {Token *t;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case CONSTANT_T:{if (!hasError) {
      
      t = jj_consume_token(CONSTANT_T);
      }
      if (!hasError) {
      
return t->image.data();
      }
      
      break;
      }
    case SIGNAL_T:{if (!hasError) {
      
      t = jj_consume_token(SIGNAL_T);
      }
      if (!hasError) {
      
return t->image.data();
      }
      
      break;
      }
    case VARIABLE_T:{if (!hasError) {
      
      t = jj_consume_token(VARIABLE_T);
      }
      if (!hasError) {
      
return t->image.data();
      }
      
      break;
      }
    default:
      jj_la1[261] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


QCString VhdlParser::external_pathname() {QCString s;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case DOT_T:{if (!hasError) {
      
      s = absolute_pathname();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    case NEG_T:{if (!hasError) {
      
      s = relative_pathname();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    case AT_T:{if (!hasError) {
      
      s = package_path_name();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    default:
      jj_la1[262] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


QCString VhdlParser::absolute_pathname() {QCString s,s1;
    if (jj_2_109(2147483647)) {if (!hasError) {
      
      jj_consume_token(DOT_T);
      }
      if (!hasError) {
      
      s = pathname_element_list();
      }
      if (!hasError) {
      
      s1 = identifier();
      }
      if (!hasError) {
      
return "."+s+s1;
      }
      
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case DOT_T:{if (!hasError) {
        
        jj_consume_token(DOT_T);
        }
        if (!hasError) {
        
        s = identifier();
        }
        if (!hasError) {
        
return "."+s;
        }
        
        break;
        }
      default:
        jj_la1[263] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
assert(false);
return QCString();
}


QCString VhdlParser::relative_pathname() {QCString s,s1,s2;if (!hasError) {
    
    s = neg_list();
    }
    if (!hasError) {
    
    if (jj_2_110(2147483647)) {if (!hasError) {
      
      s1 = pathname_element_list();
      }
      
    } else {
      ;
    }
    }
    if (!hasError) {
    
    s2 = identifier();
    }
    
return s+s1+s2;
assert(false);
}


QCString VhdlParser::neg_list() {QCString s;if (!hasError) {
    
    while (!hasError) {if (!hasError) {
      
      jj_consume_token(NEG_T);
      }
      if (!hasError) {
      
      jj_consume_token(DOT_T);
      }
      if (!hasError) {
      
s+="^.";
      }
      
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case NEG_T:{
        ;
        break;
        }
      default:
        jj_la1[264] = jj_gen;
        goto end_label_53;
      }
    }
    end_label_53: ;
    }
    
return s;
assert(false);
}


QCString VhdlParser::pathname_element() {QCString s,s1;if (!hasError) {
    
    s = identifier();
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LPAREN_T:{if (!hasError) {
      
      jj_consume_token(LPAREN_T);
      }
      if (!hasError) {
      
      s1 = expression();
      }
      if (!hasError) {
      
      jj_consume_token(RPAREN_T);
      }
      
      break;
      }
    default:
      jj_la1[265] = jj_gen;
      ;
    }
    }
    
if(!s1.isEmpty())
     return s+"("+s1+")";

    return s;
assert(false);
}


QCString VhdlParser::pathname_element_list() {QCString s,s1,s2;if (!hasError) {
    if (!hasError) {
    
    s = pathname_element();
    }
    if (!hasError) {
    
    jj_consume_token(DOT_T);
    }
    
    }
    if (!hasError) {
    
s+=".";
    }
    if (!hasError) {
    
    while (!hasError) {
      if (jj_2_111(2147483647)) {
        ;
      } else {
        goto end_label_54;
      }if (!hasError) {
      
      s1 = pathname_element();
      }
      if (!hasError) {
      
      jj_consume_token(DOT_T);
      }
      if (!hasError) {
      
s2+=s1;s2+=".";
      }
      
    }
    end_label_54: ;
    }
    
return s+s2;
assert(false);
}


QCString VhdlParser::package_path_name() {QCString s;if (!hasError) {
    
    jj_consume_token(AT_T);
    }
    if (!hasError) {
    
    s = name();
    }
    
return "@"+s;
assert(false);
}


void VhdlParser::conditional_signal_assignment_wave() {
    if (jj_2_112(2147483647)) {if (!hasError) {
      
      conditional_force_assignment();
      }
      
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case LPAREN_T:
      case SLSL_T:
      case STRINGLITERAL:
      case BASIC_IDENTIFIER:
      case EXTENDED_CHARACTER:{if (!hasError) {
        
        conditional_waveform_assignment();
        }
        
        break;
        }
      default:
        jj_la1[266] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
}


void VhdlParser::conditional_waveform_assignment() {if (!hasError) {
    
    target();
    }
    if (!hasError) {
    
    jj_consume_token(LESSTHAN_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case INERTIAL_T:
    case REJECT_T:
    case TRANSPORT_T:{if (!hasError) {
      
      delay_mechanism();
      }
      
      break;
      }
    default:
      jj_la1[267] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    waveform_element();
    }
    if (!hasError) {
    
    jj_consume_token(WHEN_T);
    }
    if (!hasError) {
    
    expression();
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case ELSE_T:{if (!hasError) {
      
      else_wave_list();
      }
      
      break;
      }
    default:
      jj_la1[268] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
}


void VhdlParser::else_wave_list() {if (!hasError) {
    
    jj_consume_token(ELSE_T);
    }
    if (!hasError) {
    
    expression();
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case WHEN_T:{if (!hasError) {
      
      jj_consume_token(WHEN_T);
      }
      if (!hasError) {
      
      expression();
      }
      
      break;
      }
    default:
      jj_la1[269] = jj_gen;
      ;
    }
    }
    
}


void VhdlParser::conditional_force_assignment() {if (!hasError) {
    
    target();
    }
    if (!hasError) {
    
    jj_consume_token(LESSTHAN_T);
    }
    if (!hasError) {
    
    jj_consume_token(FORCE_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case IN_T:
    case OUT_T:{if (!hasError) {
      
      inout_stat();
      }
      
      break;
      }
    default:
      jj_la1[270] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    expression();
    }
    if (!hasError) {
    
    jj_consume_token(WHEN_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case ABS_T:
    case NEW_T:
    case NOT_T:
    case NULL_T:
    case LPAREN_T:
    case PLUS_T:
    case MINUS_T:
    case SLSL_T:
    case INTEGER:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:
    case CHARACTER_LITERAL:
    case DECIMAL_LITERAL:
    case BASED_LITERAL:
    case BIT_STRING_LITERAL:{if (!hasError) {
      
      expression();
      }
      if (!hasError) {
      
      else_stat();
      }
      
      break;
      }
    default:
      jj_la1[271] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
}


void VhdlParser::selected_signal_assignment_wave() {
    if (jj_2_113(2147483647)) {if (!hasError) {
      
      selected_force_assignment();
      }
      
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case WITH_T:{if (!hasError) {
        
        selected_waveform_assignment();
        }
        
        break;
        }
      default:
        jj_la1[272] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
}


void VhdlParser::selected_variable_assignment() {if (!hasError) {
    
    jj_consume_token(WITH_T);
    }
    if (!hasError) {
    
    expression();
    }
    if (!hasError) {
    
    jj_consume_token(SELECT_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case Q_T:{if (!hasError) {
      
      jj_consume_token(Q_T);
      }
      
      break;
      }
    default:
      jj_la1[273] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    select_name();
    }
    if (!hasError) {
    
    jj_consume_token(VARASSIGN_T);
    }
    if (!hasError) {
    
    sel_var_list();
    }
    
}


void VhdlParser::select_name() {
    if (jj_2_114(2147483647)) {if (!hasError) {
      
      aggregate();
      }
      
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case SLSL_T:
      case STRINGLITERAL:
      case BASIC_IDENTIFIER:
      case EXTENDED_CHARACTER:{if (!hasError) {
        
        name();
        }
        
        break;
        }
      default:
        jj_la1[274] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
}


void VhdlParser::selected_waveform_assignment() {if (!hasError) {
    
    jj_consume_token(WITH_T);
    }
    if (!hasError) {
    
    expression();
    }
    if (!hasError) {
    
    jj_consume_token(SELECT_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case Q_T:{if (!hasError) {
      
      jj_consume_token(Q_T);
      }
      
      break;
      }
    default:
      jj_la1[275] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    target();
    }
    if (!hasError) {
    
    jj_consume_token(LESSTHAN_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case INERTIAL_T:
    case REJECT_T:
    case TRANSPORT_T:{if (!hasError) {
      
      delay_mechanism();
      }
      
      break;
      }
    default:
      jj_la1[276] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    sel_wave_list();
    }
    
}


void VhdlParser::selected_force_assignment() {if (!hasError) {
    
    jj_consume_token(WITH_T);
    }
    if (!hasError) {
    
    expression();
    }
    if (!hasError) {
    
    jj_consume_token(SELECT_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case Q_T:{if (!hasError) {
      
      jj_consume_token(Q_T);
      }
      
      break;
      }
    default:
      jj_la1[277] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    target();
    }
    if (!hasError) {
    
    jj_consume_token(LESSTHAN_T);
    }
    if (!hasError) {
    
    jj_consume_token(FORCE_T);
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case IN_T:
    case OUT_T:{if (!hasError) {
      
      inout_stat();
      }
      
      break;
      }
    default:
      jj_la1[278] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    sel_var_list();
    }
    
}


void VhdlParser::sel_var_list() {if (!hasError) {
    if (!hasError) {
    
    expression();
    }
    if (!hasError) {
    
    jj_consume_token(WHEN_T);
    }
    if (!hasError) {
    
    choices();
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case COMMA_T:{if (!hasError) {
      
      jj_consume_token(COMMA_T);
      }
      
      break;
      }
    case SEMI_T:{if (!hasError) {
      
      jj_consume_token(SEMI_T);
      }
      
      break;
      }
    default:
      jj_la1[279] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
    }
    
    }
    if (!hasError) {
    
    while (!hasError) {
      if (jj_2_115(2147483647)) {
        ;
      } else {
        goto end_label_55;
      }if (!hasError) {
      
      expression();
      }
      if (!hasError) {
      
      jj_consume_token(WHEN_T);
      }
      if (!hasError) {
      
      choices();
      }
      if (!hasError) {
      
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case COMMA_T:{if (!hasError) {
        
        jj_consume_token(COMMA_T);
        }
        
        break;
        }
      case SEMI_T:{if (!hasError) {
        
        jj_consume_token(SEMI_T);
        }
        
        break;
        }
      default:
        jj_la1[280] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
      }
      
    }
    end_label_55: ;
    }
    
}


void VhdlParser::sel_wave_list() {if (!hasError) {
    
    waveform_element();
    }
    if (!hasError) {
    
    jj_consume_token(WHEN_T);
    }
    if (!hasError) {
    
    choices();
    }
    if (!hasError) {
    
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case COMMA_T:{
        ;
        break;
        }
      default:
        jj_la1[281] = jj_gen;
        goto end_label_56;
      }if (!hasError) {
      
      jj_consume_token(COMMA_T);
      }
      if (!hasError) {
      
      sel_wave_list();
      }
      
    }
    end_label_56: ;
    }
    if (!hasError) {
    
    jj_consume_token(SEMI_T);
    }
    
}


void VhdlParser::inout_stat() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case IN_T:{if (!hasError) {
      
      jj_consume_token(IN_T);
      }
      
      break;
      }
    case OUT_T:{if (!hasError) {
      
      jj_consume_token(OUT_T);
      }
      
      break;
      }
    default:
      jj_la1[282] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
}


void VhdlParser::else_stat() {if (!hasError) {
    
    while (!hasError) {if (!hasError) {
      
      jj_consume_token(ELSE_T);
      }
      if (!hasError) {
      
      expression();
      }
      if (!hasError) {
      
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case WHEN_T:{if (!hasError) {
        
        jj_consume_token(WHEN_T);
        }
        if (!hasError) {
        
        expression();
        }
        
        break;
        }
      default:
        jj_la1[283] = jj_gen;
        ;
      }
      }
      
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ELSE_T:{
        ;
        break;
        }
      default:
        jj_la1[284] = jj_gen;
        goto end_label_57;
      }
    }
    end_label_57: ;
    }
    
}


QCString VhdlParser::interface_subprogram_declaration() {QCString s;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case PROCEDURE_T:{if (!hasError) {
      
      s = iproc();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    case FUNCTION_T:
    case IMPURE_T:
    case PURE_T:{if (!hasError) {
      
      s = ifunc();
      }
      if (!hasError) {
      
return s;
      }
      
      break;
      }
    default:
      jj_la1[285] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
return QCString();
}


QCString VhdlParser::iproc() {QCString s,s1;if (!hasError) {
    
    jj_consume_token(PROCEDURE_T);
    }
    if (!hasError) {
    
    s = identifier();
    }
    if (!hasError) {
    
    s1 = param();
    }
    
current->name=s;
  return "procedure "+s+s1;
assert(false);
}


QCString VhdlParser::ifunc() {QCString s,s1,s2,s3;Token *t=0;Token *t1=0;Token *t2=0;if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case IMPURE_T:
    case PURE_T:{
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case PURE_T:{if (!hasError) {
        
        t = jj_consume_token(PURE_T);
        }
        
        break;
        }
      case IMPURE_T:{if (!hasError) {
        
        t = jj_consume_token(IMPURE_T);
        }
        
        break;
        }
      default:
        jj_la1[286] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
      break;
      }
    default:
      jj_la1[287] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
    jj_consume_token(FUNCTION_T);
    }
    if (!hasError) {
    
    s = name();
    }
    if (!hasError) {
    
    s1 = param();
    }
    if (!hasError) {
    
    jj_consume_token(RETURN_T);
    }
    if (!hasError) {
    
    s2 = name();
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case IS_T:{if (!hasError) {
      
      t1 = jj_consume_token(IS_T);
      }
      if (!hasError) {
      
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case BASIC_IDENTIFIER:
      case EXTENDED_CHARACTER:{if (!hasError) {
        
        s3 = identifier();
        }
        
        break;
        }
      case BOX_T:{if (!hasError) {
        
        t2 = jj_consume_token(BOX_T);
        }
        
        break;
        }
      default:
        jj_la1[288] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
      }
      
      break;
      }
    default:
      jj_la1[289] = jj_gen;
      ;
    }
    }
    
QCString q;
      if(t) q=t->image.data();
      if(t2) s3="<>";
      if (!s3.isEmpty())
      {
        s3.prepend(" is ");
      }
     current->name=s;
    if (parse_sec==GEN_SEC)
                    {
                      QCString ss=q+" function "+s1+" return "+s2+s3;
                      int a=getLine(FUNCTION_T);
                      int b=getLine(PROCEDURE_T);

                      if (a>b) b=a;
                      addVhdlType(current->name.data(),b,Entry::VARIABLE_SEC,VhdlDocGen::GENERIC,ss.data(),0,Public);
                    }
         currP=0;return "";
assert(false);
}


QCString VhdlParser::param() {QCString s,s1;Token *tok=0;if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case PARAMETER_T:{if (!hasError) {
      
      tok = jj_consume_token(PARAMETER_T);
      }
      
      break;
      }
    default:
      jj_la1[290] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    
param_sec=PARAM_SEC;
    }
    if (!hasError) {
    
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LPAREN_T:{if (!hasError) {
      
      jj_consume_token(LPAREN_T);
      }
      if (!hasError) {
      
      s1 = interface_list();
      }
      if (!hasError) {
      
      jj_consume_token(RPAREN_T);
      }
      
      break;
      }
    default:
      jj_la1[291] = jj_gen;
      ;
    }
    }
    
if(tok)
                     {
                      s = tok->image.data();
                         param_sec=0;
                    }
                      return s+"("+s1+")";
assert(false);
}


void VhdlParser::parseInline() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case POSTPONED_T:
    case PROCESS_T:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{if (!hasError) {
      
      process_statement();
      }
      
      break;
      }
    case FUNCTION_T:
    case IMPURE_T:
    case PROCEDURE_T:
    case PURE_T:{if (!hasError) {
      
      subprogram_declaration();
      }
      
      break;
      }
    default:
      jj_la1[292] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
}


 VhdlParser::VhdlParser(TokenManager *tm){
    head = NULL;
    errorHandlerCreated = false;
    ReInit(tm);
}
VhdlParser::~VhdlParser()
{
  clear();
}

void VhdlParser::ReInit(TokenManager *tm){
    clear();
    errorHandler = new ErrorHandler();
    errorHandlerCreated = true;
    hasError = false;
    token_source = tm;
    head = token = new Token();
    token->kind = 0;
    token->next = NULL;
    jj_lookingAhead = false;
    jj_rescan = false;
    jj_done = false;
    jj_scanpos = jj_lastpos = NULL;
    jj_gc = 0;
    jj_kind = -1;
    trace_indent = 0;
    trace_enabled = false;
    jj_ntk = -1;
    jj_gen = 0;
    for (int i = 0; i < 293; i++) jj_la1[i] = -1;
  }


void VhdlParser::clear(){
  //Since token manager was generate from outside,
  //parser should not take care of deleting
  //if (token_source) delete token_source;
  if (head) {
    Token *next, *t = head;
    while (t) {
      next = t->next;
      delete t;
      t = next;
    }
  }
  if (errorHandlerCreated) {
    delete errorHandler;
  }
}


Token * VhdlParser::jj_consume_token(int kind)  {
    Token *oldToken;
    if ((oldToken = token)->next != NULL) token = token->next;
    else token = token->next = token_source->getNextToken();
    jj_ntk = -1;
    if (token->kind == kind) {
      jj_gen++;
      if (++jj_gc > 100) {
        jj_gc = 0;
        for (int i = 0; i < 115; i++) {
          JJCalls *c = &jj_2_rtns[i];
          while (c != NULL) {
            if (c->gen < jj_gen) c->first = NULL;
            c = c->next;
          }
        }
      }
      return token;
    }
    token = oldToken;
    jj_kind = kind;
    JAVACC_STRING_TYPE image = kind >= 0 ? tokenImage[kind] : tokenImage[0];
    errorHandler->handleUnexpectedToken(kind, image.substr(1, image.size() - 2), getToken(1), this);
    hasError = true;
    return token;
  }


bool  VhdlParser::jj_scan_token(int kind){
    if (jj_scanpos == jj_lastpos) {
      jj_la--;
      if (jj_scanpos->next == NULL) {
        jj_lastpos = jj_scanpos = jj_scanpos->next = token_source->getNextToken();
      } else {
        jj_lastpos = jj_scanpos = jj_scanpos->next;
      }
    } else {
      jj_scanpos = jj_scanpos->next;
    }
    if (jj_rescan) {
      int i = 0; Token *tok = token;
      while (tok != NULL && tok != jj_scanpos) { i++; tok = tok->next; }
      if (tok != NULL) jj_add_error_token(kind, i);
    }
    if (jj_scanpos->kind != kind) return true;
    if (jj_la == 0 && jj_scanpos == jj_lastpos) { return jj_done = true; }
    return false;
  }


/** Get the next Token. */

Token * VhdlParser::getNextToken(){
    if (token->next != NULL) token = token->next;
    else token = token->next = token_source->getNextToken();
    jj_ntk = -1;
    jj_gen++;
    return token;
  }

/** Get the specific Token. */

Token * VhdlParser::getToken(int index){
    Token *t = token;
    for (int i = 0; i < index; i++) {
      if (t->next != NULL) t = t->next;
      else t = t->next = token_source->getNextToken();
    }
    return t;
  }


int VhdlParser::jj_ntk_f(){
    if ((jj_nt=token->next) == NULL)
      return (jj_ntk = (token->next=token_source->getNextToken())->kind);
    else
      return (jj_ntk = jj_nt->kind);
  }


  void VhdlParser::jj_add_error_token(int kind, int pos)  {
  }


 void  VhdlParser::parseError()   {
      fprintf(stderr, "Parse error at: %d:%d, after token: %s encountered: %s\n", token->beginLine, token->beginColumn, addUnicodeEscapes(token->image).c_str(), addUnicodeEscapes(getToken(1)->image).c_str());
   }


  void VhdlParser::enable_tracing()  {
  }

  void VhdlParser::disable_tracing()  {
  }


  void VhdlParser::jj_rescan_token(){
    jj_rescan = true;
    for (int i = 0; i < 115; i++) {
      JJCalls *p = &jj_2_rtns[i];
      do {
        if (p->gen > jj_gen) {
          jj_la = p->arg; jj_lastpos = jj_scanpos = p->first;
          switch (i) {
            case 0: jj_3_1(); break;
            case 1: jj_3_2(); break;
            case 2: jj_3_3(); break;
            case 3: jj_3_4(); break;
            case 4: jj_3_5(); break;
            case 5: jj_3_6(); break;
            case 6: jj_3_7(); break;
            case 7: jj_3_8(); break;
            case 8: jj_3_9(); break;
            case 9: jj_3_10(); break;
            case 10: jj_3_11(); break;
            case 11: jj_3_12(); break;
            case 12: jj_3_13(); break;
            case 13: jj_3_14(); break;
            case 14: jj_3_15(); break;
            case 15: jj_3_16(); break;
            case 16: jj_3_17(); break;
            case 17: jj_3_18(); break;
            case 18: jj_3_19(); break;
            case 19: jj_3_20(); break;
            case 20: jj_3_21(); break;
            case 21: jj_3_22(); break;
            case 22: jj_3_23(); break;
            case 23: jj_3_24(); break;
            case 24: jj_3_25(); break;
            case 25: jj_3_26(); break;
            case 26: jj_3_27(); break;
            case 27: jj_3_28(); break;
            case 28: jj_3_29(); break;
            case 29: jj_3_30(); break;
            case 30: jj_3_31(); break;
            case 31: jj_3_32(); break;
            case 32: jj_3_33(); break;
            case 33: jj_3_34(); break;
            case 34: jj_3_35(); break;
            case 35: jj_3_36(); break;
            case 36: jj_3_37(); break;
            case 37: jj_3_38(); break;
            case 38: jj_3_39(); break;
            case 39: jj_3_40(); break;
            case 40: jj_3_41(); break;
            case 41: jj_3_42(); break;
            case 42: jj_3_43(); break;
            case 43: jj_3_44(); break;
            case 44: jj_3_45(); break;
            case 45: jj_3_46(); break;
            case 46: jj_3_47(); break;
            case 47: jj_3_48(); break;
            case 48: jj_3_49(); break;
            case 49: jj_3_50(); break;
            case 50: jj_3_51(); break;
            case 51: jj_3_52(); break;
            case 52: jj_3_53(); break;
            case 53: jj_3_54(); break;
            case 54: jj_3_55(); break;
            case 55: jj_3_56(); break;
            case 56: jj_3_57(); break;
            case 57: jj_3_58(); break;
            case 58: jj_3_59(); break;
            case 59: jj_3_60(); break;
            case 60: jj_3_61(); break;
            case 61: jj_3_62(); break;
            case 62: jj_3_63(); break;
            case 63: jj_3_64(); break;
            case 64: jj_3_65(); break;
            case 65: jj_3_66(); break;
            case 66: jj_3_67(); break;
            case 67: jj_3_68(); break;
            case 68: jj_3_69(); break;
            case 69: jj_3_70(); break;
            case 70: jj_3_71(); break;
            case 71: jj_3_72(); break;
            case 72: jj_3_73(); break;
            case 73: jj_3_74(); break;
            case 74: jj_3_75(); break;
            case 75: jj_3_76(); break;
            case 76: jj_3_77(); break;
            case 77: jj_3_78(); break;
            case 78: jj_3_79(); break;
            case 79: jj_3_80(); break;
            case 80: jj_3_81(); break;
            case 81: jj_3_82(); break;
            case 82: jj_3_83(); break;
            case 83: jj_3_84(); break;
            case 84: jj_3_85(); break;
            case 85: jj_3_86(); break;
            case 86: jj_3_87(); break;
            case 87: jj_3_88(); break;
            case 88: jj_3_89(); break;
            case 89: jj_3_90(); break;
            case 90: jj_3_91(); break;
            case 91: jj_3_92(); break;
            case 92: jj_3_93(); break;
            case 93: jj_3_94(); break;
            case 94: jj_3_95(); break;
            case 95: jj_3_96(); break;
            case 96: jj_3_97(); break;
            case 97: jj_3_98(); break;
            case 98: jj_3_99(); break;
            case 99: jj_3_100(); break;
            case 100: jj_3_101(); break;
            case 101: jj_3_102(); break;
            case 102: jj_3_103(); break;
            case 103: jj_3_104(); break;
            case 104: jj_3_105(); break;
            case 105: jj_3_106(); break;
            case 106: jj_3_107(); break;
            case 107: jj_3_108(); break;
            case 108: jj_3_109(); break;
            case 109: jj_3_110(); break;
            case 110: jj_3_111(); break;
            case 111: jj_3_112(); break;
            case 112: jj_3_113(); break;
            case 113: jj_3_114(); break;
            case 114: jj_3_115(); break;
          }
        }
        p = p->next;
      } while (p != NULL);
    }
    jj_rescan = false;
  }


  void VhdlParser::jj_save(int index, int xla){
    JJCalls *p = &jj_2_rtns[index];
    while (p->gen > jj_gen) {
      if (p->next == NULL) { p = p->next = new JJCalls(); break; }
      p = p->next;
    }
    p->gen = jj_gen + xla - jj_la; p->first = token; p->arg = xla;
  }


}
}
