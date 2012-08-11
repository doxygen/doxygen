/******************************************************************************
 *
 * 
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
#include "vhdlscanner.h"
#include "vhdldocgen.h"
#include "arguments.h"

#define START_MARKER 0x4D454D5B // MEM[
#define END_MARKER   0x4D454D5D // MEM]

#if defined(_OS_WIN32_)
#define snprintf _snprintf
#endif

// Put this macro at the start of any method of MemberDef that can directly
// or indirectly access other MemberDefs. It prevents that the content 
// pointed to by m_impl gets flushed to disk in the middle of the method call!
#define KEEP_RESIDENT_DURING_CALL makeResident();LockingPtr<MemberDef> lock(this,this)

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

static bool writeDefArgumentList(OutputList &ol,ClassDef *cd,
                                 const QCString & /*scopeName*/,MemberDef *md)
{
  LockingPtr<ArgumentList> defArgList=(md->isDocsForDefinition()) ? 
                             md->argumentList() : md->declArgumentList();
  //printf("writeDefArgumentList `%s' isDocsForDefinition()=%d\n",md->name().data(),md->isDocsForDefinition());
  if (defArgList==0 || md->isProperty()) 
  {
    return FALSE; // member has no function like argument list
  }
  if (!md->isDefine()) ol.docify(" ");

  // simple argument list for tcl
  if (md->getLanguage()==SrcLangExt_Tcl)
  {
    Argument *a=defArgList->first();
    ol.startParameterName(FALSE);
    while (a) 
    {
      if (a->defval.isEmpty())
      {
        ol.docify(a->name+" ");
      }
      else
      {
        ol.docify("?"+a->name+"? ");
      }
      a=defArgList->next();
    }
    ol.endParameterName(FALSE,FALSE,FALSE);
    ol.endMemberDocName();
    return TRUE;
  }

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

  Argument *a=defArgList->first();
  QCString cName;
  if (cd)
  {
    cName=cd->name();
    int il=cName.find('<');
    int ir=cName.findRev('>');
    if (il!=-1 && ir!=-1 && ir>il)
    {
      cName=cName.mid(il,ir-il+1);
      //printf("1. cName=%s\n",cName.data());
    }
    else if (cd->templateArguments())
    {
      cName=tempArgListToString(cd->templateArguments()); 
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
      if (!cName.isEmpty()) n=addTemplateNames(n,cd->name(),cName);
      linkifyText(TextGeneratorOLImpl(ol),cd,md->getBodyDef(),md,n);
    }
    else // non-function pointer type
    {
      QCString n=a->type;
      if (md->isObjCMethod()) { n.prepend("("); n.append(")"); }
      if (a->type!="...")
      {
        if (!cName.isEmpty()) n=addTemplateNames(n,cd->name(),cName);
        linkifyText(TextGeneratorOLImpl(ol),cd,md->getBodyDef(),md,n);
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
    if (!a->name.isEmpty() || (a->name.isEmpty() && a->type=="...")) // argument has a name
    { 
      //if (!hasFuncPtrType)
      //{
      //  ol.docify(" ");
      //}
      ol.disable(OutputGenerator::Man);
      ol.disable(OutputGenerator::Latex);
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
      linkifyText(TextGeneratorOLImpl(ol),cd,md->getBodyDef(),
                  md,a->type.right(a->type.length()-vp));
    }
    if (!a->defval.isEmpty()) // write the default value
    {
      QCString n=a->defval;
      if (!cName.isEmpty()) n=addTemplateNames(n,cd->name(),cName);
      ol.docify(" = ");

      ol.startTypewriter();
      linkifyText(TextGeneratorOLImpl(ol),cd,md->getBodyDef(),md,n,FALSE,TRUE,TRUE); 
      ol.endTypewriter();

    }
    a=defArgList->next();
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
                cd,                      // scope
                md->getBodyDef(),        // fileScope
                md,                      // self
                defArgList->trailingReturnType, // text
                FALSE                    // autoBreak
               ); 

  }
  return TRUE;
}

static void writeTemplatePrefix(OutputList &ol,ArgumentList *al)
{
  ol.docify("template<");
  Argument *a=al->first();
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
    a=al->next();
    if (a) ol.docify(", ");
  }
  ol.docify("> ");
}

QCString extractDirection(QCString &docs)
{
  QRegExp re("\\[[^\\]]+\\]"); // [...]
  int l=0;
  if (re.match(docs,0,&l)==0)
  {
    int  inPos  = docs.find("in", 1,FALSE);
    int outPos  = docs.find("out",1,FALSE);
    bool input  =  inPos!=-1 &&  inPos<l;
    bool output = outPos!=-1 && outPos<l;
    if (input || output) // in,out attributes
    {
      docs = docs.mid(l); // strip attributes
      if (input && output) return "[in,out]";
      else if (input)      return "[in]";
      else if (output)     return "[out]";
    }
  }
  return QCString();
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
              MemberDef::MemberType mt,const ArgumentList *tal,
              const ArgumentList *al
             );

    ClassDef     *classDef;   // member of or related to 
    FileDef      *fileDef;    // member of file definition 
    NamespaceDef *nspace;     // the namespace this member is in.

    MemberDef  *enumScope;    // the enclosing scope, if this is an enum field
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

    int  memSpec;             // The specifiers present for this member
    MemberDef::MemberType mtype;         // returns the kind of member
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
                     MemberDef::MemberType mt,const ArgumentList *tal,
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
  defTmpArgLists=0;
  hasCallGraph = FALSE;
  hasCallerGraph = FALSE;
  initLines=0;
  type=t;
  if (mt==MemberDef::Typedef) type.stripPrefix("typedef ");
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
  maxInitLines=Config_getInt("MAX_INITIALIZER_LINES");
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
 * \param mt The kind of member. See #MemberDef::MemberType for a list of 
 *           all types.
 * \param tal The template arguments of this member.
 * \param al  The arguments of this member. This is a structured form of 
 *            the string past as argument \a a.
 */

MemberDef::MemberDef(const char *df,int dl,
                     const char *t,const char *na,const char *a,const char *e,
                     Protection p,Specifier v,bool s,Relationship r,MemberType mt,
                     const ArgumentList *tal,const ArgumentList *al
                    ) : Definition(df,dl,removeRedundantWhiteSpace(na))
{
  //printf("MemberDef::MemberDef(%s)\n",na);
  m_storagePos=-1;
  m_cacheHandle=-1;
  m_impl = new MemberDefImpl;
  m_impl->init(this,t,a,e,p,v,s,r,mt,tal,al);
  m_flushPending = FALSE;
  m_isLinkableCached    = 0;
  m_isConstructorCached = 0;
  m_isDestructorCached  = 0;
}

MemberDef::MemberDef(const MemberDef &md) : Definition(md)
{
  m_storagePos=-1;
  m_cacheHandle=-1;
  m_impl = new MemberDefImpl;
  m_flushPending = FALSE;
  m_isLinkableCached    = 0;
  m_isConstructorCached = 0;
  m_isDestructorCached  = 0;
}

MemberDef *MemberDef::deepCopy() const
{
  makeResident();
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
    MemberListIterator mli(*m_impl->redefinedBy);
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
  if (m_cacheHandle!=-1)
  {
    Doxygen::symbolCache->del(m_cacheHandle);
    m_cacheHandle=-1;
  }
}

void MemberDef::setReimplements(MemberDef *md)   
{ 
  makeResident();

  m_impl->redefines = md;
}

void MemberDef::insertReimplementedBy(MemberDef *md)
{
  makeResident();
  if (m_impl->templateMaster)
  {
    m_impl->templateMaster->insertReimplementedBy(md);
  }
  if (m_impl->redefinedBy==0) m_impl->redefinedBy = new MemberList(MemberList::redefinedBy);
  if (m_impl->redefinedBy->findRef(md)==-1) 
  {
    m_impl->redefinedBy->inSort(md);
  }
}

MemberDef *MemberDef::reimplements() const      
{ 
  makeResident();
  return m_impl->redefines; 
}

LockingPtr<MemberList> MemberDef::reimplementedBy() const   
{ 
  makeResident();
  return LockingPtr<MemberList>(this,m_impl->redefinedBy); 
}

bool MemberDef::isReimplementedBy(ClassDef *cd) const
{
  makeResident();
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
  makeResident();
  if (m_impl->enumFields==0) m_impl->enumFields=new MemberList(MemberList::enumFields);
  m_impl->enumFields->append(md);
}

bool MemberDef::addExample(const char *anchor,const char *nameStr,
                           const char *file)
{
  makeResident();
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
  makeResident();
  if (m_impl->exampleSDict==0) 
    return FALSE;
  else
    return m_impl->exampleSDict->count()>0;
}

QCString MemberDef::getOutputFileBase() const
{
  makeResident();
  static bool separateMemberPages = Config_getBool("SEPARATE_MEMBER_PAGES");
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
  }
  else if (m_impl->nspace)
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
       "warning: Internal inconsistency: member %s does not belong to any"
       " container!",qPrint(name())
      );
    return "dummy";
  }
  else if (separateMemberPages)
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
  makeResident();
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
  KEEP_RESIDENT_DURING_CALL;
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
  KEEP_RESIDENT_DURING_CALL;
  static bool extractStatic  = Config_getBool("EXTRACT_STATIC");
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
  if (!m_impl->group && m_impl->nspace && !m_impl->related && !m_impl->nspace->isLinkableInProject()) 
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
  if (!protectionLevelVisible(m_impl->prot) && m_impl->mtype!=Friend) 
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
  makeResident();
  Definition::setDocumentation(d,docFile,docLine,stripWhiteSpace);
  m_isLinkableCached = 0;
}

void MemberDef::setBriefDescription(const char *b,const char *briefFile,int briefLine)
{
  makeResident();
  Definition::setBriefDescription(b,briefFile,briefLine);
  m_isLinkableCached = 0;
}

void MemberDef::setInbodyDocumentation(const char *d,const char *inbodyFile,int inbodyLine)
{
  makeResident();
  Definition::setInbodyDocumentation(d,inbodyFile,inbodyLine);
  m_isLinkableCached = 0;
}

void MemberDef::setHidden(bool b)
{
  makeResident();
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
  makeResident();
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
    makeResident();
    if (m_impl->defTmpArgLists) delete m_impl->defTmpArgLists;
    m_impl->defTmpArgLists = copyArgumentLists(lists);
  }
}

