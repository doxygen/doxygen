#!/usr/bin/env python

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
import re

class MemberType:
    Define="macro definition"
    Function="function"
    Variable="variable"
    Typedef="typedef"
    Enumeration="enumeration"
    EnumValue="enumvalue"
    Signal="signal"
    Slot="slot"
    Friend="friend"
    DCOP="dcop"
    Property="property"
    Event="event"
    File="file"

class RequestType:
    References="9901"
    Struct="9902"
    Includers="9903"
    Includees="9904"
    Members="9905"
    BaseClasses="9906"
    SubClasses="9907"

g_use_regexp=False
###############################################################################

# case-insensitive sqlite regexp function
def re_fn(expr, item):
    reg = re.compile(expr, re.I)
    return reg.search(item) is not None

def openDb(dbname):
    if dbname is None:
        dbname = "doxygen_sqlite3.db"

    if not os.path.isfile(dbname):
        raise BaseException("No such file %s" % dbname )

    conn = sqlite3.connect(dbname)
    conn.execute('PRAGMA temp_store = MEMORY;')
    conn.row_factory = sqlite3.Row
    conn.create_function("REGEXP", 2, re_fn)
    return conn
###############################################################################
class Finder:
    def __init__(self,cn,name,row_type=str):
        self.cn=cn
        self.name=name
        self.row_type=row_type

    def match(self,row):
        if self.row_type is int:
            return " rowid=?"
        else:
            if g_use_regexp:
                return " REGEXP (?,%s)" %row
            else:
                return " %s=?" %row

    def fileName(self,file_id):
        if self.cn.execute("SELECT COUNT(*) FROM path WHERE rowid=?",[file_id]).fetchone()[0] > 1:
            sys.stderr.write("WARNING: non-uniq fileid [%s]. Considering only the first match." % file_id)

        for r in self.cn.execute("SELECT * FROM path WHERE rowid=?",[file_id]).fetchall():
                return r['name']

        return ""

    def fileId(self,name):
        if self.cn.execute("SELECT COUNT(*) FROM path WHERE"+self.match("name"),[name]).fetchone()[0] > 1:
            sys.stderr.write("WARNING: non-uniq file name [%s]. Considering only the first match." % name)

        for r in self.cn.execute("SELECT rowid FROM path WHERE"+self.match("name"),[name]).fetchall():
                return r[0]

        return -1
###############################################################################
    def references(self):
        o=[]
        cur = self.cn.cursor()
        cur.execute("SELECT rowid FROM memberdef WHERE"+self.match("name"),[self.name])
        rowids = cur.fetchall()

        if len(rowids) == 0:
            return o

        rowid = rowids[0]['rowid']
        cur = self.cn.cursor()
        #TODO:SELECT rowid from refid where refid=refid
        for info in cur.execute("SELECT * FROM xrefs WHERE dst_rowid=?", [rowid]):
            item={}
            cur = self.cn.cursor()
            for i2 in cur.execute("SELECT * FROM memberdef WHERE rowid=?",[info['src_rowid']]):
                item['name']=i2['name']
                item['src']=info['src_rowid']
                # Below no longer directly supported on this entry; can be found from either memberdef
                #item['file']=self.fileName(info['file_id'])
                #item['line']=info['line']

            o.append(item)
        return o
###############################################################################
    def function(self):
        o=[]
        c=self.cn.execute('SELECT * FROM memberdef WHERE'+self.match("name")+' AND kind=?',[self.name,MemberType.Function])
        for r in c.fetchall():
            item={}
            item['name'] = r['name']
            item['definition'] = r['definition']
            item['argsstring'] = r['argsstring']
            item['file'] = self.fileName(r['file_id'])
            item['line'] = r['line']
            item['detaileddescription'] = r['detaileddescription']
            o.append(item)
        return o
###############################################################################
    def file(self):
        o=[]
        for r in self.cn.execute("SELECT rowid,name FROM local_file WHERE"+self.match("name"),[self.name]).fetchall():
            item={}
            item['name'] = r['name']
            item['id'] =   r['rowid']
            o.append(item)
        return o

