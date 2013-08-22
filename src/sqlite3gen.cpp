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

static void generateSqlite3ForMember(sqlite3*db,MemberDef *md,Definition *def);

//////////////////////////////////////////////////////
const char *i_q_includes="INSERT OR REPLACE INTO includes "
                            "( local, id_src, dst ) "
                            "VALUES "
                            "(:local,:id_src,:dst )" ;
const char *c_q_includes="SELECT count(*) FROM includes WHERE local=:local and id_src=:id_src and dst=:dst";
static sqlite3_stmt *c_s_includes=0;
static sqlite3_stmt *i_s_includes=0;
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
const char *id_q_files="SELECT id FROM files WHERE name=:name";
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
                            "( refid, prot, static, const, explicit, inline, final, sealed, new, optional, required, virt, mutable, initonly, readable, writable, gettable, settable, accessor, addable, removable, raisable, name, type, definition, argsstring, scope, initializer, kind, id_bfile, bline, bcolumn, id_file, line, column)"
                            "VALUES "
                            "(:refid,:prot,:static,:const,:explicit,:inline,:final,:sealed,:new,:optional,:required,:virt,:mutable,:initonly,:readable,:writable,:gettable,:settable,:accessor,:addable,:removable,:raisable,:name,:type,:definition,:argsstring,:scope,:initializer,:kind,:id_bfile,:bline,:bcolumn,:id_file,:line,:column)";
const char *id_q_memberdef="SELECT id FROM memberdef WHERE refid=:refid and id is not null";
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
      "local INTEGER NOT NULL,"
      "id_src INTEGER NOT NULL,"
      "dst TEXT NOT NULL)"
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
      "initializer TEXT,"
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


static void bindTextParameter(sqlite3_stmt *stmt,const char *name,const char *value, bool _static=TRUE)
{
  int idx = sqlite3_bind_parameter_index(stmt, name);
  sqlite3_bind_text(stmt, idx, value, -1, _static==TRUE?SQLITE_STATIC:SQLITE_TRANSIENT);
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
    msg("sqlite3_step failed: %s\n", sqlite3_errmsg(db));
    return -1;
  }
  if (getRowId) id = sqlite3_column_int(stmt, 0);
  sqlite3_reset(stmt);
  return id;
}

static int insertFile(sqlite3 *db, const char* name)
{
  int id=-1;
  if (name==0) return -1;

  // see if it's already in DB
  bindTextParameter(id_s_files,":name",name);
  id=step(db,id_s_files,TRUE);
  if (id==0)
  {
    // insert it
    bindTextParameter(i_s_files,":name",name);

    step(db,i_s_files);
    id=sqlite3_last_insert_rowid(db);
  }
  return id;
}

static int insertMemberReference(sqlite3 *db, const char*src, const char*dst, const char *file, int line, int column)
{
  int id_file = insertFile(db,file);
  bindTextParameter(i_s_xrefs,":src",src);
  bindTextParameter(i_s_xrefs,":dst",dst);

  bindIntParameter(i_s_xrefs,":id_file",id_file);
  bindIntParameter(i_s_xrefs,":line",line);
  bindIntParameter(i_s_xrefs,":column",column);

return step(db,i_s_xrefs);
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

static int prepareStatement(sqlite3 *db, const char* query, sqlite3_stmt **statement)
{
  int rc;
  rc = sqlite3_prepare_v2(db,query,-1,statement,0);
  if (rc!=SQLITE_OK)
  {
    msg("prepare failed for %s\n%s\n", query, sqlite3_errmsg(db));
    return -1;
  }
  return rc;
}

static int prepareStatements(sqlite3 *db)
{
  if (
  -1==prepareStatement(db, id_q_memberdef, &id_s_memberdef) ||
  -1==prepareStatement(db, id_q_files, &id_s_files) ||
  -1==prepareStatement(db, i_q_files, &i_s_files) ||
  -1==prepareStatement(db, i_q_xrefs, &i_s_xrefs) ||
  -1==prepareStatement(db, i_q_innerclass, &i_s_innerclass) ||
  -1==prepareStatement(db, i_q_memberdef, &i_s_memberdef) ||
  -1==prepareStatement(db, i_q_compounddef, &i_s_compounddef) ||
  -1==prepareStatement(db, i_q_basecompoundref, &i_s_basecompoundref) ||
  -1==prepareStatement(db, i_q_derivedcompoundref, &i_s_derivedcompoundref) ||
  -1==prepareStatement(db, i_q_includes, &i_s_includes)||
  -1==prepareStatement(db, c_q_includes, &c_s_includes)
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
}

static int initializeSchema(sqlite3* db)
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
      bindTextParameter(i_s_innerclass,":refid",cd->getOutputFileBase(),FALSE);
      bindIntParameter(i_s_innerclass,":prot",cd->protection());
      bindTextParameter(i_s_innerclass,":name",cd->name());
      if (-1==step(db,i_s_innerclass))
          return;
    }
  }
}

