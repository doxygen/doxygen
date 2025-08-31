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
#include <sstream>

#include "settings.h"
#include "message.h"


#include "sqlite3gen.h"
#include "doxygen.h"
#include "xmlgen.h"
#include "xmldocvisitor.h"
#include "config.h"
#include "util.h"
#include "outputlist.h"
#include "docparser.h"
#include "docnode.h"
#include "language.h"

#include "version.h"
#include "dot.h"
#include "arguments.h"
#include "classlist.h"
#include "filedef.h"
#include "namespacedef.h"
#include "filename.h"
#include "groupdef.h"
#include "membername.h"
#include "memberdef.h"
#include "pagedef.h"
#include "dirdef.h"
#include "section.h"
#include "fileinfo.h"
#include "dir.h"
#include "datetime.h"
#include "moduledef.h"

#include <sys/stat.h>
#include <string.h>
#include <sqlite3.h>

// enable to show general debug messages
// #define SQLITE3_DEBUG

// enable to print all executed SQL statements.
// I recommend using the smallest possible input list.
// #define SQLITE3_DEBUG_SQL

# ifdef SQLITE3_DEBUG
#  define DBG_CTX(x) printf x
# else // SQLITE3_DEBUG
#  define DBG_CTX(x) do { } while(0)
# endif

# ifdef SQLITE3_DEBUG_SQL
// used by sqlite3_trace in generateSqlite3()
static void sqlLog(void *dbName, const char *sql){
  msg("SQL: '{}'\n", sql);
}
# endif

const char * table_schema[][2] = {
  /* TABLES */
  { "meta",
    "CREATE TABLE IF NOT EXISTS meta (\n"
      "\t-- Information about this db and how it was generated.\n"
      "\t-- Doxygen info\n"
      "\tdoxygen_version    TEXT PRIMARY KEY NOT NULL,\n"
      /*
      Doxygen's version is likely to rollover much faster than the schema, and
      at least until it becomes a core output format, we might want to make
      fairly large schema changes even on minor iterations for Doxygen itself.
      If these tools just track a predefined semver schema version that can
      iterate independently, it *might* not be as hard to keep them in sync?
      */
      "\tschema_version     TEXT NOT NULL, -- Schema-specific semver\n"
      "\t-- run info\n"
      "\tgenerated_at       TEXT NOT NULL,\n"
      "\tgenerated_on       TEXT NOT NULL,\n"
      "\t-- project info\n"
      "\tproject_name       TEXT NOT NULL,\n"
      "\tproject_number     TEXT,\n"
      "\tproject_brief      TEXT\n"
    ");"
  },
  { "includes",
    "CREATE TABLE IF NOT EXISTS includes (\n"
      "\t-- #include relations.\n"
      "\trowid        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
      "\tlocal        INTEGER NOT NULL,\n"
      "\tsrc_id       INTEGER NOT NULL REFERENCES path, -- File id of the includer.\n"
      "\tdst_id       INTEGER NOT NULL REFERENCES path, -- File id of the includee.\n"
      /*
      In theory we could include name here to be informationally equivalent
      with the XML, but I don't see an obvious use for it.
      */
      "\tUNIQUE(local, src_id, dst_id) ON CONFLICT IGNORE\n"
    ");"
  },
  { "contains",
    "CREATE TABLE IF NOT EXISTS contains (\n"
      "\t-- inner/outer relations (file, namespace, dir, class, group, page)\n"
      "\trowid        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
      "\tinner_rowid  INTEGER NOT NULL REFERENCES compounddef,\n"
      "\touter_rowid  INTEGER NOT NULL REFERENCES compounddef\n"
    ");"
  },
  /* TODO: Path can also share rowids with refid/compounddef/def. (It could
   *       even collapse into that table...)
   *
   * I took a first swing at this by changing insertPath() to:
   * - accept a FileDef
   * - make its own call to insertRefid
   * - return a refid struct.
   *
   * I rolled this back when I had trouble getting a FileDef for all types
   * (PageDef in particular).
   *
   * Note: all columns referencing path would need an update.
   */
  { "path",
    "CREATE TABLE IF NOT EXISTS path (\n"
      "\t-- Paths of source files and includes.\n"
      "\trowid        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
      "\ttype         INTEGER NOT NULL, -- 1:file 2:dir\n"
      "\tlocal        INTEGER NOT NULL,\n"
      "\tfound        INTEGER NOT NULL,\n"
      "\tname         TEXT NOT NULL\n"
    ");"
  },
  { "refid",
    "CREATE TABLE IF NOT EXISTS refid (\n"
      "\t-- Distinct refid for all documented entities.\n"
      "\trowid        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
      "\trefid        TEXT NOT NULL UNIQUE\n"
    ");"
  },
  { "xrefs",
    "CREATE TABLE IF NOT EXISTS xrefs (\n"
      "\t-- Cross-reference relation\n"
      "\t-- (combines xml <referencedby> and <references> nodes).\n"
      "\trowid        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
      "\tsrc_rowid    INTEGER NOT NULL REFERENCES refid, -- referrer id.\n"
      "\tdst_rowid    INTEGER NOT NULL REFERENCES refid, -- referee id.\n"
      "\tcontext      TEXT NOT NULL, -- inline, argument, initializer\n"
      "\t-- Just need to know they link; ignore duplicates.\n"
      "\tUNIQUE(src_rowid, dst_rowid, context) ON CONFLICT IGNORE\n"
    ");\n"
  },
  { "memberdef",
    "CREATE TABLE IF NOT EXISTS memberdef (\n"
      "\t-- All processed identifiers.\n"
      "\trowid                INTEGER PRIMARY KEY NOT NULL,\n"
      "\tname                 TEXT NOT NULL,\n"
      "\tdefinition           TEXT,\n"
      "\ttype                 TEXT,\n"
      "\targsstring           TEXT,\n"
      "\tscope                TEXT,\n"
      "\tinitializer          TEXT,\n"
      "\tbitfield             TEXT,\n"
      "\tread                 TEXT,\n"
      "\twrite                TEXT,\n"
      "\tprot                 INTEGER DEFAULT 0, -- 0:public 1:protected 2:private 3:package\n"
      "\tstatic               INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\textern               INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\tconst                INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\texplicit             INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\tinline               INTEGER DEFAULT 0, -- 0:no 1:yes 2:both (set after encountering inline and not-inline)\n"
      "\tfinal                INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\tsealed               INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\tnew                  INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\toptional             INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\trequired             INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\tvolatile             INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\tvirt                 INTEGER DEFAULT 0, -- 0:no 1:virtual 2:pure-virtual\n"
      "\tmutable              INTEGER DEFAULT 0, -- 0:no 1:yes\n"
      "\tthread_local         INTEGER DEFAULT 0, -- 0:no 1:yes\n"
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
      "\tkind                 TEXT NOT NULL, -- 'macro definition' 'function' 'variable' 'typedef' 'enumeration' 'enumvalue' 'signal' 'slot' 'friend' 'dcop' 'property' 'event' 'interface' 'service'\n"
      "\tbodystart            INTEGER DEFAULT 0, -- starting line of definition\n"
      "\tbodyend              INTEGER DEFAULT 0, -- ending line of definition\n"
      "\tbodyfile_id          INTEGER REFERENCES path, -- file of definition\n"
      "\tfile_id              INTEGER NOT NULL REFERENCES path,  -- file where this identifier is located\n"
      "\tline                 INTEGER NOT NULL,  -- line where this identifier is located\n"
      "\tcolumn               INTEGER NOT NULL,  -- column where this identifier is located\n"
      "\tdetaileddescription  TEXT,\n"
      "\tbriefdescription     TEXT,\n"
      "\tinbodydescription    TEXT,\n"
      "\tFOREIGN KEY (rowid) REFERENCES refid (rowid)\n"
    ");"
  },
  { "member",
    "CREATE TABLE IF NOT EXISTS member (\n"
      "\t-- Memberdef <-> containing compound relation.\n"
      "\t-- Similar to XML listofallmembers.\n"
      "\trowid            INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
      "\tscope_rowid      INTEGER NOT NULL REFERENCES compounddef,\n"
      "\tmemberdef_rowid  INTEGER NOT NULL REFERENCES memberdef,\n"
      "\tprot             INTEGER NOT NULL,\n"
      "\tvirt             INTEGER NOT NULL,\n"
      "\tUNIQUE(scope_rowid, memberdef_rowid)\n"
    ");"
  },
  { "reimplements",
    "CREATE TABLE IF NOT EXISTS reimplements (\n"
      "\t-- Inherited member reimplementation relations.\n"
      "\trowid                  INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
      "\tmemberdef_rowid        INTEGER NOT NULL REFERENCES memberdef, -- reimplementing memberdef id.\n"
      "\treimplemented_rowid    INTEGER NOT NULL REFERENCES memberdef, -- reimplemented memberdef id.\n"
      "\tUNIQUE(memberdef_rowid, reimplemented_rowid) ON CONFLICT IGNORE\n"
    ");\n"
  },
  { "compounddef",
    "CREATE TABLE IF NOT EXISTS compounddef (\n"
      "\t-- Class/struct definitions.\n"
      "\trowid                INTEGER PRIMARY KEY NOT NULL,\n"
      "\tname                 TEXT NOT NULL,\n"
      "\ttitle                TEXT,\n"
      // probably won't be empty '' or unknown, but the source *could* return them...
      "\tkind                 TEXT NOT NULL, -- 'category' 'class' 'constants' 'dir' 'enum' 'example' 'exception' 'file' 'group' 'interface' 'library' 'module' 'namespace' 'package' 'page' 'protocol' 'service' 'singleton' 'struct' 'type' 'union' 'unknown' ''\n"
      "\tprot                 INTEGER,\n"
      "\tfile_id              INTEGER NOT NULL REFERENCES path,\n"
      "\tline                 INTEGER NOT NULL,\n"
      "\tcolumn               INTEGER NOT NULL,\n"
      "\theader_id            INTEGER REFERENCES path,\n"
      "\tdetaileddescription  TEXT,\n"
      "\tbriefdescription     TEXT,\n"
      "\tFOREIGN KEY (rowid) REFERENCES refid (rowid)\n"
    ");"
  },
  { "compoundref",
    "CREATE TABLE IF NOT EXISTS compoundref (\n"
      "\t-- Inheritance relation.\n"
      "\trowid          INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
      "\tbase_rowid     INTEGER NOT NULL REFERENCES compounddef,\n"
      "\tderived_rowid  INTEGER NOT NULL REFERENCES compounddef,\n"
      "\tprot           INTEGER NOT NULL,\n"
      "\tvirt           INTEGER NOT NULL,\n"
      "\tUNIQUE(base_rowid, derived_rowid)\n"
    ");"
  },
  { "param",
    "CREATE TABLE IF NOT EXISTS param (\n"
      "\t-- All processed parameters.\n"
      "\trowid        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
      "\tattributes   TEXT,\n"
      "\ttype         TEXT,\n"
      "\tdeclname     TEXT,\n"
      "\tdefname      TEXT,\n"
      "\tarray        TEXT,\n"
      "\tdefval       TEXT,\n"
      "\tbriefdescription TEXT\n"
    ");"
    "CREATE UNIQUE INDEX idx_param ON param\n"
      "\t(type, defname);"
  },
  { "memberdef_param",
    "CREATE TABLE IF NOT EXISTS memberdef_param (\n"
      "\t-- Junction table for memberdef parameters.\n"
      "\trowid        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
      "\tmemberdef_id INTEGER NOT NULL REFERENCES memberdef,\n"
      "\tparam_id     INTEGER NOT NULL REFERENCES param\n"
    ");"
  },
};
  const char * view_schema[][2] = {
  /* VIEWS *
  We'll set these up AFTER we build the database, so that they can be indexed,
  but so we don't have to pay a performance penalty for inserts as we build.
  */
  {
    /*
    Makes all reference/relation tables easier to use. For example:
    1. query xrefs and join this view on either xrefs.dst_rowid=def.rowid or
       xrefs.src_rowid=def.rowid
    2. get everything you need to output a list of references to/from an entity

    Also supports simple name search/lookup for both compound and member types.

    NOTES:
    - summary for compounds generalizes title and briefdescription because
      there's no single field that works as a quick introduction for both
      pages and classes
    - May be value in eventually extending this to fulltext or levenshtein
      distance-driven lookup/search, but I'm avoiding these for now as it
      takes some effort to enable them.
    */
    "def",
    "CREATE VIEW IF NOT EXISTS def (\n"
      "\t-- Combined summary of all -def types for easier joins.\n"
      "\trowid,\n"
      "\trefid,\n"
      "\tkind,\n"
      "\tname,\n"
      "\tsummary"
    ")\n"
    "as SELECT \n"
      "\trefid.rowid,\n"
      "\trefid.refid,\n"
      "\tmemberdef.kind,\n"
      "\tmemberdef.name,\n"
      "\tmemberdef.briefdescription \n"
    "FROM refid \n"
    "JOIN memberdef ON refid.rowid=memberdef.rowid \n"
    "UNION ALL \n"
    "SELECT \n"
      "\trefid.rowid,\n"
      "\trefid.refid,\n"
      "\tcompounddef.kind,\n"
      "\tcompounddef.name,\n"
      "\tCASE \n"
        "\t\tWHEN briefdescription IS NOT NULL \n"
        "\t\tTHEN briefdescription \n"
        "\t\tELSE title \n"
      "\tEND summary\n"
    "FROM refid \n"
    "JOIN compounddef ON refid.rowid=compounddef.rowid;"
  },
  {
    "local_file",
    "CREATE VIEW IF NOT EXISTS local_file (\n"
      "\t-- File paths found within the project.\n"
      "\trowid,\n"
      "\tfound,\n"
      "\tname\n"
    ")\n"
    "as SELECT \n"
      "\tpath.rowid,\n"
      "\tpath.found,\n"
      "\tpath.name\n"
    "FROM path WHERE path.type=1 AND path.local=1 AND path.found=1;\n"
  },
  {
    "external_file",
    "CREATE VIEW IF NOT EXISTS external_file (\n"
      "\t-- File paths outside the project (found or not).\n"
      "\trowid,\n"
      "\tfound,\n"
      "\tname\n"
    ")\n"
    "as SELECT \n"
      "\tpath.rowid,\n"
      "\tpath.found,\n"
      "\tpath.name\n"
    "FROM path WHERE path.type=1 AND path.local=0;\n"
  },
  {
    "inline_xrefs",
    "CREATE VIEW IF NOT EXISTS inline_xrefs (\n"
      "\t-- Crossrefs from inline member source.\n"
      "\trowid,\n"
      "\tsrc_rowid,\n"
      "\tdst_rowid\n"
    ")\n"
    "as SELECT \n"
      "\txrefs.rowid,\n"
      "\txrefs.src_rowid,\n"
      "\txrefs.dst_rowid\n"
    "FROM xrefs WHERE xrefs.context='inline';\n"
  },
  {
    "argument_xrefs",
    "CREATE VIEW IF NOT EXISTS argument_xrefs (\n"
      "\t-- Crossrefs from member def/decl arguments\n"
      "\trowid,\n"
      "\tsrc_rowid,\n"
      "\tdst_rowid\n"
    ")\n"
    "as SELECT \n"
      "\txrefs.rowid,\n"
      "\txrefs.src_rowid,\n"
      "\txrefs.dst_rowid\n"
    "FROM xrefs WHERE xrefs.context='argument';\n"
  },
  {
    "initializer_xrefs",
    "CREATE VIEW IF NOT EXISTS initializer_xrefs (\n"
      "\t-- Crossrefs from member initializers\n"
      "\trowid,\n"
      "\tsrc_rowid,\n"
      "\tdst_rowid\n"
    ")\n"
    "as SELECT \n"
      "\txrefs.rowid,\n"
      "\txrefs.src_rowid,\n"
      "\txrefs.dst_rowid\n"
    "FROM xrefs WHERE xrefs.context='initializer';\n"
  },
  {
    "inner_outer",
    "CREATE VIEW IF NOT EXISTS inner_outer\n"
    "\t-- Joins 'contains' relations to simplify inner/outer 'rel' queries.\n"
    "as SELECT \n"
      "\tinner.*,\n"
      "\touter.*\n"
    "FROM def as inner\n"
      "\tJOIN contains ON inner.rowid=contains.inner_rowid\n"
      "\tJOIN def AS outer ON outer.rowid=contains.outer_rowid;\n"
  },
  {
    "rel",
    "CREATE VIEW IF NOT EXISTS rel (\n"
      "\t-- Boolean indicator of relations available for a given entity.\n"
      "\t-- Join to (compound-|member-)def to find fetch-worthy relations.\n"
      "\trowid,\n"
      "\treimplemented,\n"
      "\treimplements,\n"
      "\tinnercompounds,\n"
      "\toutercompounds,\n"
      "\tinnerpages,\n"
      "\touterpages,\n"
      "\tinnerdirs,\n"
      "\touterdirs,\n"
      "\tinnerfiles,\n"
      "\touterfiles,\n"
      "\tinnerclasses,\n"
      "\touterclasses,\n"
      "\tinnernamespaces,\n"
      "\touternamespaces,\n"
      "\tinnergroups,\n"
      "\toutergroups,\n"
      "\tmembers,\n"
      "\tcompounds,\n"
      "\tsubclasses,\n"
      "\tsuperclasses,\n"
      "\tlinks_in,\n"
      "\tlinks_out,\n"
      "\targument_links_in,\n"
      "\targument_links_out,\n"
      "\tinitializer_links_in,\n"
      "\tinitializer_links_out\n"
    ")\n"
    "as SELECT \n"
      "\tdef.rowid,\n"
      "\tEXISTS (SELECT rowid FROM reimplements WHERE reimplemented_rowid=def.rowid),\n"
      "\tEXISTS (SELECT rowid FROM reimplements WHERE memberdef_rowid=def.rowid),\n"
      "\t-- rowid/kind for inner, [rowid:1/kind:1] for outer\n"
      "\tEXISTS (SELECT * FROM inner_outer WHERE [rowid:1]=def.rowid),\n"
      "\tEXISTS (SELECT * FROM inner_outer WHERE rowid=def.rowid),\n"
      "\tEXISTS (SELECT * FROM inner_outer WHERE [rowid:1]=def.rowid AND kind='page'),\n"
      "\tEXISTS (SELECT * FROM inner_outer WHERE rowid=def.rowid AND [kind:1]='page'),\n"
      "\tEXISTS (SELECT * FROM inner_outer WHERE [rowid:1]=def.rowid AND kind='dir'),\n"
      "\tEXISTS (SELECT * FROM inner_outer WHERE rowid=def.rowid AND [kind:1]='dir'),\n"
      "\tEXISTS (SELECT * FROM inner_outer WHERE [rowid:1]=def.rowid AND kind='file'),\n"
      "\tEXISTS (SELECT * FROM inner_outer WHERE rowid=def.rowid AND [kind:1]='file'),\n"
      "\tEXISTS (SELECT * FROM inner_outer WHERE [rowid:1]=def.rowid AND kind in (\n"
        "'category','class','enum','exception','interface','module','protocol',\n"
        "'service','singleton','struct','type','union'\n"
      ")),\n"
      "\tEXISTS (SELECT * FROM inner_outer WHERE rowid=def.rowid AND [kind:1] in (\n"
        "'category','class','enum','exception','interface','module','protocol',\n"
        "'service','singleton','struct','type','union'\n"
      ")),\n"
      "\tEXISTS (SELECT * FROM inner_outer WHERE [rowid:1]=def.rowid AND kind='namespace'),\n"
      "\tEXISTS (SELECT * FROM inner_outer WHERE rowid=def.rowid AND [kind:1]='namespace'),\n"
      "\tEXISTS (SELECT * FROM inner_outer WHERE [rowid:1]=def.rowid AND kind='group'),\n"
      "\tEXISTS (SELECT * FROM inner_outer WHERE rowid=def.rowid AND [kind:1]='group'),\n"
      "\tEXISTS (SELECT rowid FROM member WHERE scope_rowid=def.rowid),\n"
      "\tEXISTS (SELECT rowid FROM member WHERE memberdef_rowid=def.rowid),\n"
      "\tEXISTS (SELECT rowid FROM compoundref WHERE base_rowid=def.rowid),\n"
      "\tEXISTS (SELECT rowid FROM compoundref WHERE derived_rowid=def.rowid),\n"
      "\tEXISTS (SELECT rowid FROM inline_xrefs WHERE dst_rowid=def.rowid),\n"
      "\tEXISTS (SELECT rowid FROM inline_xrefs WHERE src_rowid=def.rowid),\n"
      "\tEXISTS (SELECT rowid FROM argument_xrefs WHERE dst_rowid=def.rowid),\n"
      "\tEXISTS (SELECT rowid FROM argument_xrefs WHERE src_rowid=def.rowid),\n"
      "\tEXISTS (SELECT rowid FROM initializer_xrefs WHERE dst_rowid=def.rowid),\n"
      "\tEXISTS (SELECT rowid FROM initializer_xrefs WHERE src_rowid=def.rowid)\n"
    "FROM def ORDER BY def.rowid;"
  }
};

