/******************************************************************************
 *
 * Copyright (C) 1997-2012 by Dimitri van Heesch.
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
#include "memberlist.h"

class Entry;
class ClassDef;
class MemberDef;
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
    static  bool    foundInsertedComponent(const QCString& name,Entry* root);
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
    static void findAllPackages(const QCString& className,
                                QDict<QCString>&);
    static MemberDef* findMemberDef(ClassDef* cd,
                                const QCString& key,
                                MemberList::ListType type);
    static ClassDef *getClass(const char *name);
    static MemberDef* findFunction(const QList<Argument> &ql,
                                   const QCString& name,
                                   const QCString& package, bool type);
    static bool compareString(const QCString& s1,
                              const QCString& s2);
    static QCString getClassTitle(const ClassDef*);
    static void writeInlineClassLink(const ClassDef*,
                                     OutputList &ol);

    static bool isConstraint(const MemberDef *mdef) 
    { return mdef->getMemberSpecifiers()==VhdlDocGen::UCF_CONST; }   
    static bool isConfig(const MemberDef *mdef) 
    { return mdef->getMemberSpecifiers()==VhdlDocGen::CONFIG; }
    static bool isAlias(const MemberDef *mdef) 
    { return mdef->getMemberSpecifiers()==VhdlDocGen::ALIAS; }
    static bool isLibrary(const MemberDef *mdef) 
    { return mdef->getMemberSpecifiers()==VhdlDocGen::LIBRARY; }
    static bool isGeneric(const MemberDef *mdef) 
    { return mdef->getMemberSpecifiers()==VhdlDocGen::GENERIC; }
    static bool isPort(const MemberDef *mdef) 
    { return mdef->getMemberSpecifiers()==VhdlDocGen::PORT; }
    static bool isComponent(const MemberDef *mdef) 
    { return mdef->getMemberSpecifiers()==VhdlDocGen::COMPONENT; }
    static bool isPackage(const MemberDef *mdef) 
    { return mdef->getMemberSpecifiers()==VhdlDocGen::USE; }
    static bool isEntity(const MemberDef *mdef) 
    { return mdef->getMemberSpecifiers()==VhdlDocGen::ENTITY; }
    static bool isConstant(const MemberDef *mdef) 
    { return mdef->getMemberSpecifiers()==VhdlDocGen::CONSTANT; }
    static bool isVType(const MemberDef *mdef) 
    { return mdef->getMemberSpecifiers()==VhdlDocGen::TYPE; }
    static bool isSubType(const MemberDef *mdef) 
    { return mdef->getMemberSpecifiers()==VhdlDocGen::SUBTYPE; }
    static bool isVhdlFunction(const MemberDef *mdef) 
    { return mdef->getMemberSpecifiers()==VhdlDocGen::FUNCTION; }
    static bool isProcess(const MemberDef *mdef) 
    { return mdef->getMemberSpecifiers()==VhdlDocGen::PROCESS; }
    static bool isSignal(const MemberDef *mdef) 
    { return mdef->getMemberSpecifiers()==VhdlDocGen::SIGNAL; }
    static bool isAttribute(const MemberDef *mdef)
    { return mdef->getMemberSpecifiers()==VhdlDocGen::ATTRIBUTE; }
    static bool isSignals(const MemberDef *mdef)
    { return mdef->getMemberSpecifiers()==VhdlDocGen::SIGNAL; }
    static bool isProcedure(const MemberDef *mdef) 
    { return mdef->getMemberSpecifiers()==VhdlDocGen::PROCEDURE; }
    static bool isRecord(const MemberDef *mdef) 
    { return mdef->getMemberSpecifiers()==VhdlDocGen::RECORD; }
    static bool isArchitecture(const MemberDef *mdef) 
    { return mdef->getMemberSpecifiers()==VhdlDocGen::ARCHITECTURE; }
    static bool isUnit(const MemberDef *mdef) 
    { return mdef->getMemberSpecifiers()==VhdlDocGen::UNITS; }
    static bool isPackageBody(const MemberDef *mdef) 
    { return mdef->getMemberSpecifiers()==VhdlDocGen::PACKAGE_BODY; }
    static bool isVariable(const MemberDef *mdef) 
    { return mdef->getMemberSpecifiers()==VhdlDocGen::SHAREDVARIABLE; }
    static bool isFile(const MemberDef *mdef) 
    { return mdef->getMemberSpecifiers()==VhdlDocGen::VFILE; }
    static bool isGroup(const MemberDef *mdef) 
    { return mdef->getMemberSpecifiers()==VhdlDocGen::GROUP; }
    static bool isCompInst(const MemberDef *mdef) 
    { return mdef->getMemberSpecifiers()==VhdlDocGen::INSTANTIATION; }
    static bool isMisc(const MemberDef *mdef) 
    { return mdef->getMemberSpecifiers()==VhdlDocGen::MISCELLANEOUS; }

    //-----------------------------------------------------
    // translatable items
    
    static QCString trTypeString(int type);
    static QCString trVhdlType(int type,bool sing=true);

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
    static void writeFuncProcDocu(const MemberDef *mdef, OutputList& ol,const ArgumentList* al,bool type=false);
    static void writeRecordProto(const MemberDef *mdef, OutputList& ol,const ArgumentList *al);

    static void writeVHDLTypeDocumentation(const MemberDef* mdef, const Definition* d, OutputList &ol);

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
    // obsolete
    // static void adjustRecordMember(MemberDef *mdef);

    static void writeLink(const MemberDef* mdef,OutputList &ol);
    static void adjustMemberName(QCString& nn);
    static bool membersHaveSpecificType(MemberList *ml,int type);
    static void startFonts(const QCString& q, const char *keyword,OutputList& ol);
    static bool isNumber(const QCString& s);
    static QCString getProtectionName(int prot);
    static QCString splitString(QCString & str,  char c);
    static void parseUCF(const char*  input,Entry* entity,QCString f,bool vendor);
	
    static bool findConstraintFile( LayoutNavEntry *lne);

    static ClassDef*  findArchitecture(const ClassDef *cd);
    static ClassDef*  findArchitecture(QCString identifier, QCString entity_name);

    static void writeCodeFragment( MemberDef *mdef,OutputList& ol);
    static void writeCodeFragment (OutputList& ol,int start, QCString & codeFragment,const MemberDef* mdef);

    static void writeSource(MemberDef *mdef,OutputList& ol,QCString & cname);
    static void writeAlphbeticalClass(OutputList& ol,const ClassDef* cd,const QCString &);

    static QCString  parseForConfig(QCString & entity,QCString & arch);
    static QCString  parseForBinding(QCString & entity,QCString & arch);
    static void addBaseClass(ClassDef* cd,ClassDef *ent);

    static void writeOverview(OutputList &ol);
    static void writeOverview();

  private:
    static void findAllArchitectures(QList<QCString>& ql,const ClassDef *cd);
    static bool compareArgList(ArgumentList*,ArgumentList*);
    static void writeVhdlLink(const ClassDef* cdd ,OutputList& ol,QCString& type,QCString& name,QCString& beh);
    static void writeStringLink(const MemberDef *mdef,QCString mem,OutputList& ol);
    static void writeRecUnitDocu( const MemberDef *md, OutputList& ol,QCString largs);
    static void  writeRecorUnit(QCString & largs,OutputList& ol ,const MemberDef *mdef);
};

#endif
