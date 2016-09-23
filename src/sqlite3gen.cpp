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
#include <stdlib.h>
#include "message.h"
#include "settings.h"

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

static void stripQualifiers(QCString &typeStr)
{
  bool done = FALSE;
  while (!done)
  {
    if (typeStr.stripPrefix("static "))
      ;
    else if (typeStr.stripPrefix("virtual "))
      ;
    else if (typeStr.stripPrefix("volatile "))
      ;
    else if (typeStr == "virtual")
      typeStr = "";
    else
      done = TRUE;
  }
}

//////////////////////////////////////////////////////

class TextGeneratorSqlite3Impl : public TextGeneratorIntf
{
  public:
  TextGeneratorSqlite3Impl(StringList &l) : l(l)
  {
    l.setAutoDelete(TRUE);
  }
  void writeString(const char * /*s*/, bool /*keepSpaces*/) const
  {
  }
  void writeBreak(int) const
  {
    DBG_CTX(("writeBreak\n"));
  }
  void writeLink(const char * /*extRef*/, const char *file, const char *anchor,
                 const char * /*text*/
                 ) const
  {
    QCString *rs = new QCString(file);
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

//////////////////////////////////////////////////////

static const char * schema_queries[][2] = {
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
      "\trefid        INTEGER NOT NULL,\n"
      "\tprot         INTEGER NOT NULL,\n"
      "\tname         TEXT NOT NULL\n"
      ");"
  },
  { "files",
    "CREATE TABLE IF NOT EXISTS files (\n"
      "\t-- Names of source files and includes.\n"
      "\tname                 TEXT PRIMARY KEY NOT NULL\n"
      ");"
  },
  { "dirs",
    "CREATE TABLE IF NOT EXISTS dirs (\n"
      "\t-- Names of directories.\n"
      "\trowid                INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
      "\trefid                INTEGER NOT NULL, -- see the refids table\n"
      "\tname                 TEXT NOT NULL,\n"
      "\tlocation             TEXT NOT NULL,\n"
      "\tbriefdescription     TEXT,\n"
      "\tdetaileddescription  TEXT\n"
      ");"
  },
  { "subdirs",
    "CREATE TABLE IF NOT EXISTS subdirs (\n"
      "\t-- Junction table for directory->subdirectory.\n"
      "\trowid        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
      "\tid_dir       INTEGER NOT NULL,\n"
      "\tid_sub       INTEGER NOT NULL\n"
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
      "\tid_file      INTEGER, -- file where the reference is done.\n"
      "\tline         INTEGER, -- line where the reference is done.\n"
      "\tcolumn       INTEGER  -- column where the reference is done.\n"
      ");\n"
    "CREATE UNIQUE INDEX idx_xrefs ON xrefs\n"
      "\t(refid_src, refid_dst, id_file, line, column);"
  },
  { "memberdef",
    "CREATE TABLE IF NOT EXISTS memberdef (\n"
      "\t-- All processed identifiers.\n"
      "\trowid                INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
      "\trefid                INTEGER NOT NULL,  -- see the refids table\n"
      "\tname                 TEXT NOT NULL,\n"
      "\tdefinition           TEXT,\n"
      "\ttype                 TEXT,\n"
      "\targsstring           TEXT,\n"
      "\tscope                TEXT,\n"
      "\tinitializer          TEXT,\n"
      "\tbitfield             TEXT,\n"
      "\tread                 TEXT,\n"
      "\twrite                TEXT,\n"
      "\texceptions           TEXT,\n"
      "\tenumvalue            TEXT,\n"
      "\tprot                 INTEGER DEFAULT 0, -- 0:public 1:protected 2:private 3:package\n"
      "\tstatic               INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\tconst                INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\texplicit             INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\tinline               INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\tfinal                INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\tsealed               INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\tnew                  INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\toptional             INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\trequired             INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\tvolatile             INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\tvirt                 INTEGER DEFAULT 0, -- 0:no 1:virtual 2:pure-virtual\n"
      "\tmutable              INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\tinitonly             INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\tattribute            INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\tproperty             INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\treadonly             INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\tbound                INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\tconstrained          INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\ttransient            INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\tmaybevoid            INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\tmaybedefault         INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\tmaybeambiguous       INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\treadable             INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\twritable             INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\tgettable             INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\tprivategettable      INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\tprotectedgettable    INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\tsettable             INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\tprivatesettable      INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\tprotectedsettable    INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\taccessor             INTEGER DEFAULT 0, -- 0:no 1:assign 2:copy 3:retain 4:string 5:weak\n"
      "\taddable              INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\tremovable            INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\traisable             INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      /// @todo make a `kind' table
      "\tkind                 INTEGER DEFAULT 0, -- 0:define 1:function 2:variable 3:typedef 4:enum 5:enumvalue 6:signal 7:slot 8:friend 9:DCOP 10:property 11:event\n"
      "\tbodystart            INTEGER DEFAULT 0, -- starting line of definition\n"
      "\tbodyend              INTEGER DEFAULT 0, -- ending line of definition\n"
      "\tid_bodyfile          INTEGER DEFAULT 0, -- file of definition\n"
      "\tid_file              INTEGER NOT NULL,  -- file where this identifier is located\n"
      "\tline                 INTEGER NOT NULL,  -- line where this identifier is located\n"
      "\tcolumn               INTEGER NOT NULL,  -- column where this identifier is located\n"
      /// @todo make a `detaileddescription' table
      "\tbriefdescription     TEXT,\n"
      "\tdetaileddescription  TEXT,\n"
      "\tinbodydescription    TEXT\n"
      ");"
  },
  { "compounddef",
    "CREATE TABLE IF NOT EXISTS compounddef (\n"
      "\t-- class/struct definitions.\n"
      "\trowid        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
      "\tname         TEXT NOT NULL,\n"
      "\tkind         TEXT NOT NULL,\n"
      "\trefid        INTEGER NOT NULL,\n"
      "\tprot         INTEGER NOT NULL,\n"
      "\tid_file      INTEGER NOT NULL,\n"
      "\tline         INTEGER NOT NULL,\n"
      "\tcolumn       INTEGER NOT NULL,\n"
      "\tbriefdescription     TEXT,\n"
      "\tdetaileddescription  TEXT\n"
      ");"
  },
  { "basecompoundref",
    "CREATE TABLE IF NOT EXISTS basecompoundref (\n"
      "\trowid        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
      "\tbase         TEXT NOT NULL,\n"
      "\tderived      TEXT NOT NULL,\n"
      "\trefid        INTEGER NOT NULL,\n"
      "\tprot         INTEGER NOT NULL,\n"
      "\tvirt         INTEGER NOT NULL\n"
      ");"
  },
  { "derivedcompoundref",
    "CREATE TABLE IF NOT EXISTS derivedcompoundref (\n"
      "\trowid        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
      "\tbase         TEXT NOT NULL,\n"
      "\tderived      TEXT NOT NULL,\n"
      "\trefid        INTEGER NOT NULL,\n"
      "\tprot         INTEGER NOT NULL,\n"
      "\tvirt         INTEGER NOT NULL\n"
      ");"
  },
  { "params",
    "CREATE TABLE IF NOT EXISTS params (\n"
      "\t-- All processed parameters.\n"
      "\trowid        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
      "\ttype         TEXT,\n"
      "\tdeclname     TEXT,\n"
      "\tdefname      TEXT,\n"
      "\tdefval       TEXT,\n"
      "\tarray        TEXT,\n"
      "\tattributes   TEXT,\n"
      "\tbriefdescription TEXT\n"
      ");"
  },
  { "memberdef_params",
    "CREATE TABLE IF NOT EXISTS memberdef_params (\n"
      "\t-- Junction table for memberdef->parameters.\n"
      "\trowid        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
      "\tid_memberdef INTEGER NOT NULL,\n"
      "\tid_param     INTEGER NOT NULL\n"
      ");"
  },
  { "compounddef_members",
    "CREATE TABLE IF NOT EXISTS compounddef_members (\n"
      "\t-- Junction table for compounddef->members.\n"
      "\trowid          INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
      "\tid_compounddef INTEGER NOT NULL,\n"
      "\tid_memberdef   INTEGER NOT NULL\n"
      ");"
  },
  { "innernamespaces",
    "CREATE TABLE IF NOT EXISTS innernamespaces (\n"
      "\trowid        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
      "\trefid        INTEGER NOT NULL,\n"
      "\tname         TEXT NOT NULL\n"
      ");"
  }
};

//////////////////////////////////////////////////////

class Sqlite3Stmt {
public:
  sqlite3_stmt *stmt;
  const char   *query;
public:
  Sqlite3Stmt(const char*q)
    : query(q)
  {
  }
public:
  void bindTextParameter(const char *name, const char *value,
                                bool _static = TRUE);
  void bindIntParameter(const char *name, int value);
};

//////////////////////////////////////////////////////

void Sqlite3Stmt::bindTextParameter(const char *name, const char *value,
                              bool _static/*= TRUE*/)
{
  int idx = sqlite3_bind_parameter_index(this->stmt, name);
  sqlite3_bind_text(this->stmt, idx, value, -1,
                    _static == TRUE ? SQLITE_STATIC : SQLITE_TRANSIENT);
}

void Sqlite3Stmt::bindIntParameter(const char *name, int value)
{
  int idx = sqlite3_bind_parameter_index(this->stmt, name);
  sqlite3_bind_int(this->stmt, idx, value);
}

//////////////////////////////////////////////////////

class Sqlite3Gen
{
public:
  bool Open(const char* f);
  bool Close();
  void BeginTransaction();
  void EndTransaction();
  void PragmaTuning();
  bool InitializeSchema();
  bool PrepareStatements();
  void InsertClasses(ClassSDict& csd);
  void InsertNamespaces(NamespaceSDict& nsd);
  void InsertFiles(FileNameList& fnl);
  void InsertGroups(GroupSDict& gsd);
  void InsertPages(PageSDict& psd);
  void InsertDirs(DirSDict& dsd);
  void InsertExamples(PageSDict& esd);
  void InsertPageDef(PageDef *pd, bool isExample);

private:
  int prepareStatement(Sqlite3Stmt &s);
  int step(Sqlite3Stmt &s, bool getRowId = FALSE,
                  bool select = FALSE);
  //////////////////////////////////////////////////////////////////////////////
  void bindMemberVariable(MemberDef *md);
  void bindMemberProperty(MemberDef *md);
  void bindMemberFunc(MemberDef *md);
  void bindMemberEvent(MemberDef *md);
  void bindMemberDescriptions(MemberDef *md);
  void bindMemberFileLocation(MemberDef *md);
  void bindMemberEnumeration(MemberDef *md);
  //////////////////////////////////////////////////////////////////////////////
  int insertFile(const char *name);
  int insertRefid(const char *refid);
  void insertDirDef(DirDef* dd);
  void insertSubDirs(const DirList *dl, int id_dir);
  void insertXref(int refid_src, int refid_dst,
                                    int id_file, int line, int column);
  int insertParam(Argument *a, Argument* defArg=NULL);
  void insertMemberReference(MemberDef *src, MemberDef *dst);
  void insertXrefsFromText(Definition *scope,
                                  FileDef *fileScope,
                                  Definition *self,
                                  const char *text);
  void insertMemberDefParams(Definition *scope,
                                        MemberDef *md,
                                        int id_memberdef);
  void insertMemberDefParams(MemberDef *md,
                                      int id_memberdef);
  int insertMemberDef(Definition *scope, MemberDef *md);
  void insertScopeMembers(Definition *scope,
                                 MemberList *ml,
                                 Sqlite3Stmt &sqlstmt,
                                 const char *scope_name,
                                 int id_scope);
  void insertClassDef(ClassDef *cd);
  void insertNamespaceDef(NamespaceDef *nd);
  void insertFileDef(FileDef *fd);
  void insertGroupDef(GroupDef *gd);
  //////////////////////////////////////////////////////////////////////////////
  void writeInnerClasses(const ClassSDict *cl);
  void writeInnerNamespaces(const NamespaceSDict *nl);
  void writeTemplateArgumentList(Definition *scope,
                                        FileDef *fileScope,
                                        ArgumentList *al);
  void writeMemberTemplateLists(MemberDef *md);
  void writeTemplateList(ClassDef *cd);

private:
  sqlite3 *m_db;
  Sqlite3Stmt incl_insert;
  Sqlite3Stmt incl_select;
  Sqlite3Stmt innerclass_insert;
  Sqlite3Stmt files_select;
  Sqlite3Stmt files_insert;
  Sqlite3Stmt dirs_select;
  Sqlite3Stmt dirs_insert;
  Sqlite3Stmt refids_select;
  Sqlite3Stmt refids_insert;
  Sqlite3Stmt xrefs_insert;
  Sqlite3Stmt memberdef_insert;
  Sqlite3Stmt compounddef_insert;
  Sqlite3Stmt basecompoundref_insert;
  Sqlite3Stmt derivedcompoundref_insert;
  Sqlite3Stmt params_select;
  Sqlite3Stmt params_insert;
  Sqlite3Stmt memberdef_params_insert;
  Sqlite3Stmt compounddef_members_insert;
  Sqlite3Stmt innernamespace_insert;
public:
  Sqlite3Gen() :
    incl_insert (
      "INSERT INTO includes "
      "( local, id_src, id_dst )"
      "VALUES "
      "(:local,:id_src,:id_dst )"
    ),
    incl_select(
      "SELECT COUNT(*) FROM includes WHERE "
      "local=:local AND id_src=:id_src AND id_dst=:id_dst"
    ),
    innerclass_insert(
      "INSERT INTO innerclass "
      "( refid, prot, name )"
      "VALUES "
      "(:refid,:prot,:name )"
    ),
    files_select(
      "SELECT rowid FROM files WHERE "
      "name=:name"
    ),
    files_insert(
      "INSERT INTO files "
      "( name )"
      "VALUES "
      "(:name )"
    ),
    dirs_select(
      "SELECT rowid FROM dirs WHERE "
      "refid=:refid"
      " AND name=:name"
      " AND location=:location"
    ),
    dirs_insert(
      "INSERT INTO dirs "
      "("
        "refid,"
        "name,"
        "location,"
        "briefdescription,"
        "detaileddescription"
      ")"
      "VALUES "
      "("
        ":refid,"
        ":name,"
        ":location,"
        ":briefdescription,"
        ":detaileddescription"
      ")"
    ),
    refids_select(
      "SELECT rowid FROM refids WHERE "
      "refid=:refid"
    ),
    refids_insert(
       "INSERT INTO refids "
       "( refid )"
       "VALUES "
       "(:refid )"
    ),
    xrefs_insert(
      "INSERT INTO xrefs "
      "( refid_src, refid_dst, id_file, line, column )"
      "VALUES "
      "(:refid_src,:refid_dst,:id_file,:line,:column )"
    ),
    memberdef_insert(
      "INSERT INTO memberdef "
      "("
        "refid,"
        "name,"
        "definition,"
        "type,"
        "argsstring,"
        "scope,"
        "initializer,"
        "bitfield,"
        "read,"
        "write,"
        "exceptions,"
        "enumvalue,"
        "prot,"
        "static,"
        "const,"
        "explicit,"
        "inline,"
        "final,"
        "sealed,"
        "new,"
        "optional,"
        "required,"
        "volatile,"
        "virt,"
        "mutable,"
        "initonly,"
        "attribute,"
        "property,"
        "readonly,"
        "bound,"
        "constrained,"
        "transient,"
        "maybevoid,"
        "maybedefault,"
        "maybeambiguous,"
        "readable,"
        "writable,"
        "gettable,"
        "protectedsettable,"
        "protectedgettable,"
        "settable,"
        "privatesettable,"
        "privategettable,"
        "accessor,"
        "addable,"
        "removable,"
        "raisable,"
        "kind,"
        "bodystart,"
        "bodyend,"
        "id_bodyfile,"
        "id_file,"
        "line,"
        "column,"
        "briefdescription,"
        "detaileddescription,"
        "inbodydescription"
      ")"
      "VALUES "
      "("
        ":refid,"
        ":name,"
        ":definition,"
        ":type,"
        ":argsstring,"
        ":scope,"
        ":initializer,"
        ":bitfield,"
        ":read,"
        ":write,"
        ":exceptions,"
        ":enumvalue,"
        ":prot,"
        ":static,"
        ":const,"
        ":explicit,"
        ":inline,"
        ":final,"
        ":sealed,"
        ":new,"
        ":optional,"
        ":required,"
        ":volatile,"
        ":virt,"
        ":mutable,"
        ":initonly,"
        ":attribute,"
        ":property,"
        ":readonly,"
        ":bound,"
        ":constrained,"
        ":transient,"
        ":maybevoid,"
        ":maybedefault,"
        ":maybeambiguous,"
        ":readable,"
        ":writable,"
        ":gettable,"
        ":privategettable,"
        ":protectedgettable,"
        ":settable,"
        ":privatesettable,"
        ":privategettable,"
        ":accessor,"
        ":addable,"
        ":removable,"
        ":raisable,"
        ":kind,"
        ":bodystart,"
        ":bodyend,"
        ":id_bodyfile,"
        ":id_file,"
        ":line,"
        ":column,"
        ":briefdescription,"
        ":detaileddescription,"
        ":inbodydescription"
      ")"
    ),
    compounddef_insert(
      "INSERT INTO compounddef "
        "( name, kind, prot, refid, id_file, line, column, briefdescription, detaileddescription)"
        "VALUES "
        "(:name,:kind,:prot,:refid,:id_file,:line,:column,:briefdescription,:detaileddescription)"
    ),
    basecompoundref_insert(
      "INSERT INTO  basecompoundref "
        "( base, derived, refid, prot, virt )"
        "VALUES "
        "(:base,:derived,:refid,:prot,:virt )"
    ),
    derivedcompoundref_insert(
      "INSERT INTO  derivedcompoundref "
        "( refid, prot, virt, base, derived )"
        "VALUES "
        "(:refid,:prot,:virt,:base,:derived )"
    ),
    params_select(
      "SELECT rowid FROM  params WHERE "
        "(attributes=:attributes OR attributes IS NULL) AND "
        "(type=:type OR type IS NULL) AND "
        "(declname=:declname OR declname IS NULL) AND "
        "(defname=:defname OR defname IS NULL) AND "
        "(array=:array OR array IS NULL) AND "
        "(defval=:defval OR defval IS NULL) AND "
        "(briefdescription=:briefdescription OR briefdescription IS NULL)"
    ),
    params_insert(
      "INSERT INTO  params "
      "( attributes, type, declname, defname, array, defval, briefdescription )"
      "VALUES "
      "(:attributes,:type,:declname,:defname,:array,:defval,:briefdescription)"
    ),
    memberdef_params_insert(
      "INSERT INTO  memberdef_params "
      "( id_memberdef, id_param)"
      "VALUES "
      "(:id_memberdef,:id_param)"
    ),
    compounddef_members_insert(
      "INSERT INTO  compounddef_members "
      "( id_compounddef, id_memberdef)"
      "VALUES "
      "(:id_compounddef,:id_memberdef)"
    ),
    innernamespace_insert(
      "INSERT INTO  innernamespaces "
      "( refid, name)"
      "VALUES "
      "(:refid,:name)"
    )
    {
      ;
    }
};

//////////////////////////////////////////////////////

bool Sqlite3Gen::Open(const char* f)
{
  int rc = sqlite3_open_v2(f, &m_db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
      0);
  if (rc != SQLITE_OK)
  {
    sqlite3_close(m_db);
    err("database open failed: %s\n", f);
    return FALSE;
  }
  return TRUE;
}

bool Sqlite3Gen::Close()
{
  sqlite3_close(m_db);
}

void Sqlite3Gen::BeginTransaction()
{
  char *sErrMsg = 0;
  sqlite3_exec(m_db, "BEGIN TRANSACTION", NULL, NULL, &sErrMsg);
}

void Sqlite3Gen::EndTransaction()
{
  char *sErrMsg = 0;
  sqlite3_exec(m_db, "END TRANSACTION", NULL, NULL, &sErrMsg);
}

void Sqlite3Gen::PragmaTuning()
{
  char *sErrMsg = 0;
  sqlite3_exec(m_db, "PRAGMA synchronous = OFF", NULL, NULL, &sErrMsg);
  sqlite3_exec(m_db, "PRAGMA journal_mode = MEMORY", NULL, NULL, &sErrMsg);
  sqlite3_exec(m_db, "PRAGMA temp_store = MEMORY;", NULL, NULL, &sErrMsg);
}

bool Sqlite3Gen::InitializeSchema()
{
  int rc;
  sqlite3_stmt *stmt = 0;

  msg("Initializing m_db schema...\n");
  for (unsigned int k = 0;
      k < sizeof(schema_queries) / sizeof(schema_queries[0]);
      k++)
  {
    const char *q = schema_queries[k][1];
    // create table
    rc = sqlite3_prepare_v2(m_db, q, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
      err("sqlite3_prepare_v2 failed: [%s]\n\t%s\n", q, sqlite3_errmsg(m_db));
      return FALSE;
    }
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
      err("sqlite3_step failed: [%s]\n\t%s\n", q, sqlite3_errmsg(m_db));
      return FALSE;
    }
    sqlite3_finalize(stmt);
  }
  return TRUE;
}

