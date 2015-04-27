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
   54, 55, 56, 57, 58, 61, 65, 66, 67, 48, 49, 2, 37, 38, 0, 3, 
   4, 5, 7, 12, 13, 15, 16, 24, 23, 25, 33, 34, 36, 39, 40, 42, 
   56, 57, 58, 61, 60, 59, 61, 65, 66, 67, 68, 69, 71, 9, 10, 28, 
   29, 45, 47, 50, 52, 27, 30, 
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

static JAVACC_CHAR_TYPE jjstrLiteralChars_126[] = {0x26, 0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_127[] = {0x27, 0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_128[] = {0x28, 0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_129[] = {0x29, 0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_130[] = {0x2a, 0x2a, 0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_131[] = {0x2a, 0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_132[] = {0x2b, 0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_133[] = {0x2d, 0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_134[] = {0x2c, 0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_135[] = {0x3a, 0x3d, 0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_136[] = {0x3a, 0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_137[] = {0x3b, 0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_138[] = {0x3c, 0x3d, 0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_139[] = {0x3e, 0x3d, 0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_140[] = {0x3c, 0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_141[] = {0x3e, 0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_142[] = {0x3d, 0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_143[] = {0x2f, 0x3d, 0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_144[] = {0x3d, 0x3e, 0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_145[] = {0x3c, 0x3e, 0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_146[] = {0x3c, 0x3c, 0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_147[] = {0x3e, 0x3e, 0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_148[] = {0x3f, 0x3f, 0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_149[] = {0x3f, 0x3e, 0x3d, 0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_150[] = {0x3f, 0x3c, 0x3d, 0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_151[] = {0x3f, 0x3e, 0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_152[] = {0x3f, 0x3c, 0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_153[] = {0x3f, 0x3d, 0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_154[] = {0x3f, 0x2f, 0x3d, 0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_155[] = {0x3f, 0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_156[] = {0x7c, 0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_157[] = {0x2e, 0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_158[] = {0x2f, 0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_159[] = {0x40, 0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_160[] = {0x5e, 0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_161[] = {0x5b, 0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_162[] = {0x5d, 0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_163[] = {0x7b, 0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_164[] = {0x7d, 0};
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
static JAVACC_CHAR_TYPE jjstrLiteralChars_186[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_187[] = {0};
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
jjstrLiteralChars_186, 
jjstrLiteralChars_187, 
};

/** Lexer state names. */
static const JAVACC_CHAR_TYPE lexStateNames_arr_0[] = 
{0x44, 0x45, 0x46, 0x41, 0x55, 0x4c, 0x54, 0};
static const JAVACC_STRING_TYPE lexStateNames[] = {
lexStateNames_arr_0, 
};
static const unsigned long long jjtoToken[] = {
   0xfffffffffffff801ULL, 0xffffffffffffffffULL, 0x8081fffffffffffULL, 
};
static const unsigned long long jjtoSkip[] = {
   0x7deULL, 0x0ULL, 0x0ULL, 
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
         jjmatchedKind = 126;
         return jjMoveNfa_0(11, 0);
      case 39:
         jjmatchedKind = 127;
         return jjMoveNfa_0(11, 0);
      case 40:
         jjmatchedKind = 128;
         return jjMoveNfa_0(11, 0);
      case 41:
         jjmatchedKind = 129;
         return jjMoveNfa_0(11, 0);
      case 42:
         jjmatchedKind = 131;
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x0ULL, 0x4ULL);
      case 43:
         jjmatchedKind = 132;
         return jjMoveNfa_0(11, 0);
      case 44:
         jjmatchedKind = 134;
         return jjMoveNfa_0(11, 0);
      case 45:
         jjmatchedKind = 133;
         return jjMoveNfa_0(11, 0);
      case 46:
         jjmatchedKind = 157;
         return jjMoveNfa_0(11, 0);
      case 47:
         jjmatchedKind = 158;
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x0ULL, 0x8000ULL);
      case 58:
         jjmatchedKind = 136;
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x0ULL, 0x80ULL);
      case 59:
         jjmatchedKind = 137;
         return jjMoveNfa_0(11, 0);
      case 60:
         jjmatchedKind = 140;
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x0ULL, 0x60400ULL);
      case 61:
         jjmatchedKind = 142;
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x0ULL, 0x10000ULL);
      case 62:
         jjmatchedKind = 141;
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x0ULL, 0x80800ULL);
      case 63:
         jjmatchedKind = 155;
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x0ULL, 0x7f00000ULL);
      case 64:
         jjmatchedKind = 159;
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
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x1ff000ULL, 0x0ULL);
      case 82:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x1ffe00000ULL, 0x0ULL);
      case 83:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0xffe00000000ULL, 0x0ULL);
      case 84:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0xf00000000000ULL, 0x0ULL);
      case 85:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0xf000000000000ULL, 0x0ULL);
      case 86:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0xf0000000000000ULL, 0x0ULL);
      case 87:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0xf00000000000000ULL, 0x0ULL);
      case 88:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x3000000000000000ULL, 0x0ULL);
      case 91:
         jjmatchedKind = 161;
         return jjMoveNfa_0(11, 0);
      case 93:
         jjmatchedKind = 162;
         return jjMoveNfa_0(11, 0);
      case 94:
         jjmatchedKind = 160;
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
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x1ff000ULL, 0x0ULL);
      case 114:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x1ffe00000ULL, 0x0ULL);
      case 115:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0xffe00000000ULL, 0x0ULL);
      case 116:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0xf00000000000ULL, 0x0ULL);
      case 117:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0xf000000000000ULL, 0x0ULL);
      case 118:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0xf0000000000000ULL, 0x0ULL);
      case 119:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0xf00000000000000ULL, 0x0ULL);
      case 120:
         return jjMoveStringLiteralDfa1_0(0x0ULL, 0x3000000000000000ULL, 0x0ULL);
      case 123:
         jjmatchedKind = 163;
         return jjMoveNfa_0(11, 0);
      case 124:
         jjmatchedKind = 156;
         return jjMoveNfa_0(11, 0);
      case 125:
         jjmatchedKind = 164;
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
         if ((active2 & 0x4ULL) != 0L)
         {
            jjmatchedKind = 130;
            jjmatchedPos = 1;
         }
         break;
      case 47:
         return jjMoveStringLiteralDfa2_0(active0, 0L, active1, 0L, active2, 0x4000000ULL);
      case 60:
         if ((active2 & 0x40000ULL) != 0L)
         {
            jjmatchedKind = 146;
            jjmatchedPos = 1;
         }
         else if ((active2 & 0x1000000ULL) != 0L)
         {
            jjmatchedKind = 152;
            jjmatchedPos = 1;
         }
         return jjMoveStringLiteralDfa2_0(active0, 0L, active1, 0L, active2, 0x400000ULL);
      case 61:
         if ((active2 & 0x80ULL) != 0L)
         {
            jjmatchedKind = 135;
            jjmatchedPos = 1;
         }
         else if ((active2 & 0x400ULL) != 0L)
         {
            jjmatchedKind = 138;
            jjmatchedPos = 1;
         }
         else if ((active2 & 0x800ULL) != 0L)
         {
            jjmatchedKind = 139;
            jjmatchedPos = 1;
         }
         else if ((active2 & 0x8000ULL) != 0L)
         {
            jjmatchedKind = 143;
            jjmatchedPos = 1;
         }
         else if ((active2 & 0x2000000ULL) != 0L)
         {
            jjmatchedKind = 153;
            jjmatchedPos = 1;
         }
         break;
      case 62:
         if ((active2 & 0x10000ULL) != 0L)
         {
            jjmatchedKind = 144;
            jjmatchedPos = 1;
         }
         else if ((active2 & 0x20000ULL) != 0L)
         {
            jjmatchedKind = 145;
            jjmatchedPos = 1;
         }
         else if ((active2 & 0x80000ULL) != 0L)
         {
            jjmatchedKind = 147;
            jjmatchedPos = 1;
         }
         else if ((active2 & 0x800000ULL) != 0L)
         {
            jjmatchedKind = 151;
            jjmatchedPos = 1;
         }
         return jjMoveStringLiteralDfa2_0(active0, 0L, active1, 0L, active2, 0x200000ULL);
      case 63:
         if ((active2 & 0x100000ULL) != 0L)
         {
            jjmatchedKind = 148;
            jjmatchedPos = 1;
         }
         break;
      case 65:
         return jjMoveStringLiteralDfa2_0(active0, 0x4200040020000000ULL, active1, 0x110000000203001ULL, active2, 0L);
      case 66:
         return jjMoveStringLiteralDfa2_0(active0, 0x800ULL, active1, 0L, active2, 0L);
      case 67:
         return jjMoveStringLiteralDfa2_0(active0, 0x1000ULL, active1, 0L, active2, 0L);
      case 69:
         return jjMoveStringLiteralDfa2_0(active0, 0x1800400800000ULL, active1, 0xf3fc00006ULL, active2, 0L);
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
         return jjMoveStringLiteralDfa2_0(active0, 0L, active1, 0x600102000000000ULL, active2, 0L);
      case 73:
         return jjMoveStringLiteralDfa2_0(active0, 0x1c00080800000000ULL, active1, 0x800001000000000ULL, active2, 0L);
      case 76:
         return jjMoveStringLiteralDfa2_0(active0, 0x600100c000ULL, active1, 0xc000000000ULL, active2, 0L);
      case 77:
         return jjMoveStringLiteralDfa2_0(active0, 0x10000000000000ULL, active1, 0x20000000000000ULL, active2, 0L);
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
         return jjMoveStringLiteralDfa2_0(active0, 0xc0018000010000ULL, active1, 0x2007000000000000ULL, active2, 0L);
      case 79:
         if ((active1 & 0x200000000000ULL) != 0L)
         {
            jjmatchedKind = 109;
            jjmatchedPos = 1;
         }
         return jjMoveStringLiteralDfa2_0(active0, 0xa0003013d2000000ULL, active1, 0x10000000c000c018ULL, active2, 0L);
      case 80:
         return jjMoveStringLiteralDfa2_0(active0, 0L, active1, 0x40000000000100ULL, active2, 0L);
      case 82:
         if ((active1 & 0x200ULL) != 0L)
         {
            jjmatchedKind = 73;
            jjmatchedPos = 1;
         }
         return jjMoveStringLiteralDfa2_0(active0, 0x2000000060000ULL, active1, 0x4300000f0000ULL, active2, 0L);
      case 83:
         if ((active0 & 0x100000000000000ULL) != 0L)
         {
            jjmatchedKind = 56;
            jjmatchedPos = 1;
         }
         return jjMoveStringLiteralDfa2_0(active0, 0x380000ULL, active1, 0x8000000000000ULL, active2, 0L);
      case 84:
         return jjMoveStringLiteralDfa2_0(active0, 0x400000ULL, active1, 0x40000000400ULL, active2, 0L);
      case 85:
         return jjMoveStringLiteralDfa2_0(active0, 0x440000c000000ULL, active1, 0x80080000100820ULL, active2, 0L);
      case 88:
         return jjMoveStringLiteralDfa2_0(active0, 0x20000000000ULL, active1, 0L, active2, 0L);
      case 89:
         return jjMoveStringLiteralDfa2_0(active0, 0L, active1, 0x800000000000ULL, active2, 0L);
      case 97:
         return jjMoveStringLiteralDfa2_0(active0, 0x4200040020000000ULL, active1, 0x110000000203001ULL, active2, 0L);
      case 98:
         return jjMoveStringLiteralDfa2_0(active0, 0x800ULL, active1, 0L, active2, 0L);
      case 99:
         return jjMoveStringLiteralDfa2_0(active0, 0x1000ULL, active1, 0L, active2, 0L);
      case 101:
         return jjMoveStringLiteralDfa2_0(active0, 0x1800400800000ULL, active1, 0xf3fc00006ULL, active2, 0L);
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
         return jjMoveStringLiteralDfa2_0(active0, 0L, active1, 0x600102000000000ULL, active2, 0L);
      case 105:
         return jjMoveStringLiteralDfa2_0(active0, 0x1c00080800000000ULL, active1, 0x800001000000000ULL, active2, 0L);
      case 108:
         return jjMoveStringLiteralDfa2_0(active0, 0x600100c000ULL, active1, 0xc000000000ULL, active2, 0L);
      case 109:
         return jjMoveStringLiteralDfa2_0(active0, 0x10000000000000ULL, active1, 0x20000000000000ULL, active2, 0L);
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
         return jjMoveStringLiteralDfa2_0(active0, 0xc0018000010000ULL, active1, 0x2007000000000000ULL, active2, 0L);
      case 111:
         if ((active1 & 0x200000000000ULL) != 0L)
         {
            jjmatchedKind = 109;
            jjmatchedPos = 1;
         }
         return jjMoveStringLiteralDfa2_0(active0, 0xa0003013d2000000ULL, active1, 0x10000000c000c018ULL, active2, 0L);
      case 112:
         return jjMoveStringLiteralDfa2_0(active0, 0L, active1, 0x40000000000100ULL, active2, 0L);
      case 114:
         if ((active1 & 0x200ULL) != 0L)
         {
            jjmatchedKind = 73;
            jjmatchedPos = 1;
         }
         return jjMoveStringLiteralDfa2_0(active0, 0x2000000060000ULL, active1, 0x4300000f0000ULL, active2, 0L);
      case 115:
         if ((active0 & 0x100000000000000ULL) != 0L)
         {
            jjmatchedKind = 56;
            jjmatchedPos = 1;
         }
         return jjMoveStringLiteralDfa2_0(active0, 0x380000ULL, active1, 0x8000000000000ULL, active2, 0L);
      case 116:
         return jjMoveStringLiteralDfa2_0(active0, 0x400000ULL, active1, 0x40000000400ULL, active2, 0L);
      case 117:
         return jjMoveStringLiteralDfa2_0(active0, 0x440000c000000ULL, active1, 0x80080000100820ULL, active2, 0L);
      case 120:
         return jjMoveStringLiteralDfa2_0(active0, 0x20000000000ULL, active1, 0L, active2, 0L);
      case 121:
         return jjMoveStringLiteralDfa2_0(active0, 0L, active1, 0x800000000000ULL, active2, 0L);
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
         if ((active2 & 0x200000ULL) != 0L)
         {
            jjmatchedKind = 149;
            jjmatchedPos = 2;
         }
         else if ((active2 & 0x400000ULL) != 0L)
         {
            jjmatchedKind = 150;
            jjmatchedPos = 2;
         }
         else if ((active2 & 0x4000000ULL) != 0L)
         {
            jjmatchedKind = 154;
            jjmatchedPos = 2;
         }
         break;
      case 65:
         if ((active1 & 0x4000000000ULL) != 0L)
         {
            jjmatchedKind = 102;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x10000000000ULL) != 0L)
         {
            jjmatchedKind = 104;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x4000000000000ULL, active1, 0x1402000000000ULL, active2, 0L);
      case 66:
         return jjMoveStringLiteralDfa3_0(active0, 0x600000000000000ULL, active1, 0x80000000000ULL, active2, 0L);
      case 67:
         return jjMoveStringLiteralDfa3_0(active0, 0x21000ULL, active1, 0x401000ULL, active2, 0L);
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
         if ((active1 & 0x8000000000000ULL) != 0L)
         {
            jjmatchedKind = 115;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x40000000000000ULL, active1, 0x200100000000100ULL, active2, 0L);
      case 70:
         return jjMoveStringLiteralDfa3_0(active0, 0x404000000ULL, active1, 0L, active2, 0L);
      case 71:
         return jjMoveStringLiteralDfa3_0(active0, 0x800000ULL, active1, 0x1000800000ULL, active2, 0L);
      case 72:
         return jjMoveStringLiteralDfa3_0(active0, 0L, active1, 0x400ULL, active2, 0L);
      case 73:
         return jjMoveStringLiteralDfa3_0(active0, 0x60000004000ULL, active1, 0x502000000000000ULL, active2, 0L);
      case 74:
         return jjMoveStringLiteralDfa3_0(active0, 0L, active1, 0x1000000ULL, active2, 0L);
      case 76:
         if ((active0 & 0x8000ULL) != 0L)
         {
            jjmatchedKind = 15;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x40000000ULL) != 0L)
         {
            jjmatchedKind = 94;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x8000000000ULL) != 0L)
         {
            jjmatchedKind = 103;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x20000000000ULL) != 0L)
         {
            jjmatchedKind = 105;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x80000000000ULL, active1, 0x202000020ULL, active2, 0L);
      case 77:
         if ((active1 & 0x10000000ULL) != 0L)
         {
            jjmatchedKind = 92;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x10000000ULL, active1, 0L, active2, 0L);
      case 78:
         return jjMoveStringLiteralDfa3_0(active0, 0x801c001c0000000ULL, active1, 0x80000000200001ULL, active2, 0L);
      case 79:
         return jjMoveStringLiteralDfa3_0(active0, 0x2082000001000000ULL, active1, 0x20200000000f0000ULL, active2, 0L);
      case 80:
         if ((active0 & 0x4000000000000000ULL) != 0L)
         {
            jjmatchedKind = 62;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x10000000000000ULL, active1, 0x800020000000ULL, active2, 0L);
      case 81:
         return jjMoveStringLiteralDfa3_0(active0, 0L, active1, 0x400000000ULL, active2, 0L);
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
         else if ((active1 & 0x80000000ULL) != 0L)
         {
            jjmatchedKind = 95;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x1000000000000000ULL) != 0L)
         {
            jjmatchedKind = 124;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x200000040000ULL, active1, 0x50040000106000ULL, active2, 0L);
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
         return jjMoveStringLiteralDfa3_0(active0, 0x6820380000ULL, active1, 0xc008000ULL, active2, 0L);
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
         return jjMoveStringLiteralDfa3_0(active0, 0x1000010000402000ULL, active1, 0x804000100000000ULL, active2, 0L);
      case 86:
         return jjMoveStringLiteralDfa3_0(active0, 0x200000000ULL, active1, 0x800000000ULL, active2, 0L);
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
         if ((active1 & 0x4000000000ULL) != 0L)
         {
            jjmatchedKind = 102;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x10000000000ULL) != 0L)
         {
            jjmatchedKind = 104;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x4000000000000ULL, active1, 0x1402000000000ULL, active2, 0L);
      case 98:
         return jjMoveStringLiteralDfa3_0(active0, 0x600000000000000ULL, active1, 0x80000000000ULL, active2, 0L);
      case 99:
         return jjMoveStringLiteralDfa3_0(active0, 0x21000ULL, active1, 0x401000ULL, active2, 0L);
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
         if ((active1 & 0x8000000000000ULL) != 0L)
         {
            jjmatchedKind = 115;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x40000000000000ULL, active1, 0x200100000000100ULL, active2, 0L);
      case 102:
         return jjMoveStringLiteralDfa3_0(active0, 0x404000000ULL, active1, 0L, active2, 0L);
      case 103:
         return jjMoveStringLiteralDfa3_0(active0, 0x800000ULL, active1, 0x1000800000ULL, active2, 0L);
      case 104:
         return jjMoveStringLiteralDfa3_0(active0, 0L, active1, 0x400ULL, active2, 0L);
      case 105:
         return jjMoveStringLiteralDfa3_0(active0, 0x60000004000ULL, active1, 0x502000000000000ULL, active2, 0L);
      case 106:
         return jjMoveStringLiteralDfa3_0(active0, 0L, active1, 0x1000000ULL, active2, 0L);
      case 108:
         if ((active0 & 0x8000ULL) != 0L)
         {
            jjmatchedKind = 15;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x40000000ULL) != 0L)
         {
            jjmatchedKind = 94;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x8000000000ULL) != 0L)
         {
            jjmatchedKind = 103;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x20000000000ULL) != 0L)
         {
            jjmatchedKind = 105;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x80000000000ULL, active1, 0x202000020ULL, active2, 0L);
      case 109:
         if ((active1 & 0x10000000ULL) != 0L)
         {
            jjmatchedKind = 92;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x10000000ULL, active1, 0L, active2, 0L);
      case 110:
         return jjMoveStringLiteralDfa3_0(active0, 0x801c001c0000000ULL, active1, 0x80000000200001ULL, active2, 0L);
      case 111:
         return jjMoveStringLiteralDfa3_0(active0, 0x2082000001000000ULL, active1, 0x20200000000f0000ULL, active2, 0L);
      case 112:
         if ((active0 & 0x4000000000000000ULL) != 0L)
         {
            jjmatchedKind = 62;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x10000000000000ULL, active1, 0x800020000000ULL, active2, 0L);
      case 113:
         return jjMoveStringLiteralDfa3_0(active0, 0L, active1, 0x400000000ULL, active2, 0L);
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
         else if ((active1 & 0x80000000ULL) != 0L)
         {
            jjmatchedKind = 95;
            jjmatchedPos = 2;
         }
         else if ((active1 & 0x1000000000000000ULL) != 0L)
         {
            jjmatchedKind = 124;
            jjmatchedPos = 2;
         }
         return jjMoveStringLiteralDfa3_0(active0, 0x200000040000ULL, active1, 0x50040000106000ULL, active2, 0L);
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
         return jjMoveStringLiteralDfa3_0(active0, 0x6820380000ULL, active1, 0xc008000ULL, active2, 0L);
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
         return jjMoveStringLiteralDfa3_0(active0, 0x1000010000402000ULL, active1, 0x804000100000000ULL, active2, 0L);
      case 118:
         return jjMoveStringLiteralDfa3_0(active0, 0x200000000ULL, active1, 0x800000000ULL, active2, 0L);
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
         return jjMoveStringLiteralDfa4_0(active0, 0x600801000000ULL, active1, 0x30000ULL);
      case 68:
         if ((active1 & 0x1ULL) != 0L)
         {
            jjmatchedKind = 64;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0L, active1, 0x20000000000000ULL);
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
         else if ((active1 & 0x100000ULL) != 0L)
         {
            jjmatchedKind = 84;
            jjmatchedPos = 3;
         }
         else if ((active1 & 0x800000000000ULL) != 0L)
         {
            jjmatchedKind = 111;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x1201800200083000ULL, active1, 0xa03000400ULL);
      case 70:
         return jjMoveStringLiteralDfa4_0(active0, 0x44000000ULL, active1, 0x1000000000000ULL);
      case 71:
         return jjMoveStringLiteralDfa4_0(active0, 0L, active1, 0x200000ULL);
      case 72:
         if ((active1 & 0x800000000000000ULL) != 0L)
         {
            jjmatchedKind = 123;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x20000ULL, active1, 0L);
      case 73:
         return jjMoveStringLiteralDfa4_0(active0, 0x14000800000ULL, active1, 0x94000000800000ULL);
      case 75:
         return jjMoveStringLiteralDfa4_0(active0, 0x800000000000000ULL, active1, 0x1000ULL);
      case 76:
         if ((active1 & 0x20ULL) != 0L)
         {
            jjmatchedKind = 69;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0L, active1, 0x400000000000000ULL);
      case 78:
         if ((active1 & 0x100ULL) != 0L)
         {
            jjmatchedKind = 72;
            jjmatchedPos = 3;
         }
         else if ((active1 & 0x100000000000ULL) != 0L)
         {
            jjmatchedKind = 108;
            jjmatchedPos = 3;
         }
         else if ((active1 & 0x200000000000000ULL) != 0L)
         {
            jjmatchedKind = 121;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x1000000000ULL, active1, 0x401000000000ULL);
      case 79:
         return jjMoveStringLiteralDfa4_0(active0, 0L, active1, 0x40040020400000ULL);
      case 80:
         if ((active0 & 0x2000000000000000ULL) != 0L)
         {
            jjmatchedKind = 61;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x10000000ULL, active1, 0x40000ULL);
      case 82:
         if ((active1 & 0x2000000000000000ULL) != 0L)
         {
            jjmatchedKind = 125;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x444040000400000ULL, active1, 0x2000000000ULL);
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
         else if ((active1 & 0x100000000000000ULL) != 0L)
         {
            jjmatchedKind = 120;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x100000000ULL, active1, 0x208000c088000ULL);
      case 85:
         return jjMoveStringLiteralDfa4_0(active0, 0x92000000300000ULL, active1, 0x500000000ULL);
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
         return jjMoveStringLiteralDfa4_0(active0, 0x600801000000ULL, active1, 0x30000ULL);
      case 100:
         if ((active1 & 0x1ULL) != 0L)
         {
            jjmatchedKind = 64;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0L, active1, 0x20000000000000ULL);
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
         else if ((active1 & 0x100000ULL) != 0L)
         {
            jjmatchedKind = 84;
            jjmatchedPos = 3;
         }
         else if ((active1 & 0x800000000000ULL) != 0L)
         {
            jjmatchedKind = 111;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x1201800200083000ULL, active1, 0xa03000400ULL);
      case 102:
         return jjMoveStringLiteralDfa4_0(active0, 0x44000000ULL, active1, 0x1000000000000ULL);
      case 103:
         return jjMoveStringLiteralDfa4_0(active0, 0L, active1, 0x200000ULL);
      case 104:
         if ((active1 & 0x800000000000000ULL) != 0L)
         {
            jjmatchedKind = 123;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x20000ULL, active1, 0L);
      case 105:
         return jjMoveStringLiteralDfa4_0(active0, 0x14000800000ULL, active1, 0x94000000800000ULL);
      case 107:
         return jjMoveStringLiteralDfa4_0(active0, 0x800000000000000ULL, active1, 0x1000ULL);
      case 108:
         if ((active1 & 0x20ULL) != 0L)
         {
            jjmatchedKind = 69;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0L, active1, 0x400000000000000ULL);
      case 110:
         if ((active1 & 0x100ULL) != 0L)
         {
            jjmatchedKind = 72;
            jjmatchedPos = 3;
         }
         else if ((active1 & 0x100000000000ULL) != 0L)
         {
            jjmatchedKind = 108;
            jjmatchedPos = 3;
         }
         else if ((active1 & 0x200000000000000ULL) != 0L)
         {
            jjmatchedKind = 121;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x1000000000ULL, active1, 0x401000000000ULL);
      case 111:
         return jjMoveStringLiteralDfa4_0(active0, 0L, active1, 0x40040020400000ULL);
      case 112:
         if ((active0 & 0x2000000000000000ULL) != 0L)
         {
            jjmatchedKind = 61;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x10000000ULL, active1, 0x40000ULL);
      case 114:
         if ((active1 & 0x2000000000000000ULL) != 0L)
         {
            jjmatchedKind = 125;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x444040000400000ULL, active1, 0x2000000000ULL);
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
         else if ((active1 & 0x100000000000000ULL) != 0L)
         {
            jjmatchedKind = 120;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x100000000ULL, active1, 0x208000c088000ULL);
      case 117:
         return jjMoveStringLiteralDfa4_0(active0, 0x92000000300000ULL, active1, 0x500000000ULL);
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
         return jjMoveStringLiteralDfa5_0(active0, 0xc00000000000000ULL, active1, 0x10001002001000ULL);
      case 67:
         return jjMoveStringLiteralDfa5_0(active0, 0L, active1, 0x201000000ULL);
      case 68:
         return jjMoveStringLiteralDfa5_0(active0, 0x4000000000000ULL, active1, 0L);
      case 69:
         if ((active0 & 0x200000000000ULL) != 0L)
         {
            jjmatchedKind = 45;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x200000ULL) != 0L)
         {
            jjmatchedKind = 85;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x20000000000000ULL) != 0L)
         {
            jjmatchedKind = 117;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x400000000000000ULL) != 0L)
         {
            jjmatchedKind = 122;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0x104000000ULL, active1, 0x24000f0000ULL);
      case 70:
         if ((active0 & 0x4000000000ULL) != 0L)
         {
            jjmatchedKind = 38;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0L, active1, 0x1000000000000ULL);
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
         else if ((active1 & 0x4000000000000ULL) != 0L)
         {
            jjmatchedKind = 114;
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
         return jjMoveStringLiteralDfa5_0(active0, 0x40000000000ULL, active1, 0x40000000000ULL);
      case 79:
         return jjMoveStringLiteralDfa5_0(active0, 0x810000000ULL, active1, 0L);
      case 80:
         if ((active0 & 0x2000000000000ULL) != 0L)
         {
            jjmatchedKind = 49;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x40000000000000ULL) != 0L)
         {
            jjmatchedKind = 118;
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
         return jjMoveStringLiteralDfa5_0(active0, 0x1011800000080000ULL, active1, 0x92c400400ULL);
      case 83:
         if ((active0 & 0x4000ULL) != 0L)
         {
            jjmatchedKind = 14;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x2000000000000ULL) != 0L)
         {
            jjmatchedKind = 113;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0x1000ULL, active1, 0x400000800000ULL);
      case 84:
         if ((active0 & 0x80000000000000ULL) != 0L)
         {
            jjmatchedKind = 55;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x80000000000000ULL) != 0L)
         {
            jjmatchedKind = 119;
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
         return jjMoveStringLiteralDfa5_0(active0, 0L, active1, 0x80000000000ULL);
      case 97:
         return jjMoveStringLiteralDfa5_0(active0, 0xc00000000000000ULL, active1, 0x10001002001000ULL);
      case 99:
         return jjMoveStringLiteralDfa5_0(active0, 0L, active1, 0x201000000ULL);
      case 100:
         return jjMoveStringLiteralDfa5_0(active0, 0x4000000000000ULL, active1, 0L);
      case 101:
         if ((active0 & 0x200000000000ULL) != 0L)
         {
            jjmatchedKind = 45;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x200000ULL) != 0L)
         {
            jjmatchedKind = 85;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x20000000000000ULL) != 0L)
         {
            jjmatchedKind = 117;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x400000000000000ULL) != 0L)
         {
            jjmatchedKind = 122;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0x104000000ULL, active1, 0x24000f0000ULL);
      case 102:
         if ((active0 & 0x4000000000ULL) != 0L)
         {
            jjmatchedKind = 38;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0L, active1, 0x1000000000000ULL);
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
         else if ((active1 & 0x4000000000000ULL) != 0L)
         {
            jjmatchedKind = 114;
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
         return jjMoveStringLiteralDfa5_0(active0, 0x40000000000ULL, active1, 0x40000000000ULL);
      case 111:
         return jjMoveStringLiteralDfa5_0(active0, 0x810000000ULL, active1, 0L);
      case 112:
         if ((active0 & 0x2000000000000ULL) != 0L)
         {
            jjmatchedKind = 49;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x40000000000000ULL) != 0L)
         {
            jjmatchedKind = 118;
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
         return jjMoveStringLiteralDfa5_0(active0, 0x1011800000080000ULL, active1, 0x92c400400ULL);
      case 115:
         if ((active0 & 0x4000ULL) != 0L)
         {
            jjmatchedKind = 14;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x2000000000000ULL) != 0L)
         {
            jjmatchedKind = 113;
            jjmatchedPos = 4;
         }
         return jjMoveStringLiteralDfa5_0(active0, 0x1000ULL, active1, 0x400000800000ULL);
      case 116:
         if ((active0 & 0x80000000000000ULL) != 0L)
         {
            jjmatchedKind = 55;
            jjmatchedPos = 4;
         }
         else if ((active1 & 0x80000000000000ULL) != 0L)
         {
            jjmatchedKind = 119;
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
         return jjMoveStringLiteralDfa5_0(active0, 0L, active1, 0x80000000000ULL);
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
         return jjMoveStringLiteralDfa6_0(active0, 0x400000ULL, active1, 0x10000000000000ULL);
      case 67:
         return jjMoveStringLiteralDfa6_0(active0, 0L, active1, 0x80000ULL);
      case 68:
         if ((active1 & 0x400000ULL) != 0L)
         {
            jjmatchedKind = 86;
            jjmatchedPos = 5;
         }
         else if ((active1 & 0x2000000000ULL) != 0L)
         {
            jjmatchedKind = 101;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0L, active1, 0x10000ULL);
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
         return jjMoveStringLiteralDfa6_0(active0, 0x4040000200000ULL, active1, 0x1000000002000ULL);
      case 71:
         if ((active1 & 0x40000000000ULL) != 0L)
         {
            jjmatchedKind = 106;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0x800000040000000ULL, active1, 0x1000ULL);
      case 73:
         return jjMoveStringLiteralDfa6_0(active0, 0x41400000000000ULL, active1, 0x80c000000ULL);
      case 76:
         if ((active1 & 0x1000000000ULL) != 0L)
         {
            jjmatchedKind = 100;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0x400000000ULL, active1, 0L);
      case 78:
         if ((active1 & 0x100000000ULL) != 0L)
         {
            jjmatchedKind = 96;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0x810000000ULL, active1, 0x400000000ULL);
      case 79:
         if ((active0 & 0x1000000000ULL) != 0L)
         {
            jjmatchedKind = 36;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0L, active1, 0x8000ULL);
      case 80:
         return jjMoveStringLiteralDfa6_0(active0, 0L, active1, 0x480000000000ULL);
      case 82:
         if ((active0 & 0x4000000ULL) != 0L)
         {
            jjmatchedKind = 26;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0x400000000000000ULL, active1, 0x40000ULL);
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
         return jjMoveStringLiteralDfa6_0(active0, 0L, active1, 0x2020000ULL);
      case 84:
         if ((active0 & 0x80000ULL) != 0L)
         {
            jjmatchedKind = 19;
            jjmatchedPos = 5;
         }
         else if ((active1 & 0x1000000ULL) != 0L)
         {
            jjmatchedKind = 88;
            jjmatchedPos = 5;
         }
         else if ((active1 & 0x20000000ULL) != 0L)
         {
            jjmatchedKind = 93;
            jjmatchedPos = 5;
         }
         else if ((active1 & 0x200000000ULL) != 0L)
         {
            jjmatchedKind = 97;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0x20000ULL, active1, 0x800000ULL);
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
         return jjMoveStringLiteralDfa6_0(active0, 0x400000ULL, active1, 0x10000000000000ULL);
      case 99:
         return jjMoveStringLiteralDfa6_0(active0, 0L, active1, 0x80000ULL);
      case 100:
         if ((active1 & 0x400000ULL) != 0L)
         {
            jjmatchedKind = 86;
            jjmatchedPos = 5;
         }
         else if ((active1 & 0x2000000000ULL) != 0L)
         {
            jjmatchedKind = 101;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0L, active1, 0x10000ULL);
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
         return jjMoveStringLiteralDfa6_0(active0, 0x4040000200000ULL, active1, 0x1000000002000ULL);
      case 103:
         if ((active1 & 0x40000000000ULL) != 0L)
         {
            jjmatchedKind = 106;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0x800000040000000ULL, active1, 0x1000ULL);
      case 105:
         return jjMoveStringLiteralDfa6_0(active0, 0x41400000000000ULL, active1, 0x80c000000ULL);
      case 108:
         if ((active1 & 0x1000000000ULL) != 0L)
         {
            jjmatchedKind = 100;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0x400000000ULL, active1, 0L);
      case 110:
         if ((active1 & 0x100000000ULL) != 0L)
         {
            jjmatchedKind = 96;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0x810000000ULL, active1, 0x400000000ULL);
      case 111:
         if ((active0 & 0x1000000000ULL) != 0L)
         {
            jjmatchedKind = 36;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0L, active1, 0x8000ULL);
      case 112:
         return jjMoveStringLiteralDfa6_0(active0, 0L, active1, 0x480000000000ULL);
      case 114:
         if ((active0 & 0x4000000ULL) != 0L)
         {
            jjmatchedKind = 26;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0x400000000000000ULL, active1, 0x40000ULL);
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
         return jjMoveStringLiteralDfa6_0(active0, 0L, active1, 0x2020000ULL);
      case 116:
         if ((active0 & 0x80000ULL) != 0L)
         {
            jjmatchedKind = 19;
            jjmatchedPos = 5;
         }
         else if ((active1 & 0x1000000ULL) != 0L)
         {
            jjmatchedKind = 88;
            jjmatchedPos = 5;
         }
         else if ((active1 & 0x20000000ULL) != 0L)
         {
            jjmatchedKind = 93;
            jjmatchedPos = 5;
         }
         else if ((active1 & 0x200000000ULL) != 0L)
         {
            jjmatchedKind = 97;
            jjmatchedPos = 5;
         }
         return jjMoveStringLiteralDfa6_0(active0, 0x20000ULL, active1, 0x800000ULL);
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
         return jjMoveStringLiteralDfa7_0(active0, 0L, active1, 0x100040c000000ULL);
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
         else if ((active1 & 0x2000000ULL) != 0L)
         {
            jjmatchedKind = 89;
            jjmatchedPos = 6;
         }
         else if ((active1 & 0x80000000000ULL) != 0L)
         {
            jjmatchedKind = 107;
            jjmatchedPos = 6;
         }
         return jjMoveStringLiteralDfa7_0(active0, 0x10020000ULL, active1, 0x800000ULL);
      case 76:
         if ((active0 & 0x1000000000000000ULL) != 0L)
         {
            jjmatchedKind = 60;
            jjmatchedPos = 6;
         }
         return jjMoveStringLiteralDfa7_0(active0, 0L, active1, 0x10000000000000ULL);
      case 78:
         return jjMoveStringLiteralDfa7_0(active0, 0x880000000ULL, active1, 0x8000ULL);
      case 79:
         return jjMoveStringLiteralDfa7_0(active0, 0x400000000000ULL, active1, 0x400000000000ULL);
      case 83:
         if ((active1 & 0x20000ULL) != 0L)
         {
            jjmatchedKind = 81;
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
         return jjMoveStringLiteralDfa7_0(active0, 0x800000000000ULL, active1, 0x8000c2000ULL);
      case 85:
         return jjMoveStringLiteralDfa7_0(active0, 0x40400000ULL, active1, 0x10000ULL);
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
         return jjMoveStringLiteralDfa7_0(active0, 0L, active1, 0x100040c000000ULL);
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
         else if ((active1 & 0x2000000ULL) != 0L)
         {
            jjmatchedKind = 89;
            jjmatchedPos = 6;
         }
         else if ((active1 & 0x80000000000ULL) != 0L)
         {
            jjmatchedKind = 107;
            jjmatchedPos = 6;
         }
         return jjMoveStringLiteralDfa7_0(active0, 0x10020000ULL, active1, 0x800000ULL);
      case 108:
         if ((active0 & 0x1000000000000000ULL) != 0L)
         {
            jjmatchedKind = 60;
            jjmatchedPos = 6;
         }
         return jjMoveStringLiteralDfa7_0(active0, 0L, active1, 0x10000000000000ULL);
      case 110:
         return jjMoveStringLiteralDfa7_0(active0, 0x880000000ULL, active1, 0x8000ULL);
      case 111:
         return jjMoveStringLiteralDfa7_0(active0, 0x400000000000ULL, active1, 0x400000000000ULL);
      case 115:
         if ((active1 & 0x20000ULL) != 0L)
         {
            jjmatchedKind = 81;
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
         return jjMoveStringLiteralDfa7_0(active0, 0x800000000000ULL, active1, 0x8000c2000ULL);
      case 117:
         return jjMoveStringLiteralDfa7_0(active0, 0x40400000ULL, active1, 0x10000ULL);
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
         else if ((active1 & 0x400000000ULL) != 0L)
         {
            jjmatchedKind = 98;
            jjmatchedPos = 7;
         }
         else if ((active1 & 0x10000000000000ULL) != 0L)
         {
            jjmatchedKind = 116;
            jjmatchedPos = 7;
         }
         return jjMoveStringLiteralDfa8_0(active0, 0x800000000ULL, active1, 0x8a000ULL);
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
         if ((active1 & 0x800000ULL) != 0L)
         {
            jjmatchedKind = 87;
            jjmatchedPos = 7;
         }
         return jjMoveStringLiteralDfa8_0(active0, 0x40000000ULL, active1, 0x400000010000ULL);
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
         else if ((active1 & 0x4000000ULL) != 0L)
         {
            jjmatchedKind = 90;
            jjmatchedPos = 7;
         }
         return jjMoveStringLiteralDfa8_0(active0, 0x400000ULL, active1, 0x1000008000000ULL);
      case 89:
         if ((active1 & 0x40000ULL) != 0L)
         {
            jjmatchedKind = 82;
            jjmatchedPos = 7;
         }
         else if ((active1 & 0x800000000ULL) != 0L)
         {
            jjmatchedKind = 99;
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
         else if ((active1 & 0x400000000ULL) != 0L)
         {
            jjmatchedKind = 98;
            jjmatchedPos = 7;
         }
         else if ((active1 & 0x10000000000000ULL) != 0L)
         {
            jjmatchedKind = 116;
            jjmatchedPos = 7;
         }
         return jjMoveStringLiteralDfa8_0(active0, 0x800000000ULL, active1, 0x8a000ULL);
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
         if ((active1 & 0x800000ULL) != 0L)
         {
            jjmatchedKind = 87;
            jjmatchedPos = 7;
         }
         return jjMoveStringLiteralDfa8_0(active0, 0x40000000ULL, active1, 0x400000010000ULL);
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
         else if ((active1 & 0x4000000ULL) != 0L)
         {
            jjmatchedKind = 90;
            jjmatchedPos = 7;
         }
         return jjMoveStringLiteralDfa8_0(active0, 0x400000ULL, active1, 0x1000008000000ULL);
      case 121:
         if ((active1 & 0x40000ULL) != 0L)
         {
            jjmatchedKind = 82;
            jjmatchedPos = 7;
         }
         else if ((active1 & 0x800000000ULL) != 0L)
         {
            jjmatchedKind = 99;
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
         else if ((active1 & 0x80000ULL) != 0L)
         {
            jjmatchedKind = 83;
            jjmatchedPos = 8;
         }
         break;
      case 69:
         if ((active0 & 0x400000ULL) != 0L)
         {
            jjmatchedKind = 22;
            jjmatchedPos = 8;
         }
         else if ((active1 & 0x10000ULL) != 0L)
         {
            jjmatchedKind = 80;
            jjmatchedPos = 8;
         }
         return jjMoveStringLiteralDfa9_0(active0, 0L, active1, 0x1000000000000ULL);
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
         else if ((active1 & 0x400000000000ULL) != 0L)
         {
            jjmatchedKind = 110;
            jjmatchedPos = 8;
         }
         return jjMoveStringLiteralDfa9_0(active0, 0x20000ULL, active1, 0L);
      case 85:
         return jjMoveStringLiteralDfa9_0(active0, 0x200000ULL, active1, 0L);
      case 95:
         return jjMoveStringLiteralDfa9_0(active0, 0L, active1, 0x8000000ULL);
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
         else if ((active1 & 0x80000ULL) != 0L)
         {
            jjmatchedKind = 83;
            jjmatchedPos = 8;
         }
         break;
      case 101:
         if ((active0 & 0x400000ULL) != 0L)
         {
            jjmatchedKind = 22;
            jjmatchedPos = 8;
         }
         else if ((active1 & 0x10000ULL) != 0L)
         {
            jjmatchedKind = 80;
            jjmatchedPos = 8;
         }
         return jjMoveStringLiteralDfa9_0(active0, 0L, active1, 0x1000000000000ULL);
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
         else if ((active1 & 0x400000000000ULL) != 0L)
         {
            jjmatchedKind = 110;
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
         if ((active1 & 0x1000000000000ULL) != 0L)
         {
            jjmatchedKind = 112;
            jjmatchedPos = 9;
         }
         break;
      case 71:
         return jjMoveStringLiteralDfa10_0(active0, 0L, active1, 0x8000000ULL);
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
         if ((active1 & 0x1000000000000ULL) != 0L)
         {
            jjmatchedKind = 112;
            jjmatchedPos = 9;
         }
         break;
      case 103:
         return jjMoveStringLiteralDfa10_0(active0, 0L, active1, 0x8000000ULL);
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
         return jjMoveStringLiteralDfa11_0(active0, 0L, active1, 0x8000000ULL);
      case 105:
         return jjMoveStringLiteralDfa11_0(active0, 0x40000000ULL, active1, 0L);
      case 114:
         return jjMoveStringLiteralDfa11_0(active0, 0x220000ULL, active1, 0L);
      case 117:
         return jjMoveStringLiteralDfa11_0(active0, 0L, active1, 0x8000000ULL);
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
         return jjMoveStringLiteralDfa12_0(active0, 0L, active1, 0x8000000ULL);
      case 69:
         if ((active0 & 0x20000ULL) != 0L)
         {
            jjmatchedKind = 17;
            jjmatchedPos = 11;
         }
         return jjMoveStringLiteralDfa12_0(active0, 0x200000ULL, active1, 0L);
      case 79:
         return jjMoveStringLiteralDfa12_0(active0, 0x40000000ULL, active1, 0L);
      case 97:
         return jjMoveStringLiteralDfa12_0(active0, 0L, active1, 0x8000000ULL);
      case 101:
         if ((active0 & 0x20000ULL) != 0L)
         {
            jjmatchedKind = 17;
            jjmatchedPos = 11;
         }
         return jjMoveStringLiteralDfa12_0(active0, 0x200000ULL, active1, 0L);
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
         return jjMoveStringLiteralDfa13_0(active0, 0L, active1, 0x8000000ULL);
      case 110:
         if ((active0 & 0x40000000ULL) != 0L)
         {
            jjmatchedKind = 30;
            jjmatchedPos = 12;
         }
         return jjMoveStringLiteralDfa13_0(active0, 0x200000ULL, active1, 0L);
      case 114:
         return jjMoveStringLiteralDfa13_0(active0, 0L, active1, 0x8000000ULL);
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
         return jjMoveStringLiteralDfa14_0(active0, 0L, active1, 0x8000000ULL);
      case 84:
         return jjMoveStringLiteralDfa14_0(active0, 0x200000ULL, active1, 0L);
      case 97:
         return jjMoveStringLiteralDfa14_0(active0, 0L, active1, 0x8000000ULL);
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
         return jjMoveStringLiteralDfa15_0(active0, 0L, active1, 0x8000000ULL);
      case 101:
         return jjMoveStringLiteralDfa15_0(active0, 0x200000ULL, active1, 0L);
      case 110:
         return jjMoveStringLiteralDfa15_0(active0, 0L, active1, 0x8000000ULL);
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
         return jjMoveStringLiteralDfa16_0(active0, 0L, active1, 0x8000000ULL);
      case 101:
         if ((active0 & 0x200000ULL) != 0L)
         {
            jjmatchedKind = 21;
            jjmatchedPos = 15;
         }
         break;
      case 116:
         return jjMoveStringLiteralDfa16_0(active0, 0L, active1, 0x8000000ULL);
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
         return jjMoveStringLiteralDfa17_0(active1, 0x8000000ULL);
      case 101:
         return jjMoveStringLiteralDfa17_0(active1, 0x8000000ULL);
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
         if ((active1 & 0x8000000ULL) != 0L)
         {
            jjmatchedKind = 91;
            jjmatchedPos = 17;
         }
         break;
      case 101:
         if ((active1 & 0x8000000ULL) != 0L)
         {
            jjmatchedKind = 91;
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
   jjnewStateCnt = 76;
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
                     if (kind > 165)
                        kind = 165;
                     { jjCheckNAddStates(0, 8); }
                  }
                  else if ((0x100000200ULL & l) != 0L)
                     { jjCheckNAddTwoStates(0, 3); }
                  else if (curChar == 47)
                     { jjAddStates(9, 10); }
                  else if (curChar == 45)
                     { jjCheckNAddStates(11, 13); }
                  else if (curChar == 39)
                     jjstateSet[jjnewStateCnt++] = 18;
                  else if (curChar == 34)
                     { jjCheckNAddTwoStates(9, 10); }
                  if ((0x3ff000000000000ULL & l) != 0L)
                  {
                     if (kind > 171)
                        kind = 171;
                     { jjCheckNAdd(20); }
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
                  { jjCheckNAddStates(14, 18); }
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
                  { jjCheckNAddStates(14, 18); }
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
                  if (kind > 166)
                     kind = 166;
                  jjstateSet[jjnewStateCnt++] = 8;
                  break;
               case 13:
                  if ((0x3ff000000000000ULL & l) == 0L)
                     break;
                  if (kind > 167)
                     kind = 167;
                  { jjAddStates(19, 20); }
                  break;
               case 15:
                  if ((0xfffffffb00000200ULL & l) != 0L)
                     { jjAddStates(21, 22); }
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
                  if (curChar == 39 && kind > 169)
                     kind = 169;
                  break;
               case 20:
                  if ((0x3ff000000000000ULL & l) == 0L)
                     break;
                  if (kind > 171)
                     kind = 171;
                  { jjCheckNAdd(20); }
                  break;
               case 22:
                  if (curChar == 34)
                     { jjCheckNAdd(23); }
                  break;
               case 23:
                  if ((0x3ff000000000000ULL & l) != 0L)
                     { jjCheckNAddStates(23, 25); }
                  break;
               case 25:
                  if (curChar == 34 && kind > 179)
                     kind = 179;
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
                  if (kind > 187)
                     kind = 187;
                  { jjCheckNAddTwoStates(27, 30); }
                  break;
               case 30:
                  if ((0xfffffffb00000200ULL & l) == 0L)
                     break;
                  if (kind > 187)
                     kind = 187;
                  { jjCheckNAddTwoStates(27, 30); }
                  break;
               case 31:
                  if (curChar == 45)
                     { jjCheckNAddStates(11, 13); }
                  break;
               case 32:
                  if (curChar != 35)
                     break;
                  if (kind > 7)
                     kind = 7;
                  { jjCheckNAddStates(26, 28); }
                  break;
               case 33:
                  if ((0xffffffffffffdbffULL & l) == 0L)
                     break;
                  if (kind > 7)
                     kind = 7;
                  { jjCheckNAddStates(26, 28); }
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
                  { jjCheckNAddStates(29, 31); }
                  break;
               case 39:
                  if ((0xffffffffffffdbffULL & l) == 0L)
                     break;
                  if (kind > 8)
                     kind = 8;
                  { jjCheckNAddStates(29, 31); }
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
                     { jjAddStates(9, 10); }
                  break;
               case 44:
                  if (curChar == 33)
                     { jjCheckNAddTwoStates(45, 47); }
                  break;
               case 45:
                  { jjCheckNAddTwoStates(45, 47); }
                  break;
               case 46:
                  if (curChar == 47 && kind > 9)
                     kind = 9;
                  break;
               case 47:
                  if (curChar == 42)
                     jjstateSet[jjnewStateCnt++] = 46;
                  break;
               case 48:
                  if (curChar == 42)
                     jjstateSet[jjnewStateCnt++] = 44;
                  break;
               case 49:
                  if (curChar == 42)
                     { jjCheckNAddTwoStates(50, 52); }
                  break;
               case 50:
                  { jjCheckNAddTwoStates(50, 52); }
                  break;
               case 51:
                  if (curChar == 47 && kind > 10)
                     kind = 10;
                  break;
               case 52:
                  if (curChar == 42)
                     jjstateSet[jjnewStateCnt++] = 51;
                  break;
               case 53:
                  if ((0x3ff000000000000ULL & l) == 0L)
                     break;
                  if (kind > 165)
                     kind = 165;
                  { jjCheckNAddStates(0, 8); }
                  break;
               case 55:
                  if ((0x3ff000000000000ULL & l) == 0L)
                     break;
                  if (kind > 165)
                     kind = 165;
                  { jjCheckNAddTwoStates(54, 55); }
                  break;
               case 57:
                  if ((0x3ff000000000000ULL & l) == 0L)
                     break;
                  if (kind > 170)
                     kind = 170;
                  { jjCheckNAddStates(32, 35); }
                  break;
               case 58:
                  if (curChar == 46)
                     { jjCheckNAdd(59); }
                  break;
               case 59:
                  if ((0x3ff000000000000ULL & l) == 0L)
                     break;
                  if (kind > 170)
                     kind = 170;
                  { jjCheckNAddStates(36, 38); }
                  break;
               case 62:
                  if ((0x280000000000ULL & l) != 0L)
                     { jjCheckNAdd(63); }
                  break;
               case 63:
                  if ((0x3ff000000000000ULL & l) == 0L)
                     break;
                  if (kind > 170)
                     kind = 170;
                  { jjCheckNAddTwoStates(64, 63); }
                  break;
               case 66:
                  if ((0x3ff000000000000ULL & l) != 0L)
                     { jjCheckNAddStates(39, 41); }
                  break;
               case 67:
                  if (curChar == 35)
                     { jjCheckNAdd(68); }
                  break;
               case 68:
                  if ((0x3ff000000000000ULL & l) != 0L)
                     { jjCheckNAddStates(42, 44); }
                  break;
               case 69:
                  if (curChar == 46)
                     { jjCheckNAdd(70); }
                  break;
               case 70:
                  if ((0x3ff000000000000ULL & l) != 0L)
                     { jjCheckNAddTwoStates(70, 71); }
                  break;
               case 71:
                  if (curChar != 35)
                     break;
                  if (kind > 172)
                     kind = 172;
                  jjstateSet[jjnewStateCnt++] = 72;
                  break;
               case 73:
                  if ((0x280000000000ULL & l) != 0L)
                     { jjCheckNAdd(74); }
                  break;
               case 74:
                  if ((0x3ff000000000000ULL & l) == 0L)
                     break;
                  if (kind > 172)
                     kind = 172;
                  { jjCheckNAddTwoStates(75, 74); }
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
                     if (kind > 171)
                        kind = 171;
                     { jjCheckNAdd(20); }
                  }
                  else if (curChar == 96)
                     { jjCheckNAddTwoStates(27, 30); }
                  else if (curChar == 92)
                     { jjCheckNAddTwoStates(15, 16); }
                  if ((0x7fffffe07fffffeULL & l) != 0L)
                  {
                     if (kind > 167)
                        kind = 167;
                     { jjCheckNAddTwoStates(12, 13); }
                  }
                  if ((0x100800401008004ULL & l) != 0L)
                     jjstateSet[jjnewStateCnt++] = 22;
                  break;
               case 4:
                  if (kind > 6)
                     kind = 6;
                  { jjAddStates(14, 18); }
                  break;
               case 9:
                  if ((0x7fffffffffffffffULL & l) != 0L)
                     { jjAddStates(45, 46); }
                  break;
               case 12:
                  if (curChar == 95)
                     { jjCheckNAddTwoStates(12, 13); }
                  break;
               case 13:
                  if ((0x7fffffe07fffffeULL & l) == 0L)
                     break;
                  if (kind > 167)
                     kind = 167;
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
                  if (curChar == 92 && kind > 168)
                     kind = 168;
                  break;
               case 18:
                  if ((0x7fffffffffffffffULL & l) != 0L)
                     jjstateSet[jjnewStateCnt++] = 19;
                  break;
               case 20:
                  if ((0x7fffffe07fffffeULL & l) == 0L)
                     break;
                  if (kind > 171)
                     kind = 171;
                  { jjCheckNAdd(20); }
                  break;
               case 21:
                  if ((0x100800401008004ULL & l) != 0L)
                     jjstateSet[jjnewStateCnt++] = 22;
                  break;
               case 23:
                  if ((0x7fffffe07fffffeULL & l) != 0L)
                     { jjCheckNAddStates(23, 25); }
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
                     { jjAddStates(47, 48); }
                  break;
               case 30:
                  if ((0x7fffffffffffffffULL & l) == 0L)
                     break;
                  if (kind > 187)
                     kind = 187;
                  { jjCheckNAddTwoStates(27, 30); }
                  break;
               case 33:
                  if (kind > 7)
                     kind = 7;
                  { jjAddStates(26, 28); }
                  break;
               case 39:
                  if (kind > 8)
                     kind = 8;
                  { jjAddStates(29, 31); }
                  break;
               case 45:
                  { jjAddStates(49, 50); }
                  break;
               case 50:
                  { jjAddStates(51, 52); }
                  break;
               case 54:
                  if (curChar == 95)
                     jjstateSet[jjnewStateCnt++] = 55;
                  break;
               case 56:
                  if (curChar == 95)
                     jjstateSet[jjnewStateCnt++] = 57;
                  break;
               case 60:
                  if (curChar == 95)
                     jjstateSet[jjnewStateCnt++] = 59;
                  break;
               case 61:
                  if ((0x2000000020ULL & l) != 0L)
                     { jjCheckNAddTwoStates(62, 63); }
                  break;
               case 64:
                  if (curChar == 95)
                     { jjCheckNAdd(63); }
                  break;
               case 65:
                  if (curChar == 95)
                     jjstateSet[jjnewStateCnt++] = 66;
                  break;
               case 68:
                  if ((0x7fffffe07fffffeULL & l) != 0L)
                     { jjCheckNAddStates(42, 44); }
                  break;
               case 70:
                  if ((0x7fffffe07fffffeULL & l) != 0L)
                     { jjCheckNAddTwoStates(70, 71); }
                  break;
               case 72:
                  if ((0x2000000020ULL & l) != 0L)
                     { jjCheckNAddTwoStates(73, 74); }
                  break;
               case 75:
                  if (curChar == 95)
                     { jjCheckNAdd(74); }
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
                  { jjAddStates(14, 18); }
                  break;
               case 9:
                  if (jjCanMove_1(hiByte, i1, i2, l1, l2))
                     { jjAddStates(45, 46); }
                  break;
               case 15:
                  if (jjCanMove_1(hiByte, i1, i2, l1, l2))
                     { jjAddStates(21, 22); }
                  break;
               case 18:
                  if (jjCanMove_1(hiByte, i1, i2, l1, l2))
                     jjstateSet[jjnewStateCnt++] = 19;
                  break;
               case 28:
                  if (jjCanMove_1(hiByte, i1, i2, l1, l2))
                     { jjAddStates(47, 48); }
                  break;
               case 30:
                  if (!jjCanMove_1(hiByte, i1, i2, l1, l2))
                     break;
                  if (kind > 187)
                     kind = 187;
                  { jjAddStates(53, 54); }
                  break;
               case 33:
                  if (!jjCanMove_0(hiByte, i1, i2, l1, l2))
                     break;
                  if (kind > 7)
                     kind = 7;
                  { jjAddStates(26, 28); }
                  break;
               case 39:
                  if (!jjCanMove_0(hiByte, i1, i2, l1, l2))
                     break;
                  if (kind > 8)
                     kind = 8;
                  { jjAddStates(29, 31); }
                  break;
               case 45:
                  if (jjCanMove_0(hiByte, i1, i2, l1, l2))
                     { jjAddStates(49, 50); }
                  break;
               case 50:
                  if (jjCanMove_0(hiByte, i1, i2, l1, l2))
                     { jjAddStates(51, 52); }
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
      if ((i = jjnewStateCnt), (jjnewStateCnt = startsAt), (i == (startsAt = 76 - startsAt)))
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
   JAVACC_STRING_TYPE curTokenImage;
   int beginLine   = -1;
   int endLine     = -1;
   int beginColumn = -1;
   int endColumn   = -1;
   JAVACC_STRING_TYPE im = jjstrLiteralImages[jjmatchedKind];
   curTokenImage = (im.length() == 0) ? input_stream->GetImage() : im;
   if (input_stream->getTrackLineColumn()) {
     beginLine = input_stream->getBeginLine();
     beginColumn = input_stream->getBeginColumn();
     endLine = input_stream->getEndLine();
     endColumn = input_stream->getEndColumn();
   }
   t = Token::newToken(jjmatchedKind);
   t->kind = jjmatchedKind;
   t->image = curTokenImage;
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
         SkipLexicalActions(NULL);
         goto EOFLoop;
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
   errorHandler->lexicalError(EOFSeen, curLexState, error_line, error_column, error_after, curChar, this);
  }
}


void  VhdlParserTokenManager::SkipLexicalActions(Token *matchedToken){
   switch(jjmatchedKind)
   {
      case 3 : {
         image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
        ::vhdl::parser::VhdlParser::lineCount();
         break;
       }
      case 6 : {
         image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
    {
    QCString doc(image.data());
    int count=doc.contains("--!");
    ::vhdl::parser::VhdlParser::setMultCommentLine();
    ::vhdl::parser::VhdlParser::lineCount(image.data());
    if (count == 1)
      ::vhdl::parser::VhdlParser::oneLineComment(doc);
    else
      ::vhdl::parser::VhdlParser::handleCommentBlock(image.data(),FALSE); ;
    }
         break;
       }
      case 7 : {
         image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                                                                               ::vhdl::parser::VhdlParser::handleFlowComment(image.data());
         break;
       }
      case 8 : {
         image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
    ::vhdl::parser::VhdlParser::lineCount(image.data());
         break;
       }
      case 9 : {
         image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
   {
     QCString q(image.data());
     q.stripPrefix("/*!");
     q.resize(q.length()-2);
     ::vhdl::parser::VhdlParser::handleCommentBlock(q.data(),TRUE);image.clear();
   }
         break;
       }
      case 10 : {
         image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                                                    ::vhdl::parser::VhdlParser::lineCount(image.data());image.clear();
         break;
       }
      default :
         break;
   }
}

void  VhdlParserTokenManager::TokenLexicalActions(Token *matchedToken){
   switch(jjmatchedKind)
   {
      case 17 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                                    ::vhdl::parser::VhdlParser::setLineParsed(ARCHITECTURE_T);
         break;
       }
      case 18 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                      VhdlParser::setLineParsed(ARRAY_T);
         break;
       }
      case 22 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                              ::vhdl::parser::VhdlParser::setLineParsed(ATTRIBUTE_T);
         break;
       }
      case 25 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                    ::vhdl::parser::VhdlParser::setLineParsed(BODY_T);
         break;
       }
      case 28 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                              VhdlParser::setLineParsed(COMPONENT_T);
         break;
       }
      case 30 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                                      VhdlParser::setLineParsed(CONFIGURATION_T);
         break;
       }
      case 31 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                             VhdlParser::setLineParsed(CONSTANT_T);
         break;
       }
      case 32 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                          VhdlParser::setLineParsed(CONTEXT_T);
         break;
       }
      case 39 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                  VhdlParser::setLineParsed(END_T);
         break;
       }
      case 40 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                        VhdlParser::setLineParsed(ENTITY_T);
         break;
       }
      case 43 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                    VhdlParser::setLineParsed(FILE_T);
         break;
       }
      case 46 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                             VhdlParser::setLineParsed(FUNCTION_T);
         break;
       }
      case 49 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                      VhdlParser::setLineParsed(GROUP_T);
         break;
       }
      case 58 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                          VhdlParser::setLineParsed(LIBRARY_T);
         break;
       }
      case 76 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                          ::vhdl::parser::VhdlParser::setLineParsed(PACKAGE_T);
         break;
       }
      case 78 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                    ::vhdl::parser::VhdlParser::setLineParsed(PORT_T);
         break;
       }
      case 80 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                              ::vhdl::parser::VhdlParser::setLineParsed(PROCEDURE_T);
         break;
       }
      case 81 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                          ::vhdl::parser::VhdlParser::setLineParsed(PROCESS_T);
         break;
       }
      case 86 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                        ::vhdl::parser::VhdlParser::setLineParsed(RECORD_T);
         break;
       }
      case 100 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                        ::vhdl::parser::VhdlParser::setLineParsed(SIGNAL_T);
         break;
       }
      case 107 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                          ::vhdl::parser::VhdlParser::setLineParsed(SUBTYPE_T);
         break;
       }
      case 111 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                    ::vhdl::parser::VhdlParser::setLineParsed(TYPE_T);
         break;
       }
      case 113 : {
        image.append(input_stream->GetSuffix(jjimageLen + (lengthOfMatch = jjmatchedPos + 1)));
                      ::vhdl::parser::VhdlParser::setLineParsed(UNITS_T);
         break;
       }
      case 137 : {
        image.append(jjstrLiteralImages[137]);
        lengthOfMatch = jjstrLiteralImages[137].length();
                  ::vhdl::parser::VhdlParser::setLineParsed(SEMI_T);
         break;
       }
      default :
         break;
   }
}
  /** Reinitialise parser. */
  void VhdlParserTokenManager::ReInit(JAVACC_CHARSTREAM *stream, int lexState, VhdlParser *parserArg) {
    clear();
    jjmatchedPos = jjnewStateCnt = 0;
    curLexState = lexState;
    input_stream = stream;
    ReInitRounds();
    debugStream = stdout; // init
    SwitchTo(lexState);
    parser = parserArg;
    errorHandler = new TokenManagerErrorHandler();
    errorHandlerCreated = true;
  }

  void VhdlParserTokenManager::ReInitRounds() {
    int i;
    jjround = 0x80000001;
    for (i = 76; i-- > 0;)
      jjrounds[i] = 0x80000000;
  }

  /** Switch to specified lex state. */
  void VhdlParserTokenManager::SwitchTo(int lexState) {
    if (lexState >= 1 || lexState < 0) {
      JAVACC_STRING_TYPE message;
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
  VhdlParserTokenManager::VhdlParserTokenManager (JAVACC_CHARSTREAM *stream, int lexState, VhdlParser *parserArg)
  {
    input_stream = NULL;
    errorHandlerCreated = false;
    ReInit(stream, lexState, parserArg);
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
    if (errorHandlerCreated) delete errorHandler;    
  }


}
}
