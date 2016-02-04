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

#include "dot.h"
#include "arguments.h"
#include "classlist.h"
#include "filedef.h"
#include "namespacedef.h"
#include "filename.h"
#include "groupdef.h"
#include "pagedef.h"
#include "dirdef.h"

#include <qdir.h>
#include <string.h>
#include <sqlite3.h>

//#define DBG_CTX(x) printf x
#define DBG_CTX(x) do { } while(0)

const char * schema_queries[][2] = {
  { "includes",
    "CREATE TABLE IF NOT EXISTS includes (\n"
      "\t-- #include relations.\n"
      "\trowid        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
      "\tlocal        INTEGER NOT NULL,\n"
      "\tid_src       INTEGER NOT NULL,  -- File id of the includer.\n"
      "\tid_dst       INTEGER NOT NULL   -- File id of the includee.\n"
      ");\n"
    "CREATE UNIQUE INDEX idx_includes ON includes\n"
      "\t(local, id_src, id_dst);"
  },
  { "innerclass",
    "CREATE TABLE IF NOT EXISTS innerclass (\n"
      "\trowid        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
      "\trefid        TEXT NOT NULL,\n"
      "\tprot         INTEGER NOT NULL,\n"
      "\tname         TEXT NOT NULL\n"
      ");"
  },
  { "files",
    "CREATE TABLE IF NOT EXISTS files (\n"
      "\t-- Names of source files and includes.\n"
      "\tname         TEXT PRIMARY KEY NOT NULL\n"
      ");"
  },
  { "refids",
    "CREATE TABLE IF NOT EXISTS refids (\n"
      "\trefid        TEXT PRIMARY KEY NOT NULL\n"
      ");"
  },
  { "xrefs",
    "CREATE TABLE IF NOT EXISTS xrefs (\n"
      "\t-- Cross reference relation.\n"
      "\trowid        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
      "\trefid_src    INTEGER NOT NULL, -- referrer id.\n"
      "\trefid_dst    INTEGER NOT NULL, -- referee id.\n"
      "\tid_file      INTEGER NOT NULL, -- file where the reference is happening.\n"
      "\tline         INTEGER NOT NULL, -- line where the reference is happening.\n"
      "\tcolumn       INTEGER NOT NULL  -- column where the reference is happening.\n"
      ");\n"
    "CREATE UNIQUE INDEX idx_xrefs ON xrefs\n"
      "\t(refid_src, refid_dst, id_file, line, column);"
  },
  { "memberdef",
    "CREATE TABLE IF NOT EXISTS memberdef (\n"
      "\t-- All processed identifiers.\n"
      "\trowid        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
      "\tid_file      INTEGER NOT NULL,  -- file where this identifier is located\n"
      "\tline         INTEGER NOT NULL,  -- line where this identifier is located\n"
      "\tcolumn       INTEGER NOT NULL,  -- column where this identifier is located\n"
      "\trefid        TEXT NOT NULL,     -- see the refids table\n"
      "\tname         TEXT NOT NULL,\n"
      "\tdefinition   TEXT,\n"
      "\ttype         TEXT,\n"
      "\targsstring   TEXT,\n"
      "\tscope        TEXT,\n"
      "\tinitializer  TEXT,\n"
      "\tprot         INTEGER DEFAULT 0, -- 0:public 1:protected 2:private 3:package\n"
      "\tconst        INTEGER DEFAULT 0, -- 0:non-constant 1:constant\n"
      "\tvirt         INTEGER DEFAULT 0, -- 0:non-virtual 1:virtual 2:pure-virtual\n"
      "\tstatic       INTEGER DEFAULT 0, -- 0:non-static 1:static\n"
      "\texplicit     INTEGER DEFAULT 0,\n"
      "\tinline       INTEGER DEFAULT 0,\n"
      "\tfinal        INTEGER DEFAULT 0,\n"
      "\tsealed       INTEGER DEFAULT 0,\n"
      "\tnew          INTEGER DEFAULT 0,\n"
      "\toptional     INTEGER DEFAULT 0,\n"
      "\trequired     INTEGER DEFAULT 0,\n"
      "\tmutable      INTEGER DEFAULT 0,\n"
      "\tinitonly     INTEGER DEFAULT 0,\n"
      "\treadable     INTEGER DEFAULT 0,\n"
      "\twritable     INTEGER DEFAULT 0,\n"
      "\tgettable     INTEGER DEFAULT 0,\n"
      "\tsettable     INTEGER DEFAULT 0,\n"
      "\taccessor     INTEGER DEFAULT 0,\n"
      "\taddable      INTEGER DEFAULT 0,\n"
      "\tremovable    INTEGER DEFAULT 0,\n"
      "\traisable     INTEGER DEFAULT 0,\n"
      /// @todo make a `kind' table
      "\tkind         INTEGER DEFAULT 0, -- 0:define 1:function 2:variable 3:typedef 4:enum 5:enumvalue 6:signal 7:slot 8:friend 9:DCOP 10:property 11:event\n"
      "\tid_bodyfile  INTEGER DEFAULT 0, -- file of definition\n"
      "\tbodystart    INTEGER DEFAULT 0, -- starting line of definition\n"
      "\tbodyend      INTEGER DEFAULT 0, -- ending line of definition\n"
      /// @todo make a `detaileddescription' table
      "\tdetaileddescription  TEXT,\n"
      "\tbriefdescription     TEXT,\n"
      "\tinbodydescription    TEXTi\n"
      ");"
  },
  { "compounddef",
    "CREATE TABLE IF NOT EXISTS compounddef (\n"
      "\t-- class/struct definitions.\n"
      "\trowid        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
      "\tname         TEXT NOT NULL,\n"
      "\tkind         TEXT NOT NULL,\n"
      "\trefid        TEXT NOT NULL,\n"
      "\tprot         INTEGER NOT NULL,\n"
      "\tid_file      INTEGER NOT NULL,\n"
      "\tline         INTEGER NOT NULL,\n"
      "\tcolumn       INTEGER NOT NULL\n"
      ");"
  },
  { "basecompoundref",
    "CREATE TABLE IF NOT EXISTS basecompoundref (\n"
      "\trowid        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
      "\tbase         TEXT NOT NULL,\n"
      "\tderived      TEXT NOT NULL,\n"
      "\trefid        TEXT NOT NULL,\n"
      "\tprot         INTEGER NOT NULL,\n"
      "\tvirt         INTEGER NOT NULL\n"
      ");"
  },
  { "derivedcompoundref",
    "CREATE TABLE IF NOT EXISTS derivedcompoundref (\n"
      "\trowid        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
      "\tbase         TEXT NOT NULL,\n"
      "\tderived      TEXT NOT NULL,\n"
      "\trefid        TEXT NOT NULL,\n"
      "\tprot         INTEGER NOT NULL,\n"
      "\tvirt         INTEGER NOT NULL\n"
      ");"
  },
  { "params",
    "CREATE TABLE IF NOT EXISTS params (\n"
      "\t-- All processed parameters.\n"
      "\trowid        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
      "\tattributes   TEXT,\n"
      "\ttype         TEXT,\n"
      "\tdeclname     TEXT,\n"
      "\tdefnname     TEXT,\n"
      "\tarray        TEXT,\n"
      "\tdefval       TEXT,\n"
      "\tbriefdescription TEXT\n"
      ");"
  },
  { "memberdef_params",
    "CREATE TABLE IF NOT EXISTS memberdef_params (\n"
      "\t-- Junction table for memberdef parameters.\n"
      "\trowid        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
      "\tid_memberdef INTEGER NOT NULL,\n"
      "\tid_param     INTEGER NOT NULL\n"
      ");"
  },
  { "innernamespaces",
    "CREATE TABLE IF NOT EXISTS innernamespaces (\n"
      "\trowid        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
      "\trefid        TEXT NOT NULL,\n"
      "\tname         TEXT NOT NULL\n"
      ");"
  }
};

