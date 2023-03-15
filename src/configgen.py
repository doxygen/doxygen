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
from xml.dom import minidom, Node
import io

def xprint(str):
    if sys.version_info[0] == 2:
        str1 = unicode(str)
        sys.stdout.write(str1)
        sys.stdout.write(u"\n")
    else:
        print(str)


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
                xprint("  %s->addValue(\"%s\");" % (var, name))


def parseHeader(node,objName):
    doc = ""
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE:
            if (n.nodeName == "docs"):
                if (n.getAttribute('doxyfile') != "0"):
                    doc += parseDocs(n)
    docC = transformDocs(doc)
    xprint("  %s->setHeader(" % (objName))
    rng = len(docC)
    for i in range(rng):
        line = docC[i]
        if i != rng - 1:  # since we go from 0 to rng-1
            xprint("              \"%s\\n\"" % (line))
        else:
            xprint("              \"%s\"" % (line))
    xprint("             );")


def prepCDocs(node):
    type = node.getAttribute('type')
    format = node.getAttribute('format')
    defval = node.getAttribute('defval')
    adefval = node.getAttribute('altdefval')
    doc = "";
    if (type != 'obsolete'):
        for n in node.childNodes:
            if (n.nodeName == "docs"):
                if (n.getAttribute('doxyfile') != "0"):
                    if n.nodeType == Node.ELEMENT_NODE:
                        doc += parseDocs(n)
        if (type == 'enum'):
            values = collectValues(node)
            doc += "<br/>Possible values are: "
            rng = len(values)
            for i in range(rng):
                val = values[i]
                if i == rng - 2:
                    doc += "%s and " % (val)
                elif i == rng - 1:
                    doc += "%s." % (val)
                else:
                    doc += "%s, " % (val)
            if (defval != ""):
                doc += "<br/>The default value is: <code>%s</code>." % (defval)
        elif (type == 'int'):
            minval = node.getAttribute('minval')
            maxval = node.getAttribute('maxval')
            doc += "<br/>%s: %s, %s: %s, %s: %s." % (" Minimum value", minval, 
                     "maximum value", maxval,
                     "default value", defval)
        elif (type == 'bool'):
            if (node.hasAttribute('altdefval')):
                doc += "<br/>%s: %s." % ("The default value is", "system dependent")
            else:
                doc += "<br/>%s: %s." % ("The default value is", "YES" if (defval == "1") else "NO")
        elif (type == 'list'):
            if format == 'string':
                values = collectValues(node)
                rng = len(values)
                for i in range(rng):
                    val = values[i]
                    if i == rng - 2:
                        doc += "%s and " % (val)
                    elif i == rng - 1:
                        doc += "%s." % (val)
                    else:
                        doc += "%s, " % (val)
        elif (type == 'string'):
            if format == 'dir':
                if defval != '':
                    doc += "<br/>The default directory is: <code>%s</code>." % (
                        defval)
            elif format == 'file':
                abspath = node.getAttribute('abspath')
                if defval != '':
                    if abspath != '1':
                        doc += "<br/>The default file is: <code>%s</code>." % (
                            defval)
                    else:
                        doc += "<br/>%s: %s%s%s." % (
                            "The default file (with absolute path) is",
                            "<code>",defval,"</code>")
                else:
                    if abspath == '1':
                        doc += "<br/>The file has to be specified with full path."
            elif format =='image':
                abspath = node.getAttribute('abspath')
                if defval != '':
                    if abspath != '1':
                        doc += "<br/>The default image is: <code>%s</code>." % (
                            defval)
                    else:
                        doc += "<br/>%s: %s%s%s." % (
                            "The default image (with absolute path) is",
                            "<code>",defval,"</code>")
                else:
                    if abspath == '1':
                        doc += "<br/>The image has to be specified with full path."
            else: # format == 'string':
                if defval != '':
                    doc += "<br/>The default value is: <code>%s</code>." % (
                        defval)
        # depends handling
        if (node.hasAttribute('depends')):
            depends = node.getAttribute('depends')
            doc += "<br/>%s \\ref cfg_%s \"%s\" is set to \\c YES." % (
                "This tag requires that the tag", depends.lower(), depends.upper())

    docC = transformDocs(doc)
    return docC;


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
    docC = prepCDocs(node);
    if len(setting) > 0:
        xprint("#if %s" % (setting))
    xprint("  //----")
    if type == 'bool':
        if len(adefval) > 0:
            enabled = adefval
        elif defval == '1':
            enabled = "TRUE"
        else:
            enabled = "FALSE"
        xprint("  cb = cfg->addBool(")
        xprint("             \"%s\"," % (name))
        rng = len(docC)
        for i in range(rng):
            line = docC[i]
            if i != rng - 1:  # since we go from 0 to rng-1
                xprint("              \"%s\\n\"" % (line))
            else:
                xprint("              \"%s\"," % (line))
        xprint("              %s" % (enabled))
        xprint("             );")
        if depends != '':
            xprint("  cb->addDependency(\"%s\");" % (depends))
    elif type == 'string':
        xprint("  cs = cfg->addString(")
        xprint("              \"%s\"," % (name))
        rng = len(docC)
        for i in range(rng):
            line = docC[i]
            if i != rng - 1:  # since we go from 0 to rng-1
                xprint("              \"%s\\n\"" % (line))
            else:
                xprint("              \"%s\"" % (line))
        xprint("             );")
        if defval != '':
            xprint("  cs->setDefaultValue(\"%s\");" % (defval.replace('\\','\\\\')))
        if format == 'file':
            xprint("  cs->setWidgetType(ConfigString::File);")
        elif format == 'image':
            xprint("  cs->setWidgetType(ConfigString::Image);")
        elif format == 'dir':
            xprint("  cs->setWidgetType(ConfigString::Dir);")
        elif format == 'filedir':
            xprint("  cs->setWidgetType(ConfigString::FileAndDir);")
        if depends != '':
            xprint("  cs->addDependency(\"%s\");" % (depends))
    elif type == 'enum':
        xprint("  ce = cfg->addEnum(")
        xprint("              \"%s\"," % (name))
        rng = len(docC)
        for i in range(rng):
            line = docC[i]
            if i != rng - 1:  # since we go from 0 to rng-1
                xprint("              \"%s\\n\"" % (line))
            else:
                xprint("              \"%s\"," % (line))
        xprint("              \"%s\"" % (defval))
        xprint("             );")
        addValues("ce", node)
        if depends != '':
            xprint("  ce->addDependency(\"%s\");" % (depends))
    elif type == 'int':
        minval = node.getAttribute('minval')
        maxval = node.getAttribute('maxval')
        xprint("  ci = cfg->addInt(")
        xprint("              \"%s\"," % (name))
        rng = len(docC)
        for i in range(rng):
            line = docC[i]
            if i != rng - 1:  # since we go from 0 to rng-1
                xprint("              \"%s\\n\"" % (line))
            else:
                xprint("              \"%s\"," % (line))
        xprint("              %s,%s,%s" % (minval, maxval, defval))
        xprint("             );")
        if depends != '':
            xprint("  ci->addDependency(\"%s\");" % (depends))
    elif type == 'list':
        xprint("  cl = cfg->addList(")
        xprint("              \"%s\"," % (name))
        rng = len(docC)
        for i in range(rng):
            line = docC[i]
            try:
                if i != rng - 1:  # since we go from 0 to rng-1
                    xprint("              \"%s\\n\"" % (line))
                else:
                    xprint("              \"%s\"" % (line))
            except Exception as inst:
                sys.stdout = sys.stderr
                xprint("")
                xprint(inst)
                xprint("")
        xprint("             );")
        addValues("cl", node)
        if depends != '':
            xprint("  cl->addDependency(\"%s\");" % (depends))
        if format == 'file':
            xprint("  cl->setWidgetType(ConfigList::File);")
        elif format == 'dir':
            xprint("  cl->setWidgetType(ConfigList::Dir);")
        elif format == 'filedir':
            xprint("  cl->setWidgetType(ConfigList::FileAndDir);")
    elif type == 'obsolete':
        xprint("  cfg->addObsolete(\"%s\",ConfigOption::O_%s);" % (name,orgtype.capitalize()))
    if len(setting) > 0:
        xprint("#else")
        xprint("  cfg->addDisabled(\"%s\");" % (name))
        xprint("#endif")


