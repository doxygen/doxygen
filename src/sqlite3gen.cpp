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


#include "sqlite3gen.h"
#include "doxygen.h"
#include "xmlgen.h"
#include "config.h"
#include "util.h"
#include "outputlist.h"
#include "docparser.h"
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
#include "moduledef.h"
#include "jsonmldocvisitor.h"

#include <string.h>
#include <sqlite3.h>

#define SQLITE3_SCHEMA_VERSION "0.2.0"

enum PATH_TYPE {
  PATH_TYPE_FILE=1,
  PATH_TYPE_DIR=2
};

struct Refid {
  int rowid;
  QCString refid;
  bool isNew;
};

struct SqlStmt {
  const char   *query = nullptr;
  sqlite3_stmt *stmt = nullptr;
  sqlite3 *db = nullptr;
};

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
  msg("SQL: '%s'\n", sql);
}
# endif


static QCString getJsonDoc( const Definition *def,
    const QCString &in_text)
{
  QCString stext = in_text.stripWhiteSpace();
  if (stext.isEmpty()) return nullptr;
  // convert the documentation string into an abstract syntax tree
  QCString out_text;
  auto parser { createDocParser() };
  auto ast    { validatingParseDoc( *parser.get(),
      def->docFile(),
      def->docLine(),
      const_cast<Definition*>(def->getOuterScope()),
      toMemberDef(def),
      stext,
      false,
      false,
      QCString(),
      false,
      false,
      Config_getBool(MARKDOWN_SUPPORT)
      )};
  auto astImpl = dynamic_cast<const DocNodeAST*>(ast.get());
  if (astImpl)
  {
    // create a parse tree visitor for JsonML
    JsonMLDocVisitor visitor = JsonMLDocVisitor(out_text);
    // visit all nodes
    std::visit(visitor,astImpl->root);
    // clean up
  }
  return out_text;
}



static bool isFunction(const MemberDef *md)
{
  switch (md->memberType())
  {
    case MemberType::Function: // fall through
    case MemberType::Signal:   // fall through
    case MemberType::Friend:   // fall through
    case MemberType::DCOP:     // fall through
    case MemberType::Slot:
      return true;
      break;
    default:
      break;
  }
  return false;
}


static void stripQualifiers(QCString &typeStr)
{
  bool done=false;
  while (!done)
  {
    if      (typeStr.stripPrefix("static "));
    else if (typeStr.stripPrefix("virtual "));
    else if (typeStr.stripPrefix("volatile "));
    else if (typeStr=="virtual") typeStr="";
    else done=true;
  }
}

static int insertPath(QCString name, bool local=true, bool found=true, int type=PATH_TYPE_FILE);


//{ Table Schema
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
      "\tgenerated_on       TEXT NOT NULL\n"
      ");"
  },
  { "project",
    "CREATE TABLE IF NOT EXISTS project (\n"
      "\t-- project info\n"
      "\tname       TEXT NOT NULL,\n"
      "\tnumber     TEXT,\n"
      "\tbrief      TEXT\n"
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
      "\tkind      TEXT NOT NULL, -- inline, argument, initializer\n"
      "\t-- Just need to know they link; ignore duplicates.\n"
      "\tUNIQUE(src_rowid, dst_rowid, kind) ON CONFLICT IGNORE\n"
    ");\n"
  },
  { "memberdef",
    "CREATE TABLE IF NOT EXISTS memberdef (\n"
      "\t-- All processed identifiers.\n"
      "\trowid                INTEGER PRIMARY KEY NOT NULL,\n"
      "\tname                 TEXT NOT NULL,\n"
      "\tqualifiedname       TEXT NOT NULL,\n"
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
      "\tdeffile_id           INTEGER REFERENCES path,  -- file where this identifier is defined\n"
      "\tdefline              INTEGER ,  -- line where this identifier is defined\n"
      "\tdefcolumn            INTEGER ,  -- column where this identifier is defined\n"
      "\tdeclfile_id          INTEGER REFERENCES path,  -- file where this identifier is declared\n"
      "\tdeclline             INTEGER ,  -- line where this identifier is declared\n"
      "\tdeclcolumn           INTEGER ,  -- column where this identifier is declared\n"
      "\tdetaileddescription  JSON,\n"
      "\tbriefdescription     JSON,\n"
      "\tinbodydescription    JSON,\n"
      "\tFOREIGN KEY (rowid) REFERENCES refid (rowid)\n"
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
  { "sectiondef",
    "CREATE TABLE IF NOT EXISTS sectiondef (\n"
      "\t-- Section definitions.\n"
      "\trowid          INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
      "\tkind           TEXT NOT NULL, -- 'public-func' ''\n"
      "\theader         JSON, -- ??? ''\n"
      "\tdescription    JSON  -- ??? ''\n"
      ");"
  },
  { "compoundmembers",
    "CREATE TABLE IF NOT EXISTS compoundmembers (\n"
      "\t-- Aggregates all members of a compound.\n"
      "\t-- Similar to XML listofallmembers.\n"
      "\trowid            INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
      "\tcompounddef_rowid      INTEGER NOT NULL REFERENCES compounddef,\n"
      "\tmemberdef_rowid  INTEGER NOT NULL REFERENCES memberdef,\n"
      "\tprot             INTEGER NOT NULL,\n"
      "\tvirt             INTEGER NOT NULL,\n"
      "\tUNIQUE(compounddef_rowid, memberdef_rowid)\n"
      ");"
  },
  { "compounddef",
    "CREATE TABLE IF NOT EXISTS compounddef (\n"
      "\t-- Class/struct definitions.\n"
      "\trowid                INTEGER PRIMARY KEY NOT NULL,\n"
      "\tname                 TEXT NOT NULL,\n"
      "\ttitle                TEXT,\n"
      // probably won't be empty '' or unknown, but the source *could* return them...
      "\tkind                 TEXT NOT NULL, -- 'category' 'class' 'constants' 'dir' 'enum' 'example' 'exception' 'file' 'group' 'interface' 'library' 'module' 'namespace' 'package' 'page' 'protocol' 'service' 'singleton' 'struct' 'type' 'union' 'unknown' ''\n"
      "\tlanguage             TEXT,\n"
      "\tprot                 INTEGER DEFAULT 0, -- 0:public 1:protected 2:private 3:package\n"
      "\tdeffile_id           INTEGER NOT NULL REFERENCES path,\n"
      "\tdefline              INTEGER NOT NULL,\n"
      "\tdefcolumn            INTEGER NOT NULL,\n"
      "\theaderfile_id        INTEGER REFERENCES path,\n"
      "\tdetaileddescription  JSON,\n"
      "\tbriefdescription     JSON,\n"
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
//}


//{ View Schema
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
    "def_view",
    "CREATE VIEW IF NOT EXISTS def_view (\n"
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
    "memberdef_param_view",
    "CREATE VIEW IF NOT EXISTS memberdef_param_view (\n"
      "\t-- Crossrefs from member params.\n"
      "\trowid,\n"
      "\ttype,\n"
      "\tdeclname\n"
      ")\n"
      "as SELECT \n"
      "\tmemberdef_param.memberdef_id,\n"
      "\tparam.type,\n"
      "\tparam.declname\n"
      "FROM memberdef_param\n"
      "JOIN param ON param.rowid=memberdef_param.param_id;\n"
  },
  {
    "local_file_view",
    "CREATE VIEW IF NOT EXISTS local_file_view (\n"
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
    "external_file_view",
    "CREATE VIEW IF NOT EXISTS external_file_view (\n"
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
    "inline_xrefs_view",
    "CREATE VIEW IF NOT EXISTS inline_xrefs_view (\n"
      "\t-- Crossrefs from inline member source.\n"
      "\trowid,\n"
      "\tsrc_rowid,\n"
      "\tdst_rowid\n"
    ")\n"
    "as SELECT \n"
      "\txrefs.rowid,\n"
      "\txrefs.src_rowid,\n"
      "\txrefs.dst_rowid\n"
    "FROM xrefs WHERE xrefs.kind='inline';\n"
  },
  {
    "argument_xrefs_view",
    "CREATE VIEW IF NOT EXISTS argument_xrefs_view (\n"
      "\t-- Crossrefs from member def/decl arguments\n"
      "\trowid,\n"
      "\tsrc_rowid,\n"
      "\tdst_rowid\n"
    ")\n"
    "as SELECT \n"
      "\txrefs.rowid,\n"
      "\txrefs.src_rowid,\n"
      "\txrefs.dst_rowid\n"
    "FROM xrefs WHERE xrefs.kind='argument';\n"
  },
  {
    "initializer_xrefs_view",
    "CREATE VIEW IF NOT EXISTS initializer_xrefs_view (\n"
      "\t-- Crossrefs from member initializers\n"
      "\trowid,\n"
      "\tsrc_rowid,\n"
      "\tdst_rowid\n"
    ")\n"
    "as SELECT \n"
      "\txrefs.rowid,\n"
      "\txrefs.src_rowid,\n"
      "\txrefs.dst_rowid\n"
    "FROM xrefs WHERE xrefs.kind='initializer';\n"
  },
  {
    "inner_outer_view",
    "CREATE VIEW IF NOT EXISTS inner_outer_view\n"
    "\t-- Joins 'contains' relations to simplify inner/outer 'rel' queries.\n"
    "as SELECT \n"
      "\tinner.*,\n"
      "\touter.*\n"
    "FROM def_view as inner\n"
      "\tJOIN contains ON inner.rowid=contains.inner_rowid\n"
      "\tJOIN def_view AS outer ON outer.rowid=contains.outer_rowid;\n"
  },
  {
    "rel_view",
    "CREATE VIEW IF NOT EXISTS rel_view (\n"
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
      "\tdef_view.rowid,\n"
      "\tEXISTS (SELECT rowid FROM reimplements WHERE reimplemented_rowid=def_view.rowid),\n"
      "\tEXISTS (SELECT rowid FROM reimplements WHERE memberdef_rowid=def_view.rowid),\n"
      "\t-- rowid/kind for inner, [rowid:1/kind:1] for outer\n"
      "\tEXISTS (SELECT * FROM inner_outer_view WHERE [rowid:1]=def_view.rowid),\n"
      "\tEXISTS (SELECT * FROM inner_outer_view WHERE rowid=def_view.rowid),\n"
      "\tEXISTS (SELECT * FROM inner_outer_view WHERE [rowid:1]=def_view.rowid AND kind='page'),\n"
      "\tEXISTS (SELECT * FROM inner_outer_view WHERE rowid=def_view.rowid AND [kind:1]='page'),\n"
      "\tEXISTS (SELECT * FROM inner_outer_view WHERE [rowid:1]=def_view.rowid AND kind='dir'),\n"
      "\tEXISTS (SELECT * FROM inner_outer_view WHERE rowid=def_view.rowid AND [kind:1]='dir'),\n"
      "\tEXISTS (SELECT * FROM inner_outer_view WHERE [rowid:1]=def_view.rowid AND kind='file'),\n"
      "\tEXISTS (SELECT * FROM inner_outer_view WHERE rowid=def_view.rowid AND [kind:1]='file'),\n"
      "\tEXISTS (SELECT * FROM inner_outer_view WHERE [rowid:1]=def_view.rowid AND kind in (\n"
        "'category','class','enum','exception','interface','module','protocol',\n"
        "'service','singleton','struct','type','union'\n"
      ")),\n"
      "\tEXISTS (SELECT * FROM inner_outer_view WHERE rowid=def_view.rowid AND [kind:1] in (\n"
        "'category','class','enum','exception','interface','module','protocol',\n"
        "'service','singleton','struct','type','union'\n"
      ")),\n"
      "\tEXISTS (SELECT * FROM inner_outer_view WHERE [rowid:1]=def_view.rowid AND kind='namespace'),\n"
      "\tEXISTS (SELECT * FROM inner_outer_view WHERE rowid=def_view.rowid AND [kind:1]='namespace'),\n"
      "\tEXISTS (SELECT * FROM inner_outer_view WHERE [rowid:1]=def_view.rowid AND kind='group'),\n"
      "\tEXISTS (SELECT * FROM inner_outer_view WHERE rowid=def_view.rowid AND [kind:1]='group'),\n"
      "\tEXISTS (SELECT rowid FROM compoundmembers WHERE compounddef_rowid=def_view.rowid),\n"
      "\tEXISTS (SELECT rowid FROM compoundmembers WHERE memberdef_rowid=def_view.rowid),\n"
      "\tEXISTS (SELECT rowid FROM compoundref WHERE base_rowid=def_view.rowid),\n"
      "\tEXISTS (SELECT rowid FROM compoundref WHERE derived_rowid=def_view.rowid),\n"
      "\tEXISTS (SELECT rowid FROM inline_xrefs_view WHERE dst_rowid=def_view.rowid),\n"
      "\tEXISTS (SELECT rowid FROM inline_xrefs_view WHERE src_rowid=def_view.rowid),\n"
      "\tEXISTS (SELECT rowid FROM argument_xrefs_view WHERE dst_rowid=def_view.rowid),\n"
      "\tEXISTS (SELECT rowid FROM argument_xrefs_view WHERE src_rowid=def_view.rowid),\n"
      "\tEXISTS (SELECT rowid FROM initializer_xrefs_view WHERE dst_rowid=def_view.rowid),\n"
      "\tEXISTS (SELECT rowid FROM initializer_xrefs_view WHERE src_rowid=def_view.rowid)\n"
    "FROM def_view ORDER BY def_view.rowid;"
  }
};
//}


