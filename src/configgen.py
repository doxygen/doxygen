#!/usr/bin/env python
# python script to generate configoptions.cpp and config.doc from config.xml
#
# Copyright (C) 1997-2015 by Dimitri van Heesch.
#
# Permission to use, copy, modify, and distribute this software and its
# documentation under the terms of the GNU General Public License is hereby
# granted. No representations are made about the suitability of this software
# for any purpose. It is provided "as is" without express or implied warranty.
# See the GNU General Public License for more details.
#
# Documents produced by Doxygen are derivative works derived from the
# input used in their production; they are not affected by this license.
#
import xml.dom.minidom
import sys
import re
import textwrap
from xml.dom import Node
import io

messages = {}

# wrapper class to write to file/output in UTF-8 format
class OutputWriter:
    def __init__(self,writer) :
        self.writer = io.open(writer.fileno(), 'w', encoding='utf8')

    def write(self, text) :
        if sys.version_info.major == 2:
            self.writer.write(unicode(text))
        else:
            self.writer.write(text)

    def flush(self):
        self.writer.flush()


def transformDocs(doc):
    # join lines, unless it is an empty line
    # remove doxygen layout constructs
        # Note: also look at expert.cpp of doxywizard for doxywizard parts
    doc = doc.strip()
    doc = doc.replace("\n", " ")
    doc = doc.replace("\r", " ")
    doc = doc.replace("\t", " ")
    doc = doc.replace("\\&", "&")
    doc = doc.replace("(\\c ", "(")
    doc = doc.replace("\\c ", " ")
    doc = doc.replace("\\b ", " ")
    doc = doc.replace("\\e ", " ")
    doc = doc.replace("\\$", "$")
    doc = doc.replace("\\#include ", "#include ")
    doc = doc.replace("\\#undef ", "#undef ")
    doc = doc.replace("-# ", "\n - ")
    doc = doc.replace(" - ", "\n - ")
    doc = doc.replace("\\sa", "\nSee also: ")
    doc = doc.replace("\\par", "\n")
    doc = doc.replace("@note", "\nNote:")
    doc = doc.replace("\\note", "\nNote:")
    doc = doc.replace("\\verbatim", "\n")
    doc = doc.replace("\\endverbatim", "\n")
    doc = doc.replace("<code>", "")
    doc = doc.replace("</code>", "")
    doc = doc.replace("`", "")
    doc = doc.replace("\\<", "<")
    doc = doc.replace("\\>", ">")
    doc = doc.replace("\\@", "@")
    doc = doc.replace("\\\\", "\\")
    # \ref name "description" -> description
    doc = re.sub('\\\\ref +[^ ]* +"([^"]*)"', '\\1', doc)
    # \ref specials
    # \ref <key> -> description
    doc = re.sub('\\\\ref +doxygen_usage', '"Doxygen usage"', doc)
    doc = re.sub('\\\\ref +extsearch', '"External Indexing and Searching"',
                 doc)
    doc = re.sub('\\\\ref +layout', '"Changing the layout of pages"', doc)
    doc = re.sub('\\\\ref +external', '"Linking to external documentation"',
                 doc)
    doc = re.sub('\\\\ref +doxygen_finetune', '"Fine-tuning the output"',
                 doc)
    doc = re.sub('\\\\ref +formulas', '"Including formulas"', doc)
    # fallback for not handled
    doc = re.sub('\\\\ref', '', doc)
    #<a href="address">description</a> -> description (see: address)
    doc = re.sub('<a +href="([^"]*)" *>([^<]*)</a>', '\\2 (see: \n\\1)', doc)
    # LaTeX name as formula -> LaTeX
    doc = doc.replace("\\f$\\mbox{\\LaTeX}\\f$", "LaTeX")
    # Other formula's (now just 2) so explicitly mentioned.
    doc = doc.replace("\\f$2^{(16+\\mbox{LOOKUP\\_CACHE\\_SIZE})}\\f$",
                      "2^(16+LOOKUP_CACHE_SIZE)")
    doc = doc.replace("\\f$2^{16} = 65536\\f$", "2^16=65536")
    # remove consecutive spaces
    doc = re.sub(" +", " ", doc)
    # a dirty trick to get an extra empty line in Doxyfile documentation.
    # <br> will be removed later on again, we need it here otherwise splitlines
    # will filter the extra line.
    doc = doc.replace("<br>", "\n<br>\n")
    # a dirty trick to go to the next line in Doxyfile documentation.
    # <br/> will be removed later on again, we need it here otherwise splitlines
    # will filter the line break.
    doc = doc.replace("<br/>", "\n<br/>\n")
    #
    doc = doc.splitlines()
    split_doc = []
    for line in doc:
        split_doc += textwrap.wrap(line, 78)
    # replace \ by \\, replace " by \", and '  ' by a newline with end string
    # and start string at next line
    docC = []
    for line in split_doc:
        if (line.strip() != "<br/>"):
            docC.append(line.strip().replace('\\', '\\\\').
                    replace('"', '\\"').replace("<br>", ""))
    return docC