void MemberDef::writeLink(OutputList &ol,ClassDef *,NamespaceDef *,
                      FileDef *fd,GroupDef *gd,bool onlyText)
{
  KEEP_RESIDENT_DURING_CALL;
  SrcLangExt lang = getLanguage();
  static bool hideScopeNames     = Config_getBool("HIDE_SCOPE_NAMES");
  QCString sep = getLanguageSpecificSeparator(lang,TRUE);
  QCString n = name();
  if (!hideScopeNames)
  {
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
    if (m_impl->mtype==EnumValue && getGroupDef()==0 &&          // enum value is not grouped
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
  // split KEEP_RESIDENT_DURING_CALL for performance
  makeResident();
  if (m_impl->cachedAnonymousType) return m_impl->cachedAnonymousType;
  LockingPtr<MemberDef> lock(this,this); // since this memberDef can access 
                                         // other memberDefs prevent it from 
                                         // being flushed to disk halfway

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
  static bool extractStatic       = Config_getBool("EXTRACT_STATIC");
  static bool hideUndocMembers    = Config_getBool("HIDE_UNDOC_MEMBERS");
  static bool briefMemberDesc     = Config_getBool("BRIEF_MEMBER_DESC");
  static bool repeatBrief         = Config_getBool("REPEAT_BRIEF");
  static bool hideFriendCompounds = Config_getBool("HIDE_FRIEND_COMPOUNDS");

  //printf("Member %s grpId=%d docs=%s file=%s args=%s\n",
  //    name().data(),
  //    0,"", //grpId,grpId==-1?"<none>":Doxygen::memberDocDict[grpId]->data(),
  //    "", //getFileDef()->name().data(),
  //    argsString());

  KEEP_RESIDENT_DURING_CALL;

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
                           m_impl->mtype==Friend
                          );
  
  // hide member if it overrides a member in a superclass and has no
  // documentation of its own
  //bool visibleIfDocVirtual = !reimplements() || 
  //                           !Config_getBool("INHERIT_DOCS") ||  
  //                           hasDocs;

  // true if this member is a constructor or destructor
  bool cOrDTor = isConstructor() || isDestructor();

  // hide default constructors or destructors (no args) without
  // documentation
  bool visibleIfNotDefaultCDTor = !(cOrDTor &&
                                   m_impl->defArgList &&
                                   (m_impl->defArgList->isEmpty() ||
                                    m_impl->defArgList->first()->type == "void"
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


void MemberDef::writeDeclaration(OutputList &ol,
               ClassDef *cd,NamespaceDef *nd,FileDef *fd,GroupDef *gd,
               bool inGroup,ClassDef *inheritedFrom,const char *inheritId
               )
{
  //printf("%s MemberDef::writeDeclaration() inGroup=%d\n",name().data(),inGroup);

  // hide enum value, since they appear already as part of the enum, unless they
  // are explicitly grouped.
  KEEP_RESIDENT_DURING_CALL;
  if (!inGroup && m_impl->mtype==EnumValue) return;

  // hide members whose brief section should not be visible
  //if (!isBriefSectionVisible()) return;

  Definition *d=0;
  ASSERT (cd!=0 || nd!=0 || fd!=0 || gd!=0); // member should belong to something
  if (cd) d=cd; else if (nd) d=nd; else if (fd) d=fd; else d=gd;

  // write tag file information of this member
  if (!Config_getString("GENERATE_TAGFILE").isEmpty() && !isReference())
  {
    Doxygen::tagFile << "    <member kind=\"";
    switch (m_impl->mtype)
    {
      case Define:      Doxygen::tagFile << "define";      break;
      case EnumValue:   Doxygen::tagFile << "enumvalue";   break;
      case Property:    Doxygen::tagFile << "property";    break;
      case Event:       Doxygen::tagFile << "event";       break;
      case Variable:    Doxygen::tagFile << "variable";    break;
      case Typedef:     Doxygen::tagFile << "typedef";     break;
      case Enumeration: Doxygen::tagFile << "enumeration"; break;
      case Function:    Doxygen::tagFile << "function";    break;
      case Signal:      Doxygen::tagFile << "signal";      break;
      //case Prototype:   Doxygen::tagFile << "prototype";   break;
      case Friend:      Doxygen::tagFile << "friend";      break;
      case DCOP:        Doxygen::tagFile << "dcop";        break;
      case Slot:        Doxygen::tagFile << "slot";        break;
    }
    if (m_impl->prot!=Public)
    {
      Doxygen::tagFile << "\" protection=\"";
      if (m_impl->prot==Protected)    Doxygen::tagFile << "protected";
      else if (m_impl->prot==Package) Doxygen::tagFile << "package";
      else /* Private */              Doxygen::tagFile << "private"; 
    }
    if (m_impl->virt!=Normal)
    {
      Doxygen::tagFile << "\" virtualness=\"";
      if (m_impl->virt==Virtual) Doxygen::tagFile << "virtual";
      else /* Pure */            Doxygen::tagFile << "pure"; 
    }
    if (isStatic())
    {
      Doxygen::tagFile << "\" static=\"yes";
    }
    Doxygen::tagFile << "\">" << endl;
    Doxygen::tagFile << "      <type>" << convertToXML(typeString()) << "</type>" << endl;
    Doxygen::tagFile << "      <name>" << convertToXML(name()) << "</name>" << endl;
    Doxygen::tagFile << "      <anchorfile>" << convertToXML(getOutputFileBase()+Doxygen::htmlFileExtension) << "</anchorfile>" << endl;
    Doxygen::tagFile << "      <anchor>" << convertToXML(anchor()) << "</anchor>" << endl;
    Doxygen::tagFile << "      <arglist>" << convertToXML(argsString()) << "</arglist>" << endl;
    writeDocAnchorsToTagFile();
    Doxygen::tagFile << "    </member>" << endl;
  }

  // write search index info
  if (Doxygen::searchIndex && isLinkableInProject())
  {
    Doxygen::searchIndex->setCurrentDoc(this,anchor(),FALSE);
    Doxygen::searchIndex->addWord(localName(),TRUE);
    Doxygen::searchIndex->addWord(qualifiedName(),FALSE);
  }

  QCString cname  = d->name();
  QCString cdname = d->displayName();
  QCString cfname = getOutputFileBase();
  //QCString osname = cname;
  // in case of class members that are put in a group the name of the outerscope
  // differs from the cname.
  //if (getOuterScope()) osname=getOuterScope()->name();

  //HtmlHelp *htmlHelp=0;
  //bool hasHtmlHelp = Config_getBool("GENERATE_HTML") && Config_getBool("GENERATE_HTMLHELP");
  //if (hasHtmlHelp) htmlHelp = HtmlHelp::getInstance();

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
  if (!detailsVisible && !m_impl->annMemb)
  {
    QCString doxyName=name().copy();
    if (!cname.isEmpty()) 
    {
      doxyName.prepend(cdname+getLanguageSpecificSeparator(getLanguage()));
    }
    QCString doxyArgs=argsString();
    ol.startDoxyAnchor(cfname,cname,anchor(),doxyName,doxyArgs);

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
  if (m_impl->mtype==Typedef) ltype.prepend("typedef ");
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
      //printf(">>>>>> indDepth=%d ltype=`%s' varName=`%s'\n",indDepth,ltype.data(),varName.data());
      ol.docify("}");
      if (varName.isEmpty() && (name().isEmpty() || name().at(0)=='@')) 
      {
        ol.docify(";"); 
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
                    TRUE                     // autoBreak
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
                    TRUE                     // autoBreak
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
                TRUE                     // autoBreak
               );
  }
  bool htmlOn = ol.isEnabled(OutputGenerator::Html);
  if (htmlOn && /*Config_getBool("HTML_ALIGN_MEMBERS") &&*/ !ltype.isEmpty())
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
    //printf("Member name=`%s gd=%p md->groupDef=%p inGroup=%d isLinkable()=%d\n",name().data(),gd,getGroupDef(),inGroup,isLinkable());
    if (!(name().isEmpty() || name().at(0)=='@') && // name valid
        (hasDocumentation() || isReference()) && // has docs
        !(m_impl->prot==Private && !Config_getBool("EXTRACT_PRIVATE") && m_impl->mtype!=Friend) && // hidden due to protection
        !(isStatic() && m_impl->classDef==0 && !Config_getBool("EXTRACT_STATIC")) // hidden due to static-ness
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
    //static bool separateMemPages = Config_getBool("SEPARATE_MEMBER_PAGES");
    //QCString cfname = getOutputFileBase();
    //QCString cfiname = d->getOutputFileBase();
    //Doxygen::indexList.addIndexItem(
    //    cname,                                 // level1
    //    name(),                                // level2
    //    separateMemPages ? cfname : cfiname,   // contRef
    //    cfname,                                // memRef
    //    anchor(),                              // anchor
    //    this);                                 // memberdef
    Doxygen::indexList.addIndexItem(d,this);
  }

  // *** write arguments
  if (argsString() && !isObjCMethod()) 
  {
    if (!isDefine()) ol.writeString(" ");
    linkifyText(TextGeneratorOLImpl(ol), // out
                d,                       // scope
                getBodyDef(),            // fileScope
                this,                    // self
                argsString(),            // text
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
    linkifyText(TextGeneratorOLImpl(ol),d,getBodyDef(),this,m_impl->bitfields.simplifyWhiteSpace());
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

  if (isProperty() && (isSettable() || isGettable()))
  {
      ol.writeLatexSpacing();
      ol.startTypewriter();
      ol.docify(" [");
      QStrList sl;
      if (isGettable())  sl.append("get");
      if (isSettable())  sl.append("set");
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

  if (!detailsVisible && !m_impl->annMemb)
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
      Config_getBool("BRIEF_MEMBER_DESC") 
      /* && !annMemb */
     )
  {
    ol.startMemberDescription(anchor(),inheritId);
    ol.parseDoc(briefFile(),briefLine(),
                getOuterScope()?getOuterScope():d,this,briefDescription(),
                TRUE,FALSE,0,TRUE,FALSE);
    if (detailsVisible) 
    {
      ol.pushGeneratorState();
      ol.disableAllBut(OutputGenerator::Html);
      //ol.endEmphasis();
      ol.docify(" ");
      if (m_impl->group!=0 && gd==0) // forward link to the group
      {
        ol.startTextLink(getOutputFileBase(),anchor());
      }
      else // local link
      {
        ol.startTextLink(0,anchor());
      }
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

  ol.endMemberDeclaration(anchor(),inheritId);

  warnIfUndocumented();
}

bool MemberDef::isDetailedSectionLinkable() const          
{ 
  static bool extractAll        = Config_getBool("EXTRACT_ALL");
  static bool alwaysDetailedSec = Config_getBool("ALWAYS_DETAILED_SEC");
  static bool repeatBrief       = Config_getBool("REPEAT_BRIEF");
  static bool briefMemberDesc   = Config_getBool("BRIEF_MEMBER_DESC");
  static bool hideUndocMembers  = Config_getBool("HIDE_UNDOC_MEMBERS");
  static bool extractStatic     = Config_getBool("EXTRACT_STATIC");

  KEEP_RESIDENT_DURING_CALL;

  // the member has details documentation for any of the following reasons
  bool docFilter = 
         // treat everything as documented
         extractAll ||          
         // has detailed docs
         !documentation().isEmpty() ||             
         // has inbody docs
         !inbodyDocumentation().isEmpty() ||
         // is an enum with values that are documented
         (m_impl->mtype==Enumeration && m_impl->docEnumValues) ||  
         // is documented enum value
         (m_impl->mtype==EnumValue && !briefDescription().isEmpty()) || 
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
         // has user comments
         Doxygen::userComments
         ; 
         
  // this is not a global static or global statics should be extracted
  bool staticFilter = getClassDef()!=0 || !isStatic() || extractStatic; 
         
  // only include members that are non-private unless EXTRACT_PRIVATE is
  // set to YES or the member is part of a   group
  bool privateFilter = protectionLevelVisible(protection()) || m_impl->mtype==Friend;

  // member is part of an anonymous scope that is the type of
  // another member in the list.
  //
  //bool inAnonymousScope = !briefDescription().isEmpty() && annUsed;

  // hide friend (class|struct|union) member if HIDE_FRIEND_COMPOUNDS
  // is true
  bool friendCompoundFilter = !(Config_getBool("HIDE_FRIEND_COMPOUNDS") &&
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
  static bool separateMemPages = Config_getBool("SEPARATE_MEMBER_PAGES");
  static bool inlineSimpleStructs = Config_getBool("INLINE_SIMPLE_STRUCTS");
  static bool hideUndocMembers = Config_getBool("HIDE_UNDOC_MEMBERS");
  bool groupFilter = getGroupDef()==0 || inGroup || separateMemPages; 
  bool fileFilter  = getNamespaceDef()==0 || !inFile;
  bool simpleFilter = !hideUndocMembers && inlineSimpleStructs && 
                      getClassDef()!=0 && getClassDef()->isSimple();

  bool visible = isDetailedSectionLinkable() && groupFilter && fileFilter && 
                 !isReference();
  bool result = visible || simpleFilter;
  //printf("%s::isDetailedSectionVisble: %d groupFilter=%d fileFilter=%d\n",
  //    name().data(),result,groupFilter,fileFilter);
  return result;
}

void MemberDef::_getLabels(QStrList &sl,Definition *container) const
{
  static bool inlineInfo = Config_getBool("INLINE_INFO");

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
        if      (Config_getBool("INLINE_INFO") && isInline()) sl.append("inline");
        if      (isExplicit())            sl.append("explicit");
        if      (isMutable())             sl.append("mutable");
        if      (isStatic())              sl.append("static");
        if      (isGettable())            sl.append("get");
        if      (isSettable())            sl.append("set");
        if      (isAddable())             sl.append("add");
        if      (isRemovable())           sl.append("remove");
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
  if ((m_impl->hasCallGraph || Config_getBool("CALL_GRAPH")) 
      && (isFunction() || isSlot() || isSignal()) && Config_getBool("HAVE_DOT")
     )
  {
    DotCallGraph callGraph(this,FALSE);
    if (callGraph.isTooBig())
    {
       err("warning: Call graph for '%s' not generated, too many nodes. Consider increasing DOT_GRAPH_MAX_NODES.\n",qPrint(qualifiedName()));
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
  if ((m_impl->hasCallerGraph || Config_getBool("CALLER_GRAPH")) 
      && (isFunction() || isSlot() || isSignal()) && Config_getBool("HAVE_DOT")
     )
  {
    DotCallGraph callerGraph(this, TRUE);
    if (callerGraph.isTooBig())
    {
       err("warning: Caller graph for '%s' not generated, too many nodes. Consider increasing DOT_GRAPH_MAX_NODES.\n",qPrint(qualifiedName()));
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
          if (bcd->isLinkableInProject()/* && !Config_getBool("PDF_HYPERLINKS")*/ )
          {
            writePageRef(ol,bcd->getOutputFileBase(),bcd->anchor());
          }
        }
        ol.parseText(reimplFromLine.right(
              reimplFromLine.length()-markerPos-2)); // text right from marker

      }
      else
      {
        err("error: translation error: no marker in trReimplementsFromList()\n");
      }
      ol.endParagraph();
    }
  }
}

void MemberDef::_writeReimplementedBy(OutputList &ol)
{
  LockingPtr<MemberList> bml=reimplementedBy();
  if (bml!=0)
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
      i=text.find("@1");
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
    LockingPtr<MemberList> fmdl=enumFieldList();
    //printf("** %s: enum values=%d\n",name().data(),fmdl!=0 ? fmdl->count() : 0);
    if (fmdl!=0)
    {
      MemberDef *fmd=fmdl->first();
      while (fmd)
      {
        //printf("Enum %p: isLinkable()=%d\n",fmd,fmd->isLinkable());
        if (fmd->isLinkable())
        {
          if (first)
          {
            ol.startSimpleSect(BaseOutputDocInterface::EnumValues,0,0,theTranslator->trEnumerationValues()+": ");
            ol.startDescForItem();
            ol.startDescTable();
          }

          ol.addIndexItem(fmd->name(),ciname);
          ol.addIndexItem(ciname,fmd->name());

          //Doxygen::indexList.addIndexItem(
          //                       ciname,                                // level1
          //                       fmd->name(),                           // level2
          //                       separateMemPages ? cfname : cfiname,   // contRef
          //                       cfname,                                // memRef
          //                       fmd->anchor(),                         // anchor
          //                       fmd);                                  // memberdef
          Doxygen::indexList.addIndexItem(container,fmd);

          //ol.writeListItem();
          ol.startDescTableTitle(); // this enables emphasis!
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

          if (!fmd->briefDescription().isEmpty())
          { 
            ol.parseDoc(fmd->briefFile(),fmd->briefLine(),getOuterScope()?getOuterScope():container,fmd,fmd->briefDescription(),TRUE,FALSE);
          }
          // FIXME:PARA
          //if (!fmd->briefDescription().isEmpty() && 
          //    !fmd->documentation().isEmpty())
          //{
          //  ol.newParagraph();
          //}
          if (!fmd->documentation().isEmpty())
          { 
            ol.parseDoc(fmd->docFile(),fmd->docLine(),getOuterScope()?getOuterScope():container,fmd,fmd->documentation()+"\n",TRUE,FALSE);
          }
          ol.endDescTableData();
        }
        fmd=fmdl->next();
      }
    }
    if (!first) 
    { 
      //ol.endItemList(); 
      ol.endDescTable();
      ol.endDescForItem();
      ol.endSimpleSect();
      ol.writeChar('\n'); 
    }
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

  //printf("MemberDef::writeDocumentation(): name=`%s' hasDocs=`%d' containerType=%d inGroup=%d\n",
  //    name().data(),hasDocs,container->definitionType(),inGroup);

  if ( !hasDocs ) return;
  if (isEnumValue() && !showEnumValues) return;

  KEEP_RESIDENT_DURING_CALL;

  SrcLangExt lang = getLanguage();
  //printf("member=%s lang=%d\n",name().data(),lang);
  bool optVhdl = lang==SrcLangExt_VHDL;
  QCString sep = getLanguageSpecificSeparator(lang,TRUE);

  QCString scopeName = scName;
  QCString memAnchor = anchor();
  QCString ciname = container->name();
  if (container->definitionType()==TypeGroup)
  {
    if (getClassDef())          scopeName=getClassDef()->displayName();
    else if (getNamespaceDef()) scopeName=getNamespaceDef()->displayName();
    else if (getFileDef())      scopeName=getFileDef()->displayName();
    ciname = ((GroupDef *)container)->groupTitle();
  }
  else if (container->definitionType()==TypeFile && getNamespaceDef())
  { // member is in a namespace, but is written as part of the file documentation
    // as well, so we need to make sure its label is unique.
    memAnchor.prepend("file_");
  }

  QCString cname   = container->name();
  QCString cfname  = getOutputFileBase();
  QCString cfiname = container->getOutputFileBase();

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
  int i=0,l;
  static QRegExp r("@[0-9]+");

  //----------------------------------------

  ol.pushGeneratorState();

  bool htmlEndLabelTable=FALSE;
  QStrList sl;
  _getLabels(sl,container);

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
        linkifyText(TextGeneratorOLImpl(ol),container,getBodyDef(),this,ldef.left(i));
        vmd->writeEnumDeclaration(ol,getClassDef(),getNamespaceDef(),getFileDef(),getGroupDef());
        linkifyText(TextGeneratorOLImpl(ol),container,getBodyDef(),this,ldef.right(ldef.length()-i-l));

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
      linkifyText(TextGeneratorOLImpl(ol),container,getBodyDef(),this,ldef.right(ldef.length()-ei));
    }
  }
  else // not an enum value or anonymous compound
  {
    ol.startDoxyAnchor(cfname,cname,memAnchor,doxyName,doxyArgs);
    ol.startMemberDoc(ciname,name(),memAnchor,title,showInline);

    ClassDef *cd=getClassDef();
    if (!Config_getBool("HIDE_SCOPE_NAMES"))
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
      VhdlDocGen::writeVHDLTypeDocumentation(this,container,ol);
    }
    else
    {
      linkifyText(TextGeneratorOLImpl(ol),
                  container,
                  getBodyDef(),
                  this,
                  substitute(ldef,"::",sep)
                 );
      hasParameterList=writeDefArgumentList(ol,cd,scopeName,this);
    }

    if (hasOneLineInitializer()) // add initializer
    {
      if (!isDefine()) 
      {
        //ol.docify(" = "); 
        ol.docify(" "); 
        QCString init = m_impl->initializer.simplifyWhiteSpace();
        linkifyText(TextGeneratorOLImpl(ol),container,getBodyDef(),this,init);
      }
      else 
      {
        ol.writeNonBreakableSpace(3);
        linkifyText(TextGeneratorOLImpl(ol),container,getBodyDef(),this,m_impl->initializer);
      }
    }
    if (excpString()) // add exception list
    {
      ol.docify(" ");
      linkifyText(TextGeneratorOLImpl(ol),container,getBodyDef(),this,excpString());
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

  /* write multi-line initializer (if any) */
  if (hasMultiLineInitializer()
      //initLines>0 && ((initLines<maxInitLines && userInitLines==-1) // implicitly enabled
      //                || initLines<userInitLines // explicitly enabled
      //               )
     )
  {
    //printf("md=%s initLines=%d init=`%s'\n",name().data(),initLines,init.data());
    ol.startBold();
    if (m_impl->mtype==Define)
      ol.parseText(theTranslator->trDefineValue());
    else
      ol.parseText(theTranslator->trInitialValue());
    ol.endBold();
    ParserInterface *pIntf = Doxygen::parserManager->getParser(getDefFileExtension());
    pIntf->resetCodeParserState();
    ol.startCodeFragment();
    pIntf->parseCode(ol,scopeName,m_impl->initializer,FALSE,0,getFileDef(),
                     -1,-1,TRUE,this,FALSE,this);
    ol.endCodeFragment();
  }

  QCString brief           = briefDescription();
  QCString detailed        = documentation();
  LockingPtr<ArgumentList> docArgList = LockingPtr<ArgumentList>(this,m_impl->defArgList);
  if (m_impl->templateMaster)
  {
    brief      = m_impl->templateMaster->briefDescription();
    detailed   = m_impl->templateMaster->documentation();
    docArgList = m_impl->templateMaster->argumentList();
  }

  /* write brief description */
  if (!brief.isEmpty() && 
      (Config_getBool("REPEAT_BRIEF") || 
       !Config_getBool("BRIEF_MEMBER_DESC")
      ) 
     )  
  { 
    ol.startParagraph();
    ol.parseDoc(briefFile(),briefLine(),
                getOuterScope()?getOuterScope():container,this,
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
      ol.parseDoc(docFile(),docLine(),getOuterScope()?getOuterScope():container,this,detailed+"\n",TRUE,FALSE);
    }
   
    if (!inbodyDocumentation().isEmpty())
    {
      ol.parseDoc(inbodyFile(),inbodyLine(),
                  getOuterScope()?getOuterScope():container,this,
                  inbodyDocumentation()+"\n",TRUE,FALSE);
    }
  }
  else if (!brief.isEmpty() && (Config_getBool("REPEAT_BRIEF") ||
        !Config_getBool("BRIEF_MEMBER_DESC")))
  {
    if (!inbodyDocumentation().isEmpty())
    {
      ol.parseDoc(inbodyFile(),inbodyLine(),getOuterScope()?getOuterScope():container,this,inbodyDocumentation()+"\n",TRUE,FALSE);
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
    ol.parseDoc(
        docFile(),docLine(),
        getOuterScope()?getOuterScope():container,
        this,         // memberDef
        paramDocs,    // docStr
        TRUE,         // indexWords
        FALSE         // isExample
        );

  }

  _writeEnumValues(ol,container,cfname,ciname,cname);
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
  //if (Config_getBool("EXTRACT_ALL") && !hasDocs) ol.enable(OutputGenerator::Latex); 
  ol.popGeneratorState();

  //------------------------------------------------

  if (!Config_getBool("EXTRACT_ALL") &&
      Config_getBool("WARN_IF_UNDOCUMENTED") &&
      Config_getBool("WARN_NO_PARAMDOC") &&
      !Doxygen::suppressDocWarnings)
  {
    if (!hasDocumentedParams())
    {
      warn_doc_error(docFile(),docLine(),
          "warning: parameters of member %s are not (all) documented",
          qPrint(qualifiedName()));
    }
    if (!hasDocumentedReturnType() && isFunction() && hasDocumentation())
    {
      warn_doc_error(docFile(),docLine(),
          "warning: return type of member %s is not documented",
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

void MemberDef::writeMemberDocSimple(OutputList &ol, Definition *container)
{
  KEEP_RESIDENT_DURING_CALL;

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

  QCString type = m_impl->accessorType;
  if (type.isEmpty())
  {
    type = m_impl->type;
  }
  QCString ts = simplifyTypeForTable(type);

  if (cd) // cd points to an anonymous struct pointed to by this member
          // so we add a link to it from the type column.
  {
    int i=0;
    const char *prefixes[] = { "struct ","union ","class ", 0 };
    const char **p = prefixes;
    while (*p)
    {
      int l=strlen(*p);
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
  if (!m_impl->bitfields.isEmpty()) // add bitfields
  {
    linkifyText(TextGeneratorOLImpl(ol),getOuterScope(),getBodyDef(),this,m_impl->bitfields.simplifyWhiteSpace());
  }
  ol.endInlineMemberName();

  ol.startInlineMemberDoc();

  QCString brief           = briefDescription();
  QCString detailed        = documentation();

  /* write brief description */
  if (!brief.isEmpty() && 
      (Config_getBool("REPEAT_BRIEF") || 
       !Config_getBool("BRIEF_MEMBER_DESC")
      ) 
     )  
  { 
    ol.parseDoc(briefFile(),briefLine(),
                getOuterScope()?getOuterScope():container,this,
                brief,FALSE,FALSE,0,TRUE,FALSE);
  }

  /* write detailed description */
  if (!detailed.isEmpty())
  { 
    ol.parseDoc(docFile(),docLine(),
                getOuterScope()?getOuterScope():container,this,
                detailed+"\n",FALSE,FALSE,0,TRUE,FALSE);
   
  }

  ol.endInlineMemberDoc();
}

QCString MemberDef::memberTypeName() const
{
  makeResident();
  switch (m_impl->mtype)
  {
    case Define:      return "macro definition";
    case Function:    return "function";
    case Variable:    return "variable";
    case Typedef:     return "typedef";
    case Enumeration: return "enumeration"; 
    case EnumValue:   return "enumvalue";
    case Signal:      return "signal";
    case Slot:        return "slot";
    case Friend:      return "friend";
    case DCOP:        return "dcop";
    case Property:    return "property";
    case Event:       return "event";
    default:          return "unknown";
  }
}

void MemberDef::warnIfUndocumented()
{
  makeResident();
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
    t="namespace", d=nd; 
  else if (gd)
    t="group", d=gd;
  else
    t="file", d=fd;
  static bool extractAll = Config_getBool("EXTRACT_ALL");

  //printf("warnIfUndoc: d->isLinkable()=%d isLinkable()=%d "
  //       "isDocumentedFriendClass()=%d name()=%s prot=%d\n",
  //       d->isLinkable(),isLinkable(),isDocumentedFriendClass(),
  //       name().data(),prot);
  if ((!hasUserDocumentation() && !extractAll) &&
      !isFriendClass() && 
      name().find('@')==-1 && d && d->name().find('@')==-1 &&
      protectionLevelVisible(m_impl->prot) &&
      !isReference()
     )
  {
    warn_undoc(getDefFileName(),getDefLine(),"warning: Member %s%s (%s) of %s %s is not documented.",
         qPrint(name()),qPrint(argsString()),qPrint(memberTypeName()),t,qPrint(d->name()));
  }
}



bool MemberDef::isFriendClass() const
{
  makeResident();
  return (isFriend() && 
         (m_impl->type=="friend class" || m_impl->type=="friend struct" || 
          m_impl->type=="friend union"));
}

bool MemberDef::isDocumentedFriendClass() const
{
  makeResident();
  ClassDef *fcd=0;
  QCString baseName=name();
  int i=baseName.find('<');
  if (i!=-1) baseName=baseName.left(i);
  return (isFriendClass() &&
         (fcd=getClass(baseName)) && fcd->isLinkable()); 
}

bool MemberDef::hasDocumentation() const
{ 
  makeResident();
  return Definition::hasDocumentation() || 
         (m_impl->mtype==Enumeration && m_impl->docEnumValues) ||  // has enum values
         (m_impl->defArgList!=0 && m_impl->defArgList->hasDocumentation()); // has doc arguments
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
  makeResident();
  m_impl->memberGroup = grp;
}

bool MemberDef::visibleMemberGroup(bool hideNoHeader) 
{ 
  makeResident();
  return m_impl->memberGroup!=0 && 
          (!hideNoHeader || m_impl->memberGroup->header()!="[NOHEADER]"); 
}

QCString MemberDef::getScopeString() const
{
  makeResident();
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

void MemberDef::setAnchor(const char *a)
{
  makeResident();
  //anc=a;
  (void)a;
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
    snprintf(buf,20,"%d:",m_impl->tArgList->count());
    buf[19]='\0';
    memAnchor.prepend(buf);
  }
  
  // convert to md5 hash
  uchar md5_sig[16];
  QCString sigStr(33);
  MD5Buffer((const unsigned char *)memAnchor.data(),memAnchor.length(),md5_sig);
  //printf("memAnchor=%s\n",memAnchor.data());
  MD5SigToString(md5_sig,sigStr.data(),33);
  m_impl->anc = "a"+sigStr;
}

void MemberDef::setGroupDef(GroupDef *gd,Grouping::GroupPri_t pri,
                            const QCString &fileName,int startLine,
                            bool hasDocs,MemberDef *member)
{
  //printf("%s MemberDef::setGroupDef(%s)\n",name().data(),gd->name().data());
  makeResident();
  m_impl->group=gd;
  m_impl->grouppri=pri;
  m_impl->groupFileName=fileName;
  m_impl->groupStartLine=startLine;
  m_impl->groupHasDocs=hasDocs;
  m_impl->groupMember=member;
  m_isLinkableCached = 0; 
}

void MemberDef::setEnumScope(MemberDef *md) 
{ 
  makeResident();
  m_impl->enumScope=md; 
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
  makeResident();
  m_impl->classDef=cd; 
  m_isLinkableCached = 0; 
  m_isConstructorCached = 0; 
  setOuterScope(cd); 
}

void MemberDef::setNamespace(NamespaceDef *nd) 
{ 
  makeResident();
  m_impl->nspace=nd; 
  setOuterScope(nd); 
}

MemberDef *MemberDef::createTemplateInstanceMember(
        ArgumentList *formalArgs,ArgumentList *actualArgs)
{
  KEEP_RESIDENT_DURING_CALL;
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
                       getDefFileName(),getDefLine(),
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
  makeResident();
  //printf("%s: init=%s, initLines=%d maxInitLines=%d userInitLines=%d\n",
  //    name().data(),m_impl->initializer.data(),m_impl->initLines,
  //    m_impl->maxInitLines,m_impl->userInitLines);
  return !m_impl->initializer.isEmpty() && m_impl->initLines==0 && // one line initializer
         ((m_impl->maxInitLines>0 && m_impl->userInitLines==-1) || m_impl->userInitLines>0); // enabled by default or explicitly
}

bool MemberDef::hasMultiLineInitializer() const
{
  makeResident();
  //printf("initLines=%d userInitLines=%d maxInitLines=%d\n",
  //    initLines,userInitLines,maxInitLines);
  return m_impl->initLines>0 && 
         ((m_impl->initLines<m_impl->maxInitLines && m_impl->userInitLines==-1) // implicitly enabled
          || m_impl->initLines<m_impl->userInitLines // explicitly enabled
         );
}

void MemberDef::setInitializer(const char *initializer)    
{ 
  makeResident();
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
  KEEP_RESIDENT_DURING_CALL;
  static bool optimizeOutputForC = Config_getBool("OPTIMIZE_OUTPUT_FOR_C");
  //static bool hideScopeNames     = Config_getBool("HIDE_SCOPE_NAMES");
  //static bool optimizeOutputJava = Config_getBool("OPTIMIZE_OUTPUT_JAVA");
  //static bool fortranOpt = Config_getBool("OPTIMIZE_FOR_FORTRAN");  
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
  LockingPtr< QList<ListItemInfo> > xrefItems = xrefListItems();
  if (xrefItems!=0)
  {
    addRefItem(xrefItems.pointer(),
        qualifiedName()+argsString(), // argsString is needed for overloaded functions (see bug 609624)
        memLabel,
        getOutputFileBase()+"#"+anchor(),memName,memArgs);
  }
}

MemberList *MemberDef::getSectionList(Definition *d) const 
{ 
  makeResident();
  char key[20];
  sprintf(key,"%p",d);
  return (d!=0 && m_impl->classSectionSDict) ? m_impl->classSectionSDict->find(key) : 0;
}

void MemberDef::setSectionList(Definition *d, MemberList *sl)   
{ 
  makeResident();
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
  KEEP_RESIDENT_DURING_CALL;
  if (count>25) 
  {
     warn(getDefFileName(),getDefLine(),
       "warning: Internal inconsistency: recursion detected in overload relation for member %s!"
       ,qPrint(name())
      );
     return Normal;
  }
  makeResident();
  Specifier v = m_impl->virt;
  MemberDef *rmd = reimplements();
  while (rmd && v==Normal)
  {
    v = rmd->virtualness(count+1)==Normal ? Normal : Virtual;
    rmd = rmd->reimplements();
  }
  return v;
}

void MemberDef::_computeIsConstructor()
{
  KEEP_RESIDENT_DURING_CALL;
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
  KEEP_RESIDENT_DURING_CALL;
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
  KEEP_RESIDENT_DURING_CALL;

  int enumMemCount=0;

  MemberList *fmdl=m_impl->enumFields;
  uint numVisibleEnumValues=0;
  if (fmdl)
  {
    MemberDef *fmd=fmdl->first();
    while (fmd)
    {
      if (fmd->isBriefSectionVisible()) numVisibleEnumValues++;
      fmd=fmdl->next();
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
      if (!Config_getString("GENERATE_TAGFILE").isEmpty() && !isReference())
      {
        Doxygen::tagFile << "    <member kind=\"enumeration\">" << endl;
        Doxygen::tagFile << "      <name>" << convertToXML(name()) << "</name>" << endl; 
        Doxygen::tagFile << "      <anchorfile>" << convertToXML(getOutputFileBase()+Doxygen::htmlFileExtension) << "</anchorfile>" << endl;
        Doxygen::tagFile << "      <anchor>" << convertToXML(anchor()) << "</anchor>" << endl; 
        Doxygen::tagFile << "      <arglist>" << convertToXML(argsString()) << "</arglist>" << endl; 
        Doxygen::tagFile << "    </member>" << endl;
      }
      writeLink(typeDecl,cd,nd,fd,gd);
    }
    else
    {
      typeDecl.startBold();
      typeDecl.docify(n);
      typeDecl.endBold();
    }
    typeDecl.writeChar(' ');
    if (!m_impl->enumBaseType.isEmpty())
    {
      typeDecl.writeChar(':');
      typeDecl.writeChar(' ');
      typeDecl.docify(m_impl->enumBaseType);
      typeDecl.writeChar(' ');
    }
  }

  uint enumValuesPerLine = (uint)Config_getInt("ENUM_VALUES_PER_LINE");
  if (numVisibleEnumValues>0 && enumValuesPerLine>0)
  {
    typeDecl.docify("{ ");
    if (fmdl)
    {
      MemberDef *fmd=fmdl->first();
      bool fmdVisible = fmd->isBriefSectionVisible();
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
            if (!Config_getString("GENERATE_TAGFILE").isEmpty() && !fmd->isReference())
            {
              Doxygen::tagFile << "    <member kind=\"enumvalue\">" << endl;
              Doxygen::tagFile << "      <name>" << convertToXML(fmd->name()) << "</name>" << endl; 
              Doxygen::tagFile << "      <anchorfile>" << convertToXML(getOutputFileBase()+Doxygen::htmlFileExtension) << "</anchorfile>" << endl;              
              Doxygen::tagFile << "      <anchor>" << convertToXML(fmd->anchor()) << "</anchor>" << endl; 
              Doxygen::tagFile << "      <arglist>" << convertToXML(fmd->argsString()) << "</arglist>" << endl; 
              Doxygen::tagFile << "    </member>" << endl;
            }
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
        fmd=fmdl->next();
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
  makeResident();
  if (m_impl->defArgList) delete m_impl->defArgList;
  m_impl->defArgList = al;
}

void MemberDef::setDeclArgumentList(ArgumentList *al)
{
  makeResident();
  if (m_impl->declArgList) delete m_impl->declArgList;
  m_impl->declArgList = al;
}

void MemberDef::setTypeConstraints(ArgumentList *al)
{
  if (al==0) return;
  makeResident();
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
  makeResident();
  m_impl->type = t;
}

void MemberDef::setAccessorType(ClassDef *cd,const char *t)
{
  makeResident();
  m_impl->accessorClass = cd;
  m_impl->accessorType = t;
}

void MemberDef::findSectionsInDocumentation()
{
  makeResident();
  docFindSections(documentation(),this,0,docFile());  
}

void MemberDef::enableCallGraph(bool e) 
{ 
  makeResident();
  m_impl->hasCallGraph=e; 
  if (e) Doxygen::parseSourcesNeeded = TRUE;
}

void MemberDef::enableCallerGraph(bool e) 
{ 
  makeResident();
  m_impl->hasCallerGraph=e; 
  if (e) Doxygen::parseSourcesNeeded = TRUE;
}

#if 0
bool MemberDef::protectionVisible() const
{
  makeResident();
  return m_impl->prot==Public || 
         (m_impl->prot==Private   && Config_getBool("EXTRACT_PRIVATE"))   ||
         (m_impl->prot==Protected && Config_getBool("EXTRACT_PROTECTED")) ||
         (m_impl->prot==Package   && Config_getBool("EXTRACT_PACKAGE"));
}
#endif

#if 0
void MemberDef::setInbodyDocumentation(const char *docs,
                  const char *docFile,int docLine)
{
  makeResident();
  m_impl->inbodyDocs = docs;
  m_impl->inbodyDocs = m_impl->inbodyDocs.stripWhiteSpace();
  m_impl->inbodyLine = docLine;
  m_impl->inbodyFile = docFile;
}
#endif

bool MemberDef::isObjCMethod() const
{
  makeResident();
  if (m_impl->classDef && m_impl->classDef->isObjectiveC() && isFunction()) return TRUE;
  return FALSE; 
}

bool MemberDef::isObjCProperty() const
{
  makeResident();
  if (m_impl->classDef && m_impl->classDef->isObjectiveC() && isProperty()) return TRUE;
  return FALSE; 
}

QCString MemberDef::qualifiedName() const
{
  makeResident();
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
  else
  {
    return Definition::qualifiedName();
  }  
}

void MemberDef::setTagInfo(TagInfo *ti)
{
  if (ti)
  {
    makeResident();
    //printf("%s: Setting tag name=%s anchor=%s\n",name().data(),ti->tagName.data(),ti->anchor.data());
    m_impl->anc=ti->anchor;
    setReference(ti->tagName);
    m_impl->explicitOutputFileBase = stripExtension(ti->fileName);
  }
}

QCString MemberDef::objCMethodName(bool localLink,bool showStatic) const
{
  makeResident();
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
  makeResident();
  return m_impl->decl; 
}

const char *MemberDef::definition() const
{ 
  makeResident();
  return m_impl->def;
}

const char *MemberDef::extraTypeChars() const
{
  makeResident();
  return m_impl->extraTypeChars;
}

const char *MemberDef::typeString() const
{ 
  makeResident();
  return m_impl->type; 
}

const char *MemberDef::argsString() const
{ 
  makeResident();
  return m_impl->args; 
}

const char *MemberDef::excpString() const
{ 
  makeResident();
  return m_impl->exception; 
}

const char *MemberDef::bitfieldString() const
{ 
  makeResident();
  return m_impl->bitfields; 
}

const QCString &MemberDef::initializer() const
{ 
  makeResident();
  return m_impl->initializer; 
}

int MemberDef::initializerLines() const
{ 
  makeResident();
  return m_impl->initLines; 
}

int  MemberDef::getMemberSpecifiers() const
{ 
  makeResident();
  return m_impl->memSpec; 
}

ClassDef *MemberDef::getClassDef() const
{ 
  makeResident();
  return m_impl->classDef; 
}

FileDef  *MemberDef::getFileDef() const
{ 
  makeResident();
  return m_impl->fileDef; 
}

NamespaceDef* MemberDef::getNamespaceDef() const
{ 
  makeResident();
  return m_impl->nspace; 
}

const char *MemberDef::getReadAccessor() const
{ 
  makeResident();
  return m_impl->read; 
}

const char *MemberDef::getWriteAccessor() const
{ 
  makeResident();
  return m_impl->write; 
}

GroupDef *MemberDef::getGroupDef() const
{ 
  makeResident();
  return m_impl->group; 
}

Grouping::GroupPri_t MemberDef::getGroupPri() const
{ 
  makeResident();
  return m_impl->grouppri; 
}

const char *MemberDef::getGroupFileName() const
{ 
  makeResident();
  return m_impl->groupFileName; 
}

int MemberDef::getGroupStartLine() const
{ 
  makeResident();
  return m_impl->groupStartLine; 
}

bool MemberDef::getGroupHasDocs() const
{ 
  makeResident();
  return m_impl->groupHasDocs; 
}

Protection MemberDef::protection() const
{ 
  makeResident();
  return m_impl->prot; 
}

MemberDef::MemberType MemberDef::memberType() const
{ 
  makeResident();
  return m_impl->mtype; 
}

bool MemberDef::isSignal() const
{ 
  makeResident();
  return m_impl->mtype==Signal;      
}

bool MemberDef::isSlot() const
{ 
  makeResident();
  return m_impl->mtype==Slot;        
}

bool MemberDef::isVariable() const
{ 
  makeResident();
  return m_impl->mtype==Variable;    
}

bool MemberDef::isEnumerate() const
{ 
  makeResident();
  return m_impl->mtype==Enumeration; 
}

bool MemberDef::isEnumValue() const
{ 
  makeResident();
  return m_impl->mtype==EnumValue;   
}

bool MemberDef::isTypedef() const
{ 
  makeResident();
  return m_impl->mtype==Typedef;     
}

bool MemberDef::isFunction() const
{ 
  makeResident();
  return m_impl->mtype==Function;    
}

bool MemberDef::isFunctionPtr() const
{
  makeResident();
  return m_impl->mtype==Variable && QCString(argsString()).find(")(")!=-1;
}

bool MemberDef::isDefine() const
{ 
  makeResident();
  return m_impl->mtype==Define;      
}

bool MemberDef::isFriend() const
{ 
  makeResident();
  return m_impl->mtype==Friend;      
}

bool MemberDef::isDCOP() const
{ 
  makeResident();
  return m_impl->mtype==DCOP;        
}

bool MemberDef::isProperty() const
{ 
  makeResident();
  return m_impl->mtype==Property;    
}

bool MemberDef::isEvent() const
{ 
  makeResident();
  return m_impl->mtype==Event;       
}

bool MemberDef::isRelated() const
{ 
  makeResident();
  return m_impl->related == Related;
}

bool MemberDef::isForeign() const
{ 
  makeResident();
  return m_impl->related == Foreign; 
}

bool MemberDef::isStatic() const
{ 
  makeResident();
  return m_impl->stat; 
}

bool MemberDef::isInline() const
{ 
  makeResident();
  return (m_impl->memSpec&Entry::Inline)!=0; 
}

bool MemberDef::isExplicit() const
{ 
  makeResident();
  return (m_impl->memSpec&Entry::Explicit)!=0; 
}

bool MemberDef::isMutable() const
{ 
  makeResident();
  return (m_impl->memSpec&Entry::Mutable)!=0; 
}

bool MemberDef::isGettable() const
{ 
  makeResident();
  return (m_impl->memSpec&Entry::Gettable)!=0; 
}

bool MemberDef::isSettable() const
{ 
  makeResident();
  return (m_impl->memSpec&Entry::Settable)!=0; 
}

bool MemberDef::isAddable() const
{ 
  makeResident();
  return (m_impl->memSpec&Entry::Addable)!=0; 
}

bool MemberDef::isRemovable() const
{ 
  makeResident();
  return (m_impl->memSpec&Entry::Removable)!=0; 
}

bool MemberDef::isRaisable() const
{ 
  makeResident();
  return (m_impl->memSpec&Entry::Raisable)!=0; 
}

bool MemberDef::isReadable() const
{ 
  makeResident();
  return (m_impl->memSpec&Entry::Readable)!=0; 
}

bool MemberDef::isWritable() const
{ 
  makeResident();
  return (m_impl->memSpec&Entry::Writable)!=0; 
}

bool MemberDef::isFinal() const
{ 
  makeResident();
  return (m_impl->memSpec&Entry::Final)!=0; 
}

bool MemberDef::isNew() const
{ 
  makeResident();
  return (m_impl->memSpec&Entry::New)!=0; 
}

bool MemberDef::isSealed() const
{ 
  makeResident();
  return (m_impl->memSpec&Entry::Sealed)!=0; 
}

bool MemberDef::isOverride() const
{ 
  makeResident();
  return (m_impl->memSpec&Entry::Override)!=0; 
}

bool MemberDef::isInitonly() const
{ 
  makeResident();
  return (m_impl->memSpec&Entry::Initonly)!=0; 
}

bool MemberDef::isAbstract() const
{ 
  makeResident();
  return (m_impl->memSpec&Entry::Abstract)!=0; 
}

bool MemberDef::isOptional() const
{ 
  makeResident();
  return (m_impl->memSpec&Entry::Optional)!=0; 
}

bool MemberDef::isRequired() const
{ 
  makeResident();
  return (m_impl->memSpec&Entry::Required)!=0; 
}

bool MemberDef::isNonAtomic() const
{ 
  makeResident();
  return (m_impl->memSpec&Entry::NonAtomic)!=0; 
}

bool MemberDef::isCopy() const
{ 
  makeResident();
  return (m_impl->memSpec&Entry::Copy)!=0; 
}

bool MemberDef::isAssign() const
{ 
  makeResident();
  return (m_impl->memSpec&Entry::Assign)!=0; 
}

bool MemberDef::isRetain() const
{ 
  makeResident();
  return (m_impl->memSpec&Entry::Retain)!=0; 
}

bool MemberDef::isWeak() const
{
  makeResident();
  return (m_impl->memSpec&Entry::Weak)!=0; 
}

bool MemberDef::isStrong() const
{
  makeResident();
  return (m_impl->memSpec&Entry::Strong)!=0; 
}

bool MemberDef::isUnretained() const
{
  makeResident();
  return (m_impl->memSpec&Entry::Unretained)!=0; 
}

bool MemberDef::isAlias() const
{
  makeResident();
  return (m_impl->memSpec&Entry::Alias)!=0;
}

bool MemberDef::isDefault() const
{
  makeResident();
  return (m_impl->memSpec&Entry::Default)!=0;
}

bool MemberDef::isDelete() const
{
  makeResident();
  return (m_impl->memSpec&Entry::Delete)!=0;
}

bool MemberDef::isNoExcept() const
{
  makeResident();
  return (m_impl->memSpec&Entry::NoExcept)!=0;
}



bool MemberDef::isImplementation() const
{ 
  makeResident();
  return m_impl->implOnly; 
}

bool MemberDef::isExternal() const
{ 
  makeResident();
  return m_impl->explExt; 
}

bool MemberDef::isTemplateSpecialization() const
{ 
  makeResident();
  return m_impl->tspec; 
}

bool MemberDef::hasDocumentedParams() const
{ 
  makeResident();
  return m_impl->hasDocumentedParams; 
}

bool MemberDef::hasDocumentedReturnType() const
{ 
  makeResident();
  return m_impl->hasDocumentedReturnType; 
}

ClassDef *MemberDef::relatedAlso() const
{ 
  makeResident();
  return m_impl->relatedAlso; 
}

bool MemberDef::hasDocumentedEnumValues() const
{ 
  makeResident();
  return m_impl->docEnumValues; 
}

MemberDef *MemberDef::getAnonymousEnumType() const
{ 
  makeResident();
  return m_impl->annEnumType; 
}

bool MemberDef::isDocsForDefinition() const
{ 
  makeResident();
  return m_impl->docsForDefinition; 
}

MemberDef *MemberDef::getEnumScope() const
{ 
  makeResident();
  return m_impl->enumScope; 
}

LockingPtr<MemberList> MemberDef::enumFieldList() const
{ 
  makeResident();
  return LockingPtr<MemberList>(this,m_impl->enumFields); 
}

LockingPtr<ExampleSDict> MemberDef::getExamples() const
{ 
  makeResident();
  return LockingPtr<ExampleSDict>(this,m_impl->exampleSDict); 
}

bool MemberDef::isPrototype() const
{ 
  makeResident();
  return m_impl->proto; 
}

LockingPtr<ArgumentList> MemberDef::argumentList() const
{ 
  makeResident();
  return LockingPtr<ArgumentList>(this,m_impl->defArgList); 
}

LockingPtr<ArgumentList> MemberDef::declArgumentList() const
{ 
  makeResident();
  return LockingPtr<ArgumentList>(this,m_impl->declArgList); 
}

LockingPtr<ArgumentList> MemberDef::templateArguments() const
{ 
  makeResident();
  return LockingPtr<ArgumentList>(this,m_impl->tArgList); 
}

LockingPtr< QList<ArgumentList> > MemberDef::definitionTemplateParameterLists() const
{ 
  makeResident();
  return LockingPtr< QList<ArgumentList> >(this,m_impl->defTmpArgLists); 
}

int MemberDef::getMemberGroupId() const
{ 
  makeResident();
  return m_impl->grpId; 
}

MemberGroup *MemberDef::getMemberGroup() const
{ 
  makeResident();
  return m_impl->memberGroup; 
}

bool MemberDef::fromAnonymousScope() const
{ 
  makeResident();
  return m_impl->annScope; 
}

bool MemberDef::anonymousDeclShown() const
{ 
  makeResident();
  return m_impl->annUsed; 
}

void MemberDef::setAnonymousUsed() 
{
  makeResident();
  m_impl->annUsed = TRUE;
}

bool MemberDef::hasCallGraph() const
{ 
  makeResident();
  return m_impl->hasCallGraph; 
}

bool MemberDef::hasCallerGraph() const
{ 
  makeResident();
  return m_impl->hasCallerGraph; 
}

MemberDef *MemberDef::templateMaster() const
{ 
  makeResident();
  return m_impl->templateMaster; 
}

bool MemberDef::isTypedefValCached() const
{ 
  makeResident();
  return m_impl->isTypedefValCached; 
}

ClassDef *MemberDef::getCachedTypedefVal() const
{ 
  makeResident();
  return m_impl->cachedTypedefValue; 
}

QCString MemberDef::getCachedTypedefTemplSpec() const
{ 
  makeResident();
  return m_impl->cachedTypedefTemplSpec; 
}

QCString MemberDef::getCachedResolvedTypedef() const
{ 
  makeResident();
  //printf("MemberDef::getCachedResolvedTypedef()=%s m_impl=%p\n",m_impl->cachedResolvedType.data(),m_impl);
  return m_impl->cachedResolvedType; 
}

MemberDef *MemberDef::memberDefinition() const
{ 
  makeResident();
  return m_impl->memDef; 
}

MemberDef *MemberDef::memberDeclaration() const
{ 
  makeResident();
  return m_impl->memDec; 
}

MemberDef *MemberDef::inheritsDocsFrom() const
{ 
  makeResident();
  return m_impl->docProvider; 
}

MemberDef *MemberDef::getGroupAlias() const
{ 
  makeResident();
  return m_impl->groupAlias; 
}

void MemberDef::setMemberType(MemberType t)
{ 
  makeResident();
  m_impl->mtype=t; 
  m_isLinkableCached = 0;
}

void MemberDef::setDefinition(const char *d)
{ 
  makeResident();
  m_impl->def=d; 
}

void MemberDef::setFileDef(FileDef *fd)
{ 
  makeResident();
  m_impl->fileDef=fd; 
  m_isLinkableCached = 0;
  m_isConstructorCached = 0;
  m_isDestructorCached = 0;
}

void MemberDef::setProtection(Protection p)
{ 
  makeResident();
  m_impl->prot=p; 
  m_isLinkableCached = 0;
}

void MemberDef::setMemberSpecifiers(int s)
{ 
  makeResident();
  m_impl->memSpec=s; 
}

void MemberDef::mergeMemberSpecifiers(int s)
{ 
  makeResident();
  m_impl->memSpec|=s; 
}

void MemberDef::setBitfields(const char *s)
{ 
  makeResident();
  m_impl->bitfields = s; 
}

void MemberDef::setMaxInitLines(int lines)
{ 
  if (lines!=-1)
  {
    makeResident();
    m_impl->userInitLines=lines; 
  }
}

void MemberDef::setExplicitExternal(bool b)
{ 
  makeResident();
  m_impl->explExt=b; 
}

void MemberDef::setReadAccessor(const char *r)
{ 
  makeResident();
  m_impl->read=r; 
}

void MemberDef::setWriteAccessor(const char *w)
{ 
  makeResident();
  m_impl->write=w; 
}

void MemberDef::setTemplateSpecialization(bool b)
{ 
  makeResident();
  m_impl->tspec=b; 
}

void MemberDef::makeRelated()
{ 
  makeResident();
  m_impl->related = Related; 
  m_isLinkableCached = 0;
}

void MemberDef::makeForeign()
{ 
  makeResident();
  m_impl->related = Foreign; 
  m_isLinkableCached = 0;
}

void MemberDef::setHasDocumentedParams(bool b)
{ 
  makeResident();
  m_impl->hasDocumentedParams = b; 
}

void MemberDef::setHasDocumentedReturnType(bool b)
{ 
  makeResident();
  m_impl->hasDocumentedReturnType = b; 
}

void MemberDef::setInheritsDocsFrom(MemberDef *md)
{ 
  makeResident();
  m_impl->docProvider = md; 
}

void MemberDef::setArgsString(const char *as)
{ 
  makeResident();
  m_impl->args = as; 
}

void MemberDef::setRelatedAlso(ClassDef *cd)
{ 
  makeResident();
  m_impl->relatedAlso=cd; 
}

void MemberDef::setEnumClassScope(ClassDef *cd)
{ 
  makeResident();
  m_impl->classDef = cd; 
  m_isLinkableCached = 0; 
  m_isConstructorCached = 0; 
}

void MemberDef::setDocumentedEnumValues(bool value)
{ 
  makeResident();
  m_impl->docEnumValues=value; 
}

void MemberDef::setAnonymousEnumType(MemberDef *md)
{ 
  makeResident();
  m_impl->annEnumType = md; 
}

void MemberDef::setPrototype(bool p)
{ 
  makeResident();
  m_impl->proto=p; 
}

void MemberDef::setMemberGroupId(int id)
{ 
  makeResident();
  m_impl->grpId=id; 
}

void MemberDef::makeImplementationDetail()
{ 
  makeResident();
  m_impl->implOnly=TRUE; 
}

void MemberDef::setFromAnonymousScope(bool b)
{ 
  makeResident();
  m_impl->annScope=b; 
}

void MemberDef::setFromAnonymousMember(MemberDef *m)
{ 
  makeResident();
  m_impl->annMemb=m; 
}

void MemberDef::setTemplateMaster(MemberDef *mt)
{ 
  makeResident();
  m_impl->templateMaster=mt; 
  m_isLinkableCached = 0; 
}

void MemberDef::setDocsForDefinition(bool b)
{ 
  makeResident();
  m_impl->docsForDefinition = b; 
}

void MemberDef::setGroupAlias(MemberDef *md)
{ 
  makeResident();
  m_impl->groupAlias = md; 
}

void MemberDef::invalidateTypedefValCache()
{ 
  makeResident();
  m_impl->isTypedefValCached=FALSE; 
}

void MemberDef::setMemberDefinition(MemberDef *md)
{ 
  makeResident();
  m_impl->memDef=md; 
}

void MemberDef::setMemberDeclaration(MemberDef *md)
{ 
  makeResident();
  m_impl->memDec=md; 
}

ClassDef *MemberDef::category() const
{
  makeResident();
  return m_impl->category;
}

void MemberDef::setCategory(ClassDef *def)
{
  makeResident();
  m_impl->category = def;
}

MemberDef *MemberDef::categoryRelation() const
{
  makeResident();
  return m_impl->categoryRelation;
}

void MemberDef::setCategoryRelation(MemberDef *md)
{
  makeResident();
  m_impl->categoryRelation = md;
}

void MemberDef::setEnumBaseType(const QCString &type)
{
  makeResident();
  m_impl->enumBaseType = type;
}

QCString MemberDef::enumBaseType() const
{
  makeResident();
  return m_impl->enumBaseType;
}


void MemberDef::cacheTypedefVal(ClassDef*val, const QCString & templSpec, const QCString &resolvedType)
{
  makeResident();
  m_impl->isTypedefValCached=TRUE; 
  m_impl->cachedTypedefValue=val; 
  m_impl->cachedTypedefTemplSpec=templSpec; 
  m_impl->cachedResolvedType=resolvedType;
  //printf("MemberDef::cacheTypedefVal=%s m_impl=%p\n",m_impl->cachedResolvedType.data(),m_impl);
}

void MemberDef::copyArgumentNames(MemberDef *bmd)
{
  makeResident();
  {
    LockingPtr<ArgumentList> arguments = bmd->argumentList();
    if (m_impl->defArgList && arguments!=0)
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
    LockingPtr<ArgumentList> arguments = bmd->declArgumentList();
    if (m_impl->declArgList && arguments!=0)
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
  makeResident();
  invalidateCachedTypesInArgumentList(m_impl->defArgList);
  invalidateCachedTypesInArgumentList(m_impl->declArgList);
}


//-----------------------------------------------------------------

void MemberDef::flushToDisk() const
{
  if (isLocked()) return;
  MemberDef *that = (MemberDef*)this;
  that->m_storagePos = Doxygen::symbolStorage->alloc();
  //printf("%p: MemberDef::flushToDisk() m_impl=%p\n",this,m_impl);
  // write the definition base class member variables to disk
  Definition::flushToDisk();

  //printf("%p:   flushing specific part\n",this);

  // write the memberdef member variables to disk
  marshalUInt(Doxygen::symbolStorage,START_MARKER);
  marshalObjPointer   (Doxygen::symbolStorage,m_impl->classDef);
  marshalObjPointer   (Doxygen::symbolStorage,m_impl->fileDef);
  marshalObjPointer   (Doxygen::symbolStorage,m_impl->nspace);
  marshalObjPointer   (Doxygen::symbolStorage,m_impl->enumScope);
  marshalObjPointer   (Doxygen::symbolStorage,m_impl->annEnumType);
  marshalMemberList   (Doxygen::symbolStorage,m_impl->enumFields);
  marshalObjPointer   (Doxygen::symbolStorage,m_impl->redefines);
  marshalMemberList   (Doxygen::symbolStorage,m_impl->redefinedBy);
  marshalObjPointer   (Doxygen::symbolStorage,m_impl->memDef);
  marshalObjPointer   (Doxygen::symbolStorage,m_impl->memDec);
  marshalObjPointer   (Doxygen::symbolStorage,m_impl->relatedAlso);
  marshalExampleSDict (Doxygen::symbolStorage,m_impl->exampleSDict);
  marshalQCString     (Doxygen::symbolStorage,m_impl->type);
  marshalQCString     (Doxygen::symbolStorage,m_impl->enumBaseType);
  marshalQCString     (Doxygen::symbolStorage,m_impl->accessorType);
  marshalObjPointer   (Doxygen::symbolStorage,m_impl->accessorClass);
  marshalQCString     (Doxygen::symbolStorage,m_impl->args);
  marshalQCString     (Doxygen::symbolStorage,m_impl->def);
  marshalQCString     (Doxygen::symbolStorage,m_impl->anc);
  marshalInt          (Doxygen::symbolStorage,(int)m_impl->virt);
  marshalInt          (Doxygen::symbolStorage,(int)m_impl->prot);
  marshalQCString     (Doxygen::symbolStorage,m_impl->decl);
  marshalQCString     (Doxygen::symbolStorage,m_impl->bitfields);
  marshalQCString     (Doxygen::symbolStorage,m_impl->read);
  marshalQCString     (Doxygen::symbolStorage,m_impl->write);
  marshalQCString     (Doxygen::symbolStorage,m_impl->exception);
  marshalQCString     (Doxygen::symbolStorage,m_impl->initializer);
  marshalQCString     (Doxygen::symbolStorage,m_impl->extraTypeChars);
  marshalInt          (Doxygen::symbolStorage,m_impl->initLines);
  marshalInt          (Doxygen::symbolStorage,m_impl->memSpec);
  marshalInt          (Doxygen::symbolStorage,(int)m_impl->mtype);
  marshalInt          (Doxygen::symbolStorage,m_impl->maxInitLines);
  marshalInt          (Doxygen::symbolStorage,m_impl->userInitLines);
  marshalObjPointer   (Doxygen::symbolStorage,m_impl->annMemb);
  marshalArgumentList (Doxygen::symbolStorage,m_impl->defArgList);
  marshalArgumentList (Doxygen::symbolStorage,m_impl->declArgList);
  marshalArgumentList (Doxygen::symbolStorage,m_impl->tArgList);
  marshalArgumentList (Doxygen::symbolStorage,m_impl->typeConstraints);
  marshalObjPointer   (Doxygen::symbolStorage,m_impl->templateMaster);
  marshalArgumentLists(Doxygen::symbolStorage,m_impl->defTmpArgLists);
  marshalObjPointer   (Doxygen::symbolStorage,m_impl->cachedAnonymousType);
  marshalMemberLists  (Doxygen::symbolStorage,m_impl->classSectionSDict);
  marshalObjPointer   (Doxygen::symbolStorage,m_impl->groupAlias);
  marshalInt          (Doxygen::symbolStorage,m_impl->grpId);
  marshalObjPointer   (Doxygen::symbolStorage,m_impl->memberGroup);
  marshalObjPointer   (Doxygen::symbolStorage,m_impl->group);
  marshalInt          (Doxygen::symbolStorage,(int)m_impl->grouppri);
  marshalQCString     (Doxygen::symbolStorage,m_impl->groupFileName);
  marshalInt          (Doxygen::symbolStorage,m_impl->groupStartLine);
  marshalObjPointer   (Doxygen::symbolStorage,m_impl->groupMember);
  marshalBool         (Doxygen::symbolStorage,m_impl->isTypedefValCached);
  marshalObjPointer   (Doxygen::symbolStorage,m_impl->cachedTypedefValue);
  marshalQCString     (Doxygen::symbolStorage,m_impl->cachedTypedefTemplSpec);
  marshalQCString     (Doxygen::symbolStorage,m_impl->cachedResolvedType);
  marshalObjPointer   (Doxygen::symbolStorage,m_impl->docProvider);
  marshalQCString     (Doxygen::symbolStorage,m_impl->explicitOutputFileBase);
  marshalBool         (Doxygen::symbolStorage,m_impl->implOnly); 
  marshalBool         (Doxygen::symbolStorage,m_impl->hasDocumentedParams);
  marshalBool         (Doxygen::symbolStorage,m_impl->hasDocumentedReturnType);
  marshalBool         (Doxygen::symbolStorage,m_impl->isDMember);
  marshalInt          (Doxygen::symbolStorage,(int)m_impl->related);
  marshalBool         (Doxygen::symbolStorage,m_impl->stat);
  marshalBool         (Doxygen::symbolStorage,m_impl->proto);
  marshalBool         (Doxygen::symbolStorage,m_impl->docEnumValues);
  marshalBool         (Doxygen::symbolStorage,m_impl->annScope);
  marshalBool         (Doxygen::symbolStorage,m_impl->annUsed);
  marshalBool         (Doxygen::symbolStorage,m_impl->hasCallGraph);
  marshalBool         (Doxygen::symbolStorage,m_impl->hasCallerGraph);
  marshalBool         (Doxygen::symbolStorage,m_impl->explExt);
  marshalBool         (Doxygen::symbolStorage,m_impl->tspec);
  marshalBool         (Doxygen::symbolStorage,m_impl->groupHasDocs);
  marshalBool         (Doxygen::symbolStorage,m_impl->docsForDefinition);
  marshalObjPointer   (Doxygen::symbolStorage,m_impl->category);
  marshalObjPointer   (Doxygen::symbolStorage,m_impl->categoryRelation);
  marshalUInt(Doxygen::symbolStorage,END_MARKER);

  // function doesn't modify the object conceptually but compiler doesn't know this.
  delete that->m_impl;
  that->m_impl=0;
  that->m_flushPending=FALSE;
}

void MemberDef::loadFromDisk() const
{
  MemberDef *that = (MemberDef *)this;
  if (isLocked()) 
  {
    //printf("%p: loadFromDisk() locked: so still in memory\n",this);
    assert(m_impl!=0);
    return;
  }
  assert(m_impl==0);

  Doxygen::symbolStorage->seek(m_storagePos);
  Definition::loadFromDisk();

  //printf("%p:   loading specific part\n",this);

  that->m_impl = new MemberDefImpl;
  //printf("%p: MemberDef::loadFromDisk(): m_impl=%p\n",this,m_impl);

  uint marker = unmarshalUInt(Doxygen::symbolStorage);
  assert(marker==START_MARKER);
  m_impl->classDef                = (ClassDef*)unmarshalObjPointer     (Doxygen::symbolStorage);
  m_impl->fileDef                 = (FileDef*)unmarshalObjPointer      (Doxygen::symbolStorage);
  m_impl->nspace                  = (NamespaceDef*)unmarshalObjPointer (Doxygen::symbolStorage);
  m_impl->enumScope               = (MemberDef*)unmarshalObjPointer    (Doxygen::symbolStorage);
  m_impl->annEnumType             = (MemberDef*)unmarshalObjPointer    (Doxygen::symbolStorage);
  m_impl->enumFields              = unmarshalMemberList                (Doxygen::symbolStorage);
  m_impl->redefines               = (MemberDef*)unmarshalObjPointer    (Doxygen::symbolStorage);
  m_impl->redefinedBy             = unmarshalMemberList                (Doxygen::symbolStorage);
  m_impl->memDef                  = (MemberDef*)unmarshalObjPointer    (Doxygen::symbolStorage);
  m_impl->memDec                  = (MemberDef*)unmarshalObjPointer    (Doxygen::symbolStorage);
  m_impl->relatedAlso             = (ClassDef*)unmarshalObjPointer     (Doxygen::symbolStorage);
  m_impl->exampleSDict            = unmarshalExampleSDict (Doxygen::symbolStorage);
  m_impl->type                    = unmarshalQCString     (Doxygen::symbolStorage);
  m_impl->enumBaseType            = unmarshalQCString     (Doxygen::symbolStorage);
  m_impl->accessorType            = unmarshalQCString     (Doxygen::symbolStorage);
  m_impl->accessorClass           = (ClassDef*)unmarshalObjPointer     (Doxygen::symbolStorage);
  m_impl->args                    = unmarshalQCString     (Doxygen::symbolStorage);
  m_impl->def                     = unmarshalQCString     (Doxygen::symbolStorage);
  m_impl->anc                     = unmarshalQCString     (Doxygen::symbolStorage);
  m_impl->virt                    = (Specifier)unmarshalInt (Doxygen::symbolStorage);
  m_impl->prot                    = (Protection)unmarshalInt(Doxygen::symbolStorage);
  m_impl->decl                    = unmarshalQCString     (Doxygen::symbolStorage);
  m_impl->bitfields               = unmarshalQCString     (Doxygen::symbolStorage);
  m_impl->read                    = unmarshalQCString     (Doxygen::symbolStorage);
  m_impl->write                   = unmarshalQCString     (Doxygen::symbolStorage);
  m_impl->exception               = unmarshalQCString     (Doxygen::symbolStorage);
  m_impl->initializer             = unmarshalQCString     (Doxygen::symbolStorage);
  m_impl->extraTypeChars          = unmarshalQCString     (Doxygen::symbolStorage);
  m_impl->initLines               = unmarshalInt          (Doxygen::symbolStorage);
  m_impl->memSpec                 = unmarshalInt          (Doxygen::symbolStorage);
  m_impl->mtype                   = (MemberDef::MemberType)unmarshalInt          (Doxygen::symbolStorage);
  m_impl->maxInitLines            = unmarshalInt          (Doxygen::symbolStorage);
  m_impl->userInitLines           = unmarshalInt          (Doxygen::symbolStorage);
  m_impl->annMemb                 = (MemberDef*)unmarshalObjPointer   (Doxygen::symbolStorage);
  m_impl->defArgList              = unmarshalArgumentList (Doxygen::symbolStorage);
  m_impl->declArgList             = unmarshalArgumentList (Doxygen::symbolStorage);
  m_impl->tArgList                = unmarshalArgumentList (Doxygen::symbolStorage);
  m_impl->typeConstraints         = unmarshalArgumentList (Doxygen::symbolStorage);
  m_impl->templateMaster          = (MemberDef*)unmarshalObjPointer   (Doxygen::symbolStorage);
  m_impl->defTmpArgLists          = unmarshalArgumentLists(Doxygen::symbolStorage);
  m_impl->cachedAnonymousType     = (ClassDef*)unmarshalObjPointer   (Doxygen::symbolStorage);
  m_impl->classSectionSDict       = unmarshalMemberLists  (Doxygen::symbolStorage);
  m_impl->groupAlias              = (MemberDef*)unmarshalObjPointer   (Doxygen::symbolStorage);
  m_impl->grpId                   = unmarshalInt          (Doxygen::symbolStorage);
  m_impl->memberGroup             = (MemberGroup*)unmarshalObjPointer   (Doxygen::symbolStorage);
  m_impl->group                   = (GroupDef*)unmarshalObjPointer   (Doxygen::symbolStorage);
  m_impl->grouppri                = (Grouping::GroupPri_t)unmarshalInt          (Doxygen::symbolStorage);
  m_impl->groupFileName           = unmarshalQCString     (Doxygen::symbolStorage);
  m_impl->groupStartLine          = unmarshalInt          (Doxygen::symbolStorage);
  m_impl->groupMember             = (MemberDef*)unmarshalObjPointer   (Doxygen::symbolStorage);
  m_impl->isTypedefValCached      = unmarshalBool         (Doxygen::symbolStorage);
  m_impl->cachedTypedefValue      = (ClassDef*)unmarshalObjPointer   (Doxygen::symbolStorage);
  m_impl->cachedTypedefTemplSpec  = unmarshalQCString     (Doxygen::symbolStorage);
  m_impl->cachedResolvedType      = unmarshalQCString     (Doxygen::symbolStorage);
  m_impl->docProvider             = (MemberDef*)unmarshalObjPointer   (Doxygen::symbolStorage);
  m_impl->explicitOutputFileBase  = unmarshalQCString     (Doxygen::symbolStorage);
  m_impl->implOnly                = unmarshalBool         (Doxygen::symbolStorage); 
  m_impl->hasDocumentedParams     = unmarshalBool         (Doxygen::symbolStorage);
  m_impl->hasDocumentedReturnType = unmarshalBool         (Doxygen::symbolStorage);
  m_impl->isDMember               = unmarshalBool         (Doxygen::symbolStorage);
  m_impl->related                 = (Relationship)unmarshalInt(Doxygen::symbolStorage);
  m_impl->stat                    = unmarshalBool         (Doxygen::symbolStorage);
  m_impl->proto                   = unmarshalBool         (Doxygen::symbolStorage);
  m_impl->docEnumValues           = unmarshalBool         (Doxygen::symbolStorage);
  m_impl->annScope                = unmarshalBool         (Doxygen::symbolStorage);
  m_impl->annUsed                 = unmarshalBool         (Doxygen::symbolStorage);
  m_impl->hasCallGraph            = unmarshalBool         (Doxygen::symbolStorage);
  m_impl->hasCallerGraph          = unmarshalBool         (Doxygen::symbolStorage);
  m_impl->explExt                 = unmarshalBool         (Doxygen::symbolStorage);
  m_impl->tspec                   = unmarshalBool         (Doxygen::symbolStorage);
  m_impl->groupHasDocs            = unmarshalBool         (Doxygen::symbolStorage);
  m_impl->docsForDefinition       = unmarshalBool         (Doxygen::symbolStorage);
  m_impl->category                = (ClassDef*)unmarshalObjPointer   (Doxygen::symbolStorage);
  m_impl->categoryRelation        = (MemberDef*)unmarshalObjPointer  (Doxygen::symbolStorage);
  marker = unmarshalUInt(Doxygen::symbolStorage);
  assert(marker==END_MARKER);

  //printf("%p: MemberDef::loadFromDisk(): sorting\n",this);
}

void MemberDef::makeResident() const
{ 
  if (Doxygen::symbolCache==0) return;
  if (m_cacheHandle==-1) // not yet in cache
  { 
    MemberDef *victim = 0;
    MemberDef *that = (MemberDef*)this; // fake method constness
    that->m_cacheHandle = Doxygen::symbolCache->add(that,(void **)&victim);
    //printf("adding %s to cache, handle=%d\n",m_impl->name.data(),that->m_cacheHandle);
    if (victim)  // cache was full, victim was the least recently used item and has to go
    {
      //printf("%p: makeResident(): cache full %p::saveToDisk(): m_impl=%p\n",this,victim,victim->m_impl);
      victim->m_cacheHandle=-1; // invalidate cache handle
      victim->saveToDisk();     // store the item on disk
    }
    else // cache not yet full
    {
      //printf("Adding %s to cache, handle=%d\n",m_impl->name.data(),m_cacheHandle);
    }
    if (m_storagePos!=-1) // already been written to disk
    {
      if (isLocked()) // locked in memory
      {
        assert(m_impl!=0);
        that->m_flushPending=FALSE; // no need to flush anymore
      }
      else // not locked in memory
      {
        assert(m_impl==0);
        loadFromDisk();
      }
    }
  }
  else // already cached, make this object the most recently used.
  {
    assert(m_impl!=0);
    //printf("Touching symbol %s\n",m_impl->name.data());
    Doxygen::symbolCache->use(m_cacheHandle);
  }
}

void MemberDef::saveToDisk() const
{
  assert(m_impl!=0);
  MemberDef *that = (MemberDef *)this;
  //printf("%p: saveToDisk(): m_impl=%p\n",this,m_impl);
  if (isLocked()) // cannot flush the item as it is locked
  {
    that->m_flushPending=TRUE; // flush when unlocked
  }
  else // ready to flush the item to disk
  {
    //printf("Adding %s to cache, handle=%d by replacing %s\n",
    //    m_impl->name.data(),m_cacheHandle,victim->m_impl->name.data());
    if (m_storagePos!=-1) 
      // if victim was stored on disk already and is not locked
    {
      // free the storage space occupied by the old store item
      Doxygen::symbolStorage->release(m_storagePos); // free up space for others
    }
    // write a the new (possibly modified) instance to disk
    flushToDisk();
    // end to write sequence (unless nothing was written due to the lock)
    Doxygen::symbolStorage->end();
  }
}

void MemberDef::lock() const
{
}

void MemberDef::unlock() const
{
  if (m_flushPending && !isLocked())
  {
    //printf("%p: flush after unlock\n",this);
    // write a the new (possibly modified) instance to disk
    flushToDisk();
    // end to write sequence (unless nothing was written due to the lock)
    Doxygen::symbolStorage->end();
  }
}

QCString MemberDef::displayName(bool) const 
{ 
  return Definition::name(); 
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

    LockingPtr<ArgumentList> mdefAl = mdef->argumentList();
    LockingPtr<ArgumentList> mdecAl = mdec->argumentList();
    if (matchArguments2(mdef->getOuterScope(),mdef->getFileDef(),mdefAl.pointer(),
          mdec->getOuterScope(),mdec->getFileDef(),mdecAl.pointer(),
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
      transferArgumentDocumentation(mdecAl.pointer(),mdefAl.pointer());

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
        //printf("transfering docs mdef->mdec (%s->%s)\n",mdef->argsString(),mdec->argsString());
        mdec->setDocumentation(mdef->documentation(),mdef->docFile(),mdef->docLine());
        mdec->setDocsForDefinition(mdef->isDocsForDefinition());
        if (mdefAl!=0)
        {
          ArgumentList *mdefAlComb = new ArgumentList;
          stringToArgumentList(mdef->argsString(),mdefAlComb);
          transferArgumentDocumentation(mdefAl.pointer(),mdefAlComb);
          mdec->setArgumentList(mdefAlComb);
        }
      }
      else if (!mdec->documentation().isEmpty())
      {
        //printf("transfering docs mdec->mdef (%s->%s)\n",mdec->argsString(),mdef->argsString());
        mdef->setDocumentation(mdec->documentation(),mdec->docFile(),mdec->docLine());
        mdef->setDocsForDefinition(mdec->isDocsForDefinition());
        if (mdecAl!=0)
        {
          ArgumentList *mdecAlComb = new ArgumentList;
          stringToArgumentList(mdec->argsString(),mdecAlComb);
          transferArgumentDocumentation(mdecAl.pointer(),mdecAlComb);
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

