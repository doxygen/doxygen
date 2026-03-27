/******************************************************************************
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby
 * granted. No representations are made about the suitability of this software
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * Documents produced by Doxygen are derivative works derived from the
 * input used in their production; they are not affected by this license.
 *
 */

#ifndef VHDLDOCGEN_H
#define VHDLDOCGEN_H

/**
 * This class implements functions for parsing and generating
 * vhdl documents
 */

#include "qcstring.h"
#include "layout.h"
#include "arguments.h"
#include "entry.h"

class Entry;
class ClassDef;
class MemberList;
class MemberDef;
class ModuleDef;
class MemberDefMutable;
class OutputList;
class Definition;
class GroupDef;
class FileDef;
class NamespaceDef;
class TextStream;
struct Argument;

struct VhdlConfNode
{
  VhdlConfNode(const QCString &a,const QCString &b,const QCString &config,const QCString &cs,bool leaf)
  {
    arch=a;              // architecture  e.g. for iobuffer
    arch=arch.lower();
    binding=b;           // binding e.g.  use entity work.xxx(bev)
    binding=binding.lower();
    confVhdl=config;     // configuration foo is bar
    compSpec=cs;
    isInlineConf=false;  // primary configuration?
    isLeaf=leaf;
  }

  QCString confVhdl;
  QCString arch;
  QCString binding;
  QCString compSpec;
  int level = 0;
  bool isLeaf = false;
  bool isInlineConf = false;

};


/** Class for generating documentation specific for VHDL */
class VhdlDocGen
{
  public:

    enum VhdlClasses       // Overlays: Protection
    {
      ENTITYCLASS,         // Overlays: Public
      PACKBODYCLASS,       // Overlays: Protected
      ARCHITECTURECLASS,   // Overlays: Private
      PACKAGECLASS         // Overlays: Package
    };
    static VhdlClasses convert(Protection prot)
    {
      switch (prot)
      {
        case Protection::Public:    return ENTITYCLASS;
        case Protection::Protected: return PACKBODYCLASS;
        case Protection::Private:   return ARCHITECTURECLASS;
        case Protection::Package:   return PACKAGECLASS;
      }
      return ENTITYCLASS;
    }

    static void init();
    static QCString convertFileNameToClassName(const QCString &name);
    // --- used by vhdlscanner.l -----------

    static bool isSubClass(ClassDef* cd,ClassDef *scd, bool followInstances,int level);

    static QCString getIndexWord(const QCString &,int index);
    static bool     deleteCharRev(QCString &s,char c);
    static void     deleteAllChars(QCString &s,char c);
    static void     parseFuncProto(const QCString &text,
                                   QCString& name,
                                   QCString& ret,
                                   bool doc=false);
    // -----------------------------------

    static void computeVhdlComponentRelations();

    static const char* findKeyWord(const QCString& word);

    static ClassDef* getPackageName(const QCString& name);
    static const MemberDef* findMember(const QCString& className,
                                 const QCString& memName);
    static void findAllPackages(ClassDef*);
    static const MemberDef* findMemberDef(ClassDef* cd,
                                const QCString& key,
                                MemberListType type);
    static ClassDef *getClass(const QCString &name);
    static const MemberDef* findFunction(const QCString& name,
                                   const QCString& package);
    static QCString getClassTitle(const ClassDef*);
    static void writeInlineClassLink(const ClassDef*,
                                     OutputList &ol);
    static void writeTagFile(MemberDefMutable *mdef,TextStream &tagFile);