def collectValues(node):
    values = []
    for n in node.childNodes:
        if (n.nodeName == "value"):
            if n.nodeType == Node.ELEMENT_NODE:
                if n.getAttribute('name') != "":
                    if n.getAttribute('show_docu') != "NO":
                        name = "<code>" + n.getAttribute('name') + "</code>"
                        desc = n.getAttribute('desc')
                        if (desc != ""):
                            name += " " + desc
                        values.append(name)
    return values


def addValues(var, node):
    for n in node.childNodes:
        if (n.nodeName == "value"):
            if n.nodeType == Node.ELEMENT_NODE:
                name = n.getAttribute('name')
                print("  %s->addValue(\"%s\");" % (var, name))


def parseHeader(node,objName):
    doc = ""
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE:
            if (n.nodeName == "docs"):
                if (n.getAttribute('doxyfile') != "0"):
                    doc += parseDocs(n)
    docC = transformDocs(doc)
    print("  %s->setHeader(" % (objName))
    rng = len(docC)
    for i in range(rng):
        line = docC[i]
        if i != rng - 1:  # since we go from 0 to rng-1
            print("              \"%s\\n\"" % (line))
        else:
            print("              \"%s\"" % (line))
    print("             );")


def prepCDocs(node):
    type = node.getAttribute('type')
    format = node.getAttribute('format')
    defval = node.getAttribute('defval')
    #adefval = node.getAttribute('altdefval')
    doc = ""
    if (type != 'obsolete'):
        for n in node.childNodes:
            if (n.nodeName == "docs"):
                if (n.getAttribute('doxyfile') != "0"):
                    if n.nodeType == Node.ELEMENT_NODE:
                        doc += parseDocs(n)
        if (type == 'enum'):
            values = collectValues(node)
            doc += "<br/>" + messages['possible']
            rng = len(values)
            for i in range(rng):
                val = values[i]
                if i == rng - 2:
                    doc += "%s%s"%(val,messages['andtxt'])
                elif i == rng - 1:
                    doc += "%s." % (val)
                else:
                    doc += "%s, " % (val)
            if (defval != ""):
                doc += "<br/>" + messages['defvalcode'].format(defval)
        elif (type == 'int'):
            minval = node.getAttribute('minval')
            maxval = node.getAttribute('maxval')
            doc += messages['minmaxdef'].format(minval, maxval, defval)
        elif (type == 'bool'):
            if (node.hasAttribute('altdefval')):
                doc += "<br/>" + messages['defvaltxt'].format(messages['sysdep'])
            else:
                doc += "<br/>" + messages['defvaltxt'].format("YES" if (defval == "1") else "NO")
        elif (type == 'list'):
            if format == 'string':
                values = collectValues(node)
                rng = len(values)
                for i in range(rng):
                    val = values[i]
                    if i == rng - 2:
                        doc += "%s%s"%(val,messages['andtxt'])
                    elif i == rng - 1:
                        doc += "%s." % (val)
                    else:
                        doc += "%s, " % (val)
        elif (type == 'string'):
            if format == 'dir':
                if defval != '':
                    doc += "<br/>" + messages['defdir'].format(defval)
            elif format == 'file':
                abspath = node.getAttribute('abspath')
                if defval != '':
                    if abspath != '1':
                        doc += "<br/>" + messages['deffile'].format(defval)
                    else:
                        doc += "<br/>" + messages['deffileabs'].format(defval)
                else:
                    if abspath == '1':
                        doc += "<br/>" + messages['deffilefull']
            elif format =='image':
                abspath = node.getAttribute('abspath')
                if defval != '':
                    if abspath != '1':
                        doc += "<br/>" + messages['defimg'].format(defval)
                    else:
                        doc += "<br/>" + messages['defimgabs'].format(defval)
                else:
                    if abspath == '1':
                        doc += "<br/>" + messages['defimgfull']
            else: # format == 'string':
                if defval != '':
                    doc += "<br/>" + messages['defvalcode'].format(defval)
        # depends handling
        if (node.hasAttribute('depends')):
            depends = node.getAttribute('depends')
            doc += "<br/>" + messages['depstxt'].format(depends.lower(), depends.upper())

    docC = transformDocs(doc)
    return docC


