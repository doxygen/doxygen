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

#include <stdio.h>
#include <qglobal.h>
#include <qregexp.h>
#include <assert.h>
#include "md5.h"
#include "memberdef.h"
#include "membername.h"
#include "doxygen.h"
#include "util.h"
#include "code.h"
#include "message.h"
#include "htmlhelp.h"
#include "language.h"
#include "outputlist.h"
#include "example.h"
#include "membergroup.h"
#include "groupdef.h"
#include "defargs.h"
#include "docparser.h"
#include "dot.h"
#include "searchindex.h"
#include "parserintf.h"
#include "marshal.h"
#include "objcache.h"

#include "vhdldocgen.h"
#include "arguments.h"
#include "memberlist.h"
#include "namespacedef.h"
#include "filedef.h"
#include "config.h"

//-----------------------------------------------------------------------------

int MemberDef::s_indentLevel = 0;

//-----------------------------------------------------------------------------

static QCString addTemplateNames(const QCString &s,const QCString &n,const QCString &t)
{
  QCString result;
  QCString clRealName=n;
  int p=0,i;
  if ((i=clRealName.find('<'))!=-1)
  {
    clRealName=clRealName.left(i); // strip template specialization
  }
  if ((i=clRealName.findRev("::"))!=-1)
  {
    clRealName=clRealName.right(clRealName.length()-i-2);
  }
  while ((i=s.find(clRealName,p))!=-1)
  {
    result+=s.mid(p,i-p);
    uint j=clRealName.length()+i;
    if (s.length()==j || (s.at(j)!='<' && !isId(s.at(j))))
    { // add template names
      //printf("Adding %s+%s\n",clRealName.data(),t.data());
      result+=clRealName+t;
    }
    else
    { // template names already present
      //printf("Adding %s\n",clRealName.data());
      result+=clRealName;
    }
    p=i+clRealName.length();
  }
  result+=s.right(s.length()-p);
  //printf("addTemplateNames(%s,%s,%s)=%s\n",s.data(),n.data(),t.data(),result.data());
  return result;
}

// ol.startMemberDocName has already been done before this is called.
// when this function returns TRUE, ol.endParameterList will be called.
//
// typical sequence:
//   ol.startMemberDoc
//   ol.startMemberDocName
//   --- enter writeDefArgumentList
//   ol.endMemberDocName
//   ol.startParameterList
//     ...
//     ol.startParameterType(first=TRUE)
//     ol.endParameterType
//     ol.startParameterName
//     ol.endParameterName(last==FALSE)
//     ...
//     ol.startParameterType(first=FALSE)
//     ol.endParamtereType
//     ol.startParameterName
//     ol.endParameterName(last==TRUE)
//     ...
//   --- leave writeDefArgumentList with return value TRUE
//   ol.endParameterList
//   ol.endMemberDoc(hasArgs=TRUE)
//
//  For an empty list the function should return FALSE, the sequence is
//   ol.startMemberDoc
//   ol.startMemberDocName
//   --- enter writeDefArgumentList
//   --- leave writeDefArgumentList with return value FALSE
//   ol.endMemberDocName
//   ol.endMemberDoc(hasArgs=FALSE);
//

static bool writeDefArgumentList(OutputList &ol,Definition *scope,MemberDef *md)
{
  ArgumentList *defArgList=(md->isDocsForDefinition()) ?
                             md->argumentList() : md->declArgumentList();
  //printf("writeDefArgumentList `%s' isDocsForDefinition()=%d\n",md->name().data(),md->isDocsForDefinition());
  if (defArgList==0 || md->isProperty())
  {
    return FALSE; // member has no function like argument list
  }

  // simple argument list for tcl
  if (md->getLanguage()==SrcLangExt_Tcl)
  {
    if (defArgList->count()==0) return FALSE;
    ArgumentListIterator ali(*defArgList);
    Argument *a;
    ol.endMemberDocName();
    ol.startParameterList(FALSE);
    ol.startParameterType(TRUE,0);
    ol.endParameterType();
    ol.startParameterName(FALSE);
    for (;(a=ali.current());++ali)
    {
      if (a->defval.isEmpty())
      {
        ol.docify(a->name+" ");
      }
      else
      {
        ol.docify("?"+a->name+"? ");
      }
    }
    ol.endParameterName(TRUE,FALSE,FALSE);
    return TRUE;
  }

  if (!md->isDefine()) ol.docify(" ");

  //printf("writeDefArgList(%d)\n",defArgList->count());
  ol.pushGeneratorState();
  //ol.disableAllBut(OutputGenerator::Html);
  bool htmlOn  = ol.isEnabled(OutputGenerator::Html);
  bool latexOn = ol.isEnabled(OutputGenerator::Latex);
  {
    // html and latex
    if (htmlOn)  ol.enable(OutputGenerator::Html);
    if (latexOn) ol.enable(OutputGenerator::Latex);

    ol.endMemberDocName();
    ol.startParameterList(!md->isObjCMethod());
  }
  ol.enableAll();
  ol.disable(OutputGenerator::Html);
  ol.disable(OutputGenerator::Latex);
  {
    // other formats
    if (!md->isObjCMethod()) ol.docify("("); // start argument list
    ol.endMemberDocName();
  }
  ol.popGeneratorState();
  //printf("===> name=%s isDefine=%d\n",md->name().data(),md->isDefine());

  QCString cName;
  if (scope)
  {
    cName=scope->name();
    int il=cName.find('<');
    int ir=cName.findRev('>');
    if (il!=-1 && ir!=-1 && ir>il)
    {
      cName=cName.mid(il,ir-il+1);
      //printf("1. cName=%s\n",cName.data());
    }
    else if (scope->definitionType()==Definition::TypeClass && ((ClassDef*)scope)->templateArguments())
    {
      cName=tempArgListToString(((ClassDef*)scope)->templateArguments(),scope->getLanguage());
      //printf("2. cName=%s\n",cName.data());
    }
    else // no template specifier
    {
      cName.resize(0);
    }
  }
  //printf("~~~ %s cName=%s\n",md->name().data(),cName.data());

  bool first=TRUE;
  bool paramTypeStarted=FALSE;
  bool isDefine = md->isDefine();
  ArgumentListIterator ali(*defArgList);
  Argument *a=ali.current();
  while (a)
  {
    if (isDefine || first)
    {
      ol.startParameterType(first,0);
      paramTypeStarted=TRUE;
      if (isDefine)
      {
        ol.endParameterType();
        ol.startParameterName(TRUE);
      }
    }
    QRegExp re(")("),res("(.*\\*");
    int vp=a->type.find(re);
    int wp=a->type.find(res);

    // use the following to put the function pointer type before the name
    bool hasFuncPtrType=FALSE;

    if (!a->attrib.isEmpty() && !md->isObjCMethod()) // argument has an IDL attribute
    {
      ol.docify(a->attrib+" ");
    }
    if (hasFuncPtrType) // argument type is a function pointer
    {
      //printf("a->type=`%s' a->name=`%s'\n",a->type.data(),a->name.data());
      QCString n=a->type.left(vp);
      if (hasFuncPtrType) n=a->type.left(wp);
      if (md->isObjCMethod()) { n.prepend("("); n.append(")"); }
      if (!cName.isEmpty()) n=addTemplateNames(n,scope->name(),cName);
      linkifyText(TextGeneratorOLImpl(ol),scope,md->getBodyDef(),md,n);
    }
    else // non-function pointer type
    {
      QCString n=a->type;
      if (md->isObjCMethod()) { n.prepend("("); n.append(")"); }
      if (a->type!="...")
      {
        if (!cName.isEmpty()) n=addTemplateNames(n,scope->name(),cName);
        linkifyText(TextGeneratorOLImpl(ol),scope,md->getBodyDef(),md,n);
      }
    }
    if (!isDefine)
    {
      if (paramTypeStarted)
      {
        ol.endParameterType();
        paramTypeStarted=FALSE;
      }
      ol.startParameterName(defArgList->count()<2);
    }
    if (hasFuncPtrType)
    {
      ol.docify(a->type.mid(wp,vp-wp));
    }
    if (!a->name.isEmpty() || a->type=="...") // argument has a name
    {
      //if (!hasFuncPtrType)
      //{
      //  ol.docify(" ");
      //}
      ol.disable(OutputGenerator::Latex);
      ol.disable(OutputGenerator::Html);
      ol.docify(" "); /* man page */
      if (htmlOn) ol.enable(OutputGenerator::Html);
      ol.disable(OutputGenerator::Man);
      ol.startEmphasis();
      ol.enable(OutputGenerator::Man);
      if (latexOn) ol.enable(OutputGenerator::Latex);
      if (a->name.isEmpty()) ol.docify(a->type); else ol.docify(a->name);
      ol.disable(OutputGenerator::Man);
      ol.disable(OutputGenerator::Latex);
      ol.endEmphasis();
      ol.enable(OutputGenerator::Man);
      if (latexOn) ol.enable(OutputGenerator::Latex);
    }
    if (!a->array.isEmpty())
    {
      ol.docify(a->array);
    }
    if (hasFuncPtrType) // write the part of the argument type
                        // that comes after the name
    {
      linkifyText(TextGeneratorOLImpl(ol),scope,md->getBodyDef(),
                  md,a->type.right(a->type.length()-vp));
    }
    if (!a->defval.isEmpty()) // write the default value
    {
      QCString n=a->defval;
      if (!cName.isEmpty()) n=addTemplateNames(n,scope->name(),cName);
      ol.docify(" = ");

      ol.startTypewriter();
      linkifyText(TextGeneratorOLImpl(ol),scope,md->getBodyDef(),md,n,FALSE,TRUE,TRUE);
      ol.endTypewriter();

    }
    ++ali;
    a=ali.current();
    if (a)
    {
      if (!md->isObjCMethod()) ol.docify(", "); // there are more arguments
      if (!isDefine)
      {
        QCString key;
        if (md->isObjCMethod() && a->attrib.length()>=2)
        {
          //printf("Found parameter keyword %s\n",a->attrib.data());
          // strip [ and ]
          key=a->attrib.mid(1,a->attrib.length()-2);
          if (key!=",") key+=":"; // for normal keywords add colon
        }
        ol.endParameterName(FALSE,FALSE,!md->isObjCMethod());
        if (paramTypeStarted)
        {
          ol.endParameterType();
        }
        ol.startParameterType(FALSE,key);
        paramTypeStarted=TRUE;
      }
      else // isDefine
      {
        ol.endParameterName(FALSE,FALSE,TRUE);
      }
    }
    first=FALSE;
  }
  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Html);
  ol.disable(OutputGenerator::Latex);
  if (!md->isObjCMethod()) ol.docify(")"); // end argument list
  ol.enableAll();
  if (htmlOn) ol.enable(OutputGenerator::Html);
  if (latexOn) ol.enable(OutputGenerator::Latex);
  if (first) ol.startParameterName(defArgList->count()<2);
  ol.endParameterName(TRUE,defArgList->count()<2,!md->isObjCMethod());
  ol.popGeneratorState();
  if (md->extraTypeChars())
  {
    ol.docify(md->extraTypeChars());
  }
  if (defArgList->constSpecifier)
  {
    ol.docify(" const");
  }
  if (defArgList->volatileSpecifier)
  {
    ol.docify(" volatile");
  }
  if (!defArgList->trailingReturnType.isEmpty())
  {
    linkifyText(TextGeneratorOLImpl(ol), // out
                scope,                   // scope
                md->getBodyDef(),        // fileScope
                md,                      // self
                defArgList->trailingReturnType, // text
                FALSE                    // autoBreak
               );

  }
  return TRUE;
}

static void writeExceptionListImpl(
        OutputList &ol, ClassDef *cd, MemberDef *md, QCString const& exception)
{
  // this is ordinary exception spec - there must be a '('
  //printf("exception='%s'\n",exception.data());
  int index = exception.find('(');
  if (index!=-1)
  {
    ol.exceptionEntry(exception.left(index),false);
    ++index; // paren in second column so skip it here
    for (int comma = exception.find(',', index); comma!=-1; )
    {
      ++comma; // include comma
      linkifyText(TextGeneratorOLImpl(ol),cd,md->getBodyDef(),md,
                  exception.mid(index,comma-index));
      ol.exceptionEntry(0,false);
      index=comma;
      comma = exception.find(',', index);
    }
    int close = exception.find(')', index);
    if (close!=-1)
    {
      QCString type=removeRedundantWhiteSpace(exception.mid(index,close-index));
      linkifyText(TextGeneratorOLImpl(ol),cd,md->getBodyDef(),md,type);
      ol.exceptionEntry(0,true);
    }
    else
    {
      warn(md->getDefFileName(),md->getDefLine(),
          "missing ) in exception list on member %s",qPrint(md->name()));
    }
  }
  else // Java Exception
  {
    ol.docify(" ");
    linkifyText(TextGeneratorOLImpl(ol),cd,md->getBodyDef(),md,exception);
  }
}

static void writeExceptionList(OutputList &ol, ClassDef *cd, MemberDef *md)
{
  QCString exception(QCString(md->excpString()).stripWhiteSpace());
  if ('{'==exception.at(0))
  {
    // this is an UNO IDL attribute - need special handling
    int index = exception.find(';');
    int oldIndex = 1;
    while (-1 != index) // there should be no more than 2 (set / get)
    {
      // omit '{' and ';' -> "set raises (...)"
      writeExceptionListImpl(ol,cd,md,exception.mid(oldIndex,index-oldIndex));
      oldIndex=index+1;
      index = exception.find(';',oldIndex);
    }
    // the rest is now just '}' - omit that
  }
  else
  {
    writeExceptionListImpl(ol,cd,md,exception);
  }
}

