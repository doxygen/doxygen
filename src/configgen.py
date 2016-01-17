#!/usr/bin/python
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

def transformDocs(doc):
	# join lines, unless it is an empty line
	# remove doxygen layout constructs
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
	doc = re.sub('\\\\ref +external', '"Linking to external documentation"',
				 doc)
	# fallback for not handled
	doc = re.sub('\\\\ref', '', doc)
	#<a href="address">description</a> -> description (see: address)
	doc = re.sub('<a +href="([^"]*)" *>([^<]*)</a>', '\\2 (see: \\1)', doc)
	# LaTeX name as formula -> LaTeX
	doc = doc.replace("\\f$\\mbox{\\LaTeX}\\f$", "LaTeX")
	# Other forula's (now just 2) so explicitely mentioned.
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
	type = node.getAttribute('type')
	format = node.getAttribute('format')
	defval = node.getAttribute('defval')
	adefval = node.getAttribute('altdefval')
	depends = node.getAttribute('depends')
	setting = node.getAttribute('setting')
	docC = prepCDocs(node);
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
			print("  cs->setDefaultValue(\"%s\");" % (defval))
		if format == 'file':
			print("  cs->setWidgetType(ConfigString::File);")
		elif format == 'image':
			print("  cs->setWidgetType(ConfigString::Image);")
		elif format == 'dir':
			print("  cs->setWidgetType(ConfigString::Dir);")
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
			if i != rng - 1:  # since we go from 0 to rng-1
				print("              \"%s\\n\"" % (line))
			else:
				print("              \"%s\"" % (line))
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
		print("  cfg->addObsolete(\"%s\");" % (name))
	if len(setting) > 0:
		print("#else")
		print("  cfg->addDisabled(\"%s\");" % (name))
		print("#endif")


def parseGroups(node):
	name = node.getAttribute('name')
	doc = node.getAttribute('docs')
	print("%s%s" % ("  //-----------------------------------------",
					"----------------------------------"))
	print("  cfg->addInfo(\"%s\",\"%s\");" % (name, doc))
	print("%s%s" % ("  //-----------------------------------------",
					"----------------------------------"))
	print("")
	for n in node.childNodes:
		if n.nodeType == Node.ELEMENT_NODE:
			parseOption(n)

def parseGroupMap(node):
	map = { 'bool':'bool', 'string':'QCString', 'enum':'QCString', 'int':'int', 'list':'QStrList' }
	for n in node.childNodes:
		if n.nodeType == Node.ELEMENT_NODE:
			setting = n.getAttribute('setting')
			if len(setting) > 0:
				print("#if %s" % (setting))
			type = n.getAttribute('type')
			name = n.getAttribute('id')
			if type in map:
				print("    %-8s %s;" % (map[type],name))
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
				print("  %-25s = ConfigImpl::instance()->get%s(__FILE__,__LINE__,\"%s\");" % (name,map[type],name))
			if len(setting) > 0:
				print("#endif")

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
				print("  m_map.insert(\"%s\",new Info%s(&ConfigValues::%s));" % (name,map[type],name))
			if len(setting) > 0:
				print("#endif")

def parseGroupCDocs(node):
	for n in node.childNodes:
		if n.nodeType == Node.ELEMENT_NODE:
			type = n.getAttribute('type')
			name = n.getAttribute('id')
			docC = prepCDocs(n);
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
			print("Possible values are: ")
			rng = len(values)
			for i in range(rng):
				val = values[i]
				if i == rng - 2:
					print("%s and " % (val))
				elif i == rng - 1:
					print("%s." % (val))
				else:
					print("%s, " % (val))
			if (defval != ""):
				print("")
				print("")
				print("The default value is: <code>%s</code>." % (defval))
			print("")
		elif (type == 'int'):
			minval = node.getAttribute('minval')
			maxval = node.getAttribute('maxval')
			print("")
			print("")
			print("%s: %s%s%s, %s: %s%s%s, %s: %s%s%s." % (
					 " Minimum value", "<code>", minval, "</code>", 
					 "maximum value", "<code>", maxval, "</code>",
					 "default value", "<code>", defval, "</code>"))
			print("")
		elif (type == 'bool'):
			print("")
			print("")
			if (node.hasAttribute('altdefval')):
				print("The default value is: system dependent.")
			else:
				print("The default value is: <code>%s</code>." % (
					"YES" if (defval == "1") else "NO"))
			print("")
		elif (type == 'list'):
			if format == 'string':
				values = collectValues(node)
				rng = len(values)
				for i in range(rng):
					val = values[i]
					if i == rng - 2:
						print("%s and " % (val))
					elif i == rng - 1:
						print("%s." % (val))
					else:
						print("%s, " % (val))
			print("")
		elif (type == 'string'):
			if format == 'dir':
				if defval != '':
					print("")
					print("The default directory is: <code>%s</code>." % (
						defval))
			elif format == 'file':
				abspath = node.getAttribute('abspath')
				if defval != '':
					print("")
					if abspath != '1':
						print("The default file is: <code>%s</code>." % (
							defval))
					else:
						print("%s: %s%s%s." % (
							"The default file (with absolute path) is",
							"<code>",defval,"</code>"))
				else:
					if abspath == '1':
						print("")
						print("The file has to be specified with full path.")
			elif format =='image':
				abspath = node.getAttribute('abspath')
				if defval != '':
					print("")
					if abspath != '1':
						print("The default image is: <code>%s</code>." % (
							defval))
					else:
						print("%s: %s%s%s." % (
							"The default image (with absolute path) is",
							"<code>",defval,"</code>"))
				else:
					if abspath == '1':
						print("")
						print("The image has to be specified with full path.")
			else: # format == 'string':
				if defval != '':
					print("")
					print("The default value is: <code>%s</code>." % (
						defval))
			print("")
		# depends handling
		if (node.hasAttribute('depends')):
			depends = node.getAttribute('depends')
			print("")
			print("%s \\ref cfg_%s \"%s\" is set to \\c YES." % (
				"This tag requires that the tag", depends.lower(), depends.upper()))
		return False