bool Sqlite3Gen::PrepareStatements()
{
  if (
      -1==prepareStatement(memberdef_insert) ||
      -1==prepareStatement(memberdef_params_insert) ||
      -1==prepareStatement(files_insert) ||
      -1==prepareStatement(files_select) ||
      -1==prepareStatement(dirs_insert) ||
      -1==prepareStatement(dirs_select) ||
      -1==prepareStatement(refids_insert) ||
      -1==prepareStatement(refids_select) ||
      -1==prepareStatement(incl_insert) ||
      -1==prepareStatement(incl_select) ||
      -1==prepareStatement(params_insert) ||
      -1==prepareStatement(params_select) ||
      -1==prepareStatement(xrefs_insert) ||
      -1==prepareStatement(innerclass_insert) ||
      -1==prepareStatement(compounddef_insert) ||
      -1==prepareStatement(basecompoundref_insert) ||
      -1==prepareStatement(derivedcompoundref_insert) ||
      -1==prepareStatement(compounddef_members_insert) ||
      -1==prepareStatement(innernamespace_insert)
     )
     {
       err("sqlite generator: prepareStatements failed!");
       return FALSE;
     }
  return TRUE;
}

void Sqlite3Gen::InsertClasses(ClassSDict& csd)
{
  ClassSDict::Iterator cli(csd);
  ClassDef *cd;
  for (cli.toFirst(); (cd = cli.current()); ++cli)
  {
    msg("Generating Sqlite3 output for class %s\n", cd->name().data());
    insertClassDef(cd);
  }
}