//////////////////////////////////////////////////////
struct SqlStmt {
  const char   *query = nullptr;
  sqlite3_stmt *stmt = nullptr;
  sqlite3 *db = nullptr;
};
//////////////////////////////////////////////////////
/* If you add a new statement below, make sure to add it to
   prepareStatements(). If sqlite3 is segfaulting (especially in
   sqlite3_clear_bindings()), using an un-prepared statement may
   be the cause. */
SqlStmt meta_insert = {
  "INSERT INTO meta "
    "( doxygen_version, schema_version, generated_at, generated_on, project_name, project_number, project_brief )"
  "VALUES "
    "(:doxygen_version,:schema_version,:generated_at,:generated_on,:project_name,:project_number,:project_brief )"
  ,nullptr
};
//////////////////////////////////////////////////////
SqlStmt incl_insert = {
  "INSERT INTO includes "
    "( local, src_id, dst_id ) "
  "VALUES "
    "(:local,:src_id,:dst_id )"
  ,nullptr
};
SqlStmt incl_select = {
  "SELECT COUNT(*) FROM includes WHERE "
  "local=:local AND src_id=:src_id AND dst_id=:dst_id"
  ,nullptr
};
//////////////////////////////////////////////////////
SqlStmt contains_insert={
  "INSERT INTO contains "
    "( inner_rowid, outer_rowid )"
  "VALUES "
    "(:inner_rowid,:outer_rowid )"
  ,nullptr
};
//////////////////////////////////////////////////////
SqlStmt path_select = {
  "SELECT rowid FROM path WHERE name=:name"
  ,nullptr
};
SqlStmt path_insert = {
  "INSERT INTO path "
    "( type, local, found, name )"
  "VALUES "
    "(:type,:local,:found,:name )"
  ,nullptr
};
//////////////////////////////////////////////////////
SqlStmt refid_select =  {
  "SELECT rowid FROM refid WHERE refid=:refid"
  ,nullptr
};
SqlStmt refid_insert = {
  "INSERT INTO refid "
    "( refid )"
  "VALUES "
    "(:refid )"
  ,nullptr
};
//////////////////////////////////////////////////////
SqlStmt xrefs_insert= {
  "INSERT INTO xrefs "
    "( src_rowid, dst_rowid, context )"
  "VALUES "
    "(:src_rowid,:dst_rowid,:context )"
  ,nullptr
};//////////////////////////////////////////////////////
SqlStmt reimplements_insert= {
  "INSERT INTO reimplements "
    "( memberdef_rowid, reimplemented_rowid )"
  "VALUES "
    "(:memberdef_rowid,:reimplemented_rowid )"
  ,nullptr
};
//////////////////////////////////////////////////////
SqlStmt memberdef_exists={
  "SELECT EXISTS (SELECT * FROM memberdef WHERE rowid = :rowid)"
  ,nullptr
};