###############################################################################
    def macro(self):
        o=[]
        c=self.cn.execute('SELECT * FROM memberdef WHERE'+self.match("name")+' AND kind=?',[self.name,MemberType.Define])
        for r in c.fetchall():
            item={}
            item['name'] = r['name']
            if r['argsstring']:
                item['argsstring'] = r['argsstring']
            item['definition'] = r['initializer']
            item['file'] = self.fileName(r['file_id'])
            item['line'] = r['line']
            o.append(item)
        return o
###############################################################################
    def typedef(self):
        o=[]
        c=self.cn.execute('SELECT * FROM memberdef WHERE'+self.match("name")+' AND kind=?',[self.name,MemberType.Typedef])
        for r in c.fetchall():
            item={}
            item['name'] = r['name']
            item['definition'] = r['definition']
            item['file'] = self.fileName(r['file_id'])
            item['line'] = r['line']
            o.append(item)
        return o
###############################################################################
    def variable(self):
        o=[]
        c=self.cn.execute('SELECT * FROM memberdef WHERE'+self.match("name")+' AND kind=?',[self.name,MemberType.Variable])
        for r in c.fetchall():
            item={}
            item['name'] = r['name']
            item['definition'] = r['definition']
            item['file'] = self.fileName(r['file_id'])
            item['line'] = r['line']
            o.append(item)
        return o
###############################################################################
    def params(self):
        o=[]
        c=self.cn.execute('SELECT rowid FROM memberdef WHERE'+self.match("name"),[self.name])
        for r in c.fetchall():
            #a=("SELECT * FROM param where id=(SELECT param_id FROM memberdef_param where memberdef_id=?",[memberdef_id])
            item={}
            item['id'] = r['id']
            o.append(item)
        return o
###############################################################################
    def struct(self):
        o=[]
        c=self.cn.execute('SELECT * FROM compounddef WHERE'+self.match("name"),[self.name])
        for r in c.fetchall():
            item={}
            item['name'] = r['name']
            o.append(item)
        return o
###############################################################################
    def includers(self):
        o=[]
        fid = self.fileId(self.name)
        c=self.cn.execute('SELECT * FROM includes WHERE dst_id=?',[fid])
        for r in c.fetchall():
            item={}
            item['name'] = self.fileName(r['src_id'])
            o.append(item)
        return o
###############################################################################
    def includees(self):
        o=[]
        fid = self.fileId(self.name)
        c=self.cn.execute('SELECT * FROM includes WHERE src_id=?',[fid])
        for r in c.fetchall():
            item={}
            item['name'] = self.fileName(r['dst_id'])
            o.append(item)
        return o
###############################################################################
    def members(self):
        o=[]
        c=self.cn.execute('SELECT * FROM memberdef WHERE'+self.match("scope"),[self.name])
        for r in c.fetchall():
            item={}
            item['name'] = r['name']
            item['definition'] = r['definition']
            item['argsstring'] = r['argsstring']
            item['file'] = self.fileName(r['file_id'])
            item['line'] = r['line']
            #item['documentation'] = r['documentation']
            o.append(item)
        return o
###############################################################################
    def baseClasses(self):
        o=[]
        c=self.cn.execute('SELECT compounddef.name FROM compounddef JOIN compoundref ON compounddef.rowid=compoundref.base_rowid WHERE compoundref.derived_rowid IN (SELECT rowid FROM compounddef WHERE'+self.match("name")+')',[self.name])
        for r in c.fetchall():
            item={}
            item['name'] = r['name']
            o.append(item)
        return o
###############################################################################
    def subClasses(self):
        o=[]
        c=self.cn.execute('SELECT compounddef.name FROM compounddef JOIN compoundref ON compounddef.rowid=compoundref.derived_rowid WHERE compoundref.base_rowid IN (SELECT rowid FROM compounddef WHERE'+self.match("name")+')',[self.name])
        for r in c.fetchall():
            item={}
            item['name'] = r['name']
            o.append(item)
        return o