def parseGroups(node):
    name = node.getAttribute('name')
    doc = node.getAttribute('docs')
    setting = node.getAttribute('setting')
    if len(setting) > 0:
        xprint("#if %s" % (setting))
    xprint("%s%s" % ("  //-----------------------------------------",
                    "----------------------------------"))
    xprint("  cfg->addInfo(\"%s\",\"%s\");" % (name, doc))
    xprint("%s%s" % ("  //-----------------------------------------",
                    "----------------------------------"))
    if len(setting) > 0:
        xprint("#endif")
    xprint("")
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
                xprint("\nenum class %s_t" % (name))
                xprint("{")
                for nv in n.childNodes:
                    if nv.nodeName == "value":
                        value = nv.getAttribute('name')
                        if value:
                            xprint("  %s," % (escape(value)))
                xprint("};\n")
                xprint("inline {0}_t {1}_str2enum(const QCString &s)".format(name,name))
                xprint("{")
                xprint("  QCString lc = s.lower();")
                xprint("  static const std::unordered_map<std::string,{0}_t> map =".format(name))
                xprint("  {")
                for nv in n.childNodes:
                    if nv.nodeName == "value":
                        value = nv.getAttribute('name')
                        if value:
                            xprint("    {{ \"{0}\", {1}_t::{2} }},".format(value.lower(),name,escape(value)))
                xprint("  };")
                xprint("  auto it = map.find(lc.str());")
                xprint("  return it!=map.end() ? it->second : {0}_t::{1};".format(name,escape(defval)))
                xprint("}\n")
                xprint("inline QCString {0}_enum2str({1}_t v)".format(name,name))
                xprint("{")
                xprint("  switch(v)")
                xprint("  {")
                for nv in n.childNodes:
                    if nv.nodeName == "value":
                        value = nv.getAttribute('name')
                        if value:
                            xprint("    case {0}_t::{1}: return \"{2}\";".format(name,escape(value),value))
                xprint("  }")
                xprint("  return \"{0}\";".format(defval))
                xprint("}")

