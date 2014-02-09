#!/usr/bin/python

# python script to search through doxygen_sqlite3.db
#
# Permission to use, copy, modify, and distribute this software and its
# documentation under the terms of the GNU General Public License is hereby
# granted. No representations are made about the suitability of this software
# for any purpose. It is provided "as is" without express or implied warranty.
# See the GNU General Public License for more details.
#

import sqlite3
import sys
import os
import getopt
import json

class MemberType:
  Define="0"
  Function="1"
  Variable="2"
  Typedef="3"
  Enumeration="4"
  EnumValue="5"
  Signal="6"
  Slot="7"
  Friend="8"
  DCOP="9"
  Property="10"
  Event="11"

class RequestType:
  References="9901"
  Struct="9902"
  Includers="9903"
  Includees="9904"
  Members="9905"
  BaseClasses="9906"
  SubClasses="9907"

g_conn=None
###############################################################################

def escapeLike(val):
    return 'LIKE "%' + val.replace("\\", "\\\\").replace("_", "\\_") \
        .replace("%", "\\%") + '%" ESCAPE "\\"'

def matchName(name):
    if type(name) is str:
        return "name "+escapeLike(name)
    else:
        return 'id=%d' %name

def fileName(id_file):
    if g_conn.execute("SELECT COUNT(*) FROM files WHERE id=?",[id_file]).fetchone()[0] > 1:
        print "non-uniq fileid"

    for r in g_conn.execute("SELECT * FROM files WHERE id=?",[id_file]).fetchall():
            return r['name']

    return ""

def fileId(name):
    if g_conn.execute("SELECT COUNT(*) FROM files WHERE name=?",[name]).fetchone()[0] > 1:
        print "non-uniq file name"

    for r in g_conn.execute("SELECT * FROM files WHERE name=?",[name]).fetchall():
            return r['id']

    return -1

###############################################################################

def findReferences(name):
    o=[]

    cur = g_conn.cursor()
    cur.execute("SELECT refid FROM memberdef WHERE name=?",[name])
    refids = cur.fetchall()

    if len(refids) == 0:
        return o

    refid = refids[0]['refid']
    cur = g_conn.cursor()
    for info in cur.execute("SELECT * FROM xrefs WHERE dst LIKE '%"+refid+"%'"):
        item={}
        cur = g_conn.cursor()
        for i2 in cur.execute("SELECT * FROM memberdef WHERE refid=?",[info['src']]):
            item['name']=i2['name']
            item['src']=info['src']
            item['file']=fileName(info['id_file'])
            item['line']=info['line']

        o.append(item)
    return o


def findFunction(name):
    o=[]
    for r in g_conn.execute('SELECT * FROM memberdef WHERE '+matchName(name)+' AND kind=?',[MemberType.Function]).fetchall():
        item={}
        item['name'] = r['name']
        item['definition'] = r['definition']
        item['argsstring'] = r['argsstring']
        item['file'] = fileName(r['id_file'])
        item['line'] = r['line']
        item['detaileddescription'] = r['detaileddescription']
        o.append(item)
    return o


def findMacro(name):
    o=[]
    for r in g_conn.execute('SELECT * FROM memberdef WHERE '+matchName(name)+' AND kind=?',[MemberType.Define]).fetchall():
        item={}
        item['name'] = r['name']
        if r['argsstring']:
            item['argsstring'] = r['argsstring']
        item['definition'] = r['initializer']
        item['file'] = fileName(r['id_file'])
        item['line'] = r['line']
        o.append(item)
    return o


def findTypedef(name):
    o=[]
    for r in g_conn.execute('SELECT * FROM memberdef WHERE '+matchName(name)+' AND kind=?',[MemberType.Typedef]).fetchall():
        item={}
        item['name'] = r['name']
        item['definition'] = r['definition']
        item['file'] = fileName(r['id_file'])
        item['line'] = r['line']
        o.append(item)
    return o


def findVariable(name):
    o=[]
    for r in g_conn.execute('SELECT * FROM memberdef WHERE '+matchName(name)+' AND kind=?',[MemberType.Variable]).fetchall():
        item={}
        item['name'] = r['name']
        item['definition'] = r['definition']
        item['file'] = fileName(r['id_file'])
        item['line'] = r['line']
        o.append(item)
    return o

def findParams(name):
    o=[]
    for r in g_conn.execute('SELECT id FROM memberdef WHERE '+matchName(name)).fetchall():
        #a=("SELECT * FROM params where id=(SELECT id_param FROM memberdef_params where id_memberdef=?",[id_memberdef])
        item={}
        item['id'] = r['id']
        o.append(item)
    return o


def findStruct(name):
    o=[]
    for r in g_conn.execute('SELECT * FROM compounddef WHERE '+matchName(name)).fetchall():
        item={}
        item['name'] = r['name']
        o.append(item)
    return o

def findIncluders(name):
    o=[]
    fid = fileId(name)
    for r in g_conn.execute('SELECT * FROM includes WHERE id_dst=?',[fid]).fetchall():
        item={}
        item['name'] = fileName(r['id_src'])
        o.append(item)
    return o

def findIncludees(name):
    o=[]
    fid = fileId(name)
    for r in g_conn.execute('SELECT * FROM includes WHERE id_src=?',[fid]).fetchall():
        item={}
        item['name'] = r['dst']
        o.append(item)
    return o


