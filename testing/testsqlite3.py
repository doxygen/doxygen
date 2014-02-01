#! /usr/bin/python
from xml.etree import cElementTree as ET
import os
import sqlite3
import sys
import getopt

# map XML attributes/elements to SQL rows
# --POC: iterate through the children and attributes of the memberdef elelement
#        and search it in doxygen_sqlite3.db

g_conn=None
val=[]
def print_unprocessed_attributes(node):
    for key in node.attrib:
        print "WARNING: '%s' has unprocessed attr '%s'" % (node.tag,key)

def extract_attribute(node,attribute,pnl):
    pnl.append("%s = ?" % attribute)
    val.append(node.attrib[attribute])
    node.attrib.pop(attribute)

def extract_element(node,chld,pnl):
    pnl.append("%s =?" % chld.tag)
    val.append(chld.text)
    node.remove(chld)

def process_memberdef(node):
    q=[]
    for chld in node.getchildren():
            if chld.tag == "location":
                extract_attribute(chld,"line",q)
                extract_attribute(chld,"column",q)
                extract_attribute(chld,"bodystart",q)

                q.append("id_bodyfile=(select id from files where name=?)")
                val.append(chld.attrib["bodyfile"])
                chld.attrib.pop("bodyfile")

                q.append("id_file=(select id from files where name=?)")
                val.append(chld.attrib["file"])
                chld.attrib.pop("file")

                print_unprocessed_attributes(chld)
            elif chld.tag == "briefdescription":
                continue
            elif chld.tag == "detaileddescription":
                continue
            elif chld.tag == "inbodydescription":
                continue
            else:
                extract_element(node,chld,q)

    for chld in node.getchildren():
        print "WARNING: '%s' has unprocessed child elem '%s'" % (node.tag,chld.tag)
    print_unprocessed_attributes(node)

    query="SELECT * FROM memberdef WHERE %s" % " AND ".join(q)
    r = g_conn.execute(query,val).fetchall()
    if not len(r) > 0:
        print "ERROR: Member not found in SQL DB"

def load_xml(name):
    context = ET.iterparse(name, events=("start", "end"))
    event, root = context.next()
    for event, elem in context:
        if event == "end" and elem.tag == "memberdef":
            process_memberdef(elem)
    print "\n== Unprocessed XML =="
    ET.dump(root)


def open_db(dbname):
    global g_conn

    if dbname == None:
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