def parseGroupMapGetter(node):
    map = { 'bool':'bool', 'string':'const QCString &', 'int':'int', 'list':'const StringVector &' }
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE:
            setting = n.getAttribute('setting')
            if len(setting) > 0:
                xprint("#if %s" % (setting))
            type = n.getAttribute('type')
            name = n.getAttribute('id')
            if type=='enum':
                xprint("    %-22s %-30s const                  { return %s(m_%s); }" % (name+'_t',name+'()',name+'_str2enum',name))
                xprint("    %-22s %-30s const                  { return m_%s; }" % ('const QCString &',name+'_str()',name))
            elif type in map:
                xprint("    %-22s %-30s const                  { return m_%s; }" % (map[type],name+'()',name))
            if len(setting) > 0:
                xprint("#endif")

def parseGroupMapSetter(node):
    map = { 'bool':'bool', 'string':'const QCString &', 'int':'int', 'list':'const StringVector &' }
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE:
            setting = n.getAttribute('setting')
            if len(setting) > 0:
                xprint("#if %s" % (setting))
            type = n.getAttribute('type')
            name = n.getAttribute('id')
            if type=='enum':
                xprint("    [[maybe_unused]] %-22s update_%-46s { m_%s = %s(v); return v; }" % (name+'_t',name+'('+name+'_t '+' v)',name,name+'_enum2str'))
            elif type in map:
                xprint("    [[maybe_unused]] %-22s update_%-46s { m_%s = v; return m_%s; }" % (map[type],name+'('+map[type]+' v)',name,name))
            if len(setting) > 0:
                xprint("#endif")

