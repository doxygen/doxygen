/* VhdlParser.cc */
#include "VhdlParser.h"
#include "TokenMgrError.h"
namespace vhdl {
namespace parser {
  unsigned int jj_la1_0[] = {
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x20000,0x0,0x90404000,0x80000,0x0,0x0,0x0,0x0,0x0,0x40000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x80004000,0x10000000,0x400000,0x0,0x90404000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x80000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x10000000,0x0,0x0,0x40000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x10800,0x0,0x0,0x0,0x0,0x0,0x40000000,0x0,0x400000,0x400000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x40020000,0x40020001,0x0,0x0,0x0,0x0,0x4000000,0x0,0x0,0x40000000,0xd0020000,0x0,0x0,0x800000,0x0,0x0,0x80000000,0x4000,0x400000,0x0,0x0,0x80404000,0x0,0x0,0x0,0x0,0x8000,0x0,0x80000,0x0,0x0,0x0,0x0,0x0,0x0,0x10000,0x10800,0x0,0x80000000,0x10000,0x0,0x10800,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x90c04000,0x80000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x41000,0x0,0x0,0x10800,0x10000000,0x0,0x50000000,0x8000,0x80000000,0x0,0x80000000,0x0,0x0,0x0,0x0,0x0,0x0,0x8000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x80000000,0x80000000,0x4000000,0x8000000,0x0,0x0,0x20000,0x0,0x0,0x0,0x0,0x800,0x4000000,0x4000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x80000000,0x0,0x0,0x0,0x0,0x80000000,0x4000,0x0,0x80004000,0x0,0x0,0x0,0x0,0x0,0x90004000,0x400000,0x0,0x0,0x90404000,0x0,0x0,0x0,0x0,0x40000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x80004000,0x400000,0x0,0x80404000,0x0,0x0,0x0,0x0,0x0,0x0,0x21080000,0x0,0x0,0x80004000,0x400000,0x0,0x80404000,0x0,0x0,0x0,0x400000,0x404000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x20000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x8000,0x0,0x0,0x0,0x1000000,0x0,0x0,0x0,0x1000000,0x0,0x1000000,0x21080000,0x80c04000,0x80404000,0x0,0x0,0x0,0x0,0x0,0x8000000,0x0,0x8000000,0x0,0x8000,0x4000000,0x8000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x80004000,0x400000,0x0,0x80404000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x21080000,0x0,0x0,0x0,0x8000,0x0,0x0,0x41000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x10800,0x2000,0x0,};
  unsigned int jj_la1_1[] = {
0x0,0x0,0x400000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x125808,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x10000,0x0,0x0,0x1000,0x104800,0x0,0x1008,0x20000,0x125808,0x10000,0x0,0x0,0x0,0x0,0x1000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x10000,0x1000,0x1000000,0x10000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x40000,0x400000,0x40000,0x400000,0x0,0x40000,0x400000,0x0,0x0,0x400000,0x20,0x0,0x200000,0x200000,0x0,0x0,0x20,0x0,0x0,0x0,0x0,0x0,0x20000,0x20000,0x1000,0x0,0x4000001,0x0,0x1,0x0,0x0,0x400000,0x4000101,0x4000101,0x0,0x10,0x0,0x0,0x8a00000,0x0,0x0,0x100,0x12024900,0x0,0x0,0x0,0x100,0x0,0x104000,0x800,0x8,0x20000,0x0,0x124808,0x0,0x10000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1000000,0x0,0x200000,0x200000,0x0,0x0,0x10000,0x10000,0x125808,0x0,0x20,0x40,0x0,0x81000,0x0,0x0,0x0,0x0,0x0,0x40,0x20,0x0,0x0,0x0,0x800,0x0,0x0,0x0,0x0,0x0,0x100,0x0,0x800,0x800,0x0,0x200000,0x0,0x10000,0x0,0x0,0x0,0x0,0x0,0x104000,0x100000,0x100000,0x0,0x1000000,0x0,0x0,0x0,0x104000,0x100000,0x100000,0x10000,0x0,0x0,0x0,0x0,0x0,0x8a00000,0x0,0x0,0x1000,0x0,0x0,0x0,0x1000,0x0,0x0,0x8a00000,0x8a00000,0x0,0x0,0x0,0x80000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x800,0x40000,0x400000,0x0,0x0,0x104000,0x800,0x20000,0x124800,0x10000,0x10000,0x0,0x0,0x104000,0x800,0x8,0x20000,0x0,0x124808,0x10000,0x0,0x0,0x0,0x100,0x1,0x0,0x0,0x0,0x10000,0x0,0x0,0x1104000,0x800,0x0,0x20000,0x1124800,0x0,0x0,0x0,0x1000000,0x0,0x0,0x20081200,0x0,0x1104000,0x800,0x0,0x20000,0x1124800,0x0,0x10000,0x10000,0x4000,0x104000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x400000,0x0,0x200000,0x200000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1000000,0x0,0x0,0x0,0x1000000,0x0,0x0,0x0,0x20081200,0x1124800,0x1124800,0x0,0x0,0x0,0x400000,0x0,0x0,0x0,0x0,0x0,0x0,0x8a00000,0x0,0x0,0x0,0x0,0x0,0x0,0x4000,0x0,0x104000,0x1000000,0x1104800,0x0,0x20000,0x1124800,0x4000,0x0,0x100000,0x100000,0x10000,0x0,0x0,0x104000,0x20081200,0x0,0x10000,0x0,0x0,0x0,0x0,0x800,0x0,0x10000,0x0,0x0,0x0,0x0,0x10000,0x0,0x0,0x0,0x0,0x1000,0x0,0x0,0x0,0x104000,};
  unsigned int jj_la1_2[] = {
0x0,0x0,0x0,0x100,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2,0x0,0x0,0x221000,0x48000,0x0,0x0,0x0,0x0,0x0,0x0,0x40000000,0x0,0x0,0x0,0x400000,0x400000,0x0,0x0,0x0,0x4000,0x0,0x0,0x220000,0x1000,0x0,0x0,0x221000,0x0,0x4000,0x4000,0x0,0x0,0x0,0x0,0x48000,0x0,0x0,0x0,0x0,0x0,0x0,0x400,0x0,0x4000,0x0,0x0,0x0,0x4000,0x0,0x0,0x4000,0x800000,0x8000,0x8000,0x8000,0x0,0x0,0x2000000,0x0,0x2000000,0x0,0x0,0x2000000,0x0,0x0,0x2000000,0x0,0x0,0x800,0x800,0x23b,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2000000,0x2000000,0x1000,0x1000,0x0,0x0,0x0,0x0,0x800,0x0,0x0,0x100,0x21000,0x0,0x0,0x0,0x0,0x0,0x220000,0x0,0x0,0x0,0x1000,0x221000,0x0,0x0,0x4000,0x0,0x400,0x8000,0x48000,0x0,0x0,0x0,0x0,0x0,0x0,0x209,0x23b,0x0,0x0,0x209,0x0,0x23b,0x100,0x100,0x800,0x800,0x0,0x0,0x0,0x0,0x221000,0x48000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x10000,0x400000,0x0,0x0,0x23b,0x0,0x0,0x0,0x400,0x0,0x0,0x0,0x0,0x0,0x0,0x2000,0x2000,0x0,0x0,0x0,0x220000,0x200000,0x200000,0x0,0x0,0x2000,0x0,0x0,0x220000,0x200000,0x200000,0x0,0x2000,0x2000,0x0,0x0,0x0,0x800,0x0,0x0,0x0,0x1000,0x20,0x0,0x0,0x0,0x10,0x800,0x800,0x0,0x0,0x0,0x20000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2000000,0x1000,0x0,0x220000,0x1000,0x0,0x221000,0x0,0x0,0x1000,0x0,0x220000,0x0,0x0,0x0,0x1000,0x221000,0x0,0x0,0x0,0x0,0x0,0x1000,0x2000,0x2000,0x0,0x0,0x2000,0x2000,0x220000,0x1000,0x0,0x0,0x221000,0x0,0x8000,0x0,0x0,0x8000,0x0,0x40000024,0x0,0x220000,0x1000,0x0,0x0,0x221000,0x0,0x0,0x0,0x10000,0x230000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x400000,0x1000,0x0,0x0,0x0,0x0,0x0,0x2000000,0x0,0x800,0x800,0x0,0x0,0x0,0x0,0x0,0x20,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x40000024,0x221000,0x221000,0x80000000,0x80000000,0x0,0x2000000,0x0,0x1000000,0x0,0x1000000,0x0,0x400,0x800,0x0,0x0,0x0,0x0,0x0,0x0,0x20000,0x0,0x220000,0x0,0x220000,0x0,0x0,0x220000,0x20000,0x0,0x200000,0x200000,0x0,0x2000,0x2000,0x220000,0x40000024,0x0,0x0,0x0,0x0,0x0,0x0,0xc00002,0x100000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x80,0x0,0x0,0x0,0x23b,0x0,0x268000,};
  unsigned int jj_la1_3[] = {
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x711060,0x20000000,0x0,0x0,0x0,0x10000,0x0,0x0,0x0,0x10,0x0,0x0,0x0,0x0,0x0,0x100000,0x0,0x0,0x100000,0x0,0x611060,0x0,0x100000,0x0,0x711060,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x20000000,0x0,0x8000000,0x0,0x0,0x0,0x8000000,0x0,0x0,0x100000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x8000,0x0,0x8000,0x0,0x0,0x8000,0x0,0x0,0x8000,0x0,0x8000000,0x0,0x0,0xc0000000,0x8000000,0x0,0x8000000,0x8000000,0x8000000,0x0,0x0,0x100000,0x100000,0x0,0x0,0x100000,0x0,0x0,0x0,0x0,0x8000,0x100000,0x100000,0x0,0x4000,0x0,0x0,0x400000,0x400000,0x0,0x0,0x251020,0x0,0x0,0x0,0x0,0x0,0x11020,0x600040,0x100000,0x0,0x0,0x711060,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x8000000,0xc0000000,0xc0020000,0x0,0x200020,0xc0000000,0x0,0xc0000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x711060,0x20000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x10000,0x40000,0x0,0x10000,0x0,0xc0000000,0x0,0x0,0x0,0x0,0x210060,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x20,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x410000,0x200060,0x200060,0x0,0x0,0x0,0x10000000,0x0,0x0,0x0,0x10000000,0x0,0x0,0x0,0x400000,0x0,0x0,0x400000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x8000000,0x0,0x0,0x210060,0x0,0x8000,0x0,0x0,0x211040,0x500000,0x0,0x711040,0x0,0x0,0x0,0x0,0x11000,0x600060,0x100000,0x0,0x0,0x711060,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x211040,0x100000,0x0,0x311040,0x0,0x0,0x0,0x0,0x0,0x0,0x34000002,0x0,0x0,0x211040,0x100000,0x0,0x311040,0x0,0x0,0x0,0x100000,0x100000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x10,0x0,0x2,0x0,0x8000000,0x0,0x40000,0x0,0x0,0x0,0x20000000,0x0,0x0,0x0,0x8000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x34000002,0x311040,0x311040,0x781,0x781,0x0,0x8000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2,0x0,0x0,0x0,0x0,0x0,0x211040,0x100000,0x0,0x311040,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x34000002,0x0,0x0,0x0,0x0,0x0,0x10000,0x0,0x0,0x0,0x0,0x0,0x20000000,0x40,0x0,0x0,0x0,0x0,0x80000,0x0,0x0,0xc0020000,0x0,0x0,};
  unsigned int jj_la1_4[] = {
0x0,0x0,0x0,0x0,0x0,0x100000,0xc1,0x100,0x400,0x0,0x0,0x0,0x0,0x200000,0x0,0x200004,0x4,0x4,0x4,0x200004,0x100,0x0,0x0,0x0,0x0,0x100,0x0,0x200000,0x4,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x4,0x4,0x0,0x0,0x200004,0x40000000,0x0,0x40000000,0x0,0x0,0x0,0x0,0x80000000,0x800,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x200004,0x0,0x0,0x0,0x0,0x40000000,0x0,0x0,0x0,0x200004,0x0,0x0,0x0,0x0,0x0,0xa000c4,0x0,0x0,0x0,0x0,0x0,0x0,0x200000,0x0,0x0,0x0,0x200,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x4,0x0,0x0,0x0,0x4,0x0,0x0,0x100000,0x100,0x0,0x0,0x200000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x100,0x200000,0x200000,0x200000,0x200000,0x0,0x100,0x0,0x0,0x0,0x0,0xa000c4,0x0,0x0,0x0,0x10,0xa00004,0x0,0x0,0x0,0x0,0x200000,0x4,0x0,0x0,0x0,0x200004,0x0,0x0,0x0,0x0,0x200000,0x100,0x0,0x100,0x0,0x0,0x0,0x0,0x200004,0x100004,0x0,0x200000,0x100,0xa000c4,0x0,0x4,0x200000,0x200000,0x0,0x0,0x0,0x0,0x200,0x0,0x0,0x4,0x0,0x0,0x200,0x0,0x0,0x0,0x100000,0x0,0x0,0x4,0x300000,0x0,0x0,0x0,0x0,0x0,0x4,0x200004,0x0,0x0,0x0,0x0,0x200,0x0,0x0,0x0,0x0,0x0,0x0,0x10,0x0,0x200004,0x0,0x0,0x0,0x20,0x200000,0x100,0x4,0x2,0x100,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x200000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x200000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x4,0x0,0x200000,0x0,0x0,0x0,0x0,0x4,0x0,0x0,0x4,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x4,0x0,0x0,0x0,0x200004,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x4,0x0,0x200000,0x200000,0x100,0x100,0x100,0x0,0x3f07e000,0x0,0x3f07e000,0x0,0x0,0x4,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x100,0x0,0x40000000,0x200000,0x40000000,0x0,0x40000000,0x0,0x0,0x900,0x900,0x100,0x100,0x200000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x200004,0x0,0x0,0x0,0x0,0xc0,0x0,0x200004,0x0,0x200,0x0,0x100,0x200000,0x0,0x0,0x200,0x100,0x200000,0x0,0xc0,0x0,0x0,0x0,0x800,0x0,0x0,0x0,0x0,0x0,0x4,0x0,0x0,0x0,0x0,0x4,0x0,0x200004,0x0,0x0,0x4,0x0,0x200004,0x0,0x4,0x0,0x0,0x100,0x100,0x200004,0x0,0x0,0x200,0x0,0x0,0x0,0x0,0x100,0xa000c4,0x0,0x0,};
  unsigned int jj_la1_5[] = {
0x1,0xa100,0x0,0x0,0xc00,0x0,0x0,0x0,0x0,0x10,0x1e00,0x0,0x0,0xe00,0x0,0xe00,0x0,0x0,0x0,0xe00,0x0,0x0,0x0,0x0,0xc00,0x0,0xc00,0xe00,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xc00,0xe00,0x0,0x0,0x0,0xc00,0xc00,0x0,0xc00,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xc00,0x0,0x0,0x0,0x0,0x0,0xe00,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xe00,0x0,0x0,0x0,0x0,0x0,0x80bf00,0x0,0x0,0x0,0x0,0x0,0x0,0xe00,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0xc00,0x0,0x0,0x0,0x0,0xe00,0x0,0x0,0xc00,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xe00,0x0,0x0,0x0,0x0,0x0,0x0,0x10,0x0,0x0,0x0,0x1e00,0xe00,0xe00,0x1e00,0x1c00,0x0,0xc00,0xc00,0x0,0x0,0x80bf00,0xd,0x0,0x0,0x0,0x80bf00,0x0,0x0,0x0,0x0,0xf00,0x0,0x0,0x0,0x0,0xe00,0x0,0x0,0xc00,0x0,0x1e00,0x0,0xc00,0x0,0xc00,0x0,0x0,0xc00,0xe00,0x0,0x0,0xe00,0x0,0x80bf00,0x0,0x0,0xe00,0xe00,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xc00,0x0,0x0,0x0,0xe00,0x0,0x0,0x0,0x0,0x0,0x0,0xe00,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x200,0xc00,0x0,0xc00,0x0,0x0,0xe00,0xc00,0xc00,0x0,0x2,0xe00,0x0,0x0,0x10,0x0,0xc00,0xc00,0x0,0xc00,0xa100,0x0,0x0,0x0,0x0,0xe00,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xe00,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xc00,0xe00,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xc00,0x0,0x0,0x0,0x0,0xc00,0xe00,0xc00,0x0,0x0,0x0,0x0,0x0,0xc00,0x0,0x0,0x0,0x0,0x0,0xc00,0xe00,0xe00,0x0,0x0,0x0,0x0,0x0,0x8,0x0,0xc00,0x0,0xc00,0xc00,0xc00,0x0,0xc00,0x0,0x0,0x0,0x0,0x0,0x0,0xe00,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xe00,0xc00,0xc00,0x0,0x0,0xc00,0xc00,0x0,0x0,0xc00,0xc00,0xe00,0x0,0x0,0x0,0x0,0x0,0x0,0xe00,0x0,0x0,0x0,0x0,0xe00,0x0,0x0,0x0,0x0,0xe00,0x0,0x0,0x0,0xe00,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xe00,0x10,0x0,0x0,0x1200,0xe00,0x0,0x0,0x0,0x0,0x0,0x0,0xe00,0x0,0x0,0x0,0xc00,0x0,0x0,0x0,0x0,0x80bf00,0x0,0xc00,};
  unsigned int jj_la1_6[] = {
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,};

  /** Constructor with user supplied TokenManager. */




QCString VhdlParser::absolute_pathname() {QCString s,s1;
    if (jj_2_1(2147483647)) {
      if (!hasError) {
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
      case DOT_T:{
        if (!hasError) {
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
        jj_la1[0] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
assert(false);
}


QCString VhdlParser::abstract_literal() {Token *tok;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case DECIMAL_LITERAL:{
      if (!hasError) {
      tok = jj_consume_token(DECIMAL_LITERAL);
      }
      if (!hasError) {
return QCString(tok->image);
      }
      break;
      }
    case INTEGER:{
      if (!hasError) {
      tok = jj_consume_token(INTEGER);
      }
      if (!hasError) {
return QCString(tok->image);
      }
      break;
      }
    case BASED_LITERAL:{
      if (!hasError) {
      tok = jj_consume_token(BASED_LITERAL);
      }
      if (!hasError) {
return QCString(tok->image);
      }
      break;
      }
    default:
      jj_la1[1] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::access_type_definition() {Token *tok=nullptr;QCString str,str1;
    if (!hasError) {
    tok = jj_consume_token(ACCESS_T);
    }
    if (!hasError) {
    str1 = subtype_indication();
    }
str=tok->image; return str+str1;
assert(false);
}


QCString VhdlParser::access_incomplete_type_definition() {Token *tok=nullptr;QCString str,str1;
    if (!hasError) {
    tok = jj_consume_token(ACCESS_T);
    }
    if (!hasError) {
    str1 = incomplete_subtype_indication();
    }
str=tok->image; return str+" "+str1;
assert(false);
}


QCString VhdlParser::actual_designator() {QCString str,s;Token *t=nullptr;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case OPEN_T:{
      if (!hasError) {
      t = jj_consume_token(OPEN_T);
      }
      if (!hasError) {
return QCString(t->image);
      }
      break;
      }
    default:
      jj_la1[3] = jj_gen;
      if (jj_2_2(2147483647)) {
        if (!hasError) {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case INERTIAL_T:{
          if (!hasError) {
          t = jj_consume_token(INERTIAL_T);
          }
          if (!hasError) {
s=QCString(t->image)+" ";
          }
          break;
          }
        default:
          jj_la1[2] = jj_gen;
          ;
        }
        }
        if (!hasError) {
        str = expression();
        }
        if (!hasError) {
return s+str;
        }
      } else if (jj_2_3(5)) {
        if (!hasError) {
        str = subtype_indication();
        }
        if (!hasError) {
return str;
        }
      } else {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case BASIC_IDENTIFIER:
        case EXTENDED_CHARACTER:{
          if (!hasError) {
          str = identifier();
          }
          if (!hasError) {
return str;
          }
          break;
          }
        default:
          jj_la1[4] = jj_gen;
          jj_consume_token(-1);
          errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
        }
      }
    }
assert(false);
}


QCString VhdlParser::actual_parameter_part() {QCString s;
    if (!hasError) {
    s = association_list();
    }
return s;
assert(false);
}


QCString VhdlParser::actual_part() {QCString s,s1;
    if (jj_2_4(2147483647)) {
      if (!hasError) {
      s = actual_designator();
      }
      if (!hasError) {
return s;
      }
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case BOX_T:{
        if (!hasError) {
        jj_consume_token(BOX_T);
        }
        if (!hasError) {
return "<>";
        }
        break;
        }
      default:
        jj_la1[5] = jj_gen;
        if (jj_2_5(2147483647)) {
          if (!hasError) {
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
        } else {
          jj_consume_token(-1);
          errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
        }
      }
    }
assert(false);
}


QCString VhdlParser::adding_operator() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case PLUS_T:{
      if (!hasError) {
      jj_consume_token(PLUS_T);
      }
      if (!hasError) {
return "+";
      }
      break;
      }
    case MINUS_T:{
      if (!hasError) {
      jj_consume_token(MINUS_T);
      }
      if (!hasError) {
return "-";
      }
      break;
      }
    case AMPERSAND_T:{
      if (!hasError) {
      jj_consume_token(AMPERSAND_T);
      }
      if (!hasError) {
return "&";
      }
      break;
      }
    default:
      jj_la1[6] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::aggregate() {QCString s,s1,s2;
    if (!hasError) {
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
        jj_la1[7] = jj_gen;
        goto end_label_1;
      }
      if (!hasError) {
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
return QCString("("+s+")");
assert(false);
}


QCString VhdlParser::alias_declaration() {QCString s,s1,s2;
    if (!hasError) {
    jj_consume_token(ALIAS_T);
    }
    if (!hasError) {
    s2 = alias_designator();
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case COLON_T:{
      if (!hasError) {
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
      jj_la1[8] = jj_gen;
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
    case LBRACKET_T:{
      if (!hasError) {
      s1 = signature();
      }
      if (!hasError) {
s+=s1;
      }
      break;
      }
    default:
      jj_la1[9] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
outlineParser()->addVhdlType(s2,outlineParser()->getLine(ALIAS_T),
      EntryType::makeVariable(),VhdlSpecifier::ALIAS,QCString(),s,Protection::Public);

 return s2+" "+s+";";
assert(false);
}


QCString VhdlParser::alias_designator() {Token *tok=nullptr;QCString s;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      s = identifier();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    case CHARACTER_LITERAL:{
      if (!hasError) {
      tok = jj_consume_token(CHARACTER_LITERAL);
      }
      if (!hasError) {
return QCString(tok->image);
      }
      break;
      }
    case STRINGLITERAL:{
      if (!hasError) {
      s = operator_symbol();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    default:
      jj_la1[10] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


void VhdlParser::allocator() {
    if (jj_2_6(3)) {
      if (!hasError) {
      jj_consume_token(NEW_T);
      }
      if (!hasError) {
      qualified_expression();
      }
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case NEW_T:{
        if (!hasError) {
        jj_consume_token(NEW_T);
        }
        if (!hasError) {
        subtype_indication();
        }
        break;
        }
      default:
        jj_la1[11] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
}


void VhdlParser::architecture_body() {QCString s,s1;
    if (!hasError) {
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
    m_sharedState->genLabels.clear();
    outlineParser()->pushLabel(m_sharedState->genLabels,s1);
    m_sharedState->lastCompound=m_sharedState->current;
    outlineParser()->addVhdlType(t,outlineParser()->getLine(ARCHITECTURE_T),
                                 EntryType::makeClass(),VhdlSpecifier::ARCHITECTURE,QCString(),QCString(),Protection::Private);
    }
    if (!hasError) {
    try {
      if (!hasError) {
      architecture_declarative_part();
      }
    } catch ( ...) {
outlineParser()->error_skipto(BEGIN_T);
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
    case ARCHITECTURE_T:{
      if (!hasError) {
      jj_consume_token(ARCHITECTURE_T);
      }
      break;
      }
    default:
      jj_la1[12] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      name();
      }
      break;
      }
    default:
      jj_la1[13] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
m_sharedState->lastEntity=nullptr;m_sharedState->lastCompound=nullptr; m_sharedState->genLabels.clear();
}


void VhdlParser::anonymous_type_indication() {
    if (!hasError) {
    jj_consume_token(TYPE_T);
    }
    if (!hasError) {
    jj_consume_token(IS_T);
    }
    if (!hasError) {
    incomplete_type_definition();
    }
}


void VhdlParser::architecture_declarative_part() {
    if (!hasError) {
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
      case PACKAGE_T:
      case PROCEDURE_T:
      case PURE_T:
      case SIGNAL_T:
      case SHARED_T:
      case SUBTYPE_T:
      case TYPE_T:
      case USE_T:
      case VARIABLE_T:
      case VIEW_T:{
        ;
        break;
        }
      default:
        jj_la1[14] = jj_gen;
        goto end_label_2;
      }
      if (!hasError) {
      block_declarative_item();
      }
    }
    end_label_2: ;
    }
}


void VhdlParser::architecture_statement_part() {
    if (!hasError) {
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ASSERT_T:
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
        jj_la1[15] = jj_gen;
        goto end_label_3;
      }
      if (!hasError) {
      concurrent_statement();
      }
    }
    end_label_3: ;
    }
}


void VhdlParser::array_constraint() {
    if (jj_2_7(2)) {
      if (!hasError) {
      jj_consume_token(LPAREN_T);
      }
      if (!hasError) {
      jj_consume_token(OPEN_T);
      }
      if (!hasError) {
      jj_consume_token(RPAREN_T);
      }
      if (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case LPAREN_T:{
        if (!hasError) {
        array_element_constraint();
        }
        break;
        }
      default:
        jj_la1[16] = jj_gen;
        ;
      }
      }
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case LPAREN_T:{
        if (!hasError) {
        index_constraint();
        }
        if (!hasError) {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case LPAREN_T:{
          if (!hasError) {
          array_element_constraint();
          }
          break;
          }
        default:
          jj_la1[17] = jj_gen;
          ;
        }
        }
        break;
        }
      default:
        jj_la1[18] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
}


void VhdlParser::array_element_constraint() {
    if (!hasError) {
    element_constraint();
    }
}


void VhdlParser::array_element_resolution() {
    if (!hasError) {
    resolution_indication();
    }
}


void VhdlParser::array_incomplete_type_definition() {
    if (!hasError) {
    jj_consume_token(ARRAY_T);
    }
    if (!hasError) {
    jj_consume_token(LPAREN_T);
    }
    if (!hasError) {
    array_index_incomplete_type_list();
    }
    if (!hasError) {
    jj_consume_token(RPAREN_T);
    }
    if (!hasError) {
    jj_consume_token(OF_T);
    }
    if (!hasError) {
    incomplete_subtype_indication();
    }
}


void VhdlParser::array_index_incomplete_type() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LPAREN_T:{
      if (!hasError) {
      index_constraint();
      }
      break;
      }
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      index_subtype_definition();
      }
      break;
      }
    case TYPE_T:{
      if (!hasError) {
      anonymous_type_indication();
      }
      break;
      }
    default:
      jj_la1[19] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
}


void VhdlParser::array_index_incomplete_type_list() {
    if (!hasError) {
    array_index_incomplete_type();
    }
    if (!hasError) {
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case COMMA_T:{
        ;
        break;
        }
      default:
        jj_la1[20] = jj_gen;
        goto end_label_4;
      }
      if (!hasError) {
      jj_consume_token(COMMA_T);
      }
      if (!hasError) {
      array_index_incomplete_type();
      }
    }
    end_label_4: ;
    }
}


QCString VhdlParser::array_mode_view_indication() {QCString s;
    if (!hasError) {
    jj_consume_token(VIEW_T);
    }
    if (!hasError) {
    jj_consume_token(LPAREN_T);
    }
    if (!hasError) {
    identifier();
    }
    if (!hasError) {
    jj_consume_token(RPAREN_T);
    }
    if (!hasError) {
    jj_consume_token(OF_T);
    }
    if (!hasError) {
    subtype_indication();
    }
return "";
assert(false);
}


QCString VhdlParser::array_type_definition() {QCString s;
    if (jj_2_8(2147483647)) {
      if (!hasError) {
      s = constraint_array_definition();
      }
      if (!hasError) {
return s;
      }
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ARRAY_T:{
        if (!hasError) {
        s = unconstraint_array_definition();
        }
        if (!hasError) {
return s;
        }
        break;
        }
      default:
        jj_la1[21] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
assert(false);
}


QCString VhdlParser::assertion() {QCString s,s1,s2;Token *t=nullptr;Token *t1=nullptr;
    if (!hasError) {
    jj_consume_token(ASSERT_T);
    }
    if (!hasError) {
    s = condition();
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case REPORT_T:{
      if (!hasError) {
      t = jj_consume_token(REPORT_T);
      }
      if (!hasError) {
      s1 = expression();
      }
      break;
      }
    default:
      jj_la1[22] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SEVERITY_T:{
      if (!hasError) {
      t1 = jj_consume_token(SEVERITY_T);
      }
      if (!hasError) {
      s2 = expression();
      }
      break;
      }
    default:
      jj_la1[23] = jj_gen;
      ;
    }
    }
s.prepend("assert ");
   if(t!=nullptr) s1.prepend(" report ");
   if(t1!=nullptr) s2.prepend(" report ");
    return s+s1+s2;
assert(false);
}


QCString VhdlParser::assertion_statement() {QCString s,s1,s2;Token *t=nullptr;
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      s = label();
      }
      if (!hasError) {
      t = jj_consume_token(COLON_T);
      }
      break;
      }
    default:
      jj_la1[24] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    s1 = assertion();
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
if(t!=nullptr) s+=":";
    return s+s1+";";
assert(false);
}