def findMembers(name):
    o=[]
    for r in g_conn.execute('SELECT * FROM memberdef WHERE scope LIKE "%'+name+'%";').fetchall():
        item={}
        item['name'] = r['name']
        item['definition'] = r['definition']
        item['argsstring'] = r['argsstring']
        item['file'] = fileName(r['id_file'])
        item['line'] = r['line']
        item['documentation'] = r['documentation']
        o.append(item)
    return o


def findBaseClasses(name):
    o=[]
    for r in g_conn.execute('SELECT base FROM basecompoundref WHERE derived=?',[name]).fetchall():
        item={}
        item['name'] = r['base']
        o.append(item)
    return o


def findSubClasses(name):
    o=[]
    for r in g_conn.execute('SELECT derived FROM basecompoundref WHERE base=?',[name]).fetchall():
        item={}
        item['name'] = r['derived']
        o.append(item)
    return o


###############################################################################

def usage():
  print """Usage: search.py [Options]
Options:
    -h, --help
    -d <D>    Use database <D> for queries
    -r <F>    Search for references to <F>
    -f <F>    Search for definition of function <F>
    -m <M>    Search for definition of macro <M>
    -t <T>    Search for definition of type <T>
    -v <V>    Search for definition of variable <V>
    -I <I>    Get the includers of <I>
    -i <I>    Get the includees of <I>
    -M <C>    Get all members of class <C>
    -B <C>    Get the base classes of class <C>
    -S <C>    Get the sub classes of class <C>
"""

def openDb(dbname):
    global g_conn

    if dbname == None:
        dbname = "doxygen_sqlite3.db"

    if not os.path.isfile(dbname):
        raise BaseException("No such file %s" % dbname )

    g_conn = sqlite3.connect(dbname)
    g_conn.execute('PRAGMA temp_store = MEMORY;')
    g_conn.row_factory = sqlite3.Row

###############################################################################
def process(kind,o):
    request_processors = {
        MemberType.Function: findFunction,
        MemberType.Define:   findMacro,
        MemberType.Variable: findVariable,
        MemberType.Typedef:  findTypedef,
        RequestType.References: findReferences,
        RequestType.Struct: findStruct,
        RequestType.Includers: findIncluders,
        RequestType.Includees: findIncludees,
        RequestType.Members: findMembers,
        RequestType.BaseClasses: findBaseClasses,
        RequestType.SubClasses: findSubClasses
    }
    return request_processors[kind](o)


def processHref(ref):
    j={}

    # is it in memberdef ?
    table="memberdef"
    if ( g_conn.execute("SELECT count(*) from %s WHERE refid='%s'" % (table,ref) ).fetchone()[0] > 0 ):
        for r in g_conn.execute("SELECT kind,id FROM %s WHERE refid='%s'" % (table,ref) ).fetchall():
            j=process(str(r['kind']),int(r['id']))

    # is it in compounddef ?
    table="compounddef"
    if ( g_conn.execute("SELECT count(*) from %s WHERE refid='%s'" % (table,ref)).fetchone()[0] > 0 ):
        for r in g_conn.execute("SELECT id FROM %s WHERE refid='%s'" % (table,ref) ).fetchall():
            j=process(RequestType.Struct,int(r['id']))

    return j


def serveCgi():
    import cgi

    print 'Content-Type: application/json\n'

    fieldStorage = cgi.FieldStorage()
    form = dict((key, fieldStorage.getvalue(key)) for key in fieldStorage.keys())

    if 'href' in form:
        ref = form['href']
    else:
        print '{"result": null, "error": "no refid given"}'
        sys.exit(0)

    openDb('doxygen_sqlite3.db')

    j = processHref(ref)

    print json.dumps({"result":j,"error":None})


def serveCli(argv):
    try:
        opts, args = getopt.getopt(argv, "hr:I:i:d:f:m:t:v:H:M:B:S:",["help"])
    except getopt.GetoptError:
        usage()
        sys.exit(1)

    ref=None
    dbname=None
    j={}

    for a, o in opts:
        if a in ('-h', '--help'):
            usage()
            sys.exit(0)
        elif a in ('-d'):
            dbname=o
            continue
        elif a in ('-r'):
            kind=RequestType.References
        elif a in ('-I'):
            kind=RequestType.Includers
        elif a in ('-i'):
            kind=RequestType.Includees
        elif a in ('-M'):
            kind=RequestType.Members
        elif a in ('-B'):
            kind=RequestType.BaseClasses
        elif a in ('-S'):
            kind=RequestType.SubClasses
        elif a in ('-f'):
            kind=MemberType.Function
        elif a in ('-m'):
            kind=MemberType.Define
        elif a in ('-t'):
            kind=MemberType.Typedef
        elif a in ('-v'):
            kind=MemberType.Variable
        elif a in ('-H'):
            ref = o

        openDb(dbname)
        if ref != None:
          j=processHref(ref)
        else:
          j=process(kind,o)
        print json.dumps(j)


def main(argv):
    if 'REQUEST_METHOD' in os.environ:
        serveCgi()
    else:
        serveCli(argv)

if __name__ == '__main__':
    main(sys.argv[1:])
