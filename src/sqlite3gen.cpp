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

#include <stdlib.h>
#include <stdio.h>
#include "settings.h"
#include "message.h"

#if USE_SQLITE3

#include "qtbc.h"
#include "sqlite3gen.h"
#include "doxygen.h"
#include "config.h"
#include "util.h"
#include "docparser.h"
#include "language.h"

#include "arguments.h"
#include "classlist.h"
#include "filedef.h"
#include "namespacedef.h"
#include "filename.h"

#include <qdir.h>
#include <string.h>
#include <sqlite3.h>

//#define DBG_CTX(x) printf x
#define DBG_CTX(x) do { } while(0)

//////////////////////////////////////////////////////
const char *i_q_includes="INSERT OR REPLACE INTO includes "
                            "( refid, local, name ) "
                            "VALUES "
                            "(:refid,:local,:name )" ;
const char *c_q_includes="SELECT count(*) from includes where refid=:refid and local=:local and name=:name";
static sqlite3_stmt *i_s_includes=0;
static sqlite3_stmt *c_s_includes=0;
//////////////////////////////////////////////////////
const char *i_q_innerclass="INSERT OR REPLACE INTO innerclass "
                            "( refid, prot, name )"
                            "VALUES "
                            "(:refid,:prot,:name )";
static sqlite3_stmt *i_s_innerclass=0;
//////////////////////////////////////////////////////
const char *i_q_files="INSERT OR REPLACE INTO files "
                            "( name )"
                            "VALUES "
                            "(:name )";
const char *id_q_files="SELECT id from files where name=:name";
static sqlite3_stmt *id_s_files=0;
static sqlite3_stmt *i_s_files=0;
//////////////////////////////////////////////////////
const char *i_q_xrefs="INSERT OR REPLACE INTO xrefs "
                            "( src, dst, id_file, line, column )"
                            "VALUES "
                            "(:src,:dst,:id_file,:line,:column )";
static sqlite3_stmt *i_s_xrefs=0;
//////////////////////////////////////////////////////
const char *i_q_memberdef="INSERT OR REPLACE INTO memberdef "
                            "( refid, prot, static, const, explicit, inline, final, sealed, new, optional, required, virt, mutable, initonly, readable, writable, gettable, settable, accessor, addable, removable, raisable, name, type, definition, argsstring, scope, kind, id_bfile, bline, bcolumn, id_file, line, column)"
                            "VALUES "
                            "(:refid,:prot,:static,:const,:explicit,:inline,:final,:sealed,:new,:optional,:required,:virt,:mutable,:initonly,:readable,:writable,:gettable,:settable,:accessor,:addable,:removable,:raisable,:name,:type,:definition,:argsstring,:scope,:kind,:id_bfile,:bline,:bcolumn,:id_file,:line,:column)";
const char *id_q_memberdef="SELECT id from memberdef where refid=:refid and id is not null";
static sqlite3_stmt *id_s_memberdef=0;
static sqlite3_stmt *i_s_memberdef=0;
//////////////////////////////////////////////////////
const char *i_q_compounddef="INSERT OR REPLACE INTO compounddef "
                            "( name, kind, prot, refid, id_file, line, column ) "
                            "VALUES "
                            "(:name,:kind,:prot,:refid,:id_file,:line,:column )";
static sqlite3_stmt *i_s_compounddef=0;
//////////////////////////////////////////////////////
const char *i_q_basecompoundref="INSERT OR REPLACE INTO  basecompoundref "
                            "( base, derived, refid, prot, virt ) "
                            "VALUES "
                            "(:base,:derived,:refid,:prot,:virt )" ;
static sqlite3_stmt *i_s_basecompoundref=0;
//////////////////////////////////////////////////////
const char *i_q_derivedcompoundref="INSERT OR REPLACE INTO  derivedcompoundref "
                            "( refid, prot, virt, base, derived ) "
                            "VALUES "
                            "(:refid,:prot,:virt,:base,:derived )" ;
static sqlite3_stmt *i_s_derivedcompoundref=0;
//////////////////////////////////////////////////////

const char * schema_queries[][2] = 
{
  {
    "compounddef",
    "CREATE TABLE IF NOT EXISTS compounddef ("
      "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
      "name TEXT NOT NULL,"
      "kind TEXT NOT NULL,"
      "prot INTEGER NOT NULL,"
      "refid TEXT NOT NULL,"
      "id_file INTEGER NOT NULL,"
      "line INTEGER NOT NULL,"
      "column INTEGER NOT NULL)"
  },
  {
    "basecompoundref",
    "CREATE TABLE IF NOT EXISTS basecompoundref ("
      "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
      "base TEXT NOT NULL,"
      "derived TEXT NOT NULL,"
      "refid TEXT NOT NULL,"
      "prot INTEGER NOT NULL,"
      "virt INTEGER NOT NULL)"
  },
  {
    "derivedcompoundref",
    "CREATE TABLE IF NOT EXISTS derivedcompoundref ("
      "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
      "base TEXT NOT NULL,"
      "derived TEXT NOT NULL,"
      "refid TEXT NOT NULL,"
      "prot INTEGER NOT NULL,"
      "virt INTEGER NOT NULL)"
  },
  {
    "includes",
    "CREATE TABLE IF NOT EXISTS includes ("
      "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
      "refid TEXT NOT NULL,"
      "local INTEGER NOT NULL,"
      "name TEXT NOT NULL)"
  },
  {
    "innerclass",
    "CREATE TABLE IF NOT EXISTS innerclass ("
      "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
      "refid TEXT NOT NULL,"
      "prot INTEGER NOT NULL,"
      "name TEXT NOT NULL)"
  },
  {
    "files",
    "CREATE TABLE IF NOT EXISTS files ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
    "name TEXT NOT NULL)"
  },
  {
    "xrefs",
    "CREATE TABLE IF NOT EXISTS xrefs ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
    "src TEXT NOT NULL, "
    "dst TEXT NOT NULL, "
    "id_file INTEGER NOT NULL, "
    "line INTEGER, "
    "column INTEGER)"
  },
  {
    "memberdef",
    "CREATE TABLE IF NOT EXISTS memberdef ("
      "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
      "name TEXT NOT NULL,"
      "definition TEXT,"
      "type TEXT,"
      "argsstring TEXT,"
      "scope TEXT,"
      "prot INTEGER NOT NULL,"
      "static INTEGER NOT NULL,"
      "const INTEGER,"
      "explicit INTEGER,"
      "inline INTEGER,"
      "final INTEGER,"
      "sealed INTEGER,"
      "new INTEGER,"
      "optional INTEGER,"
      "required INTEGER,"
      "virt INTEGER,"
      "mutable INTEGER,"
      "initonly INTEGER,"
      "readable INTEGER,"
      "writable INTEGER,"
      "gettable INTEGER,"
      "settable INTEGER,"
      "accessor INTEGER,"
      "addable INTEGER,"
      "removable INTEGER,"
      "raisable INTEGER,"
      "kind INTEGER,"
      "refid TEXT NOT NULL,"
      "id_bfile INTEGER,"
      "bline INTEGER,"
      "bcolumn INTEGER,"
      "id_file INTEGER NOT NULL,"
      "line INTEGER NOT NULL,"
      "column INTEGER NOT NULL)"
  },
};