//////////////////////////////////////////////////////
struct SqlStmt {
  const char   *query;
  sqlite3_stmt *stmt;
};
//////////////////////////////////////////////////////
SqlStmt incl_insert = { "INSERT INTO includes "
  "( local, id_src, id_dst ) "
    "VALUES "
    "(:local,:id_src,:id_dst )"
    ,NULL
};
SqlStmt incl_select = { "SELECT COUNT(*) FROM includes WHERE "
  "local=:local AND id_src=:id_src AND id_dst=:id_dst"
    ,NULL
};
//////////////////////////////////////////////////////
SqlStmt innerclass_insert={"INSERT INTO innerclass "
    "( refid, prot, name )"
    "VALUES "
    "(:refid,:prot,:name )"
    ,NULL
};
//////////////////////////////////////////////////////
SqlStmt files_select = {"SELECT rowid FROM files WHERE name=:name"
  ,NULL
};
SqlStmt files_insert = {"INSERT INTO files "
  "( name )"
    "VALUES "
    "(:name )"
    ,NULL
};
//////////////////////////////////////////////////////
SqlStmt refids_select =  {"SELECT rowid FROM refids WHERE "
  "refid=:refid"
    ,NULL
};
SqlStmt refids_insert = {"INSERT INTO refids "
  "( refid )"
    "VALUES "
    "(:refid )"
    ,NULL
};
//////////////////////////////////////////////////////
SqlStmt xrefs_insert= {"INSERT INTO xrefs "
  "( refid_src, refid_dst, id_file, line, column )"
    "VALUES "
    "(:refid_src,:refid_dst,:id_file,:line,:column )"
    ,NULL
};
//////////////////////////////////////////////////////
SqlStmt memberdef_insert={"INSERT INTO memberdef "
    "( refid, prot, static, const, explicit, inline, final, sealed, new, optional, required, virt, mutable, initonly, readable, writable, gettable, settable, accessor, addable, removable, raisable, name, type, definition, argsstring, scope, initializer, kind, id_bodyfile, bodystart, bodyend, id_file, line, column, detaileddescription, briefdescription, inbodydescription)"
    "VALUES "
    "(:refid,:prot,:static,:const,:explicit,:inline,:final,:sealed,:new,:optional,:required,:virt,:mutable,:initonly,:readable,:writable,:gettable,:settable,:accessor,:addable,:removable,:raisable,:name,:type,:definition,:argsstring,:scope,:initializer,:kind,:id_bodyfile,:bodystart,:bodyend,:id_file,:line,:column,:detaileddescription,:briefdescription,:inbodydescription)"
    ,NULL
};
//////////////////////////////////////////////////////
SqlStmt compounddef_insert={"INSERT INTO compounddef "
    "( name, kind, prot, refid, id_file, line, column ) "
    "VALUES "
    "(:name,:kind,:prot,:refid,:id_file,:line,:column )"
    ,NULL
};
//////////////////////////////////////////////////////
SqlStmt basecompoundref_insert={"INSERT INTO  basecompoundref "
    "( base, derived, refid, prot, virt ) "
    "VALUES "
    "(:base,:derived,:refid,:prot,:virt )"
    ,NULL
};
//////////////////////////////////////////////////////
SqlStmt derivedcompoundref_insert={"INSERT INTO  derivedcompoundref "
    "( refid, prot, virt, base, derived ) "
    "VALUES "
    "(:refid,:prot,:virt,:base,:derived )"
    ,NULL
};
//////////////////////////////////////////////////////
SqlStmt params_select = { "SELECT rowid FROM  params WHERE "
    "(attributes IS NULL OR attributes=:attributes) AND "
    "(type IS NULL OR type=:type) AND "
    "(declname IS NULL OR declname=:declname) AND "
    "(defnname IS NULL OR defnname=:defnname) AND "
    "(array IS NULL OR array=:array) AND "
    "(defval IS NULL OR defval=:defval) AND "
    "(briefdescription IS NULL OR briefdescription=:briefdescription)"
    ,NULL
};
SqlStmt params_insert = { "INSERT INTO  params "
  "( attributes, type, declname, defnname, array, defval, briefdescription ) "
    "VALUES "
    "(:attributes,:type,:declname,:defnname,:array,:defval,:briefdescription)"
    ,NULL
};
//////////////////////////////////////////////////////
SqlStmt memberdef_params_insert={ "INSERT INTO  memberdef_params "
    "( id_memberdef, id_param)"
    "VALUES "
    "(:id_memberdef,:id_param)"
    ,NULL
};
//////////////////////////////////////////////////////
SqlStmt innernamespace_insert={"INSERT INTO  innernamespaces "
    "( refid, name)"
    "VALUES "
    "(:refid,:name)",
    NULL
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


static void bindTextParameter(SqlStmt &s,const char *name,const char *value, bool _static=TRUE)
{
  int idx = sqlite3_bind_parameter_index(s.stmt, name);
  sqlite3_bind_text(s.stmt, idx, value, -1, _static==TRUE?SQLITE_STATIC:SQLITE_TRANSIENT);
}

static void bindIntParameter(SqlStmt &s,const char *name,int value)
{
  int idx = sqlite3_bind_parameter_index(s.stmt, name);
  sqlite3_bind_int(s.stmt, idx, value);
}

static int step(sqlite3 *db, SqlStmt &s,bool getRowId=FALSE, bool select=FALSE)
{
  int rowid=-1;
  int rc = sqlite3_step(s.stmt);
  if (rc!=SQLITE_DONE && rc!=SQLITE_ROW)
  {
    msg("sqlite3_step failed: %s\n", sqlite3_errmsg(db));
    sqlite3_clear_bindings(s.stmt);
    return -1;
  }
  if (getRowId && select) rowid = sqlite3_column_int(s.stmt, 0); // works on selects, doesnt on inserts
  if (getRowId && !select) rowid = sqlite3_last_insert_rowid(db); //works on inserts, doesnt on selects
  sqlite3_reset(s.stmt);
  sqlite3_clear_bindings(s.stmt); // XXX When should this really be called
  return rowid;
}

static int insertFile(sqlite3 *db, const char* name)
{
  int rowid=-1;
  if (name==0) return rowid;

  bindTextParameter(files_select,":name",name);
  rowid=step(db,files_select,TRUE,TRUE);
  if (rowid==0)
  {
    bindTextParameter(files_insert,":name",name);
    rowid=step(db,files_insert,TRUE);
  }
  return rowid;
}

static int insertRefid(sqlite3 *db, const char *refid)
{
  int rowid=-1;
  if (refid==0) return rowid;

  bindTextParameter(refids_select,":refid",refid);
  rowid=step(db,refids_select,TRUE,TRUE);
  if (rowid==0)
  {
    bindTextParameter(refids_insert,":refid",refid);
    rowid=step(db,refids_insert,TRUE);
  }
  return rowid;
}


static void insertMemberReference(sqlite3 *db, const char*src, const char*dst, const char *file, int line, int column)
{
  int id_file = insertFile(db,file);
  int refid_src = insertRefid(db,src);
  int refid_dst = insertRefid(db,dst);
  if (id_file==-1||refid_src==-1||refid_dst==-1)
    return;

  bindIntParameter(xrefs_insert,":refid_src",refid_src);
  bindIntParameter(xrefs_insert,":refid_dst",refid_dst);
  bindIntParameter(xrefs_insert,":id_file",id_file);
  bindIntParameter(xrefs_insert,":line",line);
  bindIntParameter(xrefs_insert,":column",1);
  step(db,xrefs_insert);
}

static void insertMemberReference(sqlite3 *db, MemberDef *src, MemberDef *dst, const char*floc)
{
  if (dst->getStartBodyLine()!=-1 && dst->getBodyDef())
  {
    static char file[4096];
    int line=0,column=0;
    if (floc)
    {
      int rv = sscanf(floc,"%[^:]:%d:%d",file,&line,&column);
      if (rv!=3)
      {
        msg("unable to read file:line:col location from string [%s]\n",floc);
        return;
      }
    }
    insertMemberReference(db,src->anchor().data(),dst->anchor().data(),file,line,column);
  }
}

static void insertMemberFunctionParams(sqlite3 *db,int id_memberdef,MemberDef *md,Definition *def)
{
  ArgumentList *declAl = md->declArgumentList();
  ArgumentList *defAl = md->argumentList();
  if (declAl!=0 && declAl->count()>0)
  {
    ArgumentListIterator declAli(*declAl);
    ArgumentListIterator defAli(*defAl);
    Argument *a;
    for (declAli.toFirst();(a=declAli.current());++declAli)
    {
      Argument *defArg = defAli.current();

      if (!a->attrib.isEmpty())
      {
        bindTextParameter(params_select,":attributes",a->attrib.data());
        bindTextParameter(params_insert,":attributes",a->attrib.data());
      }
      if (!a->type.isEmpty())
      {
        StringList l;
        linkifyText(TextGeneratorSqlite3Impl(l),def,md->getBodyDef(),md,a->type);

        StringListIterator li(l);
        QCString *s;
        while ((s=li.current()))
        {
          insertMemberReference(db,md->anchor().data(),s->data(),def->getDefFileName().data(),md->getDefLine(),1);
          ++li;
        }
        bindTextParameter(params_select,":type",a->type.data());
        bindTextParameter(params_insert,":type",a->type.data());
      }
      if (!a->name.isEmpty())
      {
        bindTextParameter(params_select,":declname",a->name.data());
        bindTextParameter(params_insert,":declname",a->name.data());
      }
      if (defArg && !defArg->name.isEmpty() && defArg->name!=a->name)
      {
        bindTextParameter(params_select,":defnname",defArg->name.data());
        bindTextParameter(params_insert,":defnname",defArg->name.data());
      }
      if (!a->array.isEmpty())
      {
        bindTextParameter(params_select,":array",a->array.data());
        bindTextParameter(params_insert,":array",a->array.data());
      }
      if (!a->defval.isEmpty())
      {
        StringList l;
        linkifyText(TextGeneratorSqlite3Impl(l),def,md->getBodyDef(),md,a->defval);
        bindTextParameter(params_select,":defval",a->defval.data());
        bindTextParameter(params_insert,":defval",a->defval.data());
      }
      if (defArg) ++defAli;

      int id_param=step(db,params_select,TRUE,TRUE);
      if (id_param==0) {
        id_param=step(db,params_insert,TRUE);
      }

      bindIntParameter(memberdef_params_insert,":id_memberdef",id_memberdef);
      bindIntParameter(memberdef_params_insert,":id_param",id_param);
      step(db,memberdef_params_insert);
    }
  }
}

static void insertMemberDefineParams(sqlite3 *db,int id_memberdef,MemberDef *md,Definition *def)
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
        bindTextParameter(params_insert,":defnname",a->type.data());
        int id_param=step(db,params_insert,TRUE);

        bindIntParameter(memberdef_params_insert,":id_memberdef",id_memberdef);
        bindIntParameter(memberdef_params_insert,":id_param",id_param);
        step(db,memberdef_params_insert);
      }
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