//{ SQL Statements
/// If you add a new statement below, make sure to add it to
/// prepareStatements(). If sqlite3 is segfaulting (especially in
/// sqlite3_clear_bindings()), using an un-prepared statement may
/// be the cause.
SqlStmt meta_insert = {
  "INSERT INTO meta "
    "( doxygen_version, schema_version, generated_at, generated_on )"
  "VALUES "
    "(:doxygen_version,:schema_version,:generated_at,:generated_on )"
  ,nullptr
};
//////////////////////////////////////////////////////
SqlStmt project_insert = {
  "INSERT INTO project "
    "( name, number, brief )"
    "VALUES "
    "(:name,:number,:brief )"
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
SqlStmt xrefs_select= {
  "SELECT rowid FROM xrefs WHERE "
    "( src_rowid=:src_rowid) AND "
    "( dst_rowid=:dst_rowid) AND "
    "( kind=:kind)"
    ,nullptr
};
SqlStmt xrefs_insert= {
  "INSERT INTO xrefs "
    "( src_rowid, dst_rowid, kind )"
  "VALUES "
    "(:src_rowid,:dst_rowid,:kind )"
  ,nullptr
};
//////////////////////////////////////////////////////
SqlStmt reimplements_insert= {
  "INSERT INTO reimplements "
    "( memberdef_rowid, reimplemented_rowid )"
  "VALUES "
    "(:memberdef_rowid,:reimplemented_rowid )"
  ,nullptr
};
//////////////////////////////////////////////////////
SqlStmt memberdef_select={
  "SELECT EXISTS (SELECT * FROM memberdef WHERE rowid = :rowid)"
  ,nullptr
};

SqlStmt memberdef_insert={
  "INSERT INTO memberdef "
  "("
    "rowid,"
    "name,"
    "qualifiedname,"
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
    "deffile_id,"
    "defline,"
    "defcolumn,"
    "declfile_id,"
    "declline,"
    "declcolumn,"
    "detaileddescription,"
    "briefdescription,"
    "inbodydescription"
  ")"
  "VALUES "
  "("
    ":rowid,"
    ":name,"
    ":qualifiedname,"
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
    ":deffile_id,"
    ":defline,"
    ":defcolumn,"
    ":declfile_id,"
    ":declline,"
    ":declcolumn,"
    ":detaileddescription,"
    ":briefdescription,"
    ":inbodydescription"
  ")"
  ,nullptr
};
SqlStmt memberdef_update={
  "UPDATE memberdef SET "
    "inline = :inline,"
    "deffile_id = :deffile_id,"
    "defline = :defline,"
    "defcolumn = :defcolumn,"
    "declfile_id = :declfile_id,"
    "declline = :declline,"
    "declcolumn = :declcolumn,"
    "bodystart = :bodystart,"
    "bodyend = :bodyend,"
    "bodyfile_id = :bodyfile_id,"
    "detaileddescription = :detaileddescription,"
    "briefdescription = :briefdescription,"
    "inbodydescription = :inbodydescription"
  "WHERE rowid = :rowid"
  ,nullptr
};
//////////////////////////////////////////////////////
SqlStmt compoundmembers_select={
  "SELECT rowid FROM compoundmembers WHERE "
    "( compounddef_rowid=:compounddef_rowid ) AND "
  "( memberdef_rowid=:memberdef_rowid )"
    ,nullptr
};
SqlStmt compoundmembers_insert={
  "INSERT INTO compoundmembers "
    "( compounddef_rowid, memberdef_rowid, prot, virt ) "
    "VALUES "
    "(:compounddef_rowid,:memberdef_rowid,:prot,:virt )"
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
    "language,"
    "prot,"
    "deffile_id,"
    "defline,"
    "defcolumn,"
    "headerfile_id,"
    "briefdescription,"
    "detaileddescription"
  ")"
  "VALUES "
  "("
    ":rowid,"
    ":name,"
    ":title,"
    ":kind,"
    ":language,"
    ":prot,"
    ":deffile_id,"
    ":defline,"
    ":defcolumn,"
    ":headerfile_id,"
    ":briefdescription,"
    ":detaileddescription"
  ")"
  ,nullptr
};
SqlStmt compounddef_select={
  "SELECT EXISTS ("
    "SELECT * FROM compounddef WHERE rowid = :rowid"
  ")"
  ,nullptr
};
//////////////////////////////////////////////////////
SqlStmt sectiondef_insert={
  "INSERT INTO sectiondef "
    "("
    "kind,"
    "header,"
    "description"
    ")"
    "VALUES "
    "("
    ":kind,"
    ":header,"
    ":description"
    ")"
    ,nullptr
};
SqlStmt sectiondef_exists={
  "SELECT EXISTS ("
    "SELECT * FROM sectiondef WHERE rowid = :rowid"
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
//////////////////////////////////////////////////////
SqlStmt memberdef_param_view_select={
  "SELECT * FROM memberdef_param_view WHERE rowid = :rowid"
    ,nullptr
};
//}


//{ SQL-to-Text Generator
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
    StringVector &m_list; //< filled by linkifyText and consumed by the caller
};
//}


//{ SQL Parameter Binders
static bool bindTextParameter(SqlStmt &s,const char *name,const QCString &value,bool _static=false)
{
  int idx = sqlite3_bind_parameter_index(s.stmt, name);
  if (idx==0)
  {
    err("sqlite3_bind_parameter_index(%s)[%s] failed: %s\n", name, s.query, sqlite3_errmsg(s.db));
    return false;
  }
  int rc = sqlite3_bind_text(s.stmt, idx, value.data(), -1, _static==true?SQLITE_STATIC:SQLITE_TRANSIENT);
  if (rc!=SQLITE_OK)
  {
    err("sqlite3_bind_text(%s)[%s] failed: %s\n", name, s.query, sqlite3_errmsg(s.db));
    return false;
  }
  return true;
}

static bool bindIntParameter(SqlStmt &s,const char *name,int value)
{
  int idx = sqlite3_bind_parameter_index(s.stmt, name);
  if (idx==0)
  {
    err("sqlite3_bind_parameter_index(%s)[%s] failed to find column: %s\n", name, s.query, sqlite3_errmsg(s.db));
    return false;
  }
  int rc = sqlite3_bind_int(s.stmt, idx, value);
  if (rc!=SQLITE_OK)
  {
    err("sqlite3_bind_int(%s)[%s] failed: %s\n", name, s.query, sqlite3_errmsg(s.db));
    return false;
  }
  return true;
}

static bool bindJsonParameter(SqlStmt &s, const char *name, const Definition *def, QCString value, bool _static=true)
{
  int idx = sqlite3_bind_parameter_index(s.stmt, name);
  if (idx==0)
  {
    err("sqlite3_bind_parameter_index(%s)[%s] failed: %s\n", name, s.query, sqlite3_errmsg(s.db));
    return false;
  }

  QCString json_value = getJsonDoc(def, value);

  int rc = sqlite3_bind_text(s.stmt, idx, json_value.data(), -1, SQLITE_TRANSIENT);
  if (rc!=SQLITE_OK)
  {
    err("sqlite3_bind_text(%s)[%s] failed: %s\n", name, s.query, sqlite3_errmsg(s.db));
    return false;
  }
  return true;
}
//}


//{ Step
static int step(SqlStmt &s)
{
  int rc = sqlite3_step(s.stmt);
  if (rc!=SQLITE_DONE && rc!=SQLITE_ROW)
  {
    err("sqlite3_step: %s (rc: %d)\n", sqlite3_errmsg(s.db), rc);
    sqlite3_reset(s.stmt);
    sqlite3_clear_bindings(s.stmt);
    return -1;
  }
  sqlite3_reset(s.stmt);
  sqlite3_clear_bindings(s.stmt); // XXX When should this really be called
  return 0;
}

/// returns row-id or -1 (on failure)
static int stepRowid(SqlStmt &s, bool select=false)
{
  int rowid=-1;
  int rc = sqlite3_step(s.stmt);
  if (rc!=SQLITE_DONE && rc!=SQLITE_ROW)
  {
    err("sqlite3_step: %s (rc: %d)\n", sqlite3_errmsg(s.db), rc);
    sqlite3_reset(s.stmt);
    sqlite3_clear_bindings(s.stmt);
    return -1;
  }
  if (select) rowid = sqlite3_column_int(s.stmt, 0); // works on selects, doesn't on inserts
  if (!select) rowid = sqlite3_last_insert_rowid(s.db); //works on inserts, doesn't on selects
  sqlite3_reset(s.stmt);
  sqlite3_clear_bindings(s.stmt); // XXX When should this really be called
  return rowid;
}
//}

static bool existsMemberDef(Refid refid)
{
  bindIntParameter(memberdef_select,":rowid",refid.rowid);
  int row = stepRowid(memberdef_select,true);
  return row>0 ? true : false;
}

static bool compounddef_exists(Refid refid)
{
  bindIntParameter(compounddef_select,":rowid",refid.rowid);
  int row = stepRowid(compounddef_select,true);
  return row>0 ? true : false;
}


//{ Prepared Statement BIND
static  void bindDefinitionHeader(SqlStmt &s, const IncludeInfo *ii)
{
  if (!ii)
    return;

  QCString nm = ii->includeName;
  if (nm.isEmpty() && ii->fileDef)
    nm = ii->fileDef->docName();

  if (nm.isEmpty())
    return;

  int headerfile_id=-1;
  if (ii->fileDef)
  {
    headerfile_id = insertPath(ii->fileDef->absFilePath(),!ii->fileDef->isReference());
  }
  DBG_CTX(("-----> ClassDef includeInfo for %s\n", qPrint(nm)));
  DBG_CTX(("       local    : %d\n", ii->local));
  DBG_CTX(("       imported : %d\n", ii->imported));
  if (ii->fileDef)
  {
    DBG_CTX(("header: %s\n", qPrint(ii->fileDef->absFilePath())));
  }
  DBG_CTX(("       headerfile_id: %d\n", headerfile_id));

  if(headerfile_id!=-1)
  {
    bindIntParameter(s,":headerfile_id",headerfile_id);
  }
}

static void bindMemberDefLocation(const MemberDef *md)
{
  int deffile_id = insertPath(md->getDefFileName(),!md->isReference());
  if (deffile_id==-1)
    return;

  bindIntParameter(memberdef_insert,":deffile_id",deffile_id);
  bindIntParameter(memberdef_insert,":defline",md->getDefLine());
  bindIntParameter(memberdef_insert,":defcolumn",md->getDefColumn());
}

static void bindMemberDeclLocation(const MemberDef *md)
{
  int declfile_id = insertPath(md->getDeclFileName(),!md->isReference());
  if (declfile_id==-1)
    return;

  bindIntParameter(memberdef_insert,":declfile_id",declfile_id);
  bindIntParameter(memberdef_insert,":declline",md->getDeclLine());
  bindIntParameter(memberdef_insert,":declcolumn",md->getDeclColumn());
}

static void bindMemberBodyLocation(const MemberDef *md)
{
  if (md->getStartBodyLine()==-1)
    return;

  int bodyfile_id = insertPath(md->getBodyDef()->absFilePath(),!md->getBodyDef()->isReference());
  if (bodyfile_id == -1)
    return;

  bindIntParameter(memberdef_insert,":bodyfile_id",bodyfile_id);
  bindIntParameter(memberdef_insert,":bodystart",md->getStartBodyLine());
  bindIntParameter(memberdef_insert,":bodyend",md->getEndBodyLine());
}


static void bindMemberTypeFunction(const MemberDef *md)
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


static void bindMemberTypeVariable(const MemberDef *md)
{
  bindIntParameter(memberdef_insert,":mutable",md->isMutable());
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
    bindTextParameter(memberdef_insert,":bitfield",bitfield.stripWhiteSpace().data(), false);
  }
}