void Sqlite3Gen::InsertNamespaces(NamespaceSDict& nsd)
{
  NamespaceSDict::Iterator nli(nsd);
  NamespaceDef *nd;
  for (nli.toFirst(); (nd = nli.current()); ++nli)
  {
    msg("Generating Sqlite3 output for namespace %s\n", nd->name().data());
    insertNamespaceDef(nd);
  }
}

void Sqlite3Gen::InsertFiles(FileNameList& fnl)
{
  FileNameListIterator fnli(fnl);
  FileName *fn;
  for (; (fn = fnli.current()); ++fnli)
  {
    FileNameIterator fni(*fn);
    FileDef *fd;
    for (; (fd = fni.current()); ++fni)
    {
      msg("Generating Sqlite3 output for file %s\n", fd->name().data());
      insertFileDef(fd);
    }
  }
}

void Sqlite3Gen::InsertGroups(GroupSDict& gsd)
{
  GroupSDict::Iterator gli(gsd);
  GroupDef *gd;
  for (; (gd = gli.current()); ++gli)
  {
    msg("Generating Sqlite3 output for group %s\n", gd->name().data());
    insertGroupDef(gd);
  }
}

void Sqlite3Gen::InsertPages(PageSDict& psd)
{
  PageSDict::Iterator pdi(psd);
  PageDef *pd = 0;
  for (pdi.toFirst(); (pd = pdi.current()); ++pdi)
  {
    msg("Generating Sqlite3 output for page %s\n", pd->name().data());
    InsertPageDef(pd, FALSE);
  }
}