static int prepareStatement(sqlite3 *db, SqlStmt &s)
{
  int rc;
  rc = sqlite3_prepare_v2(db,s.query,-1,&s.stmt,0);
  if (rc!=SQLITE_OK)
  {
    msg("prepare failed for %s\n%s\n", s.query, sqlite3_errmsg(db));
    return -1;
  }
  return rc;
}

static int prepareStatements(sqlite3 *db)
{
  if (
  -1==prepareStatement(db, memberdef_insert) ||
  -1==prepareStatement(db, files_insert) ||
  -1==prepareStatement(db, files_select) ||
  -1==prepareStatement(db, refids_insert) ||
  -1==prepareStatement(db, refids_select) ||
  -1==prepareStatement(db, incl_insert)||
  -1==prepareStatement(db, incl_select)||
  -1==prepareStatement(db, params_insert) ||
  -1==prepareStatement(db, params_select) ||
  -1==prepareStatement(db, xrefs_insert) ||
  -1==prepareStatement(db, innerclass_insert) ||
  -1==prepareStatement(db, compounddef_insert) ||
  -1==prepareStatement(db, basecompoundref_insert) ||
  -1==prepareStatement(db, derivedcompoundref_insert) ||
  -1==prepareStatement(db, memberdef_params_insert)||
  -1==prepareStatement(db, innernamespace_insert)
  )
  {
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
  sqlite3_exec(db, "PRAGMA temp_store = MEMORY;", NULL, NULL, &sErrMsg);
}

static int initializeSchema(sqlite3* db)
{
  int rc;
  sqlite3_stmt *stmt = 0;

  msg("Initializing DB schema...\n");
  for (unsigned int k = 0; k < sizeof(schema_queries) / sizeof(schema_queries[0]); k++)
  {
    const char *q = schema_queries[k][1];
    // create table
    rc = sqlite3_prepare_v2(db, q, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
      msg("failed to prepare query: %s\n\t%s\n", q, sqlite3_errmsg(db));
      return -1;
    }
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
      msg("failed to execute query: %s\n\t%s\n", q, sqlite3_errmsg(db));
      return -1;
    }
    sqlite3_finalize(stmt);
  }
  return 0;
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
      bindTextParameter(innerclass_insert,":refid",cd->getOutputFileBase(),FALSE);
      bindIntParameter(innerclass_insert,":prot",cd->protection());
      bindTextParameter(innerclass_insert,":name",cd->name());
      step(db,innerclass_insert);
    }
  }
}