static void bindMemberTypeProperty(const MemberDef *md)
{
  bindIntParameter(memberdef_insert,":readable",md->isReadable());
  bindIntParameter(memberdef_insert,":writable",md->isWritable());
  bindIntParameter(memberdef_insert,":gettable",md->isGettable());
  bindIntParameter(memberdef_insert,":privategettable",md->isPrivateGettable());
  bindIntParameter(memberdef_insert,":protectedgettable",md->isProtectedGettable());
  bindIntParameter(memberdef_insert,":settable",md->isSettable());
  bindIntParameter(memberdef_insert,":privatesettable",md->isPrivateSettable());
  bindIntParameter(memberdef_insert,":protectedsettable",md->isProtectedSettable());

  if (md->isAssign()
      || md->isCopy()
      || md->isRetain()
      || md->isStrong()
      || md->isWeak())
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

static void bindMemberTypeEvent(const MemberDef *md)
{
  bindIntParameter(memberdef_insert,":addable",md->isAddable());
  bindIntParameter(memberdef_insert,":removable",md->isRemovable());
  bindIntParameter(memberdef_insert,":raisable",md->isRaisable());
}

static void bindMemberType(const MemberDef *md)
{
  if (isFunction(md))
    bindMemberTypeFunction(md);
  else if (md->memberType() == MemberType::Variable)
    bindMemberTypeVariable(md);
  else if (md->memberType() == MemberType::Property)
    bindMemberTypeProperty(md);
  else if (md->memberType() == MemberType::Event)
    bindMemberTypeEvent(md);
}
//}


//{ INSERT
static Refid insertRefid(const QCString &refid)
{
  Refid ret;
  ret.rowid=-1;
  ret.refid=refid;
  ret.isNew = false;

  bindTextParameter(refid_select,":refid",refid);
  ret.rowid=stepRowid(refid_select,true);
  if (ret.rowid==0)
  {
    bindTextParameter(refid_insert,":refid",refid);
    ret.rowid=stepRowid(refid_insert);
    ret.isNew = true;
  }

  return ret;
}

/// Insert the contained class definitions.
/// @todo I collapsed all innerX tables into 'contains',
/// which raises the prospect that
/// all of these very similar insertInnerX funcs could be refactored into a one,
/// or a small set of common parts.
///
/// I think the hurdles are:
/// - picking a first argument that every call location can pass
/// - which yields a consistent iterator
/// - accommodates PageDef's slightly different rules for generating the
///  inner_refid (unless I'm missing a method that would uniformly return
///  the correct refid for all types).
static void insertInnerClasses(const ClassLinkedRefMap &cl, Refid outer_refid)
{
  for (const auto &cd : cl)
  {
    if (!cd->isHidden() && !cd->isAnonymous())
    {
      Refid inner_refid = insertRefid(cd->getOutputFileBase());

      bindIntParameter(contains_insert,":inner_rowid", inner_refid.rowid);
      bindIntParameter(contains_insert,":outer_rowid", outer_refid.rowid);
      step(contains_insert);
    }
  }
}

static void insertInnerConcepts(const ConceptLinkedRefMap &cl, struct Refid outer_refid)
{
  for (const auto &cd : cl)
  {
    struct Refid inner_refid = insertRefid(cd->getOutputFileBase());

    bindIntParameter(contains_insert,":inner_rowid", inner_refid.rowid);
    bindIntParameter(contains_insert,":outer_rowid", outer_refid.rowid);
    step(contains_insert);
  }
}

static void insertInnerModules(const ModuleLinkedRefMap &ml, struct Refid outer_refid)
{
  for (const auto &mod : ml)
  {
    struct Refid inner_refid = insertRefid(mod->getOutputFileBase());

    bindIntParameter(contains_insert,":inner_rowid", inner_refid.rowid);
    bindIntParameter(contains_insert,":outer_rowid", outer_refid.rowid);
    step(contains_insert);
  }
}

static int insertPath(QCString name, bool local/*=true*/, bool found/*=true*/, int type/*=PATH_TYPE_FILE*/)
{
  int rowid=-1;
  if (name==nullptr) return rowid;

  name = stripFromPath(name);

  bindTextParameter(path_select,":name",name.data(),false);
  rowid=stepRowid(path_select,true);
  if (rowid==0)
  {
    bindTextParameter(path_insert,":name",name.data(),false);
    bindIntParameter(path_insert,":type",type);
    bindIntParameter(path_insert,":local",local?1:0);
    bindIntParameter(path_insert,":found",found?1:0);
    rowid=stepRowid(path_insert);
  }
  return rowid;
}

static void insertMeta(void)
{
  bindTextParameter(meta_insert,":doxygen_version",getDoxygenVersion());
  bindTextParameter(meta_insert,":schema_version",SQLITE3_SCHEMA_VERSION);
  bindTextParameter(meta_insert,":generated_at",dateToString(DateTimeType::DateTime), false);
  bindTextParameter(meta_insert,":generated_on",dateToString(DateTimeType::Date), false);
  step(meta_insert);
}


static void insertClassMember(const MemberDef *md,
    const Refid member_refid,
    Refid compounddef_refid)
{
  if (md->isAnonymous())
    return;

  bindIntParameter(compoundmembers_select, ":compounddef_rowid", compounddef_refid.rowid);
  bindIntParameter(compoundmembers_select, ":memberdef_rowid", member_refid.rowid);
  int row = stepRowid(compoundmembers_select,true);
  if (row>0)
    return;

  bindIntParameter(compoundmembers_insert, ":compounddef_rowid", compounddef_refid.rowid);
  bindIntParameter(compoundmembers_insert, ":memberdef_rowid", member_refid.rowid);

  bindIntParameter(compoundmembers_insert, ":prot", static_cast<int>(md->protection()));
  bindIntParameter(compoundmembers_insert, ":virt", static_cast<int>(md->virtualness()));
  step(compoundmembers_insert);
}

static bool insertMemberReference(Refid src_refid, Refid dst_refid, const char *kind)
{
  if (src_refid.rowid==-1||dst_refid.rowid==-1)
    return false;

  bindIntParameter(xrefs_select,":src_rowid",src_refid.rowid);
  bindIntParameter(xrefs_select,":dst_rowid",dst_refid.rowid);
  bindTextParameter(xrefs_select,":kind",kind);
  int row = stepRowid(xrefs_select,true);
  if (row>0)
    return true;

  if (
      !bindIntParameter(xrefs_insert,":src_rowid",src_refid.rowid) ||
      !bindIntParameter(xrefs_insert,":dst_rowid",dst_refid.rowid) ||
      !bindTextParameter(xrefs_insert,":kind",kind)
     )
  {
    return false;
  }

  step(xrefs_insert);
  return true;
}

static void insertMemberReference(const MemberDef *src, const MemberDef *dst, const char *kind)
{
  QCString qdst_refid = dst->getOutputFileBase() + "_1" + dst->anchor();
  QCString qsrc_refid = src->getOutputFileBase() + "_1" + src->anchor();

  Refid src_refid = insertRefid(qsrc_refid);
  Refid dst_refid = insertRefid(qdst_refid);
  insertMemberReference(src_refid,dst_refid,kind);
}

static void insertMemberReferences(const MemberDef *md)
{
  // + source references
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
}

static void insertFunctionArgs(int memberdef_id, const MemberDef *md, const Definition *def)
{
  const ArgumentList &declAl = md->declArgumentList();
  const ArgumentList &defAl = md->argumentList();
  if (declAl.size()>0)
  {
    auto defIt = defAl.begin();
    for (const Argument &a : declAl)
    {
      const Argument *defArg = 0;
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
          Refid src_refid = insertRefid(qsrc_refid);
          Refid dst_refid = insertRefid(s.c_str());
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

      int param_id=stepRowid(param_select,true);
      if (param_id==0) {
        param_id=stepRowid(param_insert);
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


static void insertInnerPages(const PageLinkedRefMap &pl, Refid outer_refid)
{
  for (const auto &pd : pl)
  {
    Refid inner_refid = insertRefid(
      pd->getGroupDef() ? pd->getOutputFileBase()+"_"+pd->name() : pd->getOutputFileBase()
    );

    bindIntParameter(contains_insert,":inner_rowid", inner_refid.rowid);
    bindIntParameter(contains_insert,":outer_rowid", outer_refid.rowid);
    step(contains_insert);
  }
}

static void insertInnerGroups(const GroupList &gl, Refid outer_refid)
{
  for (const auto &sgd : gl)
  {
    Refid inner_refid = insertRefid(sgd->getOutputFileBase());

    bindIntParameter(contains_insert,":inner_rowid", inner_refid.rowid);
    bindIntParameter(contains_insert,":outer_rowid", outer_refid.rowid);
    step(contains_insert);
  }
}

static void insertInnerFiles(const FileList &fl, Refid outer_refid)
{
  for (const auto &fd: fl)
  {
    Refid inner_refid = insertRefid(fd->getOutputFileBase());

    bindIntParameter(contains_insert,":inner_rowid", inner_refid.rowid);
    bindIntParameter(contains_insert,":outer_rowid", outer_refid.rowid);
    step(contains_insert);
  }
}

static void insertInnerDirs(const DirList &dl, Refid outer_refid)
{
  for (const auto subdir : dl)
  {
    Refid inner_refid = insertRefid(subdir->getOutputFileBase());

    bindIntParameter(contains_insert,":inner_rowid", inner_refid.rowid);
    bindIntParameter(contains_insert,":outer_rowid", outer_refid.rowid);
    step(contains_insert);
  }
}

static void insertInnerNamespaces(const NamespaceLinkedRefMap &nl, Refid outer_refid)
{
  for (const auto &nd : nl)
  {
    if (!nd->isHidden() && !nd->isAnonymous())
    {
      Refid inner_refid = insertRefid(nd->getOutputFileBase());

      bindIntParameter(contains_insert,":inner_rowid",inner_refid.rowid);
      bindIntParameter(contains_insert,":outer_rowid",outer_refid.rowid);
      step(contains_insert);
    }
  }
}


static void insertTemplateArgumentList(const ArgumentList &al,
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
    if (!stepRowid(param_select,true))
      step(param_insert);
  }
}

static void insertMemberTemplateLists(const MemberDef *md)
{
  insertTemplateArgumentList(md->templateArguments(),md->getClassDef(),md->getFileDef());
}

static void insertTemplateList(const ClassDef *cd)
{
  insertTemplateArgumentList(cd->templateArguments(),cd,cd->getFileDef());
}

static void insertTemplateList(const ConceptDef *cd)
{
  insertTemplateArgumentList(cd->getTemplateParameterList(),cd,cd->getFileDef());
}
static void insertDefineArgs(int memberdef_id,const MemberDef *md, const Definition *def)
{
  // distinguish "foo" from "foo()"
  if (md->argumentList().empty())
  {
    DBG_CTX(("no params\n"));
    return;
  }

  for (const Argument &a : md->argumentList())
  {
    bindTextParameter(param_insert,":defname",a.type);
    int param_id=stepRowid(param_insert);
    if (param_id==-1) {
      continue;
    }

    bindIntParameter(memberdef_param_insert,":memberdef_id",memberdef_id);
    bindIntParameter(memberdef_param_insert,":param_id",param_id);
    step(memberdef_param_insert);
  }
}

static void insertMemberReimplements(const MemberDef *md, const Refid member_refid)
{
  const MemberDef *rmd = md->reimplements();
  if(!rmd)
    return;

  QCString qreimplemented_refid = rmd->getOutputFileBase() + "_1" + rmd->anchor();

  Refid reimplemented_refid = insertRefid(qreimplemented_refid);

  bindIntParameter(reimplements_insert,":memberdef_rowid", member_refid.rowid);
  bindIntParameter(reimplements_insert,":reimplemented_rowid", reimplemented_refid.rowid);
  stepRowid(reimplements_insert);
}

static void insertEnum(const MemberDef *md, Refid md_refid)
{
  for (const auto &emd : md->enumFieldList())
  {
    QCString qrefid = md->getOutputFileBase() + "_1" + emd->anchor();
    Refid refid = insertRefid(qrefid);
    insertClassMember(emd, refid, md_refid);

    bindIntParameter(memberdef_insert,":rowid", refid.rowid);
    bindTextParameter(memberdef_insert,":kind",emd->memberTypeName(),false);
    bindTextParameter(memberdef_insert,":name",emd->name(),false);
    bindIntParameter(memberdef_insert,":prot",static_cast<int>(md->protection()));
    // File location
    if (emd->getDefLine() != -1)
    {
      int deffile_id = insertPath(emd->getDefFileName(),!emd->isReference());
      if (deffile_id!=-1)
      {
        bindIntParameter(memberdef_insert,":deffile_id",deffile_id);
        bindIntParameter(memberdef_insert,":defline",emd->getDefLine());
        bindIntParameter(memberdef_insert,":defcolumn",emd->getDefColumn());
      }
    }
    step(memberdef_insert);
#if 0
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
#endif
  }
}

//////
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
static void insertMember(const MemberDef *md, Refid compounddef_refid, const Definition *def)
{
  // Skip EnumValues which are written by insertEnum()
  if (md->memberType()==MemberType::EnumValue)
    return;

  if (md->isHidden())
    return;

  QCString memType;

  QCString qrefid = md->getOutputFileBase() + "_1" + md->anchor();
  Refid refid = insertRefid(qrefid);

  if(compounddef_refid.isNew || !compounddef_exists(compounddef_refid))
    insertClassMember(md, refid, compounddef_refid);

  if(!refid.isNew && existsMemberDef(refid))
    return;

  bindIntParameter(memberdef_insert,":rowid", refid.rowid);
  bindTextParameter(memberdef_insert,":kind",md->memberTypeName(),false);
  bindIntParameter(memberdef_insert,":prot",static_cast<int>(md->protection()));
  bindIntParameter(memberdef_insert,":static",md->isStatic());
  bindMemberType(md);
  insertMemberReimplements(md, refid);
  bindTextParameter(memberdef_insert,":name",md->name(),false);
  bindTextParameter(memberdef_insert,":qualifiedname",md->qualifiedName(),false);

  if (!md->getScopeString().isEmpty())
    bindTextParameter(memberdef_insert,":scope",md->getScopeString(),false);

  bindMemberDefLocation(md);
  bindMemberDeclLocation(md);
  bindMemberBodyLocation(md);

  // + declaration/definition arg lists
  if (md->memberType()!=MemberType::Define
      && md->memberType()!=MemberType::Enumeration
     )
  {
    if (md->memberType()!=MemberType::Typedef)
    {
      insertMemberTemplateLists(md);
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

  // Extract references from initializer
  // The cross-references in initializers
  // only work when both the src and dst are defined.
  if (md->hasMultiLineInitializer()
      || md->hasOneLineInitializer())
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
              s.c_str(),
              qPrint(md->getBodyDef()->getDefFileName()),
              md->getStartBodyLine()));
        QCString qsrc_refid = md->getOutputFileBase() + "_1" + md->anchor();
        Refid src_refid = insertRefid(qsrc_refid);
        Refid dst_refid = insertRefid(s.c_str());
        insertMemberReference(src_refid,dst_refid, "initializer");
      }
    }
  }

  bindJsonParameter(memberdef_insert, ":briefdescription", md, md->briefDescription(), false);
  bindJsonParameter(memberdef_insert, ":detaileddescription", md, md->documentation(), false);

  int memberdef_id=stepRowid(memberdef_insert);

  if (isFunction(md))
  {
    insertFunctionArgs(memberdef_id,md,def);
  }
  else if (md->memberType()==MemberType::Define &&
      !md->argsString().isEmpty())
  {
    insertDefineArgs(memberdef_id,md,def);
  }
  else if (md->memberType()==MemberType::Enumeration)
  {
    insertEnum(md, compounddef_refid);
  }

  insertMemberReferences(md);
}
static bool memberVisible(const Definition *d,const MemberDef *md)
{
  return Config_getBool(XML_NS_MEMB_FILE_SCOPE) ||
    d->definitionType()!=Definition::TypeFile ||
    md->getNamespaceDef()==0;
}

static void insertSection(const Definition *d,
    const MemberList *ml,
    Refid compounddef_refid,
    const QCString &kind,
    const QCString &header=QCString(),
    const QCString &documentation=QCString())
{
  if (ml==0) return;
  int count=0;
  for (const auto &md : *ml)
  {
    if (memberVisible(d,md) && (md->memberType()!=MemberType::EnumValue) &&
        !md->isHidden())
    {
      count++;
    }
  }
  if (count==0) return; // empty list

  bindTextParameter(sectiondef_insert,":kind", kind,false);
  if (!header.isEmpty())
  {
    bindJsonParameter(sectiondef_insert, ":header", d, header, false);
  }
  if (!documentation.isEmpty())
  {
    bindJsonParameter(sectiondef_insert, ":description", d, documentation, false);
  }
  int rowid = stepRowid(sectiondef_insert);
  for (const auto &md : *ml)
  {
    if (memberVisible(d,md))
    {
      insertMember(md,compounddef_refid, d);
    }
  }
}

static void insertClassMembers(const ClassDef *cd, Refid compounddef_refid)
{
  for (auto &mni : cd->memberNameInfoLinkedMap())
  {
    for (auto &mi : *mni)
    {
      const MemberDef *md = mi->memberDef();
      QCString qrefid = md->getOutputFileBase() + "_1" + md->anchor();
      insertClassMember(md, insertRefid(qrefid), compounddef_refid);
    }
  }
}

static void insertIncludeInfo(int src_id, const IncludeInfo *ii)
{
  if (!ii)
    return;

  QCString nm = ii->includeName;
  if (nm.isEmpty() && ii->fileDef)
    nm = ii->fileDef->docName();
  if (nm.isEmpty())
    return;

  int dst_id=-1;
  if (ii->fileDef)
  {
    dst_id = insertPath(ii->fileDef->absFilePath(),!ii->fileDef->isReference());
    DBG_CTX(("-----> ClassDef IncludeInfo for %s\n", qPrint(nm)));
    DBG_CTX(("header: %s\n", ii->fileDef->absFilePath().data()));
    DBG_CTX(("       local    : %d\n", ii->local));
    DBG_CTX(("       imported : %d\n", ii->imported));
    DBG_CTX(("       deffile_id  : %d\n", src_id));
    DBG_CTX(("       dst_id: %d\n", dst_id));
  }

  bool isLocal = (ii->kind & IncludeKind_LocalMask)!=0;
  bindIntParameter(incl_select,":local",isLocal);
  bindIntParameter(incl_select,":src_id",src_id);
  bindIntParameter(incl_select,":dst_id",dst_id);
  if (stepRowid(incl_select,true)==0)
  {
    bindIntParameter(incl_insert,":local",isLocal);
    bindIntParameter(incl_insert,":src_id",src_id);
    bindIntParameter(incl_insert,":dst_id",dst_id);
    step(incl_insert);
  }
}

static void insertClass(const ClassDef *cd)
{
  // many kinds: category class enum exception interface
  // module protocol service singleton struct type union
  // enum is Java only (and is distinct from enum memberdefs)
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
  if (cd->templateMaster()!=nullptr)  return; // skip generated template instances.

  Refid refid = insertRefid(cd->getOutputFileBase());

  // skip class that already has a refid
  if(!refid.isNew && compounddef_exists(refid))
    return;

  bindIntParameter(compounddef_insert,":rowid",refid.rowid);
  bindTextParameter(compounddef_insert,":name",cd->name(),false);
  bindTextParameter(compounddef_insert,":title",cd->title(), false);
  bindTextParameter(compounddef_insert,":kind",cd->compoundTypeString(),false);
  bindTextParameter(compounddef_insert,":language", langToString(cd->getLanguage()),false);
  bindIntParameter(compounddef_insert,":prot",static_cast<int>(cd->protection()));

  int deffile_id = insertPath(cd->getDefFileName());
  bindIntParameter(compounddef_insert,":deffile_id",deffile_id);
  bindIntParameter(compounddef_insert,":defline",cd->getDefLine());
  bindIntParameter(compounddef_insert,":defcolumn",cd->getDefColumn());

  bindDefinitionHeader(compounddef_insert, cd->includeInfo());

  bindJsonParameter(compounddef_insert, ":briefdescription", cd, cd->briefDescription(), false);
  bindJsonParameter(compounddef_insert, ":detaileddescription", cd, cd->documentation(), false);

  step(compounddef_insert);

  // + list of direct super classes
  for (const auto &bcd : cd->baseClasses())
  {
    Refid base_refid = insertRefid(bcd.classDef->getOutputFileBase());
    Refid derived_refid = insertRefid(cd->getOutputFileBase());
    bindIntParameter(compoundref_insert,":base_rowid", base_refid.rowid);
    bindIntParameter(compoundref_insert,":derived_rowid", derived_refid.rowid);
    bindIntParameter(compoundref_insert,":prot",static_cast<int>(bcd.prot));
    bindIntParameter(compoundref_insert,":virt",static_cast<int>(bcd.virt));
    step(compoundref_insert);
  }

  // + list of direct sub classes
  for (const auto &bcd : cd->subClasses())
  {
    Refid derived_refid = insertRefid(bcd.classDef->getOutputFileBase());
    Refid base_refid = insertRefid(cd->getOutputFileBase());
    bindIntParameter(compoundref_insert,":base_rowid", base_refid.rowid);
    bindIntParameter(compoundref_insert,":derived_rowid", derived_refid.rowid);
    bindIntParameter(compoundref_insert,":prot",static_cast<int>(bcd.prot));
    bindIntParameter(compoundref_insert,":virt",static_cast<int>(bcd.virt));
    step(compoundref_insert);
  }

  // + list of inner classes
  insertInnerClasses(cd->getClasses(),refid);

  // + template argument list(s)
  insertTemplateList(cd);

  // + member groups
  for (const auto &mg : cd->getMemberGroups())
  {
    insertSection(cd,&mg->members(),refid,"user-defined",mg->header(),
        mg->documentation());
  }

  // this is just a list of *local* members
  for (const auto &ml : cd->getMemberLists())
  {
    if (!ml->listType().isDetailed())
    {
      insertSection(cd,ml.get(),refid,ml->listType().toXML());
    }
  }

  // + list of all members
  insertClassMembers(cd, refid);
}

static void insertConcept(const ConceptDef *cd)
{
  if (cd->isReference() || cd->isHidden()) return; // skip external references

  Refid refid = insertRefid(cd->getOutputFileBase());
  if(!refid.isNew && compounddef_exists(refid))
    return;

  bindIntParameter(compounddef_insert,":rowid", refid.rowid);
  bindTextParameter(compounddef_insert,":name",cd->name());
  bindTextParameter(compounddef_insert,":kind","concept");

  int file_id = insertPath(cd->getDefFileName());
  bindIntParameter(compounddef_insert,":file_id",file_id);
  bindIntParameter(compounddef_insert,":line",cd->getDefLine());
  bindIntParameter(compounddef_insert,":column",cd->getDefColumn());

  bindJsonParameter(compounddef_insert, ":briefdescription", cd, cd->briefDescription(), false);
  bindJsonParameter(compounddef_insert, ":detaileddescription", cd, cd->documentation(), false);

  step(compounddef_insert);

  // + template argument list(s)
  insertTemplateList(cd);
}

static void insertModule(const ModuleDef *mod)
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
  if(!refid.isNew && compounddef_exists(refid)){return;}
  bindIntParameter(compounddef_insert,":rowid", refid.rowid);
  bindTextParameter(compounddef_insert,":name",mod->name());
  bindTextParameter(compounddef_insert,":kind","module");

  int file_id = insertPath(mod->getDefFileName());
  bindIntParameter(compounddef_insert,":file_id",file_id);
  bindIntParameter(compounddef_insert,":line",mod->getDefLine());
  bindIntParameter(compounddef_insert,":column",mod->getDefColumn());

  bindJsonParameter(compounddef_insert, ":briefdescription", mod, mod->briefDescription(), false);
  bindJsonParameter(compounddef_insert, ":detaileddescription", mod, mod->documentation(), false);

  step(compounddef_insert);

  // + contained class definitions
  insertInnerClasses(mod->getClasses(),refid);

  // + contained concept definitions
  insertInnerConcepts(mod->getConcepts(),refid);

  // + member groups
  for (const auto &mg : mod->getMemberGroups())
  {
    insertSection(mod,&mg->members(),refid,"user-defined",mg->header(),
        mg->documentation());
  }

  // + normal members
  for (const auto &ml : mod->getMemberLists())
  {
    if (ml->listType().isDeclaration())
    {
      insertSection(mod,ml.get(),refid,"user-defined");
    }
  }

  // + files
  insertInnerFiles(mod->getUsedFiles(),refid);
}

