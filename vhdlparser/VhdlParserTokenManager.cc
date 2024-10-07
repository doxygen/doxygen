/* VhdlParserTokenManager.cc */
#include "VhdlParserTokenManager.h"
#include "TokenMgrError.h"
namespace vhdl {
namespace parser {
static const unsigned long long jjbitVec0[] = {
   0xfffffffffffffffeULL, 0xffffffffffffffffULL, 0xffffffffffffffffULL, 0xffffffffffffffffULL
};
static const unsigned long long jjbitVec2[] = {
   0x0ULL, 0x0ULL, 0xffffffffffffffffULL, 0xffffffffffffffffULL
};
static const unsigned long long jjbitVec3[] = {
   0x0ULL, 0x0ULL, 0xffffffff00000000ULL, 0xffffffffffffffffULL
};
static const int jjnextStates[] = {
   50, 51, 2, 37, 38, 58, 59, 60, 61, 62, 65, 69, 70, 71, 80, 20, 
   21, 81, 0, 3, 4, 5, 7, 12, 13, 15, 16, 24, 23, 25, 33, 34, 
   36, 39, 40, 42, 46, 47, 49, 53, 54, 56, 60, 61, 62, 65, 64, 63, 
   65, 69, 70, 71, 72, 73, 75, 80, 20, 21, 80, 20, 21, 81, 9, 10, 
   28, 29, 27, 30, 
};
static JJChar jjstrLiteralChars_0[] = {0};
static JJChar jjstrLiteralChars_1[] = {0};
static JJChar jjstrLiteralChars_2[] = {0};
static JJChar jjstrLiteralChars_3[] = {0};
static JJChar jjstrLiteralChars_4[] = {0};
static JJChar jjstrLiteralChars_5[] = {0};
static JJChar jjstrLiteralChars_6[] = {0};

static JJChar jjstrLiteralChars_7[] = {0};
static JJChar jjstrLiteralChars_8[] = {0};
static JJChar jjstrLiteralChars_9[] = {0};
static JJChar jjstrLiteralChars_10[] = {0};
static JJChar jjstrLiteralChars_11[] = {0};
static JJChar jjstrLiteralChars_12[] = {0};
static JJChar jjstrLiteralChars_13[] = {0};
static JJChar jjstrLiteralChars_14[] = {0};
static JJChar jjstrLiteralChars_15[] = {0};
static JJChar jjstrLiteralChars_16[] = {0};
static JJChar jjstrLiteralChars_17[] = {0};
static JJChar jjstrLiteralChars_18[] = {0};
static JJChar jjstrLiteralChars_19[] = {0};
static JJChar jjstrLiteralChars_20[] = {0};

static JJChar jjstrLiteralChars_21[] = {0};
static JJChar jjstrLiteralChars_22[] = {0};
static JJChar jjstrLiteralChars_23[] = {0};
static JJChar jjstrLiteralChars_24[] = {0};
static JJChar jjstrLiteralChars_25[] = {0};
static JJChar jjstrLiteralChars_26[] = {0};
static JJChar jjstrLiteralChars_27[] = {0};
static JJChar jjstrLiteralChars_28[] = {0};
static JJChar jjstrLiteralChars_29[] = {0};
static JJChar jjstrLiteralChars_30[] = {0};
static JJChar jjstrLiteralChars_31[] = {0};
static JJChar jjstrLiteralChars_32[] = {0};
static JJChar jjstrLiteralChars_33[] = {0};
static JJChar jjstrLiteralChars_34[] = {0};

static JJChar jjstrLiteralChars_35[] = {0};
static JJChar jjstrLiteralChars_36[] = {0};
static JJChar jjstrLiteralChars_37[] = {0};
static JJChar jjstrLiteralChars_38[] = {0};
static JJChar jjstrLiteralChars_39[] = {0};
static JJChar jjstrLiteralChars_40[] = {0};
static JJChar jjstrLiteralChars_41[] = {0};
static JJChar jjstrLiteralChars_42[] = {0};
static JJChar jjstrLiteralChars_43[] = {0};
static JJChar jjstrLiteralChars_44[] = {0};
static JJChar jjstrLiteralChars_45[] = {0};
static JJChar jjstrLiteralChars_46[] = {0};
static JJChar jjstrLiteralChars_47[] = {0};
static JJChar jjstrLiteralChars_48[] = {0};

static JJChar jjstrLiteralChars_49[] = {0};
static JJChar jjstrLiteralChars_50[] = {0};
static JJChar jjstrLiteralChars_51[] = {0};
static JJChar jjstrLiteralChars_52[] = {0};
static JJChar jjstrLiteralChars_53[] = {0};
static JJChar jjstrLiteralChars_54[] = {0};
static JJChar jjstrLiteralChars_55[] = {0};
static JJChar jjstrLiteralChars_56[] = {0};
static JJChar jjstrLiteralChars_57[] = {0};
static JJChar jjstrLiteralChars_58[] = {0};
static JJChar jjstrLiteralChars_59[] = {0};
static JJChar jjstrLiteralChars_60[] = {0};
static JJChar jjstrLiteralChars_61[] = {0};
static JJChar jjstrLiteralChars_62[] = {0};

static JJChar jjstrLiteralChars_63[] = {0};
static JJChar jjstrLiteralChars_64[] = {0};
static JJChar jjstrLiteralChars_65[] = {0};
static JJChar jjstrLiteralChars_66[] = {0};
static JJChar jjstrLiteralChars_67[] = {0};
static JJChar jjstrLiteralChars_68[] = {0};
static JJChar jjstrLiteralChars_69[] = {0};
static JJChar jjstrLiteralChars_70[] = {0};
static JJChar jjstrLiteralChars_71[] = {0};
static JJChar jjstrLiteralChars_72[] = {0};
static JJChar jjstrLiteralChars_73[] = {0};
static JJChar jjstrLiteralChars_74[] = {0};
static JJChar jjstrLiteralChars_75[] = {0};
static JJChar jjstrLiteralChars_76[] = {0};

static JJChar jjstrLiteralChars_77[] = {0};
static JJChar jjstrLiteralChars_78[] = {0};
static JJChar jjstrLiteralChars_79[] = {0};
static JJChar jjstrLiteralChars_80[] = {0};
static JJChar jjstrLiteralChars_81[] = {0};
static JJChar jjstrLiteralChars_82[] = {0};
static JJChar jjstrLiteralChars_83[] = {0};
static JJChar jjstrLiteralChars_84[] = {0};
static JJChar jjstrLiteralChars_85[] = {0};
static JJChar jjstrLiteralChars_86[] = {0};
static JJChar jjstrLiteralChars_87[] = {0};
static JJChar jjstrLiteralChars_88[] = {0};
static JJChar jjstrLiteralChars_89[] = {0};
static JJChar jjstrLiteralChars_90[] = {0};

static JJChar jjstrLiteralChars_91[] = {0};
static JJChar jjstrLiteralChars_92[] = {0};
static JJChar jjstrLiteralChars_93[] = {0};
static JJChar jjstrLiteralChars_94[] = {0};
static JJChar jjstrLiteralChars_95[] = {0};
static JJChar jjstrLiteralChars_96[] = {0};
static JJChar jjstrLiteralChars_97[] = {0};
static JJChar jjstrLiteralChars_98[] = {0};
static JJChar jjstrLiteralChars_99[] = {0};
static JJChar jjstrLiteralChars_100[] = {0};
static JJChar jjstrLiteralChars_101[] = {0};
static JJChar jjstrLiteralChars_102[] = {0};
static JJChar jjstrLiteralChars_103[] = {0};
static JJChar jjstrLiteralChars_104[] = {0};

static JJChar jjstrLiteralChars_105[] = {0};
static JJChar jjstrLiteralChars_106[] = {0};
static JJChar jjstrLiteralChars_107[] = {0};
static JJChar jjstrLiteralChars_108[] = {0};
static JJChar jjstrLiteralChars_109[] = {0};
static JJChar jjstrLiteralChars_110[] = {0};
static JJChar jjstrLiteralChars_111[] = {0};
static JJChar jjstrLiteralChars_112[] = {0};
static JJChar jjstrLiteralChars_113[] = {0};
static JJChar jjstrLiteralChars_114[] = {0};
static JJChar jjstrLiteralChars_115[] = {0};
static JJChar jjstrLiteralChars_116[] = {0};
static JJChar jjstrLiteralChars_117[] = {0};
static JJChar jjstrLiteralChars_118[] = {0};

static JJChar jjstrLiteralChars_119[] = {0};
static JJChar jjstrLiteralChars_120[] = {0};
static JJChar jjstrLiteralChars_121[] = {0};
static JJChar jjstrLiteralChars_122[] = {0};
static JJChar jjstrLiteralChars_123[] = {0};
static JJChar jjstrLiteralChars_124[] = {0};
static JJChar jjstrLiteralChars_125[] = {0};
static JJChar jjstrLiteralChars_126[] = {0};
static JJChar jjstrLiteralChars_127[] = {0};

static JJChar jjstrLiteralChars_128[] = {0x26, 0};
static JJChar jjstrLiteralChars_129[] = {0x27, 0};

static JJChar jjstrLiteralChars_130[] = {0x28, 0};
static JJChar jjstrLiteralChars_131[] = {0x29, 0};

static JJChar jjstrLiteralChars_132[] = {0x2a, 0x2a, 0};
static JJChar jjstrLiteralChars_133[] = {0x2a, 0};

static JJChar jjstrLiteralChars_134[] = {0x2b, 0};
static JJChar jjstrLiteralChars_135[] = {0x2d, 0};

static JJChar jjstrLiteralChars_136[] = {0x2c, 0};
static JJChar jjstrLiteralChars_137[] = {0x3a, 0x3d, 0};

static JJChar jjstrLiteralChars_138[] = {0x3a, 0};
static JJChar jjstrLiteralChars_139[] = {0x3b, 0};

static JJChar jjstrLiteralChars_140[] = {0x3c, 0x3d, 0x3e, 0};
static JJChar jjstrLiteralChars_141[] = {0x3c, 0x3d, 0};

static JJChar jjstrLiteralChars_142[] = {0x3e, 0x3d, 0};
static JJChar jjstrLiteralChars_143[] = {0x3c, 0};

static JJChar jjstrLiteralChars_144[] = {0x3e, 0};
static JJChar jjstrLiteralChars_145[] = {0x3d, 0};

static JJChar jjstrLiteralChars_146[] = {0x2f, 0x3d, 0};
static JJChar jjstrLiteralChars_147[] = {0x3d, 0x3e, 0};

static JJChar jjstrLiteralChars_148[] = {0x3c, 0x3e, 0};
static JJChar jjstrLiteralChars_149[] = {0x3c, 0x3c, 0};

static JJChar jjstrLiteralChars_150[] = {0x3e, 0x3e, 0};
static JJChar jjstrLiteralChars_151[] = {0x3f, 0x3f, 0};

static JJChar jjstrLiteralChars_152[] = {0x3f, 0x3e, 0x3d, 0};
static JJChar jjstrLiteralChars_153[] = {0x3f, 0x3c, 0x3d, 0};

static JJChar jjstrLiteralChars_154[] = {0x3f, 0x3e, 0};
static JJChar jjstrLiteralChars_155[] = {0x3f, 0x3c, 0};

static JJChar jjstrLiteralChars_156[] = {0x3f, 0x3d, 0};
static JJChar jjstrLiteralChars_157[] = {0x3f, 0x2f, 0x3d, 0};

static JJChar jjstrLiteralChars_158[] = {0x3f, 0};
static JJChar jjstrLiteralChars_159[] = {0x7c, 0};

static JJChar jjstrLiteralChars_160[] = {0x2e, 0};
static JJChar jjstrLiteralChars_161[] = {0x2f, 0};

static JJChar jjstrLiteralChars_162[] = {0x40, 0};
static JJChar jjstrLiteralChars_163[] = {0x5e, 0};

static JJChar jjstrLiteralChars_164[] = {0x5b, 0};
static JJChar jjstrLiteralChars_165[] = {0x5d, 0};

static JJChar jjstrLiteralChars_166[] = {0x7b, 0};
static JJChar jjstrLiteralChars_167[] = {0x7d, 0};
static JJChar jjstrLiteralChars_168[] = {0};
static JJChar jjstrLiteralChars_169[] = {0};
static JJChar jjstrLiteralChars_170[] = {0};
static JJChar jjstrLiteralChars_171[] = {0};
static JJChar jjstrLiteralChars_172[] = {0};

static JJChar jjstrLiteralChars_173[] = {0};
static JJChar jjstrLiteralChars_174[] = {0};
static JJChar jjstrLiteralChars_175[] = {0};
static JJChar jjstrLiteralChars_176[] = {0};
static JJChar jjstrLiteralChars_177[] = {0};
static JJChar jjstrLiteralChars_178[] = {0};
static JJChar jjstrLiteralChars_179[] = {0};
static JJChar jjstrLiteralChars_180[] = {0};
static JJChar jjstrLiteralChars_181[] = {0};
static JJChar jjstrLiteralChars_182[] = {0};
static JJChar jjstrLiteralChars_183[] = {0};
static JJChar jjstrLiteralChars_184[] = {0};
static JJChar jjstrLiteralChars_185[] = {0};
static JJChar jjstrLiteralChars_186[] = {0};

static JJChar jjstrLiteralChars_187[] = {0};
static JJChar jjstrLiteralChars_188[] = {0};
static JJChar jjstrLiteralChars_189[] = {0};
static JJChar jjstrLiteralChars_190[] = {0};
static JJChar jjstrLiteralChars_191[] = {0};
static JJChar jjstrLiteralChars_192[] = {0};
static const JJString jjstrLiteralImages[] = {
jjstrLiteralChars_0, 
jjstrLiteralChars_1, 
jjstrLiteralChars_2, 
jjstrLiteralChars_3, 
jjstrLiteralChars_4, 
jjstrLiteralChars_5, 
jjstrLiteralChars_6, 
jjstrLiteralChars_7, 
jjstrLiteralChars_8, 
jjstrLiteralChars_9, 
jjstrLiteralChars_10, 
jjstrLiteralChars_11, 
jjstrLiteralChars_12, 
jjstrLiteralChars_13, 
jjstrLiteralChars_14, 
jjstrLiteralChars_15, 
jjstrLiteralChars_16, 
jjstrLiteralChars_17, 
jjstrLiteralChars_18, 
jjstrLiteralChars_19, 
jjstrLiteralChars_20, 
jjstrLiteralChars_21, 
jjstrLiteralChars_22, 
jjstrLiteralChars_23, 
jjstrLiteralChars_24, 
jjstrLiteralChars_25, 
jjstrLiteralChars_26, 
jjstrLiteralChars_27, 
jjstrLiteralChars_28, 
jjstrLiteralChars_29, 
jjstrLiteralChars_30, 
jjstrLiteralChars_31, 
jjstrLiteralChars_32, 
jjstrLiteralChars_33, 
jjstrLiteralChars_34, 
jjstrLiteralChars_35, 
jjstrLiteralChars_36, 
jjstrLiteralChars_37, 
jjstrLiteralChars_38, 
jjstrLiteralChars_39, 
jjstrLiteralChars_40, 
jjstrLiteralChars_41, 
jjstrLiteralChars_42, 
jjstrLiteralChars_43, 
jjstrLiteralChars_44, 
jjstrLiteralChars_45, 
jjstrLiteralChars_46, 
jjstrLiteralChars_47, 
jjstrLiteralChars_48, 
jjstrLiteralChars_49, 
jjstrLiteralChars_50, 
jjstrLiteralChars_51, 
jjstrLiteralChars_52, 
jjstrLiteralChars_53, 
jjstrLiteralChars_54, 
jjstrLiteralChars_55, 
jjstrLiteralChars_56, 
jjstrLiteralChars_57, 
jjstrLiteralChars_58, 
jjstrLiteralChars_59, 
jjstrLiteralChars_60, 
jjstrLiteralChars_61, 
jjstrLiteralChars_62, 
jjstrLiteralChars_63, 
jjstrLiteralChars_64, 
jjstrLiteralChars_65, 
jjstrLiteralChars_66, 
jjstrLiteralChars_67, 
jjstrLiteralChars_68, 
jjstrLiteralChars_69, 
jjstrLiteralChars_70, 
jjstrLiteralChars_71, 
jjstrLiteralChars_72, 
jjstrLiteralChars_73, 
jjstrLiteralChars_74, 
jjstrLiteralChars_75, 
jjstrLiteralChars_76, 
jjstrLiteralChars_77, 
jjstrLiteralChars_78, 
jjstrLiteralChars_79, 
jjstrLiteralChars_80, 
jjstrLiteralChars_81, 
jjstrLiteralChars_82, 
jjstrLiteralChars_83, 
jjstrLiteralChars_84, 
jjstrLiteralChars_85, 
jjstrLiteralChars_86, 
jjstrLiteralChars_87, 
jjstrLiteralChars_88, 
jjstrLiteralChars_89, 
jjstrLiteralChars_90, 
jjstrLiteralChars_91, 
jjstrLiteralChars_92, 
jjstrLiteralChars_93, 
jjstrLiteralChars_94, 
jjstrLiteralChars_95, 
jjstrLiteralChars_96, 
jjstrLiteralChars_97, 
jjstrLiteralChars_98, 
jjstrLiteralChars_99, 
jjstrLiteralChars_100, 
jjstrLiteralChars_101, 
jjstrLiteralChars_102, 
jjstrLiteralChars_103, 
jjstrLiteralChars_104, 
jjstrLiteralChars_105, 
jjstrLiteralChars_106, 
jjstrLiteralChars_107, 
jjstrLiteralChars_108, 
jjstrLiteralChars_109, 
jjstrLiteralChars_110, 
jjstrLiteralChars_111, 
jjstrLiteralChars_112, 
jjstrLiteralChars_113, 
jjstrLiteralChars_114, 
jjstrLiteralChars_115, 
jjstrLiteralChars_116, 
jjstrLiteralChars_117, 
jjstrLiteralChars_118, 
jjstrLiteralChars_119, 
jjstrLiteralChars_120, 
jjstrLiteralChars_121, 
jjstrLiteralChars_122, 
jjstrLiteralChars_123, 
jjstrLiteralChars_124, 
jjstrLiteralChars_125, 
jjstrLiteralChars_126, 
jjstrLiteralChars_127, 
jjstrLiteralChars_128, 
jjstrLiteralChars_129, 
jjstrLiteralChars_130, 
jjstrLiteralChars_131, 
jjstrLiteralChars_132, 
jjstrLiteralChars_133, 
jjstrLiteralChars_134, 
jjstrLiteralChars_135, 
jjstrLiteralChars_136, 
jjstrLiteralChars_137, 
jjstrLiteralChars_138, 
jjstrLiteralChars_139, 
jjstrLiteralChars_140, 
jjstrLiteralChars_141, 
jjstrLiteralChars_142, 
jjstrLiteralChars_143, 
jjstrLiteralChars_144, 
jjstrLiteralChars_145, 
jjstrLiteralChars_146, 
jjstrLiteralChars_147, 
jjstrLiteralChars_148, 
jjstrLiteralChars_149, 
jjstrLiteralChars_150, 
jjstrLiteralChars_151, 
jjstrLiteralChars_152, 
jjstrLiteralChars_153, 
jjstrLiteralChars_154, 
jjstrLiteralChars_155, 
jjstrLiteralChars_156, 
jjstrLiteralChars_157, 
jjstrLiteralChars_158, 
jjstrLiteralChars_159, 
jjstrLiteralChars_160, 
jjstrLiteralChars_161, 
jjstrLiteralChars_162, 
jjstrLiteralChars_163, 
jjstrLiteralChars_164, 
jjstrLiteralChars_165, 
jjstrLiteralChars_166, 
jjstrLiteralChars_167, 
jjstrLiteralChars_168, 
jjstrLiteralChars_169, 
jjstrLiteralChars_170, 
jjstrLiteralChars_171, 
jjstrLiteralChars_172, 
jjstrLiteralChars_173, 
jjstrLiteralChars_174, 
jjstrLiteralChars_175, 
jjstrLiteralChars_176, 
jjstrLiteralChars_177, 
jjstrLiteralChars_178, 
jjstrLiteralChars_179, 
jjstrLiteralChars_180, 
jjstrLiteralChars_181, 
jjstrLiteralChars_182, 
jjstrLiteralChars_183, 
jjstrLiteralChars_184, 
jjstrLiteralChars_185, 
jjstrLiteralChars_186, 
jjstrLiteralChars_187, 
jjstrLiteralChars_188, 
jjstrLiteralChars_189, 
jjstrLiteralChars_190, 
jjstrLiteralChars_191, 
jjstrLiteralChars_192, 
};

/** Lexer state names. */
static const JJChar lexStateNames_arr_0[] = 
{0x44, 0x45, 0x46, 0x41, 0x55, 0x4c, 0x54, 0};
static const JJString lexStateNames[] = {
lexStateNames_arr_0, 
};
static const unsigned long long jjtoToken[] = {
   0xfffffffffffff801ULL, 0xffffffffffffffffULL, 0x88ffffffffffffULL, 0x1ULL, 
};
static const unsigned long long jjtoSkip[] = {
   0x7deULL, 0x0ULL, 0x0ULL, 0x0ULL, 
};

