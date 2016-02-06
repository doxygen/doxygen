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

#include <qdict.h>
#include <qcstring.h>
#include "layout.h"
#include "arguments.h"
#include "entry.h"

class Entry;
class ClassDef;
class MemberList;
class MemberDef;
class FTextStream;
class OutputList;
class Definition;
class GroupDef;
class FileDef;
class NamespaceDef;
struct Argument;

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

    enum VhdlKeyWords
    {
      LIBRARY=1,
      ENTITY,
      PACKAGE_BODY,
      ARCHITECTURE,
      PACKAGE,
      ATTRIBUTE,
      SIGNAL,
      COMPONENT,
      CONSTANT,
      TYPE,
      SUBTYPE,
      FUNCTION,
      RECORD,
      PROCEDURE,
      USE,
      PROCESS,
      PORT,
      UNITS,	  
      GENERIC,
      INSTANTIATION,
      GROUP,
      VFILE,   
      SHAREDVARIABLE,
      CONFIG,
      ALIAS,
      MISCELLANEOUS,
      UCF_CONST
    };

    VhdlDocGen();
    virtual ~VhdlDocGen();
    static void init();
    static QCString convertFileNameToClassName(QCString name);
    // --- used by vhdlscanner.l -----------
    
    static bool isSubClass(ClassDef* cd,ClassDef *scd, bool followInstances,int level);

    static QCString getIndexWord(const char* ,int index);
    static bool     deleteCharRev(QCString &s,char c);
    static void     deleteAllChars(QCString &s,char c);
    static void     parseFuncProto(const char* text,
                                   QList<Argument>& , 
                                   QCString& name,
                                   QCString& ret,
                                   bool doc=false);
    // -----------------------------------

    static void computeVhdlComponentRelations();

    static QCString* findKeyWord(const QCString& word);

    static ClassDef* getPackageName(const QCString& name);
    static MemberDef* findMember(const QCString& className, 
                                 const QCString& memName);
    static void findAllPackages(ClassDef*);
    static MemberDef* findMemberDef(ClassDef* cd,
                                const QCString& key,
                                MemberListType type);
    static ClassDef *getClass(const char *name);
    static MemberDef* findFunction(const QList<Argument> &ql,
                                   const QCString& name,
                                   const QCString& package, bool type);
    static QCString getClassTitle(const ClassDef*);
    static void writeInlineClassLink(const ClassDef*,
                                     OutputList &ol);
    static void writeTagFile(MemberDef *mdef,FTextStream &tagFile);

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
    // translatable items
    
    static QCString trTypeString(uint64 type);
    static QCString trVhdlType(uint64 type,bool sing=true);

    // trClassHierarchy.
    static QCString trDesignUnitHierarchy();

    // trCompoundList
    static QCString trDesignUnitList();

    // trCompoundMembers.
    static QCString trDesignUnitMembers();

    // trCompoundListDescription
    static QCString trDesignUnitListDescription();

    // trCompounds
    static QCString trDesignUnits();

    // trCompoundIndex
    static QCString trDesignUnitIndex();

    // trFunctions
    static QCString trFunctionAndProc();

    //-----------------------------------------------------

    static void prepareComment(QCString&);
    static void formatString(const QCString&,OutputList& ol,const MemberDef*);

    static void writeFormatString(const QCString&,OutputList& ol,const MemberDef*);
    static void writeFunctionProto(OutputList& ol,const ArgumentList *al,const MemberDef*);
    static void writeProcessProto(OutputList& ol,const ArgumentList *al,const MemberDef*);
    static void writeProcedureProto(OutputList& ol, const ArgumentList *al,const MemberDef*);
    static bool writeFuncProcDocu(const MemberDef *mdef, OutputList& ol,const ArgumentList* al,bool type=false);
    static void writeRecordProto(const MemberDef *mdef, OutputList& ol,const ArgumentList *al);

    static bool writeVHDLTypeDocumentation(const MemberDef* mdef, const Definition* d, OutputList &ol);

    static void writeVhdlDeclarations(MemberList*,OutputList&,GroupDef*,ClassDef*,FileDef*,NamespaceDef*);

    static void writeVHDLDeclaration(MemberDef* mdef,OutputList &ol,
        ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd,
        bool inGroup);

    static void writePlainVHDLDeclarations(MemberList* ml,OutputList &ol,
        ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd,int specifier);

    static void writeVHDLDeclarations(MemberList* ml,OutputList &ol,
        ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd,
        const char *title,const char *subtitle,bool showEnumValues,int type);

    static bool writeClassType(ClassDef *&,OutputList &ol ,QCString & cname);

    static QCString convertArgumentListToString(const ArgumentList* al,bool f);
    static QCString getProcessNumber();
    static QCString getRecordNumber();
   
    static QCString getClassName(const ClassDef*);
    static bool isNumber(const QCString& s);
    static QCString getProtectionName(int prot);

    static void parseUCF(const char*  input,Entry* entity,QCString f,bool vendor);
    static bool findConstraintFile( LayoutNavEntry *lne);

    static ClassDef*  findArchitecture(const ClassDef *cd);
    static ClassDef*  findArchitecture(QCString identifier, QCString entity_name);

    
    static void writeSource(MemberDef *mdef,OutputList& ol,QCString & cname);
    static void writeAlphbeticalClass(OutputList& ol,const ClassDef* cd,const QCString &);

    static QCString  parseForConfig(QCString & entity,QCString & arch);
    static QCString  parseForBinding(QCString & entity,QCString & arch);
    static void addBaseClass(ClassDef* cd,ClassDef *ent);
    static ClassDef* findVhdlClass(const char *className );

    static void writeOverview(OutputList &ol);
    static void writeOverview();
 
 // flowcharts
    static void createFlowChart(const MemberDef*);
    //static void addFlowImage(const FTextStream &,const QCString &);
    
    static void setFlowMember( const MemberDef *flowMember);
    static const MemberDef *getFlowMember();

    static  bool isVhdlClass (const Entry *cu) 
    {
      return cu->spec==VhdlDocGen::ENTITY       ||
             cu->spec==VhdlDocGen::PACKAGE      ||
             cu->spec==VhdlDocGen::ARCHITECTURE ||
             cu->spec==VhdlDocGen::PACKAGE_BODY;
    }

  static void resetCodeVhdlParserState();

  private:
    static void findAllArchitectures(QList<QCString>& ql,const ClassDef *cd);
    static bool compareArgList(ArgumentList*,ArgumentList*);
    static void writeVhdlLink(const ClassDef* cdd ,OutputList& ol,QCString& type,QCString& name,QCString& beh);
    static void writeStringLink(const MemberDef *mdef,QCString mem,OutputList& ol);
    static void writeRecUnitDocu( const MemberDef *md, OutputList& ol,QCString largs);
    static void  writeRecorUnit(QCString & largs,OutputList& ol ,const MemberDef *mdef);
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
    static void startDot(FTextStream &t);
    static void endDot(FTextStream &t);
    static void codify(FTextStream &t,const char *str);
    static void writeShape(FTextStream &t,const FlowChart* fl);
    static void writeEdge(FTextStream &t,int fl_from,int fl_to,int i,bool bFrom=FALSE,bool bTo=FALSE);
    static void writeEdge(FTextStream &t,const FlowChart* fl_from,const FlowChart* fl_to,int i);
    static void writeFlowLinks(FTextStream &t);

    static QCString getNodeName(int n);
    static void colTextNodes();

    static int getNextTextLink(const FlowChart* fl,uint index);
    static int getNextIfLink(const FlowChart*,uint);
    static int getNextNode(int,int);
    static int findNode(int index,int stamp,int type);
    static int findNode(int index,int type);
    static int findNextLoop(int j,int stamp);
    static int findPrevLoop(int j,int stamp,bool endif=FALSE);
    static int findLabel(int j,QCString &);
    static void delFlowList();
    static const char* getNodeType(int c);

    static void addFlowChart(int type,const char* text,const char* exp,const char * label=NULL);
    static void moveToPrevLevel();
    static int getTimeStamp();
    static void writeFlowChart();
    static void alignFuncProc(QCString & q,const ArgumentList*  al,bool isFunc);
    static QCString convertNameToFileName();
    static void printNode(const FlowChart* n);
    static void printFlowTree();
    static void buildCommentNodes(FTextStream &t);
    static void alignCommentNode(FTextStream &t,QCString com);

    static void  printUmlTree();
    static QCString printPlantUmlNode(const FlowChart *flo,bool,bool);

    static QList<FlowChart> flowList;

    FlowChart(int typ,const char*  t,const char* ex,const char* label=0);
    ~FlowChart();

private:
    int id;
    int stamp;
    int type;

    int line;

    QCString label;
    QCString text;
    QCString exp;
};

#endif
