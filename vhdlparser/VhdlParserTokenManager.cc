/* VhdlParserTokenManager.cc */
#include "./VhdlParserTokenManager.h"
namespace vhdl {
namespace parser {
static const unsigned long long jjbitVec0[] = {
   0x0ULL, 0x0ULL, 0xffffffff00000000ULL, 0xffffffffffffffffULL
};
static const unsigned long long jjbitVec1[] = {
   0xfffffffffffffffeULL, 0xffffffffffffffffULL, 0xffffffffffffffffULL, 0xffffffffffffffffULL
};
static const unsigned long long jjbitVec3[] = {
   0x0ULL, 0x0ULL, 0xffffffffffffffffULL, 0xffffffffffffffffULL
};
static const int jjnextStates[] = {
   43, 44, 45, 46, 47, 50, 54, 55, 56, 37, 38, 4, 5, 7, 8, 22, 
   23, 24, 26, 28, 29, 31, 45, 46, 47, 50, 49, 48, 50, 54, 55, 56, 
   57, 58, 60, 1, 2, 15, 16, 34, 36, 39, 41, 
};
static JAVACC_CHAR_TYPE jjstrLiteralChars_0[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_1[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_2[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_3[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_4[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_5[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_6[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_7[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_8[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_9[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_10[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_11[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_12[] = {0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_13[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_14[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_15[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_16[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_17[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_18[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_19[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_20[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_21[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_22[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_23[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_24[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_25[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_26[] = {0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_27[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_28[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_29[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_30[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_31[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_32[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_33[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_34[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_35[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_36[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_37[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_38[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_39[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_40[] = {0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_41[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_42[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_43[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_44[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_45[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_46[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_47[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_48[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_49[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_50[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_51[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_52[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_53[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_54[] = {0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_55[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_56[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_57[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_58[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_59[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_60[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_61[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_62[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_63[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_64[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_65[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_66[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_67[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_68[] = {0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_69[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_70[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_71[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_72[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_73[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_74[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_75[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_76[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_77[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_78[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_79[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_80[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_81[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_82[] = {0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_83[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_84[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_85[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_86[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_87[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_88[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_89[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_90[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_91[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_92[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_93[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_94[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_95[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_96[] = {0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_97[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_98[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_99[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_100[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_101[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_102[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_103[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_104[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_105[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_106[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_107[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_108[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_109[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_110[] = {0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_111[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_112[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_113[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_114[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_115[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_116[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_117[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_118[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_119[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_120[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_121[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_122[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_123[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_124[] = {0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_125[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_126[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_127[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_128[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_129[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_130[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_131[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_132[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_133[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_134[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_135[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_136[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_137[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_138[] = {0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_139[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_140[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_141[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_142[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_143[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_144[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_145[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_146[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_147[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_148[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_149[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_150[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_151[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_152[] = {0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_153[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_154[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_155[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_156[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_157[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_158[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_159[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_160[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_161[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_162[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_163[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_164[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_165[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_166[] = {0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_167[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_168[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_169[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_170[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_171[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_172[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_173[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_174[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_175[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_176[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_177[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_178[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_179[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_180[] = {0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_181[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_182[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_183[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_184[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_185[] = {0};
static const JAVACC_STRING_TYPE jjstrLiteralImages[] = {
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
};

/** Lexer state names. */
static const JAVACC_CHAR_TYPE lexStateNames_arr_0[] = 
{0x44, 0x45, 0x46, 0x41, 0x55, 0x4c, 0x54, 0};
static const JAVACC_STRING_TYPE lexStateNames[] = {
lexStateNames_arr_0, 
};
static const unsigned long long jjtoToken[] = {
   0xfffffffffffffc01ULL, 0xffffffffffffffffULL, 0x2040fffffffffffULL, 
};
static const unsigned long long jjtoSkip[] = {
   0x1eULL, 0x0ULL, 0x0ULL, 
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
         return jjMoveNfa_0(3, 0);
      case 10:
         jjmatchedKind = 3;
         return jjMoveNfa_0(3, 0);
      case 13:
         jjmatchedKind = 4;
         return jjMoveNfa_0(3, 0);
      case 32:
         jjmatchedKind = 1;
         return jjMoveNfa_0(3, 0);
      case 38:
         jjmatchedKind = 125;
         return jjMoveNfa_0(3, 0);
      case 39:
         jjmatchedKind = 126;
         return jjMoveNfa_0(3, 0);
      case 40:
         jjmatchedKind = 127;
         return jjMoveNfa_0(3, 0);
      case 41:
         jjmatchedKind = 128;
         return jjMoveNfa_0(3, 0);
      case 42:
         jjmatchedKind = 130;
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x0ULL, 0x2ULL);
      case 43:
         jjmatchedKind = 131;
         return jjMoveNfa_0(3, 0);
      case 44:
         jjmatchedKind = 133;
         return jjMoveNfa_0(3, 0);
      case 45:
         jjmatchedKind = 132;
         return jjMoveNfa_0(3, 0);
      case 46:
         jjmatchedKind = 156;
         return jjMoveNfa_0(3, 0);
      case 47:
         jjmatchedKind = 157;
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x0ULL, 0x4000ULL);
      case 58:
         jjmatchedKind = 135;
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x0ULL, 0x40ULL);
      case 59:
         jjmatchedKind = 136;
         return jjMoveNfa_0(3, 0);
      case 60:
         jjmatchedKind = 139;
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x0ULL, 0x30200ULL);
      case 61:
         jjmatchedKind = 141;
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x0ULL, 0x8000ULL);
      case 62:
         jjmatchedKind = 140;
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x0ULL, 0x40400ULL);
      case 63:
         jjmatchedKind = 154;
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x0ULL, 0x3f80000ULL);
      case 64:
         jjmatchedKind = 158;
         return jjMoveNfa_0(3, 0);
      case 65:
         return jjMoveStringLiteralDfa1_0(0x3ffc00ULL, 0x0ULL, 0x0ULL);
      case 66:
         return jjMoveStringLiteralDfa1_0(0x7c00000ULL, 0x0ULL, 0x0ULL);
      case 67:
         return jjMoveStringLiteralDfa1_0(0x1f8000000ULL, 0x0ULL, 0x0ULL);
      case 68:
         return jjMoveStringLiteralDfa1_0(0xe00000000ULL, 0x0ULL, 0x0ULL);
      case 69:
         return jjMoveStringLiteralDfa1_0(0x1f000000000ULL, 0x0ULL, 0x0ULL);
      case 70:
         return jjMoveStringLiteralDfa1_0(0x3e0000000000ULL, 0x0ULL, 0x0ULL);
      case 71:
         return jjMoveStringLiteralDfa1_0(0x3c00000000000ULL, 0x0ULL, 0x0ULL);
      case 73:
         return jjMoveStringLiteralDfa1_0(0xfc000000000000ULL, 0x0ULL, 0x0ULL);
      case 76:
         return jjMoveStringLiteralDfa1_0(0x1f00000000000000ULL, 0x0ULL, 0x0ULL);
      case 77:
         return jjMoveStringLiteralDfa1_0(0x6000000000000000ULL, 0x0ULL, 0x0ULL);
      case 78:
         return jjMoveStringLiteralDfa1_0(0x8000000000000000ULL, 0x1fULL, 0x0ULL);
      case 79:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x7e0ULL, 0x0ULL);
      case 80:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0xff800ULL, 0x0ULL);
      case 82:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0xfff00000ULL, 0x0ULL);
      case 83:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x7ff00000000ULL, 0x0ULL);
      case 84:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x780000000000ULL, 0x0ULL);
      case 85:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x7800000000000ULL, 0x0ULL);
      case 86:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x78000000000000ULL, 0x0ULL);
      case 87:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x780000000000000ULL, 0x0ULL);
      case 88:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x1800000000000000ULL, 0x0ULL);
      case 91:
         jjmatchedKind = 160;
         return jjMoveNfa_0(3, 0);
      case 93:
         jjmatchedKind = 161;
         return jjMoveNfa_0(3, 0);
      case 94:
         jjmatchedKind = 159;
         return jjMoveNfa_0(3, 0);
      case 97:
         return jjMoveStringLiteralDfa1_0(0x3ffc00ULL, 0x0ULL, 0x0ULL);
      case 98:
         return jjMoveStringLiteralDfa1_0(0x7c00000ULL, 0x0ULL, 0x0ULL);
      case 99:
         return jjMoveStringLiteralDfa1_0(0x1f8000000ULL, 0x0ULL, 0x0ULL);
      case 100:
         return jjMoveStringLiteralDfa1_0(0xe00000000ULL, 0x0ULL, 0x0ULL);
      case 101:
         return jjMoveStringLiteralDfa1_0(0x1f000000000ULL, 0x0ULL, 0x0ULL);
      case 102:
         return jjMoveStringLiteralDfa1_0(0x3e0000000000ULL, 0x0ULL, 0x0ULL);
      case 103:
         return jjMoveStringLiteralDfa1_0(0x3c00000000000ULL, 0x0ULL, 0x0ULL);
      case 105:
         return jjMoveStringLiteralDfa1_0(0xfc000000000000ULL, 0x0ULL, 0x0ULL);
      case 108:
         return jjMoveStringLiteralDfa1_0(0x1f00000000000000ULL, 0x0ULL, 0x0ULL);
      case 109:
         return jjMoveStringLiteralDfa1_0(0x6000000000000000ULL, 0x0ULL, 0x0ULL);
      case 110:
         return jjMoveStringLiteralDfa1_0(0x8000000000000000ULL, 0x1fULL, 0x0ULL);
      case 111:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x7e0ULL, 0x0ULL);
      case 112:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0xff800ULL, 0x0ULL);
      case 114:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0xfff00000ULL, 0x0ULL);
      case 115:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x7ff00000000ULL, 0x0ULL);
      case 116:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x780000000000ULL, 0x0ULL);
      case 117:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x7800000000000ULL, 0x0ULL);
      case 118:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x78000000000000ULL, 0x0ULL);
      case 119:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x780000000000000ULL, 0x0ULL);
      case 120:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x1800000000000000ULL, 0x0ULL);
      case 123:
         jjmatchedKind = 162;
         return jjMoveNfa_0(3, 0);
      case 124:
         jjmatchedKind = 155;
         return jjMoveNfa_0(3, 0);
      case 125:
         jjmatchedKind = 163;
         return jjMoveNfa_0(3, 0);
      default :
         return jjMoveNfa_0(3, 0);
   }
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa1_0(unsigned long long active0, unsigned long long active1, unsigned long long active2){
   if (input_stream->endOfInput()) {
   return jjMoveNfa_0(3, 0);
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 42:
         if ((active2 & 0x2ULL) != 0L)
         {
            jjmatchedKind = 129;
            jjmatchedPos = 1;
         }
         break;
      case 47:
         return jjMoveStringLiteralDfa2_0(active0, 0L, active1, 0L, active2, 0x2000000ULL);
      case 60:
         if ((active2 & 0x20000ULL) != 0L)
         {
            jjmatchedKind = 145;
            jjmatchedPos = 1;
         }
         else if ((active2 & 0x800000ULL) != 0L)
         {
            jjmatchedKind = 151;
            jjmatchedPos = 1;
         }
         return jjMoveStringLiteralDfa2_0(active0, 0L, active1, 0L, active2, 0x200000ULL);
      case 61:
         if ((active2 & 0x40ULL) != 0L)
         {
            jjmatchedKind = 134;
            jjmatchedPos = 1;
         }
         else if ((active2 & 0x200ULL) != 0L)
         {
            jjmatchedKind = 137;
            jjmatchedPos = 1;
         }
         else if ((active2 & 0x400ULL) != 0L)
         {
            jjmatchedKind = 138;
            jjmatchedPos = 1;
         }
         else if ((active2 & 0x4000ULL) != 0L)
         {
            jjmatchedKind = 142;
            jjmatchedPos = 1;
         }
         else if ((active2 & 0x1000000ULL) != 0L)
         {
            jjmatchedKind = 152;
            jjmatchedPos = 1;
         }
         break;
      case 62:
         if ((active2 & 0x8000ULL) != 0L)
         {
            jjmatchedKind = 143;
            jjmatchedPos = 1;
         }
         else if ((active2 & 0x10000ULL) != 0L)
         {
            jjmatchedKind = 144;
            jjmatchedPos = 1;
         }
         else if ((active2 & 0x40000ULL) != 0L)
         {
            jjmatchedKind = 146;
            jjmatchedPos = 1;
         }
         else if ((active2 & 0x400000ULL) != 0L)
         {
            jjmatchedKind = 150;
            jjmatchedPos = 1;
         }
         return jjMoveStringLiteralDfa2_0(active0, 0L, active1, 0L, active2, 0x100000ULL);
      case 63:
         if ((active2 & 0x80000ULL) != 0L)
         {
            jjmatchedKind = 147;
            jjmatchedPos = 1;
         }
         break;
      case 65:
         return jjMoveStringLiteralDfa2_0(active0, 0xa100020010000000ULL, active1, 0x88000000101800ULL, active2, 0L);
      case 66:
         return jjMoveStringLiteralDfa2_0(active0, 0x400ULL, active1, 0L, active2, 0L);
      case 67:
         return jjMoveStringLiteralDfa2_0(active0, 0x800ULL, active1, 0L, active2, 0L);
      case 69:
         return jjMoveStringLiteralDfa2_0(active0, 0xc00200400000ULL, active1, 0x79fe00003ULL, active2, 0L);
      case 70:
         if ((active0 & 0x4000000000000ULL) != 0L)
         {
            jjmatchedKind = 50;
            jjmatchedPos = 1;
         }
         else if ((active1 & 0x20ULL) != 0L)
         {
            jjmatchedKind = 69;
            jjmatchedPos = 1;
         }
         return jjMoveStringLiteralDfa2_0(active0, 0x1000ULL, active1, 0L, active2, 0L);
      case 72:
         return jjMoveStringLiteralDfa2_0(active0, 0L, active1, 0x300081000000000ULL, active2, 0L);
      case 73:
         return jjMoveStringLiteralDfa2_0(active0, 0xe00040400000000ULL, active1, 0x400000800000000ULL, active2, 0L);
      case 76:
         return jjMoveStringLiteralDfa2_0(active0, 0x3000806000ULL, active1, 0x6000000000ULL, active2, 0L);
      case 77:
         return jjMoveStringLiteralDfa2_0(active0, 0x8000000000000ULL, active1, 0x10000000000000ULL, active2, 0L);
      case 78:
         if ((active0 & 0x10000000000000ULL) != 0L)
         {
            jjmatchedKind = 52;
            jjmatchedPos = 1;
         }
         else if ((active1 & 0x40ULL) != 0L)
         {
            jjmatchedKind = 70;
            jjmatchedPos = 1;
         }
         return jjMoveStringLiteralDfa2_0(active0, 0x6000c000008000ULL, active1, 0x1003800000000000ULL, active2, 0L);
      case 79:
         if ((active1 & 0x100000000000ULL) != 0L)
         {
            jjmatchedKind = 108;
            jjmatchedPos = 1;
         }
         return jjMoveStringLiteralDfa2_0(active0, 0x50001809e9000000ULL, active1, 0x80000006000600cULL, active2, 0L);
      case 80:
         return jjMoveStringLiteralDfa2_0(active0, 0L, active1, 0x20000000000080ULL, active2, 0L);
      case 82:
         if ((active1 & 0x100ULL) != 0L)
         {
            jjmatchedKind = 72;
            jjmatchedPos = 1;
         }
         return jjMoveStringLiteralDfa2_0(active0, 0x1000000030000ULL, active1, 0x218000078000ULL, active2, 0L);
      case 83:
         if ((active0 & 0x80000000000000ULL) != 0L)
         {
            jjmatchedKind = 55;
            jjmatchedPos = 1;
         }
         return jjMoveStringLiteralDfa2_0(active0, 0x1c0000ULL, active1, 0x4000000000000ULL, active2, 0L);
      case 84:
         return jjMoveStringLiteralDfa2_0(active0, 0x200000ULL, active1, 0x20000000200ULL, active2, 0L);
      case 85:
         return jjMoveStringLiteralDfa2_0(active0, 0x2200006000000ULL, active1, 0x40040000080410ULL, active2, 0L);
      case 88:
         return jjMoveStringLiteralDfa2_0(active0, 0x10000000000ULL, active1, 0L, active2, 0L);
      case 89:
         return jjMoveStringLiteralDfa2_0(active0, 0L, active1, 0x400000000000ULL, active2, 0L);
      case 97:
         return jjMoveStringLiteralDfa2_0(active0, 0xa100020010000000ULL, active1, 0x88000000101800ULL, active2, 0L);
      case 98:
         return jjMoveStringLiteralDfa2_0(active0, 0x400ULL, active1, 0L, active2, 0L);
      case 99:
         return jjMoveStringLiteralDfa2_0(active0, 0x800ULL, active1, 0L, active2, 0L);
      case 101:
         return jjMoveStringLiteralDfa2_0(active0, 0xc00200400000ULL, active1, 0x79fe00003ULL, active2, 0L);
      case 102:
         if ((active0 & 0x4000000000000ULL) != 0L)
         {
            jjmatchedKind = 50;
            jjmatchedPos = 1;
         }
         else if ((active1 & 0x20ULL) != 0L)
         {
            jjmatchedKind = 69;
            jjmatchedPos = 1;
         }
         return jjMoveStringLiteralDfa2_0(active0, 0x1000ULL, active1, 0L, active2, 0L);
      case 104:
         return jjMoveStringLiteralDfa2_0(active0, 0L, active1, 0x300081000000000ULL, active2, 0L);
      case 105:
         return jjMoveStringLiteralDfa2_0(active0, 0xe00040400000000ULL, active1, 0x400000800000000ULL, active2, 0L);
      case 108:
         return jjMoveStringLiteralDfa2_0(active0, 0x3000806000ULL, active1, 0x6000000000ULL, active2, 0L);
      case 109:
         return jjMoveStringLiteralDfa2_0(active0, 0x8000000000000ULL, active1, 0x10000000000000ULL, active2, 0L);
      case 110:
         if ((active0 & 0x10000000000000ULL) != 0L)
         {
            jjmatchedKind = 52;
            jjmatchedPos = 1;
         }
         else if ((active1 & 0x40ULL) != 0L)
         {
            jjmatchedKind = 70;
            jjmatchedPos = 1;
         }
         return jjMoveStringLiteralDfa2_0(active0, 0x6000c000008000ULL, active1, 0x1003800000000000ULL, active2, 0L);
      case 111:
         if ((active1 & 0x100000000000ULL) != 0L)
         {
            jjmatchedKind = 108;
            jjmatchedPos = 1;
         }
         return jjMoveStringLiteralDfa2_0(active0, 0x50001809e9000000ULL, active1, 0x80000006000600cULL, active2, 0L);
      case 112:
         return jjMoveStringLiteralDfa2_0(active0, 0L, active1, 0x20000000000080ULL, active2, 0L);
      case 114:
         if ((active1 & 0x100ULL) != 0L)
         {
            jjmatchedKind = 72;
            jjmatchedPos = 1;
         }
         return jjMoveStringLiteralDfa2_0(active0, 0x1000000030000ULL, active1, 0x218000078000ULL, active2, 0L);
      case 115:
         if ((active0 & 0x80000000000000ULL) != 0L)
         {
            jjmatchedKind = 55;
            jjmatchedPos = 1;
         }
         return jjMoveStringLiteralDfa2_0(active0, 0x1c0000ULL, active1, 0x4000000000000ULL, active2, 0L);
      case 116:
         return jjMoveStringLiteralDfa2_0(active0, 0x200000ULL, active1, 0x20000000200ULL, active2, 0L);
      case 117:
         return jjMoveStringLiteralDfa2_0(active0, 0x2200006000000ULL, active1, 0x40040000080410ULL, active2, 0L);
      case 120:
         return jjMoveStringLiteralDfa2_0(active0, 0x10000000000ULL, active1, 0L, active2, 0L);
      case 121:
         return jjMoveStringLiteralDfa2_0(active0, 0L, active1, 0x400000000000ULL, active2, 0L);
      default :
         break;
   }
   return jjMoveNfa_0(3, 1);
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa2_0(unsigned long long old0, unsigned long long active0, unsigned long long old1, unsigned long long active1, unsigned long long old2, unsigned long long active2){
   if (((active0 &= old0) | (active1 &= old1) | (active2 &= old2)) == 0L)
      return jjMoveNfa_0(3, 1);
   if (input_stream->endOfInput()) {
   return jjMoveNfa_0(3, 1);
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 61:
         if ((active2 & 0x100000ULL) != 0L)
         {
            jjmatchedKind = 148;
            jjmatchedPos = 2;
         }
         else if ((active2 & 0x200000ULL) != 0L)
         {
            jjmatchedKind = 149;
            jjmatchedPos = 2;
         }
         else if ((active2 & 0x2000000ULL) != 0L)
         {
            jjmatchedKind = 153;
            jjmatchedPos = 2;
         }
         break;
      case 65:
         if ((active1 & 0x2000000000ULL) != 0L)
         {
            jjmatchedKind = 101;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x8000000000ULL) != 0L)
         {
            jjmatchedKind = 103;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x2000000000000ULL, active1, 0xa01000000000ULL, active2, 0L);
      case 66:
         return jjMoveStringLiteralDfa3_0(active0, 0x300000000000000ULL, active1, 0x40000000000ULL, active2, 0L);
      case 67:
         return jjMoveStringLiteralDfa3_0(active0, 0x10800ULL, active1, 0x200800ULL, active2, 0L);
      case 68:
         if ((active0 & 0x8000ULL) != 0L)
         {
            jjmatchedKind = 15;
            jjmatchedPos = 2;
         }
         else if ((active0 & 0x4000000000ULL) != 0L)
         {
            jjmatchedKind = 38;
            jjmatchedPos = 2;
         }
         else if ((active0 & 0x4000000000000000ULL) != 0L)
         {
            jjmatchedKind = 62;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x1000000ULL, active1, 0L, active2, 0L);
      case 69:
         if ((active1 & 0x4000000000000ULL) != 0L)
         {
            jjmatchedKind = 114;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x20000000000000ULL, active1, 0x100080000000080ULL, active2, 0L);
      case 70:
         return jjMoveStringLiteralDfa3_0(active0, 0x202000000ULL, active1, 0L, active2, 0L);
      case 71:
         return jjMoveStringLiteralDfa3_0(active0, 0x400000ULL, active1, 0x800400000ULL, active2, 0L);
      case 72:
         return jjMoveStringLiteralDfa3_0(active0, 0L, active1, 0x200ULL, active2, 0L);
      case 73:
         return jjMoveStringLiteralDfa3_0(active0, 0x30000002000ULL, active1, 0x281000000000000ULL, active2, 0L);
      case 74:
         return jjMoveStringLiteralDfa3_0(active0, 0L, active1, 0x800000ULL, active2, 0L);
      case 76:
         if ((active0 & 0x4000ULL) != 0L)
         {
            jjmatchedKind = 14;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x20000000ULL) != 0L)
         {
            jjmatchedKind = 93;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x4000000000ULL) != 0L)
         {
            jjmatchedKind = 102;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x10000000000ULL) != 0L)
         {
            jjmatchedKind = 104;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x40000000000ULL, active1, 0x101000010ULL, active2, 0L);
      case 77:
         if ((active1 & 0x8000000ULL) != 0L)
         {
            jjmatchedKind = 91;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x8000000ULL, active1, 0L, active2, 0L);
      case 78:
         return jjMoveStringLiteralDfa3_0(active0, 0x8400e000e0000000ULL, active1, 0x40000000100000ULL, active2, 0L);
      case 79:
         return jjMoveStringLiteralDfa3_0(active0, 0x1041000000800000ULL, active1, 0x1010000000078000ULL, active2, 0L);
      case 80:
         if ((active0 & 0x2000000000000000ULL) != 0L)
         {
            jjmatchedKind = 61;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x8000000000000ULL, active1, 0x400010000000ULL, active2, 0L);
      case 81:
         return jjMoveStringLiteralDfa3_0(active0, 0L, active1, 0x200000000ULL, active2, 0L);
      case 82:
         if ((active0 & 0x80000000000ULL) != 0L)
         {
            jjmatchedKind = 43;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x4ULL) != 0L)
         {
            jjmatchedKind = 66;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x40000000ULL) != 0L)
         {
            jjmatchedKind = 94;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x800000000000000ULL) != 0L)
         {
            jjmatchedKind = 123;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x100000020000ULL, active1, 0x28020000083000ULL, active2, 0L);
      case 83:
         if ((active0 & 0x400ULL) != 0L)
         {
            jjmatchedKind = 10;
            jjmatchedPos = 2;
         }
         else if ((active0 & 0x4000000ULL) != 0L)
         {
            jjmatchedKind = 26;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x34101c0000ULL, active1, 0x6004000ULL, active2, 0L);
      case 84:
         if ((active1 & 0x8ULL) != 0L)
         {
            jjmatchedKind = 67;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x400ULL) != 0L)
         {
            jjmatchedKind = 74;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x800008000201000ULL, active1, 0x402000080000000ULL, active2, 0L);
      case 86:
         return jjMoveStringLiteralDfa3_0(active0, 0x100000000ULL, active1, 0x400000000ULL, active2, 0L);
      case 87:
         if ((active1 & 0x1ULL) != 0L)
         {
            jjmatchedKind = 64;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x800000000ULL, active1, 0L, active2, 0L);
      case 88:
         return jjMoveStringLiteralDfa3_0(active0, 0L, active1, 0x2ULL, active2, 0L);
      case 97:
         if ((active1 & 0x2000000000ULL) != 0L)
         {
            jjmatchedKind = 101;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x8000000000ULL) != 0L)
         {
            jjmatchedKind = 103;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x2000000000000ULL, active1, 0xa01000000000ULL, active2, 0L);
      case 98:
         return jjMoveStringLiteralDfa3_0(active0, 0x300000000000000ULL, active1, 0x40000000000ULL, active2, 0L);
      case 99:
         return jjMoveStringLiteralDfa3_0(active0, 0x10800ULL, active1, 0x200800ULL, active2, 0L);
      case 100:
         if ((active0 & 0x8000ULL) != 0L)
         {
            jjmatchedKind = 15;
            jjmatchedPos = 2;
         }
         else if ((active0 & 0x4000000000ULL) != 0L)
         {
            jjmatchedKind = 38;
            jjmatchedPos = 2;
         }
         else if ((active0 & 0x4000000000000000ULL) != 0L)
         {
            jjmatchedKind = 62;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x1000000ULL, active1, 0L, active2, 0L);
      case 101:
         if ((active1 & 0x4000000000000ULL) != 0L)
         {
            jjmatchedKind = 114;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x20000000000000ULL, active1, 0x100080000000080ULL, active2, 0L);
      case 102:
         return jjMoveStringLiteralDfa3_0(active0, 0x202000000ULL, active1, 0L, active2, 0L);
      case 103:
         return jjMoveStringLiteralDfa3_0(active0, 0x400000ULL, active1, 0x800400000ULL, active2, 0L);
      case 104:
         return jjMoveStringLiteralDfa3_0(active0, 0L, active1, 0x200ULL, active2, 0L);
      case 105:
         return jjMoveStringLiteralDfa3_0(active0, 0x30000002000ULL, active1, 0x281000000000000ULL, active2, 0L);
      case 106:
         return jjMoveStringLiteralDfa3_0(active0, 0L, active1, 0x800000ULL, active2, 0L);
      case 108:
         if ((active0 & 0x4000ULL) != 0L)
         {
            jjmatchedKind = 14;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x20000000ULL) != 0L)
         {
            jjmatchedKind = 93;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x4000000000ULL) != 0L)
         {
            jjmatchedKind = 102;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x10000000000ULL) != 0L)
         {
            jjmatchedKind = 104;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x40000000000ULL, active1, 0x101000010ULL, active2, 0L);
      case 109:
         if ((active1 & 0x8000000ULL) != 0L)
         {
            jjmatchedKind = 91;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x8000000ULL, active1, 0L, active2, 0L);
      case 110:
         return jjMoveStringLiteralDfa3_0(active0, 0x8400e000e0000000ULL, active1, 0x40000000100000ULL, active2, 0L);
      case 111:
         return jjMoveStringLiteralDfa3_0(active0, 0x1041000000800000ULL, active1, 0x1010000000078000ULL, active2, 0L);
      case 112:
         if ((active0 & 0x2000000000000000ULL) != 0L)
         {
            jjmatchedKind = 61;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x8000000000000ULL, active1, 0x400010000000ULL, active2, 0L);
      case 113:
         return jjMoveStringLiteralDfa3_0(active0, 0L, active1, 0x200000000ULL, active2, 0L);
      case 114:
         if ((active0 & 0x80000000000ULL) != 0L)
         {
            jjmatchedKind = 43;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x4ULL) != 0L)
         {
            jjmatchedKind = 66;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x40000000ULL) != 0L)
         {
            jjmatchedKind = 94;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x800000000000000ULL) != 0L)
         {
            jjmatchedKind = 123;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x100000020000ULL, active1, 0x28020000083000ULL, active2, 0L);
      case 115:
         if ((active0 & 0x400ULL) != 0L)
         {
            jjmatchedKind = 10;
            jjmatchedPos = 2;
         }
         else if ((active0 & 0x4000000ULL) != 0L)
         {
            jjmatchedKind = 26;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x34101c0000ULL, active1, 0x6004000ULL, active2, 0L);
      case 116:
         if ((active1 & 0x8ULL) != 0L)
         {
            jjmatchedKind = 67;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x400ULL) != 0L)
         {
            jjmatchedKind = 74;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x800008000201000ULL, active1, 0x402000080000000ULL, active2, 0L);
      case 118:
         return jjMoveStringLiteralDfa3_0(active0, 0x100000000ULL, active1, 0x400000000ULL, active2, 0L);
      case 119:
         if ((active1 & 0x1ULL) != 0L)
         {
            jjmatchedKind = 64;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x800000000ULL, active1, 0L, active2, 0L);
      case 120:
         return jjMoveStringLiteralDfa3_0(active0, 0L, active1, 0x2ULL, active2, 0L);
      default :
         break;
   }
   return jjMoveNfa_0(3, 2);
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa3_0(unsigned long long old0, unsigned long long active0, unsigned long long old1, unsigned long long active1, unsigned long long old2, unsigned long long active2){
   if (((active0 &= old0) | (active1 &= old1) | (active2 &= old2)) == 0L)
      return jjMoveNfa_0(3, 2);
   if (input_stream->endOfInput()) {
   return jjMoveNfa_0(3, 2);
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 65:
         return jjMoveStringLiteralDfa4_0(active0, 0x200022000ULL, active1, 0x1000ULL);
      case 67:
         return jjMoveStringLiteralDfa4_0(active0, 0x300400800000ULL, active1, 0x18000ULL);
      case 68:
         if ((active0 & 0x8000000000000000ULL) != 0L)
         {
            jjmatchedKind = 63;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0L, active1, 0x10000000000000ULL);
      case 69:
         if ((active0 & 0x10000000ULL) != 0L)
         {
            jjmatchedKind = 28;
            jjmatchedPos = 3;
         }
         else if ((active0 & 0x1000000000ULL) != 0L)
         {
            jjmatchedKind = 36;
            jjmatchedPos = 3;
         }
         else if ((active0 & 0x40000000000ULL) != 0L)
         {
            jjmatchedKind = 42;
            jjmatchedPos = 3;
         }
         else if ((active1 & 0x80000ULL) != 0L)
         {
            jjmatchedKind = 83;
            jjmatchedPos = 3;
         }
         else if ((active1 & 0x400000000000ULL) != 0L)
         {
            jjmatchedKind = 110;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x900c00100041800ULL, active1, 0x501800200ULL);
      case 70:
         return jjMoveStringLiteralDfa4_0(active0, 0x22000000ULL, active1, 0x800000000000ULL);
      case 71:
         return jjMoveStringLiteralDfa4_0(active0, 0L, active1, 0x100000ULL);
      case 72:
         if ((active1 & 0x400000000000000ULL) != 0L)
         {
            jjmatchedKind = 122;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x10000ULL, active1, 0L);
      case 73:
         return jjMoveStringLiteralDfa4_0(active0, 0xa000400000ULL, active1, 0x4a000000400000ULL);
      case 75:
         return jjMoveStringLiteralDfa4_0(active0, 0x400000000000000ULL, active1, 0x800ULL);
      case 76:
         if ((active1 & 0x10ULL) != 0L)
         {
            jjmatchedKind = 68;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0L, active1, 0x200000000000000ULL);
      case 78:
         if ((active1 & 0x80ULL) != 0L)
         {
            jjmatchedKind = 71;
            jjmatchedPos = 3;
         }
         else if ((active1 & 0x80000000000ULL) != 0L)
         {
            jjmatchedKind = 107;
            jjmatchedPos = 3;
         }
         else if ((active1 & 0x100000000000000ULL) != 0L)
         {
            jjmatchedKind = 120;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x800000000ULL, active1, 0x200800000000ULL);
      case 79:
         return jjMoveStringLiteralDfa4_0(active0, 0L, active1, 0x20020010200000ULL);
      case 80:
         if ((active0 & 0x1000000000000000ULL) != 0L)
         {
            jjmatchedKind = 60;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x8000000ULL, active1, 0x20000ULL);
      case 82:
         if ((active1 & 0x1000000000000000ULL) != 0L)
         {
            jjmatchedKind = 124;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x222020000200000ULL, active1, 0x1000000000ULL);
      case 83:
         return jjMoveStringLiteralDfa4_0(active0, 0x40000000ULL, active1, 0L);
      case 84:
         if ((active0 & 0x10000000000ULL) != 0L)
         {
            jjmatchedKind = 40;
            jjmatchedPos = 3;
         }
         else if ((active1 & 0x2ULL) != 0L)
         {
            jjmatchedKind = 65;
            jjmatchedPos = 3;
         }
         else if ((active1 & 0x2000ULL) != 0L)
         {
            jjmatchedKind = 77;
            jjmatchedPos = 3;
         }
         else if ((active1 & 0x80000000000000ULL) != 0L)
         {
            jjmatchedKind = 119;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x80000000ULL, active1, 0x1040006044000ULL);
      case 85:
         return jjMoveStringLiteralDfa4_0(active0, 0x49000000180000ULL, active1, 0x280000000ULL);
      case 89:
         if ((active0 & 0x1000000ULL) != 0L)
         {
            jjmatchedKind = 24;
            jjmatchedPos = 3;
         }
         break;
      case 97:
         return jjMoveStringLiteralDfa4_0(active0, 0x200022000ULL, active1, 0x1000ULL);
      case 99:
         return jjMoveStringLiteralDfa4_0(active0, 0x300400800000ULL, active1, 0x18000ULL);
      case 100:
         if ((active0 & 0x8000000000000000ULL) != 0L)
         {
            jjmatchedKind = 63;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0L, active1, 0x10000000000000ULL);
      case 101:
         if ((active0 & 0x10000000ULL) != 0L)
         {
            jjmatchedKind = 28;
            jjmatchedPos = 3;
         }
         else if ((active0 & 0x1000000000ULL) != 0L)
         {
            jjmatchedKind = 36;
            jjmatchedPos = 3;
         }
         else if ((active0 & 0x40000000000ULL) != 0L)
         {
            jjmatchedKind = 42;
            jjmatchedPos = 3;
         }
         else if ((active1 & 0x80000ULL) != 0L)
         {
            jjmatchedKind = 83;
            jjmatchedPos = 3;
         }
         else if ((active1 & 0x400000000000ULL) != 0L)
         {
            jjmatchedKind = 110;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x900c00100041800ULL, active1, 0x501800200ULL);
      case 102:
         return jjMoveStringLiteralDfa4_0(active0, 0x22000000ULL, active1, 0x800000000000ULL);
      case 103:
         return jjMoveStringLiteralDfa4_0(active0, 0L, active1, 0x100000ULL);
      case 104:
         if ((active1 & 0x400000000000000ULL) != 0L)
         {
            jjmatchedKind = 122;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x10000ULL, active1, 0L);
      case 105:
         return jjMoveStringLiteralDfa4_0(active0, 0xa000400000ULL, active1, 0x4a000000400000ULL);
      case 107:
         return jjMoveStringLiteralDfa4_0(active0, 0x400000000000000ULL, active1, 0x800ULL);
      case 108:
         if ((active1 & 0x10ULL) != 0L)
         {
            jjmatchedKind = 68;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0L, active1, 0x200000000000000ULL);
      case 110:
         if ((active1 & 0x80ULL) != 0L)
         {
            jjmatchedKind = 71;
            jjmatchedPos = 3;
         }
         else if ((active1 & 0x80000000000ULL) != 0L)
         {
            jjmatchedKind = 107;
            jjmatchedPos = 3;
         }
         else if ((active1 & 0x100000000000000ULL) != 0L)
         {
            jjmatchedKind = 120;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x800000000ULL, active1, 0x200800000000ULL);
      case 111:
         return jjMoveStringLiteralDfa4_0(active0, 0L, active1, 0x20020010200000ULL);
      case 112:
         if ((active0 & 0x1000000000000000ULL) != 0L)
         {
            jjmatchedKind = 60;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x8000000ULL, active1, 0x20000ULL);
      case 114:
         if ((active1 & 0x1000000000000000ULL) != 0L)
         {
            jjmatchedKind = 124;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x222020000200000ULL, active1, 0x1000000000ULL);
      case 115:
         return jjMoveStringLiteralDfa4_0(active0, 0x40000000ULL, active1, 0L);
      case 116:
         if ((active0 & 0x10000000000ULL) != 0L)
         {
            jjmatchedKind = 40;
            jjmatchedPos = 3;
         }
         else if ((active1 & 0x2ULL) != 0L)
         {
            jjmatchedKind = 65;
            jjmatchedPos = 3;
         }
         else if ((active1 & 0x2000ULL) != 0L)
         {
            jjmatchedKind = 77;
            jjmatchedPos = 3;
         }
         else if ((active1 & 0x80000000000000ULL) != 0L)
         {
            jjmatchedKind = 119;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x80000000ULL, active1, 0x1040006044000ULL);
      case 117:
         return jjMoveStringLiteralDfa4_0(active0, 0x49000000180000ULL, active1, 0x280000000ULL);
      case 121:
         if ((active0 & 0x1000000ULL) != 0L)
         {
            jjmatchedKind = 24;
            jjmatchedPos = 3;
         }
         break;
      default :
         break;
   }
   return jjMoveNfa_0(3, 3);
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa4_0(unsigned long long old0, unsigned long long active0, unsigned long long old1, unsigned long long active1){
   if (((active0 &= old0) | (active1 &= old1)) == 0L)
      return jjMoveNfa_0(3, 3);
   if (input_stream->endOfInput()) {
   return jjMoveNfa_0(3, 3);
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 65:
         return jjMoveStringLiteralDfa5_0(active0, 0x600000000000000ULL, active1, 0x8000801000800ULL);
      case 67:
         return jjMoveStringLiteralDfa5_0(active0, 0L, active1, 0x100800000ULL);
      case 68:
         return jjMoveStringLiteralDfa5_0(active0, 0x2000000000000ULL, active1, 0L);
      case 69:
         if ((active0 & 0x100000000000ULL) != 0L)
         {
            jjmatchedKind = 44;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x100000ULL) != 0L)
         {
            jjmatchedKind = 84;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x10000000000000ULL) != 0L)
         {
            jjmatchedKind = 116;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x200000000000000ULL) != 0L)
         {
            jjmatchedKind = 121;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0x82000000ULL, active1, 0x1200078000ULL);
      case 70:
         if ((active0 & 0x2000000000ULL) != 0L)
         {
            jjmatchedKind = 37;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0L, active1, 0x800000000000ULL);
      case 73:
         return jjMoveStringLiteralDfa5_0(active0, 0x20210000ULL, active1, 0L);
      case 75:
         if ((active0 & 0x800000ULL) != 0L)
         {
            jjmatchedKind = 23;
            jjmatchedPos = 4;
         }
         break;
      case 76:
         if ((active0 & 0x100000000000000ULL) != 0L)
         {
            jjmatchedKind = 56;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x2000000000000ULL) != 0L)
         {
            jjmatchedKind = 113;
            jjmatchedPos = 4;
         }
         break;
      case 77:
         return jjMoveStringLiteralDfa5_0(active0, 0x180000ULL, active1, 0x1000ULL);
      case 78:
         if ((active0 & 0x400000ULL) != 0L)
         {
            jjmatchedKind = 22;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0x20000000000ULL, active1, 0x20000000000ULL);
      case 79:
         return jjMoveStringLiteralDfa5_0(active0, 0x408000000ULL, active1, 0L);
      case 80:
         if ((active0 & 0x1000000000000ULL) != 0L)
         {
            jjmatchedKind = 48;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x20000000000000ULL) != 0L)
         {
            jjmatchedKind = 117;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0L, active1, 0x4000ULL);
      case 82:
         if ((active0 & 0x1000ULL) != 0L)
         {
            jjmatchedKind = 12;
            jjmatchedPos = 4;
         }
         else if ((active0 & 0x100000000ULL) != 0L)
         {
            jjmatchedKind = 32;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0x808c00000040000ULL, active1, 0x496200200ULL);
      case 83:
         if ((active0 & 0x2000ULL) != 0L)
         {
            jjmatchedKind = 13;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x1000000000000ULL) != 0L)
         {
            jjmatchedKind = 112;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0x800ULL, active1, 0x200000400000ULL);
      case 84:
         if ((active0 & 0x40000000000000ULL) != 0L)
         {
            jjmatchedKind = 54;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x40000000000000ULL) != 0L)
         {
            jjmatchedKind = 118;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0x20208840000000ULL, active1, 0L);
      case 85:
         return jjMoveStringLiteralDfa5_0(active0, 0x200000000ULL, active1, 0L);
      case 89:
         if ((active0 & 0x20000ULL) != 0L)
         {
            jjmatchedKind = 17;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0L, active1, 0x40000000000ULL);
      case 97:
         return jjMoveStringLiteralDfa5_0(active0, 0x600000000000000ULL, active1, 0x8000801000800ULL);
      case 99:
         return jjMoveStringLiteralDfa5_0(active0, 0L, active1, 0x100800000ULL);
      case 100:
         return jjMoveStringLiteralDfa5_0(active0, 0x2000000000000ULL, active1, 0L);
      case 101:
         if ((active0 & 0x100000000000ULL) != 0L)
         {
            jjmatchedKind = 44;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x100000ULL) != 0L)
         {
            jjmatchedKind = 84;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x10000000000000ULL) != 0L)
         {
            jjmatchedKind = 116;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x200000000000000ULL) != 0L)
         {
            jjmatchedKind = 121;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0x82000000ULL, active1, 0x1200078000ULL);
      case 102:
         if ((active0 & 0x2000000000ULL) != 0L)
         {
            jjmatchedKind = 37;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0L, active1, 0x800000000000ULL);
      case 105:
         return jjMoveStringLiteralDfa5_0(active0, 0x20210000ULL, active1, 0L);
      case 107:
         if ((active0 & 0x800000ULL) != 0L)
         {
            jjmatchedKind = 23;
            jjmatchedPos = 4;
         }
         break;
      case 108:
         if ((active0 & 0x100000000000000ULL) != 0L)
         {
            jjmatchedKind = 56;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x2000000000000ULL) != 0L)
         {
            jjmatchedKind = 113;
            jjmatchedPos = 4;
         }
         break;
      case 109:
         return jjMoveStringLiteralDfa5_0(active0, 0x180000ULL, active1, 0x1000ULL);
      case 110:
         if ((active0 & 0x400000ULL) != 0L)
         {
            jjmatchedKind = 22;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0x20000000000ULL, active1, 0x20000000000ULL);
      case 111:
         return jjMoveStringLiteralDfa5_0(active0, 0x408000000ULL, active1, 0L);
      case 112:
         if ((active0 & 0x1000000000000ULL) != 0L)
         {
            jjmatchedKind = 48;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x20000000000000ULL) != 0L)
         {
            jjmatchedKind = 117;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0L, active1, 0x4000ULL);
      case 114:
         if ((active0 & 0x1000ULL) != 0L)
         {
            jjmatchedKind = 12;
            jjmatchedPos = 4;
         }
         else if ((active0 & 0x100000000ULL) != 0L)
         {
            jjmatchedKind = 32;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0x808c00000040000ULL, active1, 0x496200200ULL);
      case 115:
         if ((active0 & 0x2000ULL) != 0L)
         {
            jjmatchedKind = 13;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x1000000000000ULL) != 0L)
         {
            jjmatchedKind = 112;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0x800ULL, active1, 0x200000400000ULL);
      case 116:
         if ((active0 & 0x40000000000000ULL) != 0L)
         {
            jjmatchedKind = 54;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x40000000000000ULL) != 0L)
         {
            jjmatchedKind = 118;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0x20208840000000ULL, active1, 0L);
      case 117:
         return jjMoveStringLiteralDfa5_0(active0, 0x200000000ULL, active1, 0L);
      case 121:
         if ((active0 & 0x20000ULL) != 0L)
         {
            jjmatchedKind = 17;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0L, active1, 0x40000000000ULL);
      default :
         break;
   }
   return jjMoveNfa_0(3, 4);
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa5_0(unsigned long long old0, unsigned long long active0, unsigned long long old1, unsigned long long active1){
   if (((active0 &= old0) | (active1 &= old1)) == 0L)
      return jjMoveNfa_0(3, 4);
   if (input_stream->endOfInput()) {
   return jjMoveNfa_0(3, 4);
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 65:
         return jjMoveStringLiteralDfa6_0(active0, 0x800400040000000ULL, active1, 0L);
      case 66:
         return jjMoveStringLiteralDfa6_0(active0, 0x200000ULL, active1, 0x8000000000000ULL);
      case 67:
         return jjMoveStringLiteralDfa6_0(active0, 0L, active1, 0x40000ULL);
      case 68:
         if ((active1 & 0x200000ULL) != 0L)
         {
            jjmatchedKind = 85;
            jjmatchedPos = 5;
         }
         else if ((active1 & 0x1000000000ULL) != 0L)
         {
            jjmatchedKind = 100;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0L, active1, 0x8000ULL);
      case 69:
         if ((active0 & 0x80000ULL) != 0L)
         {
            jjmatchedKind = 19;
            jjmatchedPos = 5;
         }
         else if ((active0 & 0x8000000000000ULL) != 0L)
         {
            jjmatchedKind = 51;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0x2020000100000ULL, active1, 0x800000001000ULL);
      case 71:
         if ((active1 & 0x20000000000ULL) != 0L)
         {
            jjmatchedKind = 105;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0x400000020000000ULL, active1, 0x800ULL);
      case 73:
         return jjMoveStringLiteralDfa6_0(active0, 0x20a00000000000ULL, active1, 0x406000000ULL);
      case 76:
         if ((active1 & 0x800000000ULL) != 0L)
         {
            jjmatchedKind = 99;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0x200000000ULL, active1, 0L);
      case 78:
         if ((active1 & 0x80000000ULL) != 0L)
         {
            jjmatchedKind = 95;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0x408000000ULL, active1, 0x200000000ULL);
      case 79:
         if ((active0 & 0x800000000ULL) != 0L)
         {
            jjmatchedKind = 35;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0L, active1, 0x4000ULL);
      case 80:
         return jjMoveStringLiteralDfa6_0(active0, 0L, active1, 0x240000000000ULL);
      case 82:
         if ((active0 & 0x2000000ULL) != 0L)
         {
            jjmatchedKind = 25;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0x200000000000000ULL, active1, 0x20000ULL);
      case 83:
         if ((active0 & 0x800ULL) != 0L)
         {
            jjmatchedKind = 11;
            jjmatchedPos = 5;
         }
         else if ((active1 & 0x200ULL) != 0L)
         {
            jjmatchedKind = 73;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0L, active1, 0x1010000ULL);
      case 84:
         if ((active0 & 0x40000ULL) != 0L)
         {
            jjmatchedKind = 18;
            jjmatchedPos = 5;
         }
         else if ((active1 & 0x800000ULL) != 0L)
         {
            jjmatchedKind = 87;
            jjmatchedPos = 5;
         }
         else if ((active1 & 0x10000000ULL) != 0L)
         {
            jjmatchedKind = 92;
            jjmatchedPos = 5;
         }
         else if ((active1 & 0x100000000ULL) != 0L)
         {
            jjmatchedKind = 96;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0x10000ULL, active1, 0x400000ULL);
      case 88:
         return jjMoveStringLiteralDfa6_0(active0, 0x80000000ULL, active1, 0L);
      case 89:
         if ((active0 & 0x8000000000ULL) != 0L)
         {
            jjmatchedKind = 39;
            jjmatchedPos = 5;
         }
         break;
      case 97:
         return jjMoveStringLiteralDfa6_0(active0, 0x800400040000000ULL, active1, 0L);
      case 98:
         return jjMoveStringLiteralDfa6_0(active0, 0x200000ULL, active1, 0x8000000000000ULL);
      case 99:
         return jjMoveStringLiteralDfa6_0(active0, 0L, active1, 0x40000ULL);
      case 100:
         if ((active1 & 0x200000ULL) != 0L)
         {
            jjmatchedKind = 85;
            jjmatchedPos = 5;
         }
         else if ((active1 & 0x1000000000ULL) != 0L)
         {
            jjmatchedKind = 100;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0L, active1, 0x8000ULL);
      case 101:
         if ((active0 & 0x80000ULL) != 0L)
         {
            jjmatchedKind = 19;
            jjmatchedPos = 5;
         }
         else if ((active0 & 0x8000000000000ULL) != 0L)
         {
            jjmatchedKind = 51;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0x2020000100000ULL, active1, 0x800000001000ULL);
      case 103:
         if ((active1 & 0x20000000000ULL) != 0L)
         {
            jjmatchedKind = 105;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0x400000020000000ULL, active1, 0x800ULL);
      case 105:
         return jjMoveStringLiteralDfa6_0(active0, 0x20a00000000000ULL, active1, 0x406000000ULL);
      case 108:
         if ((active1 & 0x800000000ULL) != 0L)
         {
            jjmatchedKind = 99;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0x200000000ULL, active1, 0L);
      case 110:
         if ((active1 & 0x80000000ULL) != 0L)
         {
            jjmatchedKind = 95;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0x408000000ULL, active1, 0x200000000ULL);
      case 111:
         if ((active0 & 0x800000000ULL) != 0L)
         {
            jjmatchedKind = 35;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0L, active1, 0x4000ULL);
      case 112:
         return jjMoveStringLiteralDfa6_0(active0, 0L, active1, 0x240000000000ULL);
      case 114:
         if ((active0 & 0x2000000ULL) != 0L)
         {
            jjmatchedKind = 25;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0x200000000000000ULL, active1, 0x20000ULL);
      case 115:
         if ((active0 & 0x800ULL) != 0L)
         {
            jjmatchedKind = 11;
            jjmatchedPos = 5;
         }
         else if ((active1 & 0x200ULL) != 0L)
         {
            jjmatchedKind = 73;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0L, active1, 0x1010000ULL);
      case 116:
         if ((active0 & 0x40000ULL) != 0L)
         {
            jjmatchedKind = 18;
            jjmatchedPos = 5;
         }
         else if ((active1 & 0x800000ULL) != 0L)
         {
            jjmatchedKind = 87;
            jjmatchedPos = 5;
         }
         else if ((active1 & 0x10000000ULL) != 0L)
         {
            jjmatchedKind = 92;
            jjmatchedPos = 5;
         }
         else if ((active1 & 0x100000000ULL) != 0L)
         {
            jjmatchedKind = 96;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0x10000ULL, active1, 0x400000ULL);
      case 120:
         return jjMoveStringLiteralDfa6_0(active0, 0x80000000ULL, active1, 0L);
      case 121:
         if ((active0 & 0x8000000000ULL) != 0L)
         {
            jjmatchedKind = 39;
            jjmatchedPos = 5;
         }
         break;
      default :
         break;
   }
   return jjMoveNfa_0(3, 5);
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa6_0(unsigned long long old0, unsigned long long active0, unsigned long long old1, unsigned long long active1){
   if (((active0 &= old0) | (active1 &= old1)) == 0L)
      return jjMoveNfa_0(3, 5);
   if (input_stream->endOfInput()) {
   return jjMoveNfa_0(3, 5);
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 65:
         return jjMoveStringLiteralDfa7_0(active0, 0x20000000000000ULL, active1, 0L);
      case 67:
         if ((active0 & 0x800000000000ULL) != 0L)
         {
            jjmatchedKind = 47;
            jjmatchedPos = 6;
         }
         return jjMoveStringLiteralDfa7_0(active0, 0L, active1, 0x800206000000ULL);
      case 68:
         if ((active0 & 0x2000000000000ULL) != 0L)
         {
            jjmatchedKind = 49;
            jjmatchedPos = 6;
         }
         break;
      case 69:
         if ((active0 & 0x400000000000000ULL) != 0L)
         {
            jjmatchedKind = 58;
            jjmatchedPos = 6;
         }
         else if ((active1 & 0x800ULL) != 0L)
         {
            jjmatchedKind = 75;
            jjmatchedPos = 6;
         }
         else if ((active1 & 0x1000000ULL) != 0L)
         {
            jjmatchedKind = 88;
            jjmatchedPos = 6;
         }
         else if ((active1 & 0x40000000000ULL) != 0L)
         {
            jjmatchedKind = 106;
            jjmatchedPos = 6;
         }
         return jjMoveStringLiteralDfa7_0(active0, 0x8010000ULL, active1, 0x400000ULL);
      case 76:
         if ((active0 & 0x800000000000000ULL) != 0L)
         {
            jjmatchedKind = 59;
            jjmatchedPos = 6;
         }
         return jjMoveStringLiteralDfa7_0(active0, 0L, active1, 0x8000000000000ULL);
      case 78:
         return jjMoveStringLiteralDfa7_0(active0, 0x440000000ULL, active1, 0x4000ULL);
      case 79:
         return jjMoveStringLiteralDfa7_0(active0, 0x200000000000ULL, active1, 0x200000000000ULL);
      case 83:
         if ((active1 & 0x10000ULL) != 0L)
         {
            jjmatchedKind = 80;
            jjmatchedPos = 6;
         }
         return jjMoveStringLiteralDfa7_0(active0, 0x20000000000ULL, active1, 0L);
      case 84:
         if ((active0 & 0x80000000ULL) != 0L)
         {
            jjmatchedKind = 31;
            jjmatchedPos = 6;
         }
         else if ((active0 & 0x200000000ULL) != 0L)
         {
            jjmatchedKind = 33;
            jjmatchedPos = 6;
         }
         return jjMoveStringLiteralDfa7_0(active0, 0x400000000000ULL, active1, 0x400061000ULL);
      case 85:
         return jjMoveStringLiteralDfa7_0(active0, 0x20200000ULL, active1, 0x8000ULL);
      case 89:
         if ((active0 & 0x200000000000000ULL) != 0L)
         {
            jjmatchedKind = 57;
            jjmatchedPos = 6;
         }
         break;
      case 95:
         return jjMoveStringLiteralDfa7_0(active0, 0x100000ULL, active1, 0L);
      case 97:
         return jjMoveStringLiteralDfa7_0(active0, 0x20000000000000ULL, active1, 0L);
      case 99:
         if ((active0 & 0x800000000000ULL) != 0L)
         {
            jjmatchedKind = 47;
            jjmatchedPos = 6;
         }
         return jjMoveStringLiteralDfa7_0(active0, 0L, active1, 0x800206000000ULL);
      case 100:
         if ((active0 & 0x2000000000000ULL) != 0L)
         {
            jjmatchedKind = 49;
            jjmatchedPos = 6;
         }
         break;
      case 101:
         if ((active0 & 0x400000000000000ULL) != 0L)
         {
            jjmatchedKind = 58;
            jjmatchedPos = 6;
         }
         else if ((active1 & 0x800ULL) != 0L)
         {
            jjmatchedKind = 75;
            jjmatchedPos = 6;
         }
         else if ((active1 & 0x1000000ULL) != 0L)
         {
            jjmatchedKind = 88;
            jjmatchedPos = 6;
         }
         else if ((active1 & 0x40000000000ULL) != 0L)
         {
            jjmatchedKind = 106;
            jjmatchedPos = 6;
         }
         return jjMoveStringLiteralDfa7_0(active0, 0x8010000ULL, active1, 0x400000ULL);
      case 108:
         if ((active0 & 0x800000000000000ULL) != 0L)
         {
            jjmatchedKind = 59;
            jjmatchedPos = 6;
         }
         return jjMoveStringLiteralDfa7_0(active0, 0L, active1, 0x8000000000000ULL);
      case 110:
         return jjMoveStringLiteralDfa7_0(active0, 0x440000000ULL, active1, 0x4000ULL);
      case 111:
         return jjMoveStringLiteralDfa7_0(active0, 0x200000000000ULL, active1, 0x200000000000ULL);
      case 115:
         if ((active1 & 0x10000ULL) != 0L)
         {
            jjmatchedKind = 80;
            jjmatchedPos = 6;
         }
         return jjMoveStringLiteralDfa7_0(active0, 0x20000000000ULL, active1, 0L);
      case 116:
         if ((active0 & 0x80000000ULL) != 0L)
         {
            jjmatchedKind = 31;
            jjmatchedPos = 6;
         }
         else if ((active0 & 0x200000000ULL) != 0L)
         {
            jjmatchedKind = 33;
            jjmatchedPos = 6;
         }
         return jjMoveStringLiteralDfa7_0(active0, 0x400000000000ULL, active1, 0x400061000ULL);
      case 117:
         return jjMoveStringLiteralDfa7_0(active0, 0x20200000ULL, active1, 0x8000ULL);
      case 121:
         if ((active0 & 0x200000000000000ULL) != 0L)
         {
            jjmatchedKind = 57;
            jjmatchedPos = 6;
         }
         break;
      default :
         break;
   }
   return jjMoveNfa_0(3, 6);
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa7_0(unsigned long long old0, unsigned long long active0, unsigned long long old1, unsigned long long active1){
   if (((active0 &= old0) | (active1 &= old1)) == 0L)
      return jjMoveNfa_0(3, 6);
   if (input_stream->endOfInput()) {
   return jjMoveNfa_0(3, 6);
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 67:
         return jjMoveStringLiteralDfa8_0(active0, 0x10000ULL, active1, 0L);
      case 69:
         if ((active0 & 0x400000000000ULL) != 0L)
         {
            jjmatchedKind = 46;
            jjmatchedPos = 7;
         }
         else if ((active1 & 0x200000000ULL) != 0L)
         {
            jjmatchedKind = 97;
            jjmatchedPos = 7;
         }
         else if ((active1 & 0x8000000000000ULL) != 0L)
         {
            jjmatchedKind = 115;
            jjmatchedPos = 7;
         }
         return jjMoveStringLiteralDfa8_0(active0, 0x400000000ULL, active1, 0x45000ULL);
      case 71:
         return jjMoveStringLiteralDfa8_0(active0, 0x100000ULL, active1, 0L);
      case 76:
         if ((active0 & 0x20000000000000ULL) != 0L)
         {
            jjmatchedKind = 53;
            jjmatchedPos = 7;
         }
         break;
      case 78:
         if ((active0 & 0x200000000000ULL) != 0L)
         {
            jjmatchedKind = 45;
            jjmatchedPos = 7;
         }
         return jjMoveStringLiteralDfa8_0(active0, 0x8000000ULL, active1, 0L);
      case 82:
         if ((active1 & 0x400000ULL) != 0L)
         {
            jjmatchedKind = 86;
            jjmatchedPos = 7;
         }
         return jjMoveStringLiteralDfa8_0(active0, 0x20000000ULL, active1, 0x200000008000ULL);
      case 83:
         if ((active0 & 0x20000000000ULL) != 0L)
         {
            jjmatchedKind = 41;
            jjmatchedPos = 7;
         }
         break;
      case 84:
         if ((active0 & 0x40000000ULL) != 0L)
         {
            jjmatchedKind = 30;
            jjmatchedPos = 7;
         }
         else if ((active1 & 0x2000000ULL) != 0L)
         {
            jjmatchedKind = 89;
            jjmatchedPos = 7;
         }
         return jjMoveStringLiteralDfa8_0(active0, 0x200000ULL, active1, 0x800004000000ULL);
      case 89:
         if ((active1 & 0x20000ULL) != 0L)
         {
            jjmatchedKind = 81;
            jjmatchedPos = 7;
         }
         else if ((active1 & 0x400000000ULL) != 0L)
         {
            jjmatchedKind = 98;
            jjmatchedPos = 7;
         }
         break;
      case 99:
         return jjMoveStringLiteralDfa8_0(active0, 0x10000ULL, active1, 0L);
      case 101:
         if ((active0 & 0x400000000000ULL) != 0L)
         {
            jjmatchedKind = 46;
            jjmatchedPos = 7;
         }
         else if ((active1 & 0x200000000ULL) != 0L)
         {
            jjmatchedKind = 97;
            jjmatchedPos = 7;
         }
         else if ((active1 & 0x8000000000000ULL) != 0L)
         {
            jjmatchedKind = 115;
            jjmatchedPos = 7;
         }
         return jjMoveStringLiteralDfa8_0(active0, 0x400000000ULL, active1, 0x45000ULL);
      case 103:
         return jjMoveStringLiteralDfa8_0(active0, 0x100000ULL, active1, 0L);
      case 108:
         if ((active0 & 0x20000000000000ULL) != 0L)
         {
            jjmatchedKind = 53;
            jjmatchedPos = 7;
         }
         break;
      case 110:
         if ((active0 & 0x200000000000ULL) != 0L)
         {
            jjmatchedKind = 45;
            jjmatchedPos = 7;
         }
         return jjMoveStringLiteralDfa8_0(active0, 0x8000000ULL, active1, 0L);
      case 114:
         if ((active1 & 0x400000ULL) != 0L)
         {
            jjmatchedKind = 86;
            jjmatchedPos = 7;
         }
         return jjMoveStringLiteralDfa8_0(active0, 0x20000000ULL, active1, 0x200000008000ULL);
      case 115:
         if ((active0 & 0x20000000000ULL) != 0L)
         {
            jjmatchedKind = 41;
            jjmatchedPos = 7;
         }
         break;
      case 116:
         if ((active0 & 0x40000000ULL) != 0L)
         {
            jjmatchedKind = 30;
            jjmatchedPos = 7;
         }
         else if ((active1 & 0x2000000ULL) != 0L)
         {
            jjmatchedKind = 89;
            jjmatchedPos = 7;
         }
         return jjMoveStringLiteralDfa8_0(active0, 0x200000ULL, active1, 0x800004000000ULL);
      case 121:
         if ((active1 & 0x20000ULL) != 0L)
         {
            jjmatchedKind = 81;
            jjmatchedPos = 7;
         }
         else if ((active1 & 0x400000000ULL) != 0L)
         {
            jjmatchedKind = 98;
            jjmatchedPos = 7;
         }
         break;
      default :
         break;
   }
   return jjMoveNfa_0(3, 7);
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa8_0(unsigned long long old0, unsigned long long active0, unsigned long long old1, unsigned long long active1){
   if (((active0 &= old0) | (active1 &= old1)) == 0L)
      return jjMoveNfa_0(3, 7);
   if (input_stream->endOfInput()) {
   return jjMoveNfa_0(3, 7);
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 65:
         return jjMoveStringLiteralDfa9_0(active0, 0x20000000ULL, active1, 0L);
      case 67:
         return jjMoveStringLiteralDfa9_0(active0, 0x400000000ULL, active1, 0L);
      case 68:
         if ((active1 & 0x4000ULL) != 0L)
         {
            jjmatchedKind = 78;
            jjmatchedPos = 8;
         }
         else if ((active1 & 0x40000ULL) != 0L)
         {
            jjmatchedKind = 82;
            jjmatchedPos = 8;
         }
         break;
      case 69:
         if ((active0 & 0x200000ULL) != 0L)
         {
            jjmatchedKind = 21;
            jjmatchedPos = 8;
         }
         else if ((active1 & 0x8000ULL) != 0L)
         {
            jjmatchedKind = 79;
            jjmatchedPos = 8;
         }
         return jjMoveStringLiteralDfa9_0(active0, 0L, active1, 0x800000000000ULL);
      case 82:
         if ((active1 & 0x1000ULL) != 0L)
         {
            jjmatchedKind = 76;
            jjmatchedPos = 8;
         }
         break;
      case 84:
         if ((active0 & 0x8000000ULL) != 0L)
         {
            jjmatchedKind = 27;
            jjmatchedPos = 8;
         }
         else if ((active1 & 0x200000000000ULL) != 0L)
         {
            jjmatchedKind = 109;
            jjmatchedPos = 8;
         }
         return jjMoveStringLiteralDfa9_0(active0, 0x10000ULL, active1, 0L);
      case 85:
         return jjMoveStringLiteralDfa9_0(active0, 0x100000ULL, active1, 0L);
      case 95:
         return jjMoveStringLiteralDfa9_0(active0, 0L, active1, 0x4000000ULL);
      case 97:
         return jjMoveStringLiteralDfa9_0(active0, 0x20000000ULL, active1, 0L);
      case 99:
         return jjMoveStringLiteralDfa9_0(active0, 0x400000000ULL, active1, 0L);
      case 100:
         if ((active1 & 0x4000ULL) != 0L)
         {
            jjmatchedKind = 78;
            jjmatchedPos = 8;
         }
         else if ((active1 & 0x40000ULL) != 0L)
         {
            jjmatchedKind = 82;
            jjmatchedPos = 8;
         }
         break;
      case 101:
         if ((active0 & 0x200000ULL) != 0L)
         {
            jjmatchedKind = 21;
            jjmatchedPos = 8;
         }
         else if ((active1 & 0x8000ULL) != 0L)
         {
            jjmatchedKind = 79;
            jjmatchedPos = 8;
         }
         return jjMoveStringLiteralDfa9_0(active0, 0L, active1, 0x800000000000ULL);
      case 114:
         if ((active1 & 0x1000ULL) != 0L)
         {
            jjmatchedKind = 76;
            jjmatchedPos = 8;
         }
         break;
      case 116:
         if ((active0 & 0x8000000ULL) != 0L)
         {
            jjmatchedKind = 27;
            jjmatchedPos = 8;
         }
         else if ((active1 & 0x200000000000ULL) != 0L)
         {
            jjmatchedKind = 109;
            jjmatchedPos = 8;
         }
         return jjMoveStringLiteralDfa9_0(active0, 0x10000ULL, active1, 0L);
      case 117:
         return jjMoveStringLiteralDfa9_0(active0, 0x100000ULL, active1, 0L);
      default :
         break;
   }
   return jjMoveNfa_0(3, 8);
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa9_0(unsigned long long old0, unsigned long long active0, unsigned long long old1, unsigned long long active1){
   if (((active0 &= old0) | (active1 &= old1)) == 0L)
      return jjMoveNfa_0(3, 8);
   if (input_stream->endOfInput()) {
   return jjMoveNfa_0(3, 8);
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 65:
         return jjMoveStringLiteralDfa10_0(active0, 0x100000ULL, active1, 0L);
      case 68:
         if ((active1 & 0x800000000000ULL) != 0L)
         {
            jjmatchedKind = 111;
            jjmatchedPos = 9;
         }
         break;
      case 71:
         return jjMoveStringLiteralDfa10_0(active0, 0L, active1, 0x4000000ULL);
      case 84:
         if ((active0 & 0x400000000ULL) != 0L)
         {
            jjmatchedKind = 34;
            jjmatchedPos = 9;
         }
         return jjMoveStringLiteralDfa10_0(active0, 0x20000000ULL, active1, 0L);
      case 85:
         return jjMoveStringLiteralDfa10_0(active0, 0x10000ULL, active1, 0L);
      case 97:
         return jjMoveStringLiteralDfa10_0(active0, 0x100000ULL, active1, 0L);
      case 100:
         if ((active1 & 0x800000000000ULL) != 0L)
         {
            jjmatchedKind = 111;
            jjmatchedPos = 9;
         }
         break;
      case 103:
         return jjMoveStringLiteralDfa10_0(active0, 0L, active1, 0x4000000ULL);
      case 116:
         if ((active0 & 0x400000000ULL) != 0L)
         {
            jjmatchedKind = 34;
            jjmatchedPos = 9;
         }
         return jjMoveStringLiteralDfa10_0(active0, 0x20000000ULL, active1, 0L);
      case 117:
         return jjMoveStringLiteralDfa10_0(active0, 0x10000ULL, active1, 0L);
      default :
         break;
   }
   return jjMoveNfa_0(3, 9);
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa10_0(unsigned long long old0, unsigned long long active0, unsigned long long old1, unsigned long long active1){
   if (((active0 &= old0) | (active1 &= old1)) == 0L)
      return jjMoveNfa_0(3, 9);
   if (input_stream->endOfInput()) {
   return jjMoveNfa_0(3, 9);
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 73:
         return jjMoveStringLiteralDfa11_0(active0, 0x20000000ULL, active1, 0L);
      case 82:
         return jjMoveStringLiteralDfa11_0(active0, 0x110000ULL, active1, 0L);
      case 85:
         return jjMoveStringLiteralDfa11_0(active0, 0L, active1, 0x4000000ULL);
      case 105:
         return jjMoveStringLiteralDfa11_0(active0, 0x20000000ULL, active1, 0L);
      case 114:
         return jjMoveStringLiteralDfa11_0(active0, 0x110000ULL, active1, 0L);
      case 117:
         return jjMoveStringLiteralDfa11_0(active0, 0L, active1, 0x4000000ULL);
      default :
         break;
   }
   return jjMoveNfa_0(3, 10);
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa11_0(unsigned long long old0, unsigned long long active0, unsigned long long old1, unsigned long long active1){
   if (((active0 &= old0) | (active1 &= old1)) == 0L)
      return jjMoveNfa_0(3, 10);
   if (input_stream->endOfInput()) {
   return jjMoveNfa_0(3, 10);
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 65:
         return jjMoveStringLiteralDfa12_0(active0, 0L, active1, 0x4000000ULL);
      case 69:
         if ((active0 & 0x10000ULL) != 0L)
         {
            jjmatchedKind = 16;
            jjmatchedPos = 11;
         }
         return jjMoveStringLiteralDfa12_0(active0, 0x100000ULL, active1, 0L);
      case 79:
         return jjMoveStringLiteralDfa12_0(active0, 0x20000000ULL, active1, 0L);
      case 97:
         return jjMoveStringLiteralDfa12_0(active0, 0L, active1, 0x4000000ULL);
      case 101:
         if ((active0 & 0x10000ULL) != 0L)
         {
            jjmatchedKind = 16;
            jjmatchedPos = 11;
         }
         return jjMoveStringLiteralDfa12_0(active0, 0x100000ULL, active1, 0L);
      case 111:
         return jjMoveStringLiteralDfa12_0(active0, 0x20000000ULL, active1, 0L);
      default :
         break;
   }
   return jjMoveNfa_0(3, 11);
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa12_0(unsigned long long old0, unsigned long long active0, unsigned long long old1, unsigned long long active1){
   if (((active0 &= old0) | (active1 &= old1)) == 0L)
      return jjMoveNfa_0(3, 11);
   if (input_stream->endOfInput()) {
   return jjMoveNfa_0(3, 11);
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 78:
         if ((active0 & 0x20000000ULL) != 0L)
         {
            jjmatchedKind = 29;
            jjmatchedPos = 12;
         }
         return jjMoveStringLiteralDfa13_0(active0, 0x100000ULL, active1, 0L);
      case 82:
         return jjMoveStringLiteralDfa13_0(active0, 0L, active1, 0x4000000ULL);
      case 110:
         if ((active0 & 0x20000000ULL) != 0L)
         {
            jjmatchedKind = 29;
            jjmatchedPos = 12;
         }
         return jjMoveStringLiteralDfa13_0(active0, 0x100000ULL, active1, 0L);
      case 114:
         return jjMoveStringLiteralDfa13_0(active0, 0L, active1, 0x4000000ULL);
      default :
         break;
   }
   return jjMoveNfa_0(3, 12);
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa13_0(unsigned long long old0, unsigned long long active0, unsigned long long old1, unsigned long long active1){
   if (((active0 &= old0) | (active1 &= old1)) == 0L)
      return jjMoveNfa_0(3, 12);
   if (input_stream->endOfInput()) {
   return jjMoveNfa_0(3, 12);
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 65:
         return jjMoveStringLiteralDfa14_0(active0, 0L, active1, 0x4000000ULL);
      case 84:
         return jjMoveStringLiteralDfa14_0(active0, 0x100000ULL, active1, 0L);
      case 97:
         return jjMoveStringLiteralDfa14_0(active0, 0L, active1, 0x4000000ULL);
      case 116:
         return jjMoveStringLiteralDfa14_0(active0, 0x100000ULL, active1, 0L);
      default :
         break;
   }
   return jjMoveNfa_0(3, 13);
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa14_0(unsigned long long old0, unsigned long long active0, unsigned long long old1, unsigned long long active1){
   if (((active0 &= old0) | (active1 &= old1)) == 0L)
      return jjMoveNfa_0(3, 13);
   if (input_stream->endOfInput()) {
   return jjMoveNfa_0(3, 13);
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 69:
         return jjMoveStringLiteralDfa15_0(active0, 0x100000ULL, active1, 0L);
      case 78:
         return jjMoveStringLiteralDfa15_0(active0, 0L, active1, 0x4000000ULL);
      case 101:
         return jjMoveStringLiteralDfa15_0(active0, 0x100000ULL, active1, 0L);
      case 110:
         return jjMoveStringLiteralDfa15_0(active0, 0L, active1, 0x4000000ULL);
      default :
         break;
   }
   return jjMoveNfa_0(3, 14);
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa15_0(unsigned long long old0, unsigned long long active0, unsigned long long old1, unsigned long long active1){
   if (((active0 &= old0) | (active1 &= old1)) == 0L)
      return jjMoveNfa_0(3, 14);
   if (input_stream->endOfInput()) {
   return jjMoveNfa_0(3, 14);
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 69:
         if ((active0 & 0x100000ULL) != 0L)
         {
            jjmatchedKind = 20;
            jjmatchedPos = 15;
         }
         break;
      case 84:
         return jjMoveStringLiteralDfa16_0(active0, 0L, active1, 0x4000000ULL);
      case 101:
         if ((active0 & 0x100000ULL) != 0L)
         {
            jjmatchedKind = 20;
            jjmatchedPos = 15;
         }
         break;
      case 116:
         return jjMoveStringLiteralDfa16_0(active0, 0L, active1, 0x4000000ULL);
      default :
         break;
   }
   return jjMoveNfa_0(3, 15);
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa16_0(unsigned long long old0, unsigned long long active0, unsigned long long old1, unsigned long long active1){
   if (((active0 &= old0) | (active1 &= old1)) == 0L)
      return jjMoveNfa_0(3, 15);
   if (input_stream->endOfInput()) {
   return jjMoveNfa_0(3, 15);
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 69:
         return jjMoveStringLiteralDfa17_0(active1, 0x4000000ULL);
      case 101:
         return jjMoveStringLiteralDfa17_0(active1, 0x4000000ULL);
      default :
         break;
   }
   return jjMoveNfa_0(3, 16);
}

 int  VhdlParserTokenManager::jjMoveStringLiteralDfa17_0(unsigned long long old1, unsigned long long active1){
   if (((active1 &= old1)) == 0L)
      return jjMoveNfa_0(3, 16);
   if (input_stream->endOfInput()) {
   return jjMoveNfa_0(3, 16);
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 69:
         if ((active1 & 0x4000000ULL) != 0L)
         {
            jjmatchedKind = 90;
            jjmatchedPos = 17;
         }
         break;
      case 101:
         if ((active1 & 0x4000000ULL) != 0L)
         {
            jjmatchedKind = 90;
            jjmatchedPos = 17;
         }
         break;
      default :
         break;
   }
   return jjMoveNfa_0(3, 17);
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
   jjnewStateCnt = 65;
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
         if (l == 1);
         do
         {
            switch(jjstateSet[--i])
            {
               case 3:
                  if ((0x3ff000000000000ULL & l) != 0L)
                  {
                     if (kind > 164)
                        kind = 164;
                     { jjCheckNAddStates(0, 8); }
                  }
                  else if (curChar == 47)
                     { jjAddStates(9, 10); }
                  else if (curChar == 45)
                     { jjCheckNAddTwoStates(21, 27); }
                  else if (curChar == 39)
                     jjstateSet[jjnewStateCnt++] = 10;
                  else if (curChar == 34)
                     { jjCheckNAddTwoStates(1, 2); }
                  if ((0x3ff000000000000ULL & l) != 0L)
                  {
                     if (kind > 170)
                        kind = 170;
                     { jjCheckNAdd(12); }
                  }
                  break;
               case 0:
                  if (curChar == 34)
                     { jjCheckNAddTwoStates(1, 2); }
                  break;
               case 1:
                  if ((0xfffffffb00000200ULL & l) != 0L)
                     { jjCheckNAddTwoStates(1, 2); }
                  break;
               case 2:
                  if (curChar != 34)
                     break;
                  if (kind > 165)
                     kind = 165;
                  jjstateSet[jjnewStateCnt++] = 0;
                  break;
               case 5:
                  if ((0x3ff000000000000ULL & l) == 0L)
                     break;
                  if (kind > 166)
                     kind = 166;
                  { jjAddStates(11, 12); }
                  break;
               case 7:
                  if ((0xfffffffb00000200ULL & l) != 0L)
                     { jjAddStates(13, 14); }
                  break;
               case 9:
                  if (curChar == 39)
                     jjstateSet[jjnewStateCnt++] = 10;
                  break;
               case 10:
                  if ((0xfffffffb00000200ULL & l) != 0L)
                     jjstateSet[jjnewStateCnt++] = 11;
                  break;
               case 11:
                  if (curChar == 39 && kind > 168)
                     kind = 168;
                  break;
               case 12:
                  if ((0x3ff000000000000ULL & l) == 0L)
                     break;
                  if (kind > 170)
                     kind = 170;
                  { jjCheckNAdd(12); }
                  break;
               case 14:
                  if (curChar == 34)
                     { jjCheckNAddTwoStates(15, 16); }
                  break;
               case 15:
                  if ((0x3ff000000000000ULL & l) != 0L)
                     { jjCheckNAddTwoStates(15, 16); }
                  break;
               case 16:
                  if (curChar == 34 && kind > 178)
                     kind = 178;
                  break;
               case 18:
                  if ((0xfffffffb00000200ULL & l) == 0L)
                     break;
                  if (kind > 185)
                     kind = 185;
                  jjstateSet[jjnewStateCnt++] = 18;
                  break;
               case 19:
                  if (curChar == 45)
                     { jjCheckNAddTwoStates(21, 27); }
                  break;
               case 20:
                  if (curChar != 33)
                     break;
                  if (kind > 6)
                     kind = 6;
                  { jjCheckNAddStates(15, 18); }
                  break;
               case 21:
                  if (curChar == 45)
                     jjstateSet[jjnewStateCnt++] = 20;
                  break;
               case 22:
                  if (curChar == 45)
                     { jjCheckNAdd(21); }
                  break;
               case 23:
                  if ((0xffffffffffffdbffULL & l) == 0L)
                     break;
                  if (kind > 6)
                     kind = 6;
                  { jjCheckNAddStates(15, 18); }
                  break;
               case 24:
                  if ((0x2400ULL & l) == 0L)
                     break;
                  if (kind > 6)
                     kind = 6;
                  { jjCheckNAdd(22); }
                  break;
               case 25:
                  if (curChar != 10)
                     break;
                  if (kind > 6)
                     kind = 6;
                  { jjCheckNAdd(22); }
                  break;
               case 26:
                  if (curChar == 13)
                     jjstateSet[jjnewStateCnt++] = 25;
                  break;
               case 27:
                  if (curChar != 45)
                     break;
                  if (kind > 7)
                     kind = 7;
                  { jjCheckNAddStates(19, 21); }
                  break;
               case 28:
                  if ((0xffffffffffffdbffULL & l) == 0L)
                     break;
                  if (kind > 7)
                     kind = 7;
                  { jjCheckNAddStates(19, 21); }
                  break;
               case 29:
                  if ((0x2400ULL & l) != 0L && kind > 7)
                     kind = 7;
                  break;
               case 30:
                  if (curChar == 10 && kind > 7)
                     kind = 7;
                  break;
               case 31:
                  if (curChar == 13)
                     jjstateSet[jjnewStateCnt++] = 30;
                  break;
               case 32:
                  if (curChar == 47)
                     { jjAddStates(9, 10); }
                  break;
               case 33:
                  if (curChar == 33)
                     { jjCheckNAddTwoStates(34, 36); }
                  break;
               case 34:
                  { jjCheckNAddTwoStates(34, 36); }
                  break;
               case 35:
                  if (curChar == 47 && kind > 8)
                     kind = 8;
                  break;
               case 36:
                  if (curChar == 42)
                     jjstateSet[jjnewStateCnt++] = 35;
                  break;
               case 37:
                  if (curChar == 42)
                     jjstateSet[jjnewStateCnt++] = 33;
                  break;
               case 38:
                  if (curChar == 42)
                     { jjCheckNAddTwoStates(39, 41); }
                  break;
               case 39:
                  { jjCheckNAddTwoStates(39, 41); }
                  break;
               case 40:
                  if (curChar == 47 && kind > 9)
                     kind = 9;
                  break;
               case 41:
                  if (curChar == 42)
                     jjstateSet[jjnewStateCnt++] = 40;
                  break;
               case 42:
                  if ((0x3ff000000000000ULL & l) == 0L)
                     break;
                  if (kind > 164)
                     kind = 164;
                  { jjCheckNAddStates(0, 8); }
                  break;
               case 44:
                  if ((0x3ff000000000000ULL & l) == 0L)
                     break;
                  if (kind > 164)
                     kind = 164;
                  { jjCheckNAddTwoStates(43, 44); }
                  break;
               case 46:
                  if ((0x3ff000000000000ULL & l) == 0L)
                     break;
                  if (kind > 169)
                     kind = 169;
                  { jjCheckNAddStates(22, 25); }
                  break;
               case 47:
                  if (curChar == 46)
                     { jjCheckNAdd(48); }
                  break;
               case 48:
                  if ((0x3ff000000000000ULL & l) == 0L)
                     break;
                  if (kind > 169)
                     kind = 169;
                  { jjCheckNAddStates(26, 28); }
                  break;
               case 51:
                  if ((0x280000000000ULL & l) != 0L)
                     { jjCheckNAdd(52); }
                  break;
               case 52:
                  if ((0x3ff000000000000ULL & l) == 0L)
                     break;
                  if (kind > 169)
                     kind = 169;
                  { jjCheckNAddTwoStates(53, 52); }
                  break;
               case 55:
                  if ((0x3ff000000000000ULL & l) != 0L)
                     { jjCheckNAddStates(29, 31); }
                  break;
               case 56:
                  if (curChar == 35)
                     { jjCheckNAdd(57); }
                  break;
               case 57:
                  if ((0x3ff000000000000ULL & l) != 0L)
                     { jjCheckNAddStates(32, 34); }
                  break;
               case 58:
                  if (curChar == 46)
                     { jjCheckNAdd(59); }
                  break;
               case 59:
                  if ((0x3ff000000000000ULL & l) != 0L)
                     { jjCheckNAddTwoStates(59, 60); }
                  break;
               case 60:
                  if (curChar != 35)
                     break;
                  if (kind > 171)
                     kind = 171;
                  jjstateSet[jjnewStateCnt++] = 61;
                  break;
               case 62:
                  if ((0x280000000000ULL & l) != 0L)
                     { jjCheckNAdd(63); }
                  break;
               case 63:
                  if ((0x3ff000000000000ULL & l) == 0L)
                     break;
                  if (kind > 171)
                     kind = 171;
                  { jjCheckNAddTwoStates(64, 63); }
                  break;
               default : break;
            }
         } while(i != startsAt);
      }
      else if (curChar < 128)
      {
         unsigned long long l = 1ULL << (curChar & 077);
         if (l == 1);
         do
         {
            switch(jjstateSet[--i])
            {
               case 3:
                  if ((0x7fffffe07fffffeULL & l) != 0L)
                  {
                     if (kind > 170)
                        kind = 170;
                     { jjCheckNAdd(12); }
                  }
                  else if (curChar == 96)
                     { jjCheckNAdd(18); }
                  else if (curChar == 92)
                     { jjCheckNAddTwoStates(7, 8); }
                  if ((0x7fffffe07fffffeULL & l) != 0L)
                  {
                     if (kind > 166)
                        kind = 166;
                     { jjCheckNAddTwoStates(4, 5); }
                  }
                  if ((0x100800401008004ULL & l) != 0L)
                     jjstateSet[jjnewStateCnt++] = 14;
                  break;
               case 1:
                  if ((0x7fffffffffffffffULL & l) != 0L)
                     { jjAddStates(35, 36); }
                  break;
               case 4:
                  if (curChar == 95)
                     { jjCheckNAddTwoStates(4, 5); }
                  break;
               case 5:
                  if ((0x7fffffe07fffffeULL & l) == 0L)
                     break;
                  if (kind > 166)
                     kind = 166;
                  { jjCheckNAddTwoStates(4, 5); }
                  break;
               case 6:
                  if (curChar == 92)
                     { jjCheckNAddTwoStates(7, 8); }
                  break;
               case 7:
                  if ((0x7fffffffffffffffULL & l) != 0L)
                     { jjCheckNAddTwoStates(7, 8); }
                  break;
               case 8:
                  if (curChar == 92 && kind > 167)
                     kind = 167;
                  break;
               case 10:
                  if ((0x7fffffffffffffffULL & l) != 0L)
                     jjstateSet[jjnewStateCnt++] = 11;
                  break;
               case 12:
                  if ((0x7fffffe07fffffeULL & l) == 0L)
                     break;
                  if (kind > 170)
                     kind = 170;
                  { jjCheckNAdd(12); }
                  break;
               case 13:
                  if ((0x100800401008004ULL & l) != 0L)
                     jjstateSet[jjnewStateCnt++] = 14;
                  break;
               case 15:
                  if ((0x7fffffe07fffffeULL & l) != 0L)
                     { jjAddStates(37, 38); }
                  break;
               case 17:
                  if (curChar == 96)
                     { jjCheckNAdd(18); }
                  break;
               case 18:
                  if ((0x7fffffffffffffffULL & l) == 0L)
                     break;
                  if (kind > 185)
                     kind = 185;
                  { jjCheckNAdd(18); }
                  break;
               case 23:
                  if (kind > 6)
                     kind = 6;
                  { jjAddStates(15, 18); }
                  break;
               case 28:
                  if (kind > 7)
                     kind = 7;
                  { jjAddStates(19, 21); }
                  break;
               case 34:
                  { jjAddStates(39, 40); }
                  break;
               case 39:
                  { jjAddStates(41, 42); }
                  break;
               case 43:
                  if (curChar == 95)
                     jjstateSet[jjnewStateCnt++] = 44;
                  break;
               case 45:
                  if (curChar == 95)
                     jjstateSet[jjnewStateCnt++] = 46;
                  break;
               case 49:
                  if (curChar == 95)
                     jjstateSet[jjnewStateCnt++] = 48;
                  break;
               case 50:
                  if ((0x2000000020ULL & l) != 0L)
                     { jjCheckNAddTwoStates(51, 52); }
                  break;
               case 53:
                  if (curChar == 95)
                     { jjCheckNAdd(52); }
                  break;
               case 54:
                  if (curChar == 95)
                     jjstateSet[jjnewStateCnt++] = 55;
                  break;
               case 57:
                  if ((0x7fffffe07fffffeULL & l) != 0L)
                     { jjCheckNAddStates(32, 34); }
                  break;
               case 59:
                  if ((0x7fffffe07fffffeULL & l) != 0L)
                     { jjCheckNAddTwoStates(59, 60); }
                  break;
               case 61:
                  if ((0x2000000020ULL & l) != 0L)
                     { jjCheckNAddTwoStates(62, 63); }
                  break;
               case 64:
                  if (curChar == 95)
                     { jjCheckNAdd(63); }
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
               case 1:
                  if (jjCanMove_0(hiByte, i1, i2, l1, l2))
                     { jjAddStates(35, 36); }
                  break;
               case 7:
                  if (jjCanMove_0(hiByte, i1, i2, l1, l2))
                     { jjAddStates(13, 14); }
                  break;
               case 10:
                  if (jjCanMove_0(hiByte, i1, i2, l1, l2))
                     jjstateSet[jjnewStateCnt++] = 11;
                  break;
               case 18:
                  if (!jjCanMove_0(hiByte, i1, i2, l1, l2))
                     break;
                  if (kind > 185)
                     kind = 185;
                  jjstateSet[jjnewStateCnt++] = 18;
                  break;
               case 23:
                  if (!jjCanMove_1(hiByte, i1, i2, l1, l2))
                     break;
                  if (kind > 6)
                     kind = 6;
                  { jjAddStates(15, 18); }
                  break;
               case 28:
                  if (!jjCanMove_1(hiByte, i1, i2, l1, l2))
                     break;
                  if (kind > 7)
                     kind = 7;
                  { jjAddStates(19, 21); }
                  break;
               case 34:
                  if (jjCanMove_1(hiByte, i1, i2, l1, l2))
                     { jjAddStates(39, 40); }
                  break;
               case 39:
                  if (jjCanMove_1(hiByte, i1, i2, l1, l2))
                     { jjAddStates(41, 42); }
                  break;
               default : if (i1 == 0 || l1 == 0 || i2 == 0 || l2 == 0) break; else break;
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
      if ((i = jjnewStateCnt), (jjnewStateCnt = startsAt), (i == (startsAt = 65 - startsAt)))
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
         return ((jjbitVec0[i2] & l2) != 0L);
      default :
         return false;
   }
}

bool VhdlParserTokenManager::jjCanMove_1(int hiByte, int i1, int i2, unsigned long long l1, unsigned long long l2){
   switch(hiByte)
   {
      case 0:
         return ((jjbitVec3[i2] & l2) != 0L);
      default :
         if ((jjbitVec1[i1] & l1) != 0L)
            return true;
         return false;
   }
}

/** Token literal values. */

Token * VhdlParserTokenManager::jjFillToken(){
   Token *t;
   JAVACC_STRING_TYPE curTokenImage;
   int beginLine;
   int endLine;
   int beginColumn;
   int endColumn;
   JAVACC_STRING_TYPE im = jjstrLiteralImages[jjmatchedKind];
   curTokenImage = (im.length() == 0) ? input_stream->GetImage() : im;
   if (input_stream->getTrackLineColumn()) {
     beginLine = input_stream->getBeginLine();
     beginColumn = input_stream->getBeginColumn();
     endLine = input_stream->getEndLine();
     endColumn = input_stream->getEndColumn();
   }
   t = Token::newToken(jjmatchedKind, curTokenImage);
   t->specialToken = NULL;
   t->next = NULL;

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
  Token *matchedToken;
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

   for (;;)
   {
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
      else if ((jjtoSkip[jjmatchedKind >> 6] & (1ULL << (jjmatchedKind & 077))) != 0L)
      {
         SkipLexicalActions(NULL);
         goto EOFLoop;
      }
      MoreLexicalActions();
      curPos = 0;
      jjmatchedKind = 0x7fffffff;
   if (!input_stream->endOfInput()) {
         curChar = input_stream->readChar();
   continue;
 }
   }
   int error_line = input_stream->getEndLine();
   int error_column = input_stream->getEndColumn();
   JAVACC_STRING_TYPE error_after;
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
   lexicalError();
 }
  }
}


void  VhdlParserTokenManager::SkipLexicalActions(Token *matchedToken){
   switch(jjmatchedKind)
   {
      case 3 :
         image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
        ::vhdl::parser::VhdlParser::lineCount();
         break;
      default :
         break;
   }
}

void  VhdlParserTokenManager::MoreLexicalActions(){
   jjimageLen += (lengthOfMatch = jjmatchedPos + 1);
   switch(jjmatchedKind)
   {
      case 6 :
         image.append(input_stream->GetSuffix(jjimageLen));
         jjimageLen = 0;
                                                                                         ::vhdl::parser::VhdlParser::handleCommentBlock(image.data(),TRUE);image.clear();
         break;
      case 7 :
         image.append(input_stream->GetSuffix(jjimageLen));
         jjimageLen = 0;
                                                                    ::vhdl::parser::VhdlParser::lineCount(image.data());image.clear();
         break;
      case 8 :
         image.append(input_stream->GetSuffix(jjimageLen));
         jjimageLen = 0;
                                                          ::vhdl::parser::VhdlParser::handleCommentBlock(image.data(),TRUE);image.clear();
         break;
      case 9 :
         image.append(input_stream->GetSuffix(jjimageLen));
         jjimageLen = 0;
                                                    ::vhdl::parser::VhdlParser::lineCount(image.data());image.clear();
         break;
      default :
         break;
   }
}

void  VhdlParserTokenManager::TokenLexicalActions(Token *matchedToken){
   switch(jjmatchedKind)
   {
      case 16 :
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                                    ::vhdl::parser::VhdlParser::setLineParsed(ARCHITECTURE_T);
         break;
      case 17 :
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                      ::vhdl::parser::VhdlParser::setLineParsed(ARRAY_T);
         break;
      case 21 :
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                              ::vhdl::parser::VhdlParser::setLineParsed(ATTRIBUTE_T);
         break;
      case 24 :
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                    ::vhdl::parser::VhdlParser::setLineParsed(BODY_T);
         break;
      case 27 :
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                              ::vhdl::parser::VhdlParser::setLineParsed(COMPONENT_T);
         break;
      case 29 :
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                                      ::vhdl::parser::VhdlParser::setLineParsed(CONFIGURATION_T);
         break;
      case 30 :
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                             ::vhdl::parser::VhdlParser::setLineParsed(CONSTANT_T);
         break;
      case 31 :
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                          ::vhdl::parser::VhdlParser::setLineParsed(CONTEXT_T);
         break;
      case 38 :
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                  ::vhdl::parser::VhdlParser::setLineParsed(END_T);
         break;
      case 39 :
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                        ::vhdl::parser::VhdlParser::setLineParsed(ENTITY_T);
         break;
      case 42 :
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                    ::vhdl::parser::VhdlParser::setLineParsed(FILE_T);
         break;
      case 45 :
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                             ::vhdl::parser::VhdlParser::setLineParsed(FUNCTION_T);
         break;
      case 48 :
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                      ::vhdl::parser::VhdlParser::setLineParsed(GROUP_T);
         break;
      case 57 :
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                          ::vhdl::parser::VhdlParser::setLineParsed(LIBRARY_T);
         break;
      case 75 :
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                          ::vhdl::parser::VhdlParser::setLineParsed(PACKAGE_T);
         break;
      case 77 :
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                    ::vhdl::parser::VhdlParser::setLineParsed(PORT_T);
         break;
      case 79 :
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                              ::vhdl::parser::VhdlParser::setLineParsed(PROCEDURE_T);
         break;
      case 80 :
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                          ::vhdl::parser::VhdlParser::setLineParsed(PROCESS_T);
         break;
      case 85 :
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                        ::vhdl::parser::VhdlParser::setLineParsed(RECORD_T);
         break;
      case 99 :
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                        ::vhdl::parser::VhdlParser::setLineParsed(SIGNAL_T);
         break;
      case 106 :
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                          ::vhdl::parser::VhdlParser::setLineParsed(SUBTYPE_T);
         break;
      case 110 :
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                    ::vhdl::parser::VhdlParser::setLineParsed(TYPE_T);
         break;
      case 112 :
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                      ::vhdl::parser::VhdlParser::setLineParsed(UNITS_T);
         break;
      default :
         break;
   }
}
  /** Reinitialise parser. */
  void VhdlParserTokenManager::ReInit(JAVACC_CHARSTREAM *stream, int lexState, VhdlParser *parserArg) {
    if (input_stream) delete input_stream;
    jjmatchedPos = jjnewStateCnt = 0;
    curLexState = lexState;
    input_stream = stream;
    ReInitRounds();
    debugStream = stdout; // init
    SwitchTo(lexState);
    parser = parserArg;
  }

  void VhdlParserTokenManager::ReInitRounds() {
    int i;
    jjround = 0x80000001;
    for (i = 65; i-- > 0;)
      jjrounds[i] = 0x80000000;
  }

  /** Switch to specified lex state. */
  void VhdlParserTokenManager::SwitchTo(int lexState) {
    if (lexState >= 1 || lexState < 0)
      assert(false);
      //throw 1;//new TokenMgrError("Error: Ignoring invalid lexical state : " + lexState + ". State unchanged.", TokenMgrError.INVALID_LEXICAL_STATE);
    else
      curLexState = lexState;
  }

  /** Constructor. */
  VhdlParserTokenManager::VhdlParserTokenManager (JAVACC_CHARSTREAM *stream, int lexState, VhdlParser *parserArg)
  {
    input_stream = NULL;
    ReInit(stream, lexState, parserArg);
  }

  // Destructor
  VhdlParserTokenManager::~VhdlParserTokenManager () {
    if (input_stream) delete input_stream;
  }
}
}