def parseOption(node):
    # Handling part for Doxyfile
    name = node.getAttribute('id')
    if len(name)>23:
        raise Exception('Option name {0} too long ({1}, where max is 23 characters)'.format(name,len(name)))
    type = node.getAttribute('type')
    format = node.getAttribute('format')
    defval = node.getAttribute('defval')
    adefval = node.getAttribute('altdefval')
    depends = node.getAttribute('depends')
    setting = node.getAttribute('setting')
    orgtype = node.getAttribute('orgtype')
    docC = prepCDocs(node)
    if len(setting) > 0:
        print("#if %s" % (setting))
    print("  //----")
    if type == 'bool':
        if len(adefval) > 0:
            enabled = adefval
        elif defval == '1':
            enabled = "TRUE"
        else:
            enabled = "FALSE"
        print("  cb = cfg->addBool(")
        print("             \"%s\"," % (name))
        rng = len(docC)
        for i in range(rng):
            line = docC[i]
            if i != rng - 1:  # since we go from 0 to rng-1
                print("              \"%s\\n\"" % (line))
            else:
                print("              \"%s\"," % (line))
        print("              %s" % (enabled))
        print("             );")
        if depends != '':
            print("  cb->addDependency(\"%s\");" % (depends))
    elif type == 'string':
        print("  cs = cfg->addString(")
        print("              \"%s\"," % (name))
        rng = len(docC)
        for i in range(rng):
            line = docC[i]
            if i != rng - 1:  # since we go from 0 to rng-1
                print("              \"%s\\n\"" % (line))
            else:
                print("              \"%s\"" % (line))
        print("             );")
        if defval != '':
            print("  cs->setDefaultValue(\"%s\");" % (defval.replace('\\','\\\\')))
        if format == 'file':
            print("  cs->setWidgetType(ConfigString::File);")
        elif format == 'image':
            print("  cs->setWidgetType(ConfigString::Image);")
        elif format == 'dir':
            print("  cs->setWidgetType(ConfigString::Dir);")
        elif format == 'filedir':
            print("  cs->setWidgetType(ConfigString::FileAndDir);")
        if depends != '':
            print("  cs->addDependency(\"%s\");" % (depends))
    elif type == 'enum':
        print("  ce = cfg->addEnum(")
        print("              \"%s\"," % (name))
        rng = len(docC)
        for i in range(rng):
            line = docC[i]
            if i != rng - 1:  # since we go from 0 to rng-1
                print("              \"%s\\n\"" % (line))
            else:
                print("              \"%s\"," % (line))
        print("              \"%s\"" % (defval))
        print("             );")
        addValues("ce", node)
        if depends != '':
            print("  ce->addDependency(\"%s\");" % (depends))
    elif type == 'int':
        minval = node.getAttribute('minval')
        maxval = node.getAttribute('maxval')
        print("  ci = cfg->addInt(")
        print("              \"%s\"," % (name))
        rng = len(docC)
        for i in range(rng):
            line = docC[i]
            if i != rng - 1:  # since we go from 0 to rng-1
                print("              \"%s\\n\"" % (line))
            else:
                print("              \"%s\"," % (line))
        print("              %s,%s,%s" % (minval, maxval, defval))
        print("             );")
        if depends != '':
            print("  ci->addDependency(\"%s\");" % (depends))
    elif type == 'list':
        print("  cl = cfg->addList(")
        print("              \"%s\"," % (name))
        rng = len(docC)
        for i in range(rng):
            line = docC[i]
            try:
                if i != rng - 1:  # since we go from 0 to rng-1
                    print("              \"%s\\n\"" % (line))
                else:
                    print("              \"%s\"" % (line))
            except Exception as inst:
                sys.stdout = sys.stderr
                print("")
                print(inst)
                print("")
        print("             );")
        addValues("cl", node)
        if depends != '':
            print("  cl->addDependency(\"%s\");" % (depends))
        if format == 'file':
            print("  cl->setWidgetType(ConfigList::File);")
        elif format == 'dir':
            print("  cl->setWidgetType(ConfigList::Dir);")
        elif format == 'filedir':
            print("  cl->setWidgetType(ConfigList::FileAndDir);")
    elif type == 'obsolete':
        print("  cfg->addObsolete(\"%s\",ConfigOption::O_%s);" % (name,orgtype.capitalize()))
    if len(setting) > 0:
        print("#else")
        print("  cfg->addDisabled(\"%s\");" % (name))
        print("#endif")


