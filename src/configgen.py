#!/usr/bin/env python
# python script to generate variour configuration related files from config.xml
#
# Copyright (C) 1997-2026 by Dimitri van Heesch.
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
import argparse
import xml.dom.minidom
import sys
import re
import textwrap
from xml.dom import Node
import io
import glob
import os

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
    doc = doc.replace("\\# ", "# ")
    doc = doc.replace("-# ", "\n - ")
    doc = doc.replace(" - ", "\n - ")
    doc = doc.replace("\\sa ", "\n"+messages['seealsotxt']+" ")
    doc = doc.replace("\\par ", "\n")
    doc = doc.replace("@note ", "\n"+messages['notetxt']+" ")
    doc = doc.replace("\\note ", "\n"+messages['notetxt']+" ")
    doc = doc.replace("\\verbatim", "\n")
    doc = doc.replace("\\endverbatim", "\n")
    doc = doc.replace("<b>", "")
    doc = doc.replace("</b>", "")
    doc = doc.replace("<code>", "")
    doc = doc.replace("</code>", "")
    doc = doc.replace("`", "")
    doc = doc.replace("\\<", "<")
    doc = doc.replace("\\>", ">")
    doc = doc.replace("\\@", "@")
    doc = doc.replace("\\\\", "\\")
    doc = doc.replace("@@", "@")
    # \ref name "description" -> description
    doc = re.sub('\\\\ref +[^ ]* +"\\\\ref"', '\\\\REF', doc)
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
    doc = re.sub('\\\\ref ', ' ', doc)
    doc = re.sub('\\\\REF', '\\\\ref', doc)
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
        if line.strip() != "<br/>":
            docC.append(line.strip().replace('\\', '\\\\').
                    replace('"', '\\"').replace("<br>", ""))
    return docC


def collectValues(node):
    values = []
    for n in node.childNodes:
        if n.nodeName == "value" and n.nodeType == Node.ELEMENT_NODE and n.getAttribute('name') != "" and n.getAttribute('show_docu') != "NO":
            name = "<code>" + n.getAttribute('name') + "</code>"
            desc = n.getAttribute('desc')
            if desc != "":
                name += " " + desc
            values.append(name)
    return values


def addValues(var, node):
    for n in node.childNodes:
        if n.nodeName == "value" and n.nodeType == Node.ELEMENT_NODE:
            name = n.getAttribute('name')
            print(f"  {var}->addValue(\"{name}\");")


def getFilter(node, mode):
    attr = node.getAttribute('filter')
    return not attr or mode in attr

def parseHeader(node, objName, mode):
    doc = ""
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE and n.nodeName == "docs" and getFilter(n, mode):
            doc += parseDocs(n)
    docC = transformDocs(doc)
    print(f"  {objName}->setHeader(")
    rng = len(docC)
    for i in range(rng):
        line = docC[i]
        if i != rng - 1:  # since we go from 0 to rng-1
            print(f"              \"{line}\\n\"")
        else:
            print(f"              \"{line}\"")
    print("             );")


def prepCDocs(node, mode):
    type = node.getAttribute('type')
    format = node.getAttribute('format')
    defval = node.getAttribute('defval')
    doc = ""
    if type != 'obsolete':
        for n in node.childNodes:
            if n.nodeType == Node.ELEMENT_NODE and n.nodeName == "docs" and getFilter(n, mode):
                doc += parseDocs(n)
        if type == 'enum':
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
            if defval != "":
                doc += "<br/>" + messages['defvalcode'].format(defval)
        elif type == 'int':
            minval = node.getAttribute('minval')
            maxval = node.getAttribute('maxval')
            doc += "<br/>" + messages['minmaxdefcode'].format(minval, maxval, defval)
        elif type == 'bool':
            doc += "<br/>" + messages['defvalcode'].format("YES" if (defval == "1") else "NO")
        elif type == 'list':
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
        elif type == 'string':
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
        if node.hasAttribute('depends'):
            depends = node.getAttribute('depends')
            doc += "<br/>" + messages['depstxtref'].format(depends.lower(), depends.upper())

    docC = transformDocs(doc)
    return docC