static void writeInnerNamespaces(sqlite3 *db,const NamespaceSDict *nl)
{
  if (nl)
  {
    NamespaceSDict::Iterator nli(*nl);
    NamespaceDef *nd;
    for (nli.toFirst();(nd=nli.current());++nli)
    {
      if (!nd->isHidden() && nd->name().find('@')==-1) // skip anonymouse scopes
      {
        bindTextParameter(innernamespace_insert,":refid",nd->getOutputFileBase(),FALSE);
        bindTextParameter(innernamespace_insert,":name",nd->name(),FALSE);
        step(db,innernamespace_insert);
      }
    }
  }
}


static void writeTemplateArgumentList(sqlite3* db,
                                      ArgumentList * al,
                                      Definition * scope,
                                      FileDef * fileScope)
{
  if (al)
  {
    ArgumentListIterator ali(*al);
    Argument *a;
    for (ali.toFirst();(a=ali.current());++ali)
    {
      if (!a->type.isEmpty())
      {
        #warning linkifyText(TextGeneratorXMLImpl(t),scope,fileScope,0,a->type);
        bindTextParameter(params_select,":type",a->type);
        bindTextParameter(params_insert,":type",a->type);
      }
      if (!a->name.isEmpty())
      {
        bindTextParameter(params_select,":declname",a->name);
        bindTextParameter(params_insert,":declname",a->name);
        bindTextParameter(params_select,":defnname",a->name);
        bindTextParameter(params_insert,":defnname",a->name);
      }
      if (!a->defval.isEmpty())
      {
        #warning linkifyText(TextGeneratorXMLImpl(t),scope,fileScope,0,a->defval);
        bindTextParameter(params_select,":defval",a->defval);
        bindTextParameter(params_insert,":defval",a->defval);
      }
      if (!step(db,params_select,TRUE,TRUE))
        step(db,params_insert);
    }
  }
}