def parseGroups(node):
    name = node.getAttribute('name')
    doc = node.getAttribute('docs')
    setting = node.getAttribute('setting')
    if len(setting) > 0:
        print("#if %s" % (setting))
    print("%s%s" % ("  //-----------------------------------------",
                    "----------------------------------"))
    print("  cfg->addInfo(\"%s\",\"%s\");" % (name, doc))
    print("%s%s" % ("  //-----------------------------------------",
                    "----------------------------------"))
    if len(setting) > 0:
        print("#endif")
    print("")
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE:
            parseOption(n)

def parseGroupMapEnums(node):
    def escape(value):
        return re.sub(r'[^\w]','_',value)
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE:
            type   = n.getAttribute('type')
            name   = n.getAttribute('id')
            defval = n.getAttribute('defval')
            if type=='enum':
                print("\nenum class %s_t" % (name))
                print("{")
                for nv in n.childNodes:
                    if nv.nodeName == "value":
                        value = nv.getAttribute('name')
                        if value:
                            print("  %s," % (escape(value)))
                print("};\n")
                print("inline {0}_t {1}_str2enum(const QCString &s)".format(name,name))
                print("{")
                print("  QCString lc = s.lower();")
                print("  static const std::unordered_map<std::string,{0}_t> map =".format(name))
                print("  {")
                for nv in n.childNodes:
                    if nv.nodeName == "value":
                        value = nv.getAttribute('name')
                        if value:
                            print("    {{ \"{0}\", {1}_t::{2} }},".format(value.lower(),name,escape(value)))
                print("  };")
                print("  auto it = map.find(lc.str());")
                print("  return it!=map.end() ? it->second : {0}_t::{1};".format(name,escape(defval)))
                print("}\n")
                print("inline QCString {0}_enum2str({1}_t v)".format(name,name))
                print("{")
                print("  switch(v)")
                print("  {")
                for nv in n.childNodes:
                    if nv.nodeName == "value":
                        value = nv.getAttribute('name')
                        if value:
                            print("    case {0}_t::{1}: return \"{2}\";".format(name,escape(value),value))
                print("  }")
                print("  return \"{0}\";".format(defval))
                print("}")

def parseGroupMapGetter(node):
    map = { 'bool':'bool', 'string':'const QCString &', 'int':'int', 'list':'const StringVector &' }
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE:
            setting = n.getAttribute('setting')
            if len(setting) > 0:
                print("#if %s" % (setting))
            type = n.getAttribute('type')
            name = n.getAttribute('id')
            if type=='enum':
                print("    %-22s %-30s const                  { return %s(m_%s); }" % (name+'_t',name+'()',name+'_str2enum',name))
                print("    %-22s %-30s const                  { return m_%s; }" % ('const QCString &',name+'_str()',name))
            elif type in map:
                print("    %-22s %-30s const                  { return m_%s; }" % (map[type],name+'()',name))
            if len(setting) > 0:
                print("#endif")

def parseGroupMapSetter(node):
    map = { 'bool':'bool', 'string':'const QCString &', 'int':'int', 'list':'const StringVector &' }
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE:
            setting = n.getAttribute('setting')
            if len(setting) > 0:
                print("#if %s" % (setting))
            type = n.getAttribute('type')
            name = n.getAttribute('id')
            if type=='enum':
                print("    [[maybe_unused]] %-22s update_%-46s { m_%s = %s(v); return v; }" % (name+'_t',name+'('+name+'_t '+' v)',name,name+'_enum2str'))
            elif type in map:
                print("    [[maybe_unused]] %-22s update_%-46s { m_%s = v; return m_%s; }" % (map[type],name+'('+map[type]+' v)',name,name))
            if len(setting) > 0:
                print("#endif")