SqlStmt memberdef_incomplete={
  "SELECT EXISTS ("
    "SELECT * FROM memberdef WHERE "
    "rowid = :rowid AND inline != 2 AND inline != :new_inline"
  ")"
  ,nullptr
};

SqlStmt memberdef_insert={
  "INSERT INTO memberdef "
  "("
    "rowid,"
    "name,"
    "definition,"
    "type,"
    "argsstring,"
    "scope,"
    "initializer,"
    "bitfield,"
    "read,"
    "write,"
    "prot,"
    "static,"
    "extern,"
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
    "thread_local,"
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
    "bodyfile_id,"
    "file_id,"
    "line,"
    "column,"
    "detaileddescription,"
    "briefdescription,"
    "inbodydescription"
  ")"
  "VALUES "
  "("
    ":rowid,"
    ":name,"
    ":definition,"
    ":type,"
    ":argsstring,"
    ":scope,"
    ":initializer,"
    ":bitfield,"
    ":read,"
    ":write,"
    ":prot,"
    ":static,"
    ":extern,"
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
    ":thread_local,"
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
    ":protectedsettable,"
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
    ":bodyfile_id,"
    ":file_id,"
    ":line,"
    ":column,"
    ":detaileddescription,"
    ":briefdescription,"
    ":inbodydescription"
  ")"
  ,nullptr
};
/*
We have a slightly different need than the XML here. The XML can have two
memberdef nodes with the same refid to document the declaration and the
definition. This doesn't play very nice with a referential model. It isn't a
big issue if only one is documented, but in case both are, we'll fall back on
this kludge to combine them in a single row...
*/
SqlStmt memberdef_update_decl={
  "UPDATE memberdef SET "
    "inline = :inline,"
    "file_id = :file_id,"
    "line = :line,"
    "column = :column,"
    "detaileddescription = 'Declaration: ' || :detaileddescription || 'Definition: ' || detaileddescription,"
    "briefdescription = 'Declaration: ' || :briefdescription || 'Definition: ' || briefdescription,"
    "inbodydescription = 'Declaration: ' || :inbodydescription || 'Definition: ' || inbodydescription "
  "WHERE rowid = :rowid"
  ,nullptr
};
SqlStmt memberdef_update_def={
  "UPDATE memberdef SET "
    "inline = :inline,"
    "bodystart = :bodystart,"
    "bodyend = :bodyend,"
    "bodyfile_id = :bodyfile_id,"
    "detaileddescription = 'Declaration: ' || detaileddescription || 'Definition: ' || :detaileddescription,"
    "briefdescription = 'Declaration: ' || briefdescription || 'Definition: ' || :briefdescription,"
    "inbodydescription = 'Declaration: ' || inbodydescription || 'Definition: ' || :inbodydescription "
  "WHERE rowid = :rowid"
  ,nullptr
};
//////////////////////////////////////////////////////
SqlStmt member_insert={
  "INSERT INTO member "
    "( scope_rowid, memberdef_rowid, prot, virt ) "
  "VALUES "
    "(:scope_rowid,:memberdef_rowid,:prot,:virt )"
  ,nullptr
};
//////////////////////////////////////////////////////
SqlStmt compounddef_insert={
  "INSERT INTO compounddef "
  "("
    "rowid,"
    "name,"
    "title,"
    "kind,"
    "prot,"
    "file_id,"
    "line,"
    "column,"
    "header_id,"
    "briefdescription,"
    "detaileddescription"
  ")"
  "VALUES "
  "("
    ":rowid,"
    ":name,"
    ":title,"
    ":kind,"
    ":prot,"
    ":file_id,"
    ":line,"
    ":column,"
    ":header_id,"
    ":briefdescription,"
    ":detaileddescription"
  ")"
  ,nullptr
};
SqlStmt compounddef_exists={
  "SELECT EXISTS ("
    "SELECT * FROM compounddef WHERE rowid = :rowid"
  ")"
  ,nullptr
};
//////////////////////////////////////////////////////
SqlStmt compoundref_insert={
  "INSERT INTO compoundref "
    "( base_rowid, derived_rowid, prot, virt ) "
  "VALUES "
    "(:base_rowid,:derived_rowid,:prot,:virt )"
  ,nullptr
};
//////////////////////////////////////////////////////
SqlStmt param_select = {
  "SELECT rowid FROM param WHERE "
    "(attributes IS NULL OR attributes=:attributes) AND "
    "(type IS NULL OR type=:type) AND "
    "(declname IS NULL OR declname=:declname) AND "
    "(defname IS NULL OR defname=:defname) AND "
    "(array IS NULL OR array=:array) AND "
    "(defval IS NULL OR defval=:defval) AND "
    "(briefdescription IS NULL OR briefdescription=:briefdescription)"
  ,nullptr
};
SqlStmt param_insert = {
  "INSERT INTO param "
    "( attributes, type, declname, defname, array, defval, briefdescription ) "
  "VALUES "
    "(:attributes,:type,:declname,:defname,:array,:defval,:briefdescription)"
  ,nullptr
};
//////////////////////////////////////////////////////
SqlStmt memberdef_param_insert={
  "INSERT INTO memberdef_param "
    "( memberdef_id, param_id)"
  "VALUES "
    "(:memberdef_id,:param_id)"
  ,nullptr
};

class TextGeneratorSqlite3Impl : public TextGeneratorIntf
{
  public:
    TextGeneratorSqlite3Impl(StringVector &l) : m_list(l) { }
    void writeString(std::string_view /*s*/,bool /*keepSpaces*/) const override
    {
    }
    void writeBreak(int) const override
    {
      DBG_CTX(("writeBreak\n"));
    }
    void writeLink(const QCString & /*extRef*/,const QCString &file,
                   const QCString &anchor,std::string_view /*text*/
                  ) const override
    {
      std::string rs = file.str();
      if (!anchor.isEmpty())
      {
        rs+="_1";
        rs+=anchor.str();
      }
      m_list.push_back(rs);
    }
  private:
    StringVector &m_list;
    // the list is filled by linkifyText and consumed by the caller
};


static bool bindTextParameter(SqlStmt &s,const char *name,const QCString &value)
{
  int idx = sqlite3_bind_parameter_index(s.stmt, name);
  if (idx==0) {
    err("sqlite3_bind_parameter_index({})[{}] failed: {}\n", name, s.query, sqlite3_errmsg(s.db));
    return false;
  }
  int rv = sqlite3_bind_text(s.stmt, idx, value.data(), -1, SQLITE_TRANSIENT);
  if (rv!=SQLITE_OK) {
    err("sqlite3_bind_text({})[{}] failed: {}\n", name, s.query, sqlite3_errmsg(s.db));
    return false;
  }
  return true;
}

static bool bindIntParameter(SqlStmt &s,const char *name,int value)
{
  int idx = sqlite3_bind_parameter_index(s.stmt, name);
  if (idx==0) {
    err("sqlite3_bind_parameter_index({})[{}] failed to find column: {}\n", name, s.query, sqlite3_errmsg(s.db));
    return false;
  }
  int rv = sqlite3_bind_int(s.stmt, idx, value);
  if (rv!=SQLITE_OK) {
    err("sqlite3_bind_int({})[{}] failed: {}\n", name, s.query, sqlite3_errmsg(s.db));
    return false;
  }
  return true;
}

static int step(SqlStmt &s,bool getRowId=FALSE, bool select=FALSE)
{
  int rowid=-1;
  int rc = sqlite3_step(s.stmt);
  if (rc!=SQLITE_DONE && rc!=SQLITE_ROW)
  {
    DBG_CTX(("sqlite3_step: %s (rc: %d)\n", sqlite3_errmsg(s.db), rc));
    sqlite3_reset(s.stmt);
    sqlite3_clear_bindings(s.stmt);
    return -1;
  }
  if (getRowId && select) rowid = sqlite3_column_int(s.stmt, 0); // works on selects, doesn't on inserts
  if (getRowId && !select) rowid = static_cast<int>(sqlite3_last_insert_rowid(s.db)); //works on inserts, doesn't on selects
  sqlite3_reset(s.stmt);
  sqlite3_clear_bindings(s.stmt); // XXX When should this really be called
  return rowid;
}

static int insertPath(QCString name, bool local=TRUE, bool found=TRUE, int type=1)
{
  int rowid=-1;
  if (name==nullptr) return rowid;

  name = stripFromPath(name);

  bindTextParameter(path_select,":name",name.data());
  rowid=step(path_select,TRUE,TRUE);
  if (rowid==0)
  {
    bindTextParameter(path_insert,":name",name.data());
    bindIntParameter(path_insert,":type",type);
    bindIntParameter(path_insert,":local",local?1:0);
    bindIntParameter(path_insert,":found",found?1:0);
    rowid=step(path_insert,TRUE);
  }
  return rowid;
}

static void recordMetadata()
{
  bindTextParameter(meta_insert,":doxygen_version",getFullVersion());
  bindTextParameter(meta_insert,":schema_version","0.2.1"); //TODO: this should be a constant somewhere; not sure where
  bindTextParameter(meta_insert,":generated_at",dateToString(DateTimeType::DateTime));
  bindTextParameter(meta_insert,":generated_on",dateToString(DateTimeType::Date));
  bindTextParameter(meta_insert,":project_name",Config_getString(PROJECT_NAME));
  bindTextParameter(meta_insert,":project_number",Config_getString(PROJECT_NUMBER));
  bindTextParameter(meta_insert,":project_brief",Config_getString(PROJECT_BRIEF));
  step(meta_insert);
}

struct Refid {
  int rowid;
  QCString refid;
  bool created;
};

struct Refid insertRefid(const QCString &refid)
{
  Refid ret;
  ret.rowid=-1;
  ret.refid=refid;
  ret.created = FALSE;
  if (refid.isEmpty()) return ret;

  bindTextParameter(refid_select,":refid",refid);
  ret.rowid=step(refid_select,TRUE,TRUE);
  if (ret.rowid==0)
  {
    bindTextParameter(refid_insert,":refid",refid);
    ret.rowid=step(refid_insert,TRUE);
    ret.created = TRUE;
  }

  return ret;
}

static bool memberdefExists(struct Refid refid)
{
  bindIntParameter(memberdef_exists,":rowid",refid.rowid);
  int test = step(memberdef_exists,TRUE,TRUE);
  return test ? true : false;
}

static bool memberdefIncomplete(struct Refid refid, const MemberDef* md)
{
  bindIntParameter(memberdef_incomplete,":rowid",refid.rowid);
  bindIntParameter(memberdef_incomplete,":new_inline",md->isInline());
  int test = step(memberdef_incomplete,TRUE,TRUE);
  return test ? true : false;
}