static void writeMemberTemplateLists(sqlite3* db,MemberDef *md)
{
  ArgumentList *templMd = md->templateArguments();
  if (templMd) // function template prefix
  {
    writeTemplateArgumentList(db,templMd,md->getClassDef(),md->getFileDef());
  }
}
static void writeTemplateList(sqlite3*db,ClassDef *cd)
{
  writeTemplateArgumentList(db,cd->templateArguments(),cd,0);
}
////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
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
  // memberdef
  bindTextParameter(memberdef_insert,":refid",md->anchor().data(),FALSE);
  bindIntParameter(memberdef_insert,":kind",md->memberType());
  bindIntParameter(memberdef_insert,":prot",md->protection());

  bindIntParameter(memberdef_insert,":static",md->isStatic());

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
    ArgumentList *al = md->argumentList();
    if (al!=0)
    {
      bindIntParameter(memberdef_insert,":const",al->constSpecifier);
    }
    bindIntParameter(memberdef_insert,":explicit",md->isExplicit());
    bindIntParameter(memberdef_insert,":inline",md->isInline());
    bindIntParameter(memberdef_insert,":final",md->isFinal());
    bindIntParameter(memberdef_insert,":sealed",md->isSealed());
    bindIntParameter(memberdef_insert,":new",md->isNew());
    bindIntParameter(memberdef_insert,":optional",md->isOptional());
    bindIntParameter(memberdef_insert,":required",md->isRequired());

    bindIntParameter(memberdef_insert,":virt",md->virtualness());
  }
  // place in the arguments and linkify the arguments

  if (md->memberType() == MemberType_Variable)
  {
    bindIntParameter(memberdef_insert,":mutable",md->isMutable());
    bindIntParameter(memberdef_insert,":initonly",md->isInitonly());
  }
  else if (md->memberType() == MemberType_Property)
  {
    bindIntParameter(memberdef_insert,":readable",md->isReadable());
    bindIntParameter(memberdef_insert,":writable",md->isWritable());
    bindIntParameter(memberdef_insert,":gettable",md->isGettable());
    bindIntParameter(memberdef_insert,":settable",md->isSettable());
    if (md->isAssign() || md->isCopy() || md->isRetain())
    {
      int accessor = md->isAssign() ? md->isAssign() :
          (md->isCopy() ? md->isCopy() : md->isRetain()) ;

      bindIntParameter(memberdef_insert,":accessor",accessor);
    }
  }
  else if (md->memberType() == MemberType_Event)
  {
    bindIntParameter(memberdef_insert,":addable",md->isAddable());
    bindIntParameter(memberdef_insert,":removable",md->isRemovable());
    bindIntParameter(memberdef_insert,":raisable",md->isRaisable());
  }

  // + declaration/definition arg lists
  if (md->memberType()!=MemberType_Define &&
      md->memberType()!=MemberType_Enumeration
     )
  {
    if (md->memberType()!=MemberType_Typedef)
    {
      writeMemberTemplateLists(db,md);
    }
    QCString typeStr = md->typeString();
    stripQualifiers(typeStr);
    StringList l;
    linkifyText(TextGeneratorSqlite3Impl(l),def,md->getBodyDef(),md,typeStr);
    if (typeStr.data())
    {
      bindTextParameter(memberdef_insert,":type",typeStr.data(),FALSE);
    }

    if (md->definition())
    {
      bindTextParameter(memberdef_insert,":definition",md->definition());
    }

    if (md->argsString())
    {
      bindTextParameter(memberdef_insert,":argsstring",md->argsString());
    }
  }

  bindTextParameter(memberdef_insert,":name",md->name());

  if (md->memberType() == MemberType_Property)
  {
    if (md->isReadable())
    {
      bindIntParameter(memberdef_insert,":readable",1);
    }
    if (md->isWritable())
    {
      bindIntParameter(memberdef_insert,":writable",1);
    }
  }


  // Extract references from initializer
  if (md->hasMultiLineInitializer() || md->hasOneLineInitializer())
  {
    bindTextParameter(memberdef_insert,":initializer",md->initializer().data());

    StringList l;
    linkifyText(TextGeneratorSqlite3Impl(l),def,md->getBodyDef(),md,md->initializer());
    StringListIterator li(l);
    QCString *s;
    while ((s=li.current()))
    {
      if (md->getBodyDef())
      {
        DBG_CTX(("initializer:%s %s %s %d\n",
              md->anchor().data(),
              s->data(),
              md->getBodyDef()->getDefFileName().data(),
              md->getStartBodyLine()));
        insertMemberReference(db,md->anchor().data(),s->data(),md->getBodyDef()->getDefFileName().data(),md->getStartBodyLine(),1);
      }
      ++li;
    }
  }

  if ( md->getScopeString() )
  {
    bindTextParameter(memberdef_insert,":scope",md->getScopeString().data(),FALSE);
  }

  // +Brief, detailed and inbody description
  bindTextParameter(memberdef_insert,":briefdescription",md->briefDescription(),FALSE);
  bindTextParameter(memberdef_insert,":detaileddescription",md->documentation(),FALSE);
  bindTextParameter(memberdef_insert,":inbodydescription",md->inbodyDocumentation(),FALSE);

  // File location
  if (md->getDefLine() != -1)
  {
    int id_file = insertFile(db,md->getDefFileName());
    if (id_file!=-1)
    {
      bindIntParameter(memberdef_insert,":id_file",id_file);
      bindIntParameter(memberdef_insert,":line",md->getDefLine());
      bindIntParameter(memberdef_insert,":column",md->getDefColumn());

      if (md->getStartBodyLine()!=-1)
      {
        int id_bodyfile = insertFile(db,md->getBodyDef()->absFilePath());
        if (id_bodyfile == -1)
        {
            sqlite3_clear_bindings(memberdef_insert.stmt);
        }
        else
        {
            bindIntParameter(memberdef_insert,":id_bodyfile",id_bodyfile);
            bindIntParameter(memberdef_insert,":bodystart",md->getStartBodyLine());
            bindIntParameter(memberdef_insert,":bodyend",md->getEndBodyLine());
        }
      }
    }
  }

  int id_memberdef=step(db,memberdef_insert,TRUE);

  if (isFunc)
  {
    insertMemberFunctionParams(db,id_memberdef,md,def);
  }
  else if (md->memberType()==MemberType_Define &&
          md->argsString())
  {
    insertMemberDefineParams(db,id_memberdef,md,def);
  }

  // + source references
  // The cross-references in initializers only work when both the src and dst
  // are defined.
  MemberSDict *mdict = md->getReferencesMembers();
  if (mdict!=0)
  {
    MemberSDict::IteratorDict mdi(*mdict);
    MemberDef *rmd;
    for (mdi.toFirst();(rmd=mdi.current());++mdi)
    {
      insertMemberReference(db,md,rmd,mdi.currentKey());
    }
  }
  // + source referenced by
  mdict = md->getReferencedByMembers();
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
  for (mli.toFirst();(md=mli.current());++mli)
  {
    // namespace members are also inserted in the file scope, but
    // to prevent this duplication in the XML output, we filter those here.
    //if (d->definitionType()!=Definition::TypeFile || md->getNamespaceDef()==0)
    {
      generateSqlite3ForMember(db,md,d);
    }
  }
}