def parseGroupMapAvailable(node):
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE:
            setting = n.getAttribute('setting')
            type = n.getAttribute('type')
            name = n.getAttribute('id')
            if type=='enum':
                if len(setting) > 0:
                    print("#if %s" % (setting))
                print("    %-22s isAvailable_%-41s { return v.lower() == %s_enum2str(%s_str2enum(v)).lower(); }" % ('bool',name+'(QCString v)',name,name))
                if len(setting) > 0:
                    print("#endif")

def parseGroupMapVar(node):
    map = { 'bool':'bool', 'string':'QCString', 'enum':'QCString', 'int':'int', 'list':'StringVector' }
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE:
            setting = n.getAttribute('setting')
            if len(setting) > 0:
                print("#if %s" % (setting))
            type = n.getAttribute('type')
            name = n.getAttribute('id')
            if type in map:
                print("    %-12s m_%s;" % (map[type],name))
            if len(setting) > 0:
                print("#endif")

def parseGroupInit(node):
    map = { 'bool':'Bool', 'string':'String', 'enum':'Enum', 'int':'Int', 'list':'List' }
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE:
            setting = n.getAttribute('setting')
            if len(setting) > 0:
                print("#if %s" % (setting))
            type = n.getAttribute('type')
            name = n.getAttribute('id')
            if type in map:
                print("  %-25s = ConfigImpl::instance()->get%s(__FILE__,__LINE__,\"%s\");" % ('m_'+name,map[type],name))
            if len(setting) > 0:
                print("#endif")

def getEnum2BoolMapping(node):
    def escape(value):
        return re.sub(r'[^\w]','_',value)
    mapping = []
    for nv in node.childNodes:
        if nv.nodeName == "value":
            name = nv.getAttribute("name")
            bool_rep = nv.getAttribute("bool_representation")
            if name and bool_rep:
                bool_value = "true" if bool_rep and bool_rep.upper() == 'YES' else "false"
                mapping.append( "{{ \"{0}\", {1} }}".format(escape(name),bool_value))
    return mapping

def parseGroupMapInit(node):
    map = { 'bool':'Bool', 'string':'String', 'enum':'String', 'int':'Int', 'list':'List' }
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE:
            setting = n.getAttribute('setting')
            if len(setting) > 0:
                print("#if %s" % (setting))
            type = n.getAttribute('type')
            name = n.getAttribute('id')
            if type in map:
                if type == "enum":
                    mappingStr = "{%s}" % (', '.join(getEnum2BoolMapping(n)))
                    print("    { %-26s Info{ %-13s &ConfigValues::m_%-23s %s}}," % ('\"'+name+'\",','Info::'+map[type]+',',name+",", mappingStr))
                else:
                    print("    { %-26s Info{ %-13s &ConfigValues::m_%-24s}}," % ('\"'+name+'\",','Info::'+map[type]+',',name))
            if len(setting) > 0:
                print("#endif")

def parseGroupCDocs(node):
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE:
            type = n.getAttribute('type')
            name = n.getAttribute('id')
            docC = prepCDocs(n)
            if type != 'obsolete':
                print("  doc->add(")
                print("              \"%s\"," % (name))
                rng = len(docC)
                for i in range(rng):
                    line = docC[i]
                    if i != rng - 1:  # since we go from 0 to rng-1
                        print("              \"%s\\n\"" % (line))
                    else:
                        print("              \"%s\"" % (line))
                print("          );")