class TextGeneratorSqlite3Impl : public TextGeneratorIntf
{
  public:
    TextGeneratorSqlite3Impl(StringList &l) : l(l) {
      l.setAutoDelete(TRUE);
    }
    void writeString(const char * /*s*/,bool /*keepSpaces*/) const
    {
    }
    void writeBreak(int) const
    {
      DBG_CTX(("writeBreak\n"));
    }
    void writeLink(const char * /*extRef*/,const char *file,
                   const char *anchor,const char * /*text*/
                  ) const
    {
      QCString *rs=new QCString(file);
      if (anchor)
      {
        rs->append("_1").append(anchor);
      }
      l.append(rs);
    }
  private:
    StringList &l;
    // the list is filled by linkifyText and consumed by the caller
};

static void generateSqlite3ForMember(sqlite3*db,MemberDef *md,Definition *def);


static void bindTextParameter(sqlite3_stmt *stmt,const char *name,const char *value)
{
  int idx = sqlite3_bind_parameter_index(stmt, name);
  sqlite3_bind_text(id_s_files, idx, value, -1, SQLITE_STATIC);
}

static void bindIntParameter(sqlite3_stmt *stmt,const char *name,int value)
{
  int idx = sqlite3_bind_parameter_index(stmt, name);
  sqlite3_bind_int(stmt, idx, value);
}

static int step(sqlite3 *db, sqlite3_stmt *stmt,bool getRowId=FALSE)
{
  int id=-1;
  int rc = sqlite3_step(stmt);
  if (rc!=SQLITE_DONE && rc!=SQLITE_ROW) 
  {
    msg("failed count files: %s\n", sqlite3_errmsg(db));
  }
  if (getRowId) id = sqlite3_column_int(stmt, 0);
  sqlite3_reset(stmt);
  return id;
}

#if 0
static QCString memberOutputFileBase(MemberDef *md)
{
  //static bool inlineGroupedClasses = Config_getBool("INLINE_GROUPED_CLASSES");
  //if (inlineGroupedClasses && md->getClassDef() && md->getClassDef()->partOfGroups()!=0)
  //  return md->getClassDef()->getXmlOutputFileBase();
  //else
  //  return md->getOutputFileBase();
  return md->getOutputFileBase();
}
#endif

static int insertFile(sqlite3 *db, const char* name)
{
  int id=-1;
  if (name==0) return -1;
  // see if it's already in DB

  bindTextParameter(id_s_files,":name",name);
  id=step(db,i_s_files,TRUE);
  if (id==0)
  {
    // insert it
    bindTextParameter(i_s_files,":name",name);

    step(db,i_s_files);
    id=sqlite3_last_insert_rowid(db);
  }
  return id;
}

static void insertMemberReference(sqlite3 *db, const char*src, const char*dst, const char *file, int line, int column)
{
#if 0
  QCString scope = dst->getScopeString();
  QCString src_name = src->name();
  QCString dst_name = dst->name();
  if (!dst->getScopeString().isEmpty() && dst->getScopeString()!=def->name())
  {
    dst_name.prepend(scope+getLanguageSpecificSeparator(dst->getLanguage()));
  }
  if (!src->getScopeString().isEmpty() && src->getScopeString()!=def->name())
  {
    src_name.prepend(scope+getLanguageSpecificSeparator(src->getLanguage()));
  }
#endif
    //
  bindTextParameter(i_s_xrefs,":src",src);
  bindTextParameter(i_s_xrefs,":dst",dst);

  int id_file = insertFile(db,file);

  bindIntParameter(i_s_xrefs,":id_file",id_file);
  bindIntParameter(i_s_xrefs,":line",line);
  bindIntParameter(i_s_xrefs,":column",column);

  step(db,i_s_xrefs);
}

static void insertMemberReference(sqlite3 *db, MemberDef *src, MemberDef *dst, const char*floc)
{
  if (dst->getStartBodyLine()!=-1 && dst->getBodyDef())
  {
    char file[4096] = { 0 };
    int line=0,column=0;
    if (floc)
    {
      sscanf(floc,"%[^:]:%d:%d",file,&line,&column);
    }
    insertMemberReference(db,src->anchor().data(),dst->anchor().data(),file,line,column);
  }
}

static void stripQualifiers(QCString &typeStr)
{
  bool done=FALSE;
  while (!done)
  {
    if      (typeStr.stripPrefix("static "));
    else if (typeStr.stripPrefix("virtual "));
    else if (typeStr.stripPrefix("volatile "));
    else if (typeStr=="virtual") typeStr="";
    else done=TRUE;
  }
}