  void  VhdlParserTokenManager::setDebugStream(FILE *ds){ debugStream = ds; }

 int  VhdlParserTokenManager::jjStopAtPos(int pos, int kind){
   jjmatchedKind = kind;
   jjmatchedPos = pos;
   return pos + 1;
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa0_0(){
   switch(curChar)
   {
      case 9:
         jjmatchedKind = 2;
         return jjMoveNfa_0(11, 0);
      case 10:
         jjmatchedKind = 3;
         return jjMoveNfa_0(11, 0);
      case 13:
         jjmatchedKind = 4;
         return jjMoveNfa_0(11, 0);
      case 32:
         jjmatchedKind = 1;
         return jjMoveNfa_0(11, 0);
      case 38:
         jjmatchedKind = 128;
         return jjMoveNfa_0(11, 0);
      case 39:
         jjmatchedKind = 129;
         return jjMoveNfa_0(11, 0);
      case 40:
         jjmatchedKind = 130;
         return jjMoveNfa_0(11, 0);
      case 41:
         jjmatchedKind = 131;
         return jjMoveNfa_0(11, 0);
      case 42:
         jjmatchedKind = 133;
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x0ULL, 0x10ULL);
      case 43:
         jjmatchedKind = 134;
         return jjMoveNfa_0(11, 0);
      case 44:
         jjmatchedKind = 136;
         return jjMoveNfa_0(11, 0);
      case 45:
         jjmatchedKind = 135;
         return jjMoveNfa_0(11, 0);
      case 46:
         jjmatchedKind = 160;
         return jjMoveNfa_0(11, 0);
      case 47:
         jjmatchedKind = 161;
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x0ULL, 0x40000ULL);
      case 58:
         jjmatchedKind = 138;
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x0ULL, 0x200ULL);
      case 59:
         jjmatchedKind = 139;
         return jjMoveNfa_0(11, 0);
      case 60:
         jjmatchedKind = 143;
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x0ULL, 0x303000ULL);
      case 61:
         jjmatchedKind = 145;
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x0ULL, 0x80000ULL);
      case 62:
         jjmatchedKind = 144;
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x0ULL, 0x404000ULL);
      case 63:
         jjmatchedKind = 158;
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x0ULL, 0x3f800000ULL);
      case 64:
         jjmatchedKind = 162;
         return jjMoveNfa_0(11, 0);
      case 65:
         return jjMoveStringLiteralDfa1_0(0x7ff800ULL, 0x0ULL, 0x0ULL);
      case 66:
         return jjMoveStringLiteralDfa1_0(0xf800000ULL, 0x0ULL, 0x0ULL);
      case 67:
         return jjMoveStringLiteralDfa1_0(0x3f0000000ULL, 0x0ULL, 0x0ULL);
      case 68:
         return jjMoveStringLiteralDfa1_0(0x1c00000000ULL, 0x0ULL, 0x0ULL);
      case 69:
         return jjMoveStringLiteralDfa1_0(0x3e000000000ULL, 0x0ULL, 0x0ULL);
      case 70:
         return jjMoveStringLiteralDfa1_0(0x7c0000000000ULL, 0x0ULL, 0x0ULL);
      case 71:
         return jjMoveStringLiteralDfa1_0(0x7800000000000ULL, 0x0ULL, 0x0ULL);
      case 73:
         return jjMoveStringLiteralDfa1_0(0x1f8000000000000ULL, 0x0ULL, 0x0ULL);
      case 76:
         return jjMoveStringLiteralDfa1_0(0x3e00000000000000ULL, 0x0ULL, 0x0ULL);
      case 77:
         return jjMoveStringLiteralDfa1_0(0xc000000000000000ULL, 0x0ULL, 0x0ULL);
      case 78:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x3fULL, 0x0ULL);
      case 79:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0xfc0ULL, 0x0ULL);
      case 80:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x3ff000ULL, 0x0ULL);
      case 82:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x3ffc00000ULL, 0x0ULL);
      case 83:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x1ffc00000000ULL, 0x0ULL);
      case 84:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x1e00000000000ULL, 0x0ULL);
      case 85:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x1e000000000000ULL, 0x0ULL);
      case 86:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x3e0000000000000ULL, 0x0ULL);
      case 87:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x3c00000000000000ULL, 0x0ULL);
      case 88:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0xc000000000000000ULL, 0x0ULL);
      case 91:
         jjmatchedKind = 164;
         return jjMoveNfa_0(11, 0);
      case 93:
         jjmatchedKind = 165;
         return jjMoveNfa_0(11, 0);
      case 94:
         jjmatchedKind = 163;
         return jjMoveNfa_0(11, 0);
      case 97:
         return jjMoveStringLiteralDfa1_0(0x7ff800ULL, 0x0ULL, 0x0ULL);
      case 98:
         return jjMoveStringLiteralDfa1_0(0xf800000ULL, 0x0ULL, 0x0ULL);
      case 99:
         return jjMoveStringLiteralDfa1_0(0x3f0000000ULL, 0x0ULL, 0x0ULL);
      case 100:
         return jjMoveStringLiteralDfa1_0(0x1c00000000ULL, 0x0ULL, 0x0ULL);
      case 101:
         return jjMoveStringLiteralDfa1_0(0x3e000000000ULL, 0x0ULL, 0x0ULL);
      case 102:
         return jjMoveStringLiteralDfa1_0(0x7c0000000000ULL, 0x0ULL, 0x0ULL);
      case 103:
         return jjMoveStringLiteralDfa1_0(0x7800000000000ULL, 0x0ULL, 0x0ULL);
      case 105:
         return jjMoveStringLiteralDfa1_0(0x1f8000000000000ULL, 0x0ULL, 0x0ULL);
      case 108:
         return jjMoveStringLiteralDfa1_0(0x3e00000000000000ULL, 0x0ULL, 0x0ULL);
      case 109:
         return jjMoveStringLiteralDfa1_0(0xc000000000000000ULL, 0x0ULL, 0x0ULL);
      case 110:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x3fULL, 0x0ULL);
      case 111:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0xfc0ULL, 0x0ULL);
      case 112:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x3ff000ULL, 0x0ULL);
      case 114:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x3ffc00000ULL, 0x0ULL);
      case 115:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x1ffc00000000ULL, 0x0ULL);
      case 116:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x1e00000000000ULL, 0x0ULL);
      case 117:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x1e000000000000ULL, 0x0ULL);
      case 118:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x3e0000000000000ULL, 0x0ULL);
      case 119:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x3c00000000000000ULL, 0x0ULL);
      case 120:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0xc000000000000000ULL, 0x0ULL);
      case 123:
         jjmatchedKind = 166;
         return jjMoveNfa_0(11, 0);
      case 124:
         jjmatchedKind = 159;
         return jjMoveNfa_0(11, 0);
      case 125:
         jjmatchedKind = 167;
         return jjMoveNfa_0(11, 0);
      default :
         return jjMoveNfa_0(11, 0);
   }
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa1_0(unsigned long long active0, unsigned long long active1, unsigned long long active2){
   if (input_stream->endOfInput()) {
   return jjMoveNfa_0(11, 0);
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 42:
         if ((active2 & 0x10ULL) != 0L)
         {
            jjmatchedKind = 132;
            jjmatchedPos = 1;
         }
         break;
      case 47:
         return jjMoveStringLiteralDfa2_0(active0, 0L, active1, 0L, active2, 0x20000000ULL);
      case 60:
         if ((active2 & 0x200000ULL) != 0L)
         {
            jjmatchedKind = 149;
            jjmatchedPos = 1;
         }
         else if ((active2 & 0x8000000ULL) != 0L)
         {
            jjmatchedKind = 155;
            jjmatchedPos = 1;
         }
         return jjMoveStringLiteralDfa2_0(active0, 0L, active1, 0L, active2, 0x2000000ULL);
      case 61:
         if ((active2 & 0x200ULL) != 0L)
         {
            jjmatchedKind = 137;
            jjmatchedPos = 1;
         }
         else if ((active2 & 0x2000ULL) != 0L)
         {
            jjmatchedKind = 141;
            jjmatchedPos = 1;
         }
         else if ((active2 & 0x4000ULL) != 0L)
         {
            jjmatchedKind = 142;
            jjmatchedPos = 1;
         }
         else if ((active2 & 0x40000ULL) != 0L)
         {
            jjmatchedKind = 146;
            jjmatchedPos = 1;
         }
         else if ((active2 & 0x10000000ULL) != 0L)
         {
            jjmatchedKind = 156;
            jjmatchedPos = 1;
         }
         return jjMoveStringLiteralDfa2_0(active0, 0L, active1, 0L, active2, 0x1000ULL);
      case 62:
         if ((active2 & 0x80000ULL) != 0L)
         {
            jjmatchedKind = 147;
            jjmatchedPos = 1;
         }
         else if ((active2 & 0x100000ULL) != 0L)
         {
            jjmatchedKind = 148;
            jjmatchedPos = 1;
         }
         else if ((active2 & 0x400000ULL) != 0L)
         {
            jjmatchedKind = 150;
            jjmatchedPos = 1;
         }
         else if ((active2 & 0x4000000ULL) != 0L)
         {
            jjmatchedKind = 154;
            jjmatchedPos = 1;
         }
         return jjMoveStringLiteralDfa2_0(active0, 0L, active1, 0L, active2, 0x1000000ULL);
      case 63:
         if ((active2 & 0x800000ULL) != 0L)
         {
            jjmatchedKind = 151;
            jjmatchedPos = 1;
         }
         break;
      case 65:
         return jjMoveStringLiteralDfa2_0(active0, 0x4200040020000000ULL, active1, 0x420000000403001ULL, active2, 0L);
      case 66:
         return jjMoveStringLiteralDfa2_0(active0, 0x800ULL, active1, 0L, active2, 0L);
      case 67:
         return jjMoveStringLiteralDfa2_0(active0, 0x1000ULL, active1, 0L, active2, 0L);
      case 69:
         return jjMoveStringLiteralDfa2_0(active0, 0x1800400800000ULL, active1, 0x1e7f800006ULL, active2, 0L);
      case 70:
         if ((active0 & 0x8000000000000ULL) != 0L)
         {
            jjmatchedKind = 51;
            jjmatchedPos = 1;
         }
         else if ((active1 & 0x40ULL) != 0L)
         {
            jjmatchedKind = 70;
            jjmatchedPos = 1;
         }
         return jjMoveStringLiteralDfa2_0(active0, 0x2000ULL, active1, 0L, active2, 0L);
      case 72:
         return jjMoveStringLiteralDfa2_0(active0, 0L, active1, 0x1800204000000000ULL, active2, 0L);
      case 73:
         return jjMoveStringLiteralDfa2_0(active0, 0x1c00080800000000ULL, active1, 0x2040002000000000ULL, active2, 0L);
      case 76:
         return jjMoveStringLiteralDfa2_0(active0, 0x600100c000ULL, active1, 0x18000000000ULL, active2, 0L);
      case 77:
         return jjMoveStringLiteralDfa2_0(active0, 0x10000000000000ULL, active1, 0x80000000000000ULL, active2, 0L);
      case 78:
         if ((active0 & 0x20000000000000ULL) != 0L)
         {
            jjmatchedKind = 53;
            jjmatchedPos = 1;
         }
         else if ((active1 & 0x80ULL) != 0L)
         {
            jjmatchedKind = 71;
            jjmatchedPos = 1;
         }
         return jjMoveStringLiteralDfa2_0(active0, 0xc0018000010000ULL, active1, 0x800e000000000000ULL, active2, 0L);
      case 79:
         if ((active1 & 0x400000000000ULL) != 0L)
         {
            jjmatchedKind = 110;
            jjmatchedPos = 1;
         }
         return jjMoveStringLiteralDfa2_0(active0, 0xa0003013d2000000ULL, active1, 0x400000018000c018ULL, active2, 0L);
      case 80:
         return jjMoveStringLiteralDfa2_0(active0, 0L, active1, 0x100000000000100ULL, active2, 0L);
      case 82:
         if ((active1 & 0x200ULL) != 0L)
         {
            jjmatchedKind = 73;
            jjmatchedPos = 1;
         }
         return jjMoveStringLiteralDfa2_0(active0, 0x2000000060000ULL, active1, 0x8600001f0000ULL, active2, 0L);
      case 83:
         if ((active0 & 0x100000000000000ULL) != 0L)
         {
            jjmatchedKind = 56;
            jjmatchedPos = 1;
         }
         return jjMoveStringLiteralDfa2_0(active0, 0x380000ULL, active1, 0x10000000000000ULL, active2, 0L);
      case 84:
         return jjMoveStringLiteralDfa2_0(active0, 0x400000ULL, active1, 0x80000000400ULL, active2, 0L);
      case 85:
         return jjMoveStringLiteralDfa2_0(active0, 0x440000c000000ULL, active1, 0x200100000200820ULL, active2, 0L);
      case 88:
         return jjMoveStringLiteralDfa2_0(active0, 0x20000000000ULL, active1, 0L, active2, 0L);
      case 89:
         return jjMoveStringLiteralDfa2_0(active0, 0L, active1, 0x1000000000000ULL, active2, 0L);
      case 97:
         return jjMoveStringLiteralDfa2_0(active0, 0x4200040020000000ULL, active1, 0x420000000403001ULL, active2, 0L);
      case 98:
         return jjMoveStringLiteralDfa2_0(active0, 0x800ULL, active1, 0L, active2, 0L);
      case 99:
         return jjMoveStringLiteralDfa2_0(active0, 0x1000ULL, active1, 0L, active2, 0L);
      case 101:
         return jjMoveStringLiteralDfa2_0(active0, 0x1800400800000ULL, active1, 0x1e7f800006ULL, active2, 0L);
      case 102:
         if ((active0 & 0x8000000000000ULL) != 0L)
         {
            jjmatchedKind = 51;
            jjmatchedPos = 1;
         }
         else if ((active1 & 0x40ULL) != 0L)
         {
            jjmatchedKind = 70;
            jjmatchedPos = 1;
         }
         return jjMoveStringLiteralDfa2_0(active0, 0x2000ULL, active1, 0L, active2, 0L);
      case 104:
         return jjMoveStringLiteralDfa2_0(active0, 0L, active1, 0x1800204000000000ULL, active2, 0L);
      case 105:
         return jjMoveStringLiteralDfa2_0(active0, 0x1c00080800000000ULL, active1, 0x2040002000000000ULL, active2, 0L);
      case 108:
         return jjMoveStringLiteralDfa2_0(active0, 0x600100c000ULL, active1, 0x18000000000ULL, active2, 0L);
      case 109:
         return jjMoveStringLiteralDfa2_0(active0, 0x10000000000000ULL, active1, 0x80000000000000ULL, active2, 0L);
      case 110:
         if ((active0 & 0x20000000000000ULL) != 0L)
         {
            jjmatchedKind = 53;
            jjmatchedPos = 1;
         }
         else if ((active1 & 0x80ULL) != 0L)
         {
            jjmatchedKind = 71;
            jjmatchedPos = 1;
         }
         return jjMoveStringLiteralDfa2_0(active0, 0xc0018000010000ULL, active1, 0x800e000000000000ULL, active2, 0L);
      case 111:
         if ((active1 & 0x400000000000ULL) != 0L)
         {
            jjmatchedKind = 110;
            jjmatchedPos = 1;
         }
         return jjMoveStringLiteralDfa2_0(active0, 0xa0003013d2000000ULL, active1, 0x400000018000c018ULL, active2, 0L);
      case 112:
         return jjMoveStringLiteralDfa2_0(active0, 0L, active1, 0x100000000000100ULL, active2, 0L);
      case 114:
         if ((active1 & 0x200ULL) != 0L)
         {
            jjmatchedKind = 73;
            jjmatchedPos = 1;
         }
         return jjMoveStringLiteralDfa2_0(active0, 0x2000000060000ULL, active1, 0x8600001f0000ULL, active2, 0L);
      case 115:
         if ((active0 & 0x100000000000000ULL) != 0L)
         {
            jjmatchedKind = 56;
            jjmatchedPos = 1;
         }
         return jjMoveStringLiteralDfa2_0(active0, 0x380000ULL, active1, 0x10000000000000ULL, active2, 0L);
      case 116:
         return jjMoveStringLiteralDfa2_0(active0, 0x400000ULL, active1, 0x80000000400ULL, active2, 0L);
      case 117:
         return jjMoveStringLiteralDfa2_0(active0, 0x440000c000000ULL, active1, 0x200100000200820ULL, active2, 0L);
      case 120:
         return jjMoveStringLiteralDfa2_0(active0, 0x20000000000ULL, active1, 0L, active2, 0L);
      case 121:
         return jjMoveStringLiteralDfa2_0(active0, 0L, active1, 0x1000000000000ULL, active2, 0L);
      default :
         break;
   }
   return jjMoveNfa_0(11, 1);
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa2_0(unsigned long long old0, unsigned long long active0, unsigned long long old1, unsigned long long active1, unsigned long long old2, unsigned long long active2){
   if (((active0 &= old0) | (active1 &= old1) | (active2 &= old2)) == 0L)
      return jjMoveNfa_0(11, 1);
   if (input_stream->endOfInput()) {
   return jjMoveNfa_0(11, 1);
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 61:
         if ((active2 & 0x1000000ULL) != 0L)
         {
            jjmatchedKind = 152;
            jjmatchedPos = 2;
         }
         else if ((active2 & 0x2000000ULL) != 0L)
         {
            jjmatchedKind = 153;
            jjmatchedPos = 2;
         }
         else if ((active2 & 0x20000000ULL) != 0L)
         {
            jjmatchedKind = 157;
            jjmatchedPos = 2;
         }
         break;
      case 62:
         if ((active2 & 0x1000ULL) != 0L)
         {
            jjmatchedKind = 140;
            jjmatchedPos = 2;
         }
         break;
      case 65:
         if ((active1 & 0x8000000000ULL) != 0L)
         {
            jjmatchedKind = 103;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x20000000000ULL) != 0L)
         {
            jjmatchedKind = 105;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x4000000000000ULL, active1, 0x2804000000000ULL, active2, 0L);
      case 66:
         return jjMoveStringLiteralDfa3_0(active0, 0x600000000000000ULL, active1, 0x100000000000ULL, active2, 0L);
      case 67:
         return jjMoveStringLiteralDfa3_0(active0, 0x21000ULL, active1, 0x801000ULL, active2, 0L);
      case 68:
         if ((active0 & 0x10000ULL) != 0L)
         {
            jjmatchedKind = 16;
            jjmatchedPos = 2;
         }
         else if ((active0 & 0x8000000000ULL) != 0L)
         {
            jjmatchedKind = 39;
            jjmatchedPos = 2;
         }
         else if ((active0 & 0x8000000000000000ULL) != 0L)
         {
            jjmatchedKind = 63;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x2000000ULL, active1, 0L, active2, 0L);
      case 69:
         if ((active1 & 0x10000000000000ULL) != 0L)
         {
            jjmatchedKind = 116;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x40000000000000ULL, active1, 0x840200000000100ULL, active2, 0L);
      case 70:
         return jjMoveStringLiteralDfa3_0(active0, 0x404000000ULL, active1, 0L, active2, 0L);
      case 71:
         return jjMoveStringLiteralDfa3_0(active0, 0x800000ULL, active1, 0x2001000000ULL, active2, 0L);
      case 72:
         return jjMoveStringLiteralDfa3_0(active0, 0L, active1, 0x400ULL, active2, 0L);
      case 73:
         return jjMoveStringLiteralDfa3_0(active0, 0x60000004000ULL, active1, 0x1404000000010000ULL, active2, 0L);
      case 74:
         return jjMoveStringLiteralDfa3_0(active0, 0L, active1, 0x2000000ULL, active2, 0L);
      case 76:
         if ((active0 & 0x8000ULL) != 0L)
         {
            jjmatchedKind = 15;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x80000000ULL) != 0L)
         {
            jjmatchedKind = 95;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x10000000000ULL) != 0L)
         {
            jjmatchedKind = 104;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x40000000000ULL) != 0L)
         {
            jjmatchedKind = 106;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x80000000000ULL, active1, 0x404000020ULL, active2, 0L);
      case 77:
         if ((active1 & 0x20000000ULL) != 0L)
         {
            jjmatchedKind = 93;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x10000000ULL, active1, 0L, active2, 0L);
      case 78:
         return jjMoveStringLiteralDfa3_0(active0, 0x801c001c0000000ULL, active1, 0x200000000400001ULL, active2, 0L);
      case 79:
         return jjMoveStringLiteralDfa3_0(active0, 0x2082000001000000ULL, active1, 0x80800000001e0000ULL, active2, 0L);
      case 80:
         if ((active0 & 0x4000000000000000ULL) != 0L)
         {
            jjmatchedKind = 62;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x10000000000000ULL, active1, 0x1000040000000ULL, active2, 0L);
      case 81:
         return jjMoveStringLiteralDfa3_0(active0, 0L, active1, 0x800000000ULL, active2, 0L);
      case 82:
         if ((active0 & 0x100000000000ULL) != 0L)
         {
            jjmatchedKind = 44;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x8ULL) != 0L)
         {
            jjmatchedKind = 67;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x100000000ULL) != 0L)
         {
            jjmatchedKind = 96;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x4000000000000000ULL) != 0L)
         {
            jjmatchedKind = 126;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x200000040000ULL, active1, 0x120080000206000ULL, active2, 0L);
      case 83:
         if ((active0 & 0x800ULL) != 0L)
         {
            jjmatchedKind = 11;
            jjmatchedPos = 2;
         }
         else if ((active0 & 0x8000000ULL) != 0L)
         {
            jjmatchedKind = 27;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x6820380000ULL, active1, 0x18008000ULL, active2, 0L);
      case 84:
         if ((active1 & 0x10ULL) != 0L)
         {
            jjmatchedKind = 68;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x800ULL) != 0L)
         {
            jjmatchedKind = 75;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x1000010000402000ULL, active1, 0x2008000200000000ULL, active2, 0L);
      case 86:
         return jjMoveStringLiteralDfa3_0(active0, 0x200000000ULL, active1, 0x1000000000ULL, active2, 0L);
      case 87:
         if ((active1 & 0x2ULL) != 0L)
         {
            jjmatchedKind = 65;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x1000000000ULL, active1, 0L, active2, 0L);
      case 88:
         return jjMoveStringLiteralDfa3_0(active0, 0L, active1, 0x4ULL, active2, 0L);
      case 97:
         if ((active1 & 0x8000000000ULL) != 0L)
         {
            jjmatchedKind = 103;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x20000000000ULL) != 0L)
         {
            jjmatchedKind = 105;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x4000000000000ULL, active1, 0x2804000000000ULL, active2, 0L);
      case 98:
         return jjMoveStringLiteralDfa3_0(active0, 0x600000000000000ULL, active1, 0x100000000000ULL, active2, 0L);
      case 99:
         return jjMoveStringLiteralDfa3_0(active0, 0x21000ULL, active1, 0x801000ULL, active2, 0L);
      case 100:
         if ((active0 & 0x10000ULL) != 0L)
         {
            jjmatchedKind = 16;
            jjmatchedPos = 2;
         }
         else if ((active0 & 0x8000000000ULL) != 0L)
         {
            jjmatchedKind = 39;
            jjmatchedPos = 2;
         }
         else if ((active0 & 0x8000000000000000ULL) != 0L)
         {
            jjmatchedKind = 63;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x2000000ULL, active1, 0L, active2, 0L);
      case 101:
         if ((active1 & 0x10000000000000ULL) != 0L)
         {
            jjmatchedKind = 116;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x40000000000000ULL, active1, 0x840200000000100ULL, active2, 0L);
      case 102:
         return jjMoveStringLiteralDfa3_0(active0, 0x404000000ULL, active1, 0L, active2, 0L);
      case 103:
         return jjMoveStringLiteralDfa3_0(active0, 0x800000ULL, active1, 0x2001000000ULL, active2, 0L);
      case 104:
         return jjMoveStringLiteralDfa3_0(active0, 0L, active1, 0x400ULL, active2, 0L);
      case 105:
         return jjMoveStringLiteralDfa3_0(active0, 0x60000004000ULL, active1, 0x1404000000010000ULL, active2, 0L);
      case 106:
         return jjMoveStringLiteralDfa3_0(active0, 0L, active1, 0x2000000ULL, active2, 0L);
      case 108:
         if ((active0 & 0x8000ULL) != 0L)
         {
            jjmatchedKind = 15;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x80000000ULL) != 0L)
         {
            jjmatchedKind = 95;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x10000000000ULL) != 0L)
         {
            jjmatchedKind = 104;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x40000000000ULL) != 0L)
         {
            jjmatchedKind = 106;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x80000000000ULL, active1, 0x404000020ULL, active2, 0L);
      case 109:
         if ((active1 & 0x20000000ULL) != 0L)
         {
            jjmatchedKind = 93;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x10000000ULL, active1, 0L, active2, 0L);
      case 110:
         return jjMoveStringLiteralDfa3_0(active0, 0x801c001c0000000ULL, active1, 0x200000000400001ULL, active2, 0L);
      case 111:
         return jjMoveStringLiteralDfa3_0(active0, 0x2082000001000000ULL, active1, 0x80800000001e0000ULL, active2, 0L);
      case 112:
         if ((active0 & 0x4000000000000000ULL) != 0L)
         {
            jjmatchedKind = 62;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x10000000000000ULL, active1, 0x1000040000000ULL, active2, 0L);
      case 113:
         return jjMoveStringLiteralDfa3_0(active0, 0L, active1, 0x800000000ULL, active2, 0L);
      case 114:
         if ((active0 & 0x100000000000ULL) != 0L)
         {
            jjmatchedKind = 44;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x8ULL) != 0L)
         {
            jjmatchedKind = 67;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x100000000ULL) != 0L)
         {
            jjmatchedKind = 96;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x4000000000000000ULL) != 0L)
         {
            jjmatchedKind = 126;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x200000040000ULL, active1, 0x120080000206000ULL, active2, 0L);
      case 115:
         if ((active0 & 0x800ULL) != 0L)
         {
            jjmatchedKind = 11;
            jjmatchedPos = 2;
         }
         else if ((active0 & 0x8000000ULL) != 0L)
         {
            jjmatchedKind = 27;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x6820380000ULL, active1, 0x18008000ULL, active2, 0L);
      case 116:
         if ((active1 & 0x10ULL) != 0L)
         {
            jjmatchedKind = 68;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x800ULL) != 0L)
         {
            jjmatchedKind = 75;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x1000010000402000ULL, active1, 0x2008000200000000ULL, active2, 0L);
      case 118:
         return jjMoveStringLiteralDfa3_0(active0, 0x200000000ULL, active1, 0x1000000000ULL, active2, 0L);
      case 119:
         if ((active1 & 0x2ULL) != 0L)
         {
            jjmatchedKind = 65;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x1000000000ULL, active1, 0L, active2, 0L);
      case 120:
         return jjMoveStringLiteralDfa3_0(active0, 0L, active1, 0x4ULL, active2, 0L);
      default :
         break;
   }
   return jjMoveNfa_0(11, 2);
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa3_0(unsigned long long old0, unsigned long long active0, unsigned long long old1, unsigned long long active1, unsigned long long old2, unsigned long long active2){
   if (((active0 &= old0) | (active1 &= old1) | (active2 &= old2)) == 0L)
      return jjMoveNfa_0(11, 2);
   if (input_stream->endOfInput()) {
   return jjMoveNfa_0(11, 2);
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 65:
         return jjMoveStringLiteralDfa4_0(active0, 0x400044000ULL, active1, 0x2000ULL);
      case 67:
         return jjMoveStringLiteralDfa4_0(active0, 0x600801000000ULL, active1, 0x60000ULL);
      case 68:
         if ((active1 & 0x1ULL) != 0L)
         {
            jjmatchedKind = 64;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0L, active1, 0x80000000000000ULL);
      case 69:
         if ((active0 & 0x20000000ULL) != 0L)
         {
            jjmatchedKind = 29;
            jjmatchedPos = 3;
         }
         else if ((active0 & 0x2000000000ULL) != 0L)
         {
            jjmatchedKind = 37;
            jjmatchedPos = 3;
         }
         else if ((active0 & 0x80000000000ULL) != 0L)
         {
            jjmatchedKind = 43;
            jjmatchedPos = 3;
         }
         else if ((active1 & 0x200000ULL) != 0L)
         {
            jjmatchedKind = 85;
            jjmatchedPos = 3;
         }
         else if ((active1 & 0x1000000000000ULL) != 0L)
         {
            jjmatchedKind = 112;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x1201800200083000ULL, active1, 0x1406000400ULL);
      case 70:
         return jjMoveStringLiteralDfa4_0(active0, 0x44000000ULL, active1, 0x2000000000000ULL);
      case 71:
         return jjMoveStringLiteralDfa4_0(active0, 0L, active1, 0x400000ULL);
      case 72:
         if ((active1 & 0x2000000000000000ULL) != 0L)
         {
            jjmatchedKind = 125;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x20000ULL, active1, 0L);
      case 73:
         return jjMoveStringLiteralDfa4_0(active0, 0x14000800000ULL, active1, 0x228000001000000ULL);
      case 75:
         return jjMoveStringLiteralDfa4_0(active0, 0x800000000000000ULL, active1, 0x1000ULL);
      case 76:
         if ((active1 & 0x20ULL) != 0L)
         {
            jjmatchedKind = 69;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0L, active1, 0x1000000000000000ULL);
      case 78:
         if ((active1 & 0x100ULL) != 0L)
         {
            jjmatchedKind = 72;
            jjmatchedPos = 3;
         }
         else if ((active1 & 0x200000000000ULL) != 0L)
         {
            jjmatchedKind = 109;
            jjmatchedPos = 3;
         }
         else if ((active1 & 0x800000000000000ULL) != 0L)
         {
            jjmatchedKind = 123;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x1000000000ULL, active1, 0x802000000000ULL);
      case 79:
         return jjMoveStringLiteralDfa4_0(active0, 0L, active1, 0x100080040800000ULL);
      case 80:
         if ((active0 & 0x2000000000000000ULL) != 0L)
         {
            jjmatchedKind = 61;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x10000000ULL, active1, 0x80000ULL);
      case 82:
         if ((active1 & 0x8000000000000000ULL) != 0L)
         {
            jjmatchedKind = 127;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x444040000400000ULL, active1, 0x4000000000ULL);
      case 83:
         return jjMoveStringLiteralDfa4_0(active0, 0x80000000ULL, active1, 0L);
      case 84:
         if ((active0 & 0x20000000000ULL) != 0L)
         {
            jjmatchedKind = 41;
            jjmatchedPos = 3;
         }
         else if ((active1 & 0x4ULL) != 0L)
         {
            jjmatchedKind = 66;
            jjmatchedPos = 3;
         }
         else if ((active1 & 0x4000ULL) != 0L)
         {
            jjmatchedKind = 78;
            jjmatchedPos = 3;
         }
         else if ((active1 & 0x400000000000000ULL) != 0L)
         {
            jjmatchedKind = 122;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x100000000ULL, active1, 0x4100018108000ULL);
      case 85:
         return jjMoveStringLiteralDfa4_0(active0, 0x92000000300000ULL, active1, 0xa00000000ULL);
      case 86:
         return jjMoveStringLiteralDfa4_0(active0, 0L, active1, 0x10000ULL);
      case 87:
         if ((active1 & 0x40000000000000ULL) != 0L)
         {
            jjmatchedKind = 118;
            jjmatchedPos = 3;
         }
         break;
      case 89:
         if ((active0 & 0x2000000ULL) != 0L)
         {
            jjmatchedKind = 25;
            jjmatchedPos = 3;
         }
         break;
      case 97:
         return jjMoveStringLiteralDfa4_0(active0, 0x400044000ULL, active1, 0x2000ULL);
      case 99:
         return jjMoveStringLiteralDfa4_0(active0, 0x600801000000ULL, active1, 0x60000ULL);
      case 100:
         if ((active1 & 0x1ULL) != 0L)
         {
            jjmatchedKind = 64;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0L, active1, 0x80000000000000ULL);
      case 101:
         if ((active0 & 0x20000000ULL) != 0L)
         {
            jjmatchedKind = 29;
            jjmatchedPos = 3;
         }
         else if ((active0 & 0x2000000000ULL) != 0L)
         {
            jjmatchedKind = 37;
            jjmatchedPos = 3;
         }
         else if ((active0 & 0x80000000000ULL) != 0L)
         {
            jjmatchedKind = 43;
            jjmatchedPos = 3;
         }
         else if ((active1 & 0x200000ULL) != 0L)
         {
            jjmatchedKind = 85;
            jjmatchedPos = 3;
         }
         else if ((active1 & 0x1000000000000ULL) != 0L)
         {
            jjmatchedKind = 112;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x1201800200083000ULL, active1, 0x1406000400ULL);
      case 102:
         return jjMoveStringLiteralDfa4_0(active0, 0x44000000ULL, active1, 0x2000000000000ULL);
      case 103:
         return jjMoveStringLiteralDfa4_0(active0, 0L, active1, 0x400000ULL);
      case 104:
         if ((active1 & 0x2000000000000000ULL) != 0L)
         {
            jjmatchedKind = 125;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x20000ULL, active1, 0L);
      case 105:
         return jjMoveStringLiteralDfa4_0(active0, 0x14000800000ULL, active1, 0x228000001000000ULL);
      case 107:
         return jjMoveStringLiteralDfa4_0(active0, 0x800000000000000ULL, active1, 0x1000ULL);
      case 108:
         if ((active1 & 0x20ULL) != 0L)
         {
            jjmatchedKind = 69;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0L, active1, 0x1000000000000000ULL);
      case 110:
         if ((active1 & 0x100ULL) != 0L)
         {
            jjmatchedKind = 72;
            jjmatchedPos = 3;
         }
         else if ((active1 & 0x200000000000ULL) != 0L)
         {
            jjmatchedKind = 109;
            jjmatchedPos = 3;
         }
         else if ((active1 & 0x800000000000000ULL) != 0L)
         {
            jjmatchedKind = 123;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x1000000000ULL, active1, 0x802000000000ULL);
      case 111:
         return jjMoveStringLiteralDfa4_0(active0, 0L, active1, 0x100080040800000ULL);
      case 112:
         if ((active0 & 0x2000000000000000ULL) != 0L)
         {
            jjmatchedKind = 61;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x10000000ULL, active1, 0x80000ULL);
      case 114:
         if ((active1 & 0x8000000000000000ULL) != 0L)
         {
            jjmatchedKind = 127;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x444040000400000ULL, active1, 0x4000000000ULL);
      case 115:
         return jjMoveStringLiteralDfa4_0(active0, 0x80000000ULL, active1, 0L);
      case 116:
         if ((active0 & 0x20000000000ULL) != 0L)
         {
            jjmatchedKind = 41;
            jjmatchedPos = 3;
         }
         else if ((active1 & 0x4ULL) != 0L)
         {
            jjmatchedKind = 66;
            jjmatchedPos = 3;
         }
         else if ((active1 & 0x4000ULL) != 0L)
         {
            jjmatchedKind = 78;
            jjmatchedPos = 3;
         }
         else if ((active1 & 0x400000000000000ULL) != 0L)
         {
            jjmatchedKind = 122;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x100000000ULL, active1, 0x4100018108000ULL);
      case 117:
         return jjMoveStringLiteralDfa4_0(active0, 0x92000000300000ULL, active1, 0xa00000000ULL);
      case 118:
         return jjMoveStringLiteralDfa4_0(active0, 0L, active1, 0x10000ULL);
      case 119:
         if ((active1 & 0x40000000000000ULL) != 0L)
         {
            jjmatchedKind = 118;
            jjmatchedPos = 3;
         }
         break;
      case 121:
         if ((active0 & 0x2000000ULL) != 0L)
         {
            jjmatchedKind = 25;
            jjmatchedPos = 3;
         }
         break;
      default :
         break;
   }
   return jjMoveNfa_0(11, 3);
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa4_0(unsigned long long old0, unsigned long long active0, unsigned long long old1, unsigned long long active1){
   if (((active0 &= old0) | (active1 &= old1)) == 0L)
      return jjMoveNfa_0(11, 3);
   if (input_stream->endOfInput()) {
   return jjMoveNfa_0(11, 3);
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 65:
         return jjMoveStringLiteralDfa5_0(active0, 0xc00000000000000ULL, active1, 0x20002004011000ULL);
      case 67:
         return jjMoveStringLiteralDfa5_0(active0, 0L, active1, 0x402000000ULL);
      case 68:
         return jjMoveStringLiteralDfa5_0(active0, 0x4000000000000ULL, active1, 0L);
      case 69:
         if ((active0 & 0x200000000000ULL) != 0L)
         {
            jjmatchedKind = 45;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x400000ULL) != 0L)
         {
            jjmatchedKind = 86;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x80000000000000ULL) != 0L)
         {
            jjmatchedKind = 119;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x1000000000000000ULL) != 0L)
         {
            jjmatchedKind = 124;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0x104000000ULL, active1, 0x48001e0000ULL);
      case 70:
         if ((active0 & 0x4000000000ULL) != 0L)
         {
            jjmatchedKind = 38;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0L, active1, 0x2000000000000ULL);
      case 73:
         return jjMoveStringLiteralDfa5_0(active0, 0x40420000ULL, active1, 0L);
      case 75:
         if ((active0 & 0x1000000ULL) != 0L)
         {
            jjmatchedKind = 24;
            jjmatchedPos = 4;
         }
         break;
      case 76:
         if ((active0 & 0x200000000000000ULL) != 0L)
         {
            jjmatchedKind = 57;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x8000000000000ULL) != 0L)
         {
            jjmatchedKind = 115;
            jjmatchedPos = 4;
         }
         break;
      case 77:
         return jjMoveStringLiteralDfa5_0(active0, 0x300000ULL, active1, 0x2000ULL);
      case 78:
         if ((active0 & 0x800000ULL) != 0L)
         {
            jjmatchedKind = 23;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0x40000000000ULL, active1, 0x80000000000ULL);
      case 79:
         return jjMoveStringLiteralDfa5_0(active0, 0x810000000ULL, active1, 0L);
      case 80:
         if ((active0 & 0x2000000000000ULL) != 0L)
         {
            jjmatchedKind = 49;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x100000000000000ULL) != 0L)
         {
            jjmatchedKind = 120;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0L, active1, 0x8000ULL);
      case 82:
         if ((active0 & 0x2000ULL) != 0L)
         {
            jjmatchedKind = 13;
            jjmatchedPos = 4;
         }
         else if ((active0 & 0x200000000ULL) != 0L)
         {
            jjmatchedKind = 33;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0x1011800000080000ULL, active1, 0x1258800400ULL);
      case 83:
         if ((active0 & 0x4000ULL) != 0L)
         {
            jjmatchedKind = 14;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x4000000000000ULL) != 0L)
         {
            jjmatchedKind = 114;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0x1000ULL, active1, 0x800001000000ULL);
      case 84:
         if ((active0 & 0x80000000000000ULL) != 0L)
         {
            jjmatchedKind = 55;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x200000000000000ULL) != 0L)
         {
            jjmatchedKind = 121;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0x40411080000000ULL, active1, 0L);
      case 85:
         return jjMoveStringLiteralDfa5_0(active0, 0x400000000ULL, active1, 0L);
      case 89:
         if ((active0 & 0x40000ULL) != 0L)
         {
            jjmatchedKind = 18;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0L, active1, 0x100000000000ULL);
      case 97:
         return jjMoveStringLiteralDfa5_0(active0, 0xc00000000000000ULL, active1, 0x20002004011000ULL);
      case 99:
         return jjMoveStringLiteralDfa5_0(active0, 0L, active1, 0x402000000ULL);
      case 100:
         return jjMoveStringLiteralDfa5_0(active0, 0x4000000000000ULL, active1, 0L);
      case 101:
         if ((active0 & 0x200000000000ULL) != 0L)
         {
            jjmatchedKind = 45;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x400000ULL) != 0L)
         {
            jjmatchedKind = 86;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x80000000000000ULL) != 0L)
         {
            jjmatchedKind = 119;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x1000000000000000ULL) != 0L)
         {
            jjmatchedKind = 124;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0x104000000ULL, active1, 0x48001e0000ULL);
      case 102:
         if ((active0 & 0x4000000000ULL) != 0L)
         {
            jjmatchedKind = 38;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0L, active1, 0x2000000000000ULL);
      case 105:
         return jjMoveStringLiteralDfa5_0(active0, 0x40420000ULL, active1, 0L);
      case 107:
         if ((active0 & 0x1000000ULL) != 0L)
         {
            jjmatchedKind = 24;
            jjmatchedPos = 4;
         }
         break;
      case 108:
         if ((active0 & 0x200000000000000ULL) != 0L)
         {
            jjmatchedKind = 57;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x8000000000000ULL) != 0L)
         {
            jjmatchedKind = 115;
            jjmatchedPos = 4;
         }
         break;
      case 109:
         return jjMoveStringLiteralDfa5_0(active0, 0x300000ULL, active1, 0x2000ULL);
      case 110:
         if ((active0 & 0x800000ULL) != 0L)
         {
            jjmatchedKind = 23;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0x40000000000ULL, active1, 0x80000000000ULL);
      case 111:
         return jjMoveStringLiteralDfa5_0(active0, 0x810000000ULL, active1, 0L);
      case 112:
         if ((active0 & 0x2000000000000ULL) != 0L)
         {
            jjmatchedKind = 49;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x100000000000000ULL) != 0L)
         {
            jjmatchedKind = 120;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0L, active1, 0x8000ULL);
      case 114:
         if ((active0 & 0x2000ULL) != 0L)
         {
            jjmatchedKind = 13;
            jjmatchedPos = 4;
         }
         else if ((active0 & 0x200000000ULL) != 0L)
         {
            jjmatchedKind = 33;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0x1011800000080000ULL, active1, 0x1258800400ULL);
      case 115:
         if ((active0 & 0x4000ULL) != 0L)
         {
            jjmatchedKind = 14;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x4000000000000ULL) != 0L)
         {
            jjmatchedKind = 114;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0x1000ULL, active1, 0x800001000000ULL);
      case 116:
         if ((active0 & 0x80000000000000ULL) != 0L)
         {
            jjmatchedKind = 55;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x200000000000000ULL) != 0L)
         {
            jjmatchedKind = 121;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0x40411080000000ULL, active1, 0L);
      case 117:
         return jjMoveStringLiteralDfa5_0(active0, 0x400000000ULL, active1, 0L);
      case 121:
         if ((active0 & 0x40000ULL) != 0L)
         {
            jjmatchedKind = 18;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0L, active1, 0x100000000000ULL);
      default :
         break;
   }
   return jjMoveNfa_0(11, 4);
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa5_0(unsigned long long old0, unsigned long long active0, unsigned long long old1, unsigned long long active1){
   if (((active0 &= old0) | (active1 &= old1)) == 0L)
      return jjMoveNfa_0(11, 4);
   if (input_stream->endOfInput()) {
   return jjMoveNfa_0(11, 4);
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 65:
         return jjMoveStringLiteralDfa6_0(active0, 0x1000800080000000ULL, active1, 0L);
      case 66:
         return jjMoveStringLiteralDfa6_0(active0, 0x400000ULL, active1, 0x20000000000000ULL);
      case 67:
         return jjMoveStringLiteralDfa6_0(active0, 0L, active1, 0x100000ULL);
      case 68:
         if ((active1 & 0x800000ULL) != 0L)
         {
            jjmatchedKind = 87;
            jjmatchedPos = 5;
         }
         else if ((active1 & 0x4000000000ULL) != 0L)
         {
            jjmatchedKind = 102;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0L, active1, 0x20000ULL);
      case 69:
         if ((active0 & 0x100000ULL) != 0L)
         {
            jjmatchedKind = 20;
            jjmatchedPos = 5;
         }
         else if ((active0 & 0x10000000000000ULL) != 0L)
         {
            jjmatchedKind = 52;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0x4040000200000ULL, active1, 0x2000000002000ULL);
      case 71:
         if ((active1 & 0x80000000000ULL) != 0L)
         {
            jjmatchedKind = 107;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0x800000040000000ULL, active1, 0x1000ULL);
      case 73:
         return jjMoveStringLiteralDfa6_0(active0, 0x41400000000000ULL, active1, 0x1018000000ULL);
      case 76:
         if ((active1 & 0x2000000000ULL) != 0L)
         {
            jjmatchedKind = 101;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0x400000000ULL, active1, 0L);
      case 78:
         if ((active1 & 0x200000000ULL) != 0L)
         {
            jjmatchedKind = 97;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0x810000000ULL, active1, 0x800000000ULL);
      case 79:
         if ((active0 & 0x1000000000ULL) != 0L)
         {
            jjmatchedKind = 36;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0L, active1, 0x8000ULL);
      case 80:
         return jjMoveStringLiteralDfa6_0(active0, 0L, active1, 0x900000000000ULL);
      case 82:
         if ((active0 & 0x4000000ULL) != 0L)
         {
            jjmatchedKind = 26;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0x400000000000000ULL, active1, 0x80000ULL);
      case 83:
         if ((active0 & 0x1000ULL) != 0L)
         {
            jjmatchedKind = 12;
            jjmatchedPos = 5;
         }
         else if ((active1 & 0x400ULL) != 0L)
         {
            jjmatchedKind = 74;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0L, active1, 0x4040000ULL);
      case 84:
         if ((active0 & 0x80000ULL) != 0L)
         {
            jjmatchedKind = 19;
            jjmatchedPos = 5;
         }
         else if ((active1 & 0x2000000ULL) != 0L)
         {
            jjmatchedKind = 89;
            jjmatchedPos = 5;
         }
         else if ((active1 & 0x40000000ULL) != 0L)
         {
            jjmatchedKind = 94;
            jjmatchedPos = 5;
         }
         else if ((active1 & 0x400000000ULL) != 0L)
         {
            jjmatchedKind = 98;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0x20000ULL, active1, 0x1010000ULL);
      case 88:
         return jjMoveStringLiteralDfa6_0(active0, 0x100000000ULL, active1, 0L);
      case 89:
         if ((active0 & 0x10000000000ULL) != 0L)
         {
            jjmatchedKind = 40;
            jjmatchedPos = 5;
         }
         break;
      case 97:
         return jjMoveStringLiteralDfa6_0(active0, 0x1000800080000000ULL, active1, 0L);
      case 98:
         return jjMoveStringLiteralDfa6_0(active0, 0x400000ULL, active1, 0x20000000000000ULL);
      case 99:
         return jjMoveStringLiteralDfa6_0(active0, 0L, active1, 0x100000ULL);
      case 100:
         if ((active1 & 0x800000ULL) != 0L)
         {
            jjmatchedKind = 87;
            jjmatchedPos = 5;
         }
         else if ((active1 & 0x4000000000ULL) != 0L)
         {
            jjmatchedKind = 102;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0L, active1, 0x20000ULL);
      case 101:
         if ((active0 & 0x100000ULL) != 0L)
         {
            jjmatchedKind = 20;
            jjmatchedPos = 5;
         }
         else if ((active0 & 0x10000000000000ULL) != 0L)
         {
            jjmatchedKind = 52;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0x4040000200000ULL, active1, 0x2000000002000ULL);
      case 103:
         if ((active1 & 0x80000000000ULL) != 0L)
         {
            jjmatchedKind = 107;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0x800000040000000ULL, active1, 0x1000ULL);
      case 105:
         return jjMoveStringLiteralDfa6_0(active0, 0x41400000000000ULL, active1, 0x1018000000ULL);
      case 108:
         if ((active1 & 0x2000000000ULL) != 0L)
         {
            jjmatchedKind = 101;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0x400000000ULL, active1, 0L);
      case 110:
         if ((active1 & 0x200000000ULL) != 0L)
         {
            jjmatchedKind = 97;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0x810000000ULL, active1, 0x800000000ULL);
      case 111:
         if ((active0 & 0x1000000000ULL) != 0L)
         {
            jjmatchedKind = 36;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0L, active1, 0x8000ULL);
      case 112:
         return jjMoveStringLiteralDfa6_0(active0, 0L, active1, 0x900000000000ULL);
      case 114:
         if ((active0 & 0x4000000ULL) != 0L)
         {
            jjmatchedKind = 26;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0x400000000000000ULL, active1, 0x80000ULL);
      case 115:
         if ((active0 & 0x1000ULL) != 0L)
         {
            jjmatchedKind = 12;
            jjmatchedPos = 5;
         }
         else if ((active1 & 0x400ULL) != 0L)
         {
            jjmatchedKind = 74;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0L, active1, 0x4040000ULL);
      case 116:
         if ((active0 & 0x80000ULL) != 0L)
         {
            jjmatchedKind = 19;
            jjmatchedPos = 5;
         }
         else if ((active1 & 0x2000000ULL) != 0L)
         {
            jjmatchedKind = 89;
            jjmatchedPos = 5;
         }
         else if ((active1 & 0x40000000ULL) != 0L)
         {
            jjmatchedKind = 94;
            jjmatchedPos = 5;
         }
         else if ((active1 & 0x400000000ULL) != 0L)
         {
            jjmatchedKind = 98;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0x20000ULL, active1, 0x1010000ULL);
      case 120:
         return jjMoveStringLiteralDfa6_0(active0, 0x100000000ULL, active1, 0L);
      case 121:
         if ((active0 & 0x10000000000ULL) != 0L)
         {
            jjmatchedKind = 40;
            jjmatchedPos = 5;
         }
         break;
      default :
         break;
   }
   return jjMoveNfa_0(11, 5);
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa6_0(unsigned long long old0, unsigned long long active0, unsigned long long old1, unsigned long long active1){
   if (((active0 &= old0) | (active1 &= old1)) == 0L)
      return jjMoveNfa_0(11, 5);
   if (input_stream->endOfInput()) {
   return jjMoveNfa_0(11, 5);
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 65:
         return jjMoveStringLiteralDfa7_0(active0, 0x40000000000000ULL, active1, 0L);
      case 67:
         if ((active0 & 0x1000000000000ULL) != 0L)
         {
            jjmatchedKind = 48;
            jjmatchedPos = 6;
         }
         return jjMoveStringLiteralDfa7_0(active0, 0L, active1, 0x2000818000000ULL);
      case 68:
         if ((active0 & 0x4000000000000ULL) != 0L)
         {
            jjmatchedKind = 50;
            jjmatchedPos = 6;
         }
         break;
      case 69:
         if ((active0 & 0x800000000000000ULL) != 0L)
         {
            jjmatchedKind = 59;
            jjmatchedPos = 6;
         }
         else if ((active1 & 0x1000ULL) != 0L)
         {
            jjmatchedKind = 76;
            jjmatchedPos = 6;
         }
         else if ((active1 & 0x10000ULL) != 0L)
         {
            jjmatchedKind = 80;
            jjmatchedPos = 6;
         }
         else if ((active1 & 0x4000000ULL) != 0L)
         {
            jjmatchedKind = 90;
            jjmatchedPos = 6;
         }
         else if ((active1 & 0x100000000000ULL) != 0L)
         {
            jjmatchedKind = 108;
            jjmatchedPos = 6;
         }
         return jjMoveStringLiteralDfa7_0(active0, 0x10020000ULL, active1, 0x1000000ULL);
      case 76:
         if ((active0 & 0x1000000000000000ULL) != 0L)
         {
            jjmatchedKind = 60;
            jjmatchedPos = 6;
         }
         return jjMoveStringLiteralDfa7_0(active0, 0L, active1, 0x20000000000000ULL);
      case 78:
         return jjMoveStringLiteralDfa7_0(active0, 0x880000000ULL, active1, 0x8000ULL);
      case 79:
         return jjMoveStringLiteralDfa7_0(active0, 0x400000000000ULL, active1, 0x800000000000ULL);
      case 83:
         if ((active1 & 0x40000ULL) != 0L)
         {
            jjmatchedKind = 82;
            jjmatchedPos = 6;
         }
         return jjMoveStringLiteralDfa7_0(active0, 0x40000000000ULL, active1, 0L);
      case 84:
         if ((active0 & 0x100000000ULL) != 0L)
         {
            jjmatchedKind = 32;
            jjmatchedPos = 6;
         }
         else if ((active0 & 0x400000000ULL) != 0L)
         {
            jjmatchedKind = 34;
            jjmatchedPos = 6;
         }
         return jjMoveStringLiteralDfa7_0(active0, 0x800000000000ULL, active1, 0x1000182000ULL);
      case 85:
         return jjMoveStringLiteralDfa7_0(active0, 0x40400000ULL, active1, 0x20000ULL);
      case 89:
         if ((active0 & 0x400000000000000ULL) != 0L)
         {
            jjmatchedKind = 58;
            jjmatchedPos = 6;
         }
         break;
      case 95:
         return jjMoveStringLiteralDfa7_0(active0, 0x200000ULL, active1, 0L);
      case 97:
         return jjMoveStringLiteralDfa7_0(active0, 0x40000000000000ULL, active1, 0L);
      case 99:
         if ((active0 & 0x1000000000000ULL) != 0L)
         {
            jjmatchedKind = 48;
            jjmatchedPos = 6;
         }
         return jjMoveStringLiteralDfa7_0(active0, 0L, active1, 0x2000818000000ULL);
      case 100:
         if ((active0 & 0x4000000000000ULL) != 0L)
         {
            jjmatchedKind = 50;
            jjmatchedPos = 6;
         }
         break;
      case 101:
         if ((active0 & 0x800000000000000ULL) != 0L)
         {
            jjmatchedKind = 59;
            jjmatchedPos = 6;
         }
         else if ((active1 & 0x1000ULL) != 0L)
         {
            jjmatchedKind = 76;
            jjmatchedPos = 6;
         }
         else if ((active1 & 0x10000ULL) != 0L)
         {
            jjmatchedKind = 80;
            jjmatchedPos = 6;
         }
         else if ((active1 & 0x4000000ULL) != 0L)
         {
            jjmatchedKind = 90;
            jjmatchedPos = 6;
         }
         else if ((active1 & 0x100000000000ULL) != 0L)
         {
            jjmatchedKind = 108;
            jjmatchedPos = 6;
         }
         return jjMoveStringLiteralDfa7_0(active0, 0x10020000ULL, active1, 0x1000000ULL);
      case 108:
         if ((active0 & 0x1000000000000000ULL) != 0L)
         {
            jjmatchedKind = 60;
            jjmatchedPos = 6;
         }
         return jjMoveStringLiteralDfa7_0(active0, 0L, active1, 0x20000000000000ULL);
      case 110:
         return jjMoveStringLiteralDfa7_0(active0, 0x880000000ULL, active1, 0x8000ULL);
      case 111:
         return jjMoveStringLiteralDfa7_0(active0, 0x400000000000ULL, active1, 0x800000000000ULL);
      case 115:
         if ((active1 & 0x40000ULL) != 0L)
         {
            jjmatchedKind = 82;
            jjmatchedPos = 6;
         }
         return jjMoveStringLiteralDfa7_0(active0, 0x40000000000ULL, active1, 0L);
      case 116:
         if ((active0 & 0x100000000ULL) != 0L)
         {
            jjmatchedKind = 32;
            jjmatchedPos = 6;
         }
         else if ((active0 & 0x400000000ULL) != 0L)
         {
            jjmatchedKind = 34;
            jjmatchedPos = 6;
         }
         return jjMoveStringLiteralDfa7_0(active0, 0x800000000000ULL, active1, 0x1000182000ULL);
      case 117:
         return jjMoveStringLiteralDfa7_0(active0, 0x40400000ULL, active1, 0x20000ULL);
      case 121:
         if ((active0 & 0x400000000000000ULL) != 0L)
         {
            jjmatchedKind = 58;
            jjmatchedPos = 6;
         }
         break;
      default :
         break;
   }
   return jjMoveNfa_0(11, 6);
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa7_0(unsigned long long old0, unsigned long long active0, unsigned long long old1, unsigned long long active1){
   if (((active0 &= old0) | (active1 &= old1)) == 0L)
      return jjMoveNfa_0(11, 6);
   if (input_stream->endOfInput()) {
   return jjMoveNfa_0(11, 6);
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 67:
         return jjMoveStringLiteralDfa8_0(active0, 0x20000ULL, active1, 0L);
      case 69:
         if ((active0 & 0x800000000000ULL) != 0L)
         {
            jjmatchedKind = 47;
            jjmatchedPos = 7;
         }
         else if ((active1 & 0x800000000ULL) != 0L)
         {
            jjmatchedKind = 99;
            jjmatchedPos = 7;
         }
         else if ((active1 & 0x20000000000000ULL) != 0L)
         {
            jjmatchedKind = 117;
            jjmatchedPos = 7;
         }
         return jjMoveStringLiteralDfa8_0(active0, 0x800000000ULL, active1, 0x10a000ULL);
      case 71:
         return jjMoveStringLiteralDfa8_0(active0, 0x200000ULL, active1, 0L);
      case 76:
         if ((active0 & 0x40000000000000ULL) != 0L)
         {
            jjmatchedKind = 54;
            jjmatchedPos = 7;
         }
         break;
      case 78:
         if ((active0 & 0x400000000000ULL) != 0L)
         {
            jjmatchedKind = 46;
            jjmatchedPos = 7;
         }
         return jjMoveStringLiteralDfa8_0(active0, 0x10000000ULL, active1, 0L);
      case 82:
         if ((active1 & 0x1000000ULL) != 0L)
         {
            jjmatchedKind = 88;
            jjmatchedPos = 7;
         }
         return jjMoveStringLiteralDfa8_0(active0, 0x40000000ULL, active1, 0x800000020000ULL);
      case 83:
         if ((active0 & 0x40000000000ULL) != 0L)
         {
            jjmatchedKind = 42;
            jjmatchedPos = 7;
         }
         break;
      case 84:
         if ((active0 & 0x80000000ULL) != 0L)
         {
            jjmatchedKind = 31;
            jjmatchedPos = 7;
         }
         else if ((active1 & 0x8000000ULL) != 0L)
         {
            jjmatchedKind = 91;
            jjmatchedPos = 7;
         }
         return jjMoveStringLiteralDfa8_0(active0, 0x400000ULL, active1, 0x2000010000000ULL);
      case 89:
         if ((active1 & 0x80000ULL) != 0L)
         {
            jjmatchedKind = 83;
            jjmatchedPos = 7;
         }
         else if ((active1 & 0x1000000000ULL) != 0L)
         {
            jjmatchedKind = 100;
            jjmatchedPos = 7;
         }
         break;
      case 99:
         return jjMoveStringLiteralDfa8_0(active0, 0x20000ULL, active1, 0L);
      case 101:
         if ((active0 & 0x800000000000ULL) != 0L)
         {
            jjmatchedKind = 47;
            jjmatchedPos = 7;
         }
         else if ((active1 & 0x800000000ULL) != 0L)
         {
            jjmatchedKind = 99;
            jjmatchedPos = 7;
         }
         else if ((active1 & 0x20000000000000ULL) != 0L)
         {
            jjmatchedKind = 117;
            jjmatchedPos = 7;
         }
         return jjMoveStringLiteralDfa8_0(active0, 0x800000000ULL, active1, 0x10a000ULL);
      case 103:
         return jjMoveStringLiteralDfa8_0(active0, 0x200000ULL, active1, 0L);
      case 108:
         if ((active0 & 0x40000000000000ULL) != 0L)
         {
            jjmatchedKind = 54;
            jjmatchedPos = 7;
         }
         break;
      case 110:
         if ((active0 & 0x400000000000ULL) != 0L)
         {
            jjmatchedKind = 46;
            jjmatchedPos = 7;
         }
         return jjMoveStringLiteralDfa8_0(active0, 0x10000000ULL, active1, 0L);
      case 114:
         if ((active1 & 0x1000000ULL) != 0L)
         {
            jjmatchedKind = 88;
            jjmatchedPos = 7;
         }
         return jjMoveStringLiteralDfa8_0(active0, 0x40000000ULL, active1, 0x800000020000ULL);
      case 115:
         if ((active0 & 0x40000000000ULL) != 0L)
         {
            jjmatchedKind = 42;
            jjmatchedPos = 7;
         }
         break;
      case 116:
         if ((active0 & 0x80000000ULL) != 0L)
         {
            jjmatchedKind = 31;
            jjmatchedPos = 7;
         }
         else if ((active1 & 0x8000000ULL) != 0L)
         {
            jjmatchedKind = 91;
            jjmatchedPos = 7;
         }
         return jjMoveStringLiteralDfa8_0(active0, 0x400000ULL, active1, 0x2000010000000ULL);
      case 121:
         if ((active1 & 0x80000ULL) != 0L)
         {
            jjmatchedKind = 83;
            jjmatchedPos = 7;
         }
         else if ((active1 & 0x1000000000ULL) != 0L)
         {
            jjmatchedKind = 100;
            jjmatchedPos = 7;
         }
         break;
      default :
         break;
   }
   return jjMoveNfa_0(11, 7);
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa8_0(unsigned long long old0, unsigned long long active0, unsigned long long old1, unsigned long long active1){
   if (((active0 &= old0) | (active1 &= old1)) == 0L)
      return jjMoveNfa_0(11, 7);
   if (input_stream->endOfInput()) {
   return jjMoveNfa_0(11, 7);
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 65:
         return jjMoveStringLiteralDfa9_0(active0, 0x40000000ULL, active1, 0L);
      case 67:
         return jjMoveStringLiteralDfa9_0(active0, 0x800000000ULL, active1, 0L);
      case 68:
         if ((active1 & 0x8000ULL) != 0L)
         {
            jjmatchedKind = 79;
            jjmatchedPos = 8;
         }
         else if ((active1 & 0x100000ULL) != 0L)
         {
            jjmatchedKind = 84;
            jjmatchedPos = 8;
         }
         break;
      case 69:
         if ((active0 & 0x400000ULL) != 0L)
         {
            jjmatchedKind = 22;
            jjmatchedPos = 8;
         }
         else if ((active1 & 0x20000ULL) != 0L)
         {
            jjmatchedKind = 81;
            jjmatchedPos = 8;
         }
         return jjMoveStringLiteralDfa9_0(active0, 0L, active1, 0x2000000000000ULL);
      case 82:
         if ((active1 & 0x2000ULL) != 0L)
         {
            jjmatchedKind = 77;
            jjmatchedPos = 8;
         }
         break;
      case 84:
         if ((active0 & 0x10000000ULL) != 0L)
         {
            jjmatchedKind = 28;
            jjmatchedPos = 8;
         }
         else if ((active1 & 0x800000000000ULL) != 0L)
         {
            jjmatchedKind = 111;
            jjmatchedPos = 8;
         }
         return jjMoveStringLiteralDfa9_0(active0, 0x20000ULL, active1, 0L);
      case 85:
         return jjMoveStringLiteralDfa9_0(active0, 0x200000ULL, active1, 0L);
      case 95:
         return jjMoveStringLiteralDfa9_0(active0, 0L, active1, 0x10000000ULL);
      case 97:
         return jjMoveStringLiteralDfa9_0(active0, 0x40000000ULL, active1, 0L);
      case 99:
         return jjMoveStringLiteralDfa9_0(active0, 0x800000000ULL, active1, 0L);
      case 100:
         if ((active1 & 0x8000ULL) != 0L)
         {
            jjmatchedKind = 79;
            jjmatchedPos = 8;
         }
         else if ((active1 & 0x100000ULL) != 0L)
         {
            jjmatchedKind = 84;
            jjmatchedPos = 8;
         }
         break;
      case 101:
         if ((active0 & 0x400000ULL) != 0L)
         {
            jjmatchedKind = 22;
            jjmatchedPos = 8;
         }
         else if ((active1 & 0x20000ULL) != 0L)
         {
            jjmatchedKind = 81;
            jjmatchedPos = 8;
         }
         return jjMoveStringLiteralDfa9_0(active0, 0L, active1, 0x2000000000000ULL);
      case 114:
         if ((active1 & 0x2000ULL) != 0L)
         {
            jjmatchedKind = 77;
            jjmatchedPos = 8;
         }
         break;
      case 116:
         if ((active0 & 0x10000000ULL) != 0L)
         {
            jjmatchedKind = 28;
            jjmatchedPos = 8;
         }
         else if ((active1 & 0x800000000000ULL) != 0L)
         {
            jjmatchedKind = 111;
            jjmatchedPos = 8;
         }
         return jjMoveStringLiteralDfa9_0(active0, 0x20000ULL, active1, 0L);
      case 117:
         return jjMoveStringLiteralDfa9_0(active0, 0x200000ULL, active1, 0L);
      default :
         break;
   }
   return jjMoveNfa_0(11, 8);
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa9_0(unsigned long long old0, unsigned long long active0, unsigned long long old1, unsigned long long active1){
   if (((active0 &= old0) | (active1 &= old1)) == 0L)
      return jjMoveNfa_0(11, 8);
   if (input_stream->endOfInput()) {
   return jjMoveNfa_0(11, 8);
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 65:
         return jjMoveStringLiteralDfa10_0(active0, 0x200000ULL, active1, 0L);
      case 68:
         if ((active1 & 0x2000000000000ULL) != 0L)
         {
            jjmatchedKind = 113;
            jjmatchedPos = 9;
         }
         break;
      case 71:
         return jjMoveStringLiteralDfa10_0(active0, 0L, active1, 0x10000000ULL);
      case 84:
         if ((active0 & 0x800000000ULL) != 0L)
         {
            jjmatchedKind = 35;
            jjmatchedPos = 9;
         }
         return jjMoveStringLiteralDfa10_0(active0, 0x40000000ULL, active1, 0L);
      case 85:
         return jjMoveStringLiteralDfa10_0(active0, 0x20000ULL, active1, 0L);
      case 97:
         return jjMoveStringLiteralDfa10_0(active0, 0x200000ULL, active1, 0L);
      case 100:
         if ((active1 & 0x2000000000000ULL) != 0L)
         {
            jjmatchedKind = 113;
            jjmatchedPos = 9;
         }
         break;
      case 103:
         return jjMoveStringLiteralDfa10_0(active0, 0L, active1, 0x10000000ULL);
      case 116:
         if ((active0 & 0x800000000ULL) != 0L)
         {
            jjmatchedKind = 35;
            jjmatchedPos = 9;
         }
         return jjMoveStringLiteralDfa10_0(active0, 0x40000000ULL, active1, 0L);
      case 117:
         return jjMoveStringLiteralDfa10_0(active0, 0x20000ULL, active1, 0L);
      default :
         break;
   }
   return jjMoveNfa_0(11, 9);
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa10_0(unsigned long long old0, unsigned long long active0, unsigned long long old1, unsigned long long active1){
   if (((active0 &= old0) | (active1 &= old1)) == 0L)
      return jjMoveNfa_0(11, 9);
   if (input_stream->endOfInput()) {
   return jjMoveNfa_0(11, 9);
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 73:
         return jjMoveStringLiteralDfa11_0(active0, 0x40000000ULL, active1, 0L);
      case 82:
         return jjMoveStringLiteralDfa11_0(active0, 0x220000ULL, active1, 0L);
      case 85:
         return jjMoveStringLiteralDfa11_0(active0, 0L, active1, 0x10000000ULL);
      case 105:
         return jjMoveStringLiteralDfa11_0(active0, 0x40000000ULL, active1, 0L);
      case 114:
         return jjMoveStringLiteralDfa11_0(active0, 0x220000ULL, active1, 0L);
      case 117:
         return jjMoveStringLiteralDfa11_0(active0, 0L, active1, 0x10000000ULL);
      default :
         break;
   }
   return jjMoveNfa_0(11, 10);
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa11_0(unsigned long long old0, unsigned long long active0, unsigned long long old1, unsigned long long active1){
   if (((active0 &= old0) | (active1 &= old1)) == 0L)
      return jjMoveNfa_0(11, 10);
   if (input_stream->endOfInput()) {
   return jjMoveNfa_0(11, 10);
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 65:
         return jjMoveStringLiteralDfa12_0(active0, 0x200000ULL, active1, 0x10000000ULL);
      case 69:
         if ((active0 & 0x20000ULL) != 0L)
         {
            jjmatchedKind = 17;
            jjmatchedPos = 11;
         }
         break;
      case 79:
         return jjMoveStringLiteralDfa12_0(active0, 0x40000000ULL, active1, 0L);
      case 97:
         return jjMoveStringLiteralDfa12_0(active0, 0x200000ULL, active1, 0x10000000ULL);
      case 101:
         if ((active0 & 0x20000ULL) != 0L)
         {
            jjmatchedKind = 17;
            jjmatchedPos = 11;
         }
         break;
      case 111:
         return jjMoveStringLiteralDfa12_0(active0, 0x40000000ULL, active1, 0L);
      default :
         break;
   }
   return jjMoveNfa_0(11, 11);
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa12_0(unsigned long long old0, unsigned long long active0, unsigned long long old1, unsigned long long active1){
   if (((active0 &= old0) | (active1 &= old1)) == 0L)
      return jjMoveNfa_0(11, 11);
   if (input_stream->endOfInput()) {
   return jjMoveNfa_0(11, 11);
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 78:
         if ((active0 & 0x40000000ULL) != 0L)
         {
            jjmatchedKind = 30;
            jjmatchedPos = 12;
         }
         return jjMoveStringLiteralDfa13_0(active0, 0x200000ULL, active1, 0L);
      case 82:
         return jjMoveStringLiteralDfa13_0(active0, 0L, active1, 0x10000000ULL);
      case 110:
         if ((active0 & 0x40000000ULL) != 0L)
         {
            jjmatchedKind = 30;
            jjmatchedPos = 12;
         }
         return jjMoveStringLiteralDfa13_0(active0, 0x200000ULL, active1, 0L);
      case 114:
         return jjMoveStringLiteralDfa13_0(active0, 0L, active1, 0x10000000ULL);
      default :
         break;
   }
   return jjMoveNfa_0(11, 12);
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa13_0(unsigned long long old0, unsigned long long active0, unsigned long long old1, unsigned long long active1){
   if (((active0 &= old0) | (active1 &= old1)) == 0L)
      return jjMoveNfa_0(11, 12);
   if (input_stream->endOfInput()) {
   return jjMoveNfa_0(11, 12);
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 65:
         return jjMoveStringLiteralDfa14_0(active0, 0L, active1, 0x10000000ULL);
      case 84:
         return jjMoveStringLiteralDfa14_0(active0, 0x200000ULL, active1, 0L);
      case 97:
         return jjMoveStringLiteralDfa14_0(active0, 0L, active1, 0x10000000ULL);
      case 116:
         return jjMoveStringLiteralDfa14_0(active0, 0x200000ULL, active1, 0L);
      default :
         break;
   }
   return jjMoveNfa_0(11, 13);
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa14_0(unsigned long long old0, unsigned long long active0, unsigned long long old1, unsigned long long active1){
   if (((active0 &= old0) | (active1 &= old1)) == 0L)
      return jjMoveNfa_0(11, 13);
   if (input_stream->endOfInput()) {
   return jjMoveNfa_0(11, 13);
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 69:
         return jjMoveStringLiteralDfa15_0(active0, 0x200000ULL, active1, 0L);
      case 78:
         return jjMoveStringLiteralDfa15_0(active0, 0L, active1, 0x10000000ULL);
      case 101:
         return jjMoveStringLiteralDfa15_0(active0, 0x200000ULL, active1, 0L);
      case 110:
         return jjMoveStringLiteralDfa15_0(active0, 0L, active1, 0x10000000ULL);
      default :
         break;
   }
   return jjMoveNfa_0(11, 14);
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa15_0(unsigned long long old0, unsigned long long active0, unsigned long long old1, unsigned long long active1){
   if (((active0 &= old0) | (active1 &= old1)) == 0L)
      return jjMoveNfa_0(11, 14);
   if (input_stream->endOfInput()) {
   return jjMoveNfa_0(11, 14);
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 69:
         if ((active0 & 0x200000ULL) != 0L)
         {
            jjmatchedKind = 21;
            jjmatchedPos = 15;
         }
         break;
      case 84:
         return jjMoveStringLiteralDfa16_0(active0, 0L, active1, 0x10000000ULL);
      case 101:
         if ((active0 & 0x200000ULL) != 0L)
         {
            jjmatchedKind = 21;
            jjmatchedPos = 15;
         }
         break;
      case 116:
         return jjMoveStringLiteralDfa16_0(active0, 0L, active1, 0x10000000ULL);
      default :
         break;
   }
   return jjMoveNfa_0(11, 15);
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa16_0(unsigned long long old0, unsigned long long active0, unsigned long long old1, unsigned long long active1){
   if (((active0 &= old0) | (active1 &= old1)) == 0L)
      return jjMoveNfa_0(11, 15);
   if (input_stream->endOfInput()) {
   return jjMoveNfa_0(11, 15);
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 69:
         return jjMoveStringLiteralDfa17_0(active1, 0x10000000ULL);
      case 101:
         return jjMoveStringLiteralDfa17_0(active1, 0x10000000ULL);
      default :
         break;
   }
   return jjMoveNfa_0(11, 16);
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa17_0(unsigned long long old1, unsigned long long active1){
   if (((active1 &= old1)) == 0L)
      return jjMoveNfa_0(11, 16);
   if (input_stream->endOfInput()) {
   return jjMoveNfa_0(11, 16);
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 69:
         if ((active1 & 0x10000000ULL) != 0L)
         {
            jjmatchedKind = 92;
            jjmatchedPos = 17;
         }
         break;
      case 101:
         if ((active1 & 0x10000000ULL) != 0L)
         {
            jjmatchedKind = 92;
            jjmatchedPos = 17;
         }
         break;
      default :
         break;
   }
   return jjMoveNfa_0(11, 17);
}

int VhdlParserTokenManager::jjMoveNfa_0(int startState, int curPos){
   int strKind = jjmatchedKind;
   int strPos = jjmatchedPos;
   int seenUpto;
   input_stream->backup(seenUpto = curPos + 1);
   assert(!input_stream->endOfInput());
   curChar = input_stream->readChar();
   curPos = 0;
   int startsAt = 0;
   jjnewStateCnt = 83;
   int i = 1;
   jjstateSet[0] = startState;
   int kind = 0x7fffffff;
   for (;;)
   {
      if (++jjround == 0x7fffffff)
         ReInitRounds();
      if (curChar < 64)
      {
         unsigned long long l = 1ULL << curChar;
         (void)l;
         do
         {
            switch(jjstateSet[--i])
            {
               case 11:
                  if ((0x3ff000000000000ULL & l) != 0L)
                  {
                     if (kind > 174)
                        kind = 174;
                     { jjCheckNAdd(82); }
                  }
                  else if ((0x100000200ULL & l) != 0L)
                     { jjCheckNAddTwoStates(0, 3); }
                  else if (curChar == 47)
                     { jjAddStates(0, 1); }
                  else if (curChar == 45)
                     { jjCheckNAddStates(2, 4); }
                  else if (curChar == 39)
                     jjstateSet[jjnewStateCnt++] = 18;
                  else if (curChar == 34)
                     { jjCheckNAddTwoStates(9, 10); }
                  if ((0x3ff000000000000ULL & l) != 0L)
                  {
                     if (kind > 168)
                        kind = 168;
                     { jjCheckNAddStates(5, 17); }
                  }
                  break;
               case 0:
                  if ((0x100000200ULL & l) != 0L)
                     { jjCheckNAddTwoStates(0, 3); }
                  break;
               case 1:
                  if (curChar != 33)
                     break;
                  if (kind > 6)
                     kind = 6;
                  { jjCheckNAddStates(18, 22); }
                  break;
               case 2:
                  if (curChar == 45)
                     jjstateSet[jjnewStateCnt++] = 1;
                  break;
               case 3:
                  if (curChar == 45)
                     { jjCheckNAdd(2); }
                  break;
               case 4:
                  if ((0xffffffffffffdbffULL & l) == 0L)
                     break;
                  if (kind > 6)
                     kind = 6;
                  { jjCheckNAddStates(18, 22); }
                  break;
               case 5:
                  if ((0x2400ULL & l) == 0L)
                     break;
                  if (kind > 6)
                     kind = 6;
                  { jjCheckNAddTwoStates(0, 3); }
                  break;
               case 6:
                  if (curChar != 10)
                     break;
                  if (kind > 6)
                     kind = 6;
                  { jjCheckNAddTwoStates(0, 3); }
                  break;
               case 7:
                  if (curChar == 13)
                     jjstateSet[jjnewStateCnt++] = 6;
                  break;
               case 8:
                  if (curChar == 34)
                     { jjCheckNAddTwoStates(9, 10); }
                  break;
               case 9:
                  if ((0xfffffffb00000200ULL & l) != 0L)
                     { jjCheckNAddTwoStates(9, 10); }
                  break;
               case 10:
                  if (curChar != 34)
                     break;
                  if (kind > 169)
                     kind = 169;
                  jjstateSet[jjnewStateCnt++] = 8;
                  break;
               case 13:
                  if ((0x3ff000000000000ULL & l) == 0L)
                     break;
                  if (kind > 170)
                     kind = 170;
                  { jjAddStates(23, 24); }
                  break;
               case 15:
                  if ((0xfffffffb00000200ULL & l) != 0L)
                     { jjAddStates(25, 26); }
                  break;
               case 17:
                  if (curChar == 39)
                     jjstateSet[jjnewStateCnt++] = 18;
                  break;
               case 18:
                  if ((0xffffffff00000200ULL & l) != 0L)
                     jjstateSet[jjnewStateCnt++] = 19;
                  break;
               case 19:
                  if (curChar == 39 && kind > 172)
                     kind = 172;
                  break;
               case 22:
                  if (curChar == 34)
                     { jjCheckNAdd(23); }
                  break;
               case 23:
                  if ((0x3ff200000000000ULL & l) != 0L)
                     { jjCheckNAddStates(27, 29); }
                  break;
               case 25:
                  if (curChar == 34 && kind > 183)
                     kind = 183;
                  break;
               case 27:
                  if (curChar == 34)
                     { jjCheckNAddTwoStates(28, 29); }
                  break;
               case 28:
                  if ((0xfffffffb00000200ULL & l) != 0L)
                     { jjCheckNAddTwoStates(28, 29); }
                  break;
               case 29:
                  if (curChar != 34)
                     break;
                  if (kind > 192)
                     kind = 192;
                  { jjCheckNAddTwoStates(27, 30); }
                  break;
               case 30:
                  if ((0xfffffffb00000200ULL & l) == 0L)
                     break;
                  if (kind > 192)
                     kind = 192;
                  { jjCheckNAddTwoStates(27, 30); }
                  break;
               case 31:
                  if (curChar == 45)
                     { jjCheckNAddStates(2, 4); }
                  break;
               case 32:
                  if (curChar != 35)
                     break;
                  if (kind > 7)
                     kind = 7;
                  { jjCheckNAddStates(30, 32); }
                  break;
               case 33:
                  if ((0xffffffffffffdbffULL & l) == 0L)
                     break;
                  if (kind > 7)
                     kind = 7;
                  { jjCheckNAddStates(30, 32); }
                  break;
               case 34:
                  if ((0x2400ULL & l) != 0L && kind > 7)
                     kind = 7;
                  break;
               case 35:
                  if (curChar == 10 && kind > 7)
                     kind = 7;
                  break;
               case 36:
                  if (curChar == 13)
                     jjstateSet[jjnewStateCnt++] = 35;
                  break;
               case 37:
                  if (curChar == 45)
                     jjstateSet[jjnewStateCnt++] = 32;
                  break;
               case 38:
                  if (curChar != 45)
                     break;
                  if (kind > 8)
                     kind = 8;
                  { jjCheckNAddStates(33, 35); }
                  break;
               case 39:
                  if ((0xffffffffffffdbffULL & l) == 0L)
                     break;
                  if (kind > 8)
                     kind = 8;
                  { jjCheckNAddStates(33, 35); }
                  break;
               case 40:
                  if ((0x2400ULL & l) != 0L && kind > 8)
                     kind = 8;
                  break;
               case 41:
                  if (curChar == 10 && kind > 8)
                     kind = 8;
                  break;
               case 42:
                  if (curChar == 13)
                     jjstateSet[jjnewStateCnt++] = 41;
                  break;
               case 43:
                  if (curChar == 47)
                     { jjAddStates(0, 1); }
                  break;
               case 44:
                  if (curChar == 33)
                     { jjCheckNAddTwoStates(45, 46); }
                  break;
               case 45:
                  if ((0xfffffbffffffffffULL & l) != 0L)
                     { jjCheckNAddTwoStates(45, 46); }
                  break;
               case 46:
                  if (curChar == 42)
                     { jjCheckNAddStates(36, 38); }
                  break;
               case 47:
                  if ((0xffff7bffffffffffULL & l) != 0L)
                     { jjCheckNAddTwoStates(48, 46); }
                  break;
               case 48:
                  if ((0xfffffbffffffffffULL & l) != 0L)
                     { jjCheckNAddTwoStates(48, 46); }
                  break;
               case 49:
                  if (curChar == 47 && kind > 9)
                     kind = 9;
                  break;
               case 50:
                  if (curChar == 42)
                     jjstateSet[jjnewStateCnt++] = 44;
                  break;
               case 51:
                  if (curChar == 42)
                     { jjCheckNAddTwoStates(52, 53); }
                  break;
               case 52:
                  if ((0xfffffbffffffffffULL & l) != 0L)
                     { jjCheckNAddTwoStates(52, 53); }
                  break;
               case 53:
                  if (curChar == 42)
                     { jjCheckNAddStates(39, 41); }
                  break;
               case 54:
                  if ((0xffff7bffffffffffULL & l) != 0L)
                     { jjCheckNAddTwoStates(55, 53); }
                  break;
               case 55:
                  if ((0xfffffbffffffffffULL & l) != 0L)
                     { jjCheckNAddTwoStates(55, 53); }
                  break;
               case 56:
                  if (curChar == 47 && kind > 10)
                     kind = 10;
                  break;
               case 57:
                  if ((0x3ff000000000000ULL & l) == 0L)
                     break;
                  if (kind > 168)
                     kind = 168;
                  { jjCheckNAddStates(5, 17); }
                  break;
               case 59:
                  if ((0x3ff000000000000ULL & l) == 0L)
                     break;
                  if (kind > 168)
                     kind = 168;
                  { jjCheckNAddTwoStates(58, 59); }
                  break;
               case 61:
                  if ((0x3ff000000000000ULL & l) == 0L)
                     break;
                  if (kind > 173)
                     kind = 173;
                  { jjCheckNAddStates(42, 45); }
                  break;
               case 62:
                  if (curChar == 46)
                     { jjCheckNAdd(63); }
                  break;
               case 63:
                  if ((0x3ff000000000000ULL & l) == 0L)
                     break;
                  if (kind > 173)
                     kind = 173;
                  { jjCheckNAddStates(46, 48); }
                  break;
               case 66:
                  if ((0x280000000000ULL & l) != 0L)
                     { jjCheckNAdd(67); }
                  break;
               case 67:
                  if ((0x3ff000000000000ULL & l) == 0L)
                     break;
                  if (kind > 173)
                     kind = 173;
                  { jjCheckNAddTwoStates(68, 67); }
                  break;
               case 70:
                  if ((0x3ff000000000000ULL & l) != 0L)
                     { jjCheckNAddStates(49, 51); }
                  break;
               case 71:
                  if (curChar == 35)
                     { jjCheckNAdd(72); }
                  break;
               case 72:
                  if ((0x3ff000000000000ULL & l) != 0L)
                     { jjCheckNAddStates(52, 54); }
                  break;
               case 73:
                  if (curChar == 46)
                     { jjCheckNAdd(74); }
                  break;
               case 74:
                  if ((0x3ff000000000000ULL & l) != 0L)
                     { jjCheckNAddTwoStates(74, 75); }
                  break;
               case 75:
                  if (curChar != 35)
                     break;
                  if (kind > 175)
                     kind = 175;
                  jjstateSet[jjnewStateCnt++] = 76;
                  break;
               case 77:
                  if ((0x280000000000ULL & l) != 0L)
                     { jjCheckNAdd(78); }
                  break;
               case 78:
                  if ((0x3ff000000000000ULL & l) == 0L)
                     break;
                  if (kind > 175)
                     kind = 175;
                  { jjCheckNAddTwoStates(79, 78); }
                  break;
               case 80:
                  if ((0x3ff000000000000ULL & l) != 0L)
                     { jjCheckNAddStates(55, 57); }
                  break;
               case 81:
                  if ((0x3ff000000000000ULL & l) != 0L)
                     { jjCheckNAddStates(58, 61); }
                  break;
               case 82:
                  if ((0x3ff000000000000ULL & l) == 0L)
                     break;
                  if (kind > 174)
                     kind = 174;
                  { jjCheckNAdd(82); }
                  break;
               default : break;
            }
         } while(i != startsAt);
      }
      else if (curChar < 128)
      {
         unsigned long long l = 1ULL << (curChar & 077);
         (void)l;
         do
         {
            switch(jjstateSet[--i])
            {
               case 11:
                  if ((0x7fffffe07fffffeULL & l) != 0L)
                  {
                     if (kind > 174)
                        kind = 174;
                     { jjCheckNAdd(82); }
                  }
                  else if (curChar == 96)
                     { jjCheckNAddTwoStates(27, 30); }
                  else if (curChar == 92)
                     { jjCheckNAddTwoStates(15, 16); }
                  if ((0x7fffffe07fffffeULL & l) != 0L)
                  {
                     if (kind > 170)
                        kind = 170;
                     { jjCheckNAddTwoStates(12, 13); }
                  }
                  if ((0x100801401008014ULL & l) != 0L)
                     jjstateSet[jjnewStateCnt++] = 22;
                  else if ((0x280000ULL & l) != 0L)
                     { jjCheckNAddTwoStates(20, 21); }
                  break;
               case 4:
                  if (kind > 6)
                     kind = 6;
                  { jjAddStates(18, 22); }
                  break;
               case 9:
                  if ((0x7fffffffffffffffULL & l) != 0L)
                     { jjAddStates(62, 63); }
                  break;
               case 12:
                  if (curChar == 95)
                     { jjCheckNAddTwoStates(12, 13); }
                  break;
               case 13:
                  if ((0x7fffffe07fffffeULL & l) == 0L)
                     break;
                  if (kind > 170)
                     kind = 170;
                  { jjCheckNAddTwoStates(12, 13); }
                  break;
               case 14:
                  if (curChar == 92)
                     { jjCheckNAddTwoStates(15, 16); }
                  break;
               case 15:
                  if ((0x7fffffffffffffffULL & l) != 0L)
                     { jjCheckNAddTwoStates(15, 16); }
                  break;
               case 16:
                  if (curChar == 92 && kind > 171)
                     kind = 171;
                  break;
               case 18:
                  if ((0x7fffffffffffffffULL & l) != 0L)
                     jjstateSet[jjnewStateCnt++] = 19;
                  break;
               case 20:
                  if ((0x280000ULL & l) != 0L)
                     { jjCheckNAddTwoStates(20, 21); }
                  break;
               case 21:
                  if ((0x100801401008014ULL & l) != 0L)
                     jjstateSet[jjnewStateCnt++] = 22;
                  break;
               case 23:
                  if ((0x7fffffe07fffffeULL & l) != 0L)
                     { jjCheckNAddStates(27, 29); }
                  break;
               case 24:
                  if (curChar == 95)
                     { jjCheckNAddTwoStates(24, 23); }
                  break;
               case 26:
                  if (curChar == 96)
                     { jjCheckNAddTwoStates(27, 30); }
                  break;
               case 28:
                  if ((0x7fffffffffffffffULL & l) != 0L)
                     { jjAddStates(64, 65); }
                  break;
               case 30:
                  if ((0x7fffffffffffffffULL & l) == 0L)
                     break;
                  if (kind > 192)
                     kind = 192;
                  { jjCheckNAddTwoStates(27, 30); }
                  break;
               case 33:
                  if (kind > 7)
                     kind = 7;
                  { jjAddStates(30, 32); }
                  break;
               case 39:
                  if (kind > 8)
                     kind = 8;
                  { jjAddStates(33, 35); }
                  break;
               case 45:
                  { jjCheckNAddTwoStates(45, 46); }
                  break;
               case 47:
               case 48:
                  { jjCheckNAddTwoStates(48, 46); }
                  break;
               case 52:
                  { jjCheckNAddTwoStates(52, 53); }
                  break;
               case 54:
               case 55:
                  { jjCheckNAddTwoStates(55, 53); }
                  break;
               case 58:
                  if (curChar == 95)
                     jjstateSet[jjnewStateCnt++] = 59;
                  break;
               case 60:
                  if (curChar == 95)
                     jjstateSet[jjnewStateCnt++] = 61;
                  break;
               case 64:
                  if (curChar == 95)
                     jjstateSet[jjnewStateCnt++] = 63;
                  break;
               case 65:
                  if ((0x2000000020ULL & l) != 0L)
                     { jjCheckNAddTwoStates(66, 67); }
                  break;
               case 68:
                  if (curChar == 95)
                     { jjCheckNAdd(67); }
                  break;
               case 69:
                  if (curChar == 95)
                     jjstateSet[jjnewStateCnt++] = 70;
                  break;
               case 72:
                  if ((0x7fffffe07fffffeULL & l) != 0L)
                     { jjCheckNAddStates(52, 54); }
                  break;
               case 74:
                  if ((0x7fffffe07fffffeULL & l) != 0L)
                     { jjCheckNAddTwoStates(74, 75); }
                  break;
               case 76:
                  if ((0x2000000020ULL & l) != 0L)
                     { jjCheckNAddTwoStates(77, 78); }
                  break;
               case 79:
                  if (curChar == 95)
                     { jjCheckNAdd(78); }
                  break;
               case 82:
                  if ((0x7fffffe07fffffeULL & l) == 0L)
                     break;
                  if (kind > 174)
                     kind = 174;
                  { jjCheckNAdd(82); }
                  break;
               default : break;
            }
         } while(i != startsAt);
      }
      else
      {
         int hiByte = (curChar >> 8);
         int i1 = hiByte >> 6;
         unsigned long long l1 = 1ULL << (hiByte & 077);
         int i2 = (curChar & 0xff) >> 6;
         unsigned long long l2 = 1ULL << (curChar & 077);
         do
         {
            switch(jjstateSet[--i])
            {
               case 4:
                  if (!jjCanMove_0(hiByte, i1, i2, l1, l2))
                     break;
                  if (kind > 6)
                     kind = 6;
                  { jjAddStates(18, 22); }
                  break;
               case 9:
                  if (jjCanMove_1(hiByte, i1, i2, l1, l2))
                     { jjAddStates(62, 63); }
                  break;
               case 15:
                  if (jjCanMove_1(hiByte, i1, i2, l1, l2))
                     { jjAddStates(25, 26); }
                  break;
               case 18:
                  if (jjCanMove_1(hiByte, i1, i2, l1, l2))
                     jjstateSet[jjnewStateCnt++] = 19;
                  break;
               case 28:
                  if (jjCanMove_1(hiByte, i1, i2, l1, l2))
                     { jjAddStates(64, 65); }
                  break;
               case 30:
                  if (!jjCanMove_1(hiByte, i1, i2, l1, l2))
                     break;
                  if (kind > 192)
                     kind = 192;
                  { jjAddStates(66, 67); }
                  break;
               case 33:
                  if (!jjCanMove_0(hiByte, i1, i2, l1, l2))
                     break;
                  if (kind > 7)
                     kind = 7;
                  { jjAddStates(30, 32); }
                  break;
               case 39:
                  if (!jjCanMove_0(hiByte, i1, i2, l1, l2))
                     break;
                  if (kind > 8)
                     kind = 8;
                  { jjAddStates(33, 35); }
                  break;
               case 45:
                  if (jjCanMove_0(hiByte, i1, i2, l1, l2))
                     { jjCheckNAddTwoStates(45, 46); }
                  break;
               case 47:
               case 48:
                  if (jjCanMove_0(hiByte, i1, i2, l1, l2))
                     { jjCheckNAddTwoStates(48, 46); }
                  break;
               case 52:
                  if (jjCanMove_0(hiByte, i1, i2, l1, l2))
                     { jjCheckNAddTwoStates(52, 53); }
                  break;
               case 54:
               case 55:
                  if (jjCanMove_0(hiByte, i1, i2, l1, l2))
                     { jjCheckNAddTwoStates(55, 53); }
                  break;
               default : if (i1 == 0 || l1 == 0 || i2 == 0 ||  l2 == 0) break; else break;
            }
         } while(i != startsAt);
      }
      if (kind != 0x7fffffff)
      {
         jjmatchedKind = kind;
         jjmatchedPos = curPos;
         kind = 0x7fffffff;
      }
      ++curPos;
      if ((i = jjnewStateCnt), (jjnewStateCnt = startsAt), (i == (startsAt = 83 - startsAt)))
         break;
      if (input_stream->endOfInput()) { break; }
      curChar = input_stream->readChar();
   }
   if (jjmatchedPos > strPos)
      return curPos;

   int toRet = MAX(curPos, seenUpto);

   if (curPos < toRet)
      for (i = toRet - MIN(curPos, seenUpto); i-- > 0; )
        {  assert(!input_stream->endOfInput());
           curChar = input_stream->readChar(); }

   if (jjmatchedPos < strPos)
   {
      jjmatchedKind = strKind;
      jjmatchedPos = strPos;
   }
   else if (jjmatchedPos == strPos && jjmatchedKind > strKind)
      jjmatchedKind = strKind;

   return toRet;
}

bool VhdlParserTokenManager::jjCanMove_0(int hiByte, int i1, int i2, unsigned long long l1, unsigned long long l2){
   switch(hiByte)
   {
      case 0:
         return ((jjbitVec2[i2] & l2) != 0L);
      default :
         if ((jjbitVec0[i1] & l1) != 0L)
            return true;
         return false;
   }
}

bool VhdlParserTokenManager::jjCanMove_1(int hiByte, int i1, int i2, unsigned long long l1, unsigned long long l2){
   switch(hiByte)
   {
      case 0:
         return ((jjbitVec3[i2] & l2) != 0L);
      default :
         return false;
   }
}

/** Token literal values. */

Token * VhdlParserTokenManager::jjFillToken(){
   Token *t;
   JJString curTokenImage;
   int beginLine   = -1;
   int endLine     = -1;
   int beginColumn = -1;
   int endColumn   = -1;
   JJString im = jjstrLiteralImages[jjmatchedKind];
   curTokenImage = (im.length() == 0) ? input_stream->GetImage() : im;
   if (input_stream->getTrackLineColumn()) {
     beginLine = input_stream->getBeginLine();
     beginColumn = input_stream->getBeginColumn();
     endLine = input_stream->getEndLine();
     endColumn = input_stream->getEndColumn();
   }
   t = Token::newToken(jjmatchedKind, curTokenImage);
   t->specialToken = nullptr;
   t->next = nullptr;

   if (input_stream->getTrackLineColumn()) {
   t->beginLine = beginLine;
   t->endLine = endLine;
   t->beginColumn = beginColumn;
   t->endColumn = endColumn;
   }

   return t;
}
const int defaultLexState = 0;
/** Get the next Token. */

Token * VhdlParserTokenManager::getNextToken(){
  Token *matchedToken = nullptr;
  int curPos = 0;

  for (;;)
  {
   EOFLoop: 
   if (input_stream->endOfInput())
   {
      jjmatchedKind = 0;
      jjmatchedPos = -1;
      matchedToken = jjFillToken();
      return matchedToken;
   }
   curChar = input_stream->BeginToken();
   image = jjimage;
   image.clear();
   jjimageLen = 0;

   jjmatchedKind = 0x7fffffff;
   jjmatchedPos = 0;
   curPos = jjMoveStringLiteralDfa0_0();
   if (jjmatchedKind != 0x7fffffff)
   {
      if (jjmatchedPos + 1 < curPos)
         input_stream->backup(curPos - jjmatchedPos - 1);
      if ((jjtoToken[jjmatchedKind >> 6] & (1ULL << (jjmatchedKind & 077))) != 0L)
      {
         matchedToken = jjFillToken();
         TokenLexicalActions(matchedToken);
         return matchedToken;
      }
      else
      {
         SkipLexicalActions(nullptr);
         goto EOFLoop;
      }
   }
   int error_line = input_stream->getEndLine();
   int error_column = input_stream->getEndColumn();
   JJString error_after;
   bool EOFSeen = false;
   if (input_stream->endOfInput()) {
      EOFSeen = true;
      error_after = curPos <= 1 ? EMPTY : input_stream->GetImage();
      if (curChar == '\n' || curChar == '\r') {
         error_line++;
         error_column = 0;
      }
      else
         error_column++;
   }
   if (!EOFSeen) {
      error_after = curPos <= 1 ? EMPTY : input_stream->GetImage();
   }
   errorHandler->lexicalError(EOFSeen, curLexState, error_line, error_column, error_after, curChar, this);
  }
}


void  VhdlParserTokenManager::SkipLexicalActions(Token *matchedToken){
   switch(jjmatchedKind)
   {
      case 3 : {
         image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
        parser->outlineParser()->lineCount();
         break;
       }
      case 6 : {
         image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
    {
    QCString doc = image;
     int count=doc.contains("--!");
     parser->outlineParser()->setMultCommentLine();
    parser->outlineParser()->lineCount(image);
    if (count == 1)
      parser->outlineParser()->oneLineComment(doc);
    else
      parser->outlineParser()->handleCommentBlock(QCString(image),FALSE); ;
    }
         break;
       }
      case 7 : {
         image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                                                                               parser->outlineParser()->handleFlowComment(image);
         break;
       }
      case 8 : {
         image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
    parser->outlineParser()->lineCount(image);
         break;
       }
      case 9 : {
         image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
   {
     QCString q = filter2008VhdlComment(image);
     parser->outlineParser()->lineCount(image);
     parser->outlineParser()->handleCommentBlock(QCString(q),TRUE);image.clear();
   }
         break;
       }
      case 10 : {
         image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
      parser->outlineParser()->lineCount(image);image.clear();
         break;
       }
      default :
         break;
   }
}

void  VhdlParserTokenManager::TokenLexicalActions(Token *matchedToken){
   switch(jjmatchedKind)
   {
      case 14 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                      parser->outlineParser()->setLineParsed(ALIAS_T);
         break;
       }
      case 17 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                                    parser->outlineParser()->setLineParsed(ARCHITECTURE_T);
         break;
       }
      case 18 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                      parser->outlineParser()->setLineParsed(ARRAY_T);
         break;
       }
      case 22 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                              parser->outlineParser()->setLineParsed(ATTRIBUTE_T);
         break;
       }
      case 25 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                    parser->outlineParser()->setLineParsed(BODY_T);
         break;
       }
      case 28 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                              parser->outlineParser()->setLineParsed(COMPONENT_T);
         break;
       }
      case 30 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                                      parser->outlineParser()->setLineParsed(CONFIGURATION_T);
         break;
       }
      case 31 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                             parser->outlineParser()->setLineParsed(CONSTANT_T);
         break;
       }
      case 32 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                          parser->outlineParser()->setLineParsed(CONTEXT_T);
         break;
       }
      case 39 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                  parser->outlineParser()->setLineParsed(END_T);
         break;
       }
      case 40 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                        parser->outlineParser()->setLineParsed(ENTITY_T);
         break;
       }
      case 43 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                    parser->outlineParser()->setLineParsed(FILE_T);
         break;
       }
      case 46 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                             parser->outlineParser()->setLineParsed(FUNCTION_T);
         break;
       }
      case 49 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                      parser->outlineParser()->setLineParsed(GROUP_T);
         break;
       }
      case 58 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                          parser->outlineParser()->setLineParsed(LIBRARY_T);
         break;
       }
      case 76 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                          parser->outlineParser()->setLineParsed(PACKAGE_T);
         break;
       }
      case 78 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                    parser->outlineParser()->setLineParsed(PORT_T);
         break;
       }
      case 81 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                              parser->outlineParser()->setLineParsed(PROCEDURE_T);
         break;
       }
      case 82 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                          parser->outlineParser()->setLineParsed(PROCESS_T);
         break;
       }
      case 87 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                        parser->outlineParser()->setLineParsed(RECORD_T);
         break;
       }
      case 101 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                        parser->outlineParser()->setLineParsed(SIGNAL_T);
         break;
       }
      case 108 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                          parser->outlineParser()->setLineParsed(SUBTYPE_T);
         break;
       }
      case 112 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                    parser->outlineParser()->setLineParsed(TYPE_T);
         break;
       }
      case 114 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                      parser->outlineParser()->setLineParsed(UNITS_T);
         break;
       }
      case 139 : {
        image.append(jjstrLiteralImages[139]);
        lengthOfMatch = jjstrLiteralImages[139].length();
                  parser->outlineParser()->setLineParsed(SEMI_T);
         break;
       }
      default :
         break;
   }
}
  /** Reinitialise parser. */
  void VhdlParserTokenManager::ReInit(JAVACC_CHARSTREAM *stream, int lexState) {
    clear();
    jjmatchedPos = jjnewStateCnt = 0;
    curLexState = lexState;
    input_stream = stream;
    ReInitRounds();
    debugStream = stdout; // init
    SwitchTo(lexState);
    errorHandler = new TokenManagerErrorHandler();
  }

  void VhdlParserTokenManager::ReInitRounds() {
    int i;
    jjround = 0x80000001;
    for (i = 83; i-- > 0;)
      jjrounds[i] = 0x80000000;
  }

  /** Switch to specified lex state. */
  void VhdlParserTokenManager::SwitchTo(int lexState) {
    if (lexState >= 1 || lexState < 0) {
      JJString message;
#ifdef WIDE_CHAR
      message += L"Error: Ignoring invalid lexical state : ";
      message += lexState; message += L". State unchanged.";
#else
      message += "Error: Ignoring invalid lexical state : ";
      message += lexState; message += ". State unchanged.";
#endif
      throw new TokenMgrError(message, INVALID_LEXICAL_STATE);
    } else
      curLexState = lexState;
  }

  /** Constructor. */
  VhdlParserTokenManager::VhdlParserTokenManager (JAVACC_CHARSTREAM *stream, int lexState)
  : TokenParser()
  {
    input_stream = nullptr;
    ReInit(stream, lexState);
  }

  // Destructor
  VhdlParserTokenManager::~VhdlParserTokenManager () {
    clear();
  }

  // clear
  void VhdlParserTokenManager::clear() {
    //Since input_stream was generated outside of TokenManager
    //TokenManager should not take care of deleting it
    //if (input_stream) delete input_stream;
    if (errorHandler) delete errorHandler, errorHandler = nullptr;    
  }


}
}