def parseOption(node, mode):
    # Handling part for Doxyfile
    name = node.getAttribute('id')
    if len(name)>23:
        raise Exception('Option name {0} too long ({1}, where max is 23 characters)'.format(name,len(name)))
    type = node.getAttribute('type')
    format = node.getAttribute('format')
    defval = node.getAttribute('defval')
    depends = node.getAttribute('depends')
    setting = node.getAttribute('setting')
    orgtype = node.getAttribute('orgtype')
    docC = prepCDocs(node, mode)
    if len(setting) > 0:
        print(f"#if {setting}")
    print("  //----")
    if type == 'bool':
        if defval == '1':
            enabled = "true"
        else:
            enabled = "false"
        print("  cb = cfg->addBool(")
        print(f"             \"{name}\",")
        rng = len(docC)
        for i in range(rng):
            line = docC[i]
            if i != rng - 1:  # since we go from 0 to rng-1
                print(f"              \"{line}\\n\"")
            else:
                print(f"              \"{line}\",")
        print(f"              {enabled}")
        print("             );")
        if depends != '':
            print(f"  cb->addDependency(\"{depends}\");")
    elif type == 'string':
        print("  cs = cfg->addString(")
        print(f"              \"{name}\",")
        rng = len(docC)
        for i in range(rng):
            line = docC[i]
            if i != rng - 1:  # since we go from 0 to rng-1
                print(f"              \"{line}\\n\"")
            else:
                print(f"              \"{line}\"")
        print("             );")
        if defval != '':
            escapedDefval = defval.replace('\\','\\\\')
            print(f"  cs->setDefaultValue(\"{escapedDefval}\");")
        if format == 'file':
            print("  cs->setWidgetType(ConfigString::File);")
        elif format == 'image':
            print("  cs->setWidgetType(ConfigString::Image);")
        elif format == 'dir':
            print("  cs->setWidgetType(ConfigString::Dir);")
        elif format == 'filedir':
            print("  cs->setWidgetType(ConfigString::FileAndDir);")
        if depends != '':
            print(f"  cs->addDependency(\"{depends}\");")
    elif type == 'enum':
        print("  ce = cfg->addEnum(")
        print(f"              \"{name}\",")
        rng = len(docC)
        for i in range(rng):
            line = docC[i]
            if i != rng - 1:  # since we go from 0 to rng-1
                print(f"              \"{line}\\n\"")
            else:
                print(f"              \"{line}\",")
        print(f"              \"{defval}\"")
        print("             );")
        addValues("ce", node)
        if depends != '':
            print(f"  ce->addDependency(\"{depends}\");")
    elif type == 'int':
        minval = node.getAttribute('minval')
        maxval = node.getAttribute('maxval')
        print("  ci = cfg->addInt(")
        print(f"              \"{name}\",")
        rng = len(docC)
        for i in range(rng):
            line = docC[i]
            if i != rng - 1:  # since we go from 0 to rng-1
                print(f"              \"{line}\\n\"")
            else:
                print(f"              \"{line}\",")
        print(f"              {minval},{maxval},{defval}")
        print("             );")
        if depends != '':
            print(f"  ci->addDependency(\"{depends}\");")
    elif type == 'list':
        print("  cl = cfg->addList(")
        print(f"              \"{name}\",")
        rng = len(docC)
        for i in range(rng):
            line = docC[i]
            try:
                if i != rng - 1:  # since we go from 0 to rng-1
                    print(f"              \"{line}\\n\"")
                else:
                    print(f"              \"{line}\"")
            except Exception as inst:
                sys.stdout = sys.stderr
                print("")
                print(inst)
                print("")
        print("             );")
        addValues("cl", node)
        if depends != '':
            print(f"  cl->addDependency(\"{depends}\");")
        if format == 'file':
            print("  cl->setWidgetType(ConfigList::File);")
        elif format == 'dir':
            print("  cl->setWidgetType(ConfigList::Dir);")
        elif format == 'filedir':
            print("  cl->setWidgetType(ConfigList::FileAndDir);")
    elif type == 'obsolete':
        print(f"  cfg->addObsolete(\"{name}\",ConfigOption::O_{orgtype.capitalize()});")
    if len(setting) > 0:
        print("#else")
        print(f"  cfg->addDisabled(\"{name}\");")
        print("#endif")


def parseGroups(node, mode):
    name = node.getAttribute('name')
    doc = node.getAttribute('docs')
    setting = node.getAttribute('setting')
    if len(setting) > 0:
        print(f"#if {setting}")
    print("  //---------------------------------------------------------------------------")
    print(f"  cfg->addInfo(\"{name}\",\"{doc}\");")
    print("  //---------------------------------------------------------------------------")
    if len(setting) > 0:
        print("#endif")
    print("")
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE:
            parseOption(n, mode)

def parseGroupMapEnums(node):
    def escape(value):
        return re.sub(r'[^\w]','_',value)
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE:
            type   = n.getAttribute('type')
            name   = n.getAttribute('id')
            defval = n.getAttribute('defval')
            if type=='enum':
                print(f"\nenum class {name}_t")
                print("{")
                for nv in n.childNodes:
                    if nv.nodeName == "value":
                        value = nv.getAttribute('name')
                        if value:
                            print(f"  {escape(value)},")
                print("};\n")
                print(f"inline {name}_t {name}_str2enum(const DString &s)")
                print("{")
                print("  DString lc = s.lower();")
                print(f"  static const std::unordered_map<std::string,{name}_t> map =")
                print("  {")
                for nv in n.childNodes:
                    if nv.nodeName == "value":
                        value = nv.getAttribute('name')
                        if value:
                            print(f"    {{ \"{value.lower()}\", {name}_t::{escape(value)} }},")
                print("  };")
                print("  auto it = map.find(lc.str());")
                print(f"  return it!=map.end() ? it->second : {name}_t::{escape(defval)};")
                print("}\n")
                print(f"inline DString {name}_enum2str({name}_t v)")
                print("{")
                print("  switch(v)")
                print("  {")
                for nv in n.childNodes:
                    if nv.nodeName == "value":
                        value = nv.getAttribute('name')
                        if value:
                            print(f"    case {name}_t::{escape(value)}: return \"{value}\";")
                print("  }")
                print(f"  return \"{defval}\";")
                print("}")