###############################################################################
def process(f,kind):
    request_processors = {
        MemberType.Function: f.function,
        MemberType.File: f.file,
        MemberType.Define:   f.macro,
        MemberType.Variable: f.variable,
        MemberType.Typedef:  f.typedef,
        RequestType.References: f.references,
        RequestType.Struct: f.struct,
        RequestType.Includers: f.includers,
        RequestType.Includees: f.includees,
        RequestType.Members: f.members,
        RequestType.BaseClasses: f.baseClasses,
        RequestType.SubClasses: f.subClasses
    }
    return request_processors[kind]()
###############################################################################

# the -H option isn't documented. It's one of the more recent additions, but it's treating refids as if they would be a string. I'm just taking a stab at updating it for now, converting to use rowid, and making other edits necessary to get it to run.
def processHref(cn,ref):
    j={}

    # is it in memberdef ?
    table="memberdef"
    if ( cn.execute("SELECT count(*) from %s WHERE rowid=?"%table,[ref] ).fetchone()[0] > 0 ):
        for r in cn.execute("SELECT kind,rowid FROM %s WHERE rowid=?" % table,[ref]).fetchall():
            f=Finder(cn,r['rowid'],int)
            j=process(f,str(r['kind']))

    # is it in compounddef ?
    table="compounddef"
    if ( cn.execute("SELECT count(*) from %s WHERE rowid=?"%table,[ref]).fetchone()[0] > 0 ):
        for r in cn.execute("SELECT rowid FROM %s WHERE rowid=?"%table,[ref] ).fetchall():
            f=Finder(cn,r[0],int)
            j=process(f,RequestType.Struct)

    return j
###############################################################################
def serveCgi():
    import cgi

    print('Content-Type: application/json\n')

    fieldStorage = cgi.FieldStorage()
    form = dict((key, fieldStorage.getvalue(key)) for key in fieldStorage.keys())

    if 'href' in form:
        ref = form['href']
    else:
        print('{"result": null, "error": "no refid given"}')
        sys.exit(0)

    cn=openDb('doxygen_sqlite3.db')

    j = processHref(cn,ref)

    print(json.dumps({"result":j,"error":None}))
###############################################################################
def usage():
    sys.stderr.write("""Usage: search.py [Options]
Options:
    -h, --help
    -d <D>    Use database <D> for queries.
    -f <F>    Search for definition of function <F>.
    -m <M>    Search for definition of macro <M>.
    -r <F>    Search for references to function <F>.
    -t <T>    Search for definition of type <T>.
    -v <V>    Search for definition of variable <V>.
    -I <I>    What files are including <I>.
    -i <i>    What files are included by <i>.
    -B <C>    Get the base classes of class <C>.
    -M <C>    Get all members of class <C>.
    -S <C>    Get the sub classes of class <C>.
    -R        Consider the search <term> to be a regex.
""")
###############################################################################
def serveCli(argv):
    try:
        opts, args = getopt.getopt(argv, "hr:RI:i:d:f:m:t:v:H:M:B:S:F:",["help"])
    except getopt.GetoptError:
        usage()
        sys.exit(1)

    ref=None
    dbname=None
    j={}
    global g_use_regexp

    for a, o in opts:
        if a in ('-h', '--help'):
            usage()
            sys.exit(0)
        elif a in ('-d'):
            dbname=o
            continue
        elif a in ('-r'):
            kind=RequestType.References
        elif a in ('-R'):
            g_use_regexp=True
            continue
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
        elif a in ('-F'):
            # undocumented
            # seems to fit with the lower case "search" patterns?
            kind=MemberType.File
        elif a in ('-m'):
            kind=MemberType.Define
        elif a in ('-t'):
            kind=MemberType.Typedef
        elif a in ('-v'):
            kind=MemberType.Variable
        elif a in ('-H'):
            # undocumented
            ref = o

        cn=openDb(dbname)
        f=Finder(cn,o)
        if ref is not None:
            j=processHref(cn,ref)
        else:
            j=process(f,kind)
        print(json.dumps(j,indent=4))


def main(argv):
    if 'REQUEST_METHOD' in os.environ:
        serveCgi()
    else:
        serveCli(argv)

if __name__ == '__main__':
    main(sys.argv[1:])