static bool compounddefExists(struct Refid refid)
{
  bindIntParameter(compounddef_exists,":rowid",refid.rowid);
  int test = step(compounddef_exists,TRUE,TRUE);
  return test ? true : false;
}

static bool insertMemberReference(struct Refid src_refid, struct Refid dst_refid, const char *context)
{
  if (src_refid.rowid==-1||dst_refid.rowid==-1)
    return false;

  if (
     !bindIntParameter(xrefs_insert,":src_rowid",src_refid.rowid) ||
     !bindIntParameter(xrefs_insert,":dst_rowid",dst_refid.rowid)
     )
  {
    return false;
  }
  else
  {
    bindTextParameter(xrefs_insert,":context",context);
  }

  step(xrefs_insert);
  return true;
}

static void insertMemberReference(const MemberDef *src, const MemberDef *dst, const char *context)
{
  QCString qdst_refid = dst->getOutputFileBase() + "_1" + dst->anchor();
  QCString qsrc_refid = src->getOutputFileBase() + "_1" + src->anchor();

  struct Refid src_refid = insertRefid(qsrc_refid);
  struct Refid dst_refid = insertRefid(qdst_refid);
  insertMemberReference(src_refid,dst_refid,context);
}

static void insertMemberFunctionParams(int memberdef_id, const MemberDef *md, const Definition *def)
{
  const ArgumentList &declAl = md->declArgumentList();
  const ArgumentList &defAl = md->argumentList();
  if (declAl.size()>0)
  {
    auto defIt = defAl.begin();
    for (const Argument &a : declAl)
    {
      //const Argument *defArg = defAli.current();
      const Argument *defArg = nullptr;
      if (defIt!=defAl.end())
      {
        defArg = &(*defIt);
        ++defIt;
      }

      if (!a.attrib.isEmpty())
      {
        bindTextParameter(param_select,":attributes",a.attrib);
        bindTextParameter(param_insert,":attributes",a.attrib);
      }
      if (!a.type.isEmpty())
      {
        StringVector list;
        linkifyText(TextGeneratorSqlite3Impl(list),def,md->getBodyDef(),md,a.type);

        for (const auto &s : list)
        {
          QCString qsrc_refid = md->getOutputFileBase() + "_1" + md->anchor();
          struct Refid src_refid = insertRefid(qsrc_refid);
          struct Refid dst_refid = insertRefid(s);
          insertMemberReference(src_refid,dst_refid, "argument");
        }
        bindTextParameter(param_select,":type",a.type);
        bindTextParameter(param_insert,":type",a.type);
      }
      if (!a.name.isEmpty())
      {
        bindTextParameter(param_select,":declname",a.name);
        bindTextParameter(param_insert,":declname",a.name);
      }
      if (defArg && !defArg->name.isEmpty() && defArg->name!=a.name)
      {
        bindTextParameter(param_select,":defname",defArg->name);
        bindTextParameter(param_insert,":defname",defArg->name);
      }
      if (!a.array.isEmpty())
      {
        bindTextParameter(param_select,":array",a.array);
        bindTextParameter(param_insert,":array",a.array);
      }
      if (!a.defval.isEmpty())
      {
        StringVector list;
        linkifyText(TextGeneratorSqlite3Impl(list),def,md->getBodyDef(),md,a.defval);
        bindTextParameter(param_select,":defval",a.defval);
        bindTextParameter(param_insert,":defval",a.defval);
      }

      int param_id=step(param_select,TRUE,TRUE);
      if (param_id==0) {
        param_id=step(param_insert,TRUE);
      }
      if (param_id==-1) {
          DBG_CTX(("error INSERT params failed\n"));
          continue;
      }

      bindIntParameter(memberdef_param_insert,":memberdef_id",memberdef_id);
      bindIntParameter(memberdef_param_insert,":param_id",param_id);
      step(memberdef_param_insert);
    }
  }
}

static void insertMemberDefineParams(int memberdef_id,const MemberDef *md, const Definition *def)
{
    if (md->argumentList().empty()) // special case for "foo()" to
                                    // distinguish it from "foo".
    {
      DBG_CTX(("no params\n"));
    }
    else
    {
      for (const Argument &a : md->argumentList())
      {
        bindTextParameter(param_insert,":defname",a.type);
        int param_id=step(param_insert,TRUE);
        if (param_id==-1) {
          continue;
        }

        bindIntParameter(memberdef_param_insert,":memberdef_id",memberdef_id);
        bindIntParameter(memberdef_param_insert,":param_id",param_id);
        step(memberdef_param_insert);
      }
    }
}

static void associateMember(const MemberDef *md, struct Refid member_refid, struct Refid scope_refid)
{
  // TODO: skip EnumValue only to guard against recording refids and member records
  // for enumvalues until we can support documenting them as entities.
  if (md->memberType()==MemberType::EnumValue) return;
  if (!md->isAnonymous()) // skip anonymous members
  {
    bindIntParameter(member_insert, ":scope_rowid", scope_refid.rowid);
    bindIntParameter(member_insert, ":memberdef_rowid", member_refid.rowid);

    bindIntParameter(member_insert, ":prot", static_cast<int>(md->protection()));
    bindIntParameter(member_insert, ":virt", static_cast<int>(md->virtualness()));
    step(member_insert);
  }
}

static void stripQualifiers(QCString &typeStr)
{
  bool done=FALSE;
  while (!done)
  {
    if      (typeStr.stripPrefix("static "));
    else if (typeStr.stripPrefix("virtual "));
    else if (typeStr=="virtual") typeStr="";
    else done=TRUE;
  }
}

static int prepareStatement(sqlite3 *db, SqlStmt &s)
{
  int rc = sqlite3_prepare_v2(db,s.query,-1,&s.stmt,nullptr);
  if (rc!=SQLITE_OK)
  {
    err("prepare failed for:\n  {}\n  {}\n", s.query, sqlite3_errmsg(db));
    s.db = nullptr;
    return -1;
  }
  s.db = db;
  return rc;
}

static int prepareStatements(sqlite3 *db)
{
  if (
  -1==prepareStatement(db, meta_insert) ||
  -1==prepareStatement(db, memberdef_exists) ||
  -1==prepareStatement(db, memberdef_incomplete) ||
  -1==prepareStatement(db, memberdef_insert) ||
  -1==prepareStatement(db, memberdef_update_def) ||
  -1==prepareStatement(db, memberdef_update_decl) ||
  -1==prepareStatement(db, member_insert) ||
  -1==prepareStatement(db, path_insert) ||
  -1==prepareStatement(db, path_select) ||
  -1==prepareStatement(db, refid_insert) ||
  -1==prepareStatement(db, refid_select) ||
  -1==prepareStatement(db, incl_insert)||
  -1==prepareStatement(db, incl_select)||
  -1==prepareStatement(db, param_insert) ||
  -1==prepareStatement(db, param_select) ||
  -1==prepareStatement(db, xrefs_insert) ||
  -1==prepareStatement(db, reimplements_insert) ||
  -1==prepareStatement(db, contains_insert) ||
  -1==prepareStatement(db, compounddef_exists) ||
  -1==prepareStatement(db, compounddef_insert) ||
  -1==prepareStatement(db, compoundref_insert) ||
  -1==prepareStatement(db, memberdef_param_insert)
  )
  {
    return -1;
  }
  return 0;
}

static void beginTransaction(sqlite3 *db)
{
  char * sErrMsg = nullptr;
  sqlite3_exec(db, "BEGIN TRANSACTION", nullptr, nullptr, &sErrMsg);
}

static void endTransaction(sqlite3 *db)
{
  char * sErrMsg = nullptr;
  sqlite3_exec(db, "END TRANSACTION", nullptr, nullptr, &sErrMsg);
}

static void pragmaTuning(sqlite3 *db)
{
  char * sErrMsg = nullptr;
  sqlite3_exec(db, "PRAGMA synchronous = OFF", nullptr, nullptr, &sErrMsg);
  sqlite3_exec(db, "PRAGMA journal_mode = MEMORY", nullptr, nullptr, &sErrMsg);
  sqlite3_exec(db, "PRAGMA temp_store = MEMORY;", nullptr, nullptr, &sErrMsg);
}

static int initializeTables(sqlite3* db)
{
  msg("Initializing DB schema (tables)...\n");
  for (unsigned int k = 0; k < sizeof(table_schema) / sizeof(table_schema[0]); k++)
  {
    const char *q = table_schema[k][1];
    char *errmsg = nullptr;
    int rc = sqlite3_exec(db, q, nullptr, nullptr, &errmsg);
    if (rc != SQLITE_OK)
    {
      err("failed to execute query: {}\n\t{}\n", q, errmsg);
      return -1;
    }
  }
  return 0;
}

static int initializeViews(sqlite3* db)
{
  msg("Initializing DB schema (views)...\n");
  for (unsigned int k = 0; k < sizeof(view_schema) / sizeof(view_schema[0]); k++)
  {
    const char *q = view_schema[k][1];
    char *errmsg = nullptr;
    int rc = sqlite3_exec(db, q, nullptr, nullptr, &errmsg);
    if (rc != SQLITE_OK)
    {
      err("failed to execute query: {}\n\t{}\n", q, errmsg);
      return -1;
    }
  }
  return 0;
}

////////////////////////////////////////////
/* TODO:
I collapsed all innerX tables into 'contains', which raises the prospect that
all of these very similar writeInnerX funcs could be refactored into a one,
or a small set of common parts.

I think the hurdles are:
- picking a first argument that every call location can pass
- which yields a consistent iterator
- accommodates PageDef's slightly different rules for generating the
  inner_refid (unless I'm missing a method that would uniformly return
  the correct refid for all types).
*/
static void writeInnerClasses(const ClassLinkedRefMap &cl, struct Refid outer_refid)
{
  for (const auto &cd : cl)
  {
    if (!cd->isHidden() && !cd->isAnonymous())
    {
      struct Refid inner_refid = insertRefid(cd->getOutputFileBase());

      bindIntParameter(contains_insert,":inner_rowid", inner_refid.rowid);
      bindIntParameter(contains_insert,":outer_rowid", outer_refid.rowid);
      step(contains_insert);
    }
  }
}

static void writeInnerConcepts(const ConceptLinkedRefMap &cl, struct Refid outer_refid)
{
  for (const auto &cd : cl)
  {
    struct Refid inner_refid = insertRefid(cd->getOutputFileBase());

    bindIntParameter(contains_insert,":inner_rowid", inner_refid.rowid);
    bindIntParameter(contains_insert,":outer_rowid", outer_refid.rowid);
    step(contains_insert);
  }
}

static void writeInnerModules(const ModuleLinkedRefMap &ml, struct Refid outer_refid)
{
  for (const auto &mod : ml)
  {
    struct Refid inner_refid = insertRefid(mod->getOutputFileBase());

    bindIntParameter(contains_insert,":inner_rowid", inner_refid.rowid);
    bindIntParameter(contains_insert,":outer_rowid", outer_refid.rowid);
    step(contains_insert);
  }
}


static void writeInnerPages(const PageLinkedRefMap &pl, struct Refid outer_refid)
{
  for (const auto &pd : pl)
  {
    struct Refid inner_refid = insertRefid(
      pd->getGroupDef() ? pd->getOutputFileBase()+"_"+pd->name() : pd->getOutputFileBase()
    );

    bindIntParameter(contains_insert,":inner_rowid", inner_refid.rowid);
    bindIntParameter(contains_insert,":outer_rowid", outer_refid.rowid);
    step(contains_insert);
  }
}