def parseGroupMapAvailable(node):
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE:
            setting = n.getAttribute('setting')
            type = n.getAttribute('type')
            name = n.getAttribute('id')
            if type=='enum':
                if len(setting) > 0:
                    xprint("#if %s" % (setting))
                xprint("    %-22s isAvailable_%-41s { return v.lower() == %s_enum2str(%s_str2enum(v)).lower(); }" % ('bool',name+'(QCString v)',name,name));
                if len(setting) > 0:
                    xprint("#endif")

def parseGroupMapVar(node):
    map = { 'bool':'bool', 'string':'QCString', 'enum':'QCString', 'int':'int', 'list':'StringVector' }
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE:
            setting = n.getAttribute('setting')
            if len(setting) > 0:
                xprint("#if %s" % (setting))
            type = n.getAttribute('type')
            name = n.getAttribute('id')
            if type in map:
                xprint("    %-12s m_%s;" % (map[type],name))
            if len(setting) > 0:
                xprint("#endif")

def parseGroupInit(node):
    map = { 'bool':'Bool', 'string':'String', 'enum':'Enum', 'int':'Int', 'list':'List' }
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE:
            setting = n.getAttribute('setting')
            if len(setting) > 0:
                xprint("#if %s" % (setting))
            type = n.getAttribute('type')
            name = n.getAttribute('id')
            if type in map:
                xprint("  %-25s = ConfigImpl::instance()->get%s(__FILE__,__LINE__,\"%s\");" % ('m_'+name,map[type],name))
            if len(setting) > 0:
                xprint("#endif")

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
                xprint("#if %s" % (setting))
            type = n.getAttribute('type')
            name = n.getAttribute('id')
            if type in map:
                if type == "enum":
                    mappingStr = "{%s}" % (', '.join(getEnum2BoolMapping(n)))
                    xprint("    { %-26s Info{ %-13s &ConfigValues::m_%-23s %s}}," % ('\"'+name+'\",','Info::'+map[type]+',',name+",", mappingStr))
                else:
                    xprint("    { %-26s Info{ %-13s &ConfigValues::m_%-24s}}," % ('\"'+name+'\",','Info::'+map[type]+',',name))
            if len(setting) > 0:
                xprint("#endif")

def parseGroupCDocs(node):
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE:
            type = n.getAttribute('type')
            name = n.getAttribute('id')
            docC = prepCDocs(n);
            if type != 'obsolete':
                xprint("  doc->add(")
                xprint("              \"%s\"," % (name))
                rng = len(docC)
                for i in range(rng):
                    line = docC[i]
                    if i != rng - 1:  # since we go from 0 to rng-1
                        xprint("              \"%s\\n\"" % (line))
                    else:
                        xprint("              \"%s\"" % (line))
                xprint("          );")