void Sqlite3Gen::InsertDirs(DirSDict& dsd)
{
  DirDef *dir;
  DirSDict::Iterator sdi(dsd);
  for (sdi.toFirst(); (dir = sdi.current()); ++sdi)
  {
    msg("Generating Sqlite3 output for dir %s\n", dir->name().data());
    insertDirDef(dir);
  }
}

void Sqlite3Gen::InsertExamples(PageSDict& esd)
{
  PageSDict::Iterator pdi(esd);
  PageDef *pd = 0;
  for (pdi.toFirst(); (pd = pdi.current()); ++pdi)
  {
    msg("Generating Sqlite3 output for example %s\n", pd->name().data());
    InsertPageDef(pd, TRUE);
  }
}

int Sqlite3Gen::prepareStatement(Sqlite3Stmt &s)
{
  int rc;
  rc = sqlite3_prepare_v2(m_db, s.query, -1, &s.stmt, 0);
  if (rc != SQLITE_OK)
  {
    err("prepareStatement failed for: [%s]\n%s\n", s.query, sqlite3_errmsg(m_db));
    return -1;
  }
  return rc;
}

int Sqlite3Gen::step(Sqlite3Stmt &s, bool getRowId/*=FALSE*/,
                     bool select/*=FALSE*/)
{
  int rowid = -1;
  int rc    = sqlite3_step(s.stmt);

  if (rc != SQLITE_DONE && rc != SQLITE_ROW)
  {
    err("sqlite3_step failed: %s\n", sqlite3_errmsg(m_db));
    sqlite3_clear_bindings(s.stmt);
    return -1;
  }

  if (getRowId && select)
    rowid = sqlite3_column_int(s.stmt, 0);  // works on selects, doesnt on inserts

  if (getRowId && !select)
    rowid = sqlite3_last_insert_rowid(m_db);  // works on inserts, doesnt on selects

  sqlite3_reset(s.stmt);
  sqlite3_clear_bindings(s.stmt);  // @todo When should this really be called
  return rowid;
}

int Sqlite3Gen::insertFile(const char *name)
{
  int rowid = -1;
  if (name == 0)
    return rowid;

  files_select.bindTextParameter(":name", name);
  rowid = step(files_select, TRUE, TRUE);
  if (rowid == 0)
  {
    files_insert.bindTextParameter(":name", name);
    rowid = step(files_insert, TRUE);
  }
  return rowid;
}

int Sqlite3Gen::insertRefid(const char *refid)
{
  int rowid = -1;
  if (refid == 0)
    return rowid;

  refids_select.bindTextParameter(":refid", refid);
  rowid = step(refids_select, TRUE, TRUE);
  if (rowid == 0)
  {
    refids_insert.bindTextParameter(":refid", refid);
    rowid = step(refids_insert, TRUE);
  }
  return rowid;
}

void Sqlite3Gen::bindMemberVariable(MemberDef *md)
{
  memberdef_insert.bindIntParameter(":mutable", md->isMutable());
  memberdef_insert.bindIntParameter(":initonly", md->isInitonly());
  memberdef_insert.bindIntParameter(":attribute", md->isAttribute());
  memberdef_insert.bindIntParameter(":property", md->isProperty());
  memberdef_insert.bindIntParameter(":readonly", md->isReadonly());
  memberdef_insert.bindIntParameter(":bound", md->isBound());
  memberdef_insert.bindIntParameter(":removable", md->isRemovable());
  memberdef_insert.bindIntParameter(":constrained", md->isConstrained());
  memberdef_insert.bindIntParameter(":transient", md->isTransient());
  memberdef_insert.bindIntParameter(":maybevoid", md->isMaybeVoid());
  memberdef_insert.bindIntParameter(":maybedefault", md->isMaybeDefault());
  memberdef_insert.bindIntParameter(":maybeambiguous", md->isMaybeAmbiguous());
  if (md->bitfieldString())
  {
    QCString bitfield = md->bitfieldString();
    if (bitfield.at(0) == ':')
      bitfield = bitfield.mid(1);
    bitfield   = bitfield.stripWhiteSpace();
    memberdef_insert.bindTextParameter(":bitfield", bitfield, FALSE);
  }
}

void Sqlite3Gen::bindMemberProperty(MemberDef *md)
{
  memberdef_insert.bindIntParameter(":readable", md->isReadable());
  memberdef_insert.bindIntParameter(":writable", md->isWritable());
  memberdef_insert.bindIntParameter(":gettable", md->isGettable());
  memberdef_insert.bindIntParameter(":privategettable",
                   md->isPrivateGettable());
  memberdef_insert.bindIntParameter(":protectedgettable",
                   md->isProtectedGettable());
  memberdef_insert.bindIntParameter(":settable", md->isSettable());
  memberdef_insert.bindIntParameter(":privatesettable",
                   md->isPrivateSettable());
  memberdef_insert.bindIntParameter(":protectedsettable",
                   md->isProtectedSettable());
  if (md->isAssign() || md->isCopy() || md->isRetain() || md->isStrong()
      || md->isWeak())
  {
    int accessor = 0;
    if (md->isAssign())      accessor = 1;
    else if (md->isCopy())   accessor = 2;
    else if (md->isRetain()) accessor = 3;
    else if (md->isStrong()) accessor = 4;
    else if (md->isWeak())   accessor = 5;

    memberdef_insert.bindIntParameter(":accessor", accessor);
  }
  memberdef_insert.bindTextParameter(":read", md->getReadAccessor());
  memberdef_insert.bindTextParameter(":write", md->getWriteAccessor());
}