static void writeInnerGroups(const GroupList &gl, struct Refid outer_refid)
{
  for (const auto &sgd : gl)
  {
    struct Refid inner_refid = insertRefid(sgd->getOutputFileBase());

    bindIntParameter(contains_insert,":inner_rowid", inner_refid.rowid);
    bindIntParameter(contains_insert,":outer_rowid", outer_refid.rowid);
    step(contains_insert);
  }
}

static void writeInnerFiles(const FileList &fl, struct Refid outer_refid)
{
  for (const auto &fd: fl)
  {
    struct Refid inner_refid = insertRefid(fd->getOutputFileBase());

    bindIntParameter(contains_insert,":inner_rowid", inner_refid.rowid);
    bindIntParameter(contains_insert,":outer_rowid", outer_refid.rowid);
    step(contains_insert);
  }
}

static void writeInnerDirs(const DirList &dl, struct Refid outer_refid)
{
  for (const auto subdir : dl)
  {
    struct Refid inner_refid = insertRefid(subdir->getOutputFileBase());

    bindIntParameter(contains_insert,":inner_rowid", inner_refid.rowid);
    bindIntParameter(contains_insert,":outer_rowid", outer_refid.rowid);
    step(contains_insert);
  }
}

static void writeInnerNamespaces(const NamespaceLinkedRefMap &nl, struct Refid outer_refid)
{
  for (const auto &nd : nl)
  {
    if (!nd->isHidden() && !nd->isAnonymous())
    {
      struct Refid inner_refid = insertRefid(nd->getOutputFileBase());

      bindIntParameter(contains_insert,":inner_rowid",inner_refid.rowid);
      bindIntParameter(contains_insert,":outer_rowid",outer_refid.rowid);
      step(contains_insert);
    }
  }
}


static void writeTemplateArgumentList(const ArgumentList &al,
                                      const Definition * scope,
                                      const FileDef * fileScope)
{
  for (const Argument &a : al)
  {
    if (!a.type.isEmpty())
    {
//#warning linkifyText(TextGeneratorXMLImpl(t),scope,fileScope,0,a.type);
      bindTextParameter(param_select,":type",a.type);
      bindTextParameter(param_insert,":type",a.type);
    }
    if (!a.name.isEmpty())
    {
      bindTextParameter(param_select,":declname",a.name);
      bindTextParameter(param_insert,":declname",a.name);
      bindTextParameter(param_select,":defname",a.name);
      bindTextParameter(param_insert,":defname",a.name);
    }
    if (!a.defval.isEmpty())
    {
//#warning linkifyText(TextGeneratorXMLImpl(t),scope,fileScope,0,a.defval);
      bindTextParameter(param_select,":defval",a.defval);
      bindTextParameter(param_insert,":defval",a.defval);
    }
    if (!step(param_select,TRUE,TRUE))
      step(param_insert);
  }
}

static void writeMemberTemplateLists(const MemberDef *md)
{
  writeTemplateArgumentList(md->templateArguments(),md->getClassDef(),md->getFileDef());
}

static void writeTemplateList(const ClassDef *cd)
{
  writeTemplateArgumentList(cd->templateArguments(),cd,cd->getFileDef());
}

static void writeTemplateList(const ConceptDef *cd)
{
  writeTemplateArgumentList(cd->getTemplateParameterList(),cd,cd->getFileDef());
}

QCString getSQLDocBlock(const Definition *scope,
  const Definition *def,
  const QCString &doc,
  const QCString &fileName,
  int lineNr)
{
  if (doc.isEmpty()) return "";

  TextStream t;
  auto parser { createDocParser() };
  auto ast    { validatingParseDoc(*parser.get(),
                                   fileName,
                                   lineNr,
                                   scope,
                                   toMemberDef(def),
                                   doc,
                                   DocOptions())
              };
  auto astImpl = dynamic_cast<const DocNodeAST*>(ast.get());
  if (astImpl)
  {
    OutputCodeList xmlCodeList;
    xmlCodeList.add<XMLCodeGenerator>(&t);
    // create a parse tree visitor for XML
    XmlDocVisitor visitor(t,xmlCodeList,
        scope ? scope->getDefFileExtension() : QCString(""));
    std::visit(visitor,astImpl->root);
  }
  return convertCharEntitiesToUTF8(t.str());
}

static void getSQLDesc(SqlStmt &s,const char *col,const QCString &value,const Definition *def)
{
  bindTextParameter(
    s,
    col,
    getSQLDocBlock(
      def->getOuterScope(),
      def,
      value,
      def->docFile(),
      def->docLine()
    )
  );
}

static void getSQLDescCompound(SqlStmt &s,const char *col,const QCString &value,const Definition *def)
{
  bindTextParameter(
    s,
    col,
    getSQLDocBlock(
      def,
      def,
      value,
      def->docFile(),
      def->docLine()
    )
  );
}
////////////////////////////////////////////

/* (updated Sep 01 2018)
DoxMemberKind and DoxCompoundKind (compound.xsd) gave me some
faulty assumptions about "kind" strings, so I compiled a reference

The XML schema claims:
  DoxMemberKind: (14)
    dcop define enum event friend function interface property prototype
    service signal slot typedef variable

  DoxCompoundKind: (17)
    category class dir example exception file group interface module
    namespace page protocol service singleton struct type union

Member kind comes from MemberDef::memberTypeName()
  types.h defines 14 MemberType::*s
    _DCOP _Define _Enumeration _EnumValue _Event _Friend _Function _Interface
    _Property _Service _Signal _Slot _Typedef _Variable
      - xml doesn't include enumvalue here
        (but renders enumvalue as) a sub-node of memberdef/templateparamlist
      - xml includes 'prototype' that is unlisted here
        vestigial? commented out in docsets.cpp and perlmodgen.cpp
  MemberDef::memberTypeName() can return 15 strings:
    (sorted by MemberType to match above; quoted because whitespace...)
    "dcop" "macro definition" "enumeration" "enumvalue" "event" "friend"
    "function" "interface" "property" "service" "signal" "slot" "typedef"
    "variable"

    Above describes potential values for memberdef.kind

Compound kind is more complex. *Def::compoundTypeString()
  ClassDef kind comes from ::compoundTypeString()
    classdef.h defines 9 compound types
      Category Class Exception Interface Protocol Service Singleton Struct Union
    But ClassDef::compoundTypeString() "could" return 13 strings
      - default "unknown" shouldn't actually return
      - other 12 can vary by source language; see method for specifics
        category class enum exception interface module protocol service
        singleton struct type union

  DirDef, FileDef, GroupDef have no method to return a string
    tagfile/outputs hard-code kind to 'dir' 'file' or 'group'

  NamespaceDef kind comes from ::compoundTypeString()
    NamespaceDef::compoundTypeString() "could" return 6 strings
      - default empty ("") string
      - other 5 differ by source language
        constants library module namespace package

  PageDef also has no method to return a string
    - some locations hard-code the kind to 'page'
    - others conditionally output 'page' or 'example'

  All together, that's 23 potential strings (21 excl "" and unknown)
    "" category class constants dir enum example exception file group
    interface library module namespace package page protocol service singleton
    struct type union unknown

    Above describes potential values for compounddef.kind

For reference, there are 35 potential values of def.kind (33 excl "" and unknown):
  "" "category" "class" "constants" "dcop" "dir" "enum" "enumeration"
  "enumvalue" "event" "example" "exception" "file" "friend" "function" "group"
  "interface" "library" "macro definition" "module" "namespace" "package"
  "page" "property" "protocol" "service" "signal" "singleton" "slot" "struct"
  "type" "typedef" "union" "unknown" "variable"

This is relevant because the 'def' view generalizes memberdef and compounddef,
and two member+compound kind strings (interface and service) overlap.

I have no grasp of whether a real user docset would include one or more
member and compound using the interface or service kind.
*/