def parseOptionDoc(node, first):
    # Handling part for documentation
    name = node.getAttribute('id')
    type = node.getAttribute('type')
    format = node.getAttribute('format')
    defval = node.getAttribute('defval')
    adefval = node.getAttribute('altdefval')
    depends = node.getAttribute('depends')
    setting = node.getAttribute('setting')
    doc = ""
    if (type != 'obsolete'):
        for n in node.childNodes:
            if (n.nodeName == "docs"):
                if (n.getAttribute('documentation') != "0"):
                    if n.nodeType == Node.ELEMENT_NODE:
                        doc += parseDocs(n)
        if (first):
            xprint(" \\anchor cfg_%s" % (name.lower()))
            xprint("<dl>")
            xprint("")
            xprint("<dt>\\c %s <dd>" % (name))
        else:
            xprint(" \\anchor cfg_%s" % (name.lower()))
            xprint("<dt>\\c %s <dd>" % (name))
        xprint(" \\addindex %s" % (name))
        xprint(doc)
        if (type == 'enum'):
            values = collectValues(node)
            xprint("")
            xprint("Possible values are: ")
            rng = len(values)
            for i in range(rng):
                val = values[i]
                if i == rng - 2:
                    xprint("%s and " % (val))
                elif i == rng - 1:
                    xprint("%s." % (val))
                else:
                    xprint("%s, " % (val))
            if (defval != ""):
                xprint("")
                xprint("")
                xprint("The default value is: <code>%s</code>." % (defval))
            xprint("")
        elif (type == 'int'):
            minval = node.getAttribute('minval')
            maxval = node.getAttribute('maxval')
            xprint("")
            xprint("")
            xprint("%s: %s%s%s, %s: %s%s%s, %s: %s%s%s." % (
                     " Minimum value", "<code>", minval, "</code>", 
                     "maximum value", "<code>", maxval, "</code>",
                     "default value", "<code>", defval, "</code>"))
            xprint("")
        elif (type == 'bool'):
            xprint("")
            xprint("")
            if (node.hasAttribute('altdefval')):
                xprint("The default value is: system dependent.")
            else:
                xprint("The default value is: <code>%s</code>." % (
                    "YES" if (defval == "1") else "NO"))
            xprint("")
        elif (type == 'list'):
            if format == 'string':
                values = collectValues(node)
                rng = len(values)
                for i in range(rng):
                    val = values[i]
                    if i == rng - 2:
                        xprint("%s and " % (val))
                    elif i == rng - 1:
                        xprint("%s." % (val))
                    else:
                        xprint("%s, " % (val))
            xprint("")
        elif (type == 'string'):
            if format == 'dir':
                if defval != '':
                    xprint("")
                    xprint("The default directory is: <code>%s</code>." % (
                        defval))
            elif format == 'file':
                abspath = node.getAttribute('abspath')
                if defval != '':
                    xprint("")
                    if abspath != '1':
                        xprint("The default file is: <code>%s</code>." % (
                            defval))
                    else:
                        xprint("%s: %s%s%s." % (
                            "The default file (with absolute path) is",
                            "<code>",defval,"</code>"))
                else:
                    if abspath == '1':
                        xprint("")
                        xprint("The file has to be specified with full path.")
            elif format =='image':
                abspath = node.getAttribute('abspath')
                if defval != '':
                    xprint("")
                    if abspath != '1':
                        xprint("The default image is: <code>%s</code>." % (
                            defval))
                    else:
                        xprint("%s: %s%s%s." % (
                            "The default image (with absolute path) is",
                            "<code>",defval,"</code>"))
                else:
                    if abspath == '1':
                        xprint("")
                        xprint("The image has to be specified with full path.")
            else: # format == 'string':
                if defval != '':
                    xprint("")
                    xprint("The default value is: <code>%s</code>." % (
                        defval.replace('\\','\\\\')))
            xprint("")
        # depends handling
        if (node.hasAttribute('depends')):
            depends = node.getAttribute('depends')
            xprint("")
            xprint("%s \\ref cfg_%s \"%s\" is set to \\c YES." % (
                "This tag requires that the tag", depends.lower(), depends.upper()))
        return False


def parseGroupsDoc(node):
    name = node.getAttribute('name')
    doc = node.getAttribute('docs')
    xprint("\section config_%s %s" % (name.lower(), doc))
    # Start of list has been moved to the first option for better
    # anchor placement
    #  print "<dl>"
    #  print ""
    first = True
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE:
            first = parseOptionDoc(n, first)
    if (not first):
        xprint("</dl>")


def parseGroupsList(node, commandsList):
    list = ()
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
        if n.nodeType == Node.CDATA_SECTION_NODE:
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
    xprint(doc)


def parseFooterDoc(node):
    doc = ""
    for n in node.childNodes:
        if n.nodeType == Node.ELEMENT_NODE:
            if (n.nodeName == "docs"):
                if (n.getAttribute('documentation') != "0"):
                    doc += parseDocs(n)
    xprint(doc)