QCString VhdlParser::association_element() {QCString s,s1;
    if (!hasError) {
    if (jj_2_9(2147483647)) {
      if (!hasError) {
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


QCString VhdlParser::association_list() {QCString s,s1;
    if (!hasError) {
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
        jj_la1[25] = jj_gen;
        goto end_label_5;
      }
      if (!hasError) {
      jj_consume_token(COMMA_T);
      }
      if (!hasError) {
      s1 = association_element();
      }
      if (!hasError) {
s+=","+s1;
      }
    }
    end_label_5: ;
    }
return s;
assert(false);
}


QCString VhdlParser::attribute_declaration() {QCString s,s1;
    if (!hasError) {
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
outlineParser()->addVhdlType(s,outlineParser()->getLine(ATTRIBUTE_T),
                                 EntryType::makeVariable(),VhdlSpecifier::ATTRIBUTE,QCString(),s1,Protection::Public);
    return " attribute "+s+":"+s1+";";
assert(false);
}


QCString VhdlParser::attribute_designator() {QCString s;Token *tok=nullptr;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      s = identifier();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    case RANGE_T:{
      if (!hasError) {
      tok = jj_consume_token(RANGE_T);
      }
      if (!hasError) {
return QCString(tok->image);
      }
      break;
      }
    default:
      jj_la1[26] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::attribute_name() {QCString s,s1;
    if (!hasError) {
    s = identifier();
    }
    if (!hasError) {
    jj_consume_token(APOSTROPHE_T);
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case RANGE_T:{
      if (!hasError) {
      jj_consume_token(RANGE_T);
      }
      break;
      }
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      s1 = name();
      }
      break;
      }
    default:
      jj_la1[27] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
    }
    if (!hasError) {
s+=s1;
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LPAREN_T:{
      if (!hasError) {
      jj_consume_token(LPAREN_T);
      }
      if (!hasError) {
      s1 = expression();
      }
      if (!hasError) {
      jj_consume_token(RPAREN_T);
      }
      if (!hasError) {
s+=s1;
      }
      break;
      }
    default:
      jj_la1[28] = jj_gen;
      ;
    }
    }
return s;
assert(false);
}


QCString VhdlParser::attribute_specification() {QCString s,s1,s2;
    if (!hasError) {
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
    s2 = conditional_expression();
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
QCString t= s1+" is "+s2;
   outlineParser()->addVhdlType(s,outlineParser()->getLine(ATTRIBUTE_T),
                                EntryType::makeVariable(),VhdlSpecifier::ATTRIBUTE,QCString(),t,Protection::Public);
   return " attribute "+s+" of "+s1+ " is "+s2+";";
assert(false);
}


QCString VhdlParser::base() {Token *tok=nullptr;
    if (!hasError) {
    tok = jj_consume_token(INTEGER);
    }
return QCString(tok->image);
assert(false);
}


QCString VhdlParser::base_specifier() {Token *tok=nullptr;
    if (!hasError) {
    tok = jj_consume_token(BASIC_IDENTIFIER);
    }
return QCString(tok->image);
assert(false);
}


QCString VhdlParser::base_unit_declaration() {QCString s;
    if (!hasError) {
    s = identifier();
    }
return s;
assert(false);
}


QCString VhdlParser::based_integer() {Token *tok=nullptr;
    if (!hasError) {
    tok = jj_consume_token(BASIC_IDENTIFIER);
    }
return QCString(tok->image);
assert(false);
}


QCString VhdlParser::based_literal() {Token *tok=nullptr;
    if (!hasError) {
    tok = jj_consume_token(BASED_LITERAL);
    }
return QCString(tok->image);
assert(false);
}


QCString VhdlParser::basic_identifier() {Token *tok=nullptr;
    if (!hasError) {
    tok = jj_consume_token(BASIC_IDENTIFIER);
    }
return QCString(tok->image);
assert(false);
}


void VhdlParser::binding_indication() {
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case USE_T:{
      if (!hasError) {
      jj_consume_token(USE_T);
      }
      if (!hasError) {
      entity_aspect();
      }
      break;
      }
    default:
      jj_la1[29] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case GENERIC_T:{
      if (!hasError) {
      generic_map_aspect();
      }
      break;
      }
    default:
      jj_la1[30] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case PORT_T:{
      if (!hasError) {
      port_map_aspect();
      }
      break;
      }
    default:
      jj_la1[31] = jj_gen;
      ;
    }
    }
}


QCString VhdlParser::bit_string_literal() {Token *tok=nullptr;
    if (!hasError) {
    tok = jj_consume_token(BIT_STRING_LITERAL);
    }
return QCString(tok->image);
assert(false);
}


QCString VhdlParser::bit_value() {Token *tok=nullptr;
    if (!hasError) {
    tok = jj_consume_token(BASIC_IDENTIFIER);
    }
return QCString(tok->image);
assert(false);
}


void VhdlParser::block_configuration() {
    if (!hasError) {
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
        jj_la1[32] = jj_gen;
        goto end_label_6;
      }
      if (!hasError) {
      use_clause();
      }
    }
    end_label_6: ;
    }
    if (!hasError) {
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case FOR_T:{
        ;
        break;
        }
      default:
        jj_la1[33] = jj_gen;
        goto end_label_7;
      }
      if (!hasError) {
      configuration_item();
      }
    }
    end_label_7: ;
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
    case PURE_T:{
      if (!hasError) {
      subprogram_declaration();
      }
      break;
      }
    case TYPE_T:{
      if (!hasError) {
      type_declaration();
      }
      break;
      }
    case SUBTYPE_T:{
      if (!hasError) {
      subtype_declaration();
      }
      break;
      }
    case VIEW_T:{
      if (!hasError) {
      mode_view_declaration();
      }
      break;
      }
    case CONSTANT_T:{
      if (!hasError) {
      constant_declaration();
      }
      break;
      }
    case SIGNAL_T:{
      if (!hasError) {
      signal_declaration();
      }
      break;
      }
    case SHARED_T:
    case VARIABLE_T:{
      if (!hasError) {
      variable_declaration();
      }
      break;
      }
    case FILE_T:{
      if (!hasError) {
      file_declaration();
      }
      break;
      }
    case ALIAS_T:{
      if (!hasError) {
      alias_declaration();
      }
      break;
      }
    default:
      jj_la1[34] = jj_gen;
      if (jj_2_10(2)) {
        if (!hasError) {
        package_body();
        }
      } else if (jj_2_11(2147483647)) {
        if (!hasError) {
        package_declaration();
        }
      } else {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case PACKAGE_T:{
          if (!hasError) {
          package_instantiation_declaration();
          }
          break;
          }
        case COMPONENT_T:{
          if (!hasError) {
          component_declaration();
          }
          break;
          }
        default:
          jj_la1[35] = jj_gen;
          if (jj_2_12(2147483647)) {
            if (!hasError) {
            attribute_declaration();
            }
          } else {
            switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
            case ATTRIBUTE_T:{
              if (!hasError) {
              attribute_specification();
              }
              break;
              }
            case FOR_T:{
              if (!hasError) {
              configuration_specification();
              }
              break;
              }
            case DISCONNECT_T:{
              if (!hasError) {
              disconnection_specification();
              }
              break;
              }
            case USE_T:{
              if (!hasError) {
              use_clause();
              }
              break;
              }
            default:
              jj_la1[36] = jj_gen;
              if (jj_2_13(3)) {
                if (!hasError) {
                group_template_declaration();
                }
              } else {
                switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
                case GROUP_T:{
                  if (!hasError) {
                  group_declaration();
                  }
                  break;
                  }
                default:
                  jj_la1[37] = jj_gen;
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


void VhdlParser::block_declarative_part() {
    if (!hasError) {
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
      case PACKAGE_T:
      case PROCEDURE_T:
      case PURE_T:
      case SIGNAL_T:
      case SHARED_T:
      case SUBTYPE_T:
      case TYPE_T:
      case USE_T:
      case VARIABLE_T:
      case VIEW_T:{
        ;
        break;
        }
      default:
        jj_la1[38] = jj_gen;
        goto end_label_8;
      }
      if (!hasError) {
      block_declarative_item();
      }
    }
    end_label_8: ;
    }
}


void VhdlParser::block_header() {
    if (!hasError) {
    if (jj_2_14(2147483647)) {
      if (!hasError) {
      generic_clause();
      }
      if (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case GENERIC_T:{
        if (!hasError) {
        generic_map_aspect();
        }
        if (!hasError) {
        jj_consume_token(SEMI_T);
        }
        break;
        }
      default:
        jj_la1[39] = jj_gen;
        ;
      }
      }
    } else {
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case PORT_T:{
      if (!hasError) {
      port_clause();
      }
      if (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case PORT_T:{
        if (!hasError) {
        port_map_aspect();
        }
        if (!hasError) {
        jj_consume_token(SEMI_T);
        }
        break;
        }
      default:
        jj_la1[40] = jj_gen;
        ;
      }
      }
      break;
      }
    default:
      jj_la1[41] = jj_gen;
      ;
    }
    }
}