    static bool isConstraint(const MemberDef *mdef);
    static bool isConfig(const MemberDef *mdef);
    static bool isAlias(const MemberDef *mdef);
    static bool isLibrary(const MemberDef *mdef);
    static bool isGeneric(const MemberDef *mdef);
    static bool isPort(const MemberDef *mdef);
    static bool isComponent(const MemberDef *mdef);
    static bool isPackage(const MemberDef *mdef);
    static bool isEntity(const MemberDef *mdef);
    static bool isConstant(const MemberDef *mdef);
    static bool isVType(const MemberDef *mdef);
    static bool isSubType(const MemberDef *mdef);
    static bool isVhdlFunction(const MemberDef *mdef);
    static bool isProcess(const MemberDef *mdef);
    static bool isSignal(const MemberDef *mdef);
    static bool isAttribute(const MemberDef *mdef);
    static bool isSignals(const MemberDef *mdef);
    static bool isProcedure(const MemberDef *mdef);
    static bool isRecord(const MemberDef *mdef);
    static bool isArchitecture(const MemberDef *mdef);
    static bool isUnit(const MemberDef *mdef);
    static bool isPackageBody(const MemberDef *mdef);
    static bool isVariable(const MemberDef *mdef);
    static bool isFile(const MemberDef *mdef);
    static bool isGroup(const MemberDef *mdef);
    static bool isCompInst(const MemberDef *mdef);
    static bool isMisc(const MemberDef *mdef);

    //-----------------------------------------------------

    static void prepareComment(QCString&);
    static void formatString(const QCString&,OutputList& ol,const MemberDef*);

    static void writeFormatString(const QCString&,OutputList& ol,const MemberDef*);
    static void writeFunctionProto(OutputList& ol,const ArgumentList &al,const MemberDef*);
    static void writeProcessProto(OutputList& ol,const ArgumentList &al,const MemberDef*);
    static void writeProcedureProto(OutputList& ol, const ArgumentList &al,const MemberDef*);
    static bool writeFuncProcDocu(const MemberDef *mdef, OutputList& ol,const ArgumentList &al,bool type=false);
    static void writeRecordProto(const MemberDef *mdef, OutputList& ol,const ArgumentList &al);

    static bool writeVHDLTypeDocumentation(const MemberDef* mdef, const Definition* d, OutputList &ol);

    static void writeVhdlDeclarations(const MemberList*,OutputList&,const GroupDef*,const ClassDef*,const FileDef*,const NamespaceDef*,const ModuleDef *);

    static void writeVHDLDeclaration(MemberDefMutable* mdef,OutputList &ol,
        const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,const ModuleDef *mod,
        bool inGroup);

    static void writePlainVHDLDeclarations(const MemberList* ml,OutputList &ol,
        const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,const ModuleDef *mod,
        VhdlSpecifier specifier);

    static void writeVHDLDeclarations(const MemberList* ml,OutputList &ol,
        const ClassDef *cd,const NamespaceDef *nd,const FileDef *fd,const GroupDef *gd,const ModuleDef *mod,
        const QCString &title,const QCString &subtitle,bool showEnumValues,VhdlSpecifier type);

    static bool writeClassType(const ClassDef *,OutputList &ol ,QCString & cname);

    static QCString convertArgumentListToString(const ArgumentList &al,bool f);
    static QCString getProcessNumber();
    static QCString getRecordNumber();

    static QCString getClassName(const ClassDef*);
    static bool isNumber(const std::string& s);
    static QCString getProtectionName(int prot);

    static void parseUCF(const QCString &input,Entry* entity,const QCString &f,bool vendor);

    static const ClassDef*  findArchitecture(const ClassDef *cd);

    static void correctMemberProperties(MemberDefMutable *md);

    static void writeSource(const MemberDef *mdef,OutputList& ol,const QCString & cname);

    static QCString  parseForConfig(QCString & entity,QCString & arch);
    static QCString  parseForBinding(QCString & entity,QCString & arch);
    static void addBaseClass(ClassDef* cd,ClassDef *ent);
    static ClassDef* findVhdlClass(const QCString &className );

 // flowcharts
    static void createFlowChart(const MemberDef*);
    //static void addFlowImage(const TextStream &,const QCString &);

    static void setFlowMember( const MemberDef *flowMember);
    static const MemberDef *getFlowMember();