def main():
    if len(sys.argv)<3 or (not sys.argv[1] in ['-doc','-cpp','-wiz','-maph','-maps']):
        sys.exit('Usage: %s -doc|-cpp|-wiz|-maph|-maps config.xml' % sys.argv[0])
    try:
        if sys.version_info[0] == 2:
            fh = open(sys.argv[2],'r')
            sys.stdout = io.open(sys.stdout.fileno(), 'w')
        else:
            fh = open(sys.argv[2],'r',encoding='utf8')
            sys.stdout = io.open(sys.stdout.fileno(), 'w', encoding='utf8')
        doc = xml.dom.minidom.parse(fh)
    except Exception as inst:
        sys.stdout = sys.stderr
        xprint("")
        xprint(inst)
        xprint("")
        sys.exit(1)
    elem = doc.documentElement
    if (sys.argv[1] == "-doc"):
        xprint("/* WARNING: This file is generated!")
        xprint(" * Do not edit this file, but edit config.xml instead and run")
        xprint(" * python configgen.py -doc config.xml to regenerate this file!")
        xprint(" */")
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
        xprint("\\secreflist")
        for x in sorted(commandsList):
            xprint("\\refitem cfg_%s %s" % (x.lower(), x))
        xprint("\\endsecreflist")
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
        xprint("/* WARNING: This file is generated!")
        xprint(" * Do not edit this file, but edit config.xml instead and run")
        xprint(" * python configgen.py -map config.xml to regenerate this file!")
        xprint(" */")
        xprint("#ifndef CONFIGVALUES_H")
        xprint("#define CONFIGVALUES_H")
        xprint("")
        xprint("#include <string>")
        xprint("#include <unordered_map>")
        xprint("#include \"qcstring.h\"")
        xprint("#include \"containers.h\"")
        xprint("#include \"settings.h\"")
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE:
                if n.nodeName == "group":
                    parseGroupMapEnums(n)
        xprint("")
        xprint("class ConfigValues")
        xprint("{")
        xprint("  public:")
        xprint("    static ConfigValues &instance() { static ConfigValues theInstance; return theInstance; }")
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
        xprint("    void init();")
        xprint("    StringVector fields() const;")
        xprint("    struct Info")
        xprint("    {")
        xprint("      enum Type { Bool, Int, String, List, Unknown };")
        xprint("      using Enum2BoolMap = std::unordered_map<std::string,bool>;");
        xprint("      Info(Type t,bool         ConfigValues::*b) : type(t), value(b) {}")
        xprint("      Info(Type t,int          ConfigValues::*i) : type(t), value(i) {}")
        xprint("      Info(Type t,QCString     ConfigValues::*s, Enum2BoolMap boolMap = {}) : type(t), value(s), m_boolMap(boolMap) {}")
        xprint("      Info(Type t,StringVector ConfigValues::*l) : type(t), value(l) {}")
        xprint("      Type type;")
        xprint("      union Item")
        xprint("      {")
        xprint("        Item(bool         ConfigValues::*v) : b(v) {}")
        xprint("        Item(int          ConfigValues::*v) : i(v) {}")
        xprint("        Item(QCString     ConfigValues::*v) : s(v) {}")
        xprint("        Item(StringVector ConfigValues::*v) : l(v) {}")
        xprint("        bool         ConfigValues::*b;")
        xprint("        int          ConfigValues::*i;")
        xprint("        QCString     ConfigValues::*s;")
        xprint("        StringVector ConfigValues::*l;")
        xprint("      } value;")
        xprint("      bool getBooleanRepresentation() const;")
        xprint("    private:")
        xprint("      Enum2BoolMap m_boolMap;")
        xprint("    };")
        xprint("    const Info *get(const QCString &tag) const;")
        xprint("")
        xprint("  private:")
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE:
                if (n.nodeName == "group"):
                    parseGroupMapVar(n)
        xprint("};")
        xprint("")
        xprint("#endif")
    elif (sys.argv[1] == "-maps"):
        xprint("/* WARNING: This file is generated!")
        xprint(" * Do not edit this file, but edit config.xml instead and run")
        xprint(" * python configgen.py -maps config.xml to regenerate this file!")
        xprint(" */")
        xprint("#include \"configvalues.h\"")
        xprint("#include \"configimpl.h\"")
        xprint("#include <unordered_map>")
        xprint("")
        xprint("const ConfigValues::Info *ConfigValues::get(const QCString &tag) const");
        xprint("{");
        xprint("  static const std::unordered_map< std::string, Info > configMap =");
        xprint("  {");
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE:
                if (n.nodeName == "group"):
                    parseGroupMapInit(n)
        xprint("  };");
        xprint("  auto it = configMap.find(tag.str());");
        xprint("  return it!=configMap.end() ? &it->second : nullptr;");
        xprint("}");
        xprint("")
        xprint("void ConfigValues::init()")
        xprint("{")
        xprint("  static bool first = TRUE;")
        xprint("  if (!first) return;")
        xprint("  first = FALSE;")
        xprint("")
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE:
                if (n.nodeName == "group"):
                    parseGroupInit(n)
        xprint("}")
        xprint("")
        xprint("StringVector ConfigValues::fields() const")
        xprint("{")
        xprint("  return {");
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
                                    xprint(",")
                                first=False
                                sys.stdout.write('    "'+name+'"')
        xprint("")
        xprint("  };")
        xprint("}")
        xprint("")
        xprint("bool ConfigValues::Info::getBooleanRepresentation() const")
        xprint("{")
        xprint("  if (!m_boolMap.empty())")
        xprint("  {")
        xprint("    auto it = m_boolMap.find((ConfigValues::instance().*(value.s)).str());")
        xprint("    if (it!=m_boolMap.end())")
        xprint("    {")
        xprint("      return it->second;");
        xprint("    }")
        xprint("  }")
        xprint("  return false;")
        xprint("}")
        xprint("")
    elif (sys.argv[1] == "-cpp"):
        xprint("/* WARNING: This file is generated!")
        xprint(" * Do not edit this file, but edit config.xml instead and run")
        xprint(" * python configgen.py -cpp config.xml to regenerate this file!")
        xprint(" */")
        xprint("")
        xprint("#include \"configoptions.h\"")
        xprint("#include \"configimpl.h\"")
        xprint("#include \"portable.h\"")
        xprint("#include \"settings.h\"")
        xprint("")
        xprint("void addConfigOptions(ConfigImpl *cfg)")
        xprint("{")
        xprint("  ConfigString *cs;")
        xprint("  ConfigEnum   *ce;")
        xprint("  ConfigList   *cl;")
        xprint("  ConfigInt    *ci;")
        xprint("  ConfigBool   *cb;")
        xprint("")
        # process header
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE:
                if (n.nodeName == "header"):
                    parseHeader(n,'cfg')
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE:
                if (n.nodeName == "group"):
                    parseGroups(n)
        xprint("}")
    elif (sys.argv[1] == "-wiz"):
        xprint("/* WARNING: This file is generated!")
        xprint(" * Do not edit this file, but edit config.xml instead and run")
        xprint(" * python configgen.py -wiz config.xml to regenerate this file!")
        xprint(" */")
        xprint("#include \"configdoc.h\"")
        xprint("#include \"docintf.h\"")
        xprint("")
        xprint("void addConfigDocs(DocIntf *doc)")
        xprint("{")
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE:
                if (n.nodeName == "header"):
                    parseHeader(n,'doc')
        for n in elem.childNodes:
            if n.nodeType == Node.ELEMENT_NODE:
                if (n.nodeName == "group"):
                    parseGroupCDocs(n)
        xprint("}")

if __name__ == '__main__':
    main()