////////////////////////////////////////////
static void writeInnerClasses(sqlite3*db,const ClassSDict *cl)
{
  if (!cl) return;

  ClassSDict::Iterator cli(*cl);
  ClassDef *cd;
  for (cli.toFirst();(cd=cli.current());++cli)
  {
    if (!cd->isHidden() && cd->name().find('@')==-1) // skip anonymous scopes
    {
      bindTextParameter(i_s_innerclass,":refid",cd->getOutputFileBase());
      bindIntParameter(i_s_innerclass,":prot",cd->protection());
      bindTextParameter(i_s_innerclass,":name",cd->name());
      step(db,i_s_innerclass);
    }
  }
}

static void writeInnerNamespaces(sqlite3 * /*db*/,const NamespaceSDict *nl)
{
  if (nl)
  {
    NamespaceSDict::Iterator nli(*nl);
    NamespaceDef *nd;
    for (nli.toFirst();(nd=nli.current());++nli)
    {
      if (!nd->isHidden() && nd->name().find('@')==-1) // skip anonymouse scopes
      {
//        t << "    <innernamespace refid=\"" << nd->getOutputFileBase()
//          << "\">" << convertToXML(nd->name()) << "</innernamespace>" << endl;
      }
    }
  }
}

static void writeTemplateArgumentList(sqlite3* /*db*/,
                                      ArgumentList * /*al*/,
                                      Definition * /*scope*/,
                                      FileDef * /*fileScope*/,
                                      int /*indent*/)
{
#if 0
  QCString indentStr;
  indentStr.fill(' ',indent);
  if (al)
  {
    t << indentStr << "<templateparamlist>" << endl;
    ArgumentListIterator ali(*al);
    Argument *a;
    for (ali.toFirst();(a=ali.current());++ali)
    {
      t << indentStr << "  <param>" << endl;
      if (!a->type.isEmpty())
      {
        t << indentStr <<  "    <type>";
        linkifyText(TextGeneratorXMLImpl(t),scope,fileScope,0,a->type);
        t << "</type>" << endl;
      }
      if (!a->name.isEmpty())
      {
        t << indentStr <<  "    <declname>" << a->name << "</declname>" << endl;
        t << indentStr <<  "    <defname>" << a->name << "</defname>" << endl;
      }
      if (!a->defval.isEmpty())
      {
        t << indentStr << "    <defval>";
        linkifyText(TextGeneratorXMLImpl(t),scope,fileScope,0,a->defval);
        t << "</defval>" << endl;
      }
      t << indentStr << "  </param>" << endl;
    }
    t << indentStr << "</templateparamlist>" << endl;
  }
#endif
}

static void writeTemplateList(sqlite3*db,ClassDef *cd)
{
  writeTemplateArgumentList(db,cd->templateArguments(),cd,0,4);
}

static void generateSqlite3Section(sqlite3*db,
                      Definition *d,
                      MemberList *ml,const char * /*kind*/,const char * /*header*/=0,
                      const char * /*documentation*/=0)
{
  if (ml==0) return;
  MemberListIterator mli(*ml);
  MemberDef *md;
  int count=0;
  for (mli.toFirst();(md=mli.current());++mli)
  {
    // namespace members are also inserted in the file scope, but
    // to prevent this duplication in the XML output, we filter those here.
    if (d->definitionType()!=Definition::TypeFile || md->getNamespaceDef()==0)
    {
      count++;
    }
  }
  if (count==0) return; // empty list
#if 0

  t << "      <sectiondef kind=\"" << kind << "\">" << endl;
  if (header)
  {
    t << "      <header>" << convertToXML(header) << "</header>" << endl;
  }
  if (documentation)
  {
    t << "      <description>";
    writeXMLDocBlock(t,d->docFile(),d->docLine(),d,0,documentation);
    t << "</description>" << endl;
  }
#endif
  for (mli.toFirst();(md=mli.current());++mli)
  {
    // namespace members are also inserted in the file scope, but
    // to prevent this duplication in the XML output, we filter those here.
    //if (d->definitionType()!=Definition::TypeFile || md->getNamespaceDef()==0)
    {
      generateSqlite3ForMember(db,md,d);
    }
  }
  //t << "      </sectiondef>" << endl;
}

static int prepareStatements(sqlite3 *db)
{
  int rc;
  rc = sqlite3_prepare_v2(db,id_q_memberdef,-1,&id_s_memberdef,0);
  if (rc!=SQLITE_OK) 
  {
    msg("prepare failed for %s\n%s\n", id_q_memberdef, sqlite3_errmsg(db));
    return -1;
  }
  rc = sqlite3_prepare_v2(db,id_q_files,-1,&id_s_files,0);
  if (rc!=SQLITE_OK) 
  {
    msg("prepare failed for %s\n%s\n", id_q_files, sqlite3_errmsg(db));
    return -1;
  }
  rc = sqlite3_prepare_v2(db,i_q_files,-1,&i_s_files,0);
  if (rc!=SQLITE_OK) 
  {
    msg("prepare failed for %s\n%s\n",i_q_files,sqlite3_errmsg(db));
    return -1;
  }
  rc = sqlite3_prepare_v2(db,i_q_xrefs,-1,&i_s_xrefs,0);
  if (rc!=SQLITE_OK) 
  {
    msg("prepare failed for %s\n%s\n", i_q_xrefs, sqlite3_errmsg(db));
    return -1;
  }
  rc = sqlite3_prepare_v2(db, i_q_innerclass, -1, &i_s_innerclass, 0);
  if (rc!=SQLITE_OK) 
  {
    msg("prepare failed for %s\n%s\n", i_q_innerclass, sqlite3_errmsg(db));
    return -1;
  }
  rc = sqlite3_prepare_v2(db,i_q_memberdef,-1,&i_s_memberdef,0);
  if (rc!=SQLITE_OK) 
  {
    msg("prepare failed for %s\n%s\n",i_q_memberdef,sqlite3_errmsg(db));
    return -1;
  }
  rc = sqlite3_prepare_v2(db,i_q_compounddef,-1,&i_s_compounddef,0);
  if (rc!=SQLITE_OK) 
  {
    msg("prepare failed for %s\n%s\n",i_q_compounddef,sqlite3_errmsg(db));
    return -1;
  }
  rc = sqlite3_prepare_v2(db,i_q_basecompoundref,-1,&i_s_basecompoundref,0);
  if (rc!=SQLITE_OK) 
  {
    msg("prepare failed for %s\n%s\n",i_q_basecompoundref,sqlite3_errmsg(db));
    return -1;
  }
  rc = sqlite3_prepare_v2(db,i_q_derivedcompoundref,-1,&i_s_derivedcompoundref,0);
  if (rc!=SQLITE_OK) 
  {
    msg("prepare failed for %s\n%s\n",i_q_derivedcompoundref,sqlite3_errmsg(db));
    return -1;
  }
  rc = sqlite3_prepare_v2(db, i_q_includes, -1, &i_s_includes, 0);
  if (rc!=SQLITE_OK) 
  {
    msg("prepare failed for %s\n%s\n", i_q_includes, sqlite3_errmsg(db));
    return -1;
  }
  rc = sqlite3_prepare_v2(db, c_q_includes, -1, &c_s_includes, 0);
  if (rc!=SQLITE_OK) 
  {
    msg("prepare failed for %s\n%s\n", c_q_includes, sqlite3_errmsg(db));
    return -1;
  }
  return 0;
}