def parseGroupMapGetter(node):
    map = { 'bool':'bool', 'string':'DString', 'int':'int', 'list':'StringVector' }
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE:
            setting = n.getAttribute('setting')
            if len(setting) > 0:
                print(f"#if {setting}")
            type = n.getAttribute('type')
            name = n.getAttribute('id')
            if type=='enum':
                print(f"    {name+'_t':<22} {name+'()':<30} const                  {{ return {name}_str2enum(m_{name}); }}")
                print(f"    {'DString ':<22} {name+'_str()':<30} const                  {{ return m_{name}; }}")
            elif type in map:
                print(f"    {map[type]:<22} {name+'()':<30} const                  {{ return m_{name}; }}")
            if len(setting) > 0:
                print("#endif")

def parseGroupMapSetter(node):
    update_map = { 'bool':'bool', 'string':'const DString &', 'int':'int', 'list':'const StringVector &' }
    return_map = { 'bool':'bool', 'string':'DString', 'int':'int', 'list':'StringVector' }
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE:
            setting = n.getAttribute('setting')
            if len(setting) > 0:
                print(f"#if {setting}")
            type = n.getAttribute('type')
            name = n.getAttribute('id')
            if type=='enum':
                print(f"    [[maybe_unused]] {name+'_t':<22} update_{name+'('+name+'_t '+' v)':<46} {{ m_{name} = {name}_enum2str(v); return v; }}")
            elif type in update_map:
                print(f"    [[maybe_unused]] {return_map[type]:<22} update_{name+'('+update_map[type]+' v)':<46} {{ m_{name} = v; return m_{name}; }}")
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
                    print(f"#if {setting}")
                print(f"    {'bool':<22} isAvailable_{name+'(DString v)':<41} {{ return v.lower() == {name}_enum2str({name}_str2enum(v)).lower(); }}")
                if len(setting) > 0:
                    print("#endif")

def parseGroupMapVar(node):
    map = { 'bool':'bool', 'string':'DString', 'enum':'DString', 'int':'int', 'list':'StringVector' }
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE:
            setting = n.getAttribute('setting')
            if len(setting) > 0:
                print(f"#if {setting}")
            type = n.getAttribute('type')
            name = n.getAttribute('id')
            if type in map:
                print(f"    {map[type]:<12} m_{name};")
            if len(setting) > 0:
                print("#endif")

def parseGroupInit(node):
    map = { 'bool':'Bool', 'string':'String', 'enum':'Enum', 'int':'Int', 'list':'List' }
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE:
            setting = n.getAttribute('setting')
            if len(setting) > 0:
                print(f"#if {setting}")
            type = n.getAttribute('type')
            name = n.getAttribute('id')
            if type in map:
                print(f"  {'m_'+name:<25} = ConfigImpl::instance()->get{map[type]}(__FILE__,__LINE__,\"{name}\");")
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
                print(f"#if {setting}")
            type = n.getAttribute('type')
            name = n.getAttribute('id')
            if type in map:
                quotedName = '"'+name+'",'
                if type == "enum":
                    mappingStr = "{%s}" % (', '.join(getEnum2BoolMapping(n)))
                    print(f"    {{ {quotedName:<26} Info{{ {'Info::'+map[type]+',':<13} &ConfigValues::m_{name+',':<23} {mappingStr}}}}},")
                else:
                    print(f"    {{ {quotedName:<26} Info{{ {'Info::'+map[type]+',':<13} &ConfigValues::m_{name:<24}}}}},")
            if len(setting) > 0:
                print("#endif")

def parseGroupCDocs(node, mode):
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE:
            type = n.getAttribute('type')
            name = n.getAttribute('id')
            docC = prepCDocs(n, mode)
            if type != 'obsolete':
                print("  doc->add(")
                print(f"              \"{name}\",")
                rng = len(docC)
                for i in range(rng):
                    line = docC[i]
                    if i != rng - 1:  # since we go from 0 to rng-1
                        print(f"              \"{line}\\n\"")
                    else:
                        print(f"              \"{line}\"")
                print("          );")