    static  bool isVhdlClass (const Entry *cu)
    {
      return cu->vhdlSpec==VhdlSpecifier::ENTITY       ||
             cu->vhdlSpec==VhdlSpecifier::PACKAGE      ||
             cu->vhdlSpec==VhdlSpecifier::ARCHITECTURE ||
             cu->vhdlSpec==VhdlSpecifier::PACKAGE_BODY;
    }

  static void resetCodeVhdlParserState();

  private:
    static void findAllArchitectures(std::vector<QCString>& ql,const ClassDef *cd);
    static bool compareArgList(const ArgumentList &,const ArgumentList &);
    static void writeVhdlLink(const ClassDef* cdd ,OutputList& ol,QCString& type,QCString& name,QCString& beh);
    static void writeStringLink(const MemberDef *mdef,QCString mem,OutputList& ol);
    static void writeRecUnitDocu( const MemberDef *md, OutputList& ol,QCString largs);
    static void  writeRecordUnit(QCString & largs,QCString & ltype,OutputList& ol,MemberDefMutable *mdef);
};

//-------------------------------------------------------------------------------------------------------------------
//-------------- VHDL Flowcharts -------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------


//#define DEBUGFLOW

class FlowChart
{
  public:
    enum nodeTypes {
      IF_NO        = 1<<1,
      ELSIF_NO     = 1<<2,
      ELSE_NO      = 1<<3,
      CASE_NO      = 1<<4,
      WHEN_NO      = 1<<5,
      EXIT_NO      = 1<<6,
      END_NO       = 1<<7,
      TEXT_NO      = 1<<8,
      START_NO     = 1<<9,
      ENDIF_NO     = 1<<10,
      FOR_NO       = 1<<11,
      WHILE_NO     = 1<<12,
      END_LOOP     = 1<<13,
      END_CASE     = 1<<14,
      VARIABLE_NO  = 1<<15,
      RETURN_NO    = 1<<16,
      LOOP_NO      = 1<<17,
      NEXT_NO      = 1<<18,
      EMPTY_NO     = 1<<19,
      COMMENT_NO   = 1<<20,
      BEGIN_NO     = 1<<21
    };

    //---------- create svg -------------------------------------------------------------
    static void createSVG();
    static void startDot(TextStream &t);
    static void endDot(TextStream &t);
    static void codify(TextStream &t,const QCString &str);
    static void writeShape(TextStream &t,const FlowChart &fl);
    static void writeEdge(TextStream &t,int fl_from,int fl_to,int i,bool bFrom=FALSE,bool bTo=FALSE);
    static void writeEdge(TextStream &t,const FlowChart &fl_from,const FlowChart &fl_to,int i);
    static void writeFlowLinks(TextStream &t);

    static QCString getNodeName(int n);
    static void colTextNodes();

    static size_t getNextIfLink(const FlowChart&,size_t);
    static size_t getNextNode(size_t index,int stamp);
    static size_t findNode(size_t index,int stamp,int type);
    static size_t findNode(size_t index,int type);
    static size_t findNextLoop(size_t j,int stamp);
    static size_t findPrevLoop(size_t j,int stamp,bool endif=FALSE);
    static size_t findLabel(size_t j,const QCString &);
    static void delFlowList();
    static const char* getNodeType(int c);

    static void addFlowChart(int type,const QCString &text,const QCString &exp,const QCString &label=QCString());
    static void moveToPrevLevel();
    static int getTimeStamp();
    static void writeFlowChart();
    static void alignFuncProc(QCString & q,const ArgumentList &al,bool isFunc);
    static QCString convertNameToFileName();
    static void printNode(const FlowChart& n);
    static void printFlowTree();
    static void buildCommentNodes(TextStream &t);
    static void alignCommentNode(TextStream &t,QCString com);

    static void  printUmlTree();
    static QCString printPlantUmlNode(const FlowChart &flo,bool,bool);

    FlowChart(int typ,const QCString &t,const QCString &ex,const QCString &label=QCString());

private:
    int id = 0;
    int stamp = 0;
    int type = 0;

    int line = 0;

    QCString label;
    QCString text;
    QCString exp;
};

extern std::vector<FlowChart> flowList;

#endif