void Sqlite3Gen::bindMemberFunc(MemberDef *md)
{
  ArgumentList *al = md->argumentList();
  if (al != 0)
  {
    memberdef_insert.bindIntParameter(":const", al->constSpecifier);
    memberdef_insert.bindIntParameter(":volatile", al->volatileSpecifier);
  }
  memberdef_insert.bindIntParameter(":explicit", md->isExplicit());
  memberdef_insert.bindIntParameter(":inline", md->isInline());
  memberdef_insert.bindIntParameter(":final", md->isFinal());
  memberdef_insert.bindIntParameter(":sealed", md->isSealed());
  memberdef_insert.bindIntParameter(":new", md->isNew());
  memberdef_insert.bindIntParameter(":optional", md->isOptional());
  memberdef_insert.bindIntParameter(":required", md->isRequired());
  memberdef_insert.bindIntParameter(":virt", md->virtualness());
}

void Sqlite3Gen::bindMemberEvent(MemberDef *md)
{
  memberdef_insert.bindIntParameter(":addable", md->isAddable());
  memberdef_insert.bindIntParameter(":removable", md->isRemovable());
  memberdef_insert.bindIntParameter(":raisable", md->isRaisable());
}

void Sqlite3Gen::bindMemberDescriptions(MemberDef *md)
{
  memberdef_insert.bindTextParameter(":briefdescription",
                    md->briefDescription(), FALSE);
  memberdef_insert.bindTextParameter(":detaileddescription",
                    md->documentation(), FALSE);
  memberdef_insert.bindTextParameter(":inbodydescription",
                    md->inbodyDocumentation(), FALSE);
}

void Sqlite3Gen::bindMemberFileLocation(MemberDef *md)
{
  if (md->getDefLine() == -1)
    return;

  int id_file = insertFile(stripFromPath(md->getDefFileName()));
  if (id_file == -1)
  {
    // @todo clear memberdef_insert bindings
    return;
  }

  memberdef_insert.bindIntParameter(":id_file", id_file);
  memberdef_insert.bindIntParameter(":line", md->getDefLine());
  memberdef_insert.bindIntParameter(":column", md->getDefColumn());

  if (md->getStartBodyLine() != -1)
  {
    int id_bodyfile
        = insertFile(stripFromPath(md->getBodyDef()->absFilePath()));
    if (id_bodyfile == -1)
    {
      sqlite3_clear_bindings(memberdef_insert.stmt);
    }
    else
    {
      memberdef_insert.bindIntParameter(":id_bodyfile", id_bodyfile);
      memberdef_insert.bindIntParameter(":bodystart", md->getStartBodyLine());
      memberdef_insert.bindIntParameter(":bodyend", md->getEndBodyLine());
    }
  }
}

void Sqlite3Gen::bindMemberEnumeration(MemberDef *md)
{
  MemberList *enumFields = md->enumFieldList();

  if (enumFields == NULL)
    return;
#if 0
  MemberListIterator emli(*enumFields);
  MemberDef *emd;
  for (emli.toFirst();(emd=emli.current());++emli)
  {
    ti << "    <member refid=\"" << memberOutputFileBase(emd) 
      << "_1" << emd->anchor() << "\" kind=\"enumvalue\"><name>" 
      << convertToXML(emd->name()) << "</name></member>" << endl;

    t << "        <enumvalue id=\"" << memberOutputFileBase(emd) << "_1" 
      << emd->anchor() << "\" prot=\"";
    switch (emd->protection())
    {
      case Public:    t << "public";    break;
      case Protected: t << "protected"; break;
      case Private:   t << "private";   break;
      case Package:   t << "package";   break;
    }
    t << "\">" << endl;
    t << "          <name>";
    writeXMLString(t,emd->name());
    t << "</name>" << endl;
    if (!emd->initializer().isEmpty())
    {
      t << "          <initializer>";
      writeXMLString(t,emd->initializer());
      t << "</initializer>" << endl;
    }
    t << "          <briefdescription>" << endl;
    writeXMLDocBlock(t,emd->briefFile(),emd->briefLine(),emd->getOuterScope(),emd,emd->briefDescription());
    t << "          </briefdescription>" << endl;
    t << "          <detaileddescription>" << endl;
    writeXMLDocBlock(t,emd->docFile(),emd->docLine(),emd->getOuterScope(),emd,emd->documentation());
    t << "          </detaileddescription>" << endl;
    t << "        </enumvalue>" << endl;
  }
#endif
}

void Sqlite3Gen::insertSubDirs(const DirList *dl, int id_dir)
{
  if (dl == NULL) return;

  DirDef *subdir;
  QListIterator<DirDef> subdirs(*dl);
  for (subdirs.toFirst();(subdir=subdirs.current());++subdirs)
  {
    //t << "    <innerdir refid=\"" << subdir->getOutputFileBase() 
    //  << "\">" << convertToXML(subdir->displayName()) << "</innerdir>" << endl;
  }
}

void Sqlite3Gen::insertXref(int refid_src, int refid_dst,
                                  int id_file, int line, int column)
{
  if (id_file == -1 || refid_src == -1 || refid_dst == -1)
    return;

  xrefs_insert.bindIntParameter(":refid_src", refid_src);
  xrefs_insert.bindIntParameter(":refid_dst", refid_dst);
  //xrefs_insert.bindIntParameter(":id_file", id_file);
  //xrefs_insert.bindIntParameter(":line", line);
  //xrefs_insert.bindIntParameter(":column", column);
  step(xrefs_insert);
}

int Sqlite3Gen::insertParam(Argument *a, Argument* defArg/*=NULL*/)
{
  params_select.bindTextParameter(":type", a->type.data());
  params_select.bindTextParameter(":declname", a->name.data());
  params_select.bindTextParameter(":defval", a->defval.data());
  params_select.bindTextParameter(":array", a->array.data());
  params_select.bindTextParameter(":attributes", a->attrib.data());
  if (defArg && !defArg->name.isEmpty() && defArg->name != a->name)
  {
    params_select.bindTextParameter(":defname", defArg->name.data());
  }
  int id_param = step(params_select, TRUE, TRUE);
  if (id_param == 0)
  {
    params_insert.bindTextParameter(":type", a->type.data());
    params_insert.bindTextParameter(":declname", a->name.data());
    params_insert.bindTextParameter(":defval", a->defval.data());
    params_insert.bindTextParameter(":array", a->array.data());
    params_insert.bindTextParameter(":attributes", a->attrib.data());
    if (defArg && !defArg->name.isEmpty() && defArg->name != a->name)
    {
      params_insert.bindTextParameter(":defname", defArg->name.data());
    }
    id_param = step(params_insert, TRUE);
  }
  return id_param;
}

void Sqlite3Gen::insertMemberReference(MemberDef *src, MemberDef *dst)
{
  QCString qrefid_src = src->getOutputFileBase() + "_1" + src->anchor();
  QCString qrefid_dst = dst->getOutputFileBase() + "_1" + dst->anchor();
  int refid_src = insertRefid(qrefid_src.data());
  int refid_dst = insertRefid(qrefid_dst.data());
  insertXref(refid_src, refid_dst, 0, 0, 0);
  // @todo replace "no-file" with proper file
  if (dst->getStartBodyLine() != -1 && dst->getBodyDef())
  {
    int refid_src = insertRefid(qrefid_src.data());
    int refid_dst = insertRefid(qrefid_dst.data());

    // @todo replace "no-file" with proper file
    int id_file = insertFile("no-file");
    insertXref(refid_src, refid_dst,
                          id_file,
                          dst->getStartBodyLine(),
                          -1);
  }
}