def parseOptionDoc(node, first, mode):
    # Handling part for documentation
    name = node.getAttribute('id')
    type = node.getAttribute('type')
    format = node.getAttribute('format')
    defval = node.getAttribute('defval')
    depends = node.getAttribute('depends')
    #setting = node.getAttribute('setting')
    doc = ""
    if type != 'obsolete':
        for n in node.childNodes:
            if n.nodeType == Node.ELEMENT_NODE and n.nodeName == "docs" and getFilter(n, mode):
                doc += parseDocs(n)
        if first:
            print(f" \\anchor cfg_{name.lower()}")
            print("<dl>")
            print("")
            print(f"<dt>\\c {name} <dd>")
        else:
            print(f" \\anchor cfg_{name.lower()}")
            print(f"<dt>\\c {name} <dd>")
        print(f" \\addindex {name}")
        print(doc)
        if type == 'enum':
            values = collectValues(node)
            print("")
            print(messages['possible'])
            rng = len(values)
            for i in range(rng):
                val = values[i]
                if i == rng - 2:
                    print(f"{val}{messages['andtxt']}")
                elif i == rng - 1:
                    print(f"{val}.")
                else:
                    print(f"{val}, ")
            if defval != "":
                print("")
                print("")
                print(messages['defvalcode'].format(defval))
            print("")
        elif type == 'int':
            minval = node.getAttribute('minval')
            maxval = node.getAttribute('maxval')
            print("")
            print("")
            print(messages['minmaxdefcode'].format(minval, maxval,defval))
            print("")
        elif type == 'bool':
            print("")
            print("")
            print(messages['defvalcode'].format("YES" if (defval == "1") else "NO"))
            print("")
        elif type == 'list':
            if format == 'string':
                values = collectValues(node)
                rng = len(values)
                for i in range(rng):
                    val = values[i]
                    if i == rng - 2:
                        print(f"{val}{messages['andtxt']}")
                    elif i == rng - 1:
                        print(f"{val}.")
                    else:
                        print(f"{val}, ")
            print("")
        elif type == 'string':
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
        if node.hasAttribute('depends'):
            depends = node.getAttribute('depends')
            print("")
            print(messages['depstxtref'].format(depends.lower(), depends.upper()))
        return False


def parseGroupsDoc(node, mode):
    name = node.getAttribute('name')
    doc = node.getAttribute('docs')
    print(f"\\section config_{name.lower()} {doc}")
    # Start of list has been moved to the first option for better
    # anchor placement
    #  print "<dl>"
    #  print ""
    first = True
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE:
            first = parseOptionDoc(n, first, mode)
    if not first:
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

def parseHeaderDoc(node, mode):
    doc = ""
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE and n.nodeName == "docs" and getFilter(n, mode):
            doc += parseDocs(n)
    print(doc)


def parseFooterDoc(node, mode):
    doc = ""
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE and n.nodeName == "docs" and getFilter(n, mode):
            doc += parseDocs(n)
    print(doc)


def parseGenerator(node):
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE and n.nodeName == "message":
            name = n.getAttribute('name')
            doc = ""
            for n1 in n.childNodes:
                if n1.nodeType == Node.TEXT_NODE:
                    doc += n1.nodeValue.strip()
                elif n1.nodeType == Node.CDATA_SECTION_NODE:
                    doc += n1.nodeValue.rstrip("\r\n").lstrip("\r\n")
            messages[name] = doc

def collectOptions(elem, mode):
    """Collect all information from config....xml."""
    options = set()
    messages = set()
    optionsWithElems = {}
    attrib = {}
    values = set()
    head_docs = []

    for header in elem.getElementsByTagName('header'):
        for doc in header.getElementsByTagName('docs'):
            if getFilter(doc, mode):
                head_docs.append(doc)

    for group in elem.getElementsByTagName('group'):
        for option in group.getElementsByTagName('option'):
            optionId = option.getAttribute('id')
            optionType = option.getAttribute('type')
            if optionId and optionType!='obsolete':
                options.add(optionId)
                optionsWithElems[optionId] = option
                attrib[optionId] = sorted(option.attributes.items())
                if option.getElementsByTagName('value'):
                    values.add(optionId)

    for generator in elem.getElementsByTagName('generator'):
        for message in generator.getElementsByTagName('message'):
            messageId = message.getAttribute('name')
            messages.add(messageId)

    return (options,optionsWithElems,messages,attrib,values,head_docs)

def syncWarnings(typ, existing, language):
    missing = existing - language
    extra = language - existing

    if missing:
        print(f"  Missing {len(missing)} {typ}: {', '.join(sorted(list(missing))[:5])}")
        if len(missing) > 5:
            print(f"  ... and {len(missing) - 5} more")

    if extra:
        print(f"  Extra {len(extra)} {typ} (not in original): {', '.join(sorted(list(extra))[:5])}")
        if len(extra) > 5:
             print(f"  ... and {len(extra) - 5} more")

    if not missing and not extra:
        print(f"  OK - all {typ} are synchronized")
        return(False)

    return(True)