//////////////////////////////////////////////////////////////////////////////
static void generateSqlite3ForMember(const MemberDef *md, struct Refid scope_refid, const Definition *def)
{
  // + declaration/definition arg lists
  // + reimplements
  // + reimplementedBy
  // - exceptions
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
  if (md->memberType()==MemberType::EnumValue) return;
  if (md->isHidden()) return;

  QCString memType;

  // memberdef
  QCString qrefid = md->getOutputFileBase() + "_1" + md->anchor();
  struct Refid refid = insertRefid(qrefid);

  associateMember(md, refid, scope_refid);

  // compacting duplicate defs
  if(!refid.created && memberdefExists(refid) && memberdefIncomplete(refid, md))
  {
    /*
    For performance, ideal to skip a member we've already added.
    Unfortunately, we can have two memberdefs with the same refid documenting
    the declaration and definition. memberdefIncomplete() uses the 'inline'
    value to figure this out. Once we get to this point, we should *only* be
    seeing the *other* type of def/decl, so we'll set inline to a new value (2),
    indicating that this entry covers both inline types.
    */
    struct SqlStmt memberdef_update;

    // definitions have bodyfile/start/end
    if (md->getStartBodyLine()!=-1)
    {
      memberdef_update = memberdef_update_def;
      int bodyfile_id = insertPath(md->getBodyDef()->absFilePath(),!md->getBodyDef()->isReference());
      if (bodyfile_id == -1)
      {
          sqlite3_clear_bindings(memberdef_update.stmt);
      }
      else
      {
          bindIntParameter(memberdef_update,":bodyfile_id",bodyfile_id);
          bindIntParameter(memberdef_update,":bodystart",md->getStartBodyLine());
          bindIntParameter(memberdef_update,":bodyend",md->getEndBodyLine());
      }
    }
    // declarations don't
    else
    {
      memberdef_update = memberdef_update_decl;
      if (md->getDefLine() != -1)
      {
        int file_id = insertPath(md->getDefFileName(),!md->isReference());
        if (file_id!=-1)
        {
          bindIntParameter(memberdef_update,":file_id",file_id);
          bindIntParameter(memberdef_update,":line",md->getDefLine());
          bindIntParameter(memberdef_update,":column",md->getDefColumn());
        }
      }
    }

    bindIntParameter(memberdef_update, ":rowid", refid.rowid);
    // value 2 indicates we've seen "both" inline types.
    bindIntParameter(memberdef_update,":inline", 2);

    /* in case both are used, append/prepend descriptions */
    getSQLDesc(memberdef_update,":briefdescription",md->briefDescription(),md);
    getSQLDesc(memberdef_update,":detaileddescription",md->documentation(),md);
    getSQLDesc(memberdef_update,":inbodydescription",md->inbodyDocumentation(),md);

    step(memberdef_update,TRUE);

    // don't think we need to repeat params; should have from first encounter

    // + source references
    // The cross-references in initializers only work when both the src and dst
    // are defined.
    auto refList = md->getReferencesMembers();
    for (const auto &rmd : refList)
    {
      insertMemberReference(md,rmd, "inline");
    }
    // + source referenced by
    auto refByList = md->getReferencedByMembers();
    for (const auto &rmd : refByList)
    {
      insertMemberReference(rmd,md, "inline");
    }
    return;
  }

  bindIntParameter(memberdef_insert,":rowid", refid.rowid);
  bindTextParameter(memberdef_insert,":kind",md->memberTypeName());
  bindIntParameter(memberdef_insert,":prot",static_cast<int>(md->protection()));

  bindIntParameter(memberdef_insert,":static",md->isStatic());
  bindIntParameter(memberdef_insert,":extern",md->isExternal());

  bool isFunc=FALSE;
  switch (md->memberType())
  {
    case MemberType::Function: // fall through
    case MemberType::Signal:   // fall through
    case MemberType::Friend:   // fall through
    case MemberType::DCOP:     // fall through
    case MemberType::Slot:
      isFunc=TRUE;
      break;
    default:
      break;
  }

  if (isFunc)
  {
    const ArgumentList &al = md->argumentList();
    bindIntParameter(memberdef_insert,":const",al.constSpecifier());
    bindIntParameter(memberdef_insert,":volatile",al.volatileSpecifier());
    bindIntParameter(memberdef_insert,":explicit",md->isExplicit());
    bindIntParameter(memberdef_insert,":inline",md->isInline());
    bindIntParameter(memberdef_insert,":final",md->isFinal());
    bindIntParameter(memberdef_insert,":sealed",md->isSealed());
    bindIntParameter(memberdef_insert,":new",md->isNew());
    bindIntParameter(memberdef_insert,":optional",md->isOptional());
    bindIntParameter(memberdef_insert,":required",md->isRequired());

    bindIntParameter(memberdef_insert,":virt",static_cast<int>(md->virtualness()));
  }

  if (md->memberType() == MemberType::Variable)
  {
    bindIntParameter(memberdef_insert,":mutable",md->isMutable());
    bindIntParameter(memberdef_insert,":thread_local",md->isThreadLocal());
    bindIntParameter(memberdef_insert,":initonly",md->isInitonly());
    bindIntParameter(memberdef_insert,":attribute",md->isAttribute());
    bindIntParameter(memberdef_insert,":property",md->isProperty());
    bindIntParameter(memberdef_insert,":readonly",md->isReadonly());
    bindIntParameter(memberdef_insert,":bound",md->isBound());
    bindIntParameter(memberdef_insert,":removable",md->isRemovable());
    bindIntParameter(memberdef_insert,":constrained",md->isConstrained());
    bindIntParameter(memberdef_insert,":transient",md->isTransient());
    bindIntParameter(memberdef_insert,":maybevoid",md->isMaybeVoid());
    bindIntParameter(memberdef_insert,":maybedefault",md->isMaybeDefault());
    bindIntParameter(memberdef_insert,":maybeambiguous",md->isMaybeAmbiguous());
    if (!md->bitfieldString().isEmpty())
    {
      QCString bitfield = md->bitfieldString();
      if (bitfield.at(0)==':') bitfield=bitfield.mid(1);
      bindTextParameter(memberdef_insert,":bitfield",bitfield.stripWhiteSpace());
    }
  }
  else if (md->memberType() == MemberType::Property)
  {
    bindIntParameter(memberdef_insert,":readable",md->isReadable());
    bindIntParameter(memberdef_insert,":writable",md->isWritable());
    bindIntParameter(memberdef_insert,":gettable",md->isGettable());
    bindIntParameter(memberdef_insert,":privategettable",md->isPrivateGettable());
    bindIntParameter(memberdef_insert,":protectedgettable",md->isProtectedGettable());
    bindIntParameter(memberdef_insert,":settable",md->isSettable());
    bindIntParameter(memberdef_insert,":privatesettable",md->isPrivateSettable());
    bindIntParameter(memberdef_insert,":protectedsettable",md->isProtectedSettable());

    if (md->isAssign() || md->isCopy() || md->isRetain()
     || md->isStrong() || md->isWeak())
    {
      int accessor=0;
      if (md->isAssign())      accessor = 1;
      else if (md->isCopy())   accessor = 2;
      else if (md->isRetain()) accessor = 3;
      else if (md->isStrong()) accessor = 4;
      else if (md->isWeak())   accessor = 5;

      bindIntParameter(memberdef_insert,":accessor",accessor);
    }
    bindTextParameter(memberdef_insert,":read",md->getReadAccessor());
    bindTextParameter(memberdef_insert,":write",md->getWriteAccessor());
  }
  else if (md->memberType() == MemberType::Event)
  {
    bindIntParameter(memberdef_insert,":addable",md->isAddable());
    bindIntParameter(memberdef_insert,":removable",md->isRemovable());
    bindIntParameter(memberdef_insert,":raisable",md->isRaisable());
  }

  const MemberDef *rmd = md->reimplements();
  if (rmd)
  {
    QCString qreimplemented_refid = rmd->getOutputFileBase() + "_1" + rmd->anchor();

    struct Refid reimplemented_refid = insertRefid(qreimplemented_refid);

    bindIntParameter(reimplements_insert,":memberdef_rowid", refid.rowid);
    bindIntParameter(reimplements_insert,":reimplemented_rowid", reimplemented_refid.rowid);
    step(reimplements_insert,TRUE);
  }

  // + declaration/definition arg lists
  if (md->memberType()!=MemberType::Define &&
      md->memberType()!=MemberType::Enumeration
     )
  {
    if (md->memberType()!=MemberType::Typedef)
    {
      writeMemberTemplateLists(md);
    }
    QCString typeStr = md->typeString();
    stripQualifiers(typeStr);
    StringVector list;
    linkifyText(TextGeneratorSqlite3Impl(list), def, md->getBodyDef(),md,typeStr);
    if (!typeStr.isEmpty())
    {
      bindTextParameter(memberdef_insert,":type",typeStr);
    }

    if (!md->definition().isEmpty())
    {
      bindTextParameter(memberdef_insert,":definition",md->definition());
    }

    if (!md->argsString().isEmpty())
    {
      bindTextParameter(memberdef_insert,":argsstring",md->argsString());
    }
  }

  bindTextParameter(memberdef_insert,":name",md->name());

  // Extract references from initializer
  if (md->hasMultiLineInitializer() || md->hasOneLineInitializer())
  {
    bindTextParameter(memberdef_insert,":initializer",md->initializer());

    StringVector list;
    linkifyText(TextGeneratorSqlite3Impl(list),def,md->getBodyDef(),md,md->initializer());
    for (const auto &s : list)
    {
      if (md->getBodyDef())
      {
        DBG_CTX(("initializer:%s %s %s %d\n",
              qPrint(md->anchor()),
              qPrint(s),
              qPrint(md->getBodyDef()->getDefFileName()),
              md->getStartBodyLine()));
        QCString qsrc_refid = md->getOutputFileBase() + "_1" + md->anchor();
        struct Refid src_refid = insertRefid(qsrc_refid);
        struct Refid dst_refid = insertRefid(s);
        insertMemberReference(src_refid,dst_refid, "initializer");
      }
    }
  }

  if ( !md->getScopeString().isEmpty() )
  {
    bindTextParameter(memberdef_insert,":scope",md->getScopeString());
  }

  // +Brief, detailed and inbody description
  getSQLDesc(memberdef_insert,":briefdescription",md->briefDescription(),md);
  getSQLDesc(memberdef_insert,":detaileddescription",md->documentation(),md);
  getSQLDesc(memberdef_insert,":inbodydescription",md->inbodyDocumentation(),md);

  // File location
  if (md->getDefLine() != -1)
  {
    int file_id = insertPath(md->getDefFileName(),!md->isReference());
    if (file_id!=-1)
    {
      bindIntParameter(memberdef_insert,":file_id",file_id);
      bindIntParameter(memberdef_insert,":line",md->getDefLine());
      bindIntParameter(memberdef_insert,":column",md->getDefColumn());

      // definitions also have bodyfile/start/end
      if (md->getStartBodyLine()!=-1)
      {
        int bodyfile_id = insertPath(md->getBodyDef()->absFilePath(),!md->getBodyDef()->isReference());
        if (bodyfile_id == -1)
        {
            sqlite3_clear_bindings(memberdef_insert.stmt);
        }
        else
        {
            bindIntParameter(memberdef_insert,":bodyfile_id",bodyfile_id);
            bindIntParameter(memberdef_insert,":bodystart",md->getStartBodyLine());
            bindIntParameter(memberdef_insert,":bodyend",md->getEndBodyLine());
        }
      }
    }
  }

  int memberdef_id=step(memberdef_insert,TRUE);

  if (isFunc)
  {
    insertMemberFunctionParams(memberdef_id,md,def);
  }
  else if (md->memberType()==MemberType::Define &&
          !md->argsString().isEmpty())
  {
    insertMemberDefineParams(memberdef_id,md,def);
  }

  // + source references
  // The cross-references in initializers only work when both the src and dst
  // are defined.
  for (const auto &refmd : md->getReferencesMembers())
  {
    insertMemberReference(md,refmd, "inline");
  }
  // + source referenced by
  for (const auto &refmd : md->getReferencedByMembers())
  {
    insertMemberReference(refmd,md, "inline");
  }
}

static void generateSqlite3Section( const Definition *d,
                      const MemberList *ml,
                      struct Refid scope_refid,
                      const char * /*kind*/,
                      const QCString & /*header*/=QCString(),
                      const QCString & /*documentation*/=QCString())
{
  if (ml==nullptr) return;
  for (const auto &md : *ml)
  {
    // TODO: necessary? just tracking what xmlgen does; xmlgen says:
    // namespace members are also inserted in the file scope, but
    // to prevent this duplication in the XML output, we filter those here.
    if (d->definitionType()!=Definition::TypeFile || md->getNamespaceDef()==nullptr)
    {
      generateSqlite3ForMember(md, scope_refid, d);
    }
  }
}

static void associateAllClassMembers(const ClassDef *cd, struct Refid scope_refid)
{
  for (auto &mni : cd->memberNameInfoLinkedMap())
  {
    for (auto &mi : *mni)
    {
      const MemberDef *md = mi->memberDef();
      QCString qrefid = md->getOutputFileBase() + "_1" + md->anchor();
      associateMember(md, insertRefid(qrefid), scope_refid);
    }
  }
}