// kinds: constants library module namespace package
static void insertNamespace(const NamespaceDef *nd)
{
  // + contained class definitions
  // + contained namespace definitions
  // + member groups
  // + normal members
  // + brief desc
  // + detailed desc
  // + location (file_id, line, column)
  // - files containing (parts of) the namespace definition

  // skip external references
  if (nd->isReference() || nd->isHidden())
    return;

  Refid refid = insertRefid(nd->getOutputFileBase());
  if(!refid.isNew && compounddef_exists(refid))
    return;

  bindIntParameter(compounddef_insert,":rowid",refid.rowid);
  bindTextParameter(compounddef_insert,":name",nd->name());
  bindTextParameter(compounddef_insert,":title",nd->title(),false);
  bindTextParameter(compounddef_insert,":kind","namespace",false);
  bindTextParameter(compounddef_insert,":language", langToString(nd->getLanguage()),false);

  int deffile_id = insertPath(nd->getDefFileName());
  bindIntParameter(compounddef_insert,":deffile_id",deffile_id);
  bindIntParameter(compounddef_insert,":defline",nd->getDefLine());
  bindIntParameter(compounddef_insert,":defcolumn",nd->getDefColumn());

  bindJsonParameter(compounddef_insert, ":briefdescription", nd, nd->briefDescription(), false);
  bindJsonParameter(compounddef_insert, ":detaileddescription", nd, nd->documentation(), false);

  step(compounddef_insert);

  // + contained class definitions
  insertInnerClasses(nd->getClasses(),refid);

  // + contained concept definitions
  insertInnerConcepts(nd->getConcepts(),refid);

  // + contained namespace definitions
  insertInnerNamespaces(nd->getNamespaces(),refid);

  // + member groups
  for (const auto &mg : nd->getMemberGroups())
  {
    insertSection(nd,&mg->members(),refid,"user-defined",mg->header(),
        mg->documentation());
  }

  // + normal members
  for (const auto &ml : nd->getMemberLists())
  {
    if (ml->listType().isDeclaration())
    {
      insertSection(nd,ml.get(),refid,ml->listType().toXML());
    }
  }
}