def syncLocalizedConfig(elem, configFile, translationsDir, autoSync=False, report=False):
    """Sync localized config_xx.xml files with original config.xml.

    Args:
        elem: The root element of config.xml
        translationsDir: Path to translations directory
        autoSync: If True, automatically sync; if False, only report differences
        report: If True, if False, only report differences but for doxygen markdown file
    """
    import os
    import shutil

    mode = "doxyfile"

    if report:
        print("@page pg_trans_confi Translator configuration report")
        print("```")

    if report:
        prt_configFile = os.path.basename(configFile)
    else:
        prt_configFile = configFile

    existingOptions, existingOptionsWithElements, existingMessages, existingAttrib, existingValues, existingHeader = collectOptions(elem, mode)
    print(f"Found {len(existingOptions)} active options in {prt_configFile}")
    print(f"Found {len(existingMessages)} active messages in {prt_configFile}")

    translationFiles = sorted(glob.glob("%s/config_*.xml" % translationsDir))
    if not translationFiles:
        print(f"No translation config file in {translationsDir}")

    for configFile in translationFiles:

        if report:
            prt_configFile = os.path.basename(configFile)
        else:
            prt_configFile = configFile

        if not os.path.exists(configFile):
            print(f"Skipping {prt_configFile}: translation config file not found")
            continue

        print(f"Processing {prt_configFile}...")

        try:
            with io.open(configFile, 'r', encoding='utf8') as f:
                content = f.read()
            langDoc = xml.dom.minidom.parseString(content)
        except Exception as e:
            print(f"  Error parsing {prt_configFile}: {e}")
            continue

        langOptions, langOptionsWithElements, langMessages, langAttrib, langValues, headerDocs = collectOptions(langDoc, mode)

        headerError = len(existingHeader) - len(headerDocs)

        if not headerError:
            print("  OK - header documentation is synchronized")
        elif headerError < 0:
            print("  Extra header documentation in translation")
        else:
            print("  Header documentation not (all) has been translated")

        optionsError = syncWarnings('Options', existingOptions, langOptions)
        messagesError = syncWarnings('Messages', existingMessages, langMessages)

        # attributes handling
        extraOptions = langOptions - existingOptions
        langMatch = langOptions - extraOptions
        attribError = set()
        for optionId in langMatch:
            missingAttrib = set(existingAttrib[optionId]) - set(langAttrib[optionId])
            extraAttrib = set(langAttrib[optionId]) - set(existingAttrib[optionId])
            if missingAttrib:
                attribError.add(optionId)
                print(f"  Missing {len(missingAttrib)} attributes for {optionId}")

            if extraAttrib:
                attribError.add(optionId)
                print(f"  Extra {len(extraAttrib)} attributes for {optionId}")
        if not attribError:
            print("  OK - all attributes are synchronized")

        # values handling
        valuesError = False
        # we only need options that are also in the original
        langValues = langValues - extraOptions
        # language options that should have values
        missingAllValues = existingValues - langValues
        if missingAllValues:
            print(f"  Missing {len(missingAllValues)} all values: {', '.join(sorted(list(missingAllValues))[:5])}")
            if len(missingAllValues) > 5:
                print(f"  ... and {len(missingAllValues) - 5} more")
            valuesError = True
        # language options that should have no values
        extraAllValues = langValues - existingValues
        if extraAllValues:
            print(f"  Extra {len(extraAllValues)} all values: {', '.join(sorted(list(extraAllValues))[:5])}")
            if len(extraAllValues) > 5:
                print(f"  ... and {len(extraAllValues) - 5} more")
            valuesError = True

        # both have values, some further investigations
        bothValues = existingValues - missingAllValues
        # partial extra / missing attr
        bothError = set()
        for optionId in bothValues:
            optionElem = existingOptionsWithElements[optionId]
            langElem = langOptionsWithElements[optionId]
            optValues = set()
            langValues = set()
            for optValue in optionElem.getElementsByTagName('value'):
                 optValues.add(optValue.getAttribute('name'))
            for langValue in langElem.getElementsByTagName('value'):
                 langValues.add(langValue.getAttribute('name'))
            missingValues = optValues - langValues
            missingLen = len(missingValues)
            extraLen = len(langValues - optValues)
            if missingLen:
                print(f"  Missing {missingLen} values for {optionId}")
                bothError.add(optionId)
            if extraLen:
                print(f"  Extra {extraLen} values for {optionId}")
                bothError.add(optionId)
            # both have value elements
            bothName = optValues - missingValues
            for name in bothName:
                toCorrect = False
                for optValue in optionElem.getElementsByTagName('value'):
                    if optValue.getAttribute('name') == name:
                        optAttr = set(optValue.attributes.items())
                        break
                for langValue in langElem.getElementsByTagName('value'):
                    if langValue.getAttribute('name') == name:
                        langAttr = set(langValue.attributes.items())
                        break
                missing = optAttr - langAttr
                extra = langAttr - optAttr
                if missing or extra:
                    if len(missing) == 1 and len(extra) == 1:
                        for attr, dummy in missing:
                            missAttr = attr
                            break
                        for attr, dummy in extra:
                            extraAttr = attr
                            break
                        if missAttr != 'desc' or extraAttr != 'desc':
                           bothError.add(optionId)
                           toCorrect = True
                    else:
                        bothError.add(optionId)
                        toCorrect = True
                if toCorrect:
                    print(f"  Differences in attributes for value with name {name} for {optionId}")

        if not (valuesError or bothError):
            print("  OK - all values are synchronized")


        if autoSync and (optionsError or messagesError or attribError or valuesError or bothError or headerError):
            print("  Auto-syncing...")

            rootElement = langDoc.documentElement

            if headerError < 0:
                print("    Extra header documentation in translation")
                print("      Leaving translated file unchanged (unknown what to remove)")
            elif headerError > 0:
                head = None
                for header in rootElement.getElementsByTagName('header'):
                  head = header
                  break;
                if not head:
                    # add header element
                    head = langDoc.createElement("header")
                    langDoc.childNodes[0].appendChild(head)

                for doc in existingHeader:
                    head.appendChild(doc)
                print("    Header documentation not (all) has been translated")
                print(f"      Added all original headers containing filter: {mode}")

            missingOptions = existingOptions - langOptions
            extraOptions = langOptions - existingOptions

            parentGroupNew = None
            for group in langDoc.getElementsByTagName('group'):
                parentGroupNew = group
                break

            if not parentGroupNew:
                # add group element
                parentGroupNew = langDoc.createElement("group")
                langDoc.childNodes[0].appendChild(parentGroupNew)

            for optionId in extraOptions:
                optionElem = langOptionsWithElements[optionId]
                parentGroup = optionElem.parentNode
                parentGroup.removeChild(optionElem)
                print(f"    Removed: {optionId}")

            for optionId in missingOptions:
                optionElem = existingOptionsWithElements[optionId]
                importedElem = langDoc.importNode(optionElem, True)
                parentGroupName = optionElem.parentNode.getAttribute('name')

                parentGroupNew = None
                for group in rootElement.getElementsByTagName('group'):
                    parentGroupNew = group
                    break
                for group in rootElement.getElementsByTagName('group'):
                    if group.getAttribute('name') == parentGroupName:
                        parentGroupNew = group
                        break

                if parentGroupNew:
                    parentGroupNew.appendChild(importedElem)
                    print(f"    Added: {optionId}")

            # handle option attributes
            for optionId in attribError:
                existingElem = existingOptionsWithElements[optionId]
                langElem = langOptionsWithElements[optionId]
                for attr,val in langElem.attributes.items():
                    langElem.removeAttribute(attr)
                for attr,val in existingElem.attributes.items():
                    langElem.setAttribute(attr,val)

            # handle values
            for optionId in extraAllValues:
                optionElem = langOptionsWithElements[optionId]
                for valueElem in optionElem.getElementsByTagName('value'):
                    optionElem.removeChild(valueElem)
                print(f"    Removed all values of: {optionId}")

            for optionId in missingAllValues:
                if optionId in langOptionsWithElements:
                    existingElem = existingOptionsWithElements[optionId]
                    langElem = langOptionsWithElements[optionId]
                    for valueElem in existingElem.getElementsByTagName('value'):
                        importedElem = langDoc.importNode(valueElem, True)
                        langElem.appendChild(importedElem)
                    print(f"    Added all values for: {optionId}")

            # handle bothValue errors
            for optionId in bothError:
                optionElem = existingOptionsWithElements[optionId]
                langElem = langOptionsWithElements[optionId]
                optValues = set()
                langValues = set()
                for optValue in optionElem.getElementsByTagName('value'):
                     optValues.add(optValue.getAttribute('name'))
                for langValue in langElem.getElementsByTagName('value'):
                     langValues.add(langValue.getAttribute('name'))
                missing = optValues - langValues
                extra = langValues - optValues
                existingElem = existingOptionsWithElements[optionId]
                langElem = langOptionsWithElements[optionId]
                if missing:
                    for miss in missing:
                        for allVal in existingElem.getElementsByTagName('value'):
                            if allVal.getAttribute('name') == miss:
                                importedElem = langDoc.importNode(allVal, True)
                                langElem.appendChild(importedElem)
                                print(f"    Added: value {miss} of {optionId}")
                if extra:
                    for extr in extra:
                        for allVal in langElem.getElementsByTagName('value'):
                            if allVal.getAttribute('name') == extr:
                                langElem.removeChild(allVal)
                                print(f"    Removed: value {extr} of {optionId}")

                # both have value elements
                bothName = optValues - missing
                for name in bothName:
                    toCorrect = False
                    for optValue in optionElem.getElementsByTagName('value'):
                        if optValue.getAttribute('name') == name:
                            optAttr = set(optValue.attributes.items())
                            break
                    for langValue in langElem.getElementsByTagName('value'):
                        if langValue.getAttribute('name') == name:
                            langAttr = set(langValue.attributes.items())
                            break
                    missing = optAttr - langAttr
                    extra = langAttr - optAttr
                    if missing or extra:
                        if len(missing) == 1 and len(extra) == 1:
                            for attr, dummy in missing:
                                missAttr = attr
                                break
                            for attr, dummy in extra:
                                extraAttr = attr
                                break
                            if missAttr != 'desc' or extraAttr != 'desc':
                               toCorrect = True
                        else:
                            toCorrect = True

                    if toCorrect:
                        optHasDesc = False
                        for optValue in optionElem.getElementsByTagName('value'):
                            if optValue.getAttribute('name') == name:
                                for attr, dummy in optValue.attributes.items():
                                    if attr == 'desc':
                                        optHasDesc = True
                                        break
                                break
                        langHasDesc = False
                        for langValue in langElem.getElementsByTagName('value'):
                            if langValue.getAttribute('name') == name:
                                for attr, dummy in langValue.attributes.items():
                                    if attr == 'desc':
                                        langHasDesc = True
                                        break
                                break
                        for langValue in langElem.getElementsByTagName('value'):
                            if langValue.getAttribute('name') == name:
                                for attr, dummy in langValue.attributes.items():
                                    if attr != 'desc' or not optHasDesc:
                                        langValue.removeAttribute(attr)
                                lang = langValue
                                break
                        for optValue in optionElem.getElementsByTagName('value'):
                            if optValue.getAttribute('name') == name:
                                for attr, val in optValue.attributes.items():
                                    if attr != 'desc' or not langHasDesc:
                                        lang.setAttribute(attr,val)
                                break


            # handle generator / messages
            missingMessages = existingMessages - langMessages
            extraMessages = langMessages - existingMessages

            parentGeneratorNew = None
            for generator in langDoc.getElementsByTagName('generator'):
                parentGeneratorNew = generator

            if not parentGeneratorNew:
                    # add generator element
                    parentGeneratorNew = langDoc.createElement("generator")
                    langDoc.childNodes[0].appendChild(parentGeneratorNew)

            for messageId in extraMessages:
                for message in parentGeneratorNew.getElementsByTagName('message'):
                    if messageId == message.getAttribute('name'):
                        parentGeneratorNew.removeChild(message)
                        print(f"    Removed: {messageId}")

            for generator in elem.getElementsByTagName('generator'):
                parentGenerator = generator

            for messageId in missingMessages:
                for message in parentGenerator.getElementsByTagName('message'):
                    if messageId == message.getAttribute('name'):
                        importedElem = langDoc.importNode(message, True)
                        parentGeneratorNew.appendChild(importedElem)
                        print(f"    Added: {messageId}")

            backupFile = configFile + ".bak"
            shutil.copy2(configFile, backupFile)

            outputContent = langDoc.toprettyxml(indent='  ', encoding='utf-8')
            outputStr = outputContent.decode('utf-8') if isinstance(outputContent, bytes) else outputContent

            lines = outputStr.split('\n')
            filteredLines = re.sub("> *\n *\n *\n",">\n","\n".join(lines));
            filteredLines = re.sub("> *\n *\n",">\n",filteredLines);

            with io.open(configFile, 'w', encoding='utf8') as f:
                f.write(filteredLines)

            print(f"  Backup saved to: {backupFile}")
            print(f"  File updated: {configFile}")

    if report:
        print("```")
    else:
        print(f"\nSync {'complete' if not autoSync else 'and update complete'}!")