// many kinds: category class enum exception interface
// module protocol service singleton struct type union
// enum is Java only (and is distinct from enum memberdefs)
static void generateSqlite3ForClass(const ClassDef *cd)
{
  // NOTE: Skeptical about XML's version of these
  // 'x' marks missing items XML claims to include

  // + brief description
  // + detailed description
  // + template argument list(s)
  // + include file
  // + member groups
  // x inheritance DOT diagram
  // + list of direct super classes
  // + list of direct sub classes
  // + list of inner classes
  // x collaboration DOT diagram
  // + list of all members
  // x user defined member sections
  // x standard member sections
  // x detailed member documentation
  // - examples using the class

  if (cd->isReference())        return; // skip external references.
  if (cd->isHidden())           return; // skip hidden classes.
  if (cd->isAnonymous())        return; // skip anonymous compounds.
  if (cd->isImplicitTemplateInstance())  return; // skip generated template instances.

  struct Refid refid = insertRefid(cd->getOutputFileBase());

  // can omit a class that already has a refid
  if(!refid.created && compounddefExists(refid)){return;}

  bindIntParameter(compounddef_insert,":rowid", refid.rowid);

  bindTextParameter(compounddef_insert,":name",cd->name());
  bindTextParameter(compounddef_insert,":title",cd->title());
  bindTextParameter(compounddef_insert,":kind",cd->compoundTypeString());
  bindIntParameter(compounddef_insert,":prot",static_cast<int>(cd->protection()));

  int file_id = insertPath(cd->getDefFileName());
  bindIntParameter(compounddef_insert,":file_id",file_id);
  bindIntParameter(compounddef_insert,":line",cd->getDefLine());
  bindIntParameter(compounddef_insert,":column",cd->getDefColumn());

  // + include file
  /*
  TODO: I wonder if this can actually be cut (just here)

  We were adding this "include" to the "includes" table alongside
  other includes (from a FileDef). However, FileDef and ClassDef are using
  "includes" nodes in very a different way:
  - With FileDef, it means the file includes another.
  - With ClassDef, it means you should include this file to use this class.

  Because of this difference, I added a column to compounddef, header_id, and
  linked it back to the appropriate file. We could just add a nullable text
  column that would hold a string equivalent to what the HTML docs include,
  but the logic for generating it is embedded in
  ClassDef::writeIncludeFiles(OutputList &ol).

  That said, at least on the handful of test sets I have, header_id == file_id,
  suggesting it could be cut and clients might be able to reconstruct it from
  other values if there's a solid heuristic for *when a class will
  have a header file*.
  */
  const IncludeInfo *ii=cd->includeInfo();
  if (ii)
  {
    QCString nm = ii->includeName;
    if (nm.isEmpty() && ii->fileDef) nm = ii->fileDef->docName();
    if (!nm.isEmpty())
    {
      int header_id=-1;
      if (ii->fileDef)
      {
        insertPath(ii->fileDef->absFilePath(),!ii->fileDef->isReference());
      }
      DBG_CTX(("-----> ClassDef includeInfo for %s\n", qPrint(nm)));
      DBG_CTX(("       local    : %d\n", ii->local));
      DBG_CTX(("       imported : %d\n", ii->imported));
      if (ii->fileDef)
      {
          DBG_CTX(("header: %s\n", qPrint(ii->fileDef->absFilePath())));
      }
      DBG_CTX(("       file_id  : %d\n", file_id));
      DBG_CTX(("       header_id: %d\n", header_id));

      if(header_id!=-1)
      {
        bindIntParameter(compounddef_insert,":header_id",header_id);
      }
    }
  }

  getSQLDescCompound(compounddef_insert,":briefdescription",cd->briefDescription(),cd);
  getSQLDescCompound(compounddef_insert,":detaileddescription",cd->documentation(),cd);

  step(compounddef_insert);

  // + list of direct super classes
  for (const auto &bcd : cd->baseClasses())
  {
    struct Refid base_refid = insertRefid(bcd.classDef->getOutputFileBase());
    struct Refid derived_refid = insertRefid(cd->getOutputFileBase());
    bindIntParameter(compoundref_insert,":base_rowid", base_refid.rowid);
    bindIntParameter(compoundref_insert,":derived_rowid", derived_refid.rowid);
    bindIntParameter(compoundref_insert,":prot",static_cast<int>(bcd.prot));
    bindIntParameter(compoundref_insert,":virt",static_cast<int>(bcd.virt));
    step(compoundref_insert);
  }

  // + list of direct sub classes
  for (const auto &bcd : cd->subClasses())
  {
    struct Refid derived_refid = insertRefid(bcd.classDef->getOutputFileBase());
    struct Refid base_refid = insertRefid(cd->getOutputFileBase());
    bindIntParameter(compoundref_insert,":base_rowid", base_refid.rowid);
    bindIntParameter(compoundref_insert,":derived_rowid", derived_refid.rowid);
    bindIntParameter(compoundref_insert,":prot",static_cast<int>(bcd.prot));
    bindIntParameter(compoundref_insert,":virt",static_cast<int>(bcd.virt));
    step(compoundref_insert);
  }

  // + list of inner classes
  writeInnerClasses(cd->getClasses(),refid);

  // + template argument list(s)
  writeTemplateList(cd);

  // + member groups
  for (const auto &mg : cd->getMemberGroups())
  {
    generateSqlite3Section(cd,&mg->members(),refid,"user-defined",mg->header(),
        mg->documentation());
  }

  // this is just a list of *local* members
  for (const auto &ml : cd->getMemberLists())
  {
    if (!ml->listType().isDetailed())
    {
      generateSqlite3Section(cd,ml.get(),refid,"user-defined");
    }
  }

  // + list of all members
  associateAllClassMembers(cd, refid);
}

static void generateSqlite3ForConcept(const ConceptDef *cd)
{
  if (cd->isReference() || cd->isHidden()) return; // skip external references

  struct Refid refid = insertRefid(cd->getOutputFileBase());
  if(!refid.created && compounddefExists(refid)){return;}
  bindIntParameter(compounddef_insert,":rowid", refid.rowid);
  bindTextParameter(compounddef_insert,":name",cd->name());
  bindTextParameter(compounddef_insert,":kind","concept");

  int file_id = insertPath(cd->getDefFileName());
  bindIntParameter(compounddef_insert,":file_id",file_id);
  bindIntParameter(compounddef_insert,":line",cd->getDefLine());
  bindIntParameter(compounddef_insert,":column",cd->getDefColumn());

  getSQLDescCompound(compounddef_insert,":briefdescription",cd->briefDescription(),cd);
  getSQLDescCompound(compounddef_insert,":detaileddescription",cd->documentation(),cd);

  step(compounddef_insert);

  // + template argument list(s)
  writeTemplateList(cd);
}

static void generateSqlite3ForModule(const ModuleDef *mod)
{
  // + contained class definitions
  // + contained concept definitions
  // + member groups
  // + normal members
  // + brief desc
  // + detailed desc
  // + location (file_id, line, column)
  // - exports
  // + used files

  if (mod->isReference() || mod->isHidden()) return;
  struct Refid refid = insertRefid(mod->getOutputFileBase());
  if(!refid.created && compounddefExists(refid)){return;}
  bindIntParameter(compounddef_insert,":rowid", refid.rowid);
  bindTextParameter(compounddef_insert,":name",mod->name());
  bindTextParameter(compounddef_insert,":kind","module");

  int file_id = insertPath(mod->getDefFileName());
  bindIntParameter(compounddef_insert,":file_id",file_id);
  bindIntParameter(compounddef_insert,":line",mod->getDefLine());
  bindIntParameter(compounddef_insert,":column",mod->getDefColumn());

  getSQLDescCompound(compounddef_insert,":briefdescription",mod->briefDescription(),mod);
  getSQLDescCompound(compounddef_insert,":detaileddescription",mod->documentation(),mod);

  step(compounddef_insert);

  // + contained class definitions
  writeInnerClasses(mod->getClasses(),refid);

  // + contained concept definitions
  writeInnerConcepts(mod->getConcepts(),refid);

  // + member groups
  for (const auto &mg : mod->getMemberGroups())
  {
    generateSqlite3Section(mod,&mg->members(),refid,"user-defined",mg->header(),
        mg->documentation());
  }

  // + normal members
  for (const auto &ml : mod->getMemberLists())
  {
    if (ml->listType().isDeclaration())
    {
      generateSqlite3Section(mod,ml.get(),refid,"user-defined");
    }
  }

  // + files
  writeInnerFiles(mod->getUsedFiles(),refid);
}

// kinds: constants library module namespace package
static void generateSqlite3ForNamespace(const NamespaceDef *nd)
{
  // + contained class definitions
  // + contained namespace definitions
  // + member groups
  // + normal members
  // + brief desc
  // + detailed desc
  // + location (file_id, line, column)
  // - files containing (parts of) the namespace definition

  if (nd->isReference() || nd->isHidden()) return; // skip external references
  struct Refid refid = insertRefid(nd->getOutputFileBase());
  if(!refid.created && compounddefExists(refid)){return;}
  bindIntParameter(compounddef_insert,":rowid", refid.rowid);

  bindTextParameter(compounddef_insert,":name",nd->name());
  bindTextParameter(compounddef_insert,":title",nd->title());
  bindTextParameter(compounddef_insert,":kind","namespace");

  int file_id = insertPath(nd->getDefFileName());
  bindIntParameter(compounddef_insert,":file_id",file_id);
  bindIntParameter(compounddef_insert,":line",nd->getDefLine());
  bindIntParameter(compounddef_insert,":column",nd->getDefColumn());

  getSQLDescCompound(compounddef_insert,":briefdescription",nd->briefDescription(),nd);
  getSQLDescCompound(compounddef_insert,":detaileddescription",nd->documentation(),nd);

  step(compounddef_insert);

  // + contained class definitions
  writeInnerClasses(nd->getClasses(),refid);

  // + contained concept definitions
  writeInnerConcepts(nd->getConcepts(),refid);

  // + contained namespace definitions
  writeInnerNamespaces(nd->getNamespaces(),refid);

  // + member groups
  for (const auto &mg : nd->getMemberGroups())
  {
    generateSqlite3Section(nd,&mg->members(),refid,"user-defined",mg->header(),
        mg->documentation());
  }

  // + normal members
  for (const auto &ml : nd->getMemberLists())
  {
    if (ml->listType().isDeclaration())
    {
      generateSqlite3Section(nd,ml.get(),refid,"user-defined");
    }
  }
}

// kind: file
static void generateSqlite3ForFile(const FileDef *fd)
{
  // + includes files
  // + includedby files
  // x include graph
  // x included by graph
  // + contained class definitions
  // + contained namespace definitions
  // + member groups
  // + normal members
  // + brief desc
  // + detailed desc
  // x source code
  // + location (file_id, line, column)
  // - number of lines

  if (fd->isReference()) return; // skip external references

  struct Refid refid = insertRefid(fd->getOutputFileBase());
  if(!refid.created && compounddefExists(refid)){return;}
  bindIntParameter(compounddef_insert,":rowid", refid.rowid);

  bindTextParameter(compounddef_insert,":name",fd->name());
  bindTextParameter(compounddef_insert,":title",fd->title());
  bindTextParameter(compounddef_insert,":kind","file");

  int file_id = insertPath(fd->getDefFileName());
  bindIntParameter(compounddef_insert,":file_id",file_id);
  bindIntParameter(compounddef_insert,":line",fd->getDefLine());
  bindIntParameter(compounddef_insert,":column",fd->getDefColumn());

  getSQLDesc(compounddef_insert,":briefdescription",fd->briefDescription(),fd);
  getSQLDesc(compounddef_insert,":detaileddescription",fd->documentation(),fd);

  step(compounddef_insert);

  // + includes files
  for (const auto &ii : fd->includeFileList())
  {
    int src_id=insertPath(fd->absFilePath(),!fd->isReference());
    int dst_id=0;
    QCString dst_path;
    bool isLocal = (ii.kind & IncludeKind_LocalMask)!=0;

    if(ii.fileDef) // found file
    {
      if(ii.fileDef->isReference())
      {
        // strip tagfile from path
        QCString tagfile = ii.fileDef->getReference();
        dst_path = ii.fileDef->absFilePath();
        dst_path.stripPrefix(tagfile+":");
      }
      else
      {
        dst_path = ii.fileDef->absFilePath();
      }
      dst_id = insertPath(dst_path,isLocal);
    }
    else // can't find file
    {
      dst_id = insertPath(ii.includeName,isLocal,FALSE);
    }

    DBG_CTX(("-----> FileDef includeInfo for %s\n", qPrint(ii.includeName)));
    DBG_CTX(("       local:    %d\n", isLocal));
    DBG_CTX(("       imported: %d\n", (ii.kind & IncludeKind_ImportMask)!=0));
    if(ii.fileDef)
    {
      DBG_CTX(("include: %s\n", qPrint(ii.fileDef->absFilePath())));
    }
    DBG_CTX(("       src_id  : %d\n", src_id));
    DBG_CTX(("       dst_id: %d\n", dst_id));

    bindIntParameter(incl_select,":local",isLocal);
    bindIntParameter(incl_select,":src_id",src_id);
    bindIntParameter(incl_select,":dst_id",dst_id);
    if (step(incl_select,TRUE,TRUE)==0) {
      bindIntParameter(incl_insert,":local",isLocal);
      bindIntParameter(incl_insert,":src_id",src_id);
      bindIntParameter(incl_insert,":dst_id",dst_id);
      step(incl_insert);
    }
  }

  // + includedby files
  for (const auto &ii : fd->includedByFileList())
  {
    int dst_id=insertPath(fd->absFilePath(),!fd->isReference());
    int src_id=0;
    QCString src_path;
    bool isLocal = (ii.kind & IncludeKind_LocalMask)!=0;

    if(ii.fileDef) // found file
    {
      if(ii.fileDef->isReference())
      {
        // strip tagfile from path
        QCString tagfile = ii.fileDef->getReference();
        src_path = ii.fileDef->absFilePath();
        src_path.stripPrefix(tagfile+":");
      }
      else
      {
        src_path = ii.fileDef->absFilePath();
      }
      src_id = insertPath(src_path,isLocal);
    }
    else // can't find file
    {
      src_id = insertPath(ii.includeName,isLocal,FALSE);
    }

    bindIntParameter(incl_select,":local",isLocal);
    bindIntParameter(incl_select,":src_id",src_id);
    bindIntParameter(incl_select,":dst_id",dst_id);
    if (step(incl_select,TRUE,TRUE)==0) {
      bindIntParameter(incl_insert,":local",isLocal);
      bindIntParameter(incl_insert,":src_id",src_id);
      bindIntParameter(incl_insert,":dst_id",dst_id);
      step(incl_insert);
    }
  }

  // + contained class definitions
  writeInnerClasses(fd->getClasses(),refid);

  // + contained concept definitions
  writeInnerConcepts(fd->getConcepts(),refid);

  // + contained namespace definitions
  writeInnerNamespaces(fd->getNamespaces(),refid);

  // + member groups
  for (const auto &mg : fd->getMemberGroups())
  {
    generateSqlite3Section(fd,&mg->members(),refid,"user-defined",mg->header(),
          mg->documentation());
  }

  // + normal members
  for (const auto &ml : fd->getMemberLists())
  {
    if (ml->listType().isDeclaration())
    {
      generateSqlite3Section(fd,ml.get(),refid,"user-defined");
    }
  }
}