// kind: file
static void insertFile(const FileDef *fd)
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
  // + location (deffile_id, line, column)
  // - number of lines

  // skip external references
  if (fd->isReference())
    return;

  Refid refid = insertRefid(fd->getOutputFileBase());
  if(!refid.isNew && compounddef_exists(refid))
  {
    return;
  }

  bindIntParameter(compounddef_insert,":rowid",refid.rowid);
  bindTextParameter(compounddef_insert,":name",fd->name(),false);
  bindTextParameter(compounddef_insert,":title",fd->title(),false);
  bindTextParameter(compounddef_insert,":kind","file",false);
  bindTextParameter(compounddef_insert,":language", langToString(fd->getLanguage()),false);

  int deffile_id = insertPath(fd->getDefFileName());
  bindIntParameter(compounddef_insert,":deffile_id",deffile_id);
  bindIntParameter(compounddef_insert,":defline",fd->getDefLine());
  bindIntParameter(compounddef_insert,":defcolumn",fd->getDefColumn());

  bindJsonParameter(compounddef_insert, ":briefdescription", fd, fd->briefDescription(), false);
  bindJsonParameter(compounddef_insert, ":detaileddescription", fd, fd->documentation(), false);

  int rc = step(compounddef_insert);
  if (rc == -1)
  {
    printf("sqlite3_step: %s (rc: %d)\n", sqlite3_errmsg(compounddef_insert.db), rc);
    return;
  }
  printf("FILE insert [%s]\n", fd->getOutputFileBase().data());

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
      dst_id = insertPath(ii.includeName,isLocal,false);
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
    if (stepRowid(incl_select,true)==0)
    {
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
      src_id = insertPath(ii.includeName,isLocal,false);
    }

    bindIntParameter(incl_select,":local",isLocal);
    bindIntParameter(incl_select,":src_id",src_id);
    bindIntParameter(incl_select,":dst_id",dst_id);
    if (stepRowid(incl_select,true)==0)
    {
      bindIntParameter(incl_insert,":local",isLocal);
      bindIntParameter(incl_insert,":src_id",src_id);
      bindIntParameter(incl_insert,":dst_id",dst_id);
      step(incl_insert);
    }
  }

  // + contained class definitions
  insertInnerClasses(fd->getClasses(),refid);

  // + contained concept definitions
  insertInnerConcepts(fd->getConcepts(),refid);

  // + contained namespace definitions
  insertInnerNamespaces(fd->getNamespaces(),refid);

  // + member groups
  for (const auto &mg : fd->getMemberGroups())
  {
    insertSection(fd,&mg->members(),refid,"user-defined",mg->header(),
          mg->documentation());
  }

  // + normal members
  for (const auto &ml : fd->getMemberLists())
  {
    if (ml->listType().isDeclaration())
    {
      insertSection(fd,ml.get(),refid,ml->listType().toXML());
    }
  }
}