static void writeTemplatePrefix(OutputList &ol,ArgumentList *al)
{
  ol.docify("template<");
  ArgumentListIterator ali(*al);
  Argument *a = ali.current();
  while (a)
  {
    ol.docify(a->type);
    ol.docify(" ");
    ol.docify(a->name);
    if (a->defval.length()!=0)
    {
      ol.docify(" = ");
      ol.docify(a->defval);
    }
    ++ali;
    a=ali.current();
    if (a) ol.docify(", ");
  }
  ol.docify("> ");
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class MemberDefImpl
{
  public:
    MemberDefImpl();
   ~MemberDefImpl();
    void init(Definition *def,const char *t,const char *a,const char *e,
              Protection p,Specifier v,bool s,Relationship r,
              MemberType mt,const ArgumentList *tal,
              const ArgumentList *al
             );

    ClassDef     *classDef;   // member of or related to
    FileDef      *fileDef;    // member of file definition
    NamespaceDef *nspace;     // the namespace this member is in.

    MemberDef  *enumScope;    // the enclosing scope, if this is an enum field
    bool        livesInsideEnum;
    MemberDef  *annEnumType;  // the anonymous enum that is the type of this member
    MemberList *enumFields;   // enumeration fields

    MemberDef  *redefines;    // the members that this member redefines
    MemberList *redefinedBy;  // the list of members that redefine this one

    MemberDef  *memDef;       // member definition for this declaration
    MemberDef  *memDec;       // member declaration for this definition
    ClassDef   *relatedAlso;  // points to class marked by relatedAlso

    ExampleSDict *exampleSDict; // a dictionary of all examples for quick access

    QCString type;            // return actual type
    QCString accessorType;    // return type that tell how to get to this member
    ClassDef *accessorClass;  // class that this member accesses (for anonymous types)
    QCString args;            // function arguments/variable array specifiers
    QCString def;             // member definition in code (fully qualified name)
    QCString anc;             // HTML anchor name
    Specifier virt;           // normal/virtual/pure virtual
    Protection prot;          // protection type [Public/Protected/Private]
    QCString decl;            // member declaration in class

    QCString bitfields;       // struct member bitfields
    QCString read;            // property read accessor
    QCString write;           // property write accessor
    QCString exception;       // exceptions that can be thrown
    QCString initializer;     // initializer
    QCString extraTypeChars;  // extra type info found after the argument list
    QCString enumBaseType;    // base type of the enum (C++11)
    int initLines;            // number of lines in the initializer

    uint64  memSpec;          // The specifiers present for this member
    MemberType mtype;         // returns the kind of member
    int maxInitLines;         // when the initializer will be displayed
    int userInitLines;        // result of explicit \hideinitializer or \showinitializer
    MemberDef  *annMemb;

    ArgumentList *defArgList;    // argument list of this member definition
    ArgumentList *declArgList;   // argument list of this member declaration

    ArgumentList *tArgList;      // template argument list of function template
    ArgumentList *typeConstraints; // type constraints for template parameters
    MemberDef *templateMaster;
    QList<ArgumentList> *defTmpArgLists; // lists of template argument lists
                                         // (for template functions in nested template classes)

    ClassDef *cachedAnonymousType; // if the member has an anonymous compound
                                   // as its type then this is computed by
                                   // getClassDefOfAnonymousType() and
                                   // cached here.
    SDict<MemberList> *classSectionSDict; // not accessible

    MemberDef *groupAlias;    // Member containing the definition
    int grpId;                // group id
    MemberGroup *memberGroup; // group's member definition
    GroupDef *group;          // group in which this member is in
    Grouping::GroupPri_t grouppri; // priority of this definition
    QCString groupFileName;   // file where this grouping was defined
    int groupStartLine;       // line  "      "      "     "     "
    MemberDef *groupMember;

    bool isTypedefValCached;
    ClassDef *cachedTypedefValue;
    QCString cachedTypedefTemplSpec;
    QCString cachedResolvedType;

    // inbody documentation
    //int inbodyLine;
    //QCString inbodyFile;
    //QCString inbodyDocs;

    // documentation inheritance
    MemberDef *docProvider;

    // to store the output file base from tag files
    QCString explicitOutputFileBase;

    // objective-c
    bool implOnly; // function found in implementation but not
                     // in the interface
    bool hasDocumentedParams;
    bool hasDocumentedReturnType;
    bool isDMember;
    Relationship related;     // relationship of this to the class
    bool stat;                // is it a static function?
    bool proto;               // is it a prototype;
    bool docEnumValues;       // is an enum with documented enum values.
    bool annScope;            // member is part of an annoymous scope
    bool annUsed;
    bool hasCallGraph;
    bool hasCallerGraph;
    bool explExt;             // member was explicitly declared external
    bool tspec;               // member is a template specialization
    bool groupHasDocs;        // true if the entry that caused the grouping was documented
    bool docsForDefinition;   // TRUE => documentation block is put before
                              //         definition.
                              // FALSE => block is put before declaration.
    ClassDef *category;
    MemberDef *categoryRelation;
};

MemberDefImpl::MemberDefImpl() :
    enumFields(0),
    redefinedBy(0),
    exampleSDict(0),
    defArgList(0),
    declArgList(0),
    tArgList(0),
    typeConstraints(0),
    defTmpArgLists(0),
    classSectionSDict(0),
    category(0),
    categoryRelation(0)
{
}

MemberDefImpl::~MemberDefImpl()
{
  delete redefinedBy;
  delete exampleSDict;
  delete enumFields;
  delete defArgList;
  delete tArgList;
  delete typeConstraints;
  delete defTmpArgLists;
  delete classSectionSDict;
  delete declArgList;
}

void MemberDefImpl::init(Definition *def,
                     const char *t,const char *a,const char *e,
                     Protection p,Specifier v,bool s,Relationship r,
                     MemberType mt,const ArgumentList *tal,
                     const ArgumentList *al
                    )
{
  classDef=0;
  fileDef=0;
  redefines=0;
  relatedAlso=0;
  redefinedBy=0;
  accessorClass=0;
  nspace=0;
  memDef=0;
  memDec=0;
  group=0;
  grpId=-1;
  exampleSDict=0;
  enumFields=0;
  enumScope=0;
  livesInsideEnum=FALSE;
  defTmpArgLists=0;
  hasCallGraph = FALSE;
  hasCallerGraph = FALSE;
  initLines=0;
  type=t;
  if (mt==MemberType_Typedef) type.stripPrefix("typedef ");
  //  type.stripPrefix("struct ");
  //  type.stripPrefix("class " );
  //  type.stripPrefix("union " );
  type=removeRedundantWhiteSpace(type);
  args=a;
  args=removeRedundantWhiteSpace(args);
  if (type.isEmpty()) decl=def->name()+args; else decl=type+" "+def->name()+args;

  memberGroup=0;
  virt=v;
  prot=p;
  related=r;
  stat=s;
  mtype=mt;
  exception=e;
  proto=FALSE;
  annScope=FALSE;
  memSpec=0;
  annMemb=0;
  annUsed=FALSE;
  annEnumType=0;
  groupAlias=0;
  explExt=FALSE;
  tspec=FALSE;
  cachedAnonymousType=0;
  maxInitLines=Config_getInt(MAX_INITIALIZER_LINES);
  userInitLines=-1;
  docEnumValues=FALSE;
  // copy function template arguments (if any)
  if (tal)
  {
    tArgList = tal->deepCopy();
  }
  else
  {
    tArgList=0;
  }
  //printf("new member al=%p\n",al);
  // copy function definition arguments (if any)
  if (al)
  {
    defArgList = al->deepCopy();
  }
  else
  {
    defArgList=0;
  }
  // convert function declaration arguments (if any)
  if (!args.isEmpty())
  {
    declArgList = new ArgumentList;
    stringToArgumentList(args,declArgList,&extraTypeChars);
    //printf("setDeclArgList %s to %s const=%d\n",args.data(),
    //    argListToString(declArgList).data(),declArgList->constSpecifier);
  }
  else
  {
    declArgList = 0;
  }
  templateMaster = 0;
  classSectionSDict = 0;
  docsForDefinition = TRUE;
  isTypedefValCached = FALSE;
  cachedTypedefValue = 0;
  //inbodyLine = -1;
  implOnly=FALSE;
  groupMember = 0;
  hasDocumentedParams = FALSE;
  hasDocumentedReturnType = FALSE;
  docProvider = 0;
  isDMember = def->getDefFileName().right(2).lower()==".d";
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/*! Creates a new member definition.
 *
 * \param df File containing the definition of this member.
 * \param dl Line at which the member definition was found.
 * \param dc Column at which the member definition was found.
 * \param t  A string representing the type of the member.
 * \param na A string representing the name of the member.
 * \param a  A string representing the arguments of the member.
 * \param e  A string representing the throw clause of the members.
 * \param p  The protection context of the member, possible values are:
 *           \c Public, \c Protected, \c Private.
 * \param v  The degree of `virtualness' of the member, possible values are:
 *           \c Normal, \c Virtual, \c Pure.
 * \param s  A boolean that is true iff the member is static.
 * \param r  The relationship between the class and the member.
 * \param mt The kind of member. See #MemberType for a list of
 *           all types.
 * \param tal The template arguments of this member.
 * \param al  The arguments of this member. This is a structured form of
 *            the string past as argument \a a.
 */

MemberDef::MemberDef(const char *df,int dl,int dc,
                     const char *t,const char *na,const char *a,const char *e,
                     Protection p,Specifier v,bool s,Relationship r,MemberType mt,
                     const ArgumentList *tal,const ArgumentList *al
                    ) : Definition(df,dl,dc,removeRedundantWhiteSpace(na)), visited(FALSE)
{
  //printf("MemberDef::MemberDef(%s)\n",na);
  m_impl = new MemberDefImpl;
  m_impl->init(this,t,a,e,p,v,s,r,mt,tal,al);
  m_isLinkableCached    = 0;
  m_isConstructorCached = 0;
  m_isDestructorCached  = 0;
}

MemberDef::MemberDef(const MemberDef &md) : Definition(md), visited(FALSE)
{
  m_impl = new MemberDefImpl;
  m_isLinkableCached    = 0;
  m_isConstructorCached = 0;
  m_isDestructorCached  = 0;
}

MemberDef *MemberDef::deepCopy() const
{
  //MemberDef *result = new MemberDef(getDefFileName(),getDefLine(),name());
  MemberDef *result = new MemberDef(*this);
  // first copy everything by reference
  *result->m_impl = *m_impl;
  // clear pointers owned by object
  result->m_impl->redefinedBy= 0;
  result->m_impl->exampleSDict=0;
  result->m_impl->enumFields=0;
  result->m_impl->defArgList=0;
  result->m_impl->tArgList=0;
  result->m_impl->typeConstraints=0;
  result->m_impl->defTmpArgLists=0;
  result->m_impl->classSectionSDict=0;
  result->m_impl->declArgList=0;
  // replace pointers owned by the object by deep copies
  if (m_impl->redefinedBy)
  {
    MemberListIterator mli(*m_impl->redefinedBy);
    MemberDef *md;
    for (mli.toFirst();(md=mli.current());++mli)
    {
      result->insertReimplementedBy(md);
    }
  }
  if (m_impl->exampleSDict)
  {
    ExampleSDict::Iterator it(*m_impl->exampleSDict);
    Example *e;
    for (it.toFirst();(e=it.current());++it)
    {
      result->addExample(e->anchor,e->name,e->file);
    }
  }
  if (m_impl->enumFields)
  {
    MemberListIterator mli(*m_impl->enumFields);
    MemberDef *md;
    for (mli.toFirst();(md=mli.current());++mli)
    {
      result->insertEnumField(md);
    }
  }
  if (m_impl->defArgList)
  {
    result->m_impl->defArgList = m_impl->defArgList->deepCopy();
  }
  if (m_impl->tArgList)
  {
    result->m_impl->tArgList = m_impl->tArgList->deepCopy();
  }
  if (m_impl->typeConstraints)
  {
    result->m_impl->typeConstraints = m_impl->typeConstraints->deepCopy();
  }
  result->setDefinitionTemplateParameterLists(m_impl->defTmpArgLists);
  if (m_impl->classSectionSDict)
  {
    result->m_impl->classSectionSDict = new SDict<MemberList>(7);
    SDict<MemberList>::IteratorDict it(*m_impl->classSectionSDict);
    MemberList *ml;
    for (it.toFirst();(ml=it.current());++it)
    {
      result->m_impl->classSectionSDict->append(it.currentKey(),ml);
    }
  }
  if (m_impl->declArgList)
  {
    result->m_impl->declArgList = m_impl->declArgList->deepCopy();
  }
  return result;
}

void MemberDef::moveTo(Definition *scope)
{
   setOuterScope(scope);
   if (scope->definitionType()==Definition::TypeClass)
   {
     m_impl->classDef = (ClassDef*)scope;
   }
   else if (scope->definitionType()==Definition::TypeFile)
   {
     m_impl->fileDef = (FileDef*)scope;
   }
   else if (scope->definitionType()==Definition::TypeNamespace)
   {
     m_impl->nspace = (NamespaceDef*)scope;
   }
   m_isLinkableCached = 0;
   m_isConstructorCached = 0;
}


/*! Destroys the member definition. */
MemberDef::~MemberDef()
{
  delete m_impl;
  //printf("%p: ~MemberDef()\n",this);
  m_impl=0;
}

void MemberDef::setReimplements(MemberDef *md)
{
  m_impl->redefines = md;
}

void MemberDef::insertReimplementedBy(MemberDef *md)
{
  if (m_impl->templateMaster)
  {
    m_impl->templateMaster->insertReimplementedBy(md);
  }
  if (m_impl->redefinedBy==0) m_impl->redefinedBy = new MemberList(MemberListType_redefinedBy);
  if (m_impl->redefinedBy->findRef(md)==-1)
  {
    m_impl->redefinedBy->inSort(md);
  }
}

MemberDef *MemberDef::reimplements() const
{
  return m_impl->redefines;
}

MemberList *MemberDef::reimplementedBy() const
{
  return m_impl->redefinedBy;
}

bool MemberDef::isReimplementedBy(ClassDef *cd) const
{
  if (cd && m_impl->redefinedBy)
  {
    MemberListIterator mi(*m_impl->redefinedBy);
    MemberDef *md;
    for (mi.toFirst();(md=mi.current());++mi)
    {
      ClassDef *mcd = md->getClassDef();
      if (mcd)
      {
        if (cd==mcd || cd->isBaseClass(mcd,TRUE))
        {
          return TRUE;
        }
      }
    }
  }
  return FALSE;
}

void MemberDef::insertEnumField(MemberDef *md)
{
  if (m_impl->enumFields==0) m_impl->enumFields=new MemberList(MemberListType_enumFields);
  m_impl->enumFields->append(md);
}

bool MemberDef::addExample(const char *anchor,const char *nameStr,
                           const char *file)
{
  //printf("%s::addExample(%s,%s,%s)\n",name().data(),anchor,nameStr,file);
  if (m_impl->exampleSDict==0) m_impl->exampleSDict = new ExampleSDict;
  if (m_impl->exampleSDict->find(nameStr)==0)
  {
    //printf("Add reference to example %s to member %s\n",nameStr,name.data());
    Example *e=new Example;
    e->anchor=anchor;
    e->name=nameStr;
    e->file=file;
    m_impl->exampleSDict->inSort(nameStr,e);
    return TRUE;
  }
  return FALSE;
}

bool MemberDef::hasExamples()
{
  if (m_impl->exampleSDict==0)
    return FALSE;
  else
    return m_impl->exampleSDict->count()>0;
}

QCString MemberDef::getOutputFileBase() const
{
  static bool separateMemberPages = Config_getBool(SEPARATE_MEMBER_PAGES);
  static bool inlineSimpleClasses = Config_getBool(INLINE_SIMPLE_STRUCTS);
  QCString baseName;

  //printf("Member: %s: templateMaster=%p group=%p classDef=%p nspace=%p fileDef=%p\n",
  //    name().data(),m_impl->templateMaster,m_impl->group,m_impl->classDef,
  //    m_impl->nspace,m_impl->fileDef);
  if (!m_impl->explicitOutputFileBase.isEmpty())
  {
    return m_impl->explicitOutputFileBase;
  }
  else if (m_impl->templateMaster)
  {
    return m_impl->templateMaster->getOutputFileBase();
  }
  else if (m_impl->group)
  {
    baseName=m_impl->group->getOutputFileBase();
  }
  else if (m_impl->classDef)
  {
    baseName=m_impl->classDef->getOutputFileBase();
    if (inlineSimpleClasses && m_impl->classDef->isSimple())
    {
      return baseName;
    }
  }
  else if (m_impl->nspace && m_impl->nspace->isLinkableInProject())
  {
    baseName=m_impl->nspace->getOutputFileBase();
  }
  else if (m_impl->fileDef)
  {
    baseName=m_impl->fileDef->getOutputFileBase();
  }

  if (baseName.isEmpty())
  {
    warn(getDefFileName(),getDefLine(),
       "Internal inconsistency: member %s does not belong to any"
       " container!",qPrint(name())
      );
    return "dummy";
  }
  else if (separateMemberPages && isDetailedSectionLinkable())
  {
    if (getEnumScope()) // enum value, which is part of enum's documentation
    {
      baseName+="_"+getEnumScope()->anchor();
    }
    else
    {
      baseName+="_"+anchor();
    }
  }
  return baseName;
}

QCString MemberDef::getReference() const
{
  QCString ref = Definition::getReference();
  if (!ref.isEmpty())
  {
    return ref;
  }
  if (m_impl->templateMaster)
  {
    return m_impl->templateMaster->getReference();
  }
  else if (m_impl->group)
  {
    return m_impl->group->getReference();
  }
  else if (m_impl->classDef)
  {
    return m_impl->classDef->getReference();
  }
  else if (m_impl->nspace)
  {
    return m_impl->nspace->getReference();
  }
  else if (m_impl->fileDef)
  {
    return m_impl->fileDef->getReference();
  }
  return "";
}

QCString MemberDef::anchor() const
{
  QCString result=m_impl->anc;
  if (m_impl->groupAlias)     return m_impl->groupAlias->anchor();
  if (m_impl->templateMaster) return m_impl->templateMaster->anchor();
  if (m_impl->enumScope && m_impl->enumScope!=this) // avoid recursion for C#'s public enum E { E, F }
  {
    result.prepend(m_impl->enumScope->anchor());
  }
  if (m_impl->group)
  {
    if (m_impl->groupMember)
    {
      result=m_impl->groupMember->anchor();
    }
    else if (getReference().isEmpty())
    {
      result.prepend("g");
    }
  }
  return result;
}

void MemberDef::_computeLinkableInProject()
{
  static bool extractStatic  = Config_getBool(EXTRACT_STATIC);
  m_isLinkableCached = 2; // linkable
  //printf("MemberDef::isLinkableInProject(name=%s)\n",name().data());
  if (isHidden())
  {
    //printf("is hidden\n");
    m_isLinkableCached = 1;
    return;
  }
  if (m_impl->templateMaster)
  {
    //printf("has template master\n");
    m_isLinkableCached = m_impl->templateMaster->isLinkableInProject() ? 2 : 1;
    return;
  }
  if (name().isEmpty() || name().at(0)=='@')
  {
    //printf("name invalid\n");
    m_isLinkableCached = 1; // not a valid or a dummy name
    return;
  }
  if (!hasDocumentation() && !isReference())
  {
    //printf("no docs or reference\n");
    m_isLinkableCached = 1; // no documentation
    return;
  }
  if (m_impl->group && !m_impl->group->isLinkableInProject())
  {
    //printf("group but group not linkable!\n");
    m_isLinkableCached = 1; // group but group not linkable
    return;
  }
  if (!m_impl->group && m_impl->classDef && !m_impl->classDef->isLinkableInProject())
  {
    //printf("in a class but class not linkable!\n");
    m_isLinkableCached = 1; // in class but class not linkable
    return;
  }
  if (!m_impl->group && m_impl->nspace && !m_impl->related && !m_impl->nspace->isLinkableInProject()
      && (m_impl->fileDef==0 || !m_impl->fileDef->isLinkableInProject()))
  {
    //printf("in a namespace but namespace not linkable!\n");
    m_isLinkableCached = 1; // in namespace but namespace not linkable
    return;
  }
  if (!m_impl->group && !m_impl->nspace &&
      !m_impl->related && !m_impl->classDef &&
      m_impl->fileDef && !m_impl->fileDef->isLinkableInProject())
  {
    //printf("in a file but file not linkable!\n");
    m_isLinkableCached = 1; // in file (and not in namespace) but file not linkable
    return;
  }
  if (!protectionLevelVisible(m_impl->prot) && m_impl->mtype!=MemberType_Friend)
  {
    //printf("private and invisible!\n");
    m_isLinkableCached = 1; // hidden due to protection
    return;
  }
  if (m_impl->stat && m_impl->classDef==0 && !extractStatic)
  {
    //printf("static and invisible!\n");
    m_isLinkableCached = 1; // hidden due to staticness
    return;
  }
  //printf("linkable!\n");
  return; // linkable!
}

void MemberDef::setDocumentation(const char *d,const char *docFile,int docLine,bool stripWhiteSpace)
{
  Definition::setDocumentation(d,docFile,docLine,stripWhiteSpace);
  m_isLinkableCached = 0;
}

void MemberDef::setBriefDescription(const char *b,const char *briefFile,int briefLine)
{
  Definition::setBriefDescription(b,briefFile,briefLine);
  m_isLinkableCached = 0;
}

void MemberDef::setInbodyDocumentation(const char *d,const char *inbodyFile,int inbodyLine)
{
  Definition::setInbodyDocumentation(d,inbodyFile,inbodyLine);
  m_isLinkableCached = 0;
}

void MemberDef::setHidden(bool b)
{
  Definition::setHidden(b);
  m_isLinkableCached = 0;
}

bool MemberDef::isLinkableInProject() const
{
  if (m_isLinkableCached==0)
  {
    MemberDef *that = (MemberDef*)this;
    that->_computeLinkableInProject();
  }
  ASSERT(m_isLinkableCached>0);
  return m_isLinkableCached==2;
}

bool MemberDef::isLinkable() const
{
  if (m_impl->templateMaster)
  {
    return m_impl->templateMaster->isLinkable();
  }
  else
  {
    return isLinkableInProject() || isReference();
  }
}


void MemberDef::setDefinitionTemplateParameterLists(QList<ArgumentList> *lists)
{
  if (lists)
  {
    if (m_impl->defTmpArgLists) delete m_impl->defTmpArgLists;
    m_impl->defTmpArgLists = copyArgumentLists(lists);
  }
}

void MemberDef::writeLink(OutputList &ol,ClassDef *,NamespaceDef *,
                      FileDef *fd,GroupDef *gd,bool onlyText)
{
  SrcLangExt lang = getLanguage();
  static bool hideScopeNames     = Config_getBool(HIDE_SCOPE_NAMES);
  QCString sep = getLanguageSpecificSeparator(lang,TRUE);
  QCString n = name();
  if (!hideScopeNames)
  {
    if (m_impl->enumScope && m_impl->livesInsideEnum)
    {
      n.prepend(m_impl->enumScope->displayName()+sep);
    }
    if (m_impl->classDef && gd && !isRelated())
    {
      n.prepend(m_impl->classDef->displayName()+sep);
    }
    else if (m_impl->nspace && (gd || fd))
    {
      n.prepend(m_impl->nspace->displayName()+sep);
    }
  }

  if (isObjCMethod())
  {
    if (isStatic()) ol.docify("+ "); else ol.docify("- ");
  }
  if (!onlyText && isLinkable()) // write link
  {
    if (m_impl->mtype==MemberType_EnumValue && getGroupDef()==0 &&          // enum value is not grouped
        getEnumScope() && getEnumScope()->getGroupDef()) // but its container is
    {
      GroupDef *enumValGroup = getEnumScope()->getGroupDef();
      ol.writeObjectLink(enumValGroup->getReference(),
                         enumValGroup->getOutputFileBase(),
                         anchor(),n);
    }
    else
    {
      ol.writeObjectLink(getReference(),getOutputFileBase(),anchor(),n);
    }
  }
  else // write only text
  {
    ol.startBold();
    ol.docify(n);
    ol.endBold();
  }
}

/*! If this member has an anonymous class/struct/union as its type, then
 *  this method will return the ClassDef that describes this return type.
 */
ClassDef *MemberDef::getClassDefOfAnonymousType()
{
  if (m_impl->cachedAnonymousType) return m_impl->cachedAnonymousType;

  QCString cname;
  if (getClassDef()!=0)
  {
    cname=getClassDef()->name().copy();
  }
  else if (getNamespaceDef()!=0)
  {
    cname=getNamespaceDef()->name().copy();
  }
  QCString ltype(m_impl->type);
  // strip `static' keyword from ltype
  //if (ltype.left(7)=="static ") ltype=ltype.right(ltype.length()-7);
  // strip `friend' keyword from ltype
  ltype.stripPrefix("friend ");
  static QRegExp r("@[0-9]+");
  int l,i=r.match(ltype,0,&l);
  //printf("ltype=`%s' i=%d\n",ltype.data(),i);
  // search for the last anonymous scope in the member type
  ClassDef *annoClassDef=0;
  if (i!=-1) // found anonymous scope in type
  {
    int il=i-1,ir=i+l;
    // extract anonymous scope
    while (il>=0 && (isId(ltype.at(il)) || ltype.at(il)==':' || ltype.at(il)=='@')) il--;
    if (il>0) il++; else if (il<0) il=0;
    while (ir<(int)ltype.length() && (isId(ltype.at(ir)) || ltype.at(ir)==':' || ltype.at(ir)=='@')) ir++;

    QCString annName = ltype.mid(il,ir-il);

    // if inside a class or namespace try to prepend the scope name
    if (!cname.isEmpty() && annName.left(cname.length()+2)!=cname+"::")
    {
      QCString ts=stripAnonymousNamespaceScope(cname+"::"+annName);
      annoClassDef=getClass(ts);
    }
    // if not found yet, try without scope name
    if (annoClassDef==0)
    {
      QCString ts=stripAnonymousNamespaceScope(annName);
      annoClassDef=getClass(ts);
    }
  }
  m_impl->cachedAnonymousType = annoClassDef;
  return annoClassDef;
}

/*! This methods returns TRUE iff the brief section (also known as
 *  declaration section) is visible in the documentation.
 */
bool MemberDef::isBriefSectionVisible() const
{
  static bool extractStatic       = Config_getBool(EXTRACT_STATIC);
  static bool hideUndocMembers    = Config_getBool(HIDE_UNDOC_MEMBERS);
  static bool briefMemberDesc     = Config_getBool(BRIEF_MEMBER_DESC);
  static bool repeatBrief         = Config_getBool(REPEAT_BRIEF);
  static bool hideFriendCompounds = Config_getBool(HIDE_FRIEND_COMPOUNDS);

  //printf("Member %s grpId=%d docs=%s file=%s args=%s\n",
  //    name().data(),
  //    0,"", //grpId,grpId==-1?"<none>":Doxygen::memberDocDict[grpId]->data(),
  //    "", //getFileDef()->name().data(),
  //    argsString());

  MemberGroupInfo *info = Doxygen::memGrpInfoDict[m_impl->grpId];
  //printf("name=%s m_impl->grpId=%d info=%p\n",name().data(),m_impl->grpId,info);
  //QCString *pMemGrp = Doxygen::memberDocDict[grpId];
  bool hasDocs = hasDocumentation() ||
                  // part of a documented member group
                 (m_impl->grpId!=-1 && info && !(info->doc.isEmpty() && info->header.isEmpty()));

  // only include static members with file/namespace scope if
  // explicitly enabled in the config file
  bool visibleIfStatic = !(getClassDef()==0 &&
                           isStatic() &&
                           !extractStatic
                          );

  // only include members is the are documented or
  // HIDE_UNDOC_MEMBERS is NO in the config file
  bool visibleIfDocumented = (!hideUndocMembers ||
                              hasDocs ||
                              isDocumentedFriendClass()
                             );

  // hide members with no detailed description and brief descriptions
  // explicitly disabled.
  bool visibleIfEnabled = !(hideUndocMembers &&
                            documentation().isEmpty() &&
                            !briefMemberDesc &&
                            !repeatBrief
                           );

  // Hide friend (class|struct|union) declarations if HIDE_FRIEND_COMPOUNDS is true
  bool visibleIfFriendCompound = !(hideFriendCompounds &&
                                   isFriend() &&
                                   (m_impl->type=="friend class" ||
                                    m_impl->type=="friend struct" ||
                                    m_impl->type=="friend union"
                                   )
                                  );

  // only include members that are non-private unless EXTRACT_PRIVATE is
  // set to YES or the member is part of a group
  bool visibleIfPrivate = (protectionLevelVisible(protection()) ||
                           m_impl->mtype==MemberType_Friend
                          );

  // hide member if it overrides a member in a superclass and has no
  // documentation of its own
  //bool visibleIfDocVirtual = !reimplements() ||
  //                           !Config_getBool(INHERIT_DOCS) ||
  //                           hasDocs;

  // true if this member is a constructor or destructor
  bool cOrDTor = isConstructor() || isDestructor();

  // hide default constructors or destructors (no args) without
  // documentation
  bool visibleIfNotDefaultCDTor = !(cOrDTor &&
                                   m_impl->defArgList &&
                                   (m_impl->defArgList->isEmpty() ||
                                    m_impl->defArgList->getFirst()->type == "void"
                                   ) &&
                                   !hasDocs
                                  );


  //printf("visibleIfStatic=%d visibleIfDocumented=%d visibleIfEnabled=%d "
  //       "visibleIfPrivate=%d visibltIfNotDefaultCDTor=%d "
  //       "visibleIfFriendCompound=%d !annScope=%d\n",
  //       visibleIfStatic,visibleIfDocumented,
  //       visibleIfEnabled,visibleIfPrivate,visibleIfNotDefaultCDTor,
  //       visibleIfFriendCompound,!m_impl->annScope);

  bool visible = visibleIfStatic     && visibleIfDocumented      &&
                 visibleIfEnabled    && visibleIfPrivate         &&
                 /*visibleIfDocVirtual &&*/ visibleIfNotDefaultCDTor &&
                 visibleIfFriendCompound &&
                 !m_impl->annScope && !isHidden();
  //printf("MemberDef::isBriefSectionVisible() %d\n",visible);
  return visible;
}

QCString MemberDef::getDeclType() const
{
  QCString ltype(m_impl->type);
  if (m_impl->mtype==MemberType_Typedef)
  {
    ltype.prepend("typedef ");
  }
  if (isAlias())
  {
    ltype="using";
  }
  // strip `friend' keyword from ltype
  ltype.stripPrefix("friend ");
  if (ltype=="@") // rename type from enum values
  {
    ltype="";
  }
  else
  {
    if (isObjCMethod())
    {
      ltype.prepend("(");
      ltype.append(")");
    }
  }
  return ltype;
}

void MemberDef::writeDeclaration(OutputList &ol,
               ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd,
               bool inGroup, ClassDef *inheritedFrom,const char *inheritId)
{
  //printf("%s MemberDef::writeDeclaration() inGroup=%d\n",qualifiedName().data(),inGroup);

  // hide enum value, since they appear already as part of the enum, unless they
  // are explicitly grouped.
  if (!inGroup && m_impl->mtype==MemberType_EnumValue) return;


  Definition *d=0;
  ASSERT (cd!=0 || nd!=0 || fd!=0 || gd!=0); // member should belong to something
  if (cd) d=cd; else if (nd) d=nd; else if (fd) d=fd; else d=gd;
  if (d==gd) // see bug 753608
  {
    if (getClassDef())          d = getClassDef();
    else if (getNamespaceDef()) d = getNamespaceDef();
    else if (getFileDef())      d = getFileDef();
  }

  //_writeTagData(compoundType);
  _addToSearchIndex();

  QCString cname  = d->name();
  QCString cdname = d->displayName();
  QCString cfname = getOutputFileBase();

  // search for the last anonymous scope in the member type
  ClassDef *annoClassDef=getClassDefOfAnonymousType();

  ol.startMemberDeclaration();

  // start a new member declaration
  bool isAnonymous = annoClassDef || m_impl->annMemb || m_impl->annEnumType;
  ///printf("startMemberItem for %s\n",name().data());
  ol.startMemberItem(anchor(),
                     isAnonymous ? 1 : m_impl->tArgList ? 3 : 0,
                     inheritId
                    );

  // If there is no detailed description we need to write the anchor here.
  bool detailsVisible = isDetailedSectionLinkable();
  bool writeAnchor = (inGroup || m_impl->group==0) &&     // only write anchors for member that have no details and are
                     !detailsVisible && !m_impl->annMemb; // rendered inside the group page or are not grouped at all
  if (writeAnchor)
  {
    QCString doxyArgs=argsString();
    QCString doxyName=name();
    if (!cname.isEmpty())
    {
      doxyName.prepend(cdname+getLanguageSpecificSeparator(getLanguage()));
    }
    ol.startDoxyAnchor(cfname,cname,anchor(),doxyName,doxyArgs);
  }

  if (!detailsVisible)
  {
    ol.pushGeneratorState();
    ol.disable(OutputGenerator::Man);
    ol.disable(OutputGenerator::Latex);
    ol.docify("\n");
    ol.popGeneratorState();
  }

  if (annoClassDef || m_impl->annMemb)
  {
    int j;
    for (j=0;j<s_indentLevel;j++)
    {
      ol.writeNonBreakableSpace(3);
    }
  }

  // *** write template lists
  if (m_impl->tArgList && getLanguage()==SrcLangExt_Cpp)
  {
    if (!isAnonymous) ol.startMemberTemplateParams();
    writeTemplatePrefix(ol,m_impl->tArgList);
    if (!isAnonymous) ol.endMemberTemplateParams(anchor(),inheritId);
  }

  // *** write type
  QCString ltype(m_impl->type);
  if (m_impl->mtype==MemberType_Typedef) ltype.prepend("typedef ");
  if (isAlias())
  {
    ltype="using";
  }
  // strip `friend' keyword from ltype
  ltype.stripPrefix("friend ");
  static QRegExp r("@[0-9]+");

  bool endAnonScopeNeeded=FALSE;
  int l,i=r.match(ltype,0,&l);
  if (i!=-1) // member has an anonymous type
  {
    //printf("annoClassDef=%p annMemb=%p scopeName=`%s' anonymous=`%s'\n",
    //    annoClassDef,annMemb,cname.data(),ltype.mid(i,l).data());

    if (annoClassDef) // type is an anonymous compound
    {
      int ir=i+l;
      //printf("<<<<<<<<<<<<<<\n");
      ol.startAnonTypeScope(s_indentLevel++);
      annoClassDef->writeDeclaration(ol,m_impl->annMemb,inGroup,inheritedFrom,inheritId);
      //printf(">>>>>>>>>>>>>> startMemberItem(2)\n");
      ol.startMemberItem(anchor(),2,inheritId);
      int j;
      for (j=0;j< s_indentLevel-1;j++)
      {
        ol.writeNonBreakableSpace(3);
      }
      QCString varName=ltype.right(ltype.length()-ir).stripWhiteSpace();
      //printf(">>>>>> ltype=`%s' varName=`%s'\n",ltype.data(),varName.data());
      ol.docify("}");
      if (varName.isEmpty() && (name().isEmpty() || name().at(0)=='@'))
      {
        ol.docify(";");
      }
      else if (!varName.isEmpty() && (varName.at(0)=='*' || varName.at(0)=='&'))
      {
        ol.docify(" ");
        ol.docify(varName);
      }
      endAnonScopeNeeded=TRUE;
    }
    else
    {
      if (getAnonymousEnumType()) // type is an anonymous enum
      {
        linkifyText(TextGeneratorOLImpl(ol), // out
                    d,                       // scope
                    getBodyDef(),            // fileScope
                    this,                    // self
                    ltype.left(i),           // text
                    FALSE                    // autoBreak
                   );
        getAnonymousEnumType()->writeEnumDeclaration(ol,cd,nd,fd,gd);
        //ol+=*getAnonymousEnumType()->enumDecl();
        linkifyText(TextGeneratorOLImpl(ol),d,m_impl->fileDef,this,ltype.right(ltype.length()-i-l),TRUE);
      }
      else
      {
        ltype = ltype.left(i) + " { ... } " + removeAnonymousScopes(ltype.right(ltype.length()-i-l));
        linkifyText(TextGeneratorOLImpl(ol), // out
                    d,                       // scope
                    getBodyDef(),            // fileScope
                    this,                    // self
                    ltype,                   // text
                    FALSE                    // autoBreak
                   );
      }
    }
  }
  else if (ltype=="@") // rename type from enum values
  {
    ltype="";
  }
  else
  {
    if (isObjCMethod())
    {
      ltype.prepend("(");
      ltype.append(")");
    }
    linkifyText(TextGeneratorOLImpl(ol), // out
                d,                       // scope
                getBodyDef(),            // fileScope
                this,                    // self
                ltype,                   // text
                FALSE                    // autoBreak
               );
  }
  bool htmlOn = ol.isEnabled(OutputGenerator::Html);
  if (htmlOn && !ltype.isEmpty())
  {
    ol.disable(OutputGenerator::Html);
  }
  if (!ltype.isEmpty()) ol.docify(" ");
  if (htmlOn)
  {
    ol.enable(OutputGenerator::Html);
  }

  if (m_impl->annMemb)
  {
    ol.pushGeneratorState();
    ol.disableAllBut(OutputGenerator::Html);
    ol.writeNonBreakableSpace(3);
    ol.popGeneratorState();
  }
  else
  {
    ol.insertMemberAlign(m_impl->tArgList!=0);
  }

  // *** write name
  if (!name().isEmpty() && name().at(0)!='@') // hide anonymous stuff
  {
    static bool extractPrivate = Config_getBool(EXTRACT_PRIVATE);
    static bool extractStatic  = Config_getBool(EXTRACT_STATIC);
    //printf("Member name=`%s gd=%p md->groupDef=%p inGroup=%d isLinkable()=%d hasDocumentation=%d\n",name().data(),gd,getGroupDef(),inGroup,isLinkable(),hasDocumentation());
    if (!(name().isEmpty() || name().at(0)=='@') && // name valid
        (hasDocumentation() || isReference()) && // has docs
        !(m_impl->prot==Private && !extractPrivate && m_impl->mtype!=MemberType_Friend) && // hidden due to protection
        !(isStatic() && m_impl->classDef==0 && !extractStatic) // hidden due to static-ness
       )
    {
      if (m_impl->annMemb)
      {
        //printf("anchor=%s ann_anchor=%s\n",anchor(),annMemb->anchor());
        m_impl->annMemb->writeLink(ol,
            m_impl->annMemb->getClassDef(),
            m_impl->annMemb->getNamespaceDef(),
            m_impl->annMemb->getFileDef(),
            m_impl->annMemb->getGroupDef()
                          );
        m_impl->annMemb->setAnonymousUsed();
        setAnonymousUsed();
      }
      else
      {
        //printf("writeLink %s->%d\n",name.data(),hasDocumentation());
        ClassDef *rcd = cd;
        if (isReference() && m_impl->classDef) rcd = m_impl->classDef;
        writeLink(ol,rcd,nd,fd,gd);
      }
    }
    else if (isDocumentedFriendClass())
      // if the member is an undocumented friend declaration for some class,
      // then maybe we can link to the class
    {
      writeLink(ol,getClass(name()),0,0,0);
    }
    else
      // there is a brief member description and brief member
      // descriptions are enabled or there is no detailed description.
    {
      if (m_impl->annMemb)
      {
        m_impl->annMemb->setAnonymousUsed();
        setAnonymousUsed();
      }
      ClassDef *rcd = cd;
      if (isReference() && m_impl->classDef) rcd = m_impl->classDef;
      writeLink(ol,rcd,nd,fd,gd,TRUE);
    }
  }

  // add to index
  if (isEnumerate() && name().at(0)=='@')
  {
    // don't add to index
  }
  else // index member
  {
    //static bool separateMemPages = Config_getBool(SEPARATE_MEMBER_PAGES);
    //QCString cfname = getOutputFileBase();
    //QCString cfiname = d->getOutputFileBase();
    //Doxygen::indexList->addIndexItem(
    //    cname,                                 // level1
    //    name(),                                // level2
    //    separateMemPages ? cfname : cfiname,   // contRef
    //    cfname,                                // memRef
    //    anchor(),                              // anchor
    //    this);                                 // memberdef
    Doxygen::indexList->addIndexItem(d,this);
  }

  // *** write arguments
  if (argsString() && !isObjCMethod())
  {
    if (!isDefine() && !isTypedef()) ol.writeString(" ");
    linkifyText(TextGeneratorOLImpl(ol), // out
                d,                       // scope
                getBodyDef(),            // fileScope
                this,                    // self
                isDefine() ?
                   (const char*)substitute(argsString(),",",", ") :
                isTypedef() ?
                   (const char*)substitute(argsString(),")(",") (") :
                   argsString(),         // text
                m_impl->annMemb,         // autoBreak
                TRUE,                    // external
                FALSE,                   // keepSpaces
                s_indentLevel
               );
  }
  // *** write exceptions
  if (excpString())
  {
    ol.writeString(" ");
    ol.docify(excpString());
  }

  // *** write bitfields
  if (!m_impl->bitfields.isEmpty()) // add bitfields
  {
    linkifyText(TextGeneratorOLImpl(ol),d,getBodyDef(),this,m_impl->bitfields);
  }
  else if (hasOneLineInitializer()
      //!init.isEmpty() && initLines==0 && // one line initializer
      //((maxInitLines>0 && userInitLines==-1) || userInitLines>0) // enabled by default or explicitly
          ) // add initializer
  {
    if (!isDefine())
    {
      //ol.writeString(" = ");
      ol.writeString(" ");
      linkifyText(TextGeneratorOLImpl(ol),d,getBodyDef(),this,m_impl->initializer.simplifyWhiteSpace());
    }
    else
    {
      ol.writeNonBreakableSpace(3);
      linkifyText(TextGeneratorOLImpl(ol),d,getBodyDef(),this,m_impl->initializer);
    }
  }
  else if (isAlias()) // using template alias
  {
    ol.writeString(" = ");
    linkifyText(TextGeneratorOLImpl(ol),d,getBodyDef(),this,m_impl->type);
  }


  if ((isObjCMethod() || isObjCProperty()) && isImplementation())
  {
    ol.startTypewriter();
    ol.docify(" [implementation]");
    ol.endTypewriter();
  }
  
  bool extractPrivate = Config_getBool(EXTRACT_PRIVATE);

  if (isProperty() && (isSettable() || isGettable() ||
      isPrivateSettable() || isPrivateGettable() ||
      isProtectedSettable() || isProtectedGettable()))
  {
      ol.writeLatexSpacing();
      ol.startTypewriter();
      ol.docify(" [");
      QStrList sl;
      
      if (isGettable())             sl.append("get");
      if (isProtectedGettable())    sl.append("protected get");
      if (isSettable())             sl.append("set");
      if (isProtectedSettable())    sl.append("protected set");
      if (extractPrivate)
      {
        if (isPrivateGettable())    sl.append("private get");
        if (isPrivateSettable())    sl.append("private set");
      }
      const char *s=sl.first();
      while (s)
      {
         ol.docify(s);
         s=sl.next();
         if (s) ol.docify(", ");
      }
      ol.docify("]");
      ol.endTypewriter();
  }

  if (isEvent() && (isAddable() || isRemovable() || isRaisable()))
  {
      ol.writeLatexSpacing();
      ol.startTypewriter();
      ol.docify(" [");
      QStrList sl;
      if (isAddable())   sl.append("add");
      if (isRemovable()) sl.append("remove");
      if (isRaisable())  sl.append("raise");
      const char *s=sl.first();
      while (s)
      {
         ol.docify(s);
         s=sl.next();
         if (s) ol.docify(", ");
      }
      ol.docify("]");
      ol.endTypewriter();
  }

  if (writeAnchor)
  {
    ol.endDoxyAnchor(cfname,anchor());
  }

  //printf("endMember %s annoClassDef=%p annEnumType=%p\n",
  //    name().data(),annoClassDef,annEnumType);
  ol.endMemberItem();
  if (endAnonScopeNeeded)
  {
    ol.endAnonTypeScope(--s_indentLevel);
  }

  // write brief description
  if (!briefDescription().isEmpty() &&
      Config_getBool(BRIEF_MEMBER_DESC)
      /* && !annMemb */
     )
  {
    DocRoot *rootNode = validatingParseDoc(briefFile(),briefLine(),
                getOuterScope()?getOuterScope():d,this,briefDescription(),
                TRUE,FALSE,0,TRUE,FALSE);

    if (rootNode && !rootNode->isEmpty())
    {
      ol.startMemberDescription(anchor(),inheritId);
      ol.writeDoc(rootNode,getOuterScope()?getOuterScope():d,this);
      if (detailsVisible)
      {
        static bool separateMemberPages = Config_getBool(SEPARATE_MEMBER_PAGES);
        ol.pushGeneratorState();
        ol.disableAllBut(OutputGenerator::Html);
        //ol.endEmphasis();
        ol.docify(" ");
        if (inheritedFrom ||
            separateMemberPages ||
            (m_impl->group!=0 && gd==0) ||
            (m_impl->nspace!=0 && nd==0)
           ) // forward link to the page or group or namespace
        {
          ol.startTextLink(getOutputFileBase(),anchor());
        }
        else // local link
        {
          ol.startTextLink(0,anchor());
        }
        ol.parseText(theTranslator->trMore());
        ol.endTextLink();
        //ol.startEmphasis();
        ol.popGeneratorState();
      }
      // for RTF we need to add an extra empty paragraph
      ol.pushGeneratorState();
      ol.disableAllBut(OutputGenerator::RTF);
      ol.startParagraph();
      ol.endParagraph();
      ol.popGeneratorState();
      ol.endMemberDescription();
    }
    delete rootNode;
  }

  ol.endMemberDeclaration(anchor(),inheritId);

  warnIfUndocumented();
}

bool MemberDef::isDetailedSectionLinkable() const
{
  static bool extractAll        = Config_getBool(EXTRACT_ALL);
  static bool alwaysDetailedSec = Config_getBool(ALWAYS_DETAILED_SEC);
  static bool repeatBrief       = Config_getBool(REPEAT_BRIEF);
  static bool briefMemberDesc   = Config_getBool(BRIEF_MEMBER_DESC);
  static bool hideUndocMembers  = Config_getBool(HIDE_UNDOC_MEMBERS);
  static bool extractStatic     = Config_getBool(EXTRACT_STATIC);

  // the member has details documentation for any of the following reasons
  bool docFilter =
         // treat everything as documented
         extractAll ||
         // has detailed docs
         !documentation().isEmpty() ||
         // has inbody docs
         !inbodyDocumentation().isEmpty() ||
         // is an enum with values that are documented
         (m_impl->mtype==MemberType_Enumeration && m_impl->docEnumValues) ||
         // is documented enum value
         (m_impl->mtype==MemberType_EnumValue && !briefDescription().isEmpty()) ||
         // has brief description that is part of the detailed description
         (!briefDescription().isEmpty() &&           // has brief docs
          (alwaysDetailedSec &&                      // they are visible in
           (repeatBrief ||                           // detailed section or
            !briefMemberDesc                         // they are explicitly not
           )                                         // shown in brief section
          )
         ) ||
         // has a multi-line initialization block
         //(initLines>0 && initLines<maxInitLines) ||
         (hasMultiLineInitializer() && !hideUndocMembers) ||
         // has one or more documented arguments
         (m_impl->defArgList!=0 && m_impl->defArgList->hasDocumentation()) ||
         // is an attribute or property - need to display that tag
         (m_impl->memSpec & (Entry::Attribute|Entry::Property)) ||
         // has user comments
         Doxygen::userComments
         ;

  // this is not a global static or global statics should be extracted
  bool staticFilter = getClassDef()!=0 || !isStatic() || extractStatic;

  // only include members that are non-private unless EXTRACT_PRIVATE is
  // set to YES or the member is part of a   group
  bool privateFilter = protectionLevelVisible(protection()) || m_impl->mtype==MemberType_Friend;

  // member is part of an anonymous scope that is the type of
  // another member in the list.
  //
  //bool inAnonymousScope = !briefDescription().isEmpty() && annUsed;

  // hide friend (class|struct|union) member if HIDE_FRIEND_COMPOUNDS
  // is true
  bool friendCompoundFilter = !(Config_getBool(HIDE_FRIEND_COMPOUNDS) &&
                                isFriend() &&
                                (m_impl->type=="friend class" ||
                                 m_impl->type=="friend struct" ||
                                 m_impl->type=="friend union"
                                )
                               );


  bool result = ((docFilter && staticFilter && privateFilter && friendCompoundFilter && !isHidden()));
  //printf("%s::isDetailedSectionLinkable: %d\n",name().data(),result);
  return result;
}

bool MemberDef::isDetailedSectionVisible(bool inGroup,bool inFile) const
{
  static bool separateMemPages = Config_getBool(SEPARATE_MEMBER_PAGES);
  static bool inlineSimpleStructs = Config_getBool(INLINE_SIMPLE_STRUCTS);
  static bool hideUndocMembers = Config_getBool(HIDE_UNDOC_MEMBERS);
  bool groupFilter = getGroupDef()==0 || inGroup || separateMemPages;
  bool fileFilter  = getNamespaceDef()==0 || !getNamespaceDef()->isLinkable() || !inFile;
  bool simpleFilter = (hasBriefDescription() || !hideUndocMembers) && inlineSimpleStructs &&
                      getClassDef()!=0 && getClassDef()->isSimple();

  bool visible = isDetailedSectionLinkable() && groupFilter && fileFilter &&
                 !isReference();
  bool result = visible || simpleFilter;
  //printf("%s::isDetailedSectionVisble: %d groupFilter=%d fileFilter=%d\n",
  //    name().data(),result,groupFilter,fileFilter);
  return result;
}

void MemberDef::getLabels(QStrList &sl,Definition *container) const
{
  static bool inlineInfo = Config_getBool(INLINE_INFO);

  Specifier lvirt=virtualness();
  if ((!isObjCMethod() || isOptional() || isRequired()) &&
      (protection()!=Public || lvirt!=Normal ||
       isFriend() || isRelated() ||
       (isInline() && inlineInfo) ||
       isSignal() || isSlot() ||
       isStatic() ||
       (m_impl->classDef && m_impl->classDef!=container && container->definitionType()==TypeClass) ||
       (m_impl->memSpec & ~Entry::Inline)!=0
      )
     )
  {
    // write the member specifier list
    //ol.writeLatexSpacing();
    //ol.startTypewriter();
    //ol.docify(" [");
    SrcLangExt lang = getLanguage();
    bool optVhdl = lang==SrcLangExt_VHDL;
    bool extractPrivate = Config_getBool(EXTRACT_PRIVATE);
    if (optVhdl)
    {
      sl.append(VhdlDocGen::trTypeString(getMemberSpecifiers()));
    }
    else
    {
      if (isFriend()) sl.append("friend");
      else if (isRelated()) sl.append("related");
      else
      {
        if      (Config_getBool(INLINE_INFO) && isInline()) sl.append("inline");
        if      (isExplicit())            sl.append("explicit");
        if      (isMutable())             sl.append("mutable");
        if      (isStatic())              sl.append("static");
        if      (isGettable())            sl.append("get");
        if      (isProtectedGettable())   sl.append("protected get");
        if      (isSettable())            sl.append("set");
        if      (isProtectedSettable())   sl.append("protected set");
        if (extractPrivate)
        {
          if    (isPrivateGettable())     sl.append("private get");
          if    (isPrivateSettable())     sl.append("private set");
        }
        if      (isAddable())             sl.append("add");
        if      (!isUNOProperty() && isRemovable()) sl.append("remove");
        if      (isRaisable())            sl.append("raise");
        if      (isReadable())            sl.append("read");
        if      (isWritable())            sl.append("write");
        if      (isFinal())               sl.append("final");
        if      (isAbstract())            sl.append("abstract");
        if      (isOverride())            sl.append("override");
        if      (isInitonly())            sl.append("initonly");
        if      (isSealed())              sl.append("sealed");
        if      (isNew())                 sl.append("new");
        if      (isOptional())            sl.append("optional");
        if      (isRequired())            sl.append("required");

        if      (isNonAtomic())           sl.append("nonatomic");
        else if (isObjCProperty())        sl.append("atomic");

        // mutual exclusive Objective 2.0 property attributes
        if      (isAssign())              sl.append("assign");
        else if (isCopy())                sl.append("copy");
        else if (isRetain())              sl.append("retain");
        else if (isWeak())                sl.append("weak");
        else if (isStrong())              sl.append("strong");
        else if (isUnretained())          sl.append("unsafe_unretained");

        if (!isObjCMethod())
        {
          if      (protection()==Protected) sl.append("protected");
          else if (protection()==Private)   sl.append("private");
          else if (protection()==Package)   sl.append("package");

          if      (lvirt==Virtual)          sl.append("virtual");
          else if (lvirt==Pure)             sl.append("pure virtual");
          if      (isSignal())              sl.append("signal");
          if      (isSlot())                sl.append("slot");
          if      (isDefault())             sl.append("default");
          if      (isDelete())              sl.append("delete");
          if      (isNoExcept())            sl.append("noexcept");
          if      (isAttribute())           sl.append("attribute");
          if      (isUNOProperty())         sl.append("property");
          if      (isReadonly())            sl.append("readonly");
          if      (isBound())               sl.append("bound");
          if      (isUNOProperty() && isRemovable()) sl.append("removable");
          if      (isConstrained())         sl.append("constrained");
          if      (isTransient())           sl.append("transient");
          if      (isMaybeVoid())           sl.append("maybevoid");
          if      (isMaybeDefault())        sl.append("maybedefault");
          if      (isMaybeAmbiguous())      sl.append("maybeambiguous");
          if      (isPublished())           sl.append("published"); // enum
        }
        if (isObjCProperty() && isImplementation())
        {
          sl.append("implementation");
        }
      }
      if (m_impl->classDef &&
          container->definitionType()==TypeClass &&
          m_impl->classDef!=container &&
          !isRelated()
         )
      {
        sl.append("inherited");
      }
    }
  }
  else if (isObjCMethod() && isImplementation())
  {
    sl.append("implementation");
  }
}

void MemberDef::_writeCallGraph(OutputList &ol)
{
  // write call graph
  if (m_impl->hasCallGraph
      && (isFunction() || isSlot() || isSignal()) && Config_getBool(HAVE_DOT)
     )
  {
    DotCallGraph callGraph(this,FALSE);
    if (callGraph.isTooBig())
    {
       warn_uncond("Call graph for '%s' not generated, too many nodes. Consider increasing DOT_GRAPH_MAX_NODES.\n",qPrint(qualifiedName()));
    }
    else if (!callGraph.isTrivial())
    {
      msg("Generating call graph for function %s\n",qPrint(qualifiedName()));
      ol.disable(OutputGenerator::Man);
      ol.startParagraph();
      ol.startCallGraph();
      ol.parseText(theTranslator->trCallGraph());
      ol.endCallGraph(callGraph);
      ol.endParagraph();
      ol.enableAll();
    }
  }
}

void MemberDef::_writeCallerGraph(OutputList &ol)
{
  if (m_impl->hasCallerGraph
      && (isFunction() || isSlot() || isSignal()) && Config_getBool(HAVE_DOT)
     )
  {
    DotCallGraph callerGraph(this, TRUE);
    if (callerGraph.isTooBig())
    {
       warn_uncond("Caller graph for '%s' not generated, too many nodes. Consider increasing DOT_GRAPH_MAX_NODES.\n",qPrint(qualifiedName()));
    }
    else if (!callerGraph.isTrivial() && !callerGraph.isTooBig())
    {
      msg("Generating caller graph for function %s\n",qPrint(qualifiedName()));
      ol.disable(OutputGenerator::Man);
      ol.startParagraph();
      ol.startCallGraph();
      ol.parseText(theTranslator->trCallerGraph());
      ol.endCallGraph(callerGraph);
      ol.endParagraph();
      ol.enableAll();
    }
  }
}

void MemberDef::_writeReimplements(OutputList &ol)
{
  MemberDef *bmd=reimplements();
  ClassDef *bcd=0;
  if (bmd && (bcd=bmd->getClassDef()))
  {
    // write class that contains a member that is reimplemented by this one
    if (bcd->isLinkable())
    {
      ol.startParagraph();
      QCString reimplFromLine;
      if (bmd->virtualness()!=Pure && bcd->compoundType()!=ClassDef::Interface)
      {
        reimplFromLine = theTranslator->trReimplementedFromList(1);
      }
      else
      {
        reimplFromLine = theTranslator->trImplementedFromList(1);
      }
      int markerPos = reimplFromLine.find("@0");
      if (markerPos!=-1) // should always pass this.
      {
        ol.parseText(reimplFromLine.left(markerPos)); //text left from marker
        if (bmd->isLinkable()) // replace marker with link
        {
          //Definition *bd=bmd->group;
          //if (bd==0) bd=bcd;
          ol.writeObjectLink(bmd->getReference(),bmd->getOutputFileBase(),
              bmd->anchor(),bcd->displayName());

          //ol.writeObjectLink(bcd->getReference(),bcd->getOutputFileBase(),
          //    bmd->anchor(),bcd->name());
          if ( bmd->isLinkableInProject() )
          {
            writePageRef(ol,bmd->getOutputFileBase(),bmd->anchor());
          }
        }
        else
        {
          ol.writeObjectLink(bcd->getReference(),bcd->getOutputFileBase(),
              0,bcd->displayName());
          if (bcd->isLinkableInProject()/* && !Config_getBool(PDF_HYPERLINKS)*/ )
          {
            writePageRef(ol,bcd->getOutputFileBase(),bcd->anchor());
          }
        }
        ol.parseText(reimplFromLine.right(
              reimplFromLine.length()-markerPos-2)); // text right from marker

      }
      else
      {
        err("translation error: no marker in trReimplementsFromList()\n");
      }
      ol.endParagraph();
    }
  }
}

void MemberDef::_writeReimplementedBy(OutputList &ol)
{
  MemberList *bml=reimplementedBy();
  if (bml)
  {
    MemberListIterator mli(*bml);
    MemberDef *bmd=0;
    uint count=0;
    ClassDef *bcd=0;
    for (mli.toFirst();(bmd=mli.current()) && (bcd=bmd->getClassDef());++mli)
    {
      // count the members that directly inherit from md and for
      // which the member and class are visible in the docs.
      if ( bmd->isLinkable() && bcd->isLinkable() )
      {
        count++;
      }
    }
    if (count>0)
    {
      mli.toFirst();
      // write the list of classes that overwrite this member
      ol.startParagraph();

      QCString reimplInLine;
      if (m_impl->virt==Pure || (m_impl->classDef && m_impl->classDef->compoundType()==ClassDef::Interface))
      {
        reimplInLine = theTranslator->trImplementedInList(count);
      }
      else
      {
        reimplInLine = theTranslator->trReimplementedInList(count);
      }
      static QRegExp marker("@[0-9]+");
      int index=0,newIndex,matchLen;
      // now replace all markers in reimplInLine with links to the classes
      while ((newIndex=marker.match(reimplInLine,index,&matchLen))!=-1)
      {
        ol.parseText(reimplInLine.mid(index,newIndex-index));
        bool ok;
        uint entryIndex = reimplInLine.mid(newIndex+1,matchLen-1).toUInt(&ok);
        //bmd=bml->at(entryIndex);

        count=0;
        // find the entryIndex-th documented entry in the inheritance list.
        for (mli.toLast();(bmd=mli.current()) && (bcd=bmd->getClassDef());--mli)
        {
          if ( bmd->isLinkable() && bcd->isLinkable())
          {
            if (count==entryIndex) break;
            count++;
          }
        }

        if (ok && bcd && bmd) // write link for marker
        {
          //ol.writeObjectLink(bcd->getReference(),bcd->getOutputFileBase(),
          //    bmd->anchor(),bcd->name());
          ol.writeObjectLink(bmd->getReference(),bmd->getOutputFileBase(),
              bmd->anchor(),bcd->displayName());

          if (bmd->isLinkableInProject() )
          {
            writePageRef(ol,bmd->getOutputFileBase(),bmd->anchor());
          }
        }
        ++mli;
        index=newIndex+matchLen;
      }
      ol.parseText(reimplInLine.right(reimplInLine.length()-index));
      ol.endParagraph();
    }
  }
}

void MemberDef::_writeCategoryRelation(OutputList &ol)
{
  if (m_impl->classDef) // this should be a member of a class/category
  {
    //printf("%s: category %s relation %s class=%s categoryOf=%s\n",
    //    name().data(),
    //    m_impl->category ? m_impl->category->name().data() : "<none>",
    //    m_impl->categoryRelation ? m_impl->categoryRelation->name().data() : "<none>",
    //    m_impl->classDef->name().data(),
    //    m_impl->classDef->categoryOf() ? m_impl->classDef->categoryOf()->name().data() : "<none>"
    //    );
    QCString text;
    QCString ref;
    QCString file;
    QCString anc;
    QCString name;
    int i=-1;
    if (m_impl->categoryRelation && m_impl->categoryRelation->isLinkable())
    {
      if (m_impl->category)
      {
        // this member is in a normal class and implements method categoryRelation from category
        // so link to method 'categoryRelation' with 'provided by category 'category' text.
        text = theTranslator->trProvidedByCategory();
        name = m_impl->category->displayName();
      }
      else if (m_impl->classDef->categoryOf())
      {
        // this member is part of a category so link to the corresponding class member of the class we extend
        // so link to method 'categoryRelation' with 'extends class 'classDef->categoryOf()'
        text = theTranslator->trExtendsClass();
        name = m_impl->classDef->categoryOf()->displayName();
      }
      i=text.find("@0");
      if (i!=-1)
      {
        MemberDef *md = m_impl->categoryRelation;
        ref  = md->getReference();
        file = md->getOutputFileBase();
        anc  = md->anchor();
      }
    }
    if (i!=-1 && !name.isEmpty())
    {
      ol.startParagraph();
      ol.parseText(text.left(i));
      ol.writeObjectLink(ref,file,anc,name);
      ol.parseText(text.mid(i+2));
      ol.endParagraph();
    }
  }
}

void MemberDef::_writeExamples(OutputList &ol)
{
  // write the list of examples that use this member
  if (hasExamples())
  {
    ol.startSimpleSect(BaseOutputDocInterface::Examples,0,0,theTranslator->trExamples()+": ");
    ol.startDescForItem();
    writeExample(ol,m_impl->exampleSDict);
    ol.endDescForItem();
    ol.endSimpleSect();
  }
}

void MemberDef::_writeTypeConstraints(OutputList &ol)
{
  if (m_impl->typeConstraints)
  {
    writeTypeConstraints(ol,this,m_impl->typeConstraints);
  }
}

void MemberDef::_writeEnumValues(OutputList &ol,Definition *container,
                                 const QCString &cfname,const QCString &ciname,
                                 const QCString &cname)
{
  // For enum, we also write the documented enum values
  if (isEnumerate())
  {
    bool first=TRUE;
    MemberList *fmdl=enumFieldList();
    //printf("** %s: enum values=%d\n",name().data(),fmdl!=0 ? fmdl->count() : 0);
    if (fmdl)
    {
      MemberListIterator it(*fmdl);
      MemberDef *fmd;
      for (;(fmd=it.current());++it)
      {
        //printf("Enum %p: isLinkable()=%d\n",fmd,fmd->isLinkable());
        if (fmd->isLinkable())
        {
          if (first)
          {
            //ol.startSimpleSect(BaseOutputDocInterface::EnumValues,0,0,theTranslator->trEnumerationValues()+": ");
            //ol.startDescForItem();
            ol.startDescTable(theTranslator->trEnumerationValues());
          }

          ol.addIndexItem(fmd->name(),ciname);
          ol.addIndexItem(ciname,fmd->name());

          //Doxygen::indexList->addIndexItem(
          //                       ciname,                                // level1
          //                       fmd->name(),                           // level2
          //                       separateMemPages ? cfname : cfiname,   // contRef
          //                       cfname,                                // memRef
          //                       fmd->anchor(),                         // anchor
          //                       fmd);                                  // memberdef
          Doxygen::indexList->addIndexItem(container,fmd);

          //ol.writeListItem();
          ol.startDescTableTitle();
          ol.startDoxyAnchor(cfname,cname,fmd->anchor(),fmd->name(),fmd->argsString());
          first=FALSE;
          //ol.startEmphasis();
          ol.docify(fmd->name());
          //ol.endEmphasis();
          ol.disableAllBut(OutputGenerator::Man);
          ol.writeString(" ");
          ol.enableAll();
          ol.endDoxyAnchor(cfname,fmd->anchor());
          ol.endDescTableTitle();
          //ol.newParagraph();
          ol.startDescTableData();

          bool hasBrief = !fmd->briefDescription().isEmpty();
          bool hasDetails = !fmd->documentation().isEmpty();

          if (hasBrief)
          {
            ol.generateDoc(fmd->briefFile(),fmd->briefLine(),
                getOuterScope()?getOuterScope():container,
                fmd,fmd->briefDescription(),TRUE,FALSE);
          }
          // FIXME:PARA
          //if (!fmd->briefDescription().isEmpty() &&
          //    !fmd->documentation().isEmpty())
          //{
          //  ol.newParagraph();
          //}
          if (hasDetails)
          {
            ol.generateDoc(fmd->docFile(),fmd->docLine(),
                getOuterScope()?getOuterScope():container,
                fmd,fmd->documentation()+"\n",TRUE,FALSE);
          }
          ol.endDescTableData();
        }
      }
    }
    if (!first)
    {
      //ol.endItemList();
      ol.endDescTable();
      //ol.endDescForItem();
      //ol.endSimpleSect();
      //ol.writeChar('\n');
    }
  }
}

QCString MemberDef::displayDefinition() const
{
  QCString ldef = definition();
  QCString title = name();
  if (isEnumerate())
  {
    if (title.at(0)=='@')
    {
      ldef = title = "anonymous enum";
      if (!m_impl->enumBaseType.isEmpty())
      {
        ldef+=" : "+m_impl->enumBaseType;
      }
    }
    else
    {
      ldef.prepend("enum ");
    }
  }
  else if (isEnumValue())
  {
    if (ldef.at(0)=='@')
    {
      ldef=ldef.mid(2);
    }
  }
  static QRegExp r("@[0-9]+");
  int l,i=r.match(ldef,0,&l);
  if (i!=-1) // replace anonymous parts with { ... }
  {
    int si=ldef.find(' '),pi,ei=i+l;
    if (si==-1) si=0;
    while ((pi=r.match(ldef,i+l,&l))!=-1)
    {
      i=pi;
      ei=i+l;
    }
    int ni=ldef.find("::",si);
    if (ni>=ei) ei=ni+2;
    ldef = ldef.left(si) + " { ... } " + ldef.right(ldef.length()-ei);
  }
  ClassDef *cd=getClassDef();
  if (cd && cd->isObjectiveC())
  {
    // strip scope name
    int ep = ldef.find("::");
    if (ep!=-1)
    {
      int sp=ldef.findRev(' ',ep);
      if (sp!=-1)
      {
        ldef=ldef.left(sp+1)+ldef.mid(ep+2);
      }
    }
    // strip keywords
    int dp = ldef.find(':');
    if (dp!=-1)
    {
      ldef=ldef.left(dp+1);
    }
    l=ldef.length();
    //printf("start >%s<\n",ldef.data());
    i=l-1;
    while (i>=0 && (isId(ldef.at(i)) || ldef.at(i)==':')) i--;
    while (i>=0 && isspace((uchar)ldef.at(i))) i--;
    if (i>0)
    {
      // insert braches around the type
      QCString tmp("("+ldef.left(i+1)+")"+ldef.mid(i+1));
      ldef=tmp;
    }
    //printf("end   >%s< i=%d\n",ldef.data(),i);
    if (isStatic()) ldef.prepend("+ "); else ldef.prepend("- ");
  }
  SrcLangExt lang = getLanguage();
  QCString sep = getLanguageSpecificSeparator(lang,TRUE);
  return substitute(ldef,"::",sep);
}

void MemberDef::_writeGroupInclude(OutputList &ol,bool inGroup)
{
  // only write out the include file if this is not part of a class or file
  // definition
  static bool showGroupedMembInc = Config_getBool(SHOW_GROUPED_MEMB_INC);
  FileDef *fd = getFileDef();
  QCString nm;
  if (fd) nm = getFileDef()->docName();
  if (inGroup && fd && showGroupedMembInc && !nm.isEmpty())
  {
    ol.startParagraph();
    ol.startTypewriter();
    SrcLangExt lang = getLanguage();
    bool isIDLorJava = lang==SrcLangExt_IDL || lang==SrcLangExt_Java;
    if (isIDLorJava)
    {
      ol.docify("import ");
    }
    else
    {
      ol.docify("#include ");
    }

    if (isIDLorJava) ol.docify("\""); else ol.docify("<");

    if (fd->isLinkable())
    {
      ol.writeObjectLink(fd->getReference(),fd->getOutputFileBase(),fd->anchor(),nm);
    }
    else
    {
      ol.docify(nm);
    }

    if (isIDLorJava) ol.docify("\""); else ol.docify(">");

    ol.endTypewriter();
    ol.endParagraph();
  }
}

/*! Writes the "detailed documentation" section of this member to
 *  all active output formats.
 */
void MemberDef::writeDocumentation(MemberList *ml,OutputList &ol,
                                   const char *scName,
                                   Definition *container,
                                   bool inGroup,
                                   bool showEnumValues,
                                   bool showInline
                                  )
{
  // if this member is in a group find the real scope name.
  bool hasParameterList = FALSE;
  bool inFile = container->definitionType()==Definition::TypeFile;
  bool hasDocs = isDetailedSectionVisible(inGroup,inFile);

  //printf("MemberDef::writeDocumentation(): name=`%s' hasDocs=`%d' containerType=%d inGroup=%d sectionLinkable=%d\n",
  //    name().data(),hasDocs,container->definitionType(),inGroup,isDetailedSectionLinkable());

  if ( !hasDocs ) return;
  if (isEnumValue() && !showEnumValues) return;

  SrcLangExt lang = getLanguage();
  //printf("member=%s lang=%d\n",name().data(),lang);
  bool optVhdl = lang==SrcLangExt_VHDL;
  QCString sep = getLanguageSpecificSeparator(lang,TRUE);

  QCString scopeName = scName;
  QCString memAnchor = anchor();
  QCString ciname = container->name();
  Definition *scopedContainer = container; // see bug 753608
  if (container->definitionType()==TypeGroup)
  {
    if (getClassDef())          { scopeName=getClassDef()->displayName();     scopedContainer=getClassDef(); }
    else if (getNamespaceDef()) { scopeName=getNamespaceDef()->displayName(); scopedContainer=getNamespaceDef(); }
    else if (getFileDef())      { scopeName=getFileDef()->displayName();      scopedContainer=getFileDef(); }
    ciname = ((GroupDef *)container)->groupTitle();
  }
  else if (container->definitionType()==TypeFile && getNamespaceDef())
  { // member is in a namespace, but is written as part of the file documentation
    // as well, so we need to make sure its label is unique.
    memAnchor.prepend("file_");
  }

  QCString cname   = container->name();
  QCString cfname  = getOutputFileBase();

  // get member name
  QCString doxyName=name();
  // prepend scope if there is any. TODO: make this optional for C only docs
  if (!scopeName.isEmpty())
  {
    doxyName.prepend(scopeName+sep);
  }
  QCString doxyArgs=argsString();

  QCString ldef = definition();
  QCString title = name();
  //printf("member `%s' def=`%s'\n",name().data(),ldef.data());
  if (isEnumerate())
  {
    if (title.at(0)=='@')
    {
      ldef = title = "anonymous enum";
      if (!m_impl->enumBaseType.isEmpty())
      {
        ldef+=" : "+m_impl->enumBaseType;
      }
    }
    else
    {
      ldef.prepend("enum ");
    }
  }
  else if (isEnumValue())
  {
    if (ldef.at(0)=='@')
    {
      ldef=ldef.mid(2);
    }
  }
  else if (isFunction())
  {
    title+=argsString();
  }
  int i=0,l;
  static QRegExp r("@[0-9]+");

  //----------------------------------------

  ol.pushGeneratorState();

  bool htmlEndLabelTable=FALSE;
  QStrList sl;
  getLabels(sl,scopedContainer);

  if ((isVariable() || isTypedef()) && (i=r.match(ldef,0,&l))!=-1)
  {
    // find enum type and insert it in the definition
    QListIterator<MemberDef> vmli(*ml);
    MemberDef *vmd;
    bool found=FALSE;
    for ( ; (vmd=vmli.current()) && !found ; ++vmli)
    {
      if (vmd->isEnumerate() && ldef.mid(i,l)==vmd->name())
      {
        ol.startDoxyAnchor(cfname,cname,memAnchor,doxyName,doxyArgs);
        ol.startMemberDoc(ciname,name(),memAnchor,name(),showInline);
        linkifyText(TextGeneratorOLImpl(ol),scopedContainer,getBodyDef(),this,ldef.left(i));
        vmd->writeEnumDeclaration(ol,getClassDef(),getNamespaceDef(),getFileDef(),getGroupDef());
        linkifyText(TextGeneratorOLImpl(ol),scopedContainer,getBodyDef(),this,ldef.right(ldef.length()-i-l));

        found=TRUE;
      }
    }
    if (!found) // anonymous compound
    {
      //printf("Anonymous compound `%s'\n",cname.data());
      ol.startDoxyAnchor(cfname,cname,memAnchor,doxyName,doxyArgs);
      ol.startMemberDoc(ciname,name(),memAnchor,name(),showInline);
      // search for the last anonymous compound name in the definition
      int si=ldef.find(' '),pi,ei=i+l;
      if (si==-1) si=0;
      while ((pi=r.match(ldef,i+l,&l))!=-1)
      {
        i=pi;
        ei=i+l;
      }
      // first si characters of ldef contain compound type name
      ol.startMemberDocName(isObjCMethod());
      ol.docify(ldef.left(si));
      ol.docify(" { ... } ");
      // last ei characters of ldef contain pointer/reference specifiers
      int ni=ldef.find("::",si);
      if (ni>=ei) ei=ni+2;
      linkifyText(TextGeneratorOLImpl(ol),scopedContainer,getBodyDef(),this,ldef.right(ldef.length()-ei));
    }
  }
  else // not an enum value or anonymous compound
  {
    ol.startDoxyAnchor(cfname,cname,memAnchor,doxyName,doxyArgs);
    ol.startMemberDoc(ciname,name(),memAnchor,title,showInline);

    ClassDef *cd=getClassDef();
    NamespaceDef *nd=getNamespaceDef();
    if (!Config_getBool(HIDE_SCOPE_NAMES))
    {
      bool first=TRUE;
      SrcLangExt lang = getLanguage();
      if (m_impl->defTmpArgLists && lang==SrcLangExt_Cpp)
        // definition has explicit template parameter declarations
      {
        QListIterator<ArgumentList> ali(*m_impl->defTmpArgLists);
        ArgumentList *tal;
        for (ali.toFirst();(tal=ali.current());++ali)
        {
          if (tal->count()>0)
          {
            if (!first) ol.docify(" ");
            ol.startMemberDocPrefixItem();
            writeTemplatePrefix(ol,tal);
            ol.endMemberDocPrefixItem();
          }
        }
      }
      else // definition gets it template parameters from its class
        // (since no definition was found)
      {
        if (cd && lang==SrcLangExt_Cpp && !isTemplateSpecialization())
        {
          QList<ArgumentList> tempParamLists;
          cd->getTemplateParameterLists(tempParamLists);
          //printf("#tempParamLists=%d\n",tempParamLists.count());
          QListIterator<ArgumentList> ali(tempParamLists);
          ArgumentList *tal;
          for (ali.toFirst();(tal=ali.current());++ali)
          {
            if (tal->count()>0)
            {
              if (!first) ol.docify(" ");
              ol.startMemberDocPrefixItem();
              writeTemplatePrefix(ol,tal);
              ol.endMemberDocPrefixItem();
            }
          }
        }
        if (m_impl->tArgList && lang==SrcLangExt_Cpp) // function template prefix
        {
          ol.startMemberDocPrefixItem();
          writeTemplatePrefix(ol,m_impl->tArgList);
          ol.endMemberDocPrefixItem();
        }
      }
    }

    if (sl.count()>0)
    {
      ol.pushGeneratorState();
      ol.disableAll();
      ol.enable(OutputGenerator::Html);
      ol.writeString("<table class=\"mlabels\">\n");
      ol.writeString("  <tr>\n");
      ol.writeString("  <td class=\"mlabels-left\">\n");
      ol.popGeneratorState();
      htmlEndLabelTable=TRUE;
    }

    ol.startMemberDocName(isObjCMethod());
    if (cd && cd->isObjectiveC())
    {
      // strip scope name
      int ep = ldef.find("::");
      if (ep!=-1)
      {
        int sp=ldef.findRev(' ',ep);
        if (sp!=-1)
        {
          ldef=ldef.left(sp+1)+ldef.mid(ep+2);
        } else {
          ldef=ldef.mid(ep+2);
        }
      }
      // strip keywords
      int dp = ldef.find(':');
      if (dp!=-1)
      {
        ldef=ldef.left(dp+1);
      }
      int l=ldef.length();
      //printf("start >%s<\n",ldef.data());
      int i=l-1;
      while (i>=0 && (isId(ldef.at(i)) || ldef.at(i)==':')) i--;
      while (i>=0 && isspace((uchar)ldef.at(i))) i--;
      if (i>0)
      {
        // insert braches around the type
        QCString tmp("("+ldef.left(i+1)+")"+ldef.mid(i+1));
        ldef=tmp;
      }
      //printf("end   >%s< i=%d\n",ldef.data(),i);
      if (isStatic()) ldef.prepend("+ "); else ldef.prepend("- ");
    }

    if (optVhdl)
    {
      hasParameterList=VhdlDocGen::writeVHDLTypeDocumentation(this,scopedContainer,ol);
    }
    else
    {
      linkifyText(TextGeneratorOLImpl(ol),
                  scopedContainer,
                  getBodyDef(),
                  this,
                  substitute(ldef,"::",sep)
                 );
      Definition *scope = cd;
      if (scope==0) scope = nd;
      hasParameterList=writeDefArgumentList(ol,scope,this);
    }

    if (hasOneLineInitializer()) // add initializer
    {
      if (!isDefine())
      {
        //ol.docify(" = ");
        ol.docify(" ");
        QCString init = m_impl->initializer.simplifyWhiteSpace();
        linkifyText(TextGeneratorOLImpl(ol),scopedContainer,getBodyDef(),this,init);
      }
      else
      {
        ol.writeNonBreakableSpace(3);
        linkifyText(TextGeneratorOLImpl(ol),scopedContainer,getBodyDef(),this,m_impl->initializer);
      }
    }
    if (excpString()) // add exception list
    {
      writeExceptionList(ol,cd,this);
      hasParameterList=true; // call endParameterList below
    }
  }

  ol.pushGeneratorState();
  ol.disable(OutputGenerator::Html);
  if (sl.count()>0)
  {
    ol.startLabels();
    const char *s=sl.first();
    while (s)
    {
      const char *ns = sl.next();
      ol.writeLabel(s,ns==0);
      s=ns;
    }
    ol.endLabels();
  }
  ol.popGeneratorState();

  if (hasParameterList)
  {
    ol.endParameterList();
    ol.endMemberDoc(TRUE);
  }
  else
  {
    ol.endMemberDocName();
    ol.endMemberDoc(FALSE);
  }

  // for HTML write the labels here
  ol.pushGeneratorState();
  ol.disableAll();
  ol.enable(OutputGenerator::Html);
  if (htmlEndLabelTable)
  {
    ol.writeString("  </td>\n");
    ol.writeString("  <td class=\"mlabels-right\">\n");
    ol.startLabels();
    const char *s=sl.first();
    while (s)
    {
      const char *ns = sl.next();
      ol.writeLabel(s,ns==0);
      s=ns;
    }
    ol.endLabels();
    ol.writeString("  </td>\n");
    ol.writeString("  </tr>\n");
    ol.writeString("</table>\n");
  }
  ol.writeString("</div>");
  ol.popGeneratorState();


  ol.endDoxyAnchor(cfname,memAnchor);
  ol.startIndent();

  _writeGroupInclude(ol,inGroup);

  /* write multi-line initializer (if any) */
  if (hasMultiLineInitializer()
      //initLines>0 && ((initLines<maxInitLines && userInitLines==-1) // implicitly enabled
      //                || initLines<userInitLines // explicitly enabled
      //               )
     )
  {
    //printf("md=%s initLines=%d init=`%s'\n",name().data(),initLines,init.data());
    ol.startBold();
    if (m_impl->mtype==MemberType_Define)
      ol.parseText(theTranslator->trDefineValue());
    else
      ol.parseText(theTranslator->trInitialValue());
    ol.endBold();
    ParserInterface *pIntf = Doxygen::parserManager->getParser(getDefFileExtension());
    pIntf->resetCodeParserState();
    ol.startCodeFragment();
    pIntf->parseCode(ol,scopeName,m_impl->initializer,lang,FALSE,0,getFileDef(),
                     -1,-1,TRUE,this,FALSE,this);
    ol.endCodeFragment();
  }

  QCString brief           = briefDescription();
  QCString detailed        = documentation();
  ArgumentList *docArgList = m_impl->defArgList;
  if (m_impl->templateMaster)
  {
    brief      = m_impl->templateMaster->briefDescription();
    detailed   = m_impl->templateMaster->documentation();
    docArgList = m_impl->templateMaster->argumentList();
  }

  /* write brief description */
  if (!brief.isEmpty() &&
      (Config_getBool(REPEAT_BRIEF) ||
       !Config_getBool(BRIEF_MEMBER_DESC)
      )
     )
  {
    ol.startParagraph();
    ol.generateDoc(briefFile(),briefLine(),
                scopedContainer,this,
                brief,FALSE,FALSE,0,TRUE,FALSE);
    ol.endParagraph();
  }

  /* write detailed description */
  if (!detailed.isEmpty() ||
      !inbodyDocumentation().isEmpty())
  {
    // write vhdl inline code with or without option INLINE_SOURCE
    if (optVhdl && VhdlDocGen::isMisc(this))
    {
      VhdlDocGen::writeSource(this,ol,cname);
      return;
    }
    else
    {
      ol.generateDoc(docFile(),docLine(),scopedContainer,this,detailed+"\n",TRUE,FALSE);
    }

    if (!inbodyDocumentation().isEmpty())
    {
      ol.generateDoc(inbodyFile(),inbodyLine(),
                  scopedContainer,this,
                  inbodyDocumentation()+"\n",TRUE,FALSE);
    }
  }
  else if (!brief.isEmpty() && (Config_getBool(REPEAT_BRIEF) ||
        !Config_getBool(BRIEF_MEMBER_DESC)))
  {
    if (!inbodyDocumentation().isEmpty())
    {
      ol.generateDoc(inbodyFile(),inbodyLine(),scopedContainer,this,inbodyDocumentation()+"\n",TRUE,FALSE);
    }
  }


  //printf("***** defArgList=%p name=%s docs=%s hasDocs=%d\n",
  //     defArgList,
  //     defArgList?defArgList->hasDocumentation():-1);
  if (docArgList!=0 && docArgList->hasDocumentation())
  {
    QCString paramDocs;
    ArgumentListIterator ali(*docArgList);
    Argument *a;
    // convert the parameter documentation into a list of @param commands
    for (ali.toFirst();(a=ali.current());++ali)
    {
      if (a->hasDocumentation())
      {
        QCString direction = extractDirection(a->docs);
        paramDocs+="@param"+direction+" "+a->name+" "+a->docs;
      }
    }
    // feed the result to the documentation parser
    ol.generateDoc(
        docFile(),docLine(),
        scopedContainer,
        this,         // memberDef
        paramDocs,    // docStr
        TRUE,         // indexWords
        FALSE         // isExample
        );

  }

  _writeEnumValues(ol,scopedContainer,cfname,ciname,cname);
  _writeReimplements(ol);
  _writeReimplementedBy(ol);
  _writeCategoryRelation(ol);
  _writeExamples(ol);
  _writeTypeConstraints(ol);
  writeSourceDef(ol,cname);
  writeSourceRefs(ol,cname);
  writeSourceReffedBy(ol,cname);
  writeInlineCode(ol,cname);
  _writeCallGraph(ol);
  _writeCallerGraph(ol);

  if (Doxygen::userComments)
  {
    ol.pushGeneratorState();
    ol.disableAllBut(OutputGenerator::Html);
    QCString cmd = "<? $root=$_SERVER['DOCUMENT_ROOT']; "
                   "passthru(\"$root/doxynotes --lookup "+
                   getOutputFileBase()+":"+anchor()+"\") ?>";
    ol.writeString(cmd);
    ol.popGeneratorState();
  }

  ol.endIndent();

  // enable LaTeX again
  //if (Config_getBool(EXTRACT_ALL) && !hasDocs) ol.enable(OutputGenerator::Latex);
  ol.popGeneratorState();

  //------------------------------------------------

  if (!Config_getBool(EXTRACT_ALL) &&
      Config_getBool(WARN_IF_UNDOCUMENTED) &&
      Config_getBool(WARN_NO_PARAMDOC) &&
      !Doxygen::suppressDocWarnings)
  {
    if (!hasDocumentedParams())
    {
      warn_doc_error(getDefFileName(),getDefLine(),
          "parameters of member %s are not (all) documented",
          qPrint(qualifiedName()));
    }
    if (!hasDocumentedReturnType() && isFunction() && hasDocumentation())
    {
      warn_doc_error(getDefFileName(),getDefLine(),
          "return type of member %s is not documented",
          qPrint(qualifiedName()));
    }
  }
}

// strip scope and field name from the type
// example: "struct N::S.v.c" will become "struct v"
static QCString simplifyTypeForTable(const QCString &s)
{
  QCString ts=removeAnonymousScopes(s);
  if (ts.right(2)=="::") ts = ts.left(ts.length()-2);
  static QRegExp re("[A-Z_a-z0-9]+::");
  int i,l;
  while ((i=re.match(ts,0,&l))!=-1)
  {
    ts=ts.left(i)+ts.mid(i+l);
  }
  i=ts.findRev('.');
  if (i!=-1) ts = ts.left(i);
  i=ts.findRev('.');
  if (i!=-1) ts = ts.right(ts.length()-i-1);
  //printf("simplifyTypeForTable(%s)->%s\n",s.data(),ts.data());
  return ts;
}

#if 0
/** Returns the type definition corresponding to a member's return type.
 *  @param[in]  scope The scope in which to search for the class definition.
 *  @param[in]  type  The string representing the member's return type.
 *  @param[in]  lang  The programming language in which the class is defined.
 *  @param[out] start The string position where the class definition name was found.
 *  @param[out] length The length of the class definition's name.
 */
static Definition *getClassFromType(Definition *scope,const QCString &type,SrcLangExt lang,int &start,int &length)
{
  int pos=0;
  int i;
  QCString name;
  QCString templSpec;
  while ((i=extractClassNameFromType(type,pos,name,templSpec,lang))!=-1)
  {
    ClassDef *cd=0;
    MemberDef *md=0;
    int l = name.length()+templSpec.length();
    if (!templSpec.isEmpty())
    {
      cd = getResolvedClass(scope,0,name+templSpec,&md);
    }
    cd = getResolvedClass(scope,0,name);
    if (cd)
    {
      start=i;
      length=l;
      printf("getClassFromType: type=%s name=%s start=%d length=%d\n",type.data(),name.data(),start,length);
      return cd;
    }
    else if (md)
    {
      start=i;
      length=l;
      printf("getClassFromType: type=%s name=%s start=%d length=%d\n",type.data(),name.data(),start,length);
      return md;
    }
    pos=i+l;
  }
  return 0;
}
#endif

QCString MemberDef::fieldType() const
{
  QCString type = m_impl->accessorType;
  if (type.isEmpty())
  {
    type = m_impl->type;
  }

  if (isTypedef()) type.prepend("typedef ");
  return simplifyTypeForTable(type);
}

void MemberDef::writeMemberDocSimple(OutputList &ol, Definition *container)
{
  Definition *scope  = getOuterScope();
  QCString doxyName  = name();
  QCString doxyArgs  = argsString();
  QCString memAnchor = anchor();
  QCString cfname    = getOutputFileBase();
  QCString cname;
  if (scope) cname   = scope->name();
  if (doxyName.at(0)=='@')
  {
    doxyName="__unnamed__";
  }

  ClassDef *cd = m_impl->accessorClass;
  //printf("===> %s::anonymous: %s\n",name().data(),cd?cd->name().data():"<none>");

  ol.startInlineMemberType();
  ol.startDoxyAnchor(cfname,cname,memAnchor,doxyName,doxyArgs);

  QCString ts = fieldType();

  if (cd) // cd points to an anonymous struct pointed to by this member
          // so we add a link to it from the type column.
  {
    int i=0;
    const char *prefixes[] = { "struct ","union ","class ", 0 };
    const char **p = prefixes;
    while (*p)
    {
      int l=qstrlen(*p);
      if (ts.left(l)==*p)
      {
        ol.writeString(*p);
        i=l;
      }
      p++;
    }
    ol.writeObjectLink(cd->getReference(),
                       cd->getOutputFileBase(),
                       cd->anchor(),ts.mid(i));
  }
  else // use standard auto linking
  {
    linkifyText(TextGeneratorOLImpl(ol), // out
                scope,                   // scope
                getBodyDef(),            // fileScope
                this,                    // self
                ts,                      // text
                TRUE                     // autoBreak
               );
  }
  ol.endDoxyAnchor(cfname,memAnchor);
  ol.endInlineMemberType();

  ol.startInlineMemberName();
  ol.docify(doxyName);
  if (isVariable() && argsString() && !isObjCMethod())
  {
    linkifyText(TextGeneratorOLImpl(ol),getOuterScope(),getBodyDef(),this,argsString());
  }
  if (!m_impl->bitfields.isEmpty()) // add bitfields
  {
    linkifyText(TextGeneratorOLImpl(ol),getOuterScope(),getBodyDef(),this,m_impl->bitfields);
  }
  ol.endInlineMemberName();

  ol.startInlineMemberDoc();

  QCString brief           = briefDescription();
  QCString detailed        = documentation();

  /* write brief description */
  if (!brief.isEmpty())
  {
    ol.generateDoc(briefFile(),briefLine(),
                getOuterScope()?getOuterScope():container,this,
                brief,FALSE,FALSE,0,TRUE,FALSE);
  }

  /* write detailed description */
  if (!detailed.isEmpty())
  {
    ol.generateDoc(docFile(),docLine(),
                getOuterScope()?getOuterScope():container,this,
                detailed+"\n",FALSE,FALSE,0,FALSE,FALSE);

  }

  ol.endInlineMemberDoc();
}

QCString MemberDef::memberTypeName() const
{
  switch (m_impl->mtype)
  {
    case MemberType_Define:      return "macro definition";
    case MemberType_Function:    return "function";
    case MemberType_Variable:    return "variable";
    case MemberType_Typedef:     return "typedef";
    case MemberType_Enumeration: return "enumeration";
    case MemberType_EnumValue:   return "enumvalue";
    case MemberType_Signal:      return "signal";
    case MemberType_Slot:        return "slot";
    case MemberType_Friend:      return "friend";
    case MemberType_DCOP:        return "dcop";
    case MemberType_Property:    return "property";
    case MemberType_Event:       return "event";
    case MemberType_Interface:   return "interface";
    case MemberType_Service:     return "service";
    default:          return "unknown";
  }
}

void MemberDef::warnIfUndocumented()
{
  if (m_impl->memberGroup) return;
  ClassDef     *cd = getClassDef();
  NamespaceDef *nd = getNamespaceDef();
  FileDef      *fd = getFileDef();
  GroupDef     *gd = getGroupDef();
  Definition *d=0;
  const char *t=0;
  if (cd)
    t="class", d=cd;
  else if (nd)
  {
    d=nd;
    if (d->getLanguage() == SrcLangExt_Fortran)
      t="module";
    else
      t="namespace";
  }
  else if (gd)
    t="group", d=gd;
  else
    t="file", d=fd;
  static bool extractAll = Config_getBool(EXTRACT_ALL);

  //printf("%s:warnIfUndoc: hasUserDocs=%d isFriendClass=%d protection=%d isRef=%d isDel=%d\n",
  //    name().data(),
  //    hasUserDocumentation(),isFriendClass(),protectionLevelVisible(m_impl->prot),isReference(),isDeleted());
  if ((!hasUserDocumentation() && !extractAll) &&
      !isFriendClass() &&
      name().find('@')==-1 && d && d->name().find('@')==-1 &&
      protectionLevelVisible(m_impl->prot) &&
      !isReference() && !isDeleted()
     )
  {
    warn_undoc(getDefFileName(),getDefLine(),"Member %s%s (%s) of %s %s is not documented.",
         qPrint(name()),qPrint(argsString()),qPrint(memberTypeName()),t,qPrint(d->name()));
  }
}



bool MemberDef::isFriendClass() const
{
  return (isFriend() &&
         (m_impl->type=="friend class" || m_impl->type=="friend struct" ||
          m_impl->type=="friend union"));
}

bool MemberDef::isDocumentedFriendClass() const
{
  ClassDef *fcd=0;
  QCString baseName=name();
  int i=baseName.find('<');
  if (i!=-1) baseName=baseName.left(i);
  return (isFriendClass() &&
         (fcd=getClass(baseName)) && fcd->isLinkable());
}

bool MemberDef::isDeleted() const
{
  return m_impl->defArgList && m_impl->defArgList->isDeleted;
}

bool MemberDef::hasDocumentation() const
{
  return Definition::hasDocumentation() ||
         (m_impl->mtype==MemberType_Enumeration && m_impl->docEnumValues) ||  // has enum values
         (m_impl->defArgList!=0 && m_impl->defArgList->hasDocumentation());   // has doc arguments
}

#if 0
bool MemberDef::hasUserDocumentation() const
{
  bool hasDocs = Definition::hasUserDocumentation();
  return hasDocs;
}
#endif


void MemberDef::setMemberGroup(MemberGroup *grp)
{
  m_impl->memberGroup = grp;
}

bool MemberDef::visibleMemberGroup(bool hideNoHeader)
{
  return m_impl->memberGroup!=0 &&
          (!hideNoHeader || m_impl->memberGroup->header()!="[NOHEADER]");
}

QCString MemberDef::getScopeString() const
{
  QCString result;
  if (getClassDef()) result=getClassDef()->displayName();
  else if (getNamespaceDef()) result=getNamespaceDef()->displayName();
  return result;
}

#if 0
static QCString escapeAnchor(const QCString &anchor)
{
  QCString result;
  int l = anchor.length(),i;
  for (i=0;i<l;i++)
  {
    char c = anchor.at(i);
    if ((c>='a' && c<='z') || (c>='A' && c<='Z'))
    {
      result+=c;
    }
    else
    {
      static char hexStr[]="0123456789ABCDEF";
      char escChar[]={ '_', 0, 0, 0 };
      escChar[1]=hexStr[c>>4];
      escChar[2]=hexStr[c&0xf];
      result+=escChar;
    }
  }
  return result;
}
#endif

void MemberDef::setAnchor()
{
  QCString memAnchor = name();
  if (!m_impl->args.isEmpty()) memAnchor+=m_impl->args;

  memAnchor.prepend(definition()); // actually the method name is now included
            // twice, which is silly, but we keep it this way for backward
            // compatibility.

  // include number of template arguments as well,
  // to distinguish between two template
  // specializations that only differ in the template parameters.
  if (m_impl->tArgList)
  {
    char buf[20];
    qsnprintf(buf,20,"%d:",m_impl->tArgList->count());
    buf[19]='\0';
    memAnchor.prepend(buf);
  }

  // convert to md5 hash
  uchar md5_sig[16];
  QCString sigStr(33);
  MD5Buffer((const unsigned char *)memAnchor.data(),memAnchor.length(),md5_sig);
  //printf("memAnchor=%s\n",memAnchor.data());
  MD5SigToString(md5_sig,sigStr.rawData(),33);
  m_impl->anc = "a"+sigStr;
}

void MemberDef::setGroupDef(GroupDef *gd,Grouping::GroupPri_t pri,
                            const QCString &fileName,int startLine,
                            bool hasDocs,MemberDef *member)
{
  //printf("%s MemberDef::setGroupDef(%s)\n",name().data(),gd->name().data());
  m_impl->group=gd;
  m_impl->grouppri=pri;
  m_impl->groupFileName=fileName;
  m_impl->groupStartLine=startLine;
  m_impl->groupHasDocs=hasDocs;
  m_impl->groupMember=member;
  m_isLinkableCached = 0;
}

void MemberDef::setEnumScope(MemberDef *md,bool livesInsideEnum)
{
  m_impl->enumScope=md;
  m_impl->livesInsideEnum=livesInsideEnum;
  if (md->getGroupDef())
  {
    m_impl->group=md->getGroupDef();
    m_impl->grouppri=md->getGroupPri();
    m_impl->groupFileName=md->getGroupFileName();
    m_impl->groupStartLine=md->getGroupStartLine();
    m_impl->groupHasDocs=md->getGroupHasDocs();
    m_isLinkableCached = 0;
  }
}

void MemberDef::setMemberClass(ClassDef *cd)
{
  m_impl->classDef=cd;
  m_isLinkableCached = 0;
  m_isConstructorCached = 0;
  setOuterScope(cd);
}

void MemberDef::setNamespace(NamespaceDef *nd)
{
  m_impl->nspace=nd;
  setOuterScope(nd);
}

MemberDef *MemberDef::createTemplateInstanceMember(
        ArgumentList *formalArgs,ArgumentList *actualArgs)
{
  //printf("  Member %s %s %s\n",typeString(),name().data(),argsString());
  ArgumentList *actualArgList = 0;
  if (m_impl->defArgList)
  {
    actualArgList = m_impl->defArgList->deepCopy();

    // replace formal arguments with actuals
    ArgumentListIterator ali(*actualArgList);
    Argument *arg;
    for (;(arg=ali.current());++ali)
    {
      arg->type = substituteTemplateArgumentsInString(arg->type,formalArgs,actualArgs);
    }
    actualArgList->trailingReturnType =
       substituteTemplateArgumentsInString(actualArgList->trailingReturnType,formalArgs,actualArgs);
  }

  QCString methodName=name();
  if (methodName.left(9)=="operator ") // conversion operator
  {
    methodName=substituteTemplateArgumentsInString(methodName,formalArgs,actualArgs);
  }

  MemberDef *imd = new MemberDef(
                       getDefFileName(),getDefLine(),getDefColumn(),
                       substituteTemplateArgumentsInString(m_impl->type,formalArgs,actualArgs),
                       methodName,
                       substituteTemplateArgumentsInString(m_impl->args,formalArgs,actualArgs),
                       m_impl->exception, m_impl->prot,
                       m_impl->virt, m_impl->stat, m_impl->related, m_impl->mtype, 0, 0
                   );
  imd->setArgumentList(actualArgList);
  imd->setDefinition(substituteTemplateArgumentsInString(m_impl->def,formalArgs,actualArgs));
  imd->setBodyDef(getBodyDef());
  imd->setBodySegment(getStartBodyLine(),getEndBodyLine());
  //imd->setBodyMember(this);

  // TODO: init other member variables (if needed).
  // TODO: reimplemented info
  return imd;
}

bool MemberDef::hasOneLineInitializer() const
{
  //printf("%s: init=%s, initLines=%d maxInitLines=%d userInitLines=%d\n",
  //    name().data(),m_impl->initializer.data(),m_impl->initLines,
  //    m_impl->maxInitLines,m_impl->userInitLines);
  return !m_impl->initializer.isEmpty() && m_impl->initLines==0 && // one line initializer
         ((m_impl->maxInitLines>0 && m_impl->userInitLines==-1) || m_impl->userInitLines>0); // enabled by default or explicitly
}

bool MemberDef::hasMultiLineInitializer() const
{
  //printf("initLines=%d userInitLines=%d maxInitLines=%d\n",
  //    initLines,userInitLines,maxInitLines);
  return m_impl->initLines>0 &&
         ((m_impl->initLines<m_impl->maxInitLines && m_impl->userInitLines==-1) // implicitly enabled
          || m_impl->initLines<m_impl->userInitLines // explicitly enabled
         );
}

void MemberDef::setInitializer(const char *initializer)
{
  m_impl->initializer=initializer;
  int l=m_impl->initializer.length();
  int p=l-1;
  while (p>=0 && isspace((uchar)m_impl->initializer.at(p))) p--;
  m_impl->initializer=m_impl->initializer.left(p+1);
  m_impl->initLines=m_impl->initializer.contains('\n');
  //printf("%s::setInitializer(%s)\n",name().data(),m_impl->initializer.data());
}

void MemberDef::addListReference(Definition *)
{
  static bool optimizeOutputForC = Config_getBool(OPTIMIZE_OUTPUT_FOR_C);
  //static bool hideScopeNames     = Config_getBool(HIDE_SCOPE_NAMES);
  //static bool optimizeOutputJava = Config_getBool(OPTIMIZE_OUTPUT_JAVA);
  //static bool fortranOpt = Config_getBool(OPTIMIZE_FOR_FORTRAN);
  SrcLangExt lang = getLanguage();
  visited=TRUE;
  if (!isLinkableInProject()) return;
  QCString memLabel;
  if (optimizeOutputForC)
  {
    memLabel=theTranslator->trGlobal(TRUE,TRUE);
  }
  else if (lang==SrcLangExt_Fortran)
  {
    memLabel=theTranslator->trSubprogram(TRUE,TRUE);
  }
  else
  {
    memLabel=theTranslator->trMember(TRUE,TRUE);
  }
  QCString memName = name();
  Definition *pd=getOuterScope();
  QCString pdName = pd->definitionType()==Definition::TypeClass ?
                    ((ClassDef*)pd)->displayName() : pd->name();
  QCString sep = getLanguageSpecificSeparator(lang,TRUE);
  QCString memArgs;
  if (!isRelated()
      /* && commented out as a result of bug 597016
      (
       (!hideScopeNames &&                    // there is a scope
        pd && pd!=Doxygen::globalScope)       // and we can show it
       ||
       (pd=getClassDef())                     // it's a class so we
                                              // show the scope anyway
      )
      */
     )
  {
    if (isObjCMethod())
    {
      memName = "[" + pd->name() + " " + name() + "]";
    }
    else
    {
      if (pd!=Doxygen::globalScope) memName.prepend(pdName+sep);
      memArgs = argsString();
    }
  }
  QList<ListItemInfo> *xrefItems = xrefListItems();
  if (xrefItems)
  {
    addRefItem(xrefItems,
        qualifiedName()+argsString(), // argsString is needed for overloaded functions (see bug 609624)
        memLabel,
        getOutputFileBase()+"#"+anchor(),memName,memArgs,pd);
  }
}

MemberList *MemberDef::getSectionList(Definition *d) const
{
  char key[20];
  sprintf(key,"%p",d);
  return (d!=0 && m_impl->classSectionSDict) ? m_impl->classSectionSDict->find(key) : 0;
}

void MemberDef::setSectionList(Definition *d, MemberList *sl)
{
  //printf("MemberDef::setSectionList(%p,%p) name=%s\n",d,sl,name().data());
  char key[20];
  sprintf(key,"%p",d);
  if (m_impl->classSectionSDict==0)
  {
    m_impl->classSectionSDict = new SDict<MemberList>(7);
  }
  m_impl->classSectionSDict->append(key,sl);
}

Specifier MemberDef::virtualness(int count) const
{
  if (count>25)
  {
     warn(getDefFileName(),getDefLine(),
       "Internal inconsistency: recursion detected in overload relation for member %s!"
       ,qPrint(name())
      );
     return Normal;
  }
  Specifier v = m_impl->virt;
  MemberDef *rmd = reimplements();
  while (rmd && v==Normal)
  {
    v = rmd->virtualness(count+1)==Normal ? Normal : Virtual;
    rmd = rmd->reimplements();
  }
  return v;
}

void MemberDef::writeTagFile(FTextStream &tagFile)
{
  if (!isLinkableInProject()) return;
  tagFile << "    <member kind=\"";
  switch (m_impl->mtype)
  {
    case MemberType_Define:      tagFile << "define";      break;
    case MemberType_EnumValue:   tagFile << "enumvalue";   break;
    case MemberType_Property:    tagFile << "property";    break;
    case MemberType_Event:       tagFile << "event";       break;
    case MemberType_Variable:    tagFile << "variable";    break;
    case MemberType_Typedef:     tagFile << "typedef";     break;
    case MemberType_Enumeration: tagFile << "enumeration"; break;
    case MemberType_Function:    tagFile << "function";    break;
    case MemberType_Signal:      tagFile << "signal";      break;
    case MemberType_Friend:      tagFile << "friend";      break;
    case MemberType_DCOP:        tagFile << "dcop";        break;
    case MemberType_Slot:        tagFile << "slot";        break;
    case MemberType_Interface:   tagFile << "interface";   break;
    case MemberType_Service:     tagFile << "service";     break;
  }
  if (m_impl->prot!=Public)
  {
    tagFile << "\" protection=\"";
    if (m_impl->prot==Protected)    tagFile << "protected";
    else if (m_impl->prot==Package) tagFile << "package";
    else /* Private */              tagFile << "private";
  }
  if (m_impl->virt!=Normal)
  {
    tagFile << "\" virtualness=\"";
    if (m_impl->virt==Virtual) tagFile << "virtual";
    else /* Pure */            tagFile << "pure";
  }
  if (isStatic())
  {
    tagFile << "\" static=\"yes";
  }
  tagFile << "\">" << endl;
  if (typeString()!=QCString("@"))
  {
    tagFile << "      <type>" << convertToXML(typeString()) << "</type>" << endl;
  }
  tagFile << "      <name>" << convertToXML(name()) << "</name>" << endl;
  tagFile << "      <anchorfile>" << convertToXML(getOutputFileBase()+Doxygen::htmlFileExtension) << "</anchorfile>" << endl;
  tagFile << "      <anchor>" << convertToXML(anchor()) << "</anchor>" << endl;
  QCString idStr = id();
  if (!idStr.isEmpty())
  {
    tagFile << "      <clangid>" << convertToXML(idStr) << "</clangid>" << endl;
  }
  tagFile << "      <arglist>" << convertToXML(argsString()) << "</arglist>" << endl;
  if (isStrong())
  {
    MemberList *fmdl=m_impl->enumFields;
    if (fmdl)
    {
      MemberListIterator mli(*fmdl);
      MemberDef *fmd;
      for (mli.toFirst();(fmd=mli.current());++mli)
      {
        if (!fmd->isReference())
        {
          tagFile << "      <enumvalue file=\"" << convertToXML(getOutputFileBase()+Doxygen::htmlFileExtension);
          tagFile << "\" anchor=\"" << convertToXML(fmd->anchor());
          QCString idStr = fmd->id();
          if (!idStr.isEmpty())
          {
            tagFile << "\" clangid=\"" << convertToXML(idStr);
          }
          tagFile  << "\">" << convertToXML(fmd->name()) << "</enumvalue>" << endl;
        }
      }
    }
  }
  writeDocAnchorsToTagFile(tagFile);
  tagFile << "    </member>" << endl;
}

void MemberDef::_computeIsConstructor()
{
  m_isConstructorCached=1; // FALSE
  if (m_impl->classDef)
  {
    if (m_impl->isDMember) // for D
    {
      m_isConstructorCached = name()=="this" ? 2 : 1;
      return;
    }
    else if (getLanguage()==SrcLangExt_PHP) // for PHP
    {
      m_isConstructorCached = name()=="__construct" ? 2 : 1;
      return;
    }
    else if (name()=="__init__" &&
             getLanguage()==SrcLangExt_Python) // for Python
    {
      m_isConstructorCached = 2; // TRUE
      return;
    }
    else if (getLanguage()==SrcLangExt_Tcl) // for Tcl
    {
      m_isConstructorCached = name()=="constructor" ? 2 : 1;
      return;
    }
    else // for other languages
    {
      QCString locName = m_impl->classDef->localName();
      int i=locName.find('<');
      if (i==-1) // not a template class
      {
        m_isConstructorCached = name()==locName ? 2 : 1;
      }
      else
      {
        m_isConstructorCached = name()==locName.left(i) ? 2 : 1;
      }
      return;
    }
  }
}

bool MemberDef::isConstructor() const
{
  if (m_isConstructorCached==0)
  {
    MemberDef *that = (MemberDef*)this;
    that->_computeIsConstructor();
  }
  ASSERT(m_isConstructorCached>0);
  return m_isConstructorCached==2;

}

void MemberDef::_computeIsDestructor()
{
  bool isDestructor;
  if (m_impl->isDMember) // for D
  {
    isDestructor = name()=="~this";
  }
  else if (getLanguage()==SrcLangExt_PHP) // for PHP
  {
    isDestructor = name()=="__destruct";
  }
  else if (getLanguage()==SrcLangExt_Tcl) // for Tcl
  {
    isDestructor = name()=="destructor";
  }
  else if (name()=="__del__" &&
           getLanguage()==SrcLangExt_Python) // for Python
  {
    isDestructor=TRUE;
  }
  else // other languages
  {
    isDestructor =
           (name().find('~')!=-1 || name().find('!')!=-1)  // The ! is for C++/CLI
           && name().find("operator")==-1;
  }
  m_isDestructorCached = isDestructor ? 2 : 1;
}

bool MemberDef::isDestructor() const
{
  if (m_isDestructorCached==0)
  {
    MemberDef *that=(MemberDef*)this;
    that->_computeIsDestructor();
  }
  ASSERT(m_isDestructorCached>0);
  return m_isDestructorCached==2;
}

void MemberDef::writeEnumDeclaration(OutputList &typeDecl,
     ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd)
{
  int enumMemCount=0;

  MemberList *fmdl=m_impl->enumFields;
  uint numVisibleEnumValues=0;
  if (fmdl)
  {
    MemberListIterator mli(*fmdl);
    MemberDef *fmd;
    for (mli.toFirst();(fmd=mli.current());++mli)
    {
      if (fmd->isBriefSectionVisible()) numVisibleEnumValues++;
    }
  }
  if (numVisibleEnumValues==0 && !isBriefSectionVisible())
  {
    return;
  }

  QCString n = name();
  int i=n.findRev("::");
  if (i!=-1) n=n.right(n.length()-i-2); // strip scope (TODO: is this needed?)
  if (n[0]!='@') // not an anonymous enum
  {
    if (isLinkableInProject() || hasDocumentedEnumValues())
    {
      //_writeTagData(compoundType);
      _addToSearchIndex();
      writeLink(typeDecl,cd,nd,fd,gd);
    }
    else
    {
      typeDecl.startBold();
      typeDecl.docify(n);
      typeDecl.endBold();
    }
    typeDecl.writeChar(' ');
  }
  if (!m_impl->enumBaseType.isEmpty())
  {
    typeDecl.writeChar(':');
    typeDecl.writeChar(' ');
    typeDecl.docify(m_impl->enumBaseType);
    typeDecl.writeChar(' ');
  }

  uint enumValuesPerLine = (uint)Config_getInt(ENUM_VALUES_PER_LINE);
  if (numVisibleEnumValues>0 && enumValuesPerLine>0)
  {
    typeDecl.docify("{ ");
    if (fmdl)
    {
      MemberListIterator mli(*fmdl);
      MemberDef *fmd=mli.current();
      bool fmdVisible = fmd ? fmd->isBriefSectionVisible() : TRUE;
      while (fmd)
      {
        if (fmdVisible)
        {
          /* in html we start a new line after a number of items */
          if (numVisibleEnumValues>enumValuesPerLine
              && (enumMemCount%enumValuesPerLine)==0
             )
          {
            typeDecl.pushGeneratorState();
            typeDecl.disableAllBut(OutputGenerator::Html);
            typeDecl.enable(OutputGenerator::Latex);
            typeDecl.lineBreak();
            typeDecl.disable(OutputGenerator::Latex);
            typeDecl.writeString("&#160;&#160;");
            typeDecl.popGeneratorState();
          }

          if (fmd->hasDocumentation()) // enum value has docs
          {
            //fmd->_writeTagData(compoundType);
            fmd->_addToSearchIndex();
            fmd->writeLink(typeDecl,cd,nd,fd,gd);
          }
          else // no docs for this enum value
          {
            typeDecl.startBold();
            typeDecl.docify(fmd->name());
            typeDecl.endBold();
          }
          if (fmd->hasOneLineInitializer()) // enum value has initializer
          {
            //typeDecl.writeString(" = ");
            typeDecl.writeString(" ");
            typeDecl.parseText(fmd->initializer());
          }
        }

        bool prevVisible = fmdVisible;
        ++mli;
        fmd=mli.current();
        if (fmd && (fmdVisible=fmd->isBriefSectionVisible()))
        {
          typeDecl.writeString(", ");
        }
        if (prevVisible)
        {
          typeDecl.disable(OutputGenerator::Man);
          typeDecl.writeString("\n"); // to prevent too long lines in LaTeX
          typeDecl.enable(OutputGenerator::Man);
          enumMemCount++;
        }
      }
      if (numVisibleEnumValues>enumValuesPerLine)
      {
        typeDecl.pushGeneratorState();
        typeDecl.disableAllBut(OutputGenerator::Html);
        typeDecl.lineBreak();
        typeDecl.popGeneratorState();
      }
    }
    typeDecl.docify(" }");
  }
}

void MemberDef::setArgumentList(ArgumentList *al)
{
  if (m_impl->defArgList) delete m_impl->defArgList;
  m_impl->defArgList = al;
}

void MemberDef::setDeclArgumentList(ArgumentList *al)
{
  if (m_impl->declArgList) delete m_impl->declArgList;
  m_impl->declArgList = al;
}

void MemberDef::setTypeConstraints(ArgumentList *al)
{
  if (al==0) return;
  if (m_impl->typeConstraints) delete m_impl->typeConstraints;
  m_impl->typeConstraints = new ArgumentList;
  m_impl->typeConstraints->setAutoDelete(TRUE);
  ArgumentListIterator ali(*al);
  Argument *a;
  for (;(a=ali.current());++ali)
  {
    m_impl->typeConstraints->append(new Argument(*a));
  }
}

void MemberDef::setType(const char *t)
{
  m_impl->type = t;
}

void MemberDef::setAccessorType(ClassDef *cd,const char *t)
{
  m_impl->accessorClass = cd;
  m_impl->accessorType = t;
}

ClassDef *MemberDef::accessorClass() const
{
  return m_impl->accessorClass;
}

void MemberDef::findSectionsInDocumentation()
{
  docFindSections(documentation(),this,0,docFile());
}

void MemberDef::enableCallGraph(bool e)
{
  m_impl->hasCallGraph=e;
  if (e) Doxygen::parseSourcesNeeded = TRUE;
}

void MemberDef::enableCallerGraph(bool e)
{
  m_impl->hasCallerGraph=e;
  if (e) Doxygen::parseSourcesNeeded = TRUE;
}

#if 0
bool MemberDef::protectionVisible() const
{
  return m_impl->prot==Public ||
         (m_impl->prot==Private   && Config_getBool(EXTRACT_PRIVATE))   ||
         (m_impl->prot==Protected && Config_getBool(EXTRACT_PROTECTED)) ||
         (m_impl->prot==Package   && Config_getBool(EXTRACT_PACKAGE));
}
#endif

#if 0
void MemberDef::setInbodyDocumentation(const char *docs,
                  const char *docFile,int docLine)
{
  m_impl->inbodyDocs = docs;
  m_impl->inbodyDocs = m_impl->inbodyDocs.stripWhiteSpace();
  m_impl->inbodyLine = docLine;
  m_impl->inbodyFile = docFile;
}
#endif

bool MemberDef::isObjCMethod() const
{
  if (m_impl->classDef && m_impl->classDef->isObjectiveC() && isFunction()) return TRUE;
  return FALSE;
}

bool MemberDef::isObjCProperty() const
{
  if (m_impl->classDef && m_impl->classDef->isObjectiveC() && isProperty()) return TRUE;
  return FALSE;
}

QCString MemberDef::qualifiedName() const
{
  if (isObjCMethod())
  {
    QCString qm;
    if (isStatic()) qm="+"; else qm="-";
    qm+="[";
    qm+=m_impl->classDef->name()+" ";
    qm+=name();
    qm+="]";
    return qm;
  }
  else if (m_impl->enumScope && m_impl->enumScope->isStrong())
  {
    return m_impl->enumScope->qualifiedName()+
           getLanguageSpecificSeparator(getLanguage())+
           localName();
  }
  else
  {
    return Definition::qualifiedName();
  }
}

void MemberDef::setTagInfo(TagInfo *ti)
{
  if (ti)
  {
    //printf("%s: Setting tag name=%s anchor=%s\n",name().data(),ti->tagName.data(),ti->anchor.data());
    m_impl->anc=ti->anchor;
    setReference(ti->tagName);
    m_impl->explicitOutputFileBase = stripExtension(ti->fileName);
  }
}

QCString MemberDef::objCMethodName(bool localLink,bool showStatic) const
{
  QCString qm;
  if (showStatic)
  {
    if (isStatic()) qm="+ "; else qm="- ";
  }
  qm+=name();
  if (!localLink) // link to method of same class
  {
    qm+=" (";
    qm+=m_impl->classDef->name();
    qm+=")";
  }
  return qm;
}

const char *MemberDef::declaration() const
{
  return m_impl->decl;
}

const char *MemberDef::definition() const
{
  return m_impl->def;
}

const char *MemberDef::extraTypeChars() const
{
  return m_impl->extraTypeChars;
}

const char *MemberDef::typeString() const
{
  return m_impl->type;
}

const char *MemberDef::argsString() const
{
  return m_impl->args;
}

const char *MemberDef::excpString() const
{
  return m_impl->exception;
}

const char *MemberDef::bitfieldString() const
{
  return m_impl->bitfields;
}

const QCString &MemberDef::initializer() const
{
  return m_impl->initializer;
}

int MemberDef::initializerLines() const
{
  return m_impl->initLines;
}

uint64 MemberDef::getMemberSpecifiers() const
{
  return m_impl->memSpec;
}

ClassDef *MemberDef::getClassDef() const
{
  return m_impl->classDef;
}

FileDef  *MemberDef::getFileDef() const
{
  return m_impl->fileDef;
}

NamespaceDef* MemberDef::getNamespaceDef() const
{
  return m_impl->nspace;
}

const char *MemberDef::getReadAccessor() const
{
  return m_impl->read;
}

const char *MemberDef::getWriteAccessor() const
{
  return m_impl->write;
}

GroupDef *MemberDef::getGroupDef() const
{
  return m_impl->group;
}

Grouping::GroupPri_t MemberDef::getGroupPri() const
{
  return m_impl->grouppri;
}

const char *MemberDef::getGroupFileName() const
{
  return m_impl->groupFileName;
}

int MemberDef::getGroupStartLine() const
{
  return m_impl->groupStartLine;
}

bool MemberDef::getGroupHasDocs() const
{
  return m_impl->groupHasDocs;
}

Protection MemberDef::protection() const
{
  return m_impl->prot;
}

MemberType MemberDef::memberType() const
{
  return m_impl->mtype;
}

bool MemberDef::isSignal() const
{
  return m_impl->mtype==MemberType_Signal;
}

bool MemberDef::isSlot() const
{
  return m_impl->mtype==MemberType_Slot;
}

bool MemberDef::isVariable() const
{
  return m_impl->mtype==MemberType_Variable;
}

bool MemberDef::isEnumerate() const
{
  return m_impl->mtype==MemberType_Enumeration;
}

bool MemberDef::isEnumValue() const
{
  return m_impl->mtype==MemberType_EnumValue;
}

bool MemberDef::isTypedef() const
{
  return m_impl->mtype==MemberType_Typedef;
}

bool MemberDef::isFunction() const
{
  return m_impl->mtype==MemberType_Function;
}

bool MemberDef::isFunctionPtr() const
{
  return m_impl->mtype==MemberType_Variable && QCString(argsString()).find(")(")!=-1;
}

bool MemberDef::isDefine() const
{
  return m_impl->mtype==MemberType_Define;
}

bool MemberDef::isFriend() const
{
  return m_impl->mtype==MemberType_Friend;
}

bool MemberDef::isDCOP() const
{
  return m_impl->mtype==MemberType_DCOP;
}

bool MemberDef::isProperty() const
{
  return m_impl->mtype==MemberType_Property;
}

bool MemberDef::isEvent() const
{
  return m_impl->mtype==MemberType_Event;
}

bool MemberDef::isRelated() const
{
  return m_impl->related == Related;
}

bool MemberDef::isForeign() const
{
  return m_impl->related == Foreign;
}

bool MemberDef::isStatic() const
{
  return m_impl->stat;
}

bool MemberDef::isInline() const
{
  return (m_impl->memSpec&Entry::Inline)!=0;
}

bool MemberDef::isExplicit() const
{
  return (m_impl->memSpec&Entry::Explicit)!=0;
}

bool MemberDef::isMutable() const
{
  return (m_impl->memSpec&Entry::Mutable)!=0;
}

bool MemberDef::isGettable() const
{
  return (m_impl->memSpec&Entry::Gettable)!=0;
}

bool MemberDef::isPrivateGettable() const
{
  return (m_impl->memSpec&Entry::PrivateGettable)!=0;
}

bool MemberDef::isProtectedGettable() const
{
  return (m_impl->memSpec&Entry::ProtectedGettable)!=0;
}

bool MemberDef::isSettable() const
{
  return (m_impl->memSpec&Entry::Settable)!=0;
}

bool MemberDef::isPrivateSettable() const
{
  return (m_impl->memSpec&Entry::PrivateSettable)!=0;
}

bool MemberDef::isProtectedSettable() const
{
  return (m_impl->memSpec&Entry::ProtectedSettable)!=0;
}

bool MemberDef::isAddable() const
{
  return (m_impl->memSpec&Entry::Addable)!=0;
}

bool MemberDef::isRemovable() const
{
  return (m_impl->memSpec&Entry::Removable)!=0;
}

bool MemberDef::isRaisable() const
{
  return (m_impl->memSpec&Entry::Raisable)!=0;
}

bool MemberDef::isReadable() const
{
  return (m_impl->memSpec&Entry::Readable)!=0;
}

bool MemberDef::isWritable() const
{
  return (m_impl->memSpec&Entry::Writable)!=0;
}

bool MemberDef::isFinal() const
{
  return (m_impl->memSpec&Entry::Final)!=0;
}

bool MemberDef::isNew() const
{
  return (m_impl->memSpec&Entry::New)!=0;
}

bool MemberDef::isSealed() const
{
  return (m_impl->memSpec&Entry::Sealed)!=0;
}

bool MemberDef::isOverride() const
{
  return (m_impl->memSpec&Entry::Override)!=0;
}

bool MemberDef::isInitonly() const
{
  return (m_impl->memSpec&Entry::Initonly)!=0;
}

bool MemberDef::isAbstract() const
{
  return (m_impl->memSpec&Entry::Abstract)!=0;
}

bool MemberDef::isOptional() const
{
  return (m_impl->memSpec&Entry::Optional)!=0;
}

bool MemberDef::isRequired() const
{
  return (m_impl->memSpec&Entry::Required)!=0;
}

bool MemberDef::isNonAtomic() const
{
  return (m_impl->memSpec&Entry::NonAtomic)!=0;
}

bool MemberDef::isCopy() const
{
  return (m_impl->memSpec&Entry::Copy)!=0;
}

bool MemberDef::isAssign() const
{
  return (m_impl->memSpec&Entry::Assign)!=0;
}

bool MemberDef::isRetain() const
{
  return (m_impl->memSpec&Entry::Retain)!=0;
}

bool MemberDef::isWeak() const
{
  return (m_impl->memSpec&Entry::Weak)!=0;
}

bool MemberDef::isStrong() const
{
  return (m_impl->memSpec&Entry::Strong)!=0;
}

bool MemberDef::isStrongEnumValue() const
{
  return m_impl->mtype==MemberType_EnumValue &&
         m_impl->enumScope &&
         m_impl->enumScope->isStrong();
}

bool MemberDef::isUnretained() const
{
  return (m_impl->memSpec&Entry::Unretained)!=0;
}

bool MemberDef::isAlias() const
{
  return (m_impl->memSpec&Entry::Alias)!=0;
}

bool MemberDef::isDefault() const
{
  return (m_impl->memSpec&Entry::Default)!=0;
}

bool MemberDef::isDelete() const
{
  return (m_impl->memSpec&Entry::Delete)!=0;
}

bool MemberDef::isNoExcept() const
{
  return (m_impl->memSpec&Entry::NoExcept)!=0;
}

bool MemberDef::isAttribute() const
{
  return (m_impl->memSpec&Entry::Attribute)!=0;
}

bool MemberDef::isUNOProperty() const
{
  return (m_impl->memSpec&Entry::Property)!=0;
}

bool MemberDef::isReadonly() const
{
  return (m_impl->memSpec&Entry::Readonly)!=0;
}

bool MemberDef::isBound() const
{
  return (m_impl->memSpec&Entry::Bound)!=0;
}

bool MemberDef::isConstrained() const
{
  return (m_impl->memSpec&Entry::Constrained)!=0;
}

bool MemberDef::isTransient() const
{
  return (m_impl->memSpec&Entry::Transient)!=0;
}

bool MemberDef::isMaybeVoid() const
{
  return (m_impl->memSpec&Entry::MaybeVoid)!=0;
}

bool MemberDef::isMaybeDefault() const
{
  return (m_impl->memSpec&Entry::MaybeDefault)!=0;
}

bool MemberDef::isMaybeAmbiguous() const
{
  return (m_impl->memSpec&Entry::MaybeAmbiguous)!=0;
}

bool MemberDef::isPublished() const
{
  return (m_impl->memSpec&Entry::Published)!=0;
}


bool MemberDef::isImplementation() const
{
  return m_impl->implOnly;
}

bool MemberDef::isExternal() const
{
  return m_impl->explExt;
}

bool MemberDef::isTemplateSpecialization() const
{
  return m_impl->tspec;
}

bool MemberDef::hasDocumentedParams() const
{
  return m_impl->hasDocumentedParams;
}

bool MemberDef::hasDocumentedReturnType() const
{
  return m_impl->hasDocumentedReturnType;
}

bool MemberDef::showInCallGraph() const
{
  return isFunction() ||
         isSlot() ||
         isConstructor() ||
         isDestructor() ||
         isObjCMethod();
}

ClassDef *MemberDef::relatedAlso() const
{
  return m_impl->relatedAlso;
}

bool MemberDef::hasDocumentedEnumValues() const
{
  return m_impl->docEnumValues;
}

MemberDef *MemberDef::getAnonymousEnumType() const
{
  return m_impl->annEnumType;
}

bool MemberDef::isDocsForDefinition() const
{
  return m_impl->docsForDefinition;
}

MemberDef *MemberDef::getEnumScope() const
{
  return m_impl->enumScope;
}

bool MemberDef::livesInsideEnum() const
{
  return m_impl->livesInsideEnum;
}

MemberList *MemberDef::enumFieldList() const
{
  return m_impl->enumFields;
}

ExampleSDict *MemberDef::getExamples() const
{
  return m_impl->exampleSDict;
}

bool MemberDef::isPrototype() const
{
  return m_impl->proto;
}

ArgumentList *MemberDef::argumentList() const
{
  return m_impl->defArgList;
}

ArgumentList *MemberDef::declArgumentList() const
{
  return m_impl->declArgList;
}

ArgumentList *MemberDef::templateArguments() const
{
  return m_impl->tArgList;
}

QList<ArgumentList> *MemberDef::definitionTemplateParameterLists() const
{
  return m_impl->defTmpArgLists;
}

int MemberDef::getMemberGroupId() const
{
  return m_impl->grpId;
}

MemberGroup *MemberDef::getMemberGroup() const
{
  return m_impl->memberGroup;
}

bool MemberDef::fromAnonymousScope() const
{
  return m_impl->annScope;
}

bool MemberDef::anonymousDeclShown() const
{
  return m_impl->annUsed;
}

void MemberDef::setAnonymousUsed()
{
  m_impl->annUsed = TRUE;
}

bool MemberDef::hasCallGraph() const
{
  return m_impl->hasCallGraph;
}

bool MemberDef::hasCallerGraph() const
{
  return m_impl->hasCallerGraph;
}

MemberDef *MemberDef::templateMaster() const
{
  return m_impl->templateMaster;
}

bool MemberDef::isTypedefValCached() const
{
  return m_impl->isTypedefValCached;
}

ClassDef *MemberDef::getCachedTypedefVal() const
{
  return m_impl->cachedTypedefValue;
}

QCString MemberDef::getCachedTypedefTemplSpec() const
{
  return m_impl->cachedTypedefTemplSpec;
}

QCString MemberDef::getCachedResolvedTypedef() const
{
  //printf("MemberDef::getCachedResolvedTypedef()=%s m_impl=%p\n",m_impl->cachedResolvedType.data(),m_impl);
  return m_impl->cachedResolvedType;
}

MemberDef *MemberDef::memberDefinition() const
{
  return m_impl->memDef;
}

MemberDef *MemberDef::memberDeclaration() const
{
  return m_impl->memDec;
}

MemberDef *MemberDef::inheritsDocsFrom() const
{
  return m_impl->docProvider;
}

MemberDef *MemberDef::getGroupAlias() const
{
  return m_impl->groupAlias;
}

void MemberDef::setMemberType(MemberType t)
{
  m_impl->mtype=t;
  m_isLinkableCached = 0;
}

void MemberDef::setDefinition(const char *d)
{
  m_impl->def=d;
}

void MemberDef::setFileDef(FileDef *fd)
{
  m_impl->fileDef=fd;
  m_isLinkableCached = 0;
  m_isConstructorCached = 0;
  m_isDestructorCached = 0;
}

void MemberDef::setProtection(Protection p)
{
  m_impl->prot=p;
  m_isLinkableCached = 0;
}

void MemberDef::setMemberSpecifiers(uint64 s)
{
  m_impl->memSpec=s;
}

void MemberDef::mergeMemberSpecifiers(uint64 s)
{
  m_impl->memSpec|=s;
}

void MemberDef::setBitfields(const char *s)
{
  m_impl->bitfields = QCString(s).simplifyWhiteSpace();
}

void MemberDef::setMaxInitLines(int lines)
{
  if (lines!=-1)
  {
    m_impl->userInitLines=lines;
  }
}

void MemberDef::setExplicitExternal(bool b)
{
  m_impl->explExt=b;
}

void MemberDef::setReadAccessor(const char *r)
{
  m_impl->read=r;
}

void MemberDef::setWriteAccessor(const char *w)
{
  m_impl->write=w;
}

void MemberDef::setTemplateSpecialization(bool b)
{
  m_impl->tspec=b;
}

void MemberDef::makeRelated()
{
  m_impl->related = Related;
  m_isLinkableCached = 0;
}

void MemberDef::makeForeign()
{
  m_impl->related = Foreign;
  m_isLinkableCached = 0;
}

void MemberDef::setHasDocumentedParams(bool b)
{
  m_impl->hasDocumentedParams = b;
}

void MemberDef::setHasDocumentedReturnType(bool b)
{
  m_impl->hasDocumentedReturnType = b;
}

void MemberDef::setInheritsDocsFrom(MemberDef *md)
{
  m_impl->docProvider = md;
}

void MemberDef::setArgsString(const char *as)
{
  m_impl->args = as;
}

void MemberDef::setRelatedAlso(ClassDef *cd)
{
  m_impl->relatedAlso=cd;
}

void MemberDef::setEnumClassScope(ClassDef *cd)
{
  m_impl->classDef = cd;
  m_isLinkableCached = 0;
  m_isConstructorCached = 0;
}

void MemberDef::setDocumentedEnumValues(bool value)
{
  m_impl->docEnumValues=value;
}

void MemberDef::setAnonymousEnumType(MemberDef *md)
{
  m_impl->annEnumType = md;
}

void MemberDef::setPrototype(bool p)
{
  m_impl->proto=p;
}

void MemberDef::setMemberGroupId(int id)
{
  m_impl->grpId=id;
}

void MemberDef::makeImplementationDetail()
{
  m_impl->implOnly=TRUE;
}

void MemberDef::setFromAnonymousScope(bool b)
{
  m_impl->annScope=b;
}

void MemberDef::setFromAnonymousMember(MemberDef *m)
{
  m_impl->annMemb=m;
}

MemberDef *MemberDef::fromAnonymousMember() const
{
  return m_impl->annMemb;
}

void MemberDef::setTemplateMaster(MemberDef *mt)
{
  m_impl->templateMaster=mt;
  m_isLinkableCached = 0;
}

void MemberDef::setDocsForDefinition(bool b)
{
  m_impl->docsForDefinition = b;
}

void MemberDef::setGroupAlias(MemberDef *md)
{
  m_impl->groupAlias = md;
}

void MemberDef::invalidateTypedefValCache()
{
  m_impl->isTypedefValCached=FALSE;
}

void MemberDef::setMemberDefinition(MemberDef *md)
{
  m_impl->memDef=md;
}

void MemberDef::setMemberDeclaration(MemberDef *md)
{
  m_impl->memDec=md;
}

ClassDef *MemberDef::category() const
{
  return m_impl->category;
}

void MemberDef::setCategory(ClassDef *def)
{
  m_impl->category = def;
}

MemberDef *MemberDef::categoryRelation() const
{
  return m_impl->categoryRelation;
}

void MemberDef::setCategoryRelation(MemberDef *md)
{
  m_impl->categoryRelation = md;
}

void MemberDef::setEnumBaseType(const QCString &type)
{
  m_impl->enumBaseType = type;
}

QCString MemberDef::enumBaseType() const
{
  return m_impl->enumBaseType;
}


void MemberDef::cacheTypedefVal(ClassDef*val, const QCString & templSpec, const QCString &resolvedType)
{
  m_impl->isTypedefValCached=TRUE;
  m_impl->cachedTypedefValue=val;
  m_impl->cachedTypedefTemplSpec=templSpec;
  m_impl->cachedResolvedType=resolvedType;
  //printf("MemberDef::cacheTypedefVal=%s m_impl=%p\n",m_impl->cachedResolvedType.data(),m_impl);
}

void MemberDef::copyArgumentNames(MemberDef *bmd)
{
  {
    ArgumentList *arguments = bmd->argumentList();
    if (m_impl->defArgList && arguments)
    {
      ArgumentListIterator aliDst(*m_impl->defArgList);
      ArgumentListIterator aliSrc(*arguments);
      Argument *argDst, *argSrc;
      for (;(argDst=aliDst.current()) && (argSrc=aliSrc.current());++aliDst,++aliSrc)
      {
        argDst->name = argSrc->name;
      }
    }
  }
  {
    ArgumentList *arguments = bmd->declArgumentList();
    if (m_impl->declArgList && arguments)
    {
      ArgumentListIterator aliDst(*m_impl->declArgList);
      ArgumentListIterator aliSrc(*arguments);
      Argument *argDst, *argSrc;
      for (;(argDst=aliDst.current()) && (argSrc=aliSrc.current());++aliDst,++aliSrc)
      {
        argDst->name = argSrc->name;
      }
    }
  }
}

static void invalidateCachedTypesInArgumentList(ArgumentList *al)
{
  if (al)
  {
    ArgumentListIterator ali(*al);
    Argument *a;
    for (ali.toFirst();(a=ali.current());++ali)
    {
      a->canType.resize(0);
    }
  }
}

void MemberDef::invalidateCachedArgumentTypes()
{
  invalidateCachedTypesInArgumentList(m_impl->defArgList);
  invalidateCachedTypesInArgumentList(m_impl->declArgList);
}

//----------------

QCString MemberDef::displayName(bool) const
{
  return Definition::name();
}

void MemberDef::_addToSearchIndex()
{
  // write search index info
  if (Doxygen::searchIndex && isLinkableInProject())
  {
    Doxygen::searchIndex->setCurrentDoc(this,anchor(),FALSE);
    QCString ln=localName(),qn=qualifiedName();
    Doxygen::searchIndex->addWord(ln,TRUE);
    if (ln!=qn)
    {
      Doxygen::searchIndex->addWord(qn,TRUE);
      if (getClassDef())
      {
        Doxygen::searchIndex->addWord(getClassDef()->displayName(),TRUE);
      }
      else if (getNamespaceDef())
      {
        Doxygen::searchIndex->addWord(getNamespaceDef()->displayName(),TRUE);
      }
    }
  }
}



//----------------

static void transferArgumentDocumentation(ArgumentList *decAl,ArgumentList *defAl)
{
  if (decAl && defAl)
  {
    ArgumentListIterator decAli(*decAl);
    ArgumentListIterator defAli(*defAl);
    Argument *decA,*defA;
    for (decAli.toFirst(),defAli.toFirst();
        (decA=decAli.current()) && (defA=defAli.current());
        ++decAli,++defAli)
    {
      //printf("Argument decA->name=%s (doc=%s) defA->name=%s (doc=%s)\n",
      //    decA->name.data(),decA->docs.data(),
      //    defA->name.data(),defA->docs.data()
      //      );
      if (decA->docs.isEmpty() && !defA->docs.isEmpty())
      {
        decA->docs = defA->docs.copy();
      }
      else if (defA->docs.isEmpty() && !decA->docs.isEmpty())
      {
        defA->docs = decA->docs.copy();
      }
    }
  }
}

void combineDeclarationAndDefinition(MemberDef *mdec,MemberDef *mdef)
{
  //printf("mdec=%s isPrototype()=%d\n",mdec->name().data(),mdec->isPrototype());
  if (
      (mdef->isFunction() && !mdef->isStatic() && !mdef->isPrototype()) ||
      (mdef->isVariable() && !mdef->isExternal() && !mdef->isStatic())
     )
  {
    //printf("mdef=(%p,%s) mdec=(%p,%s)\n",
    //    mdef, mdef ? mdef->name().data() : "",
    //    mdec, mdec ? mdec->name().data() : "");

    ArgumentList *mdefAl = mdef->argumentList();
    ArgumentList *mdecAl = mdec->argumentList();
    if (matchArguments2(mdef->getOuterScope(),mdef->getFileDef(),mdefAl,
          mdec->getOuterScope(),mdec->getFileDef(),mdecAl,
          TRUE
          )
       ) /* match found */
    {
      //printf("Found member %s: definition in %s (doc=`%s') and declaration in %s (doc=`%s')\n",
      //    mn->memberName(),
      //    mdef->getFileDef()->name().data(),mdef->documentation().data(),
      //    mdec->getFileDef()->name().data(),mdec->documentation().data()
      //    );

      // first merge argument documentation
      transferArgumentDocumentation(mdecAl,mdefAl);

      /* copy documentation between function definition and declaration */
      if (!mdec->briefDescription().isEmpty())
      {
        mdef->setBriefDescription(mdec->briefDescription(),mdec->briefFile(),mdec->briefLine());
      }
      else if (!mdef->briefDescription().isEmpty())
      {
        mdec->setBriefDescription(mdef->briefDescription(),mdef->briefFile(),mdef->briefLine());
      }
      if (!mdef->documentation().isEmpty())
      {
        //printf("transferring docs mdef->mdec (%s->%s)\n",mdef->argsString(),mdec->argsString());
        mdec->setDocumentation(mdef->documentation(),mdef->docFile(),mdef->docLine());
        mdec->setDocsForDefinition(mdef->isDocsForDefinition());
        if (mdefAl!=0)
        {
          ArgumentList *mdefAlComb = new ArgumentList;
          stringToArgumentList(mdef->argsString(),mdefAlComb);
          transferArgumentDocumentation(mdefAl,mdefAlComb);
          mdec->setArgumentList(mdefAlComb);
        }
      }
      else if (!mdec->documentation().isEmpty())
      {
        //printf("transferring docs mdec->mdef (%s->%s)\n",mdec->argsString(),mdef->argsString());
        mdef->setDocumentation(mdec->documentation(),mdec->docFile(),mdec->docLine());
        mdef->setDocsForDefinition(mdec->isDocsForDefinition());
        if (mdecAl!=0)
        {
          ArgumentList *mdecAlComb = new ArgumentList;
          stringToArgumentList(mdec->argsString(),mdecAlComb);
          transferArgumentDocumentation(mdecAl,mdecAlComb);
          mdef->setDeclArgumentList(mdecAlComb);
        }
      }
      if (!mdef->inbodyDocumentation().isEmpty())
      {
        mdec->setInbodyDocumentation(mdef->inbodyDocumentation(),mdef->inbodyFile(),mdef->inbodyLine());
      }
      else if (!mdec->inbodyDocumentation().isEmpty())
      {
        mdef->setInbodyDocumentation(mdec->inbodyDocumentation(),mdec->inbodyFile(),mdec->inbodyLine());
      }
      if (mdec->getStartBodyLine()!=-1 && mdef->getStartBodyLine()==-1)
      {
        //printf("body mdec->mdef %d-%d\n",mdec->getStartBodyLine(),mdef->getEndBodyLine());
        mdef->setBodySegment(mdec->getStartBodyLine(),mdec->getEndBodyLine());
        mdef->setBodyDef(mdec->getBodyDef());
        //mdef->setBodyMember(mdec);
      }
      else if (mdef->getStartBodyLine()!=-1 && mdec->getStartBodyLine()==-1)
      {
        //printf("body mdef->mdec %d-%d\n",mdef->getStartBodyLine(),mdec->getEndBodyLine());
        mdec->setBodySegment(mdef->getStartBodyLine(),mdef->getEndBodyLine());
        mdec->setBodyDef(mdef->getBodyDef());
        //mdec->setBodyMember(mdef);
      }
      mdec->mergeMemberSpecifiers(mdef->getMemberSpecifiers());
      mdef->mergeMemberSpecifiers(mdec->getMemberSpecifiers());


      // copy group info.
      if (mdec->getGroupDef()==0 && mdef->getGroupDef()!=0)
      {
        mdec->setGroupDef(mdef->getGroupDef(),
            mdef->getGroupPri(),
            mdef->docFile(),
            mdef->docLine(),
            mdef->hasDocumentation(),
            mdef
            );
      }
      else if (mdef->getGroupDef()==0 && mdec->getGroupDef()!=0)
      {
        mdef->setGroupDef(mdec->getGroupDef(),
            mdec->getGroupPri(),
            mdec->docFile(),
            mdec->docLine(),
            mdec->hasDocumentation(),
            mdec
            );
      }


      mdec->mergeRefItems(mdef);
      mdef->mergeRefItems(mdec);

      mdef->setMemberDeclaration(mdec);
      mdec->setMemberDefinition(mdef);

      mdef->enableCallGraph(mdec->hasCallGraph() || mdef->hasCallGraph());
      mdef->enableCallerGraph(mdec->hasCallerGraph() || mdef->hasCallerGraph());
      mdec->enableCallGraph(mdec->hasCallGraph() || mdef->hasCallGraph());
      mdec->enableCallerGraph(mdec->hasCallerGraph() || mdef->hasCallerGraph());
    }
  }
}

QCString MemberDef::briefDescription(bool abbr) const
{
  if (m_impl->templateMaster)
  {
    return m_impl->templateMaster->briefDescription(abbr);
  }
  else
  {
    return Definition::briefDescription(abbr);
  }
}

QCString MemberDef::documentation() const
{
  if (m_impl->templateMaster)
  {
    return m_impl->templateMaster->documentation();
  }
  else
  {
    return Definition::documentation();
  }
}

const ArgumentList *MemberDef::typeConstraints() const
{
  return m_impl->typeConstraints;
}

bool MemberDef::isFriendToHide() const
{
  static bool hideFriendCompounds = Config_getBool(HIDE_FRIEND_COMPOUNDS);
  bool isFriendToHide = hideFriendCompounds &&
     (m_impl->type=="friend class"  ||
      m_impl->type=="friend struct" ||
      m_impl->type=="friend union");
  return isFriendToHide;
}

bool MemberDef::isNotFriend() const
{
  return !(isFriend() && isFriendToHide());
}

bool MemberDef::isFunctionOrSignalSlot() const
{
  return isFunction() || isSlot() || isSignal();
}

bool MemberDef::isRelatedOrFriend() const
{
  return isRelated() || isForeign() || (isFriend() && !isFriendToHide());
}

bool MemberDef::isReference() const
{
  return Definition::isReference() ||
         (m_impl->templateMaster && m_impl->templateMaster->isReference());
}