def parseOptionDoc(node, first):
    # Handling part for documentation
    name = node.getAttribute('id')
    type = node.getAttribute('type')
    format = node.getAttribute('format')
    defval = node.getAttribute('defval')
    #adefval = node.getAttribute('altdefval')
    depends = node.getAttribute('depends')
    #setting = node.getAttribute('setting')
    doc = ""
    if (type != 'obsolete'):
        for n in node.childNodes:
            if (n.nodeName == "docs"):
                if (n.getAttribute('documentation') != "0"):
                    if n.nodeType == Node.ELEMENT_NODE:
                        doc += parseDocs(n)
        if (first):
            print(" \\anchor cfg_%s" % (name.lower()))
            print("<dl>")
            print("")
            print("<dt>\\c %s <dd>" % (name))
        else:
            print(" \\anchor cfg_%s" % (name.lower()))
            print("<dt>\\c %s <dd>" % (name))
        print(" \\addindex %s" % (name))
        print(doc)
        if (type == 'enum'):
            values = collectValues(node)
            print("")
            print(messages['possible'])
            rng = len(values)
            for i in range(rng):
                val = values[i]
                if i == rng - 2:
                    print("%s%s" % (val,messages['andtxt']))
                elif i == rng - 1:
                    print("%s." % (val))
                else:
                    print("%s, " % (val))
            if (defval != ""):
                print("")
                print("")
                print(messages['defvalcode'].format(defval))
            print("")
        elif (type == 'int'):
            minval = node.getAttribute('minval')
            maxval = node.getAttribute('maxval')
            print("")
            print("")
            print(messages['minmaxdefcode'].format(minval, maxval,defval))
            print("")
        elif (type == 'bool'):
            print("")
            print("")
            if (node.hasAttribute('altdefval')):
                print(messages['defvaltxt'].format(messages['sysdep']))
            else:
                print(messages['defvalcode'].format("YES" if (defval == "1") else "NO"))
            print("")
        elif (type == 'list'):
            if format == 'string':
                values = collectValues(node)
                rng = len(values)
                for i in range(rng):
                    val = values[i]
                    if i == rng - 2:
                        print("%s%s" % (val,messages['andtxt']))
                    elif i == rng - 1:
                        print("%s." % (val))
                    else:
                        print("%s, " % (val))
            print("")
        elif (type == 'string'):
            if format == 'dir':
                if defval != '':
                    print("")
                    print(messages['defdir'].format(defval))
            elif format == 'file':
                abspath = node.getAttribute('abspath')
                if defval != '':
                    print("")
                    if abspath != '1':
                        print(messages['deffile'].format(defval))
                    else:
                        print(messages['deffileabs'].format(defval))
                else:
                    if abspath == '1':
                        print("")
                        print(messages['deffilefull'])
            elif format =='image':
                abspath = node.getAttribute('abspath')
                if defval != '':
                    print("")
                    if abspath != '1':
                        print(messages['defimg'].format(defval))
                    else:
                        print(messages['defimgabs'].format(defval))
                else:
                    if abspath == '1':
                        print("")
                        print(messages['defimgfull'])
            else: # format == 'string':
                if defval != '':
                    print("")
                    print(messages['defvalcode'].format(defval.replace('\\','\\\\')))
            print("")
        # depends handling
        if (node.hasAttribute('depends')):
            depends = node.getAttribute('depends')
            print("")
            print(messages['depstxt'].format(depends.lower(), depends.upper()))
        return False


def parseGroupsDoc(node):
    name = node.getAttribute('name')
    doc = node.getAttribute('docs')
    print("\\section config_%s %s" % (name.lower(), doc))
    # Start of list has been moved to the first option for better
    # anchor placement
    #  print "<dl>"
    #  print ""
    first = True
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE:
            first = parseOptionDoc(n, first)
    if (not first):
        print("</dl>")


def parseGroupsList(node, commandsList):
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE:
            type = n.getAttribute('type')
            if type != 'obsolete':
                commandsList = commandsList + (n.getAttribute('id'),)
    return commandsList


def parseDocs(node):
    doc = ""
    for n in node.childNodes:
        if n.nodeType == Node.TEXT_NODE:
            doc += n.nodeValue.strip()
        elif n.nodeType == Node.CDATA_SECTION_NODE:
            doc += n.nodeValue.rstrip("\r\n ").lstrip("\r\n")
    #doc += "<br>"
    return doc


def parseHeaderDoc(node):
    doc = ""
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE:
            if (n.nodeName == "docs"):
                if (n.getAttribute('documentation') != "0"):
                    doc += parseDocs(n)
    print(doc)


def parseFooterDoc(node):
    doc = ""
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE:
            if (n.nodeName == "docs"):
                if (n.getAttribute('documentation') != "0"):
                    doc += parseDocs(n)
    print(doc)