void Sqlite3Gen::insertXrefsFromText(Definition *scope,
                                FileDef *fileScope,
                                Definition *self,
                                const char *text)
{
  StringList l;
  linkifyText(TextGeneratorSqlite3Impl(l), scope, fileScope, self, text);

  QCString *s;
  StringListIterator li(l);
  while ((s = li.current()))
  {
    QCString qrefid_src = self->getOutputFileBase() + "_1" + self->anchor();
    int refid_src = insertRefid(qrefid_src.data());
    int refid_dst = insertRefid(s->data());
    int id_file = insertFile(stripFromPath(scope->getDefFileName()));
    //@todo test if self->getDefLine() works for all Definitions. maybe use getStartBodyLine()
    insertXref(refid_src, refid_dst, id_file, self->getDefLine(), -1);
    ++li;
  }
}

void Sqlite3Gen::insertMemberDefParams(Definition *scope,
                                      MemberDef *md,
                                      int id_memberdef)
{
  ArgumentList *defAl  = md->argumentList();
  ArgumentList *declAl = md->declArgumentList();
  if (declAl != 0 && declAl->count() > 0)
  {
    Argument *a;
    ArgumentListIterator defAli(*defAl);
    ArgumentListIterator declAli(*declAl);
    for (declAli.toFirst(); (a = declAli.current()); ++declAli)
    {
      Argument *defArg = defAli.current();
      if (!a->type.isEmpty())
      {
        insertXrefsFromText(scope, md->getBodyDef(), md, a->type);
      }
      int id_param = insertParam(a, defArg);
      memberdef_params_insert.bindIntParameter(":id_param", id_param);
      memberdef_params_insert.bindIntParameter(":id_memberdef", id_memberdef);
      step(memberdef_params_insert);

      if (defArg)
        ++defAli;
    }
  }
}

void Sqlite3Gen::insertMemberDefParams(MemberDef *md,
                                    int id_memberdef)
{
  if (md->argumentList()->count() == 0)  // special case for "foo()" to
                                         // disguish it from "foo".
  {
    DBG_CTX(("no params\n"));
  }
  else
  {
    Argument *a;
    ArgumentListIterator ali(*md->argumentList());
    for (ali.toFirst(); (a = ali.current()); ++ali)
    {
      params_select.bindTextParameter(":defname", a->type.data());
      int id_param = step(params_select, TRUE, TRUE);

      if (id_param == 0)
      {
        params_insert.bindTextParameter(":defname", a->type.data());
        id_param = step(params_insert, TRUE);
      }

      memberdef_params_insert.bindIntParameter(":id_param", id_param);
      memberdef_params_insert.bindIntParameter(":id_memberdef", id_memberdef);
      step(memberdef_params_insert);
    }
  }
}


void Sqlite3Gen::writeInnerClasses(const ClassSDict *cl)
{
  if (!cl)
    return;

  ClassSDict::Iterator cli(*cl);
  ClassDef *cd;
  for (cli.toFirst(); (cd = cli.current()); ++cli)
  {
    if (!cd->isHidden() && cd->name().find('@') == -1)  // skip anonymous scopes
    {
      int refid = insertRefid(cd->getOutputFileBase());
      innerclass_insert.bindIntParameter(":refid", refid);
      innerclass_insert.bindIntParameter(":prot", cd->protection());
      innerclass_insert.bindTextParameter(":name", cd->name());
      step(innerclass_insert);
    }
  }
}


void Sqlite3Gen::writeInnerNamespaces(const NamespaceSDict *nl)
{
  if (nl)
  {
    NamespaceDef *nd;
    NamespaceSDict::Iterator nli(*nl);
    for (nli.toFirst(); (nd = nli.current()); ++nli)
    {
      if (!nd->isHidden() && nd->name().find('@') == -1)  // skip anonymouse scopes
      {
        int refid = insertRefid(nd->getOutputFileBase());
        innernamespace_insert.bindIntParameter(":refid", refid);
        innernamespace_insert.bindTextParameter(":name", nd->name(), FALSE);
        step(innernamespace_insert);
      }
    }
  }
}


void Sqlite3Gen::writeTemplateArgumentList(Definition *scope,
                                      FileDef *fileScope,
                                      ArgumentList *al)
{
  if (al == NULL)
    return;

  ArgumentListIterator ali(*al);
  Argument *a;
  for (ali.toFirst(); (a = ali.current()); ++ali)
  {
    params_select.bindTextParameter(":type", a->type);
    params_select.bindTextParameter(":declname", a->name);
    params_select.bindTextParameter(":defname", a->name);
    params_select.bindTextParameter(":defval", a->defval);
    if (step(params_select, TRUE, TRUE) == 0)
    {
      params_insert.bindTextParameter(":declname", a->name);
      params_insert.bindTextParameter(":defname", a->name);
      params_insert.bindTextParameter(":type", a->type);
      params_insert.bindTextParameter(":defval", a->defval);
#warning insertXrefsFromText(scope, fileScope, 0, a->type);
#warning insertXrefsFromText(scope, fileScope, 0, a->defval);
      step(params_insert);
    }
  }
}

void Sqlite3Gen::writeMemberTemplateLists(MemberDef *md)
{
  ArgumentList *templMd = md->templateArguments();
  if (templMd)  // function template prefix
  {
    writeTemplateArgumentList(md->getClassDef(), md->getFileDef(), templMd);
  }
}

void Sqlite3Gen::writeTemplateList(ClassDef *cd)
{
  writeTemplateArgumentList(cd, NULL, cd->templateArguments());
}

int Sqlite3Gen::insertMemberDef(Definition *scope, MemberDef *md)
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
  if (md->memberType() == MemberType_EnumValue)
    return -1;

  if (md->isHidden())
    return -1;
  // if (md->name().at(0)=='@') return; // anonymous member

  // group members are only visible in their group
  // if (scope->definitionType()!=Definition::TypeGroup && md->getGroupDef()) return;
  // QCString memType;

  // memberdef
  QCString qrefid = md->getOutputFileBase() + "_1" + md->anchor();
  int refid = insertRefid(qrefid.data());

  memberdef_insert.bindIntParameter(":refid", refid);
  memberdef_insert.bindIntParameter(":kind", md->memberType());
  memberdef_insert.bindIntParameter(":prot", md->protection());
  memberdef_insert.bindIntParameter(":static", md->isStatic());
  memberdef_insert.bindTextParameter(":name", md->name());

  bool isFunc = FALSE;
  switch (md->memberType())
  {
    case MemberType_Variable:
      bindMemberVariable(md);
      break;
    case MemberType_Property:
      bindMemberProperty(md);
      break;
    case MemberType_Event:
      bindMemberEvent(md);
      break;
    case MemberType_Function:  // fall through
    case MemberType_Signal:    // fall through
    case MemberType_Friend:    // fall through
    case MemberType_DCOP:      // fall through
    case MemberType_Slot:
      isFunc = TRUE;
      bindMemberFunc(md);
      break;
    default:
      break;
  }

  // + declaration/definition arg lists
  if (md->memberType() != MemberType_Define &&
      md->memberType() != MemberType_Enumeration
     )
  {
    if (md->memberType() != MemberType_Typedef)
    {
      writeMemberTemplateLists(md);
    }
    QCString typeStr = md->typeString();
    stripQualifiers(typeStr);
    insertXrefsFromText(scope, md->getBodyDef(), md, typeStr);
    if (typeStr.data())
    {
      memberdef_insert.bindTextParameter(":type", typeStr.data(), FALSE);
    }

    if (md->definition())
    {
      memberdef_insert.bindTextParameter(":definition", md->definition());
    }

    if (md->argsString())
    {
      memberdef_insert.bindTextParameter(":argsstring", md->argsString());
    }
  }


  if (md->excpString())
  {
    memberdef_insert.bindTextParameter(":exceptions", md->excpString());
    insertXrefsFromText(scope, md->getBodyDef(), md, md->excpString());
  }
  if (md->memberType() == MemberType_Enumeration) // enum
  {
    bindMemberEnumeration(md);
  }
  if (md->getScopeString())
  {
    memberdef_insert.bindTextParameter(":scope", md->getScopeString().data(),
                      FALSE);
  }

  // +Brief, detailed and inbody description
  bindMemberDescriptions(md);

  // File location
  bindMemberFileLocation(md);

  // + source references
  // The cross-references in initializers only work when both the src and dst
  // are defined.
  MemberSDict *mdict = md->getReferencesMembers();
  if (mdict != NULL)
  {
    MemberSDict::IteratorDict mdi(*mdict);
    MemberDef *rmd;
    for (mdi.toFirst(); (rmd = mdi.current()); ++mdi)
    {
      insertMemberReference(md, rmd);
    }
  }

  // + source referenced by
  mdict = md->getReferencedByMembers();
  if (mdict != NULL)
  {
    MemberSDict::IteratorDict mdi(*mdict);
    MemberDef *rmd;
    for (mdi.toFirst(); (rmd = mdi.current()); ++mdi)
    {
      insertMemberReference(rmd, md);
    }
  }

  // Extract references from initializer
  if (md->hasMultiLineInitializer() || md->hasOneLineInitializer())
  {
    memberdef_insert.bindTextParameter(":initializer", md->initializer());
    insertXrefsFromText(scope, md->getBodyDef(), md, md->initializer());
  }


  /////////////////////////////////////
  // Things using the memberdef ID
  int id_memberdef = step(memberdef_insert, TRUE);

  if (isFunc)
  {
    insertMemberDefParams(scope, md, id_memberdef);
  }
  else if (md->memberType() == MemberType_Define && md->argsString())
  {
    insertMemberDefParams(md, id_memberdef);
  }