static void generateSqlite3ForClass(sqlite3 *db, ClassDef *cd)
{
  // + list of direct super classes
  // + list of direct sub classes
  // + include file
  // + list of inner classes
  // - template argument list(s)
  // + member groups
  // + list of all members
  // - brief description
  // - detailed description
  // - inheritance DOT diagram
  // - collaboration DOT diagram
  // - user defined member sections
  // - standard member sections
  // - detailed member documentation
  // - examples using the class

  if (cd->isReference())        return; // skip external references.
  if (cd->isHidden())           return; // skip hidden classes.
  if (cd->name().find('@')!=-1) return; // skip anonymous compounds.
  if (cd->templateMaster()!=0)  return; // skip generated template instances.

  msg("Generating Sqlite3 output for class %s\n",cd->name().data());

  bindTextParameter(compounddef_insert,":name",cd->name());
  bindTextParameter(compounddef_insert,":kind",cd->compoundTypeString(),FALSE);
  bindIntParameter(compounddef_insert,":prot",cd->protection());
  bindTextParameter(compounddef_insert,":refid",cd->getOutputFileBase(),FALSE);

  int id_file = insertFile(db,cd->getDefFileName().data());
  bindIntParameter(compounddef_insert,":id_file",id_file);
  bindIntParameter(compounddef_insert,":line",cd->getDefLine());
  bindIntParameter(compounddef_insert,":column",cd->getDefColumn());

  step(db,compounddef_insert);

  // + list of direct super classes
  if (cd->baseClasses())
  {
    BaseClassListIterator bcli(*cd->baseClasses());
    BaseClassDef *bcd;
    for (bcli.toFirst();(bcd=bcli.current());++bcli)
    {
      bindTextParameter(basecompoundref_insert,":refid",bcd->classDef->getOutputFileBase(),FALSE);
      bindIntParameter(basecompoundref_insert,":prot",bcd->prot);
      bindIntParameter(basecompoundref_insert,":virt",bcd->virt);

      if (!bcd->templSpecifiers.isEmpty())
      {
        bindTextParameter(basecompoundref_insert,":base",insertTemplateSpecifierInScope(bcd->classDef->name(),bcd->templSpecifiers),FALSE);
      }
      else
      {
        bindTextParameter(basecompoundref_insert,":base",bcd->classDef->displayName(),FALSE);
      }
      bindTextParameter(basecompoundref_insert,":derived",cd->displayName(),FALSE);
      step(db,basecompoundref_insert);
    }
  }

  // + list of direct sub classes
  if (cd->subClasses())
  {
    BaseClassListIterator bcli(*cd->subClasses());
    BaseClassDef *bcd;
    for (bcli.toFirst();(bcd=bcli.current());++bcli)
    {
      bindTextParameter(derivedcompoundref_insert,":base",cd->displayName(),FALSE);
      if (!bcd->templSpecifiers.isEmpty())
      {
        bindTextParameter(derivedcompoundref_insert,":derived",insertTemplateSpecifierInScope(bcd->classDef->name(),bcd->templSpecifiers),FALSE);
      }
      else
      {
        bindTextParameter(derivedcompoundref_insert,":derived",bcd->classDef->displayName(),FALSE);
      }
      bindTextParameter(derivedcompoundref_insert,":refid",bcd->classDef->getOutputFileBase(),FALSE);
      bindIntParameter(derivedcompoundref_insert,":prot",bcd->prot);
      bindIntParameter(derivedcompoundref_insert,":virt",bcd->virt);
      step(db,derivedcompoundref_insert);
    }
  }

  // + include file
  IncludeInfo *ii=cd->includeInfo();
  if (ii)
  {
    QCString nm = ii->includeName;
    if (nm.isEmpty() && ii->fileDef) nm = ii->fileDef->docName();
    if (!nm.isEmpty())
    {
      int id_dst=insertFile(db,nm);
      if (id_dst!=-1) {
        bindIntParameter(incl_select,":local",ii->local);
        bindIntParameter(incl_select,":id_src",id_file);
        bindIntParameter(incl_select,":id_dst",id_dst);
        int count=step(db,incl_select,TRUE,TRUE);
        if (count==0)
        {
          bindIntParameter(incl_insert,":local",ii->local);
          bindIntParameter(incl_insert,":id_src",id_file);
          bindIntParameter(incl_insert,":id_dst",id_dst);
          step(db,incl_insert);
        }
      }
    }
  }
  // + list of inner classes
  writeInnerClasses(db,cd->getClassSDict());

  // - template argument list(s)
  writeTemplateList(db,cd);

  // + member groups
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

  // + list of all members
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

static void generateSqlite3ForNamespace(sqlite3 *db, NamespaceDef *nd)
{
  // + contained class definitions
  // + contained namespace definitions
  // + member groups
  // + normal members
  // - brief desc
  // - detailed desc
  // - location
  // - files containing (parts of) the namespace definition

  if (nd->isReference() || nd->isHidden()) return; // skip external references

  // + contained class definitions
  writeInnerClasses(db,nd->getClassSDict());

  // + contained namespace definitions
  writeInnerNamespaces(db,nd->getNamespaceSDict());

  // + member groups
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

  // + normal members
  QListIterator<MemberList> mli(nd->getMemberLists());
  MemberList *ml;
  for (mli.toFirst();(ml=mli.current());++mli)
  {
    if ((ml->listType()&MemberListType_declarationLists)!=0)
    {
      generateSqlite3Section(db,nd,ml,"user-defined");//g_xmlSectionMapper.find(ml->listType()));
    }
  }
}

static void generateSqlite3ForFile(sqlite3 *db, FileDef *fd)
{
  // + includes files
  // + includedby files
  // - include graph
  // - included by graph
  // + contained class definitions
  // + contained namespace definitions
  // + member groups
  // + normal members
  // - brief desc
  // - detailed desc
  // - source code
  // - location
  // - number of lines

  if (fd->isReference()) return; // skip external references

  // + includes files
  IncludeInfo *ii;
  if (fd->includeFileList())
  {
    QListIterator<IncludeInfo> ili(*fd->includeFileList());
    for (ili.toFirst();(ii=ili.current());++ili)
    {
      int id_src=insertFile(db,fd->absFilePath().data());
      int id_dst=insertFile(db,ii->includeName.data());
      bindIntParameter(incl_select,":local",ii->local);
      bindIntParameter(incl_select,":id_src",id_src);
      bindIntParameter(incl_select,":id_dst",id_dst);
      if (step(db,incl_select,TRUE,TRUE)==0) {
        bindIntParameter(incl_insert,":local",ii->local);
        bindIntParameter(incl_insert,":id_src",id_src);
        bindIntParameter(incl_insert,":id_dst",id_dst);
        step(db,incl_insert);
      }
    }
  }

  // + includedby files
  if (fd->includedByFileList())
  {
    QListIterator<IncludeInfo> ili(*fd->includedByFileList());
    for (ili.toFirst();(ii=ili.current());++ili)
    {
      int id_src=insertFile(db,ii->includeName);
      int id_dst=insertFile(db,fd->absFilePath());
      bindIntParameter(incl_select,":local",ii->local);
      bindIntParameter(incl_select,":id_src",id_src);
      bindIntParameter(incl_select,":id_dst",id_dst);
      if (step(db,incl_select,TRUE,TRUE)==0) {
        bindIntParameter(incl_insert,":local",ii->local);
        bindIntParameter(incl_insert,":id_src",id_src);
        bindIntParameter(incl_insert,":id_dst",id_dst);
        step(db,incl_insert);
      }
    }
  }

  // + contained class definitions
  if (fd->getClassSDict())
  {
    writeInnerClasses(db,fd->getClassSDict());
  }

  // + contained namespace definitions
  if (fd->getNamespaceSDict())
  {
    writeInnerNamespaces(db,fd->getNamespaceSDict());
  }

  // + member groups
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

  // + normal members
  QListIterator<MemberList> mli(fd->getMemberLists());
  MemberList *ml;
  for (mli.toFirst();(ml=mli.current());++mli)
  {
    if ((ml->listType()&MemberListType_declarationLists)!=0)
    {
      generateSqlite3Section(db,fd,ml,"user-defined");//g_xmlSectionMapper.find(ml->listType()));
    }
  }
}

static void generateSqlite3ForGroup(sqlite3*db,GroupDef *gd)
{
#warning WorkInProgress
}

static void generateSqlite3ForDir(sqlite3 *db,DirDef *dd)
{
#warning WorkInProgress
}

static void generateSqlite3ForPage(sqlite3 *db,PageDef *pd,bool isExample)
{
#warning WorkInProgress
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void generateSqlite3()
{
  // + classes
  // + namespaces
  // + files
  // + groups
  // + related pages
  // + examples
  // + main page

  QCString outputDirectory = Config_getString(OUTPUT_DIRECTORY);
  QDir sqlite3Dir(outputDirectory);
  sqlite3 *db;
  sqlite3_initialize();
  int rc = sqlite3_open_v2(outputDirectory+"/doxygen_sqlite3.db", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);
  if (rc != SQLITE_OK)
  {
    sqlite3_close(db);
    msg("database open failed: %s\n", "doxygen_sqlite3.db");
    return;
  }
  beginTransaction(db);
  pragmaTuning(db);

  if (-1==initializeSchema(db))
    return;

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

  // + groups
  GroupSDict::Iterator gli(*Doxygen::groupSDict);
  GroupDef *gd;
  for (;(gd=gli.current());++gli)
  {
    msg("Generating Sqlite3 output for group %s\n",gd->name().data());
    generateSqlite3ForGroup(db,gd);
  }

  // + page
  {
    PageSDict::Iterator pdi(*Doxygen::pageSDict);
    PageDef *pd=0;
    for (pdi.toFirst();(pd=pdi.current());++pdi)
    {
      msg("Generating Sqlite3 output for page %s\n",pd->name().data());
      generateSqlite3ForPage(db,pd,FALSE);
    }
  }

  // + dirs
  {
    DirDef *dir;
    DirSDict::Iterator sdi(*Doxygen::directories);
    for (sdi.toFirst();(dir=sdi.current());++sdi)
    {
      msg("Generating Sqlite3 output for dir %s\n",dir->name().data());
      generateSqlite3ForDir(db,dir);
    }
  }

  // + examples
  {
    PageSDict::Iterator pdi(*Doxygen::exampleSDict);
    PageDef *pd=0;
    for (pdi.toFirst();(pd=pdi.current());++pdi)
    {
      msg("Generating Sqlite3 output for example %s\n",pd->name().data());
      generateSqlite3ForPage(db,pd,TRUE);
    }
  }

  // + main page
  if (Doxygen::mainPage)
  {
    msg("Generating Sqlite3 output for the main page\n");
    generateSqlite3ForPage(db,Doxygen::mainPage,FALSE);
  }

  endTransaction(db);
}

#else // USE_SQLITE3
void generateSqlite3()
{
  err("sqlite3 support has not been compiled in!");
}
#endif
// vim: noai:ts=2:sw=2:ss=2:expandtab