static void beginTransaction(sqlite3 *db)
{
  char * sErrMsg = 0;
  sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &sErrMsg);
}

static void endTransaction(sqlite3 *db)
{
  char * sErrMsg = 0;
  sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &sErrMsg);
}

static void pragmaTuning(sqlite3 *db)
{
  char * sErrMsg = 0;
  sqlite3_exec(db, "PRAGMA synchronous = OFF", NULL, NULL, &sErrMsg);
  sqlite3_exec(db, "PRAGMA journal_mode = MEMORY", NULL, NULL, &sErrMsg);
}

static void initializeSchema(sqlite3* db) 
{
  int rc;
  sqlite3_stmt *stmt = 0;

  msg("Initializing DB schema...\n");
  for (unsigned int k = 0; k < sizeof(schema_queries) / sizeof(schema_queries[0]); k++) 
  {
    //const char *tname = schema_queries[k][0];
    const char *q = schema_queries[k][1];
    // create table
    rc = sqlite3_prepare_v2(db, q, -1, &stmt, 0);
    if (rc != SQLITE_OK) 
    {
      msg("failed to prepare query: %s\n\t%s\n", q, sqlite3_errmsg(db));
      exit(-1);
    }
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) 
    {
      msg("failed to execute query: %s\n\t%s\n", q, sqlite3_errmsg(db));
      exit(-1);
    }
    sqlite3_finalize(stmt);

  }
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

static void generateSqlite3ForNamespace(sqlite3 *db, NamespaceDef *nd)
{
  // + contained class definitions
  // + contained namespace definitions
  // + member groups
  // + normal members
  // + brief desc
  // + detailed desc
  // + location
  // - files containing (parts of) the namespace definition

  if (nd->isReference() || nd->isHidden()) return; // skip external references
#if 0
  ti << "  <compound refid=\"" << nd->getOutputFileBase()
     << "\" kind=\"namespace\"" << "><name>"
     << convertToXML(nd->name()) << "</name>" << endl;

  QCString outputDirectory = Config_getString("XML_OUTPUT");
  QCString fileName=outputDirectory+"/"+nd->getOutputFileBase()+".xml";
  QFile f(fileName);
  if (!f.open(IO_WriteOnly))
  {
    err("Cannot open file %s for writing!\n",fileName.data());
    return;
  }
  FTextStream t(&f);
  //t.setEncoding(FTextStream::UnicodeUTF8);

  writeXMLHeader(t);
  t << "  <compounddef id=\""
    << nd->getOutputFileBase() << "\" kind=\"namespace\">" << endl;
  t << "    <compoundname>";
  writeXMLString(t,nd->name());
  t << "</compoundname>" << endl;
#endif
  writeInnerClasses(db,nd->getClassSDict());
  writeInnerNamespaces(db,nd->getNamespaceSDict());

  if (nd->getMemberGroupSDict())
  {
    MemberGroupSDict::Iterator mgli(*nd->getMemberGroupSDict());
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      generateSqlite3Section(db,nd,mg->members(),"user-defined",mg->header(),
          mg->documentation());
    }
  }

  QListIterator<MemberList> mli(nd->getMemberLists());
  MemberList *ml;
  for (mli.toFirst();(ml=mli.current());++mli)
  {
    if ((ml->listType()&MemberListType_declarationLists)!=0)
    {
      generateSqlite3Section(db,nd,ml,"user-defined");//g_xmlSectionMapper.find(ml->listType()));
    }
  }
#if 0
  generateXMLSection(nd,ti,t,&nd->decDefineMembers,"define");
  generateXMLSection(nd,ti,t,&nd->decProtoMembers,"prototype");
  generateXMLSection(nd,ti,t,&nd->decTypedefMembers,"typedef");
  generateXMLSection(nd,ti,t,&nd->decEnumMembers,"enum");
  generateXMLSection(nd,ti,t,&nd->decFuncMembers,"func");
  generateXMLSection(nd,ti,t,&nd->decVarMembers,"var");

  t << "    <briefdescription>" << endl;
  writeXMLDocBlock(t,nd->briefFile(),nd->briefLine(),nd,0,nd->briefDescription());
  t << "    </briefdescription>" << endl;
  t << "    <detaileddescription>" << endl;
  writeXMLDocBlock(t,nd->docFile(),nd->docLine(),nd,0,nd->documentation());
  t << "    </detaileddescription>" << endl;
  t << "    <location file=\""
    << nd->getDefFileName() << "\" line=\""
    << nd->getDefLine() << "\"" << " column=\""
    << nd->getDefColumn() << "\"/>" << endl ;
  t << "  </compounddef>" << endl;
  t << "</doxygen>" << endl;

  ti << "  </compound>" << endl;