// kind: group
static void generateSqlite3ForGroup(const GroupDef *gd)
{
  // + members
  // + member groups
  // + files
  // + classes
  // + namespaces
  // - packages
  // + pages
  // + child groups
  // - examples
  // + brief description
  // + detailed description

  if (gd->isReference()) return; // skip external references.

  struct Refid refid = insertRefid(gd->getOutputFileBase());
  if(!refid.created && compounddefExists(refid)){return;}
  bindIntParameter(compounddef_insert,":rowid", refid.rowid);

  bindTextParameter(compounddef_insert,":name",gd->name());
  bindTextParameter(compounddef_insert,":title",gd->groupTitle());
  bindTextParameter(compounddef_insert,":kind","group");

  int file_id = insertPath(gd->getDefFileName());
  bindIntParameter(compounddef_insert,":file_id",file_id);
  bindIntParameter(compounddef_insert,":line",gd->getDefLine());
  bindIntParameter(compounddef_insert,":column",gd->getDefColumn());

  getSQLDesc(compounddef_insert,":briefdescription",gd->briefDescription(),gd);
  getSQLDesc(compounddef_insert,":detaileddescription",gd->documentation(),gd);

  step(compounddef_insert);

  // + files
  writeInnerFiles(gd->getFiles(),refid);

  // + classes
  writeInnerClasses(gd->getClasses(),refid);

  // + concepts
  writeInnerConcepts(gd->getConcepts(),refid);

  // + modules
  writeInnerModules(gd->getModules(),refid);

  // + namespaces
  writeInnerNamespaces(gd->getNamespaces(),refid);

  // + pages
  writeInnerPages(gd->getPages(),refid);

  // + groups
  writeInnerGroups(gd->getSubGroups(),refid);

  // + member groups
  for (const auto &mg : gd->getMemberGroups())
  {
    generateSqlite3Section(gd,&mg->members(),refid,"user-defined",mg->header(),
        mg->documentation());
  }

  // + members
  for (const auto &ml : gd->getMemberLists())
  {
    if (ml->listType().isDeclaration())
    {
      generateSqlite3Section(gd,ml.get(),refid,"user-defined");
    }
  }
}

// kind: dir
static void generateSqlite3ForDir(const DirDef *dd)
{
  // + dirs
  // + files
  // + briefdescription
  // + detaileddescription
  // + location (below uses file_id, line, column; XML just uses file)
  if (dd->isReference()) return; // skip external references

  struct Refid refid = insertRefid(dd->getOutputFileBase());
  if(!refid.created && compounddefExists(refid)){return;}
  bindIntParameter(compounddef_insert,":rowid", refid.rowid);

  bindTextParameter(compounddef_insert,":name",dd->displayName());
  bindTextParameter(compounddef_insert,":kind","dir");

  int file_id = insertPath(dd->getDefFileName(),TRUE,TRUE,2);
  bindIntParameter(compounddef_insert,":file_id",file_id);

  /*
  line and column are weird here, but:
  - dir goes into compounddef with all of the others
  - the semantics would be fine if we set them to NULL here
  - but defining line and column as NOT NULL is an important promise
    for other compounds, so I don't want to loosen it

  For reference, the queries return 1.
  0 or -1 make more sense, but I see that as a change for DirDef.
  */
  bindIntParameter(compounddef_insert,":line",dd->getDefLine());
  bindIntParameter(compounddef_insert,":column",dd->getDefColumn());

  getSQLDesc(compounddef_insert,":briefdescription",dd->briefDescription(),dd);
  getSQLDesc(compounddef_insert,":detaileddescription",dd->documentation(),dd);

  step(compounddef_insert);

  // + files
  writeInnerDirs(dd->subDirs(),refid);

  // + files
  writeInnerFiles(dd->getFiles(),refid);
}

// kinds: page, example
static void generateSqlite3ForPage(const PageDef *pd,bool isExample)
{
  // + name
  // + title
  // + brief description
  // + documentation (detailed description)
  // + inbody documentation
  // + sub pages
  if (pd->isReference()) return; // skip external references.

  // TODO: do we more special handling if isExample?

  QCString qrefid = pd->getOutputFileBase();
  if (pd->getGroupDef())
  {
    qrefid+="_"+pd->name();
  }
  if (qrefid=="index") qrefid="indexpage"; // to prevent overwriting the generated index page.

  struct Refid refid = insertRefid(qrefid);

  // can omit a page that already has a refid
  if(!refid.created && compounddefExists(refid)){return;}

  bindIntParameter(compounddef_insert,":rowid",refid.rowid);
  // + name
  bindTextParameter(compounddef_insert,":name",pd->name());

  QCString title;
  if (pd==Doxygen::mainPage.get()) // main page is special
  {
    if (mainPageHasTitle())
    {
      title = filterTitle(convertCharEntitiesToUTF8(Doxygen::mainPage->title()));
    }
    else
    {
      title = Config_getString(PROJECT_NAME);
    }
  }
  else
  {
    SectionInfo *si = SectionManager::instance().find(pd->name());
    if (si)
    {
      title = si->title();
    }
    if (title.isEmpty())
    {
      title = pd->title();
    }
  }

  // + title
  bindTextParameter(compounddef_insert,":title",title);

  bindTextParameter(compounddef_insert,":kind", isExample ? "example" : "page");

  int file_id = insertPath(pd->getDefFileName());

  bindIntParameter(compounddef_insert,":file_id",file_id);
  bindIntParameter(compounddef_insert,":line",pd->getDefLine());
  bindIntParameter(compounddef_insert,":column",pd->getDefColumn());

  // + brief description
  getSQLDesc(compounddef_insert,":briefdescription",pd->briefDescription(),pd);
  // + documentation (detailed description)
  getSQLDesc(compounddef_insert,":detaileddescription",pd->documentation(),pd);

  step(compounddef_insert);
  // + sub pages
  writeInnerPages(pd->getSubPages(),refid);
}


static sqlite3* openDbConnection()
{

  QCString outputDirectory = Config_getString(SQLITE3_OUTPUT);
  sqlite3 *db = nullptr;

  int rc = sqlite3_initialize();
  if (rc != SQLITE_OK)
  {
    err("sqlite3_initialize failed\n");
    return nullptr;
  }

  std::string dbFileName = "doxygen_sqlite3.db";
  FileInfo fi(outputDirectory.str()+"/"+dbFileName);

  if (fi.exists())
  {
    if (Config_getBool(SQLITE3_RECREATE_DB))
    {
       Dir().remove(fi.absFilePath());
    }
    else
    {
      err("doxygen_sqlite3.db already exists! Rename, remove, or archive it to regenerate\n");
      return nullptr;
    }
  }

  rc = sqlite3_open_v2(
    fi.absFilePath().c_str(),
    &db,
    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
    nullptr
  );
  if (rc != SQLITE_OK)
  {
    sqlite3_close(db);
    err("Database open failed: doxygen_sqlite3.db\n");
  }
  return db;
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
  sqlite3 *db = openDbConnection();
  if (db==nullptr)
  {
    return;
  }

# ifdef SQLITE3_DEBUG
  // debug: show all executed statements
  sqlite3_trace(db, &sqlLog, nullptr);
# endif

  beginTransaction(db);
  pragmaTuning(db);

  if (-1==initializeTables(db))
    return;

  if ( -1 == prepareStatements(db) )
  {
    err("sqlite generator: prepareStatements failed!\n");
    return;
  }

  recordMetadata();

  // + classes
  for (const auto &cd : *Doxygen::classLinkedMap)
  {
    msg("Generating Sqlite3 output for class {}\n",cd->name());
    generateSqlite3ForClass(cd.get());
  }

  // + concepts
  for (const auto &cd : *Doxygen::conceptLinkedMap)
  {
    msg("Generating Sqlite3 output for concept {}\n",cd->name());
    generateSqlite3ForConcept(cd.get());
  }

  // + modules
  for (const auto &mod : ModuleManager::instance().modules())
  {
    msg("Generating Sqlite3 output for module {}\n",mod->name());
    generateSqlite3ForModule(mod.get());
  }

  // + namespaces
  for (const auto &nd : *Doxygen::namespaceLinkedMap)
  {
    msg("Generating Sqlite3 output for namespace {}\n",nd->name());
    generateSqlite3ForNamespace(nd.get());
  }

  // + files
  for (const auto &fn : *Doxygen::inputNameLinkedMap)
  {
    for (const auto &fd : *fn)
    {
      msg("Generating Sqlite3 output for file {}\n",fd->name());
      generateSqlite3ForFile(fd.get());
    }
  }

  // + groups
  for (const auto &gd : *Doxygen::groupLinkedMap)
  {
    msg("Generating Sqlite3 output for group {}\n",gd->name());
    generateSqlite3ForGroup(gd.get());
  }

  // + page
  for (const auto &pd : *Doxygen::pageLinkedMap)
  {
    msg("Generating Sqlite3 output for page {}\n",pd->name());
    generateSqlite3ForPage(pd.get(),FALSE);
  }

  // + dirs
  for (const auto &dd : *Doxygen::dirLinkedMap)
  {
    msg("Generating Sqlite3 output for dir {}\n",dd->name());
    generateSqlite3ForDir(dd.get());
  }

  // + examples
  for (const auto &pd : *Doxygen::exampleLinkedMap)
  {
    msg("Generating Sqlite3 output for example {}\n",pd->name());
    generateSqlite3ForPage(pd.get(),TRUE);
  }

  // + main page
  if (Doxygen::mainPage)
  {
    msg("Generating Sqlite3 output for the main page\n");
    generateSqlite3ForPage(Doxygen::mainPage.get(),FALSE);
  }

  // TODO: copied from initializeSchema; not certain if we should say/do more
  // if there's a failure here?
  if (-1==initializeViews(db))
    return;

  endTransaction(db);
}

// vim: noai:ts=2:sw=2:ss=2:expandtab