def parseGenerator(node):
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE:
            if (n.nodeName == "message"):
                name = n.getAttribute('name')
                doc = ""
                for n1 in n.childNodes:
                    if n1.nodeType == Node.TEXT_NODE:
                        doc += n1.nodeValue.strip()
                    elif n1.nodeType == Node.CDATA_SECTION_NODE:
                        doc += n1.nodeValue.rstrip("\r\n").lstrip("\r\n")
                messages[name] = doc

def main():
    if len(sys.argv)<3 or (sys.argv[1] not in ['-doc','-cpp','-wiz','-maph','-maps']):
        sys.exit('Usage: %s -doc|-cpp|-wiz|-maph|-maps config.xml' % sys.argv[0])
    try:
        configFile = sys.argv[2]
        if sys.version_info.major == 2:
            fh = open(configFile,'r')
        else:
            fh = open(configFile,'r',encoding='utf8')
        sys.stdout = OutputWriter(sys.stdout)
        doc = xml.dom.minidom.parse(fh)
    except Exception as inst:
        sys.stdout = sys.stderr
        print("")
        print(inst)
        print("")
        sys.exit(1)
    elem = doc.documentElement

    for n in elem.childNodes:
        if n.nodeType == Node.ELEMENT_NODE:
            if (n.nodeName == "generator"):
                parseGenerator(n)
    if len(messages)==0:
        sys.exit('<generator> section missing in %s' % configFile)

    if (sys.argv[1] == "-doc"):
        print("/* WARNING: This file is generated!")
        print(" * Do not edit this file, but edit config.xml instead and run")
        print(" * python configgen.py -doc config.xml to regenerate this file!")
        print(" */")
        # process header
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE:
                if (n.nodeName == "header"):
                    parseHeaderDoc(n)
        # generate list with all commands
        commandsList = ()
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE:
                if (n.nodeName == "group"):
                    commandsList = parseGroupsList(n, commandsList)
        print("\\secreflist")
        for x in sorted(commandsList):
            print("\\refitem cfg_%s %s" % (x.lower(), x))
        print("\\endsecreflist")
        # process groups and options
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE:
                if (n.nodeName == "group"):
                    parseGroupsDoc(n)
        # process footers
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE:
                if (n.nodeName == "footer"):
                    parseFooterDoc(n)
    elif (sys.argv[1] == "-maph"):
        print("/* WARNING: This file is generated!")
        print(" * Do not edit this file, but edit config.xml instead and run")
        print(" * python configgen.py -map config.xml to regenerate this file!")
        print(" */")
        print("#ifndef CONFIGVALUES_H")
        print("#define CONFIGVALUES_H")
        print("")
        print("#include <string>")
        print("#include <unordered_map>")
        print("#include \"qcstring.h\"")
        print("#include \"containers.h\"")
        print("#include \"settings.h\"")
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE:
                if n.nodeName == "group":
                    parseGroupMapEnums(n)
        print("")
        print("class ConfigValues")
        print("{")
        print("  public:")
        print("    static ConfigValues &instance() { static ConfigValues theInstance; return theInstance; }")
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE:
                if n.nodeName == "group":
                    parseGroupMapGetter(n)
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE:
                if n.nodeName == "group":
                    parseGroupMapSetter(n)
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE:
                if n.nodeName == "group":
                    parseGroupMapAvailable(n)
        print("    void init();")
        print("    StringVector fields() const;")
        print("    struct Info")
        print("    {")
        print("      enum Type { Bool, Int, String, List, Unknown };")
        print("      using Enum2BoolMap = std::unordered_map<std::string,bool>;")
        print("      Info(Type t,bool         ConfigValues::*b) : type(t), value(b) {}")
        print("      Info(Type t,int          ConfigValues::*i) : type(t), value(i) {}")
        print("      Info(Type t,QCString     ConfigValues::*s, const Enum2BoolMap &boolMap = {}) : type(t), value(s), m_boolMap(boolMap) {}")
        print("      Info(Type t,StringVector ConfigValues::*l) : type(t), value(l) {}")
        print("      Type type;")
        print("      union Item")
        print("      {")
        print("        Item(bool         ConfigValues::*v) : b(v) {}")
        print("        Item(int          ConfigValues::*v) : i(v) {}")
        print("        Item(QCString     ConfigValues::*v) : s(v) {}")
        print("        Item(StringVector ConfigValues::*v) : l(v) {}")
        print("        bool         ConfigValues::*b;")
        print("        int          ConfigValues::*i;")
        print("        QCString     ConfigValues::*s;")
        print("        StringVector ConfigValues::*l;")
        print("      } value;")
        print("      bool getBooleanRepresentation() const;")
        print("    private:")
        print("      Enum2BoolMap m_boolMap;")
        print("    };")
        print("    const Info *get(const QCString &tag) const;")
        print("")
        print("  private:")
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE:
                if (n.nodeName == "group"):
                    parseGroupMapVar(n)
        print("};")
        print("")
        print("#endif")
    elif (sys.argv[1] == "-maps"):
        print("/* WARNING: This file is generated!")
        print(" * Do not edit this file, but edit config.xml instead and run")
        print(" * python configgen.py -maps config.xml to regenerate this file!")
        print(" */")
        print("#include \"configvalues.h\"")
        print("#include \"configimpl.h\"")
        print("#include <unordered_map>")
        print("")
        print("const ConfigValues::Info *ConfigValues::get(const QCString &tag) const")
        print("{")
        print("  static const std::unordered_map< std::string, Info > configMap =")
        print("  {")
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE:
                if (n.nodeName == "group"):
                    parseGroupMapInit(n)
        print("  };")
        print("  auto it = configMap.find(tag.str());")
        print("  return it!=configMap.end() ? &it->second : nullptr;")
        print("}")
        print("")
        print("void ConfigValues::init()")
        print("{")
        print("  static bool first = TRUE;")
        print("  if (!first) return;")
        print("  first = FALSE;")
        print("")
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE:
                if (n.nodeName == "group"):
                    parseGroupInit(n)
        print("}")
        print("")
        print("StringVector ConfigValues::fields() const")
        print("{")
        print("  return {")
        first=True
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE:
                if (n.nodeName == "group"):
                    for c in n.childNodes:
                        if c.nodeType == Node.ELEMENT_NODE:
                            name = c.getAttribute('id')
                            type = c.getAttribute('type')
                            if type!='obsolete':
                                if not first:
                                    print(",")
                                first=False
                                sys.stdout.write('    "'+name+'"')
        print("")
        print("  };")
        print("}")
        print("")
        print("bool ConfigValues::Info::getBooleanRepresentation() const")
        print("{")
        print("  if (!m_boolMap.empty())")
        print("  {")
        print("    auto it = m_boolMap.find((ConfigValues::instance().*(value.s)).str());")
        print("    if (it!=m_boolMap.end())")
        print("    {")
        print("      return it->second;")
        print("    }")
        print("  }")
        print("  return false;")
        print("}")
        print("")
    elif (sys.argv[1] == "-cpp"):
        print("/* WARNING: This file is generated!")
        print(" * Do not edit this file, but edit config.xml instead and run")
        print(" * python configgen.py -cpp config.xml to regenerate this file!")
        print(" */")
        print("")
        print("#include \"configoptions.h\"")
        print("#include \"configimpl.h\"")
        print("#include \"portable.h\"")
        print("#include \"settings.h\"")
        print("")
        print("void addConfigOptions(ConfigImpl *cfg)")
        print("{")
        print("  ConfigString *cs = nullptr;")
        print("  ConfigEnum   *ce = nullptr;")
        print("  ConfigList   *cl = nullptr;")
        print("  ConfigInt    *ci = nullptr;")
        print("  ConfigBool   *cb = nullptr;")
        print("")
        # process header
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE:
                if (n.nodeName == "header"):
                    parseHeader(n,'cfg')
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE:
                if (n.nodeName == "group"):
                    parseGroups(n)
        print("}")
    elif (sys.argv[1] == "-wiz"):
        print("/* WARNING: This file is generated!")
        print(" * Do not edit this file, but edit config.xml instead and run")
        print(" * python configgen.py -wiz config.xml to regenerate this file!")
        print(" */")
        print("#include \"configdoc.h\"")
        print("#include \"docintf.h\"")
        print("")
        print("void addConfigDocs(DocIntf *doc)")
        print("{")
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE:
                if (n.nodeName == "header"):
                    parseHeader(n,'doc')
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE:
                if (n.nodeName == "group"):
                    parseGroupCDocs(n)
        print("}")

if __name__ == '__main__':
    main()