#endif
}


static void generateSqlite3ForFile(sqlite3 *db, FileDef *fd)
{
  // + includes files
  // + includedby files
  // + include graph
  // + included by graph
  // + contained class definitions
  // + contained namespace definitions
  // + member groups
  // + normal members
  // + brief desc
  // + detailed desc
  // + source code
  // + location
  // - number of lines

  if (fd->isReference()) return; // skip external references
#if 0
  ti << "  <compound refid=\"" << fd->getOutputFileBase()
     << "\" kind=\"file\"><name>" << convertToXML(fd->name())
     << "</name>" << endl;

  QCString outputDirectory = Config_getString("XML_OUTPUT");
  QCString fileName=outputDirectory+"/"+fd->getOutputFileBase()+".xml";
  QFile f(fileName);
  if (!f.open(IO_WriteOnly))
  {
    err("Cannot open file %s for writing!\n",fileName.data());
    return;
  }
  FTextStream t(&f);
  //t.setEncoding(FTextStream::UnicodeUTF8);

  writeXMLHeader(t);
  t << "  <compounddef id=\""
    << fd->getOutputFileBase() << "\" kind=\"file\">" << endl;
  t << "    <compoundname>";
  writeXMLString(t,fd->name());
  t << "</compoundname>" << endl;

  IncludeInfo *inc;

  if (fd->includeFileList())
  {
    QListIterator<IncludeInfo> ili1(*fd->includeFileList());
    for (ili1.toFirst();(inc=ili1.current());++ili1)
    {
      t << "    <includes";
      if (inc->fileDef && !inc->fileDef->isReference()) // TODO: support external references
      {
        t << " refid=\"" << inc->fileDef->getOutputFileBase() << "\"";
      }
      t << " local=\"" << (inc->local ? "yes" : "no") << "\">";
      t << inc->includeName;
      t << "</includes>" << endl;
    }
  }

  if (fd->includedByFileList())
  {
    QListIterator<IncludeInfo> ili2(*fd->includedByFileList());
    for (ili2.toFirst();(inc=ili2.current());++ili2)
    {
      t << "    <includedby";
      if (inc->fileDef && !inc->fileDef->isReference()) // TODO: support external references
      {
        t << " refid=\"" << inc->fileDef->getOutputFileBase() << "\"";
      }
      t << " local=\"" << (inc->local ? "yes" : "no") << "\">";
      t << inc->includeName;
      t << "</includedby>" << endl;
    }
  }

  DotInclDepGraph incDepGraph(fd,FALSE);
  if (!incDepGraph.isTrivial())
  {
    t << "    <incdepgraph>" << endl;
    incDepGraph.writeXML(t);
    t << "    </incdepgraph>" << endl;
  }

  DotInclDepGraph invIncDepGraph(fd,TRUE);
  if (!invIncDepGraph.isTrivial())
  {
    t << "    <invincdepgraph>" << endl;
    invIncDepGraph.writeXML(t);
    t << "    </invincdepgraph>" << endl;
  }
#endif
  if (fd->getClassSDict())
  {
    writeInnerClasses(db,fd->getClassSDict());
  }
  if (fd->getNamespaceSDict())
  {
    writeInnerNamespaces(db,fd->getNamespaceSDict());
  }

  if (fd->getMemberGroupSDict())
  {
    MemberGroupSDict::Iterator mgli(*fd->getMemberGroupSDict());
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      generateSqlite3Section(db,fd,mg->members(),"user-defined",mg->header(),
          mg->documentation());
    }
  }

  QListIterator<MemberList> mli(fd->getMemberLists());
  MemberList *ml;
  for (mli.toFirst();(ml=mli.current());++mli)
  {
    if ((ml->listType()&MemberListType_declarationLists)!=0)
    {
      generateSqlite3Section(db,fd,ml,"user-defined");//g_xmlSectionMapper.find(ml->listType()));
    }
  }
#if 0
  generateXMLSection(fd,ti,t,fd->decDefineMembers,"define");
  generateXMLSection(fd,ti,t,fd->decProtoMembers,"prototype");
  generateXMLSection(fd,ti,t,fd->decTypedefMembers,"typedef");
  generateXMLSection(fd,ti,t,fd->decEnumMembers,"enum");
  generateXMLSection(fd,ti,t,fd->decFuncMembers,"func");
  generateXMLSection(fd,ti,t,fd->decVarMembers,"var");
#endif
#if 0
  t << "    <briefdescription>" << endl;
  writeXMLDocBlock(t,fd->briefFile(),fd->briefLine(),fd,0,fd->briefDescription());
  t << "    </briefdescription>" << endl;
  t << "    <detaileddescription>" << endl;
  writeXMLDocBlock(t,fd->docFile(),fd->docLine(),fd,0,fd->documentation());
  t << "    </detaileddescription>" << endl;
  if (Config_getBool("XML_PROGRAMLISTING"))
  {
    t << "    <programlisting>" << endl;
    writeXMLCodeBlock(t,fd);
    t << "    </programlisting>" << endl;
  }
  t << "    <location file=\"" << fd->getDefFileName() << "\"/>" << endl;
  t << "  </compounddef>" << endl;
  t << "</doxygen>" << endl;

  ti << "  </compound>" << endl;
#endif
}