def parseGroupsDoc(node):
	name = node.getAttribute('name')
	doc = node.getAttribute('docs')
	print("\section config_%s %s" % (name.lower(), doc))
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
	print(doc)


def parseFooterDoc(node):
	doc = ""
	for n in node.childNodes:
		if n.nodeType == Node.ELEMENT_NODE:
			if (n.nodeName == "docs"):
				if (n.getAttribute('documentation') != "0"):
					doc += parseDocs(n)
	print(doc)


def main():
	if len(sys.argv)<3 or (not sys.argv[1] in ['-doc','-cpp','-wiz','-maph','-maps']):
		sys.exit('Usage: %s -doc|-cpp|-wiz|-maph|-maps config.xml' % sys.argv[0])
	try:
		doc = xml.dom.minidom.parse(sys.argv[2])
	except Exception as inst:
		sys.stdout = sys.stderr
		print("")
		print(inst)
		print("")
		sys.exit(1)
	elem = doc.documentElement
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
		print("#include <qdict.h>")
		print("#include <qstrlist.h>")
		print("#include <qcstring.h>")
		print("#include \"settings.h\"")
		print("")
		print("class ConfigValues")
		print("{")
		print("  public:")
		print("    static ConfigValues &instance() { static ConfigValues theInstance; return theInstance; }")
		for n in elem.childNodes:
			if n.nodeType == Node.ELEMENT_NODE:
				if (n.nodeName == "group"):
					parseGroupMap(n)
		print("    void init();")
		print("    struct Info")
		print("    {")
		print("      enum Type { Bool, Int, String, List, Unknown };")
		print("      Info(Type t) : type(t) {}")
		print("      virtual ~Info() {}")
		print("      Type type;")
		print("    };")
		print("    struct InfoBool : public Info")
		print("    {")
		print("      InfoBool(bool ConfigValues::*ptm) : Info(Info::Bool), item(ptm) {}")
		print("      bool ConfigValues::*item;")
		print("    };")
		print("    struct InfoInt : public Info")
		print("    {")
		print("      InfoInt(int ConfigValues::*ptm) : Info(Info::Int), item(ptm) {}")
		print("      int ConfigValues::*item;")
		print("    };")
		print("    struct InfoString : public Info")
		print("    {")
		print("      InfoString(QCString ConfigValues::*ptm) : Info(Info::String), item(ptm) {}")
		print("      QCString ConfigValues::*item;")
		print("    };")
		print("    struct InfoList : public Info")
		print("    {")
		print("      InfoList(QStrList ConfigValues::*ptm) : Info(Info::List), item(ptm) {}")
		print("      QStrList ConfigValues::*item;")
		print("    };")
		print("    const Info *get(const char *tag) const")
		print("    {")
		print("      return m_map.find(tag);")
		print("    }")
		print("  private:")
		print("    ConfigValues();")
		print("    QDict<Info> m_map;")
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
		print("")
		print("ConfigValues::ConfigValues() : m_map(257)")
		print("{")
		print("  m_map.setAutoDelete(TRUE);")
		for n in elem.childNodes:
			if n.nodeType == Node.ELEMENT_NODE:
				if (n.nodeName == "group"):
					parseGroupMapInit(n)
		print("}")
		print("")
		print("void ConfigValues::init()")
		print("{")
		for n in elem.childNodes:
			if n.nodeType == Node.ELEMENT_NODE:
				if (n.nodeName == "group"):
					parseGroupInit(n)
		print("}")
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
		print("  ConfigString *cs;")
		print("  ConfigEnum   *ce;")
		print("  ConfigList   *cl;")
		print("  ConfigInt    *ci;")
		print("  ConfigBool   *cb;")
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
