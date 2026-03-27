#!/usr/bin/env python
from xml.etree import cElementTree as ET
import os
import sqlite3
import sys
import getopt

# map XML attributes/elements to SQL rows
# --POC: iterate through the children and attributes of the memberdef element
#        and search it in doxygen_sqlite3.db

g_conn=None
val=[]
def print_unprocessed_attributes(node):
    for key in node.attrib:
        print("WARNING: '%s' has unprocessed attr '%s'" % (node.tag,key))

def extract_attribute(node,attribute,pnl):
    if attribute not in node.attrib:
        return
    pnl.append("%s = ?" % attribute)
    val.append(node.attrib[attribute])
    node.attrib.pop(attribute)

def extract_element(node,chld,pnl):
    # deal with <tag />
    if chld.text is None:
        if len(chld.attrib)==0:
            node.remove(chld)
        return

    a=chld.text.strip()
    if not a == "":
        pnl.append("%s =?" % chld.tag)
        val.append(chld.text.strip())
    else:
        pnl.append("%s IS NULL OR %s = ''" % (chld.tag,chld.tag))
    node.remove(chld)

def process_memberdef(node):
    q=[]
    for chld in node.getchildren():
        if chld.tag == "referencedby":
            continue
        if chld.tag == "references":
            continue
        if chld.tag == "param":
            continue
        if chld.tag == "type":
            continue
        if chld.tag == "location":
            extract_attribute(chld,"line",q)
            extract_attribute(chld,"column",q)
            extract_attribute(chld,"bodystart",q)
            extract_attribute(chld,"bodyend",q)

            q.append("id_bodyfile=(select id from files where name=?)")
            val.append(chld.attrib["bodyfile"])
            chld.attrib.pop("bodyfile")

            q.append("id_file=(select id from files where name=?)")
            val.append(chld.attrib["file"])
            chld.attrib.pop("file")

            print_unprocessed_attributes(chld)
            if len(chld.attrib) == 0:
                node.remove(chld)
        else:
            extract_element(node,chld,q)

    for chld in node.getchildren():
        print("WARNING: '%s' has unprocessed child elem '%s'" % (node.tag,chld.tag))

    extract_attribute(node,"kind",q)
    extract_attribute(node,"prot",q)
    extract_attribute(node,"static",q)
    extract_attribute(node,"mutable",q)
    extract_attribute(node,"const",q)
    extract_attribute(node,"virt",q)
    extract_attribute(node,"explicit",q)
    extract_attribute(node,"inline",q)

    q.append("refid=?")
    val.append(node.attrib['id'])
    node.attrib.pop('id')

    print_unprocessed_attributes(node)

    query="SELECT * FROM memberdef WHERE %s" % " AND ".join(q)
    r=[]
    try:
        r = g_conn.execute(query,val).fetchall()
    except(sqlite3.OperationalError,e):
        print("SQL_ERROR:%s"%e)

    del val[:]
    if not len(r) > 0:
        print("TEST_ERROR: Member not found in SQL DB")


def load_xml(name):
    context = ET.iterparse(name, events=("start", "end"))
    event, root = context.next()
    for event, elem in context:
        if event == "end" and elem.tag == "memberdef":
            process_memberdef(elem)
    print("\n== Unprocessed XML ==")
#    ET.dump(root)


def open_db(dbname):
    global g_conn

    if dbname is None:
        dbname = "doxygen_sqlite3.db"

    if not os.path.isfile(dbname):
        raise BaseException("No such file %s" % dbname )

    g_conn = sqlite3.connect(dbname)
    g_conn.execute('PRAGMA temp_store = MEMORY;')
    g_conn.row_factory = sqlite3.Row

def main(argv):
    try:
        opts, args = getopt.getopt(argv, "hd:x:",["help"])
    except getopt.GetoptError:
        sys.exit(1)

    dbname=None
    xmlfile=None

    for a, o in opts:
        if a in ('-h', '--help'):
            sys.exit(0)
        elif a in ('-d'):
            dbname=o
            continue
        elif a in ('-x'):
            xmlfile=o
            continue
    open_db(dbname)
    load_xml(xmlfile)

if __name__ == '__main__':
    main(sys.argv[1:])