static void generateSqlite3ForMember(sqlite3*db,MemberDef *md,Definition *def)
{
  // + declaration/definition arg lists
  // + reimplements
  // + reimplementedBy
  // + exceptions
  // + const/volatile specifiers
  // - examples
  // + source definition
  // + source references
  // + source referenced by
  // - body code
  // + template arguments
  //     (templateArguments(), definitionTemplateParameterLists())
  // - call graph

  // enum values are written as part of the enum
  if (md->memberType()==MemberType_EnumValue) return;
  if (md->isHidden()) return;
  //if (md->name().at(0)=='@') return; // anonymous member

  // group members are only visible in their group
  //if (def->definitionType()!=Definition::TypeGroup && md->getGroupDef()) return;
  QCString memType;
#if 0
  // member
  idx = sqlite3_bind_parameter_index(stmt, ":refid");
  sqlite3_bind_text(stmt, idx, memberOutputFileBase(md).data(),-1,SQLITE_TRANSIENT);

  idx = sqlite3_bind_parameter_index(stmt,":kind");
  sqlite3_bind_int(stmt, idx, md->memberType());

  idx = sqlite3_bind_parameter_index(stmt, ":name");
  sqlite3_bind_text(stmt, idx, md->name().data(),-1,SQLITE_TRANSIENT);
#endif
  // memberdef
  bindTextParameter(i_s_memberdef,":refid",md->anchor());
  bindIntParameter(i_s_memberdef,":kind",md->memberType());
  bindIntParameter(i_s_memberdef,":prot",md->protection());
  bindIntParameter(i_s_memberdef,":static",md->isStatic());

  bool isFunc=FALSE;
  switch (md->memberType())
  {
    case MemberType_Function: // fall through
    case MemberType_Signal:   // fall through
    case MemberType_Friend:   // fall through
    case MemberType_DCOP:     // fall through
    case MemberType_Slot:        
      isFunc=TRUE; 
      break;
    default: 
      break;
  }
  if (isFunc)
  {
    LockingPtr<ArgumentList> al = md->argumentList();
    if (al!=0 && al->constSpecifier) 
    {
      bindIntParameter(i_s_memberdef,":const",al->constSpecifier);
    }

    bindIntParameter(i_s_memberdef,":explicit",md->isExplicit());
    bindIntParameter(i_s_memberdef,":inline",md->isInline());
    bindIntParameter(i_s_memberdef,":final",md->isFinal());
    bindIntParameter(i_s_memberdef,":sealed",md->isSealed());
    bindIntParameter(i_s_memberdef,":new",md->isNew());
    bindIntParameter(i_s_memberdef,":optional",md->isOptional());
    bindIntParameter(i_s_memberdef,":required",md->isRequired());
    bindIntParameter(i_s_memberdef,":virt",md->virtualness());
  }
  // place in the arguments and linkify the arguments

  if (md->memberType() == MemberType_Variable)
  {
    bindIntParameter(i_s_memberdef,":mutable",md->isMutable());
    bindIntParameter(i_s_memberdef,":initonly",md->isInitonly());
  }
  else if (md->memberType() == MemberType_Property)
  {
    bindIntParameter(i_s_memberdef,":readable",md->isReadable());
    bindIntParameter(i_s_memberdef,":writable",md->isWritable());
    bindIntParameter(i_s_memberdef,":gettable",md->isGettable());
    bindIntParameter(i_s_memberdef,":settable",md->isSettable());

    if (md->isAssign() || md->isCopy() || md->isRetain())
    {
      int accessor = md->isAssign() ? md->isAssign() :
          (md->isCopy() ? md->isCopy() : md->isRetain()) ;

      bindIntParameter(i_s_memberdef,":accessor",accessor);
    }
  }
  else if (md->memberType() == MemberType_Event)
  {
    bindIntParameter(i_s_memberdef,":addable",md->isAddable());
    bindIntParameter(i_s_memberdef,":removable",md->isRemovable());
    bindIntParameter(i_s_memberdef,":raisable",md->isRaisable());
  }

  if (md->memberType()!=MemberType_Define &&
      md->memberType()!=MemberType_Enumeration
     )
  {
    QCString typeStr = md->typeString();
    stripQualifiers(typeStr);
    StringList l;
    linkifyText(TextGeneratorSqlite3Impl(l),def,md->getBodyDef(),md,typeStr);
    if (typeStr.data()) 
    {
      bindTextParameter(i_s_memberdef,":type",typeStr);
    }

    if (md->definition()) 
    {
      bindTextParameter(i_s_memberdef,":definition",md->definition());
    }

    if (md->argsString()) 
    {
      bindTextParameter(i_s_memberdef,":argsstring",md->argsString());
    }
  }

  bindTextParameter(i_s_memberdef,":name",md->name());

  if (md->memberType() == MemberType_Property)
  {
    if (md->isReadable())
    {
      DBG_CTX(("<read>\n"));
    }
    if (md->isWritable())
    {
      DBG_CTX(("<write>\n"));
    }
  }
#if 0
  if (md->memberType()==MemberType_Variable && md->bitfieldString())
  {
    QCString bitfield = md->bitfieldString();
    if (bitfield.at(0)==':') bitfield=bitfield.mid(1);
    t << "        <bitfield>" << bitfield << "</bitfield>" << endl;
  }

  MemberDef *rmd = md->reimplements();
  if (rmd)
  {
    t << "        <reimplements refid=\""
      << memberOutputFileBase(rmd) << "_1" << rmd->anchor() << "\">"
      << convertToXML(rmd->name()) << "</reimplements>" << endl;
  }
  LockingPtr<MemberList> rbml = md->reimplementedBy();
  if (rbml!=0)
  {
    MemberListIterator mli(*rbml);
    for (mli.toFirst();(rmd=mli.current());++mli)
    {
      t << "        <reimplementedby refid=\""
        << memberOutputFileBase(rmd) << "_1" << rmd->anchor() << "\">"
        << convertToXML(rmd->name()) << "</reimplementedby>" << endl;
    }
  }
#endif
  if (isFunc) //function
  {
    LockingPtr<ArgumentList> declAl = md->declArgumentList();
    LockingPtr<ArgumentList> defAl = md->argumentList();
    if (declAl!=0 && declAl->count()>0)
    {
      ArgumentListIterator declAli(*declAl);
      ArgumentListIterator defAli(*defAl);
      Argument *a;
      for (declAli.toFirst();(a=declAli.current());++declAli)
      {
        Argument *defArg = defAli.current();
        DBG_CTX(("<param>\n"));
        if (!a->attrib.isEmpty())
        {
          DBG_CTX(("<attributes>:%s\n",a->attrib.data()));
        }
        if (!a->type.isEmpty())
        {
          StringList l;
          linkifyText(TextGeneratorSqlite3Impl(l),def,md->getBodyDef(),md,a->type);

          QCString *s=l.first();
          while (s) 
          {
            insertMemberReference(db,md->anchor().data(),s->data(),def->getDefFileName().data(),md->getDefLine(),1);
            s=l.next();
          }
        }
        if (!a->name.isEmpty())
        {
          DBG_CTX(("<declname>%s\n",a->name.data()));
        }
        if (defArg && !defArg->name.isEmpty() && defArg->name!=a->name)
        {
          DBG_CTX(("<defname>%s\n",defArg->name.data()));
        }
        if (!a->array.isEmpty())
        {
          DBG_CTX(("<array>%s",a->array.data()));
        }
        if (!a->defval.isEmpty())
        {
          StringList l;
          linkifyText(TextGeneratorSqlite3Impl(l),def,md->getBodyDef(),md,a->defval);
        }
        if (defArg) ++defAli;
      }
    }
  }
  else if (md->memberType()==MemberType_Define &&
          md->argsString()) // define
  {
    if (md->argumentList()->count()==0) // special case for "foo()" to
                                        // disguish it from "foo".
    {
      DBG_CTX(("no params\n"));
    }
    else
    {
      ArgumentListIterator ali(*md->argumentList());
      Argument *a;
      for (ali.toFirst();(a=ali.current());++ali)
      {
        DBG_CTX(("<param><defname>%s\n",a->type.data()));
      }
    }
  }


  // Extract references from initializer
  // avoid that extremely large tables are written to the output.
  // todo: it's better to adhere to MAX_INITIALIZER_LINES.
  // drm_mod_register_buffer,
  if (!md->initializer().isEmpty() && md->initializer().length()<2000)
  {
    StringList l;
    linkifyText(TextGeneratorSqlite3Impl(l),def,md->getBodyDef(),md,md->initializer());
    QCString *s=l.first();
    while (s) 
    {
      DBG_CTX(("initializer:%s %s %s %d\n",
              md->anchor().data(),
              s->data(), 
              md->getBodyDef()->getDefFileName().data(), 
              md->getStartBodyLine()));
      insertMemberReference(db,md->anchor().data(),s->data(),md->getBodyDef()->getDefFileName().data(),md->getStartBodyLine(),1);
      s=l.next();
    }
  }

#if 0
  if (md->excpString())
  {
    linkifyText(TextGeneratorXMLImpl(t),def,md->getBodyDef(),md,md->excpString());
  }
#endif
  if ( md->getScopeString() ) 
  {
    bindTextParameter(i_s_memberdef,":scope",md->getScopeString());
  }

  // File location
  if (md->getDefLine() != -1)
  {
    int id_file = insertFile(db,md->getDefFileName());
    if (id_file!=-1) 
    {
      bindIntParameter(i_s_memberdef,":id_file",id_file);
      bindIntParameter(i_s_memberdef,":line",md->getDefLine());
      bindIntParameter(i_s_memberdef,":column",md->getDefColumn());

      if (md->getStartBodyLine()!=-1) 
      {
        int id_bfile = insertFile(db,md->getBodyDef()->absFilePath());
        if (id_bfile == -1) exit(-1);
        bindIntParameter(i_s_memberdef,":id_ibfile",id_bfile);
        bindIntParameter(i_s_memberdef,":bline",md->getStartBodyLine());

        // XXX implement getStartBodyColumn
        bindIntParameter(i_s_memberdef,":bcolumn",1);
      }
    }
  }


  step(db,i_s_memberdef);
  /*int id_src =*/ sqlite3_last_insert_rowid(db);

  // + cross-references
  // The cross-references in initializers only work when both the src and dst
  // are defined.
  LockingPtr<MemberSDict> mdict = md->getReferencesMembers();
  // references
  if (mdict!=0)
  {
    MemberSDict::IteratorDict mdi(*mdict);
    MemberDef *rmd;
    for (mdi.toFirst();(rmd=mdi.current());++mdi)
    {
      insertMemberReference(db,md,rmd,mdi.currentKey());
    }
  }

  mdict = md->getReferencedByMembers();
  // referencedby
  if (mdict!=0)
  {
    MemberSDict::IteratorDict mdi(*mdict);
    MemberDef *rmd;
    for (mdi.toFirst();(rmd=mdi.current());++mdi)
    {
      insertMemberReference(db,rmd,md,mdi.currentKey());
    }
  }
}

