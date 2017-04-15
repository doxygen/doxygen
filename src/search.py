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
import re
import getopt

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

###############################################################################

def escapeLike(val):
    return 'LIKE "%' + val.replace("\\", "\\\\").replace("_", "\\_") \
        .replace("%", "\\%") + '%" ESCAPE "\\"'


def getLine(id_file,line,column):
    rv=""
    for r in g_conn.execute('SELECT * FROM files WHERE id=%d;' % id_file).fetchall():
            rv += "found in file %s:%d:%d" % (r['name'],line,column)
    return rv

###############################################################################

def processCallers(caller, path=None, fid=None):
    cur = g_conn.cursor()
    if fid is None:
        cur.execute("SELECT refid FROM memberdef WHERE name='%s'" % caller)
        refids = cur.fetchall()
        if len(refids) == 0:
            print 'No results found'
            return
        refid = refids[0]['refid']
        print "%s" % refid
        cur = g_conn.cursor()
        for info in cur.execute("SELECT * FROM xrefs WHERE dst LIKE '%"+refid+"%'"):
            cur = g_conn.cursor()
            for i2 in cur.execute("SELECT * FROM memberdef WHERE refid='%s'" % info['src']):
                print "caller:%s" % i2['name']
            for i2 in cur.execute("SELECT * FROM files WHERE id='%s'" % info['id_file']):
                print "file    : %s:%d" % (i2['name'],info['line'])


def processFunction(name):
    for r in g_conn.execute('SELECT * FROM memberdef WHERE name LIKE "%'
            +name+'%" AND kind='+MemberType.Function+';').fetchall():
        print '==> %s %s %s' % (r['type'],r['name'],r['argsstring'])
        print getLine(r['id_file'],r['line'],r['column'])


def processMacro(name):
    for r in g_conn.execute('SELECT * FROM memberdef WHERE name LIKE "%'
            +name+'%" AND kind='+MemberType.Define+';').fetchall():
        mname = r['name']
        if r['argsstring']:
            mname += r['argsstring']
        print '%s' % (mname)
        print getLine(r['id_file'],r['line'],r['column'])


def processType(name, path=None):
    for r in g_conn.execute('SELECT * FROM memberdef WHERE name LIKE "%'
            +name+'%" AND kind='+MemberType.Typedef+';').fetchall():
        print '%s' % (r['name'])
        print getLine(r['id_file'],r['line'],r['column'])


def processVariable(name):
    for r in g_conn.execute('SELECT * FROM memberdef WHERE name LIKE "%'+
            name + '%" AND kind='+MemberType.Variable+';').fetchall():
        print '> %s' % (r['name'])
        print getLine(r['id_file'],r['line'],r['column'])

###############################################################################

def usage():
  print """Usage: search.py [options]
Options:
    -h, --help
    -d <D>    Use database <D> for queries
    -c <F>    Search for callers of <F>
    -f <F>    Search for function <F>
    -m <M>    Search for macro <M>
    -t <T>    Search for type <T>
    -v <V>    Search for variable <V>
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


def main(argv):

    try:
        opts, args = getopt.getopt(argv, "hc:d:f:m:t:v:",["help"])
    except getopt.GetoptError:
        usage()
        sys.exit(1)

    dbname=None

    for a, o in opts:
        if a in ('-h', '--help'):
            usage()
            sys.exit(0)
        elif a in ('-d'):
            dbname=o
        elif a in ('-c'):
            openDb(dbname)
            processCallers(o)
        elif a in ('-f'):
            openDb(dbname)
            processFunction(o)
        elif a in ('-m'):
            openDb(dbname)
            processMacro(o)
        elif a in ('-t'):
            openDb(dbname)
            processType(o)
        elif a in ('-v'):
            openDb(dbname)
            processVariable(o)

if __name__ == '__main__':
    main(sys.argv[1:])