// kind: group
static void insertGroup(const GroupDef *gd)
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

  // skip external references.
  if (gd->isReference())
    return;

  Refid refid = insertRefid(gd->getOutputFileBase());
  if(!refid.isNew && compounddef_exists(refid))
    return;

  bindIntParameter(compounddef_insert,":rowid",refid.rowid);
  bindTextParameter(compounddef_insert,":name",gd->name());
  bindTextParameter(compounddef_insert,":title",gd->groupTitle(), false);
  bindTextParameter(compounddef_insert,":kind","group",false);

  int deffile_id = insertPath(gd->getDefFileName());
  bindIntParameter(compounddef_insert,":deffile_id",deffile_id);
  bindIntParameter(compounddef_insert,":defline",gd->getDefLine());
  bindIntParameter(compounddef_insert,":defcolumn",gd->getDefColumn());

  bindJsonParameter(compounddef_insert, ":briefdescription", gd, gd->briefDescription(), false);
  bindJsonParameter(compounddef_insert, ":detaileddescription", gd, gd->documentation(), false);

  step(compounddef_insert);

  // + files
  insertInnerFiles(gd->getFiles(),refid);

  // + classes
  insertInnerClasses(gd->getClasses(),refid);

  // + concepts
  insertInnerConcepts(gd->getConcepts(),refid);

  // + modules
  insertInnerModules(gd->getModules(),refid);

  // + namespaces
  insertInnerNamespaces(gd->getNamespaces(),refid);

  // + pages
  insertInnerPages(gd->getPages(),refid);

  // + groups
  insertInnerGroups(gd->getSubGroups(),refid);

  // + member groups
  for (const auto &mg : gd->getMemberGroups())
  {
    insertSection(gd,&mg->members(),refid,"user-defined",mg->header(),
        mg->documentation());
  }

  // + members
  for (const auto &ml : gd->getMemberLists())
  {
    if (ml->listType().isDeclaration())
    {
      insertSection(gd,ml.get(),refid,ml->listType().toXML());
    }
  }
}