static void generateSqlite3ForClass(sqlite3 *db, ClassDef *cd)
{
  // + brief description
  // + detailed description
  // + template argument list(s)
  // - include file
  // + member groups
  // + inheritance diagram
  // + list of direct super classes
  // + list of direct sub classes
  // + list of inner classes
  // + collaboration diagram
  // + list of all members
  // + user defined member sections
  // + standard member sections
  // + detailed member documentation
  // - examples using the class

  if (cd->isReference())        return; // skip external references.
  if (cd->isHidden())           return; // skip hidden classes.
  if (cd->name().find('@')!=-1) return; // skip anonymous compounds.
  if (cd->templateMaster()!=0)  return; // skip generated template instances.

  msg("Generating Sqlite3 output for class %s\n",cd->name().data());

  //int idx,rc;
  //sqlite3_stmt *stmt ;

  // + compounddef
  //stmt = i_s_compounddef;

  bindTextParameter(i_s_compounddef,":name",cd->name());
  bindTextParameter(i_s_compounddef,":kind",cd->compoundTypeString());
  bindIntParameter(i_s_compounddef,":prot",cd->protection());
  bindTextParameter(i_s_compounddef,":refid",cd->getOutputFileBase());

  int id_file = insertFile(db,cd->getDefFileName().data());
  bindIntParameter(i_s_compounddef,":id_file",id_file);
  bindIntParameter(i_s_compounddef,":line",cd->getDefLine());
  bindIntParameter(i_s_compounddef,":column",cd->getDefColumn());

  step(db,i_s_compounddef);
  sqlite3_int64 id_compound=0 ;
  id_compound = sqlite3_last_insert_rowid(db);

  // + basecompoundref
  if (cd->baseClasses())
  {
    BaseClassListIterator bcli(*cd->baseClasses());
    BaseClassDef *bcd;
    for (bcli.toFirst();(bcd=bcli.current());++bcli)
    {
      bindTextParameter(i_s_basecompoundref,":refid",bcd->classDef->getOutputFileBase());
      bindIntParameter(i_s_basecompoundref,":prot",bcd->prot);
      bindIntParameter(i_s_basecompoundref,":virt",bcd->virt);

      if (!bcd->templSpecifiers.isEmpty())
      {
        bindTextParameter(i_s_basecompoundref,":base",insertTemplateSpecifierInScope(bcd->classDef->name(),bcd->templSpecifiers));
      }
      else
      {
        bindTextParameter(i_s_basecompoundref,":base",bcd->classDef->displayName());
      }
      bindTextParameter(i_s_basecompoundref,":derived",cd->displayName());
      step(db,i_s_basecompoundref);
    }
  }

  // + derivedcompoundref
  if (cd->subClasses())
  {
    BaseClassListIterator bcli(*cd->subClasses());
    BaseClassDef *bcd;
    for (bcli.toFirst();(bcd=bcli.current());++bcli)
    {
      bindTextParameter(i_s_derivedcompoundref,":base",cd->displayName());
      bindTextParameter(i_s_derivedcompoundref,":dervied",bcd->classDef->displayName());
      bindTextParameter(i_s_derivedcompoundref,":refid",bcd->classDef->getOutputFileBase());
      bindIntParameter(i_s_derivedcompoundref,":prot",bcd->prot);
      bindIntParameter(i_s_derivedcompoundref,":virt",bcd->virt);
      step(db,i_s_derivedcompoundref);
    }
  }

  ///////////////////////////////////////////////////////////////////
  // INCLUDEINFO
  IncludeInfo *ii=cd->includeInfo();
  if (ii)
  {
    QCString nm = ii->includeName;
    if (nm.isEmpty() && ii->fileDef) nm = ii->fileDef->docName();
    if (!nm.isEmpty())
    {
      bindTextParameter(c_s_includes,":refid",ii->fileDef->getOutputFileBase());
      bindIntParameter(c_s_includes,":local",ii->local);
      bindTextParameter(c_s_includes,":name",nm);
      int count=step(db,c_s_includes,TRUE);
      if ( count==0 ) 
      {
        bindTextParameter(i_s_includes,":refid",ii->fileDef->getOutputFileBase());
        bindIntParameter(i_s_includes,":local",ii->local);
        bindTextParameter(i_s_includes,":name",nm);
        step(db,i_s_includes);
      }
    }
  }
  ///////////////////////////////////////////////////////////////////
  writeInnerClasses(db,cd->getClassSDict());
  writeTemplateList(db,cd);

  if (cd->getMemberGroupSDict())
  {
    MemberGroupSDict::Iterator mgli(*cd->getMemberGroupSDict());
    MemberGroup *mg;
    for (;(mg=mgli.current());++mgli)
    {
      generateSqlite3Section(db,cd,mg->members(),"user-defined",mg->header(),
          mg->documentation());
    }
  }

  QListIterator<MemberList> mli(cd->getMemberLists());
  MemberList *ml;
  for (mli.toFirst();(ml=mli.current());++mli)
  {
    if ((ml->listType()&MemberListType_detailedLists)==0)
    {
      generateSqlite3Section(db,cd,ml,"user-defined");//g_xmlSectionMapper.find(ml->listType()));
    }
  }
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void generateSqlite3()
{
  // + classes
  // + namespaces
  // + files
  // - groups
  // - related pages
  // - examples
  //QCString outputDirectory = Config_getString("SQLITE3_OUTPUT");
  QCString outputDirectory = Config_getString("OUTPUT_DIRECTORY");
  QDir sqlite3Dir(outputDirectory);
  sqlite3 *db;
  sqlite3_initialize();
  int rc = sqlite3_open_v2(outputDirectory+"/doxygen_sqlite3.db", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);
  if (rc != SQLITE_OK) 
  {
    sqlite3_close(db);
    msg("database open failed: %s\n", "doxygen_sqlite3.db");
    exit(-1);
  }
  beginTransaction(db);
  pragmaTuning(db);

  initializeSchema(db);
  if ( -1 == prepareStatements(db) )
  {
    err("sqlite generator: prepareStatements failed!");
    return;
  }

  // + classes
  ClassSDict::Iterator cli(*Doxygen::classSDict);
  ClassDef *cd;
  for (cli.toFirst();(cd=cli.current());++cli)
  {
    msg("Generating Sqlite3 output for class %s\n",cd->name().data());
    generateSqlite3ForClass(db,cd);
  }

  // + namespaces
  NamespaceSDict::Iterator nli(*Doxygen::namespaceSDict);
  NamespaceDef *nd;
  for (nli.toFirst();(nd=nli.current());++nli)
  {
    msg("Generating Sqlite3 output for namespace %s\n",nd->name().data());
    generateSqlite3ForNamespace(db,nd);
  }

  // + files
  FileNameListIterator fnli(*Doxygen::inputNameList);
  FileName *fn;
  for (;(fn=fnli.current());++fnli)
  {
    FileNameIterator fni(*fn);
    FileDef *fd;
    for (;(fd=fni.current());++fni)
    {
      msg("Generating Sqlite3 output for file %s\n",fd->name().data());
      generateSqlite3ForFile(db,fd);
    }
  }
  endTransaction(db);
}

#else // USE_SQLITE3
void generateSqlite3()
{
  err("Error: sqlite3 support has not been compiled in!");
}
#endif