#warning Add link between id_memberdef and its template params
  return id_memberdef;
}

void Sqlite3Gen::insertScopeMembers(Definition *scope,
                               MemberList *ml,
                               Sqlite3Stmt &s,
                               const char *scope_name,
                               int id_scope)
{
  if (ml == NULL)
    return;

  MemberDef *md;
  MemberListIterator mli(*ml);

  for (mli.toFirst(); (md = mli.current()); ++mli)
  {
    int id_memberdef = insertMemberDef(scope, md);
    s.bindIntParameter(scope_name, id_scope);
    s.bindIntParameter(":id_memberdef", id_memberdef);
    step(s);
  }
}

void Sqlite3Gen::insertClassDef(ClassDef *cd)
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

  msg("Generating Sqlite3 output for class %s\n", cd->name().data());

  compounddef_insert.bindTextParameter(":name", cd->name());
  compounddef_insert.bindTextParameter(":kind", cd->compoundTypeString(),
                    FALSE);
  compounddef_insert.bindIntParameter(":prot", cd->protection());
  int refid = insertRefid(cd->getOutputFileBase());
  compounddef_insert.bindIntParameter(":refid", refid);

  int id_file = insertFile(stripFromPath(cd->getDefFileName()));
  compounddef_insert.bindIntParameter(":id_file", id_file);
  compounddef_insert.bindIntParameter(":line", cd->getDefLine());
  compounddef_insert.bindIntParameter(":column", cd->getDefColumn());
  compounddef_insert.bindTextParameter(":briefdescription", cd->briefDescription());
  compounddef_insert.bindTextParameter(":detaileddescription", cd->documentation());

  /////////////////////////////////////
  // Things using the compounddef ID
  int id_compounddef = step(compounddef_insert, TRUE);

  // + list of direct super classes
  if (cd->baseClasses())
  {
    BaseClassListIterator bcli(*cd->baseClasses());
    BaseClassDef *bcd;
    for (bcli.toFirst(); (bcd = bcli.current()); ++bcli)
    {
      int refid = insertRefid(bcd->classDef->getOutputFileBase());
      basecompoundref_insert.bindIntParameter(":refid", refid);
      basecompoundref_insert.bindIntParameter(":prot", bcd->prot);
      basecompoundref_insert.bindIntParameter(":virt", bcd->virt);

      if (!bcd->templSpecifiers.isEmpty())
      {
        basecompoundref_insert.bindTextParameter(":base",
                          insertTemplateSpecifierInScope(bcd->classDef->name(),
                                                         bcd->templSpecifiers),
                          FALSE);
      }
      else
      {
        basecompoundref_insert.bindTextParameter(":base",
                          bcd->classDef->displayName(), FALSE);
      }
      basecompoundref_insert.bindTextParameter(":derived", cd->displayName(),
                        FALSE);
      step(basecompoundref_insert);
    }
  }

  // + list of direct sub classes
  if (cd->subClasses())
  {
    BaseClassListIterator bcli(*cd->subClasses());
    BaseClassDef *bcd;
    for (bcli.toFirst(); (bcd = bcli.current()); ++bcli)
    {
      derivedcompoundref_insert.bindTextParameter(":base", cd->displayName(),
                        FALSE);
      if (!bcd->templSpecifiers.isEmpty())
      {
        derivedcompoundref_insert.bindTextParameter(":derived",
                          insertTemplateSpecifierInScope(bcd->classDef->name(),
                                                         bcd->templSpecifiers),
                          FALSE);
      }
      else
      {
        derivedcompoundref_insert.bindTextParameter(":derived",
                          bcd->classDef->displayName(), FALSE);
      }
      int refid = insertRefid(bcd->classDef->getOutputFileBase());
      derivedcompoundref_insert.bindIntParameter(":refid", refid);
      derivedcompoundref_insert.bindIntParameter(":prot", bcd->prot);
      derivedcompoundref_insert.bindIntParameter(":virt", bcd->virt);
      step(derivedcompoundref_insert);
    }
  }

  // + include file
  IncludeInfo *ii = cd->includeInfo();
  if (ii)
  {
    QCString nm = ii->includeName;
    if (nm.isEmpty() && ii->fileDef)
      nm = ii->fileDef->docName();
    if (!nm.isEmpty())
    {
      int id_dst = insertFile(nm);
      if (id_dst != -1)
      {
        incl_select.bindIntParameter(":local", ii->local);
        incl_select.bindIntParameter(":id_src", id_file);
        incl_select.bindIntParameter(":id_dst", id_dst);
        int count = step(incl_select, TRUE, TRUE);
        if (count == 0)
        {
          incl_insert.bindIntParameter(":local", ii->local);
          incl_insert.bindIntParameter(":id_src", id_file);
          incl_insert.bindIntParameter(":id_dst", id_dst);
          step(incl_insert);
        }
      }
    }
  }
  // + list of inner classes
  writeInnerClasses(cd->getClassSDict());

  // - template argument list(s)
  writeTemplateList(cd);

  // + member groups
  if (cd->getMemberGroupSDict())
  {
    MemberGroupSDict::Iterator mgli(*cd->getMemberGroupSDict());
    MemberGroup *mg;
    compounddef_members_insert.bindIntParameter(":id_compounddef", id_compounddef);
    for (; (mg = mgli.current()); ++mgli)
    {
      insertScopeMembers(cd, mg->members(), compounddef_members_insert, ":id_compounddef", id_compounddef);
    }
  }

  // + list of all members
  QListIterator<MemberList> mli(cd->getMemberLists());
  MemberList *ml;
  for (mli.toFirst(); (ml = mli.current()); ++mli)
  {
    if ((ml->listType() & MemberListType_detailedLists) == 0)
    {
      insertScopeMembers(cd, ml, compounddef_members_insert, ":id_compounddef", id_compounddef);
    }
  }
}