void VhdlParser::block_specification() {
    if (!hasError) {
    name();
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LPAREN_T:{
      if (!hasError) {
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
      jj_la1[42] = jj_gen;
      ;
    }
    }
}


void VhdlParser::block_statement() {QCString s;
    if (!hasError) {
    s = identifier();
    }
    if (!hasError) {
    jj_consume_token(COLON_T);
    }
    if (!hasError) {
    jj_consume_token(BLOCK_T);
    }
    if (!hasError) {
outlineParser()->pushLabel(m_sharedState->genLabels,s);
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LPAREN_T:{
      if (!hasError) {
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
      jj_la1[43] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case IS_T:{
      if (!hasError) {
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
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      identifier();
      }
      break;
      }
    default:
      jj_la1[45] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
m_sharedState->genLabels=outlineParser()->popLabel(m_sharedState->genLabels);
}


void VhdlParser::block_statement_part() {
    if (!hasError) {
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ASSERT_T:
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
        jj_la1[46] = jj_gen;
        goto end_label_9;
      }
      if (!hasError) {
      concurrent_statement();
      }
    }
    end_label_9: ;
    }
}


void VhdlParser::case_statement() {QCString s;
    if (!hasError) {
    if (jj_2_15(2)) {
      if (!hasError) {
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
    jj_consume_token(CASE_T);
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case Q_T:{
      if (!hasError) {
      jj_consume_token(Q_T);
      }
      break;
      }
    default:
      jj_la1[47] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    s = expression();
    }
    if (!hasError) {
QCString ca="case "+s;
     FlowChart::addFlowChart(FlowChart::CASE_NO,QCString(),ca);
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
        jj_la1[48] = jj_gen;
        goto end_label_10;
      }
      if (!hasError) {
      case_statement_alternative();
      }
    }
    end_label_10: ;
    }
    if (!hasError) {
    jj_consume_token(END_T);
    }
    if (!hasError) {
    jj_consume_token(CASE_T);
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case Q_T:{
      if (!hasError) {
      jj_consume_token(Q_T);
      }
      break;
      }
    default:
      jj_la1[49] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      identifier();
      }
      break;
      }
    default:
      jj_la1[50] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
FlowChart::moveToPrevLevel();
         FlowChart::addFlowChart(FlowChart::END_CASE,"end case",QCString());
}


void VhdlParser::case_scheme() {
    if (!hasError) {
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
    if (jj_2_16(3)) {
      if (!hasError) {
      jj_consume_token(END_T);
      }
      if (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case BASIC_IDENTIFIER:
      case EXTENDED_CHARACTER:{
        if (!hasError) {
        identifier();
        }
        break;
        }
      default:
        jj_la1[51] = jj_gen;
        ;
      }
      }
      if (!hasError) {
      jj_consume_token(SEMI_T);
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


void VhdlParser::when_stats() {
    if (!hasError) {
    while (!hasError) {
      if (!hasError) {
      jj_consume_token(WHEN_T);
      }
      if (!hasError) {
      if (jj_2_17(2)) {
        if (!hasError) {
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
        jj_la1[52] = jj_gen;
        goto end_label_11;
      }
    }
    end_label_11: ;
    }
}


void VhdlParser::case_statement_alternative() {QCString s;
    if (!hasError) {
    jj_consume_token(WHEN_T);
    }
    if (!hasError) {
    s = choices();
    }
    if (!hasError) {
    jj_consume_token(ARROW_T);
    }
    if (!hasError) {
QCString t("when ");
    t+=s+"=> ";
    FlowChart::addFlowChart(FlowChart::WHEN_NO,s,t);
    }
    if (!hasError) {
    sequential_statement_body();
    }
FlowChart::moveToPrevLevel();
}


QCString VhdlParser::character_literal() {Token *tok=nullptr;
    if (!hasError) {
    tok = jj_consume_token(CHARACTER_LITERAL);
    }
return QCString(tok->image);
assert(false);
}


QCString VhdlParser::choice() {QCString s;
    if (jj_2_18(2147483647)) {
      if (!hasError) {
      s = range();
      }
      if (!hasError) {
return s;
      }
    } else if (jj_2_19(2147483647)) {
      if (!hasError) {
      s = simple_expression();
      }
      if (!hasError) {
return s;
      }
    } else if (jj_2_20(2147483647)) {
      if (!hasError) {
      s = discrete_range();
      }
      if (!hasError) {
return s;
      }
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case BASIC_IDENTIFIER:
      case EXTENDED_CHARACTER:{
        if (!hasError) {
        s = identifier();
        }
        if (!hasError) {
return s;
        }
        break;
        }
      case OTHER_T:{
        if (!hasError) {
        jj_consume_token(OTHER_T);
        }
        if (!hasError) {
return " others ";
        }
        break;
        }
      default:
        jj_la1[53] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
assert(false);
}


QCString VhdlParser::choices() {QCString s,s1;
    if (!hasError) {
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
        jj_la1[54] = jj_gen;
        goto end_label_12;
      }
      if (!hasError) {
      jj_consume_token(BAR_T);
      }
      if (!hasError) {
      s1 = choice();
      }
      if (!hasError) {
s+="|";s+=s1;
      }
    }
    end_label_12: ;
    }
return s;
assert(false);
}


void VhdlParser::component_configuration() {
    if (!hasError) {
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
    case SEMI_T:{
      if (!hasError) {
      binding_indication();
      }
      if (!hasError) {
      jj_consume_token(SEMI_T);
      }
      break;
      }
    default:
      jj_la1[55] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case FOR_T:{
      if (!hasError) {
      block_configuration();
      }
      break;
      }
    default:
      jj_la1[56] = jj_gen;
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


void VhdlParser::component_declaration() {QCString s;
    if (!hasError) {
    jj_consume_token(COMPONENT_T);
    }
    if (!hasError) {
    s = identifier();
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case IS_T:{
      if (!hasError) {
      jj_consume_token(IS_T);
      }
      break;
      }
    default:
      jj_la1[57] = jj_gen;
      ;
    }
    }
    if (!hasError) {
m_sharedState->currP=VhdlSpecifier::COMPONENT;
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case GENERIC_T:{
      if (!hasError) {
      generic_clause();
      }
      break;
      }
    default:
      jj_la1[58] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case PORT_T:{
      if (!hasError) {
      port_clause();
      }
      break;
      }
    default:
      jj_la1[59] = jj_gen;
      ;
    }
    }
    if (!hasError) {
outlineParser()->addVhdlType(s,outlineParser()->getLine(COMPONENT_T),
                                  EntryType::makeVariable(),VhdlSpecifier::COMPONENT,QCString(),QCString(),Protection::Public);
     m_sharedState->currP=VhdlSpecifier::UNKNOWN;
    }
    if (!hasError) {
    jj_consume_token(END_T);
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case COMPONENT_T:{
      if (!hasError) {
      jj_consume_token(COMPONENT_T);
      }
      break;
      }
    default:
      jj_la1[60] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      identifier();
      }
      break;
      }
    default:
      jj_la1[61] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
}


void VhdlParser::component_instantiation_statement() {QCString s,s1;
    if (!hasError) {
    s = identifier();
    }
    if (!hasError) {
    jj_consume_token(COLON_T);
    }
    if (!hasError) {
    s1 = instantiation_unit();
    }
    if (!hasError) {
QCString s3;
       if (s1.contains("|")) {
         s3=VhdlDocGen::getIndexWord(s1,0);
         s1=VhdlDocGen::getIndexWord(s1,1);
       }

       outlineParser()->addCompInst(s.lower(),s1.lower(),s3,outlineParser()->getLine());
    }
    if (!hasError) {
    if (jj_2_21(2147483647)) {
      if (!hasError) {
      generic_map_aspect();
      }
    } else {
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case PORT_T:{
      if (!hasError) {
      port_map_aspect();
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
}


void VhdlParser::component_specification() {
    if (!hasError) {
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
    case ARRAY_T:{
      if (!hasError) {
      s = array_type_definition();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    case RECORD_T:{
      if (!hasError) {
      record_type_definition();
      }
      if (!hasError) {
return s+"#";
      }
      break;
      }
    default:
      jj_la1[63] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


void VhdlParser::concurrent_assertion_statement() {
    if (!hasError) {
    if (jj_2_22(2)) {
      if (!hasError) {
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
    case POSTPONED_T:{
      if (!hasError) {
      jj_consume_token(POSTPONED_T);
      }
      break;
      }
    default:
      jj_la1[64] = jj_gen;
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


void VhdlParser::concurrent_procedure_call_statement() {
    if (!hasError) {
    if (jj_2_23(2)) {
      if (!hasError) {
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
    case POSTPONED_T:{
      if (!hasError) {
      jj_consume_token(POSTPONED_T);
      }
      break;
      }
    default:
      jj_la1[65] = jj_gen;
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


void VhdlParser::concurrent_signal_assignment_statement() {
    if (!hasError) {
    if (jj_2_24(2)) {
      if (!hasError) {
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
    case POSTPONED_T:{
      if (!hasError) {
      jj_consume_token(POSTPONED_T);
      }
      break;
      }
    default:
      jj_la1[66] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    if (jj_2_25(2147483647)) {
      if (!hasError) {
      concurrent_conditional_signal_assignment();
      }
    } else if (jj_2_26(2147483647)) {
      if (!hasError) {
      concurrent_selected_signal_assignment();
      }
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case LPAREN_T:
      case SLSL_T:
      case STRINGLITERAL:
      case BASIC_IDENTIFIER:
      case EXTENDED_CHARACTER:{
        if (!hasError) {
        concurrent_simple_signal_assignment();
        }
        break;
        }
      default:
        jj_la1[67] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
    }
}


void VhdlParser::concurrent_conditional_signal_assignment() {
    if (!hasError) {
    target();
    }
    if (!hasError) {
    jj_consume_token(LESSTHAN_T);
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case GUARDED_T:{
      if (!hasError) {
      jj_consume_token(GUARDED_T);
      }
      break;
      }
    default:
      jj_la1[68] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case INERTIAL_T:
    case REJECT_T:
    case TRANSPORT_T:{
      if (!hasError) {
      delay_mechanism();
      }
      break;
      }
    default:
      jj_la1[69] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    conditional_waveforms();
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
}


void VhdlParser::concurrent_simple_signal_assignment() {
    if (!hasError) {
    target();
    }
    if (!hasError) {
    jj_consume_token(LESSTHAN_T);
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case GUARDED_T:{
      if (!hasError) {
      jj_consume_token(GUARDED_T);
      }
      break;
      }
    default:
      jj_la1[70] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case INERTIAL_T:
    case REJECT_T:
    case TRANSPORT_T:{
      if (!hasError) {
      delay_mechanism();
      }
      break;
      }
    default:
      jj_la1[71] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    waveform();
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
}


void VhdlParser::concurrent_selected_signal_assignment() {
    if (!hasError) {
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
    case Q_T:{
      if (!hasError) {
      jj_consume_token(Q_T);
      }
      break;
      }
    default:
      jj_la1[72] = jj_gen;
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
    case GUARDED_T:{
      if (!hasError) {
      jj_consume_token(GUARDED_T);
      }
      break;
      }
    default:
      jj_la1[73] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case INERTIAL_T:
    case REJECT_T:
    case TRANSPORT_T:{
      if (!hasError) {
      delay_mechanism();
      }
      break;
      }
    default:
      jj_la1[74] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    selected_waveforms();
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
}


void VhdlParser::concurrent_signal_association_statement() {
    if (!hasError) {
    if (jj_2_27(2)) {
      if (!hasError) {
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
    concurrent_simple_signal_association();
    }
}


void VhdlParser::concurrent_simple_signal_association() {
    if (!hasError) {
    identifier();
    }
    if (!hasError) {
    jj_consume_token(LGT_T);
    }
    if (!hasError) {
    identifier();
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
}


void VhdlParser::concurrent_statement() {
    if (jj_2_28(2147483647)) {
      if (!hasError) {
      block_statement();
      }
    } else if (jj_2_29(2147483647)) {
      if (!hasError) {
      process_statement();
      }
    } else if (jj_2_30(3)) {
      if (!hasError) {
      generate_statement();
      }
    } else if (jj_2_31(2147483647)) {
      if (!hasError) {
      concurrent_assertion_statement();
      }
    } else if (jj_2_32(2147483647)) {
      if (!hasError) {
      concurrent_signal_assignment_statement();
      }
    } else if (jj_2_33(2147483647)) {
      if (!hasError) {
      concurrent_signal_association_statement();
      }
    } else if (jj_2_34(2147483647)) {
      if (!hasError) {
      component_instantiation_statement();
      }
    } else if (jj_2_35(2147483647)) {
      if (!hasError) {
      concurrent_procedure_call_statement();
      }
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case VHDL2008TOOLDIR:{
        if (!hasError) {
        jj_consume_token(VHDL2008TOOLDIR);
        }
        break;
        }
      default:
        jj_la1[75] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
}


QCString VhdlParser::condition() {QCString s;
    if (!hasError) {
    s = expression();
    }
return s;
assert(false);
}


QCString VhdlParser::condition_operator() {QCString s;
    if (!hasError) {
    jj_consume_token(QQ_T);
    }
return "??";
assert(false);
}


QCString VhdlParser::condition_clause() {QCString s;
    if (!hasError) {
    jj_consume_token(UNTIL_T);
    }
    if (!hasError) {
    s = condition();
    }
return " until "+s;
assert(false);
}


void VhdlParser::conditional_signal_assignment_wave() {
    if (jj_2_36(2147483647)) {
      if (!hasError) {
      conditional_force_assignment();
      }
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case LPAREN_T:
      case SLSL_T:
      case STRINGLITERAL:
      case BASIC_IDENTIFIER:
      case EXTENDED_CHARACTER:{
        if (!hasError) {
        conditional_waveform_assignment();
        }
        break;
        }
      default:
        jj_la1[76] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
}


void VhdlParser::conditional_waveform_assignment() {
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
    case TRANSPORT_T:{
      if (!hasError) {
      delay_mechanism();
      }
      break;
      }
    default:
      jj_la1[77] = jj_gen;
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
    case ELSE_T:{
      if (!hasError) {
      else_wave_list();
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
}


void VhdlParser::else_wave_list() {
    if (!hasError) {
    jj_consume_token(ELSE_T);
    }
    if (!hasError) {
    expression();
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case WHEN_T:{
      if (!hasError) {
      jj_consume_token(WHEN_T);
      }
      if (!hasError) {
      expression();
      }
      break;
      }
    default:
      jj_la1[79] = jj_gen;
      ;
    }
    }
}


void VhdlParser::conditional_force_assignment() {
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
    case OUT_T:{
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case IN_T:{
        if (!hasError) {
        jj_consume_token(IN_T);
        }
        break;
        }
      case OUT_T:{
        if (!hasError) {
        jj_consume_token(OUT_T);
        }
        break;
        }
      default:
        jj_la1[80] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
      break;
      }
    default:
      jj_la1[81] = jj_gen;
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
    case AND_T:
    case NAND_T:
    case NEW_T:
    case NOR_T:
    case NOT_T:
    case NULL_T:
    case OR_T:
    case XOR_T:
    case XNOR_T:
    case LPAREN_T:
    case PLUS_T:
    case MINUS_T:
    case SLSL_T:
    case QQ_T:
    case INTEGER:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:
    case CHARACTER_LITERAL:
    case DECIMAL_LITERAL:
    case BASED_LITERAL:
    case BIT_STRING_LITERAL:{
      if (!hasError) {
      expression();
      }
      if (!hasError) {
      else_stat();
      }
      break;
      }
    default:
      jj_la1[82] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
}


void VhdlParser::else_stat() {
    if (!hasError) {
    while (!hasError) {
      if (!hasError) {
      jj_consume_token(ELSE_T);
      }
      if (!hasError) {
      expression();
      }
      if (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case WHEN_T:{
        if (!hasError) {
        jj_consume_token(WHEN_T);
        }
        if (!hasError) {
        expression();
        }
        break;
        }
      default:
        jj_la1[83] = jj_gen;
        ;
      }
      }
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ELSE_T:{
        ;
        break;
        }
      default:
        jj_la1[84] = jj_gen;
        goto end_label_13;
      }
    }
    end_label_13: ;
    }
}


QCString VhdlParser::conditional_expression() {QCString s;
    if (!hasError) {
    s = expression();
    }
    if (!hasError) {
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case WHEN_T:{
        ;
        break;
        }
      default:
        jj_la1[85] = jj_gen;
        goto end_label_14;
      }
      if (!hasError) {
      jj_consume_token(WHEN_T);
      }
      if (!hasError) {
      condition();
      }
      if (!hasError) {
      jj_consume_token(ELSE_T);
      }
      if (!hasError) {
      expression();
      }
    }
    end_label_14: ;
    }
return s;
assert(false);
}


QCString VhdlParser::conditional_or_unaffected_expression() {QCString s;
    if (!hasError) {
    s = expression_or_unaffected();
    }
    if (!hasError) {
    while (!hasError) {
      if (jj_2_37(2147483647)) {
        ;
      } else {
        goto end_label_15;
      }
      if (!hasError) {
      jj_consume_token(WHEN_T);
      }
      if (!hasError) {
      condition();
      }
      if (!hasError) {
      jj_consume_token(ELSE_T);
      }
      if (!hasError) {
      expression_or_unaffected();
      }
    }
    end_label_15: ;
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case WHEN_T:{
      if (!hasError) {
      jj_consume_token(WHEN_T);
      }
      if (!hasError) {
      condition();
      }
      break;
      }
    default:
      jj_la1[86] = jj_gen;
      ;
    }
    }
return s;
assert(false);
}


void VhdlParser::conditional_signal_assignment() {
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
    conditional_waveforms();
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
}


void VhdlParser::conditional_waveforms() {
    if (!hasError) {
    waveform();
    }
    if (!hasError) {
    while (!hasError) {
      if (jj_2_38(2147483647)) {
        ;
      } else {
        goto end_label_16;
      }
      if (!hasError) {
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
    end_label_16: ;
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case WHEN_T:{
      if (!hasError) {
      jj_consume_token(WHEN_T);
      }
      if (!hasError) {
      condition();
      }
      break;
      }
    default:
      jj_la1[87] = jj_gen;
      ;
    }
    }
}


void VhdlParser::configuration_declaration() {QCString s,s1;
    if (!hasError) {
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
m_sharedState->confName=s+"::"+s1;
  outlineParser()->addVhdlType(s,outlineParser()->getLine(CONFIGURATION_T),
                               EntryType::makeVariable(),VhdlSpecifier::CONFIG,"configuration",s1,Protection::Public);
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
    case CONFIGURATION_T:{
      if (!hasError) {
      jj_consume_token(CONFIGURATION_T);
      }
      break;
      }
    default:
      jj_la1[88] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      name();
      }
      break;
      }
    default:
      jj_la1[89] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
m_sharedState->genLabels.clear(); m_sharedState->confName="";
}


void VhdlParser::configuration_declarative_item() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case USE_T:{
      if (!hasError) {
      use_clause();
      }
      break;
      }
    case ATTRIBUTE_T:{
      if (!hasError) {
      attribute_specification();
      }
      break;
      }
    case GROUP_T:{
      if (!hasError) {
      group_declaration();
      }
      break;
      }
    default:
      jj_la1[90] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
}


void VhdlParser::configuration_declarative_part() {
    if (!hasError) {
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ATTRIBUTE_T:
      case GROUP_T:
      case USE_T:{
        ;
        break;
        }
      default:
        jj_la1[91] = jj_gen;
        goto end_label_17;
      }
      if (!hasError) {
      configuration_declarative_item();
      }
    }
    end_label_17: ;
    }
}


void VhdlParser::configuration_item() {
    if (jj_2_39(2147483647)) {
      if (!hasError) {
      component_configuration();
      }
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case FOR_T:{
        if (!hasError) {
        block_configuration();
        }
        break;
        }
      default:
        jj_la1[92] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
}


void VhdlParser::configuration_specification() {
    if (!hasError) {
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


QCString VhdlParser::constant_declaration() {QCString s,s1,s2;Token *t=nullptr;
    if (!hasError) {
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
    case VARASSIGN_T:{
      if (!hasError) {
      t = jj_consume_token(VARASSIGN_T);
      }
      if (!hasError) {
      s2 = conditional_expression();
      }
      break;
      }
    default:
      jj_la1[93] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
if(t!=nullptr)
      s2.prepend(":=");
     QCString it=s1+s2;
     outlineParser()->addVhdlType(s,outlineParser()->getLine(CONSTANT_T),
                                  EntryType::makeVariable(),VhdlSpecifier::CONSTANT,QCString(),it,Protection::Public);
     it.prepend("constant ");
     return it;
assert(false);
}


QCString VhdlParser::constraint_array_definition() {QCString s,s1;
    if (!hasError) {
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


void VhdlParser::context_clause() {
    if (!hasError) {
    while (!hasError) {
      if (!hasError) {
      context_item();
      }
      if (jj_2_40(3)) {
        ;
      } else {
        goto end_label_18;
      }
    }
    end_label_18: ;
    }
}


QCString VhdlParser::constraint() {QCString s;
    if (jj_2_41(2147483647)) {
      if (!hasError) {
      s = range_constraint();
      }
      if (!hasError) {
return s;
      }
    } else if (jj_2_42(2147483647)) {
      if (!hasError) {
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
}


void VhdlParser::context_item() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LIBRARY_T:{
      if (!hasError) {
      library_clause();
      }
      break;
      }
    case USE_T:{
      if (!hasError) {
      use_clause();
      }
      break;
      }
    case CONTEXT_T:{
      if (!hasError) {
      context_ref();
      }
      break;
      }
    default:
      jj_la1[94] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
}


QCString VhdlParser::context_ref() {QCString s, s1;
    if (!hasError) {
    jj_consume_token(CONTEXT_T);
    }
    if (!hasError) {
    s = identifier();
    }
    if (!hasError) {
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case DOT_T:{
        ;
        break;
        }
      default:
        jj_la1[95] = jj_gen;
        goto end_label_19;
      }
      if (!hasError) {
      jj_consume_token(DOT_T);
      }
      if (!hasError) {
      s1 = identifier();
      }
      if (!hasError) {
s+=".";s+=s1;
      }
    }
    end_label_19: ;
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
return "context "+s ;
assert(false);
}


void VhdlParser::context_declaration() {QCString s,s1;
    if (!hasError) {
    jj_consume_token(CONTEXT_T);
    }
    if (!hasError) {
    s = identifier();
    }
    if (!hasError) {
    jj_consume_token(IS_T);
    }
    if (!hasError) {
m_sharedState->parse_sec=VhdlSection::CONTEXT_SEC;
    }
    if (!hasError) {
    context_clause();
    }
    if (!hasError) {
    jj_consume_token(END_T);
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case CONTEXT_T:{
      if (!hasError) {
      jj_consume_token(CONTEXT_T);
      }
      break;
      }
    default:
      jj_la1[96] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      identifier();
      }
      break;
      }
    default:
      jj_la1[97] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
m_sharedState->parse_sec=VhdlSection::UNKNOWN;
                          outlineParser()->addVhdlType(s,outlineParser()->getLine(LIBRARY_T),
                                                       EntryType::makeVariable(),VhdlSpecifier::LIBRARY,"context",s1,Protection::Public);
}


QCString VhdlParser::decimal_literal() {Token *tok=nullptr;
    if (!hasError) {
    tok = jj_consume_token(DECIMAL_LITERAL);
    }
return QCString(tok->image);
assert(false);
}


QCString VhdlParser::delay_mechanism() {QCString s;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case TRANSPORT_T:{
      if (!hasError) {
      jj_consume_token(TRANSPORT_T);
      }
      if (!hasError) {
return " transport ";
      }
      break;
      }
    case INERTIAL_T:
    case REJECT_T:{
      if (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case REJECT_T:{
        if (!hasError) {
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
        jj_la1[98] = jj_gen;
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
      jj_la1[99] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


void VhdlParser::design_file() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case ARCHITECTURE_T:
    case CONFIGURATION_T:
    case CONTEXT_T:
    case ENTITY_T:
    case LIBRARY_T:
    case PACKAGE_T:
    case USE_T:{
      if (!hasError) {
      while (!hasError) {
        if (!hasError) {
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
          jj_la1[100] = jj_gen;
          goto end_label_20;
        }
      }
      end_label_20: ;
      }
      if (!hasError) {

      }
      break;
      }
    case 0:{
      if (!hasError) {
      jj_consume_token(0);
      }
      break;
      }
    default:
      jj_la1[101] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
}


void VhdlParser::design_unit() {
    if (!hasError) {
    if (jj_2_43(3)) {
      if (!hasError) {
      context_clause();
      }
    } else {
      ;
    }
    }
    if (!hasError) {
    library_unit();
    }
}


QCString VhdlParser::designator() {QCString s;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      s = identifier();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    case STRINGLITERAL:{
      if (!hasError) {
      s = operator_symbol();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    default:
      jj_la1[102] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::direction() {Token *tok=nullptr;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case TO_T:{
      if (!hasError) {
      tok = jj_consume_token(TO_T);
      }
      if (!hasError) {
return QCString(tok->image);
      }
      break;
      }
    case DOWNTO_T:{
      if (!hasError) {
      tok = jj_consume_token(DOWNTO_T);
      }
      if (!hasError) {
return QCString(tok->image);
      }
      break;
      }
    default:
      jj_la1[103] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::discrete_incomplete_type_definition() {Token *tok=nullptr;
    if (!hasError) {
    jj_consume_token(LPAREN_T);
    }
    if (!hasError) {
    jj_consume_token(BOX_T);
    }
    if (!hasError) {
    jj_consume_token(RPAREN_T);
    }
return "(<>)";
assert(false);
}


void VhdlParser::disconnection_specification() {
    if (!hasError) {
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


void VhdlParser::guarded_signal_specificatio() {
    if (!hasError) {
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
    if (jj_2_44(2147483647)) {
      if (!hasError) {
      s = range();
      }
      if (!hasError) {
return s;
      }
    } else if (jj_2_45(2147483647)) {
      if (!hasError) {
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
}


QCString VhdlParser::element_array_mode_view_indication() {QCString s;
    if (!hasError) {
    jj_consume_token(VIEW_T);
    }
    if (!hasError) {
    jj_consume_token(LPAREN_T);
    }
    if (!hasError) {
    s = identifier();
    }
    if (!hasError) {
    jj_consume_token(RPAREN_T);
    }
return "view("+s+")";
assert(false);
}


QCString VhdlParser::element_association() {QCString s,s1;
    if (!hasError) {
    if (jj_2_46(2147483647)) {
      if (!hasError) {
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


QCString VhdlParser::element_constraint() {QCString s,s1;
    if (jj_2_47(5)) {
      if (!hasError) {
      record_constraint();
      }
      if (!hasError) {
return " ";
      }
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case LPAREN_T:{
        if (!hasError) {
        array_constraint();
        }
        if (!hasError) {
return " ";
        }
        break;
        }
      default:
        jj_la1[104] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
assert(false);
}


QCString VhdlParser::element_declaration() {QCString rec_name,s1,s2;
    if (!hasError) {
    rec_name = identifier_list();
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
auto ql = split(rec_name.str(),",");
    for (const auto &n : ql)
    {
      std::string name=n+"~";
      name+=outlineParser()->getNameID().str();
      outlineParser()->addVhdlType(
          name.c_str(),outlineParser()->getLine(),
          EntryType::makeVariable(),
          VhdlSpecifier::RECORD,QCString(),
          s1,
          Protection::Public);
    }
    s2=rec_name+":"+s1;
    return s2;
assert(false);
}


QCString VhdlParser::element_resolution() {QCString s,s1;
    if (jj_2_48(2147483647)) {
      if (!hasError) {
      array_element_resolution();
      }
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case BASIC_IDENTIFIER:
      case EXTENDED_CHARACTER:{
        if (!hasError) {
        record_resolution();
        }
        break;
        }
      default:
        jj_la1[105] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
assert(false);
}


QCString VhdlParser::element_mode_indication() {QCString s;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BUFFER_T:
    case IN_T:
    case INOUT_T:
    case LINKAGE_T:
    case OUT_T:{
      if (!hasError) {
      s = mode();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    case VIEW_T:{
      if (!hasError) {
      s = element_mode_view_indication();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    default:
      jj_la1[106] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::element_mode_view_indication() {QCString s;
    if (jj_2_49(2)) {
      if (!hasError) {
      s = element_record_mode_view_indication();
      }
      if (!hasError) {
return s;
      }
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case VIEW_T:{
        if (!hasError) {
        s = element_array_mode_view_indication();
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
    }
assert(false);
}


QCString VhdlParser::element_record_mode_view_indication() {QCString s;
    if (!hasError) {
    jj_consume_token(VIEW_T);
    }
    if (!hasError) {
    s = identifier();
    }
return "view "+s;
assert(false);
}


QCString VhdlParser::entity_aspect() {Token *tok=nullptr;QCString s,s1;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case ENTITY_T:{
      if (!hasError) {
      tok = jj_consume_token(ENTITY_T);
      }
      if (!hasError) {
      s = name();
      }
      if (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case LPAREN_T:{
        if (!hasError) {
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
        jj_la1[108] = jj_gen;
        ;
      }
      }
      if (!hasError) {
return s;
      }
      break;
      }
    case CONFIGURATION_T:{
      if (!hasError) {
      tok = jj_consume_token(CONFIGURATION_T);
      }
      if (!hasError) {
      s = name();
      }
      if (!hasError) {
return QCString(tok->image)+s;
      }
      break;
      }
    case OPEN_T:{
      if (!hasError) {
      tok = jj_consume_token(OPEN_T);
      }
      if (!hasError) {
return QCString(tok->image);
      }
      break;
      }
    default:
      jj_la1[109] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::entity_class() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case ENTITY_T:{
      if (!hasError) {
      jj_consume_token(ENTITY_T);
      }
      if (!hasError) {
return "entity";
      }
      break;
      }
    case ARCHITECTURE_T:{
      if (!hasError) {
      jj_consume_token(ARCHITECTURE_T);
      }
      if (!hasError) {
return "architecture";
      }
      break;
      }
    case CONFIGURATION_T:{
      if (!hasError) {
      jj_consume_token(CONFIGURATION_T);
      }
      if (!hasError) {
return "configuration";
      }
      break;
      }
    case PROCEDURE_T:{
      if (!hasError) {
      jj_consume_token(PROCEDURE_T);
      }
      if (!hasError) {
return "procedure";
      }
      break;
      }
    case FUNCTION_T:{
      if (!hasError) {
      jj_consume_token(FUNCTION_T);
      }
      if (!hasError) {
return "function";
      }
      break;
      }
    case PACKAGE_T:{
      if (!hasError) {
      jj_consume_token(PACKAGE_T);
      }
      if (!hasError) {
return "package";
      }
      break;
      }
    case TYPE_T:{
      if (!hasError) {
      jj_consume_token(TYPE_T);
      }
      if (!hasError) {
return "type";
      }
      break;
      }
    case SUBTYPE_T:{
      if (!hasError) {
      jj_consume_token(SUBTYPE_T);
      }
      if (!hasError) {
return "subtype";
      }
      break;
      }
    case CONSTANT_T:{
      if (!hasError) {
      jj_consume_token(CONSTANT_T);
      }
      if (!hasError) {
return "constant";
      }
      break;
      }
    case SIGNAL_T:{
      if (!hasError) {
      jj_consume_token(SIGNAL_T);
      }
      if (!hasError) {
return "signal";
      }
      break;
      }
    case VARIABLE_T:{
      if (!hasError) {
      jj_consume_token(VARIABLE_T);
      }
      if (!hasError) {
return "variable";
      }
      break;
      }
    case COMPONENT_T:{
      if (!hasError) {
      jj_consume_token(COMPONENT_T);
      }
      if (!hasError) {
return "component";
      }
      break;
      }
    case LABEL_T:{
      if (!hasError) {
      jj_consume_token(LABEL_T);
      }
      if (!hasError) {
return "label";
      }
      break;
      }
    case LITERAL_T:{
      if (!hasError) {
      jj_consume_token(LITERAL_T);
      }
      if (!hasError) {
return "literal";
      }
      break;
      }
    case UNITS_T:{
      if (!hasError) {
      jj_consume_token(UNITS_T);
      }
      if (!hasError) {
return "units";
      }
      break;
      }
    case GROUP_T:{
      if (!hasError) {
      jj_consume_token(GROUP_T);
      }
      if (!hasError) {
return "group";
      }
      break;
      }
    case FILE_T:{
      if (!hasError) {
      jj_consume_token(FILE_T);
      }
      if (!hasError) {
return "file";
      }
      break;
      }
    default:
      jj_la1[110] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::entity_class_entry() {QCString s;
    if (!hasError) {
    s = entity_class();
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BOX_T:{
      if (!hasError) {
      jj_consume_token(BOX_T);
      }
      if (!hasError) {
s+="<>";
      }
      break;
      }
    default:
      jj_la1[111] = jj_gen;
      ;
    }
    }
return s;
assert(false);
}


QCString VhdlParser::entity_class_entry_list() {QCString s,s1,s2;
    if (!hasError) {
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
        jj_la1[112] = jj_gen;
        goto end_label_21;
      }
      if (!hasError) {
      jj_consume_token(COMMA_T);
      }
      if (!hasError) {
      s = entity_class_entry();
      }
      if (!hasError) {
s2+=",";s2+=s;
      }
    }
    end_label_21: ;
    }
return s1+s2;
assert(false);
}


void VhdlParser::entity_declaration() {QCString s;
    if (!hasError) {
    jj_consume_token(ENTITY_T);
    }
    if (!hasError) {
    s = identifier();
    }
    if (!hasError) {
    jj_consume_token(IS_T);
    }
    if (!hasError) {
m_sharedState->lastEntity=m_sharedState->current;
                m_sharedState->lastCompound=nullptr;
                outlineParser()->addVhdlType(s,outlineParser()->getLine(ENTITY_T),
                              EntryType::makeClass(),VhdlSpecifier::ENTITY,QCString(),QCString(),Protection::Public);
    }
    if (!hasError) {
    entity_header();
    }
    if (!hasError) {
    entity_declarative_part();
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BEGIN_T:{
      if (!hasError) {
      jj_consume_token(BEGIN_T);
      }
      if (!hasError) {
      entity_statement_part();
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
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case ENTITY_T:{
      if (!hasError) {
      jj_consume_token(ENTITY_T);
      }
      break;
      }
    default:
      jj_la1[114] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      name();
      }
      break;
      }
    default:
      jj_la1[115] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
m_sharedState->lastEntity=nullptr;m_sharedState->lastCompound=nullptr; m_sharedState->genLabels.clear();
}


void VhdlParser::entity_declarative_item() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case FUNCTION_T:
    case IMPURE_T:
    case PROCEDURE_T:
    case PURE_T:{
      if (!hasError) {
      subprogram_declaration();
      }
      break;
      }
    case TYPE_T:{
      if (!hasError) {
      type_declaration();
      }
      break;
      }
    case SUBTYPE_T:{
      if (!hasError) {
      subtype_declaration();
      }
      break;
      }
    case CONSTANT_T:{
      if (!hasError) {
      constant_declaration();
      }
      break;
      }
    case SIGNAL_T:{
      if (!hasError) {
      signal_declaration();
      }
      break;
      }
    default:
      jj_la1[116] = jj_gen;
      if (jj_2_50(5)) {
        if (!hasError) {
        package_instantiation_declaration();
        }
      } else if (jj_2_51(3)) {
        if (!hasError) {
        package_body();
        }
      } else if (jj_2_52(3)) {
        if (!hasError) {
        package_declaration();
        }
      } else {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case SHARED_T:
        case VARIABLE_T:{
          if (!hasError) {
          variable_declaration();
          }
          break;
          }
        case FILE_T:{
          if (!hasError) {
          file_declaration();
          }
          break;
          }
        case ALIAS_T:{
          if (!hasError) {
          alias_declaration();
          }
          break;
          }
        case VIEW_T:{
          if (!hasError) {
          mode_view_declaration();
          }
          break;
          }
        default:
          jj_la1[117] = jj_gen;
          if (jj_2_53(2147483647)) {
            if (!hasError) {
            attribute_declaration();
            }
          } else {
            switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
            case ATTRIBUTE_T:{
              if (!hasError) {
              attribute_specification();
              }
              break;
              }
            case DISCONNECT_T:{
              if (!hasError) {
              disconnection_specification();
              }
              break;
              }
            case USE_T:{
              if (!hasError) {
              use_clause();
              }
              break;
              }
            default:
              jj_la1[118] = jj_gen;
              if (jj_2_54(3)) {
                if (!hasError) {
                group_template_declaration();
                }
              } else {
                switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
                case GROUP_T:{
                  if (!hasError) {
                  group_declaration();
                  }
                  break;
                  }
                default:
                  jj_la1[119] = jj_gen;
                  if (jj_2_55(5)) {
                    if (!hasError) {
                    package_instantiation_declaration();
                    }
                  } else {
                    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
                    case PACKAGE_T:{
                      if (!hasError) {
                      package_declaration();
                      }
                      break;
                      }
                    case VHDL2008TOOLDIR:{
                      if (!hasError) {
                      jj_consume_token(VHDL2008TOOLDIR);
                      }
                      break;
                      }
                    default:
                      jj_la1[120] = jj_gen;
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
    }
}


void VhdlParser::entity_declarative_part() {
    if (!hasError) {
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
      case VIEW_T:
      case VHDL2008TOOLDIR:{
        ;
        break;
        }
      default:
        jj_la1[121] = jj_gen;
        goto end_label_22;
      }
      if (!hasError) {
      entity_declarative_item();
      }
    }
    end_label_22: ;
    }
}


QCString VhdlParser::entity_designator() {QCString s,s1;
    if (!hasError) {
    s = entity_tag();
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LBRACKET_T:{
      if (!hasError) {
      s1 = signature();
      }
      break;
      }
    default:
      jj_la1[122] = jj_gen;
      ;
    }
    }
return s+s1;
assert(false);
}


void VhdlParser::entity_header() {
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case GENERIC_T:{
      if (!hasError) {
m_sharedState->currP=VhdlSpecifier::GENERIC;m_sharedState->parse_sec=VhdlSection::GEN_SEC;
      }
      if (!hasError) {
      generic_clause();
      }
      break;
      }
    default:
      jj_la1[123] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case PORT_T:{
      if (!hasError) {
m_sharedState->currP=VhdlSpecifier::PORT;
      }
      if (!hasError) {
      port_clause();
      }
      break;
      }
    default:
      jj_la1[124] = jj_gen;
      ;
    }
    }
}


QCString VhdlParser::entity_name_list() {QCString s,s1,s2;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:
    case CHARACTER_LITERAL:{
      if (!hasError) {
      if (!hasError) {
      s1 = entity_designator();
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
          jj_la1[125] = jj_gen;
          goto end_label_23;
        }
        if (!hasError) {
        jj_consume_token(COMMA_T);
        }
        if (!hasError) {
        s = entity_designator();
        }
        if (!hasError) {
s2+=s;
        }
      }
      end_label_23: ;
      }
      if (!hasError) {
return s2;
      }
      break;
      }
    case OTHER_T:{
      if (!hasError) {
      jj_consume_token(OTHER_T);
      }
      if (!hasError) {
return "other";
      }
      break;
      }
    case ALL_T:{
      if (!hasError) {
      jj_consume_token(ALL_T);
      }
      if (!hasError) {
return "all";
      }
      break;
      }
    default:
      jj_la1[126] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::entity_specification() {QCString s,s1;
    if (!hasError) {
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
    if (jj_2_56(2147483647)) {
      if (!hasError) {
      concurrent_assertion_statement();
      }
    } else if (jj_2_57(2147483647)) {
      if (!hasError) {
      process_statement();
      }
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case POSTPONED_T:
      case SLSL_T:
      case STRINGLITERAL:
      case BASIC_IDENTIFIER:
      case EXTENDED_CHARACTER:{
        if (!hasError) {
        concurrent_procedure_call_statement();
        }
        break;
        }
      default:
        jj_la1[127] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
}


void VhdlParser::entity_statement_part() {
    if (!hasError) {
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
        jj_la1[128] = jj_gen;
        goto end_label_24;
      }
      if (!hasError) {
      entity_statement();
      }
    }
    end_label_24: ;
    }
}


QCString VhdlParser::entity_tag() {QCString s;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      s = name();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    case CHARACTER_LITERAL:{
      if (!hasError) {
      s = character_literal();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    default:
      jj_la1[129] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::enumeration_literal() {QCString s;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      s = identifier();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    case CHARACTER_LITERAL:{
      if (!hasError) {
      s = character_literal();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    default:
      jj_la1[130] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::enumeration_type_definition() {QCString s,s1;
    if (!hasError) {
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
        jj_la1[131] = jj_gen;
        goto end_label_25;
      }
      if (!hasError) {
      jj_consume_token(COMMA_T);
      }
      if (!hasError) {
      s1 = enumeration_literal();
      }
      if (!hasError) {
s+=",";s+=s1;
      }
    }
    end_label_25: ;
    }
    if (!hasError) {
    jj_consume_token(RPAREN_T);
    }
return "("+s+")";
assert(false);
}


QCString VhdlParser::exit_statement() {QCString s,s1,s2;Token *t=nullptr;Token *t1=nullptr;
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      s = identifier();
      }
      if (!hasError) {
      t = jj_consume_token(COLON_T);
      }
      break;
      }
    default:
      jj_la1[132] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(EXIT_T);
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      s1 = identifier();
      }
      break;
      }
    default:
      jj_la1[133] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case WHEN_T:{
      if (!hasError) {
      t1 = jj_consume_token(WHEN_T);
      }
      if (!hasError) {
      s2 = condition();
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
m_sharedState->lab.clear();
  if(t!=nullptr) s+=":";
  if(t1!=nullptr) s2.prepend(" when ");
   FlowChart::addFlowChart(FlowChart::EXIT_NO,"exit",s2,s1);

  return s+s1+s2+";";
assert(false);
}


QCString VhdlParser::expression() {QCString s,s1,s2;
    if (!hasError) {
    /* LOOKAHEAD(5) */ s = relation();
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
        jj_la1[135] = jj_gen;
        goto end_label_26;
      }
      if (!hasError) {
      s1 = logop();
      }
      if (!hasError) {
      s2 = relation();
      }
      if (!hasError) {
s+=s1;s+=s2;
      }
    }
    end_label_26: ;
    }
return s;
assert(false);
}


QCString VhdlParser::expression_or_unaffected() {QCString s;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case ABS_T:
    case AND_T:
    case NAND_T:
    case NEW_T:
    case NOR_T:
    case NOT_T:
    case NULL_T:
    case OR_T:
    case XOR_T:
    case XNOR_T:
    case LPAREN_T:
    case PLUS_T:
    case MINUS_T:
    case SLSL_T:
    case QQ_T:
    case INTEGER:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:
    case CHARACTER_LITERAL:
    case DECIMAL_LITERAL:
    case BASED_LITERAL:
    case BIT_STRING_LITERAL:{
      if (!hasError) {
      s = expression();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    case UNAFFECTED_T:{
      if (!hasError) {
      jj_consume_token(UNAFFECTED_T);
      }
      if (!hasError) {
return "unaffected";
      }
      break;
      }
    default:
      jj_la1[136] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::external_pathname() {QCString s;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case DOT_T:{
      if (!hasError) {
      s = absolute_pathname();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    case NEG_T:{
      if (!hasError) {
      s = relative_pathname();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    case AT_T:{
      if (!hasError) {
      s = package_path_name();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    default:
      jj_la1[137] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::external_name() {QCString s,s1,s2;
    if (!hasError) {
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
    case CONSTANT_T:{
      if (!hasError) {
      t = jj_consume_token(CONSTANT_T);
      }
      if (!hasError) {
return QCString(t->image);
      }
      break;
      }
    case SIGNAL_T:{
      if (!hasError) {
      t = jj_consume_token(SIGNAL_T);
      }
      if (!hasError) {
return QCString(t->image);
      }
      break;
      }
    case VARIABLE_T:{
      if (!hasError) {
      t = jj_consume_token(VARIABLE_T);
      }
      if (!hasError) {
return QCString(t->image);
      }
      break;
      }
    default:
      jj_la1[138] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::logop() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case AND_T:{
      if (!hasError) {
      jj_consume_token(AND_T);
      }
      if (!hasError) {
return "and" ;
      }
      break;
      }
    case NAND_T:{
      if (!hasError) {
      jj_consume_token(NAND_T);
      }
      if (!hasError) {
return "nand" ;
      }
      break;
      }
    case NOR_T:{
      if (!hasError) {
      jj_consume_token(NOR_T);
      }
      if (!hasError) {
return "nor" ;
      }
      break;
      }
    case XNOR_T:{
      if (!hasError) {
      jj_consume_token(XNOR_T);
      }
      if (!hasError) {
return "xnor" ;
      }
      break;
      }
    case XOR_T:{
      if (!hasError) {
      jj_consume_token(XOR_T);
      }
      if (!hasError) {
return "xor" ;
      }
      break;
      }
    case OR_T:{
      if (!hasError) {
      jj_consume_token(OR_T);
      }
      if (!hasError) {
return "or" ;
      }
      break;
      }
    default:
      jj_la1[139] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::extended_identifier() {Token *t;
    if (!hasError) {
    t = jj_consume_token(EXTENDED_CHARACTER);
    }
return QCString(t->image);
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
    case BIT_STRING_LITERAL:{
      if (!hasError) {
      s = primary();
      }
      if (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case DOUBLEMULT_T:{
        if (!hasError) {
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
        jj_la1[140] = jj_gen;
        ;
      }
      }
      if (!hasError) {
return s;
      }
      break;
      }
    case ABS_T:{
      if (!hasError) {
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
    case NOT_T:{
      if (!hasError) {
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
    case QQ_T:{
      if (!hasError) {
      jj_consume_token(QQ_T);
      }
      if (!hasError) {
      s = primary();
      }
      if (!hasError) {
s1="?? ";return s1;
      }
      break;
      }
    case AND_T:
    case NAND_T:
    case NOR_T:
    case OR_T:
    case XOR_T:
    case XNOR_T:{
      if (!hasError) {
      s = logop();
      }
      if (!hasError) {
      s1 = primary();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    default:
      jj_la1[141] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::file_declaration() {QCString s,s1,s2,s3;
    if (!hasError) {
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
    case OPEN_T:{
      if (!hasError) {
      s3 = file_open_information();
      }
      break;
      }
    default:
      jj_la1[142] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
QCString t1=s2+" "+s3;
   outlineParser()->addVhdlType(s,outlineParser()->getLine(),
                                EntryType::makeVariable(),VhdlSpecifier::VFILE,QCString(),t1,Protection::Public);
   return " file "+s+":"+s2+" "+s3+";";
assert(false);
}


QCString VhdlParser::file_logical_name() {QCString s;
    if (!hasError) {
    s = expression();
    }
return s;
assert(false);
}


QCString VhdlParser::file_open_information() {QCString s,s1,s2;
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case OPEN_T:{
      if (!hasError) {
      jj_consume_token(OPEN_T);
      }
      if (!hasError) {
      s = expression();
      }
      break;
      }
    default:
      jj_la1[143] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(IS_T);
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case IN_T:
    case OUT_T:{
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case IN_T:{
        if (!hasError) {
        jj_consume_token(IN_T);
        }
        break;
        }
      case OUT_T:{
        if (!hasError) {
        jj_consume_token(OUT_T);
        }
        break;
        }
      default:
        jj_la1[144] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
      break;
      }
    default:
      jj_la1[145] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    s1 = file_logical_name();
    }
s2="open "+s+" is "+s1;  return s2;
assert(false);
}


QCString VhdlParser::file_type_definition() {QCString s,s1;
    if (!hasError) {
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


QCString VhdlParser::file_incomplete_type_definition() {QCString s,s1;
    if (!hasError) {
    jj_consume_token(FILE_T);
    }
    if (!hasError) {
    jj_consume_token(OF_T);
    }
    if (!hasError) {
    type_mark();
    }
return  "";
assert(false);
}


QCString VhdlParser::floating_incomplete_type_definition() {QCString s,s1;
    if (!hasError) {
    jj_consume_token(RANGE_T);
    }
    if (!hasError) {
    jj_consume_token(BOX_T);
    }
    if (!hasError) {
    jj_consume_token(DOT_T);
    }
    if (!hasError) {
    jj_consume_token(BOX_T);
    }
return  "range <>.<>";
assert(false);
}


QCString VhdlParser::floating_type_definition() {QCString s;
    if (!hasError) {
    s = range_constraint();
    }
return s;
assert(false);
}


QCString VhdlParser::formal_designator() {QCString s;Token *tok=nullptr;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      s = name();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    case INTEGER:{
      if (!hasError) {
      tok = jj_consume_token(INTEGER);
      }
      if (!hasError) {
return QCString(tok->image);
      }
      break;
      }
    default:
      jj_la1[146] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::formal_parameter_list() {QCString s;
    if (!hasError) {
    s = interface_list();
    }
return s;
assert(false);
}


QCString VhdlParser::formal_part() {QCString s,s1;
    if (!hasError) {
    s = name();
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LPAREN_T:{
      if (!hasError) {
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
      jj_la1[147] = jj_gen;
      ;
    }
    }
return s;
assert(false);
}


QCString VhdlParser::full_type_declaration() {std::shared_ptr<Entry> tmpEntry;QCString s,s1,s2;
    if (!hasError) {
    jj_consume_token(TYPE_T);
    }
    if (!hasError) {
    s = identifier();
    }
    if (!hasError) {
    jj_consume_token(IS_T);
    }
    if (!hasError) {
tmpEntry=m_sharedState->current;
    outlineParser()->addVhdlType(s,outlineParser()->getLine(),
                                 EntryType::makeVariable(),VhdlSpecifier::RECORD,QCString(),QCString(),Protection::Public);
    }
    if (!hasError) {
    s2 = type_definition();
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
if (s2.contains("#")) {
      VhdlDocGen::deleteAllChars(s2,'#');
      tmpEntry->vhdlSpec=VhdlSpecifier::RECORD;
      tmpEntry->type=s2;
    }
    else if (s2.contains("%")) {
      VhdlDocGen::deleteAllChars(s2,'%');
      tmpEntry->vhdlSpec=VhdlSpecifier::UNITS;
      tmpEntry->type=s2;
     }
    else {
      tmpEntry->vhdlSpec=VhdlSpecifier::TYPE;
      tmpEntry->type=s2;
    }

    tmpEntry.reset();
    return "type "+s+" is "+s2+";";
assert(false);
}


QCString VhdlParser::function_call() {QCString s,s1;
    if (!hasError) {
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


void VhdlParser::geninter() {
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case GENERIC_T:{
      if (!hasError) {
      gen_interface_list();
      }
      if (!hasError) {
      jj_consume_token(SEMI_T);
      }
      if (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case GENERIC_T:{
        if (!hasError) {
        gen_assoc_list();
        }
        if (!hasError) {
        jj_consume_token(SEMI_T);
        }
        break;
        }
      default:
        jj_la1[148] = jj_gen;
        ;
      }
      }
      break;
      }
    default:
      jj_la1[149] = jj_gen;
      ;
    }
    }
}


void VhdlParser::gen_assoc_list() {
    if (!hasError) {
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


void VhdlParser::gen_interface_list() {
    if (!hasError) {
    jj_consume_token(GENERIC_T);
    }
    if (!hasError) {
    jj_consume_token(LPAREN_T);
    }
    if (!hasError) {
m_sharedState->parse_sec=VhdlSection::GEN_SEC;
    }
    if (!hasError) {
    interface_list();
    }
    if (!hasError) {
m_sharedState->parse_sec=VhdlSection::UNKNOWN;
    }
    if (!hasError) {
    jj_consume_token(RPAREN_T);
    }
}


void VhdlParser::generate_statement_body() {
    if (!hasError) {
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
    case PACKAGE_T:
    case PROCEDURE_T:
    case PURE_T:
    case SIGNAL_T:
    case SHARED_T:
    case SUBTYPE_T:
    case TYPE_T:
    case USE_T:
    case VARIABLE_T:
    case VIEW_T:{
      if (!hasError) {
      while (!hasError) {
        if (jj_2_58(2147483647)) {
          ;
        } else {
          goto end_label_27;
        }
        if (!hasError) {
        block_declarative_item();
        }
      }
      end_label_27: ;
      }
      if (!hasError) {
      jj_consume_token(BEGIN_T);
      }
      break;
      }
    default:
      jj_la1[150] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ASSERT_T:
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
        jj_la1[151] = jj_gen;
        goto end_label_28;
      }
      if (!hasError) {
      concurrent_statement();
      }
    }
    end_label_28: ;
    }
}


void VhdlParser::generate_statement_body1() {
    if (!hasError) {
    generate_statement_body();
    }
    if (!hasError) {
    generate_scheme_1();
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case ELSE_T:{
      if (!hasError) {
      generate_scheme_2();
      }
      break;
      }
    default:
      jj_la1[152] = jj_gen;
      ;
    }
    }
}


void VhdlParser::generate_scheme_1() {
    if (!hasError) {
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ELSIF_T:{
        ;
        break;
        }
      default:
        jj_la1[153] = jj_gen;
        goto end_label_29;
      }
      if (!hasError) {
      generate_scheme_3();
      }
    }
    end_label_29: ;
    }
}


void VhdlParser::generate_scheme_2() {
    if (!hasError) {
    jj_consume_token(ELSE_T);
    }
    if (!hasError) {
    if (jj_2_59(2)) {
      if (!hasError) {
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
    jj_consume_token(GENERATE_T);
    }
    if (!hasError) {
    generate_statement_body();
    }
}


void VhdlParser::generate_scheme_3() {
    if (!hasError) {
    jj_consume_token(ELSIF_T);
    }
    if (!hasError) {
    if (jj_2_60(2)) {
      if (!hasError) {
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
    expression();
    }
    if (!hasError) {
    jj_consume_token(GENERATE_T);
    }
    if (!hasError) {
    generate_statement_body();
    }
}


void VhdlParser::generate_statement() {QCString s;
    if (!hasError) {
    s = identifier();
    }
    if (!hasError) {
    jj_consume_token(COLON_T);
    }
    if (!hasError) {
    try {
      if (!hasError) {
      generate_scheme();
      }
      if (!hasError) {
      jj_consume_token(GENERATE_T);
      }
      if (!hasError) {
outlineParser()->pushLabel(m_sharedState->genLabels,s);
      }
      if (!hasError) {
      generate_statement_body1();
      }
      if (!hasError) {
      jj_consume_token(END_T);
      }
    } catch ( ...) {
outlineParser()->error_skipto(GENERATE_T);
    }
    }
    if (!hasError) {
    jj_consume_token(GENERATE_T);
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      identifier();
      }
      break;
      }
    default:
      jj_la1[154] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
m_sharedState->genLabels=outlineParser()->popLabel(m_sharedState->genLabels);
}


void VhdlParser::generate_scheme() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case FOR_T:{
      if (!hasError) {
      jj_consume_token(FOR_T);
      }
      if (!hasError) {
      parameter_specification();
      }
      break;
      }
    case IF_T:{
      if (!hasError) {
      jj_consume_token(IF_T);
      }
      if (!hasError) {
      if (jj_2_61(2)) {
        if (!hasError) {
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
      condition();
      }
      break;
      }
    default:
      jj_la1[155] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
}


void VhdlParser::generic_clause() {QCString s;
    if (!hasError) {
    jj_consume_token(GENERIC_T);
    }
    if (!hasError) {
    jj_consume_token(LPAREN_T);
    }
    if (!hasError) {
m_sharedState->parse_sec=VhdlSection::GEN_SEC;
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
m_sharedState->parse_sec=VhdlSection::UNKNOWN;
}


QCString VhdlParser::generic_list() {QCString s;
    if (!hasError) {
    s = interface_list();
    }
return s;
assert(false);
}


void VhdlParser::generic_map_aspect() {
    if (!hasError) {
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
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      s = name();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    case CHARACTER_LITERAL:{
      if (!hasError) {
      s = character_literal();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    default:
      jj_la1[156] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::group_constituent_list() {QCString s,s1,s2;
    if (!hasError) {
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
        jj_la1[157] = jj_gen;
        goto end_label_30;
      }
      if (!hasError) {
      jj_consume_token(COMMA_T);
      }
      if (!hasError) {
      s = group_constituent();
      }
      if (!hasError) {
s2+=",";s2+=s1;
      }
    }
    end_label_30: ;
    }
return s+s2;
assert(false);
}


QCString VhdlParser::group_declaration() {QCString s,s1,s2;
    if (!hasError) {
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


QCString VhdlParser::group_template_declaration() {QCString s,s1;
    if (!hasError) {
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


void VhdlParser::guarded_signal_specification() {
    if (!hasError) {
    signal_list();
    }
    if (!hasError) {
    jj_consume_token(COLON_T);
    }
    if (!hasError) {
    type_mark();
    }
}


QCString VhdlParser::identifier() {Token *tok=nullptr;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      tok = jj_consume_token(EXTENDED_CHARACTER);
      }
      if (!hasError) {
return QCString(tok->image);
      }
      break;
      }
    case BASIC_IDENTIFIER:{
      if (!hasError) {
      tok = jj_consume_token(BASIC_IDENTIFIER);
      }
      if (!hasError) {
return QCString(tok->image);
      }
      break;
      }
    default:
      jj_la1[158] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::identifier_list() {QCString str,str1;
    if (!hasError) {
    str = name();
    }
    if (!hasError) {
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case COMMA_T:{
        ;
        break;
        }
      default:
        jj_la1[159] = jj_gen;
        goto end_label_31;
      }
      if (!hasError) {
      jj_consume_token(COMMA_T);
      }
      if (!hasError) {
      str1 = name();
      }
      if (!hasError) {
str+=",";str+=str1;
      }
    }
    end_label_31: ;
    }
    if (!hasError) {
    if (jj_2_62(1) && (checkListTok())) {
      if (!hasError) {
      jj_consume_token(SEMI_T);
      }
    } else {
      ;
    }
    }
return str;
assert(false);
}


void VhdlParser::if_statement() {QCString s,s1;
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      identifier();
      }
      if (!hasError) {
      jj_consume_token(COLON_T);
      }
      break;
      }
    default:
      jj_la1[160] = jj_gen;
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
      FlowChart::addFlowChart(FlowChart::IF_NO,QCString(),s);
    }
    if (!hasError) {
    sequence_of_statements();
    }
    if (!hasError) {
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ELSIF_T:{
        ;
        break;
        }
      default:
        jj_la1[161] = jj_gen;
        goto end_label_32;
      }
      if (!hasError) {
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
     FlowChart::addFlowChart(FlowChart::ELSIF_NO,QCString(),s1);
      }
      if (!hasError) {
      sequence_of_statements();
      }
    }
    end_label_32: ;
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case ELSE_T:{
      if (!hasError) {
      jj_consume_token(ELSE_T);
      }
      if (!hasError) {
FlowChart::addFlowChart(FlowChart::ELSE_NO,QCString(),QCString());
      }
      if (!hasError) {
      sequence_of_statements();
      }
      break;
      }
    default:
      jj_la1[162] = jj_gen;
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
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      identifier();
      }
      break;
      }
    default:
      jj_la1[163] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
FlowChart::moveToPrevLevel();
    FlowChart::addFlowChart(FlowChart::ENDIF_NO,QCString(),QCString());
}


QCString VhdlParser::incomplete_subtype_indication() {QCString s;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LPAREN_T:
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      subtype_indication();
      }
      if (!hasError) {
return "";
      }
      break;
      }
    case TYPE_T:{
      if (!hasError) {
      anonymous_type_indication();
      }
      if (!hasError) {
return "";
      }
      break;
      }
    default:
      jj_la1[164] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::incomplete_type_definition() {QCString s;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case PRIVATE_T:{
      if (!hasError) {
      private_incomplete_type_definition();
      }
      if (!hasError) {
return "";
      }
      break;
      }
    case BOX_T:{
      if (!hasError) {
      scalar_incomplete_type_definition();
      }
      if (!hasError) {
return "";
      }
      break;
      }
    case LPAREN_T:{
      if (!hasError) {
      discrete_incomplete_type_definition();
      }
      if (!hasError) {
return "";
      }
      break;
      }
    case UNITS_T:{
      if (!hasError) {
      physical_incomplete_type_definition();
      }
      if (!hasError) {
return "";
      }
      break;
      }
    default:
      jj_la1[165] = jj_gen;
      if (jj_2_63(4)) {
        if (!hasError) {
        floating_incomplete_type_definition();
        }
        if (!hasError) {
return "";
        }
      } else {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case RANGE_T:{
          if (!hasError) {
          integer_incomplete_type_definition();
          }
          if (!hasError) {
return "";
          }
          break;
          }
        case ARRAY_T:{
          if (!hasError) {
          array_incomplete_type_definition();
          }
          if (!hasError) {
return "";
          }
          break;
          }
        case ACCESS_T:{
          if (!hasError) {
          access_incomplete_type_definition();
          }
          if (!hasError) {
return "";
          }
          break;
          }
        case FILE_T:{
          if (!hasError) {
          file_incomplete_type_definition();
          }
          if (!hasError) {
return "";
          }
          break;
          }
        default:
          jj_la1[166] = jj_gen;
          jj_consume_token(-1);
          errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
        }
      }
    }
assert(false);
}


QCString VhdlParser::incomplete_type_mark() {QCString s;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      type_mark();
      }
      if (!hasError) {
return "";
      }
      break;
      }
    case TYPE_T:{
      if (!hasError) {
      anonymous_type_indication();
      }
      if (!hasError) {
return "";
      }
      break;
      }
    default:
      jj_la1[167] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::integer_incomplete_type_definition() {QCString s;
    if (!hasError) {
    jj_consume_token(RANGE_T);
    }
    if (!hasError) {
    jj_consume_token(BOX_T);
    }
return "range <>";
assert(false);
}


QCString VhdlParser::incomplete_type_declaration() {QCString s;
    if (!hasError) {
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


QCString VhdlParser::index_constraint() {QCString s("("); QCString s1,s2;
    if (!hasError) {
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
        jj_la1[168] = jj_gen;
        goto end_label_33;
      }
      if (!hasError) {
      jj_consume_token(COMMA_T);
      }
      if (!hasError) {
      s1 = discrete_range();
      }
      if (!hasError) {
s+=",";s+=s1;
      }
    }
    end_label_33: ;
    }
    if (!hasError) {
    jj_consume_token(RPAREN_T);
    }
return s+")";
assert(false);
}


QCString VhdlParser::index_specification() {QCString s;
    if (jj_2_64(2147483647)) {
      if (!hasError) {
      s = discrete_range();
      }
      if (!hasError) {
return s;
      }
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ABS_T:
      case AND_T:
      case NAND_T:
      case NEW_T:
      case NOR_T:
      case NOT_T:
      case NULL_T:
      case OR_T:
      case XOR_T:
      case XNOR_T:
      case LPAREN_T:
      case PLUS_T:
      case MINUS_T:
      case SLSL_T:
      case QQ_T:
      case INTEGER:
      case STRINGLITERAL:
      case BASIC_IDENTIFIER:
      case EXTENDED_CHARACTER:
      case CHARACTER_LITERAL:
      case DECIMAL_LITERAL:
      case BASED_LITERAL:
      case BIT_STRING_LITERAL:{
        if (!hasError) {
        s = expression();
        }
        if (!hasError) {
return s;
        }
        break;
        }
      default:
        jj_la1[169] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
assert(false);
}


QCString VhdlParser::index_subtype_definition() {QCString s;
    if (!hasError) {
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
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case COMPONENT_T:{
        if (!hasError) {
        jj_consume_token(COMPONENT_T);
        }
        break;
        }
      default:
        jj_la1[170] = jj_gen;
        ;
      }
      }
      if (!hasError) {
      s = name();
      }
      if (!hasError) {
s1="component "; return s;
      }
      break;
      }
    case ENTITY_T:{
      if (!hasError) {
      jj_consume_token(ENTITY_T);
      }
      if (!hasError) {
      if (jj_2_65(2)) {
        if (!hasError) {
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
s="entity|"+s2;
      }
      if (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case LPAREN_T:{
        if (!hasError) {
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
        jj_la1[171] = jj_gen;
        ;
      }
      }
      if (!hasError) {
return s;
      }
      break;
      }
    case CONFIGURATION_T:{
      if (!hasError) {
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
      jj_la1[172] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::instantiation_list() {QCString s;Token *tok=nullptr;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      s = identifier_list();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    case OTHER_T:{
      if (!hasError) {
      tok = jj_consume_token(OTHER_T);
      }
      if (!hasError) {
return QCString(tok->image);
      }
      break;
      }
    case ALL_T:{
      if (!hasError) {
      tok = jj_consume_token(ALL_T);
      }
      if (!hasError) {
return QCString(tok->image);
      }
      break;
      }
    default:
      jj_la1[173] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::integer() {Token *t;
    if (!hasError) {
    t = jj_consume_token(INTEGER);
    }
return QCString(t->image);
assert(false);
}


QCString VhdlParser::integer_type_definition() {QCString s;
    if (!hasError) {
    s = range_constraint();
    }
return s;
assert(false);
}


QCString VhdlParser::interface_declaration() {QCString s,s1;
    if (jj_2_66(2147483647)) {
      if (!hasError) {
      s = interface_variable_declaration();
      }
      if (!hasError) {
return s;
      }
    } else if (jj_2_67(2147483647)) {
      if (!hasError) {
      interface_object_declaration();
      }
      if (!hasError) {
return " ";
      }
    } else if (jj_2_68(2147483647)) {
      if (!hasError) {
m_sharedState->interf_sec=true;
      }
      if (!hasError) {
      s = interface_subprogram_declaration();
      }
      if (!hasError) {
m_sharedState->interf_sec=false ;return s;
      }
    } else if (jj_2_69(2147483647)) {
      if (!hasError) {
      interface_package_declaration();
      }
      if (!hasError) {
return s;
      }
    } else if (jj_2_70(5)) {
      if (!hasError) {
      s = interface_type_declaration();
      }
      if (!hasError) {
return s;
      }
    } else if (jj_2_71(2147483647)) {
      if (!hasError) {
      interface_subprogram_declaration();
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
      case VARIABLE_T:{
        if (!hasError) {
        s = object_class();
        }
        if (!hasError) {
        s1 = identifier();
        }
        if (!hasError) {
if (m_sharedState->parse_sec==VhdlSection::GEN_SEC)
     outlineParser()->addVhdlType(s,outlineParser()->getLine(),
                                  EntryType::makeVariable(),m_sharedState->currP,s1,QCString(),Protection::Public);
   return s;
        }
        break;
        }
      default:
        jj_la1[174] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
assert(false);
}


void VhdlParser::interface_object_declaration() {QCString s,s1,s2;
    if (jj_2_72(2147483647)) {
      if (!hasError) {
      interface_constant_declaration();
      }
    } else if (jj_2_73(2147483647)) {
      if (!hasError) {
      interface_signal_declaration();
      }
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case FILE_T:{
        if (!hasError) {
        interface_file_declaration();
        }
        break;
        }
      default:
        jj_la1[175] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
}


QCString VhdlParser::interface_constant_declaration() {QCString s,s1,s2;
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case CONSTANT_T:{
      if (!hasError) {
      jj_consume_token(CONSTANT_T);
      }
      break;
      }
    default:
      jj_la1[176] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    identifier_list();
    }
    if (!hasError) {
    jj_consume_token(COLON_T);
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case IN_T:{
      if (!hasError) {
      jj_consume_token(IN_T);
      }
      break;
      }
    default:
      jj_la1[177] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    interface_type_indication();
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case VARASSIGN_T:{
      if (!hasError) {
      jj_consume_token(VARASSIGN_T);
      }
      if (!hasError) {
      conditional_expression();
      }
      break;
      }
    default:
      jj_la1[178] = jj_gen;
      ;
    }
    }
return "";
assert(false);
}


QCString VhdlParser::interface_element() {QCString s;
    if (!hasError) {
    s = interface_declaration();
    }
return s;
assert(false);
}


QCString VhdlParser::interface_file_declaration() {QCString s,s1;
    if (!hasError) {
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
outlineParser()->addVhdlType(s,outlineParser()->getLine(),
                                 EntryType::makeVariable(),VhdlSpecifier::VFILE,QCString(),s1,Protection::Public);
    return QCString(" file "+s+":"+s1);
assert(false);
}


QCString VhdlParser::interface_list() {QCString s,s1,s2;
    if (!hasError) {
    s = interface_element();
    }
    if (!hasError) {
    while (!hasError) {
      if (jj_2_74(2)) {
        ;
      } else {
        goto end_label_34;
      }
      if (!hasError) {
      jj_consume_token(SEMI_T);
      }
      if (!hasError) {
      s1 = interface_element();
      }
      if (!hasError) {
s2+=";";s2+=s1;
      }
    }
    end_label_34: ;
    }
    if (!hasError) {
    if (jj_2_75(1) && (checkListTok())) {
      if (!hasError) {
      jj_consume_token(SEMI_T);
      }
    } else {
      ;
    }
    }
return s+s2;
assert(false);
}


QCString VhdlParser::interface_package_declaration() {QCString s,s1,s2;
    if (!hasError) {
    jj_consume_token(PACKAGE_T);
    }
    if (!hasError) {
    identifier();
    }
    if (!hasError) {
    jj_consume_token(IS_T);
    }
    if (!hasError) {
    jj_consume_token(NEW_T);
    }
    if (!hasError) {
    name();
    }
    if (!hasError) {
    interface_package_generic_map_aspect();
    }
return "";
assert(false);
}


QCString VhdlParser::interface_package_generic_map_aspect() {QCString s,s1,s2;
    if (jj_2_76(4)) {
      if (!hasError) {
      jj_consume_token(GENERIC_T);
      }
      if (!hasError) {
      jj_consume_token(MAP_T);
      }
      if (!hasError) {
      jj_consume_token(LPAREN_T);
      }
      if (!hasError) {
      jj_consume_token(BOX_T);
      }
      if (!hasError) {
      jj_consume_token(RPAREN_T);
      }
      if (!hasError) {
return "";
      }
    } else if (jj_2_77(4)) {
      if (!hasError) {
      jj_consume_token(GENERIC_T);
      }
      if (!hasError) {
      jj_consume_token(MAP_T);
      }
      if (!hasError) {
      jj_consume_token(LPAREN_T);
      }
      if (!hasError) {
      jj_consume_token(DEFAULT_T);
      }
      if (!hasError) {
      jj_consume_token(RPAREN_T);
      }
      if (!hasError) {
return "";
      }
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case GENERIC_T:{
        if (!hasError) {
        generic_map_aspect();
        }
        if (!hasError) {
return "";
        }
        break;
        }
      default:
        jj_la1[179] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
assert(false);
}


QCString VhdlParser::interface_procedure_specification() {QCString s,s1,s2;
    if (!hasError) {
    jj_consume_token(PROCEDURE_T);
    }
    if (!hasError) {
    designator();
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case PARAMETER_T:
    case LPAREN_T:{
      if (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case PARAMETER_T:{
        if (!hasError) {
        jj_consume_token(PARAMETER_T);
        }
        break;
        }
      default:
        jj_la1[180] = jj_gen;
        ;
      }
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
      break;
      }
    default:
      jj_la1[181] = jj_gen;
      ;
    }
    }
return "";
assert(false);
}


QCString VhdlParser::interface_signal_declaration() {QCString s,s1,s2;
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SIGNAL_T:{
      if (!hasError) {
      jj_consume_token(SIGNAL_T);
      }
      break;
      }
    default:
      jj_la1[182] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    identifier_list();
    }
    if (!hasError) {
    jj_consume_token(COLON_T);
    }
    if (!hasError) {
    interface_type_indication();
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BUS_T:{
      if (!hasError) {
      jj_consume_token(BUS_T);
      }
      break;
      }
    default:
      jj_la1[183] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case VARASSIGN_T:{
      if (!hasError) {
      jj_consume_token(VARASSIGN_T);
      }
      if (!hasError) {
      conditional_expression();
      }
      break;
      }
    default:
      jj_la1[184] = jj_gen;
      ;
    }
    }
return "";
assert(false);
}


QCString VhdlParser::interface_subprogram_declaration() {QCString s;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case PROCEDURE_T:{
      if (!hasError) {
      s = iproc();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    case FUNCTION_T:
    case IMPURE_T:
    case PURE_T:{
      if (!hasError) {
      s = ifunc();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    default:
      jj_la1[185] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::iproc() {QCString s,s1;
    if (!hasError) {
    jj_consume_token(PROCEDURE_T);
    }
    if (!hasError) {
    s = identifier();
    }
    if (!hasError) {
    s1 = param();
    }
m_sharedState->current->name=s;
    return "procedure "+s+s1;
assert(false);
}


QCString VhdlParser::ifunc() {QCString s,s1,s2,s3;Token *t=nullptr;Token *t1=nullptr;Token *t2=nullptr;
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case IMPURE_T:
    case PURE_T:{
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case PURE_T:{
        if (!hasError) {
        t = jj_consume_token(PURE_T);
        }
        break;
        }
      case IMPURE_T:{
        if (!hasError) {
        t = jj_consume_token(IMPURE_T);
        }
        break;
        }
      default:
        jj_la1[186] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
      break;
      }
    default:
      jj_la1[187] = jj_gen;
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
    case IS_T:{
      if (!hasError) {
      t1 = jj_consume_token(IS_T);
      }
      if (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case BASIC_IDENTIFIER:
      case EXTENDED_CHARACTER:{
        if (!hasError) {
        s3 = identifier();
        }
        break;
        }
      case BOX_T:{
        if (!hasError) {
        t2 = jj_consume_token(BOX_T);
        }
        break;
        }
      default:
        jj_la1[188] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
      }
      break;
      }
    default:
      jj_la1[189] = jj_gen;
      ;
    }
    }
QCString q;
   if(t!=nullptr) q=t->image;
   if(t2!=nullptr) s3="<>";
   if (!s3.isEmpty())
   {
     s3.prepend(" is ");
   }
   m_sharedState->current->name=s;
   if (m_sharedState->parse_sec==VhdlSection::GEN_SEC)
   {
     QCString ss=q+" function "+s1+" return "+s2+s3;
     int a=outlineParser()->getLine(FUNCTION_T);
     int b=outlineParser()->getLine(PROCEDURE_T);

     if (a>b) b=a;
     outlineParser()->addVhdlType(m_sharedState->current->name,b,EntryType::makeVariable(),
                                  VhdlSpecifier::GENERIC,ss,QCString(),Protection::Public);
   }
   m_sharedState->currP=VhdlSpecifier::UNKNOWN;return QCString();
assert(false);
}


QCString VhdlParser::param() {QCString s,s1;Token *tok=nullptr;
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case PARAMETER_T:{
      if (!hasError) {
      tok = jj_consume_token(PARAMETER_T);
      }
      break;
      }
    default:
      jj_la1[190] = jj_gen;
      ;
    }
    }
    if (!hasError) {
m_sharedState->param_sec=VhdlSection::PARAM_SEC;
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LPAREN_T:{
      if (!hasError) {
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
      jj_la1[191] = jj_gen;
      ;
    }
    }
if(tok!=nullptr)
    {
      s = tok->image;
    }
    m_sharedState->param_sec=VhdlSection::UNKNOWN;
    return s+"("+s1+")";
assert(false);
}


QCString VhdlParser::interface_subprogram_default() {QCString s,s1,s2;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      name();
      }
      break;
      }
    case BOX_T:{
      if (!hasError) {
      jj_consume_token(BOX_T);
      }
      break;
      }
    default:
      jj_la1[192] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::interface_subprogram_specification() {QCString s,s1,s2;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case PROCEDURE_T:{
      if (!hasError) {
      interface_procedure_specification();
      }
      if (!hasError) {
return "";
      }
      break;
      }
    case FUNCTION_T:
    case IMPURE_T:
    case PURE_T:{
      if (!hasError) {
      interface_function_specification();
      }
      if (!hasError) {
return "";
      }
      break;
      }
    default:
      jj_la1[193] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::interface_function_specification() {QCString s,s1,s2;
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case IMPURE_T:
    case PURE_T:{
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case PURE_T:{
        if (!hasError) {
        jj_consume_token(PURE_T);
        }
        break;
        }
      case IMPURE_T:{
        if (!hasError) {
        jj_consume_token(IMPURE_T);
        }
        break;
        }
      default:
        jj_la1[194] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
      break;
      }
    default:
      jj_la1[195] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(FUNCTION_T);
    }
    if (!hasError) {
    designator();
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case GENERIC_T:{
      if (!hasError) {
      subprogram_header();
      }
      break;
      }
    default:
      jj_la1[196] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case PARAMETER_T:
    case LPAREN_T:{
      if (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case PARAMETER_T:{
        if (!hasError) {
        jj_consume_token(PARAMETER_T);
        }
        break;
        }
      default:
        jj_la1[197] = jj_gen;
        ;
      }
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
      break;
      }
    default:
      jj_la1[198] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(RETURN_T);
    }
    if (!hasError) {
    if (jj_2_78(2)) {
      if (!hasError) {
      identifier();
      }
      if (!hasError) {
      jj_consume_token(OF_T);
      }
    } else {
      ;
    }
    }
    if (!hasError) {
    type_mark();
    }
return "";
assert(false);
}


QCString VhdlParser::interface_incomplete_type_declaration() {QCString s="type";QCString s1;
    if (!hasError) {
    jj_consume_token(TYPE_T);
    }
    if (!hasError) {
    s1 = identifier();
    }
if (m_sharedState->currP!=VhdlSpecifier::COMPONENT && m_sharedState->interf_sec==false)
    {
      if (m_sharedState->currP==VhdlSpecifier::FUNCTION || m_sharedState->currP==VhdlSpecifier::PROCEDURE)
      {
        outlineParser()->addProto("",s1,s,"","","");
      }
      else if(m_sharedState->parse_sec==VhdlSection::GEN_SEC)
      {
        outlineParser()->addVhdlType(s,outlineParser()->getLine(),EntryType::makeVariable(),VhdlSpecifier::GENERIC,s1,"",Protection::Public);
      }

      return s+" "+s1;
    }
assert(false);
}


QCString VhdlParser::interface_type_declaration() {QCString s;
    if (!hasError) {
    s = interface_incomplete_type_declaration();
    }
return s;
assert(false);
}


QCString VhdlParser::interface_type_indication() {QCString s,s1,s2;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LPAREN_T:
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      subtype_indication();
      }
      if (!hasError) {
return "";
      }
      break;
      }
    case TYPE_T:{
      if (!hasError) {
      anonymous_type_indication();
      }
      if (!hasError) {
return "";
      }
      break;
      }
    case VIEW_T:{
      if (!hasError) {
      mode_view_indication();
      }
      if (!hasError) {
return "";
      }
      break;
      }
    default:
      jj_la1[199] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::interface_variable_declaration() {Token *tok=nullptr;Token *tok1=nullptr;Token *tok2=nullptr;QCString s,s1,s2,s3,s4,s5;
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case CONSTANT_T:
    case SIGNAL_T:
    case SHARED_T:
    case VARIABLE_T:{
      if (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case VARIABLE_T:{
        if (!hasError) {
        tok = jj_consume_token(VARIABLE_T);
        }
        break;
        }
      case SIGNAL_T:{
        if (!hasError) {
        tok = jj_consume_token(SIGNAL_T);
        }
        break;
        }
      case CONSTANT_T:{
        if (!hasError) {
        tok = jj_consume_token(CONSTANT_T);
        }
        break;
        }
      case SHARED_T:{
        if (!hasError) {
        tok = jj_consume_token(SHARED_T);
        }
        break;
        }
      default:
        jj_la1[200] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
      }
      break;
      }
    default:
      jj_la1[201] = jj_gen;
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
    case OUT_T:{
      if (!hasError) {
      s1 = mode();
      }
      break;
      }
    default:
      jj_la1[202] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    s2 = subtype_indication();
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BUS_T:{
      if (!hasError) {
      tok1 = jj_consume_token(BUS_T);
      }
      break;
      }
    default:
      jj_la1[203] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case VARASSIGN_T:{
      if (!hasError) {
      tok2 = jj_consume_token(VARASSIGN_T);
      }
      if (!hasError) {
      s4 = expression();
      }
      break;
      }
    default:
      jj_la1[204] = jj_gen;
      ;
    }
    }
if(tok!=nullptr)
        s5=QCString(tok->image);

      if(tok1!=nullptr)
        s3=tok1->image;

      if(tok2!=nullptr)
        s3+=":=";

      QCString it=s+":"+s1+" "+s2+" "+s3+" "+s4;
      if (m_sharedState->currP!=VhdlSpecifier::COMPONENT && m_sharedState->interf_sec==false)
      {
          if (m_sharedState->currP==VhdlSpecifier::FUNCTION || m_sharedState->currP==VhdlSpecifier::PROCEDURE)
        {
          outlineParser()->addProto(s5,s,s1,s2,s3,s4);
        }
        else
        {
          QCString i=s2+s3+s4;
          outlineParser()->addVhdlType(s,outlineParser()->getLine(),
                                       EntryType::makeVariable(),m_sharedState->currP,i,s1,Protection::Public);
        }
      } // if component
      return it;
assert(false);
}


QCString VhdlParser::iteration_scheme() {QCString s;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case WHILE_T:{
      if (!hasError) {
      jj_consume_token(WHILE_T);
      }
      if (!hasError) {
      s = condition();
      }
      if (!hasError) {
s.prepend("while ");
   FlowChart::addFlowChart(FlowChart::WHILE_NO,QCString(),s,m_sharedState->lab);
   m_sharedState->lab="";
  return s;
      }
      break;
      }
    case FOR_T:{
      if (!hasError) {
      jj_consume_token(FOR_T);
      }
      if (!hasError) {
      s = parameter_specification();
      }
      if (!hasError) {
QCString q=m_sharedState->lab+" for "+s;
    FlowChart::addFlowChart(FlowChart::FOR_NO,QCString(),q,m_sharedState->lab);
    m_sharedState->lab="";
    return q;
      }
      break;
      }
    default:
      jj_la1[205] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::label() {QCString s;
    if (!hasError) {
    s = identifier();
    }
return s;
assert(false);
}


QCString VhdlParser::library_clause() {QCString s;
    if (!hasError) {
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
if ( m_sharedState->parse_sec==VhdlSection::UNKNOWN && Config_getBool(SHOW_INCLUDE_FILES) )
   {
     outlineParser()->addVhdlType(s,outlineParser()->getLine(),
                                  EntryType::makeVariable(),VhdlSpecifier::LIBRARY,s,"_library_",Protection::Public);
   }
   QCString s1="library "+s;
   return s1;
assert(false);
}


QCString VhdlParser::library_unit() {QCString s;
    if (jj_2_79(2)) {
      if (!hasError) {
      primary_unit();
      }
      if (!hasError) {
return s;
      }
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ARCHITECTURE_T:
      case PACKAGE_T:{
        if (!hasError) {
        secondary_unit();
        }
        if (!hasError) {
return s;
        }
        break;
        }
      default:
        jj_la1[206] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
assert(false);
}


QCString VhdlParser::literal() {QCString s;
    if (jj_2_80(2147483647)) {
      if (!hasError) {
      s = bit_string_literal();
      }
      if (!hasError) {
return s;
      }
    } else if (jj_2_81(2147483647)) {
      if (!hasError) {
      s = numeric_literal();
      }
      if (!hasError) {
return s;
      }
    } else if (jj_2_82(2147483647)) {
      if (!hasError) {
      s = enumeration_literal();
      }
      if (!hasError) {
return s;
      }
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case STRINGLITERAL:{
        if (!hasError) {
        s = string_literal();
        }
        if (!hasError) {
return s;
        }
        break;
        }
      case NULL_T:{
        if (!hasError) {
        jj_consume_token(NULL_T);
        }
        if (!hasError) {
return "null";
        }
        break;
        }
      default:
        jj_la1[207] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
assert(false);
}


QCString VhdlParser::logical_operator() {QCString s;
    if (!hasError) {
    s = logop();
    }
return s;
assert(false);
}


QCString VhdlParser::loop_statement() {QCString s,s1,s2,s3;
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
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
      jj_la1[208] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case FOR_T:
    case WHILE_T:{
      if (!hasError) {
      s1 = iteration_scheme();
      }
      break;
      }
    default:
      jj_la1[209] = jj_gen;
      ;
    }
    }
    if (!hasError) {
if(s1.isEmpty())
        FlowChart::addFlowChart(FlowChart::LOOP_NO,QCString(),"infinite");
    }
    if (!hasError) {
    jj_consume_token(LOOP_T);
    }
    if (!hasError) {
    s2 = sequence_of_statements();
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
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      s3 = identifier();
      }
      break;
      }
    default:
      jj_la1[210] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
QCString q = s+" loop "+s2+" end loop" +s3;
         QCString endLoop="end loop" + s3;
         FlowChart::moveToPrevLevel();
         FlowChart::addFlowChart(FlowChart::END_LOOP,endLoop,QCString());
        return q;
assert(false);
}


QCString VhdlParser::miscellaneous_operator() {Token *t=nullptr;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case DOUBLEMULT_T:{
      if (!hasError) {
      jj_consume_token(DOUBLEMULT_T);
      }
      if (!hasError) {
return "**";
      }
      break;
      }
    case ABS_T:{
      if (!hasError) {
      jj_consume_token(ABS_T);
      }
      if (!hasError) {
return "abs";
      }
      break;
      }
    case NOT_T:{
      if (!hasError) {
      jj_consume_token(NOT_T);
      }
      if (!hasError) {
return "not";
      }
      break;
      }
    default:
      jj_la1[211] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::mode() {Token *tok=nullptr;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case IN_T:{
      if (!hasError) {
      tok = jj_consume_token(IN_T);
      }
      if (!hasError) {
return "in";
      }
      break;
      }
    case OUT_T:{
      if (!hasError) {
      tok = jj_consume_token(OUT_T);
      }
      if (!hasError) {
return "out";
      }
      break;
      }
    case INOUT_T:{
      if (!hasError) {
      tok = jj_consume_token(INOUT_T);
      }
      if (!hasError) {
return "inout";
      }
      break;
      }
    case BUFFER_T:{
      if (!hasError) {
      tok = jj_consume_token(BUFFER_T);
      }
      if (!hasError) {
return "buffer";
      }
      break;
      }
    case LINKAGE_T:{
      if (!hasError) {
      tok = jj_consume_token(LINKAGE_T);
      }
      if (!hasError) {
return "linkage";
      }
      break;
      }
    default:
      jj_la1[212] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::mode_indication() {Token *tok=nullptr;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BUFFER_T:
    case IN_T:
    case INOUT_T:
    case LINKAGE_T:
    case OUT_T:
    case LPAREN_T:
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      simple_mode_indication();
      }
      if (!hasError) {
return " ";
      }
      break;
      }
    case VIEW_T:{
      if (!hasError) {
      mode_view_indication();
      }
      if (!hasError) {
return " ";
      }
      break;
      }
    default:
      jj_la1[213] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::mode_view_declaration() {Token *tok=nullptr;
    if (!hasError) {
    jj_consume_token(VIEW_T);
    }
    if (!hasError) {
    identifier();
    }
    if (!hasError) {
    jj_consume_token(OF_T);
    }
    if (!hasError) {
    subtype_indication();
    }
    if (!hasError) {
    jj_consume_token(IS_T);
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
        jj_la1[214] = jj_gen;
        goto end_label_35;
      }
      if (!hasError) {
      mode_view_element_definition();
      }
    }
    end_label_35: ;
    }
    if (!hasError) {
    jj_consume_token(END_T);
    }
    if (!hasError) {
    jj_consume_token(VIEW_T);
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      identifier();
      }
      break;
      }
    default:
      jj_la1[215] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
return "";
assert(false);
}


QCString VhdlParser::mode_view_element_definition() {Token *tok=nullptr;
    if (!hasError) {
    record_element_list();
    }
    if (!hasError) {
    jj_consume_token(COLON_T);
    }
    if (!hasError) {
    element_mode_indication();
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
return "";
assert(false);
}


QCString VhdlParser::mode_view_indication() {Token *tok=nullptr;
    if (jj_2_83(3)) {
      if (!hasError) {
      array_mode_view_indication();
      }
      if (!hasError) {
return "";
      }
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case VIEW_T:{
        if (!hasError) {
        record_mode_view_indication();
        }
        if (!hasError) {
return "";
        }
        break;
        }
      default:
        jj_la1[216] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
assert(false);
}


QCString VhdlParser::multiplying_operation() {Token *tok=nullptr;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case MULT_T:{
      if (!hasError) {
      tok = jj_consume_token(MULT_T);
      }
      if (!hasError) {
return QCString(tok->image);
      }
      break;
      }
    case SLASH_T:{
      if (!hasError) {
      tok = jj_consume_token(SLASH_T);
      }
      if (!hasError) {
return QCString(tok->image);
      }
      break;
      }
    case MOD_T:{
      if (!hasError) {
      tok = jj_consume_token(MOD_T);
      }
      if (!hasError) {
return QCString(tok->image);
      }
      break;
      }
    case REM_T:{
      if (!hasError) {
      tok = jj_consume_token(REM_T);
      }
      if (!hasError) {
return QCString(tok->image);
      }
      break;
      }
    default:
      jj_la1[217] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::name() {QCString s,s1;
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case STRINGLITERAL:{
      if (!hasError) {
      s = operator_symbol();
      }
      if (!hasError) {
      if (jj_2_84(2147483647)) {
        if (!hasError) {
        s1 = name_ext1();
        }
        if (!hasError) {
s+=s1;
        }
      } else {
        ;
      }
      }
      if (!hasError) {
return s;
      }
      break;
      }
    case SLSL_T:{
      if (!hasError) {
      s = external_name();
      }
      if (!hasError) {
      if (jj_2_85(2147483647)) {
        if (!hasError) {
        s1 = name_ext1();
        }
        if (!hasError) {
s+=s1;
        }
      } else {
        ;
      }
      }
      if (!hasError) {
return s;
      }
      break;
      }
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      s = identifier();
      }
      if (!hasError) {
      if (jj_2_86(2147483647)) {
        if (!hasError) {
        s1 = name_ext1();
        }
        if (!hasError) {
s+=s1;
        }
      } else {
        ;
      }
      }
      if (!hasError) {
return s;
      }
      break;
      }
    default:
      jj_la1[218] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
    }
assert(false);
}


QCString VhdlParser::name_ext1() {QCString s,s1,s2;
    if (!hasError) {
    s = name_ext();
    }
    if (!hasError) {
    while (!hasError) {
      if (jj_2_87(2147483647)) {
        ;
      } else {
        goto end_label_36;
      }
      if (!hasError) {
      s1 = name_ext();
      }
      if (!hasError) {
s+=s1;
      }
    }
    end_label_36: ;
    }
return s;
assert(false);
}


QCString VhdlParser::name_ext() {QCString s,s1,s2;
    if (!hasError) {
    if (jj_2_88(2147483647)) {
      if (!hasError) {
      jj_consume_token(APOSTROPHE_T);
      }
      if (!hasError) {
      jj_consume_token(SUBTYPE_T);
      }
      if (!hasError) {
s+="'subtype"; return s;
      }
    } else if (jj_2_89(2147483647)) {
      if (!hasError) {
      jj_consume_token(DOT_T);
      }
      if (!hasError) {
      s1 = suffix();
      }
      if (!hasError) {
s+=".";s+=s1; return s;
      }
    } else if (jj_2_90(2147483647)) {
      if (!hasError) {
      s1 = test_att_name();
      }
      if (!hasError) {
s+=s1;return s;
      }
    } else if (jj_2_91(2147483647)) {
      if (!hasError) {
      jj_consume_token(LPAREN_T);
      }
      if (!hasError) {
      s1 = discrete_range();
      }
      if (!hasError) {
      jj_consume_token(RPAREN_T);
      }
      if (!hasError) {
s+="(";s+=s1;s+=")";return s;
      }
    } else if (jj_2_92(2147483647)) {
      if (!hasError) {
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
          jj_la1[219] = jj_gen;
          goto end_label_37;
        }
        if (!hasError) {
        jj_consume_token(COMMA_T);
        }
        if (!hasError) {
        s1 = expression();
        }
        if (!hasError) {
s+=",";s+=s1;
        }
      }
      end_label_37: ;
      }
      if (!hasError) {
      jj_consume_token(RPAREN_T);
      }
      if (!hasError) {
s+=")";return s;
      }
    } else {
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
    }
assert(false);
}


QCString VhdlParser::test_att_name() {QCString s,s1;
    if (jj_2_94(4)) {
      if (!hasError) {
      jj_consume_token(CHARACTER_LITERAL);
      }
      if (!hasError) {
      jj_consume_token(INTEGER);
      }
      if (!hasError) {
      jj_consume_token(APOSTROPHE_T);
      }
      if (!hasError) {
      jj_consume_token(RPAREN_T);
      }
      if (!hasError) {
return "kkk";
      }
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case APOSTROPHE_T:
      case LBRACKET_T:{
        if (!hasError) {
        if (jj_2_93(2147483647)) {
          if (!hasError) {
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
        case LPAREN_T:{
          if (!hasError) {
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
          jj_la1[220] = jj_gen;
          ;
        }
        }
        if (!hasError) {
return s;
        }
        break;
        }
      default:
        jj_la1[221] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
assert(false);
}


QCString VhdlParser::indexed_name() {QCString s,s1,s2;
    if (!hasError) {
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
        jj_la1[222] = jj_gen;
        goto end_label_38;
      }
      if (!hasError) {
      jj_consume_token(COMMA_T);
      }
      if (!hasError) {
      s1 = expression();
      }
      if (!hasError) {
s+=",";s+=s1;
      }
    }
    end_label_38: ;
    }
    if (!hasError) {
    jj_consume_token(RPAREN_T);
    }
return s+")";
assert(false);
}


QCString VhdlParser::next_statement() {QCString s,s1,s2;Token *t=nullptr;Token *t1=nullptr;
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      s = identifier();
      }
      if (!hasError) {
      t = jj_consume_token(COLON_T);
      }
      break;
      }
    default:
      jj_la1[223] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(NEXT_T);
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      s1 = identifier();
      }
      break;
      }
    default:
      jj_la1[224] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case WHEN_T:{
      if (!hasError) {
      t1 = jj_consume_token(WHEN_T);
      }
      if (!hasError) {
      s2 = condition();
      }
      break;
      }
    default:
      jj_la1[225] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
if(t!=nullptr) s+=":";
   FlowChart::addFlowChart(FlowChart::NEXT_NO,"next ",s2,s1);
    m_sharedState->lab.clear();
  if(t1!=nullptr) s2.prepend("when ");
  return s+s1+s2+";";
assert(false);
}


QCString VhdlParser::null_statement() {QCString s;
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
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
      jj_la1[226] = jj_gen;
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
    if (jj_2_95(2147483647)) {
      if (!hasError) {
      s = physical_literal();
      }
      if (!hasError) {
return s;
      }
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case INTEGER:
      case DECIMAL_LITERAL:
      case BASED_LITERAL:{
        if (!hasError) {
        s = abstract_literal();
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
}


QCString VhdlParser::object_class() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case CONSTANT_T:{
      if (!hasError) {
      jj_consume_token(CONSTANT_T);
      }
      if (!hasError) {
return "constant";
      }
      break;
      }
    case SIGNAL_T:{
      if (!hasError) {
      jj_consume_token(SIGNAL_T);
      }
      if (!hasError) {
return "signal";
      }
      break;
      }
    case VARIABLE_T:{
      if (!hasError) {
      jj_consume_token(VARIABLE_T);
      }
      if (!hasError) {
return "variable";
      }
      break;
      }
    case SHARED_T:{
      if (!hasError) {
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
    case FILE_T:{
      if (!hasError) {
      jj_consume_token(FILE_T);
      }
      if (!hasError) {
return "file";
      }
      break;
      }
    case TYPE_T:{
      if (!hasError) {
      jj_consume_token(TYPE_T);
      }
      if (!hasError) {
return "type";
      }
      break;
      }
    default:
      jj_la1[228] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::operator_symbol() {Token *tok=nullptr;
    if (!hasError) {
    tok = jj_consume_token(STRINGLITERAL);
    }
return QCString(tok->image);
assert(false);
}


void VhdlParser::options() {
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case GUARDED_T:{
      if (!hasError) {
      jj_consume_token(GUARDED_T);
      }
      break;
      }
    default:
      jj_la1[229] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case INERTIAL_T:
    case REJECT_T:
    case TRANSPORT_T:{
      if (!hasError) {
      delay_mechanism();
      }
      break;
      }
    default:
      jj_la1[230] = jj_gen;
      ;
    }
    }
}


void VhdlParser::package_body() {QCString s;
    if (!hasError) {
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
m_sharedState->lastCompound=m_sharedState->current;
                        s.prepend("_");
                        outlineParser()->addVhdlType(s,outlineParser()->getLine(),
                                          EntryType::makeClass(),VhdlSpecifier::PACKAGE_BODY,QCString(),QCString(),Protection::Protected);
    }
    if (!hasError) {
    package_body_declarative_part();
    }
    if (!hasError) {
    jj_consume_token(END_T);
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case PACKAGE_T:{
      if (!hasError) {
      jj_consume_token(PACKAGE_T);
      }
      if (!hasError) {
      jj_consume_token(BODY_T);
      }
      break;
      }
    default:
      jj_la1[231] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      name();
      }
      break;
      }
    default:
      jj_la1[232] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
m_sharedState->lastCompound=nullptr; m_sharedState->genLabels.clear();
}


void VhdlParser::package_body_declarative_item() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case FUNCTION_T:
    case IMPURE_T:
    case PROCEDURE_T:
    case PURE_T:{
      if (!hasError) {
      subprogram_declaration();
      }
      break;
      }
    case TYPE_T:{
      if (!hasError) {
      type_declaration();
      }
      break;
      }
    case SUBTYPE_T:{
      if (!hasError) {
      subtype_declaration();
      }
      break;
      }
    case CONSTANT_T:{
      if (!hasError) {
      constant_declaration();
      }
      break;
      }
    case SHARED_T:
    case VARIABLE_T:{
      if (!hasError) {
      variable_declaration();
      }
      break;
      }
    default:
      jj_la1[233] = jj_gen;
      if (jj_2_96(2)) {
        if (!hasError) {
        package_body();
        }
      } else if (jj_2_97(4)) {
        if (!hasError) {
        package_instantiation_declaration();
        }
      } else {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case PACKAGE_T:{
          if (!hasError) {
          package_declaration();
          }
          break;
          }
        case FILE_T:{
          if (!hasError) {
          file_declaration();
          }
          break;
          }
        case ALIAS_T:{
          if (!hasError) {
          alias_declaration();
          }
          break;
          }
        case VIEW_T:{
          if (!hasError) {
          mode_view_declaration();
          }
          break;
          }
        case USE_T:{
          if (!hasError) {
          use_clause();
          }
          break;
          }
        default:
          jj_la1[234] = jj_gen;
          if (jj_2_98(3)) {
            if (!hasError) {
            group_template_declaration();
            }
          } else {
            switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
            case GROUP_T:{
              if (!hasError) {
              group_declaration();
              }
              break;
              }
            default:
              jj_la1[235] = jj_gen;
              jj_consume_token(-1);
              errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
            }
          }
        }
      }
    }
}


void VhdlParser::package_body_declarative_part() {
    if (!hasError) {
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ALIAS_T:
      case CONSTANT_T:
      case FILE_T:
      case FUNCTION_T:
      case GROUP_T:
      case IMPURE_T:
      case PACKAGE_T:
      case PROCEDURE_T:
      case PURE_T:
      case SHARED_T:
      case SUBTYPE_T:
      case TYPE_T:
      case USE_T:
      case VARIABLE_T:
      case VIEW_T:{
        ;
        break;
        }
      default:
        jj_la1[236] = jj_gen;
        goto end_label_39;
      }
      if (!hasError) {
      package_body_declarative_item();
      }
    }
    end_label_39: ;
    }
}


void VhdlParser::package_header() {QCString s;
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case GENERIC_T:{
      if (!hasError) {
      generic_clause();
      }
      if (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case GENERIC_T:{
        if (!hasError) {
        generic_map_aspect();
        }
        if (!hasError) {
        jj_consume_token(SEMI_T);
        }
        break;
        }
      default:
        jj_la1[237] = jj_gen;
        ;
      }
      }
      break;
      }
    default:
      jj_la1[238] = jj_gen;
      ;
    }
    }
}


void VhdlParser::package_declaration() {QCString s;
    if (!hasError) {
    jj_consume_token(PACKAGE_T);
    }
    if (!hasError) {
    s = identifier();
    }
    if (!hasError) {
    jj_consume_token(IS_T);
    }
    if (!hasError) {
m_sharedState->lastCompound=m_sharedState->current;
                          std::shared_ptr<Entry> clone=std::make_shared<Entry>(*m_sharedState->current);
                          clone->section=EntryType::makeNamespace();
                          clone->vhdlSpec=VhdlSpecifier::PACKAGE;
                          clone->name=s;
                          clone->startLine=outlineParser()->getLine(PACKAGE_T);
                          clone->bodyLine=outlineParser()->getLine(PACKAGE_T);
                          clone->protection=Protection::Package;
                          m_sharedState->current_root->moveToSubEntryAndKeep(clone);
                          outlineParser()->addVhdlType(s,outlineParser()->getLine(PACKAGE_T),
                                            EntryType::makeClass(),VhdlSpecifier::PACKAGE,QCString(),QCString(),Protection::Package);
    }
    if (!hasError) {
    package_header();
    }
    if (!hasError) {
    package_declarative_part();
    }
    if (!hasError) {
    jj_consume_token(END_T);
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case PACKAGE_T:{
      if (!hasError) {
      jj_consume_token(PACKAGE_T);
      }
      break;
      }
    default:
      jj_la1[239] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      name();
      }
      break;
      }
    default:
      jj_la1[240] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
m_sharedState->lastEntity=nullptr;m_sharedState->lastCompound=nullptr; m_sharedState->genLabels.clear();
}


void VhdlParser::package_declarative_item() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case FUNCTION_T:
    case IMPURE_T:
    case PROCEDURE_T:
    case PURE_T:{
      if (!hasError) {
      subprogram_declaration();
      }
      break;
      }
    case TYPE_T:{
      if (!hasError) {
      type_declaration();
      }
      break;
      }
    case SUBTYPE_T:{
      if (!hasError) {
      subtype_declaration();
      }
      break;
      }
    default:
      jj_la1[241] = jj_gen;
      if (jj_2_99(5)) {
        if (!hasError) {
        package_instantiation_declaration();
        }
      } else if (jj_2_100(3)) {
        if (!hasError) {
        package_declaration();
        }
      } else {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case CONSTANT_T:{
          if (!hasError) {
          constant_declaration();
          }
          break;
          }
        case SIGNAL_T:{
          if (!hasError) {
          signal_declaration();
          }
          break;
          }
        case SHARED_T:
        case VARIABLE_T:{
          if (!hasError) {
          variable_declaration();
          }
          break;
          }
        case FILE_T:{
          if (!hasError) {
          file_declaration();
          }
          break;
          }
        case ALIAS_T:{
          if (!hasError) {
          alias_declaration();
          }
          break;
          }
        case COMPONENT_T:{
          if (!hasError) {
          component_declaration();
          }
          break;
          }
        case VIEW_T:{
          if (!hasError) {
          mode_view_declaration();
          }
          break;
          }
        default:
          jj_la1[242] = jj_gen;
          if (jj_2_101(2147483647)) {
            if (!hasError) {
            attribute_declaration();
            }
          } else {
            switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
            case ATTRIBUTE_T:{
              if (!hasError) {
              attribute_specification();
              }
              break;
              }
            case DISCONNECT_T:{
              if (!hasError) {
              disconnection_specification();
              }
              break;
              }
            case USE_T:{
              if (!hasError) {
              use_clause();
              }
              break;
              }
            default:
              jj_la1[243] = jj_gen;
              if (jj_2_102(3)) {
                if (!hasError) {
                group_template_declaration();
                }
              } else {
                switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
                case GROUP_T:{
                  if (!hasError) {
                  group_declaration();
                  }
                  break;
                  }
                default:
                  jj_la1[244] = jj_gen;
                  if (jj_2_103(5)) {
                    if (!hasError) {
                    package_instantiation_declaration();
                    }
                  } else {
                    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
                    case PACKAGE_T:{
                      if (!hasError) {
                      package_declaration();
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
        }
      }
    }
}


void VhdlParser::package_declarative_part() {
    if (!hasError) {
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
      case VARIABLE_T:
      case VIEW_T:{
        ;
        break;
        }
      default:
        jj_la1[246] = jj_gen;
        goto end_label_40;
      }
      if (!hasError) {
      package_declarative_item();
      }
    }
    end_label_40: ;
    }
}


void VhdlParser::package_instantiation_declaration() {QCString s,s1,s2;
    if (!hasError) {
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
    case GENERIC_T:{
      if (!hasError) {
      gen_assoc_list();
      }
      break;
      }
    default:
      jj_la1[247] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
QCString q=" is new "+s1+s2;
      outlineParser()->addVhdlType(s,outlineParser()->getLine(PACKAGE_T),
                                   EntryType::makeVariable(),VhdlSpecifier::INSTANTIATION,"package",q,Protection::Public);
}


QCString VhdlParser::parameter_specification() {QCString s,s1;
    if (!hasError) {
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


QCString VhdlParser::pathname_element() {QCString s,s1;
    if (!hasError) {
    s = identifier();
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LPAREN_T:{
      if (!hasError) {
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
      jj_la1[248] = jj_gen;
      ;
    }
    }
if(!s1.isEmpty())
     return s+"("+s1+")";

    return s;
assert(false);
}


QCString VhdlParser::pathname_element_list() {QCString s,s1,s2;
    if (!hasError) {
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
      if (jj_2_104(2147483647)) {
        ;
      } else {
        goto end_label_41;
      }
      if (!hasError) {
      s1 = pathname_element();
      }
      if (!hasError) {
      jj_consume_token(DOT_T);
      }
      if (!hasError) {
s2+=s1;s2+=".";
      }
    }
    end_label_41: ;
    }
return s+s2;
assert(false);
}


QCString VhdlParser::package_path_name() {QCString s;
    if (!hasError) {
    jj_consume_token(AT_T);
    }
    if (!hasError) {
    s = name();
    }
return "@"+s;
assert(false);
}


QCString VhdlParser::physical_literal() {QCString s,s1;
    if (!hasError) {
    if (jj_2_105(2147483647)) {
      if (!hasError) {
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


QCString VhdlParser::physical_type_definition() {QCString s,s1,s2;Token *t=nullptr;
    if (!hasError) {
    t = jj_consume_token(UNITS_T);
    }
    if (!hasError) {
    s = identifier();
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
    if (!hasError) {
outlineParser()->addVhdlType(s,t->beginLine,
                                                  EntryType::makeVariable(),VhdlSpecifier::UNITS,
                                                  QCString(),QCString(),Protection::Public);
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
        jj_la1[249] = jj_gen;
        goto end_label_42;
      }
      if (!hasError) {
      s1 = secondary_unit_declaration();
      }
    }
    end_label_42: ;
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
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      name();
      }
      break;
      }
    default:
      jj_la1[250] = jj_gen;
      ;
    }
    }
return s;
assert(false);
}


void VhdlParser::physical_incomplete_type_definition() {
    if (!hasError) {
    jj_consume_token(UNITS_T);
    }
    if (!hasError) {
    jj_consume_token(BOX_T);
    }
}


void VhdlParser::port_clause() {
    if (!hasError) {
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
m_sharedState->currP=VhdlSpecifier::UNKNOWN;
}


QCString VhdlParser::port_list() {QCString s;
    if (!hasError) {
    s = interface_list();
    }
return s;
assert(false);
}


void VhdlParser::port_map_aspect() {
    if (!hasError) {
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
    if (jj_2_106(2147483647)) {
      if (!hasError) {
      s = function_call();
      }
      if (!hasError) {
return s;
      }
    } else if (jj_2_107(2147483647)) {
      if (!hasError) {
      jj_consume_token(LPAREN_T);
      }
      if (!hasError) {
      s1 = conditional_expression();
      }
      if (!hasError) {
      jj_consume_token(RPAREN_T);
      }
      if (!hasError) {
s="("+s1+")"; return s;
      }
    } else if (jj_2_108(2147483647)) {
      if (!hasError) {
      s = qualified_expression();
      }
      if (!hasError) {
return s;
      }
    } else if (jj_2_109(2147483647)) {
      if (!hasError) {
      s = type_conversion();
      }
      if (!hasError) {
return s;
      }
    } else if (jj_2_110(2147483647)) {
      if (!hasError) {
      s = literal();
      }
      if (!hasError) {
s.prepend(" ");return s;
      }
    } else if (jj_2_111(2147483647)) {
      if (!hasError) {
      s = name();
      }
      if (!hasError) {
return s;
      }
    } else if (jj_2_112(2147483647)) {
      if (!hasError) {
      allocator();
      }
      if (!hasError) {
return QCString();
      }
    } else if (jj_2_113(2147483647)) {
      if (!hasError) {
      s = aggregate();
      }
      if (!hasError) {
return s;
      }
    } else {
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


void VhdlParser::primary_unit() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case ENTITY_T:{
      if (!hasError) {
      entity_declaration();
      }
      break;
      }
    case CONFIGURATION_T:{
      if (!hasError) {
      configuration_declaration();
      }
      break;
      }
    default:
      jj_la1[251] = jj_gen;
      if (jj_2_114(2147483647)) {
        if (!hasError) {
        package_instantiation_declaration();
        }
      } else {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case PACKAGE_T:{
          if (!hasError) {
          package_declaration();
          }
          break;
          }
        case CONTEXT_T:{
          if (!hasError) {
          context_declaration();
          }
          break;
          }
        default:
          jj_la1[252] = jj_gen;
          jj_consume_token(-1);
          errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
        }
      }
    }
}


QCString VhdlParser::private_variable_declaration() {QCString s,s1;
    if (!hasError) {
    jj_consume_token(PRIVATE_T);
    }
    if (!hasError) {
    variable_declaration();
    }
return "private";
assert(false);
}


QCString VhdlParser::private_incomplete_type_definition() {QCString s,s1;
    if (!hasError) {
    jj_consume_token(PRIVATE_T);
    }
return "private";
assert(false);
}


QCString VhdlParser::parameter_map_aspect() {QCString s,s1;
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case PARAMETER_T:{
      if (!hasError) {
      jj_consume_token(PARAMETER_T);
      }
      if (!hasError) {
      jj_consume_token(MAP_T);
      }
      break;
      }
    default:
      jj_la1[253] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(LPAREN_T);
    }
    if (!hasError) {
    actual_parameter_part();
    }
    if (!hasError) {
    jj_consume_token(RPAREN_T);
    }
assert(false);
}


QCString VhdlParser::procedure_call() {QCString s,s1;
    if (!hasError) {
    name();
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case PARAMETER_T:{
      if (!hasError) {
      jj_consume_token(PARAMETER_T);
      }
      if (!hasError) {
      jj_consume_token(MAP_T);
      }
      break;
      }
    default:
      jj_la1[254] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LPAREN_T:{
      if (!hasError) {
      jj_consume_token(LPAREN_T);
      }
      if (!hasError) {
      actual_parameter_part();
      }
      if (!hasError) {
      jj_consume_token(RPAREN_T);
      }
      break;
      }
    default:
      jj_la1[255] = jj_gen;
      ;
    }
    }
return "";
assert(false);
}


QCString VhdlParser::procedure_call_statement() {QCString s,s1;
    if (!hasError) {
    if (jj_2_115(2)) {
      if (!hasError) {
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


QCString VhdlParser::procedure_specifiction() {QCString s,s1;
    if (!hasError) {
    jj_consume_token(PROCEDURE_T);
    }
    if (!hasError) {
    designator();
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case GENERIC_T:{
      if (!hasError) {
      subprogram_header();
      }
      break;
      }
    default:
      jj_la1[256] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case PARAMETER_T:
    case LPAREN_T:{
      if (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case PARAMETER_T:{
        if (!hasError) {
        jj_consume_token(PARAMETER_T);
        }
        break;
        }
      default:
        jj_la1[257] = jj_gen;
        ;
      }
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
      break;
      }
    default:
      jj_la1[258] = jj_gen;
      ;
    }
    }
return s+s1+";";
assert(false);
}


QCString VhdlParser::process_declarative_item() {QCString s;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case FUNCTION_T:
    case IMPURE_T:
    case PROCEDURE_T:
    case PURE_T:{
      if (!hasError) {
      subprogram_declaration();
      }
      if (!hasError) {
return QCString();
      }
      break;
      }
    case IS_T:{
      if (!hasError) {
      subprogram_body();
      }
      break;
      }
    default:
      jj_la1[259] = jj_gen;
      if (jj_2_116(2)) {
        if (!hasError) {
        package_body();
        }
        if (!hasError) {
return "";
        }
      } else if (jj_2_117(2147483647)) {
        if (!hasError) {
        package_declaration();
        }
        if (!hasError) {
return "";
        }
      } else {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case PACKAGE_T:{
          if (!hasError) {
          package_instantiation_declaration();
          }
          if (!hasError) {
return "";
          }
          break;
          }
        case SUBTYPE_T:{
          if (!hasError) {
          s = subtype_declaration();
          }
          if (!hasError) {
return s;
          }
          break;
          }
        case TYPE_T:{
          if (!hasError) {
          s = type_declaration();
          }
          if (!hasError) {
return s;
          }
          break;
          }
        case CONSTANT_T:{
          if (!hasError) {
          s = constant_declaration();
          }
          if (!hasError) {
return s;
          }
          break;
          }
        case SHARED_T:
        case VARIABLE_T:{
          if (!hasError) {
          s = variable_declaration();
          }
          if (!hasError) {
return s;
          }
          break;
          }
        case FILE_T:{
          if (!hasError) {
          s = file_declaration();
          }
          if (!hasError) {
return s;
          }
          break;
          }
        case ALIAS_T:{
          if (!hasError) {
          s = alias_declaration();
          }
          if (!hasError) {
return s;
          }
          break;
          }
        default:
          jj_la1[260] = jj_gen;
          if (jj_2_118(3)) {
            if (!hasError) {
            s = attribute_declaration();
            }
            if (!hasError) {
return s;
            }
          } else {
            switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
            case ATTRIBUTE_T:{
              if (!hasError) {
              s = attribute_specification();
              }
              if (!hasError) {
return s;
              }
              break;
              }
            case USE_T:{
              if (!hasError) {
              s = use_clause();
              }
              if (!hasError) {
return s;
              }
              break;
              }
            default:
              jj_la1[261] = jj_gen;
              if (jj_2_119(3)) {
                if (!hasError) {
                s = group_template_declaration();
                }
                if (!hasError) {
return s;
                }
              } else {
                switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
                case GROUP_T:{
                  if (!hasError) {
                  s = group_declaration();
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
              }
            }
          }
        }
      }
    }
assert(false);
}


QCString VhdlParser::process_declarative_part() {QCString s,s1;
    if (!hasError) {
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
      case PACKAGE_T:
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
        jj_la1[263] = jj_gen;
        goto end_label_43;
      }
      if (!hasError) {
      s1 = process_declarative_item();
      }
      if (!hasError) {
s+=s1;
      }
    }
    end_label_43: ;
    }
return s;
assert(false);
}


void VhdlParser::process_statement() {QCString s,s1,s2;Token *tok=nullptr;Token *tok1=nullptr;
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      s = identifier();
      }
      if (!hasError) {
      jj_consume_token(COLON_T);
      }
      break;
      }
    default:
      jj_la1[264] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case POSTPONED_T:{
      if (!hasError) {
      jj_consume_token(POSTPONED_T);
      }
      break;
      }
    default:
      jj_la1[265] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    tok1 = jj_consume_token(PROCESS_T);
    }
    if (!hasError) {
m_sharedState->currP=VhdlSpecifier::PROCESS;
    m_sharedState->current->startLine=tok1->beginLine;
    m_sharedState->current->bodyLine=tok1->beginLine;
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LPAREN_T:{
      if (!hasError) {
      jj_consume_token(LPAREN_T);
      }
      if (!hasError) {
      if (!hasError) {
      s1 = sensitivity_list();
      }
      }
      if (!hasError) {
      jj_consume_token(RPAREN_T);
      }
      break;
      }
    default:
      jj_la1[266] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case IS_T:{
      if (!hasError) {
      jj_consume_token(IS_T);
      }
      break;
      }
    default:
      jj_la1[267] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    s2 = process_declarative_part();
    }
    if (!hasError) {
if (!s2.isEmpty())
                FlowChart::addFlowChart(FlowChart::VARIABLE_NO,s2,QCString());
                FlowChart::addFlowChart(FlowChart::BEGIN_NO,"BEGIN",QCString());
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
    case POSTPONED_T:{
      if (!hasError) {
      jj_consume_token(POSTPONED_T);
      }
      break;
      }
    default:
      jj_la1[268] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(PROCESS_T);
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      identifier();
      }
      break;
      }
    default:
      jj_la1[269] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
if(s.isEmpty())
            m_sharedState->currName=VhdlDocGen::getProcessNumber();
          else
            m_sharedState->currName=s;
          m_sharedState->current->name=m_sharedState->currName;
          m_sharedState->tempEntry=m_sharedState->current;
                  m_sharedState->tempEntry->type="";
          m_sharedState->current->endBodyLine=outlineParser()->getLine();
          m_sharedState->currP=VhdlSpecifier::UNKNOWN;
          if(tok!=nullptr)
            s1=tok->image;
          outlineParser()->createFunction(m_sharedState->currName,VhdlSpecifier::PROCESS,s1);
          outlineParser()->createFlow();
          m_sharedState->currName="";
          outlineParser()->newEntry();
}


void VhdlParser::process_statement_part() {
    if (!hasError) {
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ASSERT_T:
      case BLOCK_T:
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
        jj_la1[270] = jj_gen;
        goto end_label_44;
      }
      if (!hasError) {
      sequential_statement();
      }
    }
    end_label_44: ;
    }
}


QCString VhdlParser::protected_type_body() {
    if (!hasError) {
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
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      identifier();
      }
      break;
      }
    default:
      jj_la1[271] = jj_gen;
      ;
    }
    }
return QCString();
assert(false);
}


void VhdlParser::protected_type_body_declarative_item() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case FUNCTION_T:
    case IMPURE_T:
    case PROCEDURE_T:
    case PURE_T:{
      if (!hasError) {
      subprogram_declaration();
      }
      break;
      }
    case IS_T:{
      if (!hasError) {
      subprogram_body();
      }
      break;
      }
    default:
      jj_la1[272] = jj_gen;
      if (jj_2_120(2)) {
        if (!hasError) {
        package_body();
        }
      } else if (jj_2_121(2147483647)) {
        if (!hasError) {
        package_declaration();
        }
      } else {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case PACKAGE_T:{
          if (!hasError) {
          package_instantiation_declaration();
          }
          break;
          }
        case TYPE_T:{
          if (!hasError) {
          type_declaration();
          }
          break;
          }
        case SUBTYPE_T:{
          if (!hasError) {
          subtype_declaration();
          }
          break;
          }
        case CONSTANT_T:{
          if (!hasError) {
          constant_declaration();
          }
          break;
          }
        case SHARED_T:
        case VARIABLE_T:{
          if (!hasError) {
          variable_declaration();
          }
          break;
          }
        case FILE_T:{
          if (!hasError) {
          file_declaration();
          }
          break;
          }
        case ALIAS_T:{
          if (!hasError) {
          alias_declaration();
          }
          break;
          }
        default:
          jj_la1[273] = jj_gen;
          if (jj_2_122(2147483647)) {
            if (!hasError) {
            attribute_declaration();
            }
          } else {
            switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
            case ATTRIBUTE_T:{
              if (!hasError) {
              attribute_specification();
              }
              break;
              }
            case USE_T:{
              if (!hasError) {
              use_clause();
              }
              break;
              }
            default:
              jj_la1[274] = jj_gen;
              if (jj_2_123(3)) {
                if (!hasError) {
                group_template_declaration();
                }
              } else {
                switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
                case GROUP_T:{
                  if (!hasError) {
                  group_declaration();
                  }
                  break;
                  }
                default:
                  jj_la1[275] = jj_gen;
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


void VhdlParser::protected_type_body_declarative_part() {
    if (!hasError) {
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
      case PACKAGE_T:
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
        jj_la1[276] = jj_gen;
        goto end_label_45;
      }
      if (!hasError) {
      protected_type_body_declarative_item();
      }
    }
    end_label_45: ;
    }
}


QCString VhdlParser::protected_type_declaration() {
    if (!hasError) {
    jj_consume_token(PROTECTED_T);
    }
    if (!hasError) {
    try {
      if (!hasError) {
      protected_type_header();
      }
      if (!hasError) {
      protected_type_declarative_part();
      }
    } catch ( ...) {
outlineParser()->error_skipto(END_T);
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
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      identifier();
      }
      break;
      }
    default:
      jj_la1[277] = jj_gen;
      ;
    }
    }
return QCString();
assert(false);
}


void VhdlParser::protected_type_header() {
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case GENERIC_T:{
      if (!hasError) {
      generic_clause();
      }
      if (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case GENERIC_T:{
        if (!hasError) {
        generic_map_aspect();
        }
        break;
        }
      default:
        jj_la1[278] = jj_gen;
        ;
      }
      }
      break;
      }
    default:
      jj_la1[279] = jj_gen;
      ;
    }
    }
}


void VhdlParser::protected_type_declarative_item() {
    if (jj_2_124(2147483647)) {
      if (!hasError) {
      subprogram_declaration();
      }
    } else if (jj_2_125(4)) {
      if (!hasError) {
      alias_declaration();
      }
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case FUNCTION_T:{
        if (!hasError) {
        subprogram_instantiation_declaration();
        }
        break;
        }
      case ATTRIBUTE_T:{
        if (!hasError) {
        attribute_specification();
        }
        break;
        }
      case PRIVATE_T:{
        if (!hasError) {
        private_variable_declaration();
        }
        break;
        }
      case USE_T:{
        if (!hasError) {
        use_clause();
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


void VhdlParser::protected_type_declarative_part() {
    if (!hasError) {
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ALIAS_T:
      case ATTRIBUTE_T:
      case FUNCTION_T:
      case IMPURE_T:
      case PRIVATE_T:
      case PROCEDURE_T:
      case PURE_T:
      case USE_T:{
        ;
        break;
        }
      default:
        jj_la1[281] = jj_gen;
        goto end_label_46;
      }
      if (!hasError) {
      protected_type_declarative_item();
      }
    }
    end_label_46: ;
    }
}


QCString VhdlParser::qualified_expression() {QCString s,s1;Token *tok=nullptr;
    if (jj_2_127(4)) {
      if (!hasError) {
      s1 = identifier();
      }
      if (!hasError) {
      jj_consume_token(CHARACTER_LITERAL);
      }
      if (!hasError) {
      tok = jj_consume_token(LETTER_OR_DIGIT);
      }
      if (!hasError) {
      jj_consume_token(APOSTROPHE_T);
      }
      if (!hasError) {
      jj_consume_token(RPAREN_T);
      }
      if (!hasError) {
return s1+"'('"+QCString(tok->image)+"')";
      }
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case BASIC_IDENTIFIER:
      case EXTENDED_CHARACTER:{
        if (!hasError) {
        s1 = identifier();
        }
        if (!hasError) {
        jj_consume_token(APOSTROPHE_T);
        }
        if (!hasError) {
s=s1+"'";
        }
        if (!hasError) {
        if (jj_2_126(2147483647)) {
          if (!hasError) {
          s1 = aggregate();
          }
          if (!hasError) {
s+=s1;
          }
        } else {
          switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
          case LPAREN_T:{
            if (!hasError) {
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
            jj_la1[282] = jj_gen;
            jj_consume_token(-1);
            errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
          }
        }
        }
        if (!hasError) {
return s;
        }
        break;
        }
      default:
        jj_la1[283] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
assert(false);
}


QCString VhdlParser::range() {QCString s,s1,s2;
    if (jj_2_128(2147483647)) {
      if (!hasError) {
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
    } else if (jj_2_129(2147483647)) {
      if (!hasError) {
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
}


QCString VhdlParser::range_constraint() {QCString s,s1;
    if (!hasError) {
    jj_consume_token(RANGE_T);
    }
    if (!hasError) {
    s = range();
    }
return " range "+s;
assert(false);
}


void VhdlParser::record_type_definition() {
    if (!hasError) {
    jj_consume_token(RECORD_T);
    }
    if (!hasError) {
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case SLSL_T:
      case STRINGLITERAL:
      case BASIC_IDENTIFIER:
      case EXTENDED_CHARACTER:{
        ;
        break;
        }
      default:
        jj_la1[284] = jj_gen;
        goto end_label_47;
      }
      if (!hasError) {
      element_declaration();
      }
    }
    end_label_47: ;
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
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      name();
      }
      break;
      }
    default:
      jj_la1[285] = jj_gen;
      ;
    }
    }
}


void VhdlParser::record_constraint() {
    if (!hasError) {
    jj_consume_token(LPAREN_T);
    }
    if (!hasError) {
    record_element_constraint();
    }
    if (!hasError) {
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case COMMA_T:{
        ;
        break;
        }
      default:
        jj_la1[286] = jj_gen;
        goto end_label_48;
      }
      if (!hasError) {
      jj_consume_token(COMMA_T);
      }
      if (!hasError) {
      record_element_constraint();
      }
    }
    end_label_48: ;
    }
    if (!hasError) {
    jj_consume_token(RPAREN_T);
    }
}


void VhdlParser::record_element_constraint() {
    if (!hasError) {
    identifier();
    }
    if (!hasError) {
    element_constraint();
    }
}


void VhdlParser::record_element_list() {
    if (!hasError) {
    identifier();
    }
    if (!hasError) {
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case COMMA_T:{
        ;
        break;
        }
      default:
        jj_la1[287] = jj_gen;
        goto end_label_49;
      }
      if (!hasError) {
      jj_consume_token(COMMA_T);
      }
      if (!hasError) {
      identifier();
      }
    }
    end_label_49: ;
    }
}


void VhdlParser::record_element_resolution() {
    if (!hasError) {
    identifier();
    }
    if (!hasError) {
    resolution_indication();
    }
}


void VhdlParser::record_resolution() {
    if (!hasError) {
    record_element_resolution();
    }
    if (!hasError) {
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case COMMA_T:{
        ;
        break;
        }
      default:
        jj_la1[288] = jj_gen;
        goto end_label_50;
      }
      if (!hasError) {
      jj_consume_token(COMMA_T);
      }
      if (!hasError) {
      record_element_resolution();
      }
    }
    end_label_50: ;
    }
}


void VhdlParser::record_mode_view_indication() {
    if (!hasError) {
    jj_consume_token(VIEW_T);
    }
    if (!hasError) {
    name();
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case OF_T:{
      if (!hasError) {
      jj_consume_token(OF_T);
      }
      if (!hasError) {
      subtype_indication();
      }
      break;
      }
    default:
      jj_la1[289] = jj_gen;
      ;
    }
    }
}


QCString VhdlParser::relation() {QCString s,s1,s2;
    if (!hasError) {
    s = shift_expression();
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LESSTHAN_T:
    case GREATERTHAN_T:
    case LT_T:
    case GT_T:
    case EQU_T:
    case NOTEQU_T:
    case QGT_T:
    case QLT_T:
    case QG_T:
    case QL_T:
    case QEQU_T:
    case QNEQU_T:{
      if (!hasError) {
      s1 = relation_operator();
      }
      if (!hasError) {
      s2 = shift_expression();
      }
      break;
      }
    default:
      jj_la1[290] = jj_gen;
      ;
    }
    }
return s+s1+s2;
assert(false);
}


QCString VhdlParser::relative_pathname() {QCString s,s1,s2;
    if (!hasError) {
    s = neg_list();
    }
    if (!hasError) {
    if (jj_2_130(2147483647)) {
      if (!hasError) {
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


QCString VhdlParser::neg_list() {QCString s;
    if (!hasError) {
    while (!hasError) {
      if (!hasError) {
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
        jj_la1[291] = jj_gen;
        goto end_label_51;
      }
    }
    end_label_51: ;
    }
return s;
assert(false);
}


QCString VhdlParser::relation_operator() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LT_T:{
      if (!hasError) {
      jj_consume_token(LT_T);
      }
      if (!hasError) {
return "<";
      }
      break;
      }
    case GT_T:{
      if (!hasError) {
      jj_consume_token(GT_T);
      }
      if (!hasError) {
return ">";
      }
      break;
      }
    case EQU_T:{
      if (!hasError) {
      jj_consume_token(EQU_T);
      }
      if (!hasError) {
return "=";
      }
      break;
      }
    case GREATERTHAN_T:{
      if (!hasError) {
      jj_consume_token(GREATERTHAN_T);
      }
      if (!hasError) {
return ">=";
      }
      break;
      }
    case LESSTHAN_T:{
      if (!hasError) {
      jj_consume_token(LESSTHAN_T);
      }
      if (!hasError) {
return "<=";
      }
      break;
      }
    case NOTEQU_T:{
      if (!hasError) {
      jj_consume_token(NOTEQU_T);
      }
      if (!hasError) {
return "/=";
      }
      break;
      }
    case QGT_T:{
      if (!hasError) {
      jj_consume_token(QGT_T);
      }
      if (!hasError) {
return "?>=";
      }
      break;
      }
    case QLT_T:{
      if (!hasError) {
      jj_consume_token(QLT_T);
      }
      if (!hasError) {
return "?<=";
      }
      break;
      }
    case QG_T:{
      if (!hasError) {
      jj_consume_token(QG_T);
      }
      if (!hasError) {
return "?>";
      }
      break;
      }
    case QL_T:{
      if (!hasError) {
      jj_consume_token(QL_T);
      }
      if (!hasError) {
return "?<";
      }
      break;
      }
    case QEQU_T:{
      if (!hasError) {
      jj_consume_token(QEQU_T);
      }
      if (!hasError) {
return "?=";
      }
      break;
      }
    case QNEQU_T:{
      if (!hasError) {
      jj_consume_token(QNEQU_T);
      }
      if (!hasError) {
return "?/=";
      }
      break;
      }
    default:
      jj_la1[292] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::report_statement() {Token *t=nullptr;Token *t1=nullptr;QCString s,s1,s2;
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      s = identifier();
      }
      if (!hasError) {
      t = jj_consume_token(COLON_T);
      }
      break;
      }
    default:
      jj_la1[293] = jj_gen;
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
    case SEVERITY_T:{
      if (!hasError) {
      t1 = jj_consume_token(SEVERITY_T);
      }
      if (!hasError) {
      s2 = expression();
      }
      break;
      }
    default:
      jj_la1[294] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
if(t!=nullptr) s.append(":");
        s1.prepend(" report ");
        if(t1!=nullptr) s2.prepend(" severity ");
        return s+s1+s2+";";
assert(false);
}


QCString VhdlParser::resolution_indication() {QCString s;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      s = identifier();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    case LPAREN_T:{
      if (!hasError) {
      jj_consume_token(LPAREN_T);
      }
      if (!hasError) {
      s = expression();
      }
      if (!hasError) {
      jj_consume_token(RPAREN_T);
      }
      if (!hasError) {
return "("+s+")";
      }
      break;
      }
    default:
      jj_la1[295] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::return_statement() {QCString s,s1;
    if (jj_2_131(2147483647)) {
      if (!hasError) {
      s = plain_return_statement();
      }
      if (!hasError) {
return s;
      }
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case RETURN_T:
      case BASIC_IDENTIFIER:
      case EXTENDED_CHARACTER:{
        if (!hasError) {
        s = value_return_statement();
        }
        if (!hasError) {
return s;
        }
        break;
        }
      default:
        jj_la1[296] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
assert(false);
}


QCString VhdlParser::plain_return_statement() {QCString s,s1;
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      s = identifier();
      }
      if (!hasError) {
      jj_consume_token(COLON_T);
      }
      break;
      }
    default:
      jj_la1[297] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(RETURN_T);
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case WHEN_T:{
      if (!hasError) {
      jj_consume_token(WHEN_T);
      }
      if (!hasError) {
      condition();
      }
      break;
      }
    default:
      jj_la1[298] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
return s;
assert(false);
}


QCString VhdlParser::value_return_statement() {QCString s,s1;
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
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
      jj_la1[299] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(RETURN_T);
    }
    if (!hasError) {
    s1 = conditional_or_unaffected_expression();
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
return s+" return "+s1+";";
assert(false);
}


QCString VhdlParser::scalar_type_definition() {QCString s,s1;
    if (jj_2_132(2147483647)) {
      if (!hasError) {
      s = enumeration_type_definition();
      }
      if (!hasError) {
return s;
      }
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case RANGE_T:{
        if (!hasError) {
        s = range_constraint();
        }
        if (!hasError) {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case UNITS_T:{
          if (!hasError) {
          s1 = physical_type_definition();
          }
          break;
          }
        default:
          jj_la1[300] = jj_gen;
          ;
        }
        }
        if (!hasError) {
return s+" "+s1;
        }
        break;
        }
      default:
        jj_la1[301] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
assert(false);
}


void VhdlParser::scalar_incomplete_type_definition() {
    if (!hasError) {
    jj_consume_token(BOX_T);
    }
}


void VhdlParser::secondary_unit() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case ARCHITECTURE_T:{
      if (!hasError) {
      architecture_body();
      }
      break;
      }
    case PACKAGE_T:{
      if (!hasError) {
      package_body();
      }
      break;
      }
    default:
      jj_la1[302] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
}


QCString VhdlParser::secondary_unit_declaration() {QCString s,s1;Token *t1=nullptr;
    if (!hasError) {
    s = identifier();
    }
    if (!hasError) {
    t1 = jj_consume_token(EQU_T);
    }
    if (!hasError) {
    s1 = physical_literal();
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
outlineParser()->addVhdlType(s,t1->beginLine,
                                EntryType::makeVariable(),VhdlSpecifier::UNITS,
                                QCString(),s1,Protection::Public);
   return s+"="+s1;
assert(false);
}


QCString VhdlParser::selected_name() {QCString s,s1;
    if (!hasError) {
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


void VhdlParser::selected_signal_assignment() {
    if (!hasError) {
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


void VhdlParser::selected_waveforms() {
    if (!hasError) {
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
        jj_la1[303] = jj_gen;
        goto end_label_52;
      }
      if (!hasError) {
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
    end_label_52: ;
    }
}


QCString VhdlParser::sensitivity_clause() {QCString s;
    if (!hasError) {
    jj_consume_token(ON_T);
    }
    if (!hasError) {
    s = sensitivity_list();
    }
s.prepend(" on ");
  return s;
assert(false);
}


void VhdlParser::selected_signal_assignment_wave() {
    if (jj_2_133(2147483647)) {
      if (!hasError) {
      selected_force_assignment();
      }
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case WITH_T:{
        if (!hasError) {
        selected_waveform_assignment();
        }
        break;
        }
      default:
        jj_la1[304] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
}


void VhdlParser::selected_variable_assignment() {
    if (!hasError) {
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
    case Q_T:{
      if (!hasError) {
      jj_consume_token(Q_T);
      }
      break;
      }
    default:
      jj_la1[305] = jj_gen;
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
    if (jj_2_134(2147483647)) {
      if (!hasError) {
      aggregate();
      }
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case SLSL_T:
      case STRINGLITERAL:
      case BASIC_IDENTIFIER:
      case EXTENDED_CHARACTER:{
        if (!hasError) {
        name();
        }
        break;
        }
      default:
        jj_la1[306] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
}


void VhdlParser::selected_waveform_assignment() {
    if (!hasError) {
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
    case Q_T:{
      if (!hasError) {
      jj_consume_token(Q_T);
      }
      break;
      }
    default:
      jj_la1[307] = jj_gen;
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
    case TRANSPORT_T:{
      if (!hasError) {
      delay_mechanism();
      }
      break;
      }
    default:
      jj_la1[308] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    sel_wave_list();
    }
}


void VhdlParser::selected_force_assignment() {
    if (!hasError) {
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
    case Q_T:{
      if (!hasError) {
      jj_consume_token(Q_T);
      }
      break;
      }
    default:
      jj_la1[309] = jj_gen;
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
    case OUT_T:{
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case IN_T:{
        if (!hasError) {
        jj_consume_token(IN_T);
        }
        break;
        }
      case OUT_T:{
        if (!hasError) {
        jj_consume_token(OUT_T);
        }
        break;
        }
      default:
        jj_la1[310] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
      break;
      }
    default:
      jj_la1[311] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    sel_var_list();
    }
}


void VhdlParser::sel_var_list() {
    if (!hasError) {
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
    case COMMA_T:{
      if (!hasError) {
      jj_consume_token(COMMA_T);
      }
      break;
      }
    case SEMI_T:{
      if (!hasError) {
      jj_consume_token(SEMI_T);
      }
      break;
      }
    default:
      jj_la1[312] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
    }
    }
    if (!hasError) {
    while (!hasError) {
      if (jj_2_135(2147483647)) {
        ;
      } else {
        goto end_label_53;
      }
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
      case COMMA_T:{
        if (!hasError) {
        jj_consume_token(COMMA_T);
        }
        break;
        }
      case SEMI_T:{
        if (!hasError) {
        jj_consume_token(SEMI_T);
        }
        break;
        }
      default:
        jj_la1[313] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
      }
    }
    end_label_53: ;
    }
}


void VhdlParser::sel_wave_list() {
    if (!hasError) {
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
        jj_la1[314] = jj_gen;
        goto end_label_54;
      }
      if (!hasError) {
      jj_consume_token(COMMA_T);
      }
      if (!hasError) {
      sel_wave_list();
      }
    }
    end_label_54: ;
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
}


QCString VhdlParser::sensitivity_list() {QCString s,s1;Token* tok=nullptr;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case ALL_T:{
      if (!hasError) {
      tok = jj_consume_token(ALL_T);
      }
      if (!hasError) {
if(tok!=nullptr) return "all" ;
      }
      break;
      }
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
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
          jj_la1[315] = jj_gen;
          goto end_label_55;
        }
        if (!hasError) {
        jj_consume_token(COMMA_T);
        }
        if (!hasError) {
        s1 = name();
        }
        if (!hasError) {
s+=",";s+=s1;
        }
      }
      end_label_55: ;
      }
      if (!hasError) {
return s;
      }
      break;
      }
    default:
      jj_la1[316] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::sequence_of_statements() {QCString s,s1;
    if (!hasError) {
    while (!hasError) {
      if (jj_2_136(3)) {
        ;
      } else {
        goto end_label_56;
      }
      if (!hasError) {
      s1 = sequential_statement();
      }
      if (!hasError) {
s+=s1;
      }
    }
    end_label_56: ;
    }
return s;
assert(false);
}


QCString VhdlParser::sequential_statement() {QCString s;
    if (jj_2_137(2147483647)) {
      if (!hasError) {
      s = signal_assignment_statement();
      }
      if (!hasError) {
FlowChart::addFlowChart(FlowChart::TEXT_NO,s,QCString());return s;
      }
    } else if (jj_2_138(3)) {
      if (!hasError) {
      s = assertion_statement();
      }
      if (!hasError) {
FlowChart::addFlowChart(FlowChart::TEXT_NO,s,QCString());return s;
      }
    } else if (jj_2_139(3)) {
      if (!hasError) {
      s = report_statement();
      }
      if (!hasError) {
FlowChart::addFlowChart(FlowChart::TEXT_NO,s,QCString());return s;
      }
    } else if (jj_2_140(3)) {
      if (!hasError) {
      s = wait_statement();
      }
      if (!hasError) {
FlowChart::addFlowChart(FlowChart::TEXT_NO,s,QCString());return s;
      }
    } else if (jj_2_141(2147483647)) {
      if (!hasError) {
      s = variable_assignment_statement();
      }
      if (!hasError) {
FlowChart::addFlowChart(FlowChart::TEXT_NO,s,QCString());return s;
      }
    } else if (jj_2_142(3)) {
      if (!hasError) {
      s = procedure_call_statement();
      }
      if (!hasError) {
FlowChart::addFlowChart(FlowChart::TEXT_NO,s,QCString());return s;
      }
    } else if (jj_2_143(3)) {
      if (!hasError) {
      if_statement();
      }
      if (!hasError) {
return s;
      }
    } else if (jj_2_144(3)) {
      if (!hasError) {
      case_statement();
      }
      if (!hasError) {
return s;
      }
    } else if (jj_2_145(3)) {
      if (!hasError) {
      loop_statement();
      }
      if (!hasError) {
return s;
      }
    } else if (jj_2_146(3)) {
      if (!hasError) {
      s = next_statement();
      }
      if (!hasError) {
return s;
      }
    } else if (jj_2_147(3)) {
      if (!hasError) {
      s = exit_statement();
      }
      if (!hasError) {
return s;
      }
    } else if (jj_2_148(3)) {
      if (!hasError) {
      s = return_statement();
      }
      if (!hasError) {
FlowChart::addFlowChart(FlowChart::RETURN_NO,s,QCString());return s;
      }
    } else if (jj_2_149(3)) {
      if (!hasError) {
      sequential_bock_statement();
      }
      if (!hasError) {
return "";
      }
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case NULL_T:
      case BASIC_IDENTIFIER:
      case EXTENDED_CHARACTER:{
        if (!hasError) {
        s = null_statement();
        }
        if (!hasError) {
FlowChart::addFlowChart(FlowChart::TEXT_NO,s,QCString());return s;
        }
        break;
        }
      default:
        jj_la1[317] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
assert(false);
}


QCString VhdlParser::sequential_bock_statement() {QCString s,s1,s2;
    if (jj_2_150(6)) {
      if (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case BASIC_IDENTIFIER:
      case EXTENDED_CHARACTER:{
        if (!hasError) {
        label();
        }
        if (!hasError) {
        jj_consume_token(COLON_T);
        }
        break;
        }
      default:
        jj_la1[318] = jj_gen;
        ;
      }
      }
      if (!hasError) {
      jj_consume_token(BLOCK_T);
      }
      if (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case IS_T:{
        if (!hasError) {
        jj_consume_token(IS_T);
        }
        break;
        }
      default:
        jj_la1[319] = jj_gen;
        ;
      }
      }
      if (!hasError) {
      jj_consume_token(BEGIN_T);
      }
      if (!hasError) {
      sequential_block_statement_part();
      }
      if (!hasError) {
      jj_consume_token(END_T);
      }
      if (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case BLOCK_T:{
        if (!hasError) {
        jj_consume_token(BLOCK_T);
        }
        break;
        }
      default:
        jj_la1[320] = jj_gen;
        ;
      }
      }
      if (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case BASIC_IDENTIFIER:
      case EXTENDED_CHARACTER:{
        if (!hasError) {
        label();
        }
        break;
        }
      default:
        jj_la1[321] = jj_gen;
        ;
      }
      }
      if (!hasError) {
      jj_consume_token(SEMI_T);
      }
      if (!hasError) {
return "";
      }
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case BLOCK_T:
      case BASIC_IDENTIFIER:
      case EXTENDED_CHARACTER:{
        if (!hasError) {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case BASIC_IDENTIFIER:
        case EXTENDED_CHARACTER:{
          if (!hasError) {
          label();
          }
          if (!hasError) {
          jj_consume_token(COLON_T);
          }
          break;
          }
        default:
          jj_la1[322] = jj_gen;
          ;
        }
        }
        if (!hasError) {
        jj_consume_token(BLOCK_T);
        }
        if (!hasError) {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case IS_T:{
          if (!hasError) {
          jj_consume_token(IS_T);
          }
          break;
          }
        default:
          jj_la1[323] = jj_gen;
          ;
        }
        }
        if (!hasError) {
        sequential_block_declarative_part();
        }
        if (!hasError) {
        jj_consume_token(BEGIN_T);
        }
        if (!hasError) {
        sequential_block_statement_part();
        }
        if (!hasError) {
        jj_consume_token(END_T);
        }
        if (!hasError) {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case BLOCK_T:{
          if (!hasError) {
          jj_consume_token(BLOCK_T);
          }
          break;
          }
        default:
          jj_la1[324] = jj_gen;
          ;
        }
        }
        if (!hasError) {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case BASIC_IDENTIFIER:
        case EXTENDED_CHARACTER:{
          if (!hasError) {
          label();
          }
          break;
          }
        default:
          jj_la1[325] = jj_gen;
          ;
        }
        }
        if (!hasError) {
        jj_consume_token(SEMI_T);
        }
        if (!hasError) {
return "";
        }
        break;
        }
      default:
        jj_la1[326] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
assert(false);
}


QCString VhdlParser::sequential_block_declarative_part() {QCString s,s1,s2;
    if (!hasError) {
    process_declarative_part();
    }
return "";
assert(false);
}


QCString VhdlParser::sequential_block_statement_part() {QCString s,s1,s2;
    if (!hasError) {
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ASSERT_T:
      case BLOCK_T:
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
        jj_la1[327] = jj_gen;
        goto end_label_57;
      }
      if (!hasError) {
      sequential_statement();
      }
    }
    end_label_57: ;
    }
return "";
assert(false);
}


QCString VhdlParser::sequential_statement_body() {QCString s,s1,s2;
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case ALIAS_T:
    case ATTRIBUTE_T:
    case BEGIN_T:
    case CONSTANT_T:
    case FILE_T:
    case FUNCTION_T:
    case GROUP_T:
    case IMPURE_T:
    case IS_T:
    case PACKAGE_T:
    case PROCEDURE_T:
    case PURE_T:
    case SHARED_T:
    case SUBTYPE_T:
    case TYPE_T:
    case USE_T:
    case VARIABLE_T:{
      if (!hasError) {
      sequential_statement_declarative_part();
      }
      if (!hasError) {
      jj_consume_token(BEGIN_T);
      }
      break;
      }
    default:
      jj_la1[328] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    sequence_of_statements();
    }
return "";
assert(false);
}


QCString VhdlParser::sequential_statement_declarative_part() {QCString s,s1,s2;
    if (!hasError) {
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
      case PACKAGE_T:
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
        jj_la1[329] = jj_gen;
        goto end_label_58;
      }
      if (!hasError) {
      process_declarative_item();
      }
    }
    end_label_58: ;
    }
return "";
assert(false);
}


QCString VhdlParser::shift_expression() {QCString s,s1,s2;
    if (!hasError) {
    s = simple_expression();
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case ROL_T:
    case ROR_T:
    case SLA_T:
    case SLL_T:
    case SRA_T:
    case SRL_T:{
      if (!hasError) {
      s1 = shift_operator();
      }
      if (!hasError) {
      s2 = simple_expression();
      }
      break;
      }
    default:
      jj_la1[330] = jj_gen;
      ;
    }
    }
return s+s1+s2;
assert(false);
}


QCString VhdlParser::shift_operator() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SLL_T:{
      if (!hasError) {
      jj_consume_token(SLL_T);
      }
      if (!hasError) {
return "sll";
      }
      break;
      }
    case SRL_T:{
      if (!hasError) {
      jj_consume_token(SRL_T);
      }
      if (!hasError) {
return "srl";
      }
      break;
      }
    case SLA_T:{
      if (!hasError) {
      jj_consume_token(SLA_T);
      }
      if (!hasError) {
return "sla";
      }
      break;
      }
    case SRA_T:{
      if (!hasError) {
      jj_consume_token(SRA_T);
      }
      if (!hasError) {
return "sra";
      }
      break;
      }
    case ROL_T:{
      if (!hasError) {
      jj_consume_token(ROL_T);
      }
      if (!hasError) {
return "rol";
      }
      break;
      }
    case ROR_T:{
      if (!hasError) {
      jj_consume_token(ROR_T);
      }
      if (!hasError) {
return "ror";
      }
      break;
      }
    default:
      jj_la1[331] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::sign() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case PLUS_T:{
      if (!hasError) {
      jj_consume_token(PLUS_T);
      }
      if (!hasError) {
return "+";
      }
      break;
      }
    case MINUS_T:{
      if (!hasError) {
      jj_consume_token(MINUS_T);
      }
      if (!hasError) {
return "-";
      }
      break;
      }
    default:
      jj_la1[332] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::signal_assignment_statement() {QCString s,s1,s2,s3;
    if (jj_2_152(2147483647)) {
      if (!hasError) {
      conditional_signal_assignment_wave();
      }
      if (!hasError) {
return QCString();
      }
    } else if (jj_2_153(2147483647)) {
      if (!hasError) {
      selected_signal_assignment_wave();
      }
      if (!hasError) {
return QCString();
      }
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case LPAREN_T:
      case SLSL_T:
      case STRINGLITERAL:
      case BASIC_IDENTIFIER:
      case EXTENDED_CHARACTER:{
        if (!hasError) {
        if (jj_2_151(2)) {
          if (!hasError) {
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
        case TRANSPORT_T:{
          if (!hasError) {
          s2 = delay_mechanism();
          }
          break;
          }
        default:
          jj_la1[333] = jj_gen;
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
        jj_la1[334] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
assert(false);
}


void VhdlParser::signal_declaration() {Token* tok=nullptr;QCString s,s1,s2,s3,s4;
    if (!hasError) {
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
    case REGISTER_T:{
      if (!hasError) {
      s2 = signal_kind();
      }
      break;
      }
    default:
      jj_la1[335] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case VARASSIGN_T:{
      if (!hasError) {
      tok = jj_consume_token(VARASSIGN_T);
      }
      if (!hasError) {
      s3 = expression();
      }
      break;
      }
    default:
      jj_la1[336] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
if(tok!=nullptr)
      s3.prepend(":=");
     s4=s1+s2+s3;
     outlineParser()->addVhdlType(s,outlineParser()->getLine(),
                                  EntryType::makeVariable(),VhdlSpecifier::SIGNAL,QCString(),s4,Protection::Public);
}


QCString VhdlParser::signal_kind() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case REGISTER_T:{
      if (!hasError) {
      jj_consume_token(REGISTER_T);
      }
      if (!hasError) {
return "register";
      }
      break;
      }
    case BUS_T:{
      if (!hasError) {
      jj_consume_token(BUS_T);
      }
      if (!hasError) {
return "bus";
      }
      break;
      }
    default:
      jj_la1[337] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::signal_list() {QCString s,s1;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
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
          jj_la1[338] = jj_gen;
          goto end_label_59;
        }
        if (!hasError) {
        jj_consume_token(COMMA_T);
        }
        if (!hasError) {
        s1 = name();
        }
        if (!hasError) {
s+=",";s+=s1;
        }
      }
      end_label_59: ;
      }
      break;
      }
    case OTHER_T:{
      if (!hasError) {
      jj_consume_token(OTHER_T);
      }
      if (!hasError) {
return "other";
      }
      break;
      }
    case ALL_T:{
      if (!hasError) {
      jj_consume_token(ALL_T);
      }
      if (!hasError) {
return "all";
      }
      break;
      }
    default:
      jj_la1[339] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::simple_mode_indication() {QCString s,s1;
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BUFFER_T:
    case IN_T:
    case INOUT_T:
    case LINKAGE_T:
    case OUT_T:{
      if (!hasError) {
      mode();
      }
      break;
      }
    default:
      jj_la1[340] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    subtype_indication();
    }
    if (!hasError) {
    interface_type_indication();
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BUS_T:{
      if (!hasError) {
      jj_consume_token(BUS_T);
      }
      break;
      }
    default:
      jj_la1[341] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case VARASSIGN_T:{
      if (!hasError) {
      jj_consume_token(VARASSIGN_T);
      }
      if (!hasError) {
      expression();
      }
      break;
      }
    default:
      jj_la1[342] = jj_gen;
      ;
    }
    }
assert(false);
}


QCString VhdlParser::signature() {QCString s,s1,s2;
    if (!hasError) {
    jj_consume_token(LBRACKET_T);
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
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
          jj_la1[343] = jj_gen;
          goto end_label_60;
        }
        if (!hasError) {
        jj_consume_token(COMMA_T);
        }
        if (!hasError) {
        s1 = name();
        }
        if (!hasError) {
s+=",";s+=s1;
        }
      }
      end_label_60: ;
      }
      break;
      }
    default:
      jj_la1[344] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case RETURN_T:{
      if (!hasError) {
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
      jj_la1[345] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(RBRACKET_T);
    }
s1="["+s+"]";return s1;
assert(false);
}


QCString VhdlParser::simple_expression() {QCString s,s1,s2;
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case PLUS_T:
    case MINUS_T:{
      if (!hasError) {
      s = sign();
      }
      break;
      }
    default:
      jj_la1[346] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    s1 = simpleTerm();
    }
    if (!hasError) {
s+=s1;
    }
    if (!hasError) {
    while (!hasError) {
      if (jj_2_154(2147483647)) {
        ;
      } else {
        goto end_label_61;
      }
      if (!hasError) {
      s1 = adding_operator();
      }
      if (!hasError) {
      s2 = simpleTerm();
      }
      if (!hasError) {
s+=s1;s+=s2;
      }
    }
    end_label_61: ;
    }
return s;
assert(false);
}


void VhdlParser::simple_name() {
    if (!hasError) {
    name();
    }
}


QCString VhdlParser::slice_name() {QCString s,s1;
    if (!hasError) {
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


QCString VhdlParser::string_literal() {Token *tok=nullptr;
    if (!hasError) {
    tok = jj_consume_token(STRINGLITERAL);
    }
return QCString(tok->image);
assert(false);
}


void VhdlParser::subprogram_body() {QCString s;
    if (!hasError) {
    jj_consume_token(IS_T);
    }
    if (!hasError) {
    s = subprogram_declarative_part();
    }
    if (!hasError) {
if (!s.isEmpty())
      {
        FlowChart::addFlowChart(FlowChart::VARIABLE_NO,s,QCString());
      }
      FlowChart::addFlowChart(FlowChart::BEGIN_NO,"BEGIN",QCString());
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
    case PROCEDURE_T:{
      if (!hasError) {
      subprogram_kind();
      }
      break;
      }
    default:
      jj_la1[347] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      designator();
      }
      break;
      }
    default:
      jj_la1[348] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
m_sharedState->tempEntry->endBodyLine=outlineParser()->getLine(END_T);
      outlineParser()->createFlow();
      m_sharedState->currP=VhdlSpecifier::UNKNOWN;
}


void VhdlParser::subprogram_declaration() {
    if (jj_2_155(2147483647)) {
      if (!hasError) {
      subprogram_instantiation_declaration();
      }
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case FUNCTION_T:
      case IMPURE_T:
      case PROCEDURE_T:
      case PURE_T:{
        if (!hasError) {
        subprogram_specification();
        }
        if (!hasError) {
        subprogram_1();
        }
        if (!hasError) {
m_sharedState->currP=VhdlSpecifier::UNKNOWN;
        }
        break;
        }
      default:
        jj_la1[349] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
}


void VhdlParser::subprogram_1() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case IS_T:{
      if (!hasError) {
      subprogram_body();
      }
      break;
      }
    case SEMI_T:{
      if (!hasError) {
      jj_consume_token(SEMI_T);
      }
      break;
      }
    default:
      jj_la1[350] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
}


QCString VhdlParser::subprogram_declarative_item() {QCString s;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case FUNCTION_T:
    case IMPURE_T:
    case PROCEDURE_T:
    case PURE_T:{
      if (!hasError) {
      subprogram_declaration();
      }
      if (!hasError) {
return QCString();
      }
      break;
      }
    case TYPE_T:{
      if (!hasError) {
      s = type_declaration();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    case IS_T:{
      if (!hasError) {
      subprogram_body();
      }
      if (!hasError) {
return QCString();
      }
      break;
      }
    case SUBTYPE_T:{
      if (!hasError) {
      s = subtype_declaration();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    case CONSTANT_T:{
      if (!hasError) {
      s = constant_declaration();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    case SHARED_T:
    case VARIABLE_T:{
      if (!hasError) {
      s = variable_declaration();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    case FILE_T:{
      if (!hasError) {
      s = file_declaration();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    case ALIAS_T:{
      if (!hasError) {
      s = alias_declaration();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    default:
      jj_la1[351] = jj_gen;
      if (jj_2_156(2147483647)) {
        if (!hasError) {
        s = attribute_declaration();
        }
        if (!hasError) {
return s;
        }
      } else {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case ATTRIBUTE_T:{
          if (!hasError) {
          s = attribute_specification();
          }
          if (!hasError) {
return s;
          }
          break;
          }
        case USE_T:{
          if (!hasError) {
          s = use_clause();
          }
          if (!hasError) {
return s;
          }
          break;
          }
        default:
          jj_la1[352] = jj_gen;
          if (jj_2_157(3)) {
            if (!hasError) {
            s = group_template_declaration();
            }
            if (!hasError) {
return s;
            }
          } else {
            switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
            case GROUP_T:{
              if (!hasError) {
              s = group_declaration();
              }
              if (!hasError) {
return s;
              }
              break;
              }
            default:
              jj_la1[353] = jj_gen;
              jj_consume_token(-1);
              errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
            }
          }
        }
      }
    }
assert(false);
}


QCString VhdlParser::subprogram_declarative_part() {QCString s,s1;
    if (!hasError) {
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
        jj_la1[354] = jj_gen;
        goto end_label_62;
      }
      if (!hasError) {
      s1 = subprogram_declarative_item();
      }
      if (!hasError) {
s+=s1;
      }
    }
    end_label_62: ;
    }
return s;
assert(false);
}


void VhdlParser::subprogram_kind() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case FUNCTION_T:{
      if (!hasError) {
      jj_consume_token(FUNCTION_T);
      }
      break;
      }
    case PROCEDURE_T:{
      if (!hasError) {
      jj_consume_token(PROCEDURE_T);
      }
      break;
      }
    default:
      jj_la1[355] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
}


void VhdlParser::subprogram_header() {
    if (!hasError) {
    jj_consume_token(GENERIC_T);
    }
    if (!hasError) {
m_sharedState->parse_sec=VhdlSection::GEN_SEC;
    }
    if (!hasError) {
    jj_consume_token(LPAREN_T);
    }
    if (!hasError) {
    generic_list();
    }
    if (!hasError) {
    jj_consume_token(RPAREN_T);
    }
    if (!hasError) {
m_sharedState->parse_sec=VhdlSection::UNKNOWN;
    }
    if (!hasError) {
    if (jj_2_158(2)) {
      if (!hasError) {
      generic_map_aspect();
      }
    } else {
      ;
    }
    }
}


void VhdlParser::subprogram_specification() {QCString s;Token *tok=nullptr;Token *t;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case PROCEDURE_T:{
      if (!hasError) {
      jj_consume_token(PROCEDURE_T);
      }
      if (!hasError) {
      s = designator();
      }
      if (!hasError) {
m_sharedState->currP=VhdlSpecifier::PROCEDURE;
   outlineParser()->createFunction(s,m_sharedState->currP,QCString());
   m_sharedState->tempEntry=m_sharedState->current;
   m_sharedState->current->startLine=outlineParser()->getLine(PROCEDURE_T);
   m_sharedState->current->bodyLine=outlineParser()->getLine(PROCEDURE_T);
      }
      if (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case LPAREN_T:{
        if (!hasError) {
        jj_consume_token(LPAREN_T);
        }
        if (!hasError) {
m_sharedState->param_sec=VhdlSection::PARAM_SEC;
        }
        if (!hasError) {
        interface_list();
        }
        if (!hasError) {
m_sharedState->param_sec=VhdlSection::UNKNOWN;
        }
        if (!hasError) {
        jj_consume_token(RPAREN_T);
        }
        break;
        }
      default:
        jj_la1[356] = jj_gen;
        ;
      }
      }
      if (!hasError) {
      if (jj_2_159(2)) {
        if (!hasError) {
        gen_interface_list();
        }
      } else {
        ;
      }
      }
      if (!hasError) {
      if (jj_2_160(2)) {
        if (!hasError) {
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
outlineParser()->newEntry();
      }
      break;
      }
    case FUNCTION_T:
    case IMPURE_T:
    case PURE_T:{
      if (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case IMPURE_T:
      case PURE_T:{
        if (!hasError) {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case PURE_T:{
          if (!hasError) {
          tok = jj_consume_token(PURE_T);
          }
          break;
          }
        case IMPURE_T:{
          if (!hasError) {
          tok = jj_consume_token(IMPURE_T);
          }
          break;
          }
        default:
          jj_la1[357] = jj_gen;
          jj_consume_token(-1);
          errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
        }
        }
        break;
        }
      default:
        jj_la1[358] = jj_gen;
        ;
      }
      }
      if (!hasError) {
      t = jj_consume_token(FUNCTION_T);
      }
      if (!hasError) {
m_sharedState->currP=VhdlSpecifier::FUNCTION;
      }
      if (!hasError) {
      s = designator();
      }
      if (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case GENERIC_T:{
        if (!hasError) {
        subprogram_header();
        }
        break;
        }
      default:
        jj_la1[359] = jj_gen;
        ;
      }
      }
      if (!hasError) {
if (tok!=nullptr)
     outlineParser()->createFunction(tok->image,m_sharedState->currP,s);
   else
     outlineParser()->createFunction(QCString(),m_sharedState->currP,s);

   m_sharedState->tempEntry=m_sharedState->current;
   m_sharedState->current->startLine=outlineParser()->getLine(FUNCTION_T);
   m_sharedState->current->bodyLine=outlineParser()->getLine(FUNCTION_T);
      }
      if (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case PARAMETER_T:
      case LPAREN_T:{
        if (!hasError) {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case PARAMETER_T:{
          if (!hasError) {
          jj_consume_token(PARAMETER_T);
          }
          break;
          }
        default:
          jj_la1[360] = jj_gen;
          ;
        }
        }
        if (!hasError) {
m_sharedState->param_sec=VhdlSection::PARAM_SEC;
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
m_sharedState->param_sec=VhdlSection::UNKNOWN;
        }
        break;
        }
      default:
        jj_la1[361] = jj_gen;
        ;
      }
      }
      if (!hasError) {
      jj_consume_token(RETURN_T);
      }
      if (!hasError) {
      if (jj_2_161(2)) {
        if (!hasError) {
        identifier();
        }
        if (!hasError) {
        jj_consume_token(OF_T);
        }
      } else {
        ;
      }
      }
      if (!hasError) {
      s = type_mark();
      }
      if (!hasError) {
m_sharedState->tempEntry=m_sharedState->current;
                m_sharedState->current->type=s;
                outlineParser()->newEntry();
      }
      break;
      }
    default:
      jj_la1[362] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
}


void VhdlParser::subprogram_statement_part() {
    if (!hasError) {
    while (!hasError) {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case ASSERT_T:
      case BLOCK_T:
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
        jj_la1[363] = jj_gen;
        goto end_label_63;
      }
      if (!hasError) {
      sequential_statement();
      }
    }
    end_label_63: ;
    }
}


QCString VhdlParser::subprogram_instantiation_declaration() {QCString s,s1,s2;
    if (!hasError) {
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
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LBRACKET_T:{
      if (!hasError) {
      s2 = signature();
      }
      break;
      }
    default:
      jj_la1[364] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case GENERIC_T:{
      if (!hasError) {
      gen_assoc_list();
      }
      break;
      }
    default:
      jj_la1[365] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
QCString q= " is new "+s1+s2;
      outlineParser()->addVhdlType(s,outlineParser()->getLine(FUNCTION_T),
                                   EntryType::makeVariable(),VhdlSpecifier::INSTANTIATION,"function ",q,Protection::Public);
    return q;
assert(false);
}


QCString VhdlParser::subtype_declaration() {QCString s,s1;
    if (!hasError) {
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
outlineParser()->addVhdlType(s,outlineParser()->getLine(),
                               EntryType::makeVariable(),VhdlSpecifier::SUBTYPE,QCString(),s1,Protection::Public);
  return " subtype "+s+" is "+s1+";";
assert(false);
}


QCString VhdlParser::subtype_indication() {QCString s,s1,s2;
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LPAREN_T:{
      if (!hasError) {
      jj_consume_token(LPAREN_T);
      }
      if (!hasError) {
      s = expression();
      }
      if (!hasError) {
      jj_consume_token(RPAREN_T);
      }
      break;
      }
    default:
      jj_la1[366] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    while (!hasError) {
      if (!hasError) {
      s1 = name();
      }
      if (jj_2_162(2)) {
        ;
      } else {
        goto end_label_64;
      }
    }
    end_label_64: ;
    }
    if (!hasError) {
    if (jj_2_163(2147483647)) {
      if (!hasError) {
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
    if (jj_2_164(2147483647)) {
      if (!hasError) {
      s = name();
      }
      if (!hasError) {
return s;
      }
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case CHARACTER_LITERAL:{
        if (!hasError) {
        s = character_literal();
        }
        if (!hasError) {
return s;
        }
        break;
        }
      case STRINGLITERAL:{
        if (!hasError) {
        s = operator_symbol();
        }
        if (!hasError) {
return s;
        }
        break;
        }
      case ALL_T:{
        if (!hasError) {
        jj_consume_token(ALL_T);
        }
        if (!hasError) {
return " all ";
        }
        break;
        }
      default:
        jj_la1[367] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
assert(false);
}


QCString VhdlParser::target() {QCString s;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SLSL_T:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      s = name();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    case LPAREN_T:{
      if (!hasError) {
      s = aggregate();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    default:
      jj_la1[368] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::simpleTerm() {QCString s,s1,s2;
    if (!hasError) {
    s = factor();
    }
    if (!hasError) {
    while (!hasError) {
      if (jj_2_165(2)) {
        ;
      } else {
        goto end_label_65;
      }
      if (!hasError) {
      s1 = multiplying_operation();
      }
      if (!hasError) {
      s2 = factor();
      }
      if (!hasError) {
s+=s1;s+=s2;
      }
    }
    end_label_65: ;
    }
return s;
assert(false);
}


QCString VhdlParser::timeout_clause() {QCString s;
    if (!hasError) {
    jj_consume_token(FOR_T);
    }
    if (!hasError) {
    s = expression();
    }
return " for "+s;
assert(false);
}


QCString VhdlParser::type_conversion() {QCString s,s1;
    if (!hasError) {
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
    if (jj_2_166(3)) {
      if (!hasError) {
      s = full_type_declaration();
      }
      if (!hasError) {
return s;
      }
    } else {
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case TYPE_T:{
        if (!hasError) {
        s = incomplete_type_declaration();
        }
        if (!hasError) {
return s;
        }
        break;
        }
      default:
        jj_la1[369] = jj_gen;
        jj_consume_token(-1);
        errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
      }
    }
assert(false);
}


QCString VhdlParser::type_definition() {QCString s;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case RANGE_T:
    case LPAREN_T:{
      if (!hasError) {
      s = scalar_type_definition();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    case ARRAY_T:
    case RECORD_T:{
      if (!hasError) {
      s = composite_type_definition();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    case ACCESS_T:{
      if (!hasError) {
      s = access_type_definition();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    case FILE_T:{
      if (!hasError) {
      s = file_type_definition();
      }
      if (!hasError) {
return s;
      }
      break;
      }
    case NEW_T:{
      if (!hasError) {
      protected_type_instantiation_definition();
      }
      if (!hasError) {
return QCString();
      }
      break;
      }
    default:
      jj_la1[370] = jj_gen;
      if (jj_2_167(2)) {
        if (!hasError) {
        protected_type_body();
        }
        if (!hasError) {
return QCString();
        }
      } else {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case PROTECTED_T:{
          if (!hasError) {
          protected_type_declaration();
          }
          if (!hasError) {
return QCString();
          }
          break;
          }
        default:
          jj_la1[371] = jj_gen;
          jj_consume_token(-1);
          errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
        }
      }
    }
assert(false);
}


void VhdlParser::protected_type_instantiation_definition() {QCString s;
    if (!hasError) {
    jj_consume_token(NEW_T);
    }
    if (!hasError) {
    name();
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case GENERIC_T:{
      if (!hasError) {
      generic_map_aspect();
      }
      break;
      }
    default:
      jj_la1[372] = jj_gen;
      ;
    }
    }
}


QCString VhdlParser::type_mark() {QCString s;
    if (!hasError) {
    s = name();
    }
return s;
assert(false);
}


QCString VhdlParser::unconstraint_array_definition() {QCString s,s1,s2,s3;
    if (!hasError) {
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
        jj_la1[373] = jj_gen;
        goto end_label_66;
      }
      if (!hasError) {
      jj_consume_token(COMMA_T);
      }
      if (!hasError) {
      s1 = index_subtype_definition();
      }
      if (!hasError) {
s3+=",";s3+=s1;
      }
    }
    end_label_66: ;
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


QCString VhdlParser::use_clause() {QCString s,s1;
    if (!hasError) {
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
        jj_la1[374] = jj_gen;
        goto end_label_67;
      }
      if (!hasError) {
      jj_consume_token(COMMA_T);
      }
      if (!hasError) {
      s1 = selected_name();
      }
      if (!hasError) {
s+=",";s+=s1;
      }
    }
    end_label_67: ;
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
auto ql1=split(s.str(),",");
                   for (const auto &name : ql1)
                   {
                     auto ql2=split(name,".");
                     if (ql2.size()>1)
                     {
                       std::string it=ql2[1];
                       if ( m_sharedState->parse_sec==VhdlSection::UNKNOWN && Config_getBool(SHOW_INCLUDE_FILES) )
                       {
                         outlineParser()->addVhdlType(it.c_str(),
                                                      outlineParser()->getLine(),
                                                      EntryType::makeVariable(),
                                                      VhdlSpecifier::USE,
                                                      it.c_str(),
                                                      "_use_",Protection::Public);
                       }
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
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      if (jj_2_168(2)) {
        if (!hasError) {
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
      s2 = conditional_or_unaffected_expression();
      }
      if (!hasError) {
      jj_consume_token(SEMI_T);
      }
      if (!hasError) {
return s+s1+":="+s2+";";
      }
      break;
      }
    case WITH_T:{
      if (!hasError) {
      selected_variable_assignment();
      }
      if (!hasError) {
return QCString();
      }
      break;
      }
    default:
      jj_la1[375] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::variable_declaration() {Token *tok=nullptr;Token *t1=nullptr;QCString s,s1,s2;
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case SHARED_T:{
      if (!hasError) {
      tok = jj_consume_token(SHARED_T);
      }
      break;
      }
    default:
      jj_la1[376] = jj_gen;
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
    case GENERIC_T:{
      if (!hasError) {
      generic_map_aspect();
      }
      break;
      }
    default:
      jj_la1[377] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case VARASSIGN_T:{
      if (!hasError) {
      t1 = jj_consume_token(VARASSIGN_T);
      }
      if (!hasError) {
      s2 = conditional_expression();
      }
      break;
      }
    default:
      jj_la1[378] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
if(t1!=nullptr)
    s2.prepend(":=");
  QCString val=" variable "+s+":"+s1+s2+";";
  QCString it=s1;
  if(tok != nullptr)
  {
    it.prepend(" shared ");
    val.prepend(" shared");
  }
  VhdlSpecifier spec=VhdlSpecifier::SHAREDVARIABLE;

  if(t1 != nullptr)
  {
    it+=":=";
    it+=s2;
  }
  outlineParser()->addVhdlType(s,outlineParser()->getLine(),
                               EntryType::makeVariable(),spec,QCString(),it,Protection::Public);
  return val;
assert(false);
}


QCString VhdlParser::wait_statement() {QCString s,s1,s2,s3;Token *t=nullptr;
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      s = identifier();
      }
      if (!hasError) {
      t = jj_consume_token(COLON_T);
      }
      break;
      }
    default:
      jj_la1[379] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(WAIT_T);
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case ON_T:{
      if (!hasError) {
      s1 = sensitivity_clause();
      }
      break;
      }
    default:
      jj_la1[380] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case UNTIL_T:{
      if (!hasError) {
      s2 = condition_clause();
      }
      break;
      }
    default:
      jj_la1[381] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case FOR_T:{
      if (!hasError) {
      s3 = timeout_clause();
      }
      break;
      }
    default:
      jj_la1[382] = jj_gen;
      ;
    }
    }
    if (!hasError) {
    jj_consume_token(SEMI_T);
    }
if(t!=nullptr) s.append(":");
  return s+" wait "+s1+s2+s3+";";
assert(false);
}


QCString VhdlParser::waveform() {QCString s,s1;
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case ABS_T:
    case AND_T:
    case NAND_T:
    case NEW_T:
    case NOR_T:
    case NOT_T:
    case NULL_T:
    case OR_T:
    case XOR_T:
    case XNOR_T:
    case LPAREN_T:
    case PLUS_T:
    case MINUS_T:
    case SLSL_T:
    case QQ_T:
    case INTEGER:
    case STRINGLITERAL:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:
    case CHARACTER_LITERAL:
    case DECIMAL_LITERAL:
    case BASED_LITERAL:
    case BIT_STRING_LITERAL:{
      if (!hasError) {
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
          jj_la1[383] = jj_gen;
          goto end_label_68;
        }
        if (!hasError) {
        jj_consume_token(COMMA_T);
        }
        if (!hasError) {
        s1 = waveform_element();
        }
        if (!hasError) {
s+=","; s+=s1;
        }
      }
      end_label_68: ;
      }
      if (!hasError) {
return s;
      }
      break;
      }
    case UNAFFECTED_T:{
      if (!hasError) {
      jj_consume_token(UNAFFECTED_T);
      }
      if (!hasError) {
return " unaffected ";
      }
      break;
      }
    default:
      jj_la1[384] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
assert(false);
}


QCString VhdlParser::waveform_element() {QCString s,s1;
    if (!hasError) {
    s = expression();
    }
    if (!hasError) {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case AFTER_T:{
      if (!hasError) {
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
      jj_la1[385] = jj_gen;
      ;
    }
    }
return s+s1;
assert(false);
}


void VhdlParser::parseInline() {
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case POSTPONED_T:
    case PROCESS_T:
    case BASIC_IDENTIFIER:
    case EXTENDED_CHARACTER:{
      if (!hasError) {
      process_statement();
      }
      break;
      }
    case FUNCTION_T:
    case IMPURE_T:
    case PROCEDURE_T:
    case PURE_T:{
      if (!hasError) {
      subprogram_declaration();
      }
      break;
      }
    default:
      jj_la1[386] = jj_gen;
      jj_consume_token(-1);
      errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this), hasError = true;
    }
}


  VhdlParser::VhdlParser(TokenManager *tokenManager){
    head = nullptr;
    ReInit(tokenManager);
}
VhdlParser::~VhdlParser()
{
  clear();
}

void VhdlParser::ReInit(TokenManager* tokenManager){
    clear();
    errorHandler = new ErrorHandler();
    hasError = false;
    token_source = tokenManager;
    head = token = new Token();
    token->kind = 0;
    token->next = nullptr;
    jj_lookingAhead = false;
    jj_rescan = false;
    jj_done = false;
    jj_scanpos = jj_lastpos = nullptr;
    jj_gc = 0;
    jj_kind = -1;
    indent = 0;
    trace = false;
    jj_ntk = -1;
    jj_gen = 0;
    for (int i = 0; i < 387; i++) jj_la1[i] = -1;
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
  if (errorHandler) {
    delete errorHandler, errorHandler = nullptr;
  }
}


Token * VhdlParser::jj_consume_token(int kind)  {
    Token *oldToken;
    if ((oldToken = token)->next != nullptr) token = token->next;
    else token = token->next = token_source->getNextToken();
    jj_ntk = -1;
    if (token->kind == kind) {
      jj_gen++;
      if (++jj_gc > 100) {
        jj_gc = 0;
        for (int i = 0; i < 168; i++) {
          JJCalls *c = &jj_2_rtns[i];
          while (c != nullptr) {
            if (c->gen < jj_gen) c->first = nullptr;
            c = c->next;
          }
        }
      }
      return token;
    }
    token = oldToken;
    jj_kind = kind;
    JJString image = kind >= 0 ? tokenImage[kind] : tokenImage[0];
    errorHandler->handleUnexpectedToken(kind, image.substr(1, image.size() - 2), getToken(1), this);
    hasError = true;
    return token;
  }


bool  VhdlParser::jj_scan_token(int kind){
    if (jj_scanpos == jj_lastpos) {
      jj_la--;
      if (jj_scanpos->next == nullptr) {
        jj_lastpos = jj_scanpos = jj_scanpos->next = token_source->getNextToken();
      } else {
        jj_lastpos = jj_scanpos = jj_scanpos->next;
      }
    } else {
      jj_scanpos = jj_scanpos->next;
    }
    if (jj_rescan) {
      int i = 0; Token *tok = token;
      while (tok != nullptr && tok != jj_scanpos) { i++; tok = tok->next; }
      if (tok != nullptr) jj_add_error_token(kind, i);
    }
    if (jj_scanpos->kind != kind) return true;
    if (jj_la == 0 && jj_scanpos == jj_lastpos) { return jj_done = true; }
    return false;
  }


/** Get the next Token. */

Token * VhdlParser::getNextToken(){
    if (token->next != nullptr) token = token->next;
    else token = token->next = token_source->getNextToken();
    jj_ntk = -1;
    jj_gen++;
    return token;
  }

/** Get the specific Token. */

Token * VhdlParser::getToken(int index){
    Token *t = token;
    for (int i = 0; i < index; i++) {
      if (t->next != nullptr) t = t->next;
      else t = t->next = token_source->getNextToken();
    }
    return t;
  }


int VhdlParser::jj_ntk_f(){
    if ((jj_nt=token->next) == nullptr)
      return (jj_ntk = (token->next=token_source->getNextToken())->kind);
    else
      return (jj_ntk = jj_nt->kind);
  }


  void VhdlParser::jj_add_error_token(int kind, int pos)  {
  }


 void  VhdlParser::parseError()   {
      fprintf(stderr, "Parse error at: %d:%d, after token: %s encountered: %s\n", token->beginLine, token->beginColumn, addUnicodeEscapes(token->image).c_str(), addUnicodeEscapes(getToken(1)->image).c_str());
   }


  bool VhdlParser::trace_enabled()  {
    return trace;
  }


  void VhdlParser::enable_tracing()  {
  }

  void VhdlParser::disable_tracing()  {
  }


  void VhdlParser::jj_rescan_token(){
    jj_rescan = true;
    for (int i = 0; i < 168; i++) {
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
            case 115: jj_3_116(); break;
            case 116: jj_3_117(); break;
            case 117: jj_3_118(); break;
            case 118: jj_3_119(); break;
            case 119: jj_3_120(); break;
            case 120: jj_3_121(); break;
            case 121: jj_3_122(); break;
            case 122: jj_3_123(); break;
            case 123: jj_3_124(); break;
            case 124: jj_3_125(); break;
            case 125: jj_3_126(); break;
            case 126: jj_3_127(); break;
            case 127: jj_3_128(); break;
            case 128: jj_3_129(); break;
            case 129: jj_3_130(); break;
            case 130: jj_3_131(); break;
            case 131: jj_3_132(); break;
            case 132: jj_3_133(); break;
            case 133: jj_3_134(); break;
            case 134: jj_3_135(); break;
            case 135: jj_3_136(); break;
            case 136: jj_3_137(); break;
            case 137: jj_3_138(); break;
            case 138: jj_3_139(); break;
            case 139: jj_3_140(); break;
            case 140: jj_3_141(); break;
            case 141: jj_3_142(); break;
            case 142: jj_3_143(); break;
            case 143: jj_3_144(); break;
            case 144: jj_3_145(); break;
            case 145: jj_3_146(); break;
            case 146: jj_3_147(); break;
            case 147: jj_3_148(); break;
            case 148: jj_3_149(); break;
            case 149: jj_3_150(); break;
            case 150: jj_3_151(); break;
            case 151: jj_3_152(); break;
            case 152: jj_3_153(); break;
            case 153: jj_3_154(); break;
            case 154: jj_3_155(); break;
            case 155: jj_3_156(); break;
            case 156: jj_3_157(); break;
            case 157: jj_3_158(); break;
            case 158: jj_3_159(); break;
            case 159: jj_3_160(); break;
            case 160: jj_3_161(); break;
            case 161: jj_3_162(); break;
            case 162: jj_3_163(); break;
            case 163: jj_3_164(); break;
            case 164: jj_3_165(); break;
            case 165: jj_3_166(); break;
            case 166: jj_3_167(); break;
            case 167: jj_3_168(); break;
          }
        }
        p = p->next;
      } while (p != nullptr);
    }
    jj_rescan = false;
  }


  void VhdlParser::jj_save(int index, int xla){
    JJCalls *p = &jj_2_rtns[index];
    while (p->gen > jj_gen) {
      if (p->next == nullptr) { p = p->next = new JJCalls(); break; }
      p = p->next;
    }
    p->gen = jj_gen + xla - jj_la; p->first = token; p->arg = xla;
  }


}
}