// kind: dir
static void insertDir(const DirDef *dd)
{
  // + dirs
  // + files
  // + briefdescription
  // + detaileddescription
  // + location (below uses file_id, line, column; XML just uses file)

  // skip external references
  if (dd->isReference())
    return;

  Refid refid = insertRefid(dd->getOutputFileBase());
  if(!refid.isNew && compounddef_exists(refid))
    return;

  bindIntParameter(compounddef_insert,":rowid",refid.rowid);
  bindTextParameter(compounddef_insert,":name",dd->displayName());
  bindTextParameter(compounddef_insert,":kind","dir",false);

  int deffile_id = insertPath(dd->getDefFileName(),true,true,PATH_TYPE_DIR);
  bindIntParameter(compounddef_insert,":deffile_id",deffile_id);

  //line and column are weird here, but:
  //- dir goes into compounddef with all of the others
  //- the semantics would be fine if we set them to nullptr here
  //- but defining line and column as NOT nullptr is an important promise
  //for other compounds, so I don't want to loosen it

  //For reference, the queries return 1.
  //0 or -1 make more sense, but I see that as a change for DirDef.
  bindIntParameter(compounddef_insert,":defline",dd->getDefLine());
  bindIntParameter(compounddef_insert,":defcolumn",dd->getDefColumn());

  bindJsonParameter(compounddef_insert, ":briefdescription", dd, dd->briefDescription(), false);
  bindJsonParameter(compounddef_insert, ":detaileddescription", dd, dd->documentation(), false);

  step(compounddef_insert);

  // + dirs
  insertInnerDirs(dd->subDirs(),refid);

  // + files
  insertInnerFiles(dd->getFiles(),refid);
}