void Sqlite3Gen::insertNamespaceDef(NamespaceDef *nd)
{
  // + contained class definitions
  // + contained namespace definitions
  // + member groups
  // + normal members
  // - brief desc
  // - detailed desc
  // - location
  // - files containing (parts of) the namespace definition

  if (nd->isReference() || nd->isHidden())
    return;  // skip external references

  // + contained class definitions
  writeInnerClasses(nd->getClassSDict());

  // + contained namespace definitions
  writeInnerNamespaces(nd->getNamespaceSDict());

  // + member groups
  if (nd->getMemberGroupSDict())
  {
    MemberGroupSDict::Iterator mgli(*nd->getMemberGroupSDict());
    MemberGroup *mg;
    for (; (mg = mgli.current()); ++mgli)
    {
    #warning TODO replace the 0 with a proper id
      insertScopeMembers(nd, mg->members(), compounddef_members_insert, ":id_compounddef", 0);
    }
  }

  // + normal members
  MemberList *ml;
  QListIterator<MemberList> mli(nd->getMemberLists());
  for (mli.toFirst(); (ml = mli.current()); ++mli)
  {
    if ((ml->listType() & MemberListType_declarationLists) != 0)
    {
    #warning TODO replace the 0 with a proper id
      insertScopeMembers(nd, ml, compounddef_members_insert, ":id_compounddef", 0);
    }
  }
}

void Sqlite3Gen::insertFileDef(FileDef *fd)
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

  if (fd->isReference())
    return;  // skip external references

  // + includes files
  IncludeInfo *ii;
  if (fd->includeFileList())
  {
    QListIterator<IncludeInfo> ili(*fd->includeFileList());
    for (ili.toFirst(); (ii = ili.current()); ++ili)
    {
      int id_src = insertFile(fd->absFilePath().data());
      int id_dst = insertFile(ii->includeName.data());
      incl_select.bindIntParameter(":local", ii->local);
      incl_select.bindIntParameter(":id_src", id_src);
      incl_select.bindIntParameter(":id_dst", id_dst);
      if (step(incl_select, TRUE, TRUE) == 0)
      {
        incl_insert.bindIntParameter(":local", ii->local);
        incl_insert.bindIntParameter(":id_src", id_src);
        incl_insert.bindIntParameter(":id_dst", id_dst);
        step(incl_insert);
      }
    }
  }

  // + includedby files
  if (fd->includedByFileList())
  {
    QListIterator<IncludeInfo> ili(*fd->includedByFileList());
    for (ili.toFirst(); (ii = ili.current()); ++ili)
    {
      int id_src = insertFile(ii->includeName);
      int id_dst = insertFile(fd->absFilePath());
      incl_select.bindIntParameter(":local", ii->local);
      incl_select.bindIntParameter(":id_src", id_src);
      incl_select.bindIntParameter(":id_dst", id_dst);
      if (step(incl_select, TRUE, TRUE) == 0)
      {
        incl_insert.bindIntParameter(":local", ii->local);
        incl_insert.bindIntParameter(":id_src", id_src);
        incl_insert.bindIntParameter(":id_dst", id_dst);
        step(incl_insert);
      }
    }
  }

  // + contained class definitions
  if (fd->getClassSDict())
  {
    writeInnerClasses(fd->getClassSDict());
  }

  // + contained namespace definitions
  if (fd->getNamespaceSDict())
  {
    writeInnerNamespaces(fd->getNamespaceSDict());
  }

  // + member groups
  if (fd->getMemberGroupSDict())
  {
    MemberGroupSDict::Iterator mgli(*fd->getMemberGroupSDict());
    MemberGroup *mg;
    for (; (mg = mgli.current()); ++mgli)
    {
#warning TODO replace the 0 with a proper id
      insertScopeMembers(fd, mg->members(), compounddef_members_insert, ":id_compounddef", 0);
    }
  }

  // + normal members
  MemberList *ml;
  QListIterator<MemberList> mli(fd->getMemberLists());
  for (mli.toFirst(); (ml = mli.current()); ++mli)
  {
    if ((ml->listType() & MemberListType_declarationLists) != 0)
    {
#warning TODO replace the 0 with a proper id
      insertScopeMembers(fd, ml, compounddef_members_insert, ":id_compounddef", 0);
    }
  }
}

void Sqlite3Gen::insertGroupDef(GroupDef *gd)
{
#warning WorkInProgress
}

void Sqlite3Gen::insertDirDef(DirDef *dd)
{
  if (dd->isReference()) return; // skip external references

  int id_dir = -1;
  int refid = insertRefid(dd->getOutputFileBase());

  dirs_select.bindIntParameter(":refid", refid);
  dirs_select.bindTextParameter(":name", dd->displayName());
  dirs_select.bindTextParameter(":location", stripFromPath(dd->name()));
  id_dir = step(dirs_select, TRUE, TRUE);
  if (id_dir == 0)
  {
    dirs_insert.bindIntParameter(":refid", refid);
    dirs_insert.bindTextParameter(":name", dd->displayName());
    dirs_insert.bindTextParameter(":location", stripFromPath(dd->name()));

    dirs_insert.bindTextParameter(":briefdescription",
                      dd->briefDescription(), FALSE);
    dirs_insert.bindTextParameter(":detaileddescription",
                      dd->documentation(), FALSE);
    id_dir = step(dirs_insert, TRUE);
  }

  insertSubDirs(&dd->subDirs(),id_dir);
#warning insertInnerFiles(dd->getFiles(),t);
}

void Sqlite3Gen::InsertPageDef(PageDef *pd, bool isExample)
{
#warning WorkInProgress
}

//////////////////////////////////////////////////////////////////////////////

void generateSqlite3()
{
  int rv;

  QCString outputDirectory = Config_getString(OUTPUT_DIRECTORY);
  QDir sqlite3Dir(outputDirectory);

  if (SQLITE_OK != sqlite3_initialize())
  {
    msg("sqlite3_initialize failed\n");
    return;
  }

  Sqlite3Gen sqlite3gen;

  rv = sqlite3gen.Open(outputDirectory + "/doxygen_sqlite3.db");
  if (! rv)
    return;

  sqlite3gen.BeginTransaction();
  sqlite3gen.PragmaTuning();

  if (! sqlite3gen.InitializeSchema())
  {
    sqlite3gen.Close();
    return;
  }

  if (! sqlite3gen.PrepareStatements())
  {
    sqlite3gen.EndTransaction();
    sqlite3gen.Close();
    return;
  }

  sqlite3gen.InsertClasses(*Doxygen::classSDict);

  sqlite3gen.InsertNamespaces(*Doxygen::namespaceSDict);

  sqlite3gen.InsertFiles(*Doxygen::inputNameList);

  sqlite3gen.InsertGroups(*Doxygen::groupSDict);

  sqlite3gen.InsertPages(*Doxygen::pageSDict);

  sqlite3gen.InsertDirs(*Doxygen::directories);

  sqlite3gen.InsertExamples(*Doxygen::exampleSDict);

  if (Doxygen::mainPage)
  {
    msg("Generating Sqlite3 output for the main page\n");
    sqlite3gen.InsertPageDef(Doxygen::mainPage, FALSE);
  }

  sqlite3gen.EndTransaction();

  sqlite3gen.Close();

  sqlite3_shutdown();
}

#else  // USE_SQLITE3
void generateSqlite3()
{
  err("sqlite3 support has not been compiled in!");
}
#endif
// vim: noai:ts=2:sw=2:ss=2:expandtab