static void writeInnerNamespaces(sqlite3 * /*db*/,const NamespaceSDict *nl)
{
#warning WorkInProgress
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
#warning WorkInProgress
}

static void writeTemplateList(sqlite3*db,ClassDef *cd)
{
  writeTemplateArgumentList(db,cd->templateArguments(),cd,0,4);
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
  msg("=====%s\n",md->name().data());

  // enum values are written as part of the enum
  if (md->memberType()==MemberType_EnumValue) return;
  if (md->isHidden()) return;
  //if (md->name().at(0)=='@') return; // anonymous member

  // group members are only visible in their group
  //if (def->definitionType()!=Definition::TypeGroup && md->getGroupDef()) return;
  QCString memType;
  // memberdef
  bindTextParameter(i_s_memberdef,":refid",md->anchor().data(),FALSE);
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
    ArgumentList *al = md->argumentList();
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

  // + declaration/definition arg lists
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
      bindTextParameter(i_s_memberdef,":type",typeStr.data(),FALSE);
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
  if (isFunc) //function
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
    bindTextParameter(i_s_memberdef,":initializer",md->initializer().data());

    StringList l;
    linkifyText(TextGeneratorSqlite3Impl(l),def,md->getBodyDef(),md,md->initializer());
    QCString *s=l.first();
    while (s)
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
      s=l.next();
    }
  }

  if ( md->getScopeString() )
  {
    bindTextParameter(i_s_memberdef,":scope",md->getScopeString().data(),FALSE);
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
        if (id_bfile == -1)
        {
            sqlite3_clear_bindings(i_s_memberdef);
            return;
        }

        bindIntParameter(i_s_memberdef,":id_ibfile",id_bfile);
        bindIntParameter(i_s_memberdef,":bline",md->getStartBodyLine());

        // XXX implement getStartBodyColumn
        bindIntParameter(i_s_memberdef,":bcolumn",1);
      }
    }
  }

  if (-1==step(db,i_s_memberdef))
  {
      sqlite3_clear_bindings(i_s_memberdef);
  }
  /*int id_src =*/ sqlite3_last_insert_rowid(db);

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
    msg("I:%s\n",md->name().data());
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

  //int idx,rc;
  //sqlite3_stmt *stmt ;

  // + compounddef
  //stmt = i_s_compounddef;

  bindTextParameter(i_s_compounddef,":name",cd->name());
  bindTextParameter(i_s_compounddef,":kind",cd->compoundTypeString(),FALSE);
  bindIntParameter(i_s_compounddef,":prot",cd->protection());
  bindTextParameter(i_s_compounddef,":refid",cd->getOutputFileBase(),FALSE);

  int id_file = insertFile(db,cd->getDefFileName().data());
  bindIntParameter(i_s_compounddef,":id_file",id_file);
  bindIntParameter(i_s_compounddef,":line",cd->getDefLine());
  bindIntParameter(i_s_compounddef,":column",cd->getDefColumn());

  if (-1==step(db,i_s_compounddef))
    return;

  // + list of direct super classes
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
      if (-1==step(db,i_s_basecompoundref))
        continue;
    }
  }

  // + list of direct sub classes
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
      if (-1==step(db,i_s_derivedcompoundref))
        continue;
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
      bindIntParameter(c_s_includes,":local",ii->local);
      bindIntParameter(c_s_includes,":id_src",id_file);
      bindTextParameter(c_s_includes,":dst",nm);
      int count=step(db,c_s_includes,TRUE);
      if (count==0)
      {
        bindIntParameter(i_s_includes,":local",ii->local);
        bindIntParameter(i_s_includes,":id_src",id_file);
        bindTextParameter(i_s_includes,":dst",nm);
        if (-1==step(db,i_s_includes))
          return;
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
      int id_file=insertFile(db,fd->absFilePath().data());
      bindIntParameter(i_s_includes,":local",ii->local);
      bindIntParameter(i_s_includes,":id_src",id_file);
      bindTextParameter(i_s_includes,":dst",ii->includeName.data(),FALSE);
      if (-1==step(db,i_s_includes))
        continue;
    }
  }

  // + includedby files
  if (fd->includedByFileList())
  {
    QListIterator<IncludeInfo> ili(*fd->includedByFileList());
    for (ili.toFirst();(ii=ili.current());++ili)
    {
      int id_file=insertFile(db,ii->includeName);
      bindIntParameter(i_s_includes,":local",ii->local);
      bindIntParameter(i_s_includes,":id_src",id_file);
      bindTextParameter(i_s_includes,":dst",fd->absFilePath().data(),FALSE);
      if (-1==step(db,i_s_includes))
        continue;

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
    db=db;
    gd=gd;
}

static void generateSqlite3ForDir(sqlite3 *db,DirDef *dd)
{
}

static void generateSqlite3ForPage(sqlite3 *db,PageDef *pd,bool isExample)
{
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

  QCString outputDirectory = Config_getString("OUTPUT_DIRECTORY");
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