// kinds: page, example
static void insertPage(const PageDef *pd,bool isExample)
{
  // + name
  // + title
  // + brief description
  // + documentation (detailed description)
  // + inbody documentation
  // + sub pages

  // skip external references.
  if (pd->isReference())
    return;

  QCString qrefid = pd->getOutputFileBase();
  if (pd->getGroupDef())
  {
    qrefid+="_"+pd->name();
  }

  // prevent overwriting the generated index page.
  if (qrefid=="index")
    qrefid="indexpage";

  Refid refid = insertRefid(qrefid);

  // can omit a page that already has a refid
  if(!refid.isNew && compounddef_exists(refid))
    return;

  bindIntParameter(compounddef_insert,":rowid",refid.rowid);
  // + name
  bindTextParameter(compounddef_insert,":name",pd->name(),false);

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
  bindTextParameter(compounddef_insert,":title",title,false);

  bindTextParameter(compounddef_insert,":kind", isExample ? "example" : "page");

  int deffile_id = insertPath(pd->getDefFileName());

  bindIntParameter(compounddef_insert,":deffile_id",deffile_id);
  bindIntParameter(compounddef_insert,":defline",pd->getDefLine());
  bindIntParameter(compounddef_insert,":defcolumn",pd->getDefColumn());

  // + brief description
  bindJsonParameter(compounddef_insert, ":briefdescription",pd,pd->briefDescription(),false);
  // + documentation (detailed description)
  bindJsonParameter(compounddef_insert, ":detaileddescription",pd,pd->documentation(),false);

  step(compounddef_insert);
  // + sub pages
  insertInnerPages(pd->getSubPages(),refid);
}


static void insertProject(void)
{
  bindTextParameter(project_insert,":name",Config_getString(PROJECT_NAME));
  bindTextParameter(project_insert,":number",Config_getString(PROJECT_NUMBER));
  bindTextParameter(project_insert,":brief",Config_getString(PROJECT_BRIEF));
  step(project_insert);
}

  // + classes
static void insertClasses(void)
{
  for (const auto &cd : *Doxygen::classLinkedMap)
  {
    msg("Generating Sqlite3 output for class %s\n",qPrint(cd->name()));
    insertClass(cd.get());
  }
}

  // + concepts
static void insertConcepts(void)
{
  for (const auto &cd : *Doxygen::conceptLinkedMap)
  {
    msg("Generating Sqlite3 output for concept %s\n",qPrint(cd->name()));
    insertConcept(cd.get());
  }
}

  // + modules
static void insertModules(void)
{
  for (const auto &mod : ModuleManager::instance().modules())
  {
    msg("Generating Sqlite3 output for module %s\n",qPrint(mod->name()));
    insertModule(mod.get());
  }
}

  // + namespaces
static void insertNamespaces(void)
{
  for (const auto &nd : *Doxygen::namespaceLinkedMap)
  {
    msg("Generating Sqlite3 output for namespace %s\n",qPrint(nd->name()));
    insertNamespace(nd.get());
  }
}

  // + files
static void insertFiles(void)
{
  for (const auto &fn : *Doxygen::inputNameLinkedMap)
  {
    for (const auto &fd : *fn)
    {
      msg("Generating Sqlite3 output for file %s\n",qPrint(fd->name()));
      insertFile(fd.get());
    }
  }
}

  // + groups
static void insertGroups(void)
{
  for (const auto &gd : *Doxygen::groupLinkedMap)
  {
    msg("Generating Sqlite3 output for group %s\n",qPrint(gd->name()));
    insertGroup(gd.get());
  }
}

  // + page
static void insertPages(void)
{
  for (const auto &pd : *Doxygen::pageLinkedMap)
  {
    msg("Generating Sqlite3 output for page %s\n",qPrint(pd->name()));
    insertPage(pd.get(),false);
  }
}

  // + dirs
static void insertDirs(void)
{
  for (const auto &dd : *Doxygen::dirLinkedMap)
  {
    msg("Generating Sqlite3 output for dir %s\n",qPrint(dd->name()));
    insertDir(dd.get());
  }
}

  // + examples
static void insertExamples(void)
{
  for (const auto &pd : *Doxygen::exampleLinkedMap)
  {
    msg("Generating Sqlite3 output for example %s\n",qPrint(pd->name()));
    insertPage(pd.get(),true);
  }
}

  // + main page
static void insertMainPage(void)
{
  if (Doxygen::mainPage)
  {
    msg("Generating Sqlite3 output for the main page\n");
    insertPage(Doxygen::mainPage.get(),false);
  }
}
//}


//{ SQL Transaction
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
//}


//{ PRAGMA
static void pragmaTuning(sqlite3 *db)
{
  char * sErrMsg = nullptr;
  sqlite3_exec(db, "PRAGMA synchronous = OFF", nullptr, nullptr, &sErrMsg);
  sqlite3_exec(db, "PRAGMA journal_mode = MEMORY", nullptr, nullptr, &sErrMsg);
  sqlite3_exec(db, "PRAGMA temp_store = MEMORY;", nullptr, nullptr, &sErrMsg);
}
//}


//{ Schema setup
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
      err("failed to execute query: %s\n\t%s\n", q, errmsg);
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
      err("failed to execute query: %s\n\t%s\n", q, errmsg);
      return -1;
    }
  }
  return 0;
}
//}


//{ SQL Connection
static sqlite3* openDbConnection(void)
{
  QCString outputDirectory = Config_getString(SQLITE3_OUTPUT);
  sqlite3 *db;

  int rc = sqlite3_initialize();
  if (rc != SQLITE_OK)
  {
    err("sqlite3_initialize failed\n");
    return nullptr;
  }

  FileInfo fi(outputDirectory.str()+"/doxygen_sqlite3.db");
  if (fi.exists())
  {
    if (Config_getBool(SQLITE3_RECREATE_DB))
    {
      Dir().remove(fi.absFilePath());
    }
    else
    {
      err("doxygen_sqlite3.db already exists! aborting sqlite3 output generation!\n");
      err("If you wish to re-generate the database, remove or archive the existing copy first.\n");
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
    err("sqlite3_open_v2 failed: %s\n", fi.absFilePath().c_str());
    return nullptr;
  }
  return db;
}

static sqlite3* getDbConnection(void)
{
  static sqlite3* db = nullptr;

  if (db)
    return db;

  db = openDbConnection();

  return db;
}
//}


//{ Prepare tools
static int prepareStatement(sqlite3 *db, SqlStmt &s)
{
  int rc = sqlite3_prepare_v2(db,s.query,-1,&s.stmt,nullptr);
  if (rc!=SQLITE_OK)
  {
    err("prepare failed for:\n  %s\n  %s\n", s.query, sqlite3_errmsg(db));
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
  -1==prepareStatement(db, project_insert) ||
  -1==prepareStatement(db, memberdef_select) ||
  -1==prepareStatement(db, memberdef_insert) ||
  -1==prepareStatement(db, compoundmembers_select) ||
  -1==prepareStatement(db, compoundmembers_insert) ||
  -1==prepareStatement(db, path_insert) ||
  -1==prepareStatement(db, path_select) ||
  -1==prepareStatement(db, refid_insert) ||
  -1==prepareStatement(db, refid_select) ||
  -1==prepareStatement(db, incl_insert)||
  -1==prepareStatement(db, incl_select)||
  -1==prepareStatement(db, param_insert) ||
  -1==prepareStatement(db, param_select) ||
  -1==prepareStatement(db, xrefs_select) ||
  -1==prepareStatement(db, xrefs_insert) ||
  -1==prepareStatement(db, reimplements_insert) ||
  -1==prepareStatement(db, contains_insert) ||
  -1==prepareStatement(db, compounddef_select) ||
  -1==prepareStatement(db, compounddef_insert) ||
  -1==prepareStatement(db, sectiondef_exists) ||
  -1==prepareStatement(db, sectiondef_insert) ||
  -1==prepareStatement(db, compoundref_insert) ||
  -1==prepareStatement(db, memberdef_param_insert) ||
  -1==prepareStatement(db, memberdef_param_view_select)
  )
  {
    return -1;
  }
  return 0;
}
//}


//////////////////////////////////////////////////////////////////////////////
void generateSqlite3(void)
{
  sqlite3 *db = getDbConnection();

  if (db==nullptr)
    return;

# ifdef SQLITE3_DEBUG
  // debug: show all executed statements
  sqlite3_trace(db, &sqlLog, nullptr);
# endif

  beginTransaction(db);
  pragmaTuning(db);

  if (-1==initializeTables(db))
  {
    err("sqlite3: initializeTables failed!\n");
    return;
  }

  if (-1==initializeViews(db))
  {
    err("sqlite3: initializeViews failed!\n");
    return;
  }

  if (-1==prepareStatements(db) )
  {
    err("sqlite3: prepareStatements failed!\n");
    return;
  }

  insertMeta();
  insertProject();
  insertClasses();
  insertConcepts();
  insertModules();
  insertNamespaces();
  insertFiles();
  insertGroups();
  insertPages();
  insertDirs();
  insertExamples();
  insertMainPage();

  endTransaction(db);
}

// vim: noai:ts=2:sw=2:ss=2:expandtab