def main():
    modes = ['-doc','-cpp','-wiz','-wizswitch','-maph','-maps','-sync','-report','-auto']
    parser = argparse.ArgumentParser(add_help=False,
            usage='%(prog)s -doc|-cpp|-wiz|-wizswitch|-maph|-maps|-sync|-report|-auto config.xml [translations_dir]')
    modeGroup = parser.add_mutually_exclusive_group(required=True)
    for opt in modes:
        modeGroup.add_argument(opt, dest='mode', action='store_const', const=opt)
    parser.add_argument('files', nargs='+')
    args = parser.parse_args()
    mode = args.mode
    files = args.files

    try:
        configFile = files[0]
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
        if n.nodeType == Node.ELEMENT_NODE and n.nodeName == "generator":
            parseGenerator(n)
    if len(messages)==0:
        sys.exit('<generator> section missing in %s' % configFile)

    if mode == "-doc":
        mode = "documentation"
        print("/* WARNING: This file is generated!")
        print(f" * Do not edit this file, but edit {configFile} instead and run")
        print(f" * python configgen.py -doc {configFile} to regenerate this file!")
        print(" */")
        # process header
        foundHeader = False
        foundFooter = False
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE and n.nodeName == "header":
                foundHeader = True
                parseHeaderDoc(n, mode)
        if not foundHeader:
            print(f"/*! \\page {re.sub('.xml','',os.path.basename(configFile))}")
        # generate list with all commands
        commandsList = ()
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE and n.nodeName == "group":
                commandsList = parseGroupsList(n, commandsList)
        print("\\secreflist")
        for x in sorted(commandsList):
            print(f"\\refitem cfg_{x.lower()} {x}")
        print("\\endsecreflist")
        # process groups and options
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE and n.nodeName == "group":
                parseGroupsDoc(n, mode)
        # process footers
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE and n.nodeName == "footer":
                foundFooter = True
                parseFooterDoc(n, mode)
        if not foundFooter:
            print("*/")
    elif mode == "-maph":
        print("/* WARNING: This file is generated!")
        print(f" * Do not edit this file, but edit {configFile} instead and run")
        print(f" * python configgen.py -maph {configFile} to regenerate this file!")
        print(" */")
        print("#ifndef CONFIGVALUES_H")
        print("#define CONFIGVALUES_H")
        print("")
        print("#include <string>")
        print("#include <unordered_map>")
        print("#include \"dstring.h\"")
        print("#include \"containers.h\"")
        print("#include \"settings.h\"")
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE and n.nodeName == "group":
                parseGroupMapEnums(n)
        print("")
        print("class ConfigValues")
        print("{")
        print("  public:")
        print("    static ConfigValues &instance() { static ConfigValues theInstance; return theInstance; }")
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE and n.nodeName == "group":
                parseGroupMapGetter(n)
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE and n.nodeName == "group":
                parseGroupMapSetter(n)
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE and n.nodeName == "group":
                parseGroupMapAvailable(n)
        print("    void init();")
        print("    StringVector fields() const;")
        print("    struct Info")
        print("    {")
        print("      enum Type { Bool, Int, String, List, Unknown };")
        print("      using Enum2BoolMap = std::unordered_map<std::string,bool>;")
        print("      Info(Type t,bool         ConfigValues::*b) : type(t), value(b) {}")
        print("      Info(Type t,int          ConfigValues::*i) : type(t), value(i) {}")
        print("      Info(Type t,DString     ConfigValues::*s, const Enum2BoolMap &boolMap = {}) : type(t), value(s), m_boolMap(boolMap) {}")
        print("      Info(Type t,StringVector ConfigValues::*l) : type(t), value(l) {}")
        print("      Type type;")
        print("      union Item")
        print("      {")
        print("        Item(bool         ConfigValues::*v) : b(v) {}")
        print("        Item(int          ConfigValues::*v) : i(v) {}")
        print("        Item(DString     ConfigValues::*v) : s(v) {}")
        print("        Item(StringVector ConfigValues::*v) : l(v) {}")
        print("        bool         ConfigValues::*b;")
        print("        int          ConfigValues::*i;")
        print("        DString     ConfigValues::*s;")
        print("        StringVector ConfigValues::*l;")
        print("      } value;")
        print("      bool getBooleanRepresentation() const;")
        print("    private:")
        print("      Enum2BoolMap m_boolMap;")
        print("    };")
        print("    const Info *get(const DString &tag) const;")
        print("")
        print("  private:")
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE and n.nodeName == "group":
                parseGroupMapVar(n)
        print("};")
        print("")
        print("#endif")
    elif mode == "-maps":
        print("/* WARNING: This file is generated!")
        print(f" * Do not edit this file, but edit {configFile} instead and run")
        print(f" * python configgen.py -maps {configFile} to regenerate this file!")
        print(" */")
        print("#include \"configvalues.h\"")
        print("#include \"configimpl.h\"")
        print("#include <unordered_map>")
        print("")
        print("const ConfigValues::Info *ConfigValues::get(const DString &tag) const")
        print("{")
        print("  static const std::unordered_map< std::string, Info > configMap =")
        print("  {")
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE and n.nodeName == "group":
                parseGroupMapInit(n)
        print("  };")
        print("  auto it = configMap.find(tag.str());")
        print("  return it!=configMap.end() ? &it->second : nullptr;")
        print("}")
        print("")
        print("void ConfigValues::init()")
        print("{")
        print("  static bool first = true;")
        print("  if (!first) return;")
        print("  first = false;")
        print("")
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE and n.nodeName == "group":
                parseGroupInit(n)
        print("}")
        print("")
        print("StringVector ConfigValues::fields() const")
        print("{")
        print("  return {")
        first=True
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE and n.nodeName == "group":
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
    elif mode == "-cpp":
        mode = "doxyfile"
        print("/* WARNING: This file is generated!")
        print(f" * Do not edit this file, but edit {configFile} instead and run")
        print(f" * python configgen.py -cpp {configFile} to regenerate this file!")
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
            if n.nodeType == Node.ELEMENT_NODE and n.nodeName == "header":
                parseHeader(n,'cfg', mode)
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE and n.nodeName == "group":
                parseGroups(n, mode)
        print("}")
    elif mode == "-wiz":
        mode = "doxywizard"
        configFile = files[0]
        locale = re.sub('.*config', '', configFile)
        locale = re.sub('.xml', '', locale)
        print("/* WARNING: This file is generated!")
        print(f" * Do not edit this file, but edit {configFile} instead and run")
        print(f" * python configgen.py -wiz {configFile} to regenerate this file!")
        print(" */")
        print("#include \"configdoc.h\"")
        print("#include \"docintf.h\"")
        print("")

        print(f"void addConfigDocs{locale}(DocIntf *doc)")
        print("{")
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE and n.nodeName == "header":
                parseHeader(n,'doc', mode)
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE and n.nodeName == "group":
                parseGroupCDocs(n, mode)
        print("}")
    elif mode == "-wizswitch":
        print("#ifndef CONFIGSWITCHER_H")
        print("#define CONFIGSWITCHER_H")
        print("")
        print("using LangFunc = void (*)(DocIntf *);")
        print("")
        for x in files:
            locale = re.sub('.*config', '', x)
            locale = re.sub('.xml', '', locale)
            print(f"void addConfigDocs{locale}(DocIntf *doc);")
        print("")
        print("static const std::unordered_map<std::string,LangFunc> langNames =")
        print("{")
        for x in files:
            locale = re.sub('.*config_', '', x)
            locale = re.sub('.*config', '', locale)
            locale = re.sub('.xml', '', locale)
            if not locale:
                print('  { "en", addConfigDocs},')
            else:
                print(f'  {{ "{locale}", addConfigDocs_{locale}}},')
        print("};")
        print("")
        print("#endif")
    elif mode == "-sync" or mode == "-report" or mode == "-auto":
        if len(files) < 2:
            translationsDir = 'i18n'
        else:
            translationsDir = files[1]
        syncLocalizedConfig(elem, configFile, translationsDir, mode == "-auto", mode == "-report")

if __name__ == '__main__':
    main()
