#!/usr/bin/env python3
"""
sync_option_translations.py

This script synchronizes configuration options between config.xml and optiontranslations.cpp.
It also generates and applies documentation translation entries for .ts files.

The script identifies options used in the wizard interface and:
1. Reports missing entries in optiontranslations.cpp
2. Generates documentation translation entries for .ts files (only missing ones)
3. Can automatically apply changes to .ts files with --apply flag

Usage:
    python sync_option_translations.py [--config CONFIG_XML] [--options OPTION_TRANSLATIONS_CPP] [--update] [--docs] [--apply]

Options:
    --config       Path to config.xml (default: ../../src/config.xml)
    --options      Path to optiontranslations.cpp (default: ./optiontranslations.cpp)
    --update       Update optiontranslations.cpp instead of just reporting
    --docs         Generate documentation translation entries for .ts files
    --apply        Actually modify .ts files (use with --docs)
    --ts-dir       Directory containing .ts translation files (default: current directory)
    --ts-file      Specific .ts file to check (optional)
"""

import argparse
import os
import re
import xml.etree.ElementTree as ET
from xml.dom import minidom
from typing import Dict, List, Set, Tuple
import html


WIZARD_OPTIONS = {
    "PROJECT_NAME", "PROJECT_LOGO", "PROJECT_BRIEF", "OUTPUT_DIRECTORY", 
    "PROJECT_NUMBER", "RECURSIVE", "OPTIMIZE_OUTPUT_FOR_C", "OPTIMIZE_OUTPUT_JAVA",
    "OPTIMIZE_FOR_FORTRAN", "OPTIMIZE_OUTPUT_VHDL", "OPTIMIZE_OUTPUT_SLICE",
    "CPP_CLI_SUPPORT", "HIDE_SCOPE_NAMES", "EXTRACT_ALL", "GENERATE_HTML",
    "GENERATE_LATEX", "GENERATE_MAN", "GENERATE_RTF", "GENERATE_XML",
    "GENERATE_DOCBOOK", "GENERATE_HTMLHELP", "GENERATE_TREEVIEW", "USE_PDFLATEX",
    "PDF_HYPERLINKS", "SEARCHENGINE", "HAVE_DOT", "CLASS_GRAPH",
    "COLLABORATION_GRAPH", "GRAPHICAL_HIERARCHY", "INCLUDE_GRAPH",
    "INCLUDED_BY_GRAPH", "CALL_GRAPH", "CALLER_GRAPH", "HTML_COLORSTYLE_HUE",
    "HTML_COLORSTYLE_SAT", "HTML_COLORSTYLE_GAMMA",
    
    "DOXYFILE_ENCODING", "PROJECT_ICON", "CREATE_SUBDIRS", "CREATE_SUBDIRS_LEVEL",
    "ALLOW_UNICODE_NAMES", "OUTPUT_LANGUAGE", "BRIEF_MEMBER_DESC", "REPEAT_BRIEF",
    "ABBREVIATE_BRIEF", "ALWAYS_DETAILED_SEC", "INLINE_INHERITED_MEMB",
    "FULL_PATH_NAMES", "STRIP_FROM_PATH", "STRIP_FROM_INC_PATH", "SHORT_NAMES",
    "JAVADOC_AUTOBRIEF", "JAVADOC_BANNER", "QT_AUTOBRIEF", "MULTILINE_CPP_IS_BRIEF",
    "PYTHON_DOCSTRING", "INHERIT_DOCS", "SEPARATE_MEMBER_PAGES", "TAB_SIZE",
    "ALIASES", "EXTENSION_MAPPING", "MARKDOWN_SUPPORT", "MARKDOWN_STRICT",
    "TOC_INCLUDE_HEADINGS", "MARKDOWN_ID_STYLE", "AUTOLINK_SUPPORT",
    "AUTOLINK_IGNORE_WORDS", "BUILTIN_STL_SUPPORT", "SIP_SUPPORT",
    "IDL_PROPERTY_SUPPORT", "DISTRIBUTE_GROUP_DOC", "GROUP_NESTED_COMPOUNDS",
    "SUBGROUPING", "INLINE_GROUPED_CLASSES", "INLINE_SIMPLE_STRUCTS",
    "TYPEDEF_HIDES_STRUCT", "LOOKUP_CACHE_SIZE", "NUM_PROC_THREADS", "TIMESTAMP",
    "EXTRACT_PRIVATE", "EXTRACT_PRIV_VIRTUAL", "EXTRACT_PACKAGE", "EXTRACT_STATIC",
    "EXTRACT_LOCAL_CLASSES", "EXTRACT_LOCAL_METHODS", "EXTRACT_ANON_NSPACES",
    "RESOLVE_UNNAMED_PARAMS", "HIDE_UNDOC_MEMBERS", "HIDE_UNDOC_CLASSES",
    "HIDE_UNDOC_NAMESPACES", "HIDE_FRIEND_COMPOUNDS", "HIDE_IN_BODY_DOCS",
    "INTERNAL_DOCS", "CASE_SENSE_NAMES", "HIDE_COMPOUND_REFERENCE",
    "SHOW_HEADERFILE", "SHOW_INCLUDE_FILES", "SHOW_GROUPED_MEMB_INC",
    "FORCE_LOCAL_INCLUDES", "INLINE_INFO", "SORT_MEMBER_DOCS", "SORT_BRIEF_DOCS",
    "SORT_MEMBERS_CTORS_1ST", "SORT_GROUP_NAMES", "SORT_BY_SCOPE_NAME",
    "STRICT_PROTO_MATCHING", "GENERATE_TODOLIST", "GENERATE_TESTLIST",
    "GENERATE_BUGLIST", "GENERATE_DEPRECATEDLIST", "GENERATE_REQUIREMENTS",
    "REQ_TRACEABILITY_INFO", "ENABLED_SECTIONS", "MAX_INITIALIZER_LINES",
    "SHOW_USED_FILES", "SHOW_FILES", "SHOW_NAMESPACES", "FILE_VERSION_FILTER",
    "LAYOUT_FILE", "CITE_BIB_FILES", "EXTERNAL_TOOL_PATH", "QUIET", "WARNINGS",
    "WARN_IF_UNDOCUMENTED", "WARN_IF_DOC_ERROR", "WARN_IF_INCOMPLETE_DOC",
    "WARN_NO_PARAMDOC", "WARN_IF_UNDOC_ENUM_VAL", "WARN_LAYOUT_FILE",
    "WARN_AS_ERROR", "WARN_FORMAT", "WARN_LINE_FORMAT", "WARN_LOGFILE",
    "INPUT_ENCODING", "INPUT_FILE_ENCODING", "FILE_PATTERNS", "EXCLUDE",
    "EXCLUDE_SYMLINKS", "EXCLUDE_PATTERNS", "EXCLUDE_SYMBOLS", "EXAMPLE_PATH",
    "EXAMPLE_PATTERNS", "EXAMPLE_RECURSIVE", "IMAGE_PATH", "INPUT_FILTER",
    "FILTER_PATTERNS", "FILTER_SOURCE_FILES", "FILTER_SOURCE_PATTERNS",
    "USE_MDFILE_AS_MAINPAGE", "IMPLICIT_DIR_DOCS", "FORTRAN_COMMENT_AFTER",
    "INLINE_SOURCES", "STRIP_CODE_COMMENTS", "REFERENCED_BY_RELATION",
    "REFERENCES_RELATION", "REFERENCES_LINK_SOURCE", "SOURCE_TOOLTIPS",
    "USE_HTAGS", "VERBATIM_HEADERS", "CLANG_ASSISTED_PARSING",
    "CLANG_ADD_INC_PATHS", "CLANG_OPTIONS", "CLANG_DATABASE_PATH",
    "ALPHABETICAL_INDEX", "IGNORE_PREFIX", "HTML_OUTPUT", "HTML_FILE_EXTENSION",
    "HTML_HEADER", "HTML_FOOTER", "HTML_STYLESHEET", "HTML_EXTRA_STYLESHEET",
    "HTML_EXTRA_FILES", "HTML_COLORSTYLE", "HTML_DYNAMIC_MENUS",
    "HTML_DYNAMIC_SECTIONS", "HTML_CODE_FOLDING", "HTML_COPY_CLIPBOARD",
    "HTML_PROJECT_COOKIE", "HTML_INDEX_NUM_ENTRIES", "GENERATE_DOCSET",
    "DOCSET_FEEDNAME", "DOCSET_FEEDURL", "DOCSET_BUNDLE_ID",
    "DOCSET_PUBLISHER_ID", "DOCSET_PUBLISHER_NAME", "CHM_FILE", "HHC_LOCATION",
    "GENERATE_CHI", "CHM_INDEX_ENCODING", "BINARY_TOC", "TOC_EXPAND",
    "SITEMAP_URL", "GENERATE_QHP", "QCH_FILE", "QHP_NAMESPACE",
    "QHP_VIRTUAL_FOLDER", "QHP_CUST_FILTER_NAME", "QHP_CUST_FILTER_ATTRS",
    "QHP_SECT_FILTER_ATTRS", "QHG_LOCATION", "GENERATE_ECLIPSEHELP",
    "ECLIPSE_DOC_ID", "DISABLE_INDEX", "PAGE_OUTLINE_PANEL", "FULL_SIDEBAR",
    "ENUM_VALUES_PER_LINE", "SHOW_ENUM_VALUES", "TREEVIEW_WIDTH",
    "EXT_LINKS_IN_WINDOW", "OBFUSCATE_EMAILS", "HTML_FORMULA_FORMAT",
    "FORMULA_FONTSIZE", "FORMULA_MACROFILE", "USE_MATHJAX", "MATHJAX_VERSION",
    "MATHJAX_FORMAT", "MATHJAX_RELPATH", "MATHJAX_EXTENSIONS",
    "MATHJAX_CODEFILE", "SERVER_BASED_SEARCH", "EXTERNAL_SEARCH",
    "SEARCHENGINE_URL", "SEARCHDATA_FILE", "EXTERNAL_SEARCH_ID",
    "EXTRA_SEARCH_MAPPINGS", "LATEX_OUTPUT", "LATEX_CMD_NAME",
    "MAKEINDEX_CMD_NAME", "LATEX_MAKEINDEX_CMD", "COMPACT_LATEX", "PAPER_TYPE",
    "EXTRA_PACKAGES", "LATEX_HEADER", "LATEX_FOOTER", "LATEX_EXTRA_STYLESHEET",
    "LATEX_EXTRA_FILES", "LATEX_BATCHMODE", "LATEX_HIDE_INDICES",
    "LATEX_BIB_STYLE", "LATEX_EMOJI_DIRECTORY", "RTF_OUTPUT", "COMPACT_RTF",
    "RTF_HYPERLINKS", "RTF_STYLESHEET_FILE", "RTF_EXTENSIONS_FILE",
    "RTF_EXTRA_FILES", "MAN_OUTPUT", "MAN_EXTENSION", "MAN_SUBDIR", "MAN_LINKS",
    "XML_OUTPUT", "XML_PROGRAMLISTING", "XML_NS_MEMB_FILE_SCOPE",
    "DOCBOOK_OUTPUT", "GENERATE_AUTOGEN_DEF", "GENERATE_SQLITE3",
    "SQLITE3_OUTPUT", "SQLITE3_RECREATE_DB", "GENERATE_PERLMOD", "PERLMOD_LATEX",
    "PERLMOD_PRETTY", "PERLMOD_MAKEVAR_PREFIX", "ENABLE_PREPROCESSING",
    "MACRO_EXPANSION", "EXPAND_ONLY_PREDEF", "SEARCH_INCLUDES", "INCLUDE_PATH",
    "INCLUDE_FILE_PATTERNS", "PREDEFINED", "EXPAND_AS_DEFINED",
    "SKIP_FUNCTION_MACROS", "TAGFILES", "GENERATE_TAGFILE", "ALLEXTERNALS",
    "EXTERNAL_GROUPS", "EXTERNAL_PAGES", "HIDE_UNDOC_RELATIONS", "DOT_NUM_THREADS",
    "DOT_COMMON_ATTR", "DOT_EDGE_ATTR", "DOT_NODE_ATTR", "DOT_FONTPATH",
    "UML_LOOK", "UML_LIMIT_NUM_FIELDS", "UML_MAX_EDGE_LABELS",
    "PLANTUML_JAR_PATH", "PLANTUML_CFG_FILE", "PLANTUML_INCLUDE_PATH",
    "DOT_UML_DETAILS", "DOT_WRAP_THRESHOLD", "TEMPLATE_RELATIONS",
    "DIRECTORY_GRAPH", "DIR_GRAPH_MAX_DEPTH", "DOT_IMAGE_FORMAT",
    "INTERACTIVE_SVG", "DOT_PATH", "DOTFILE_DIRS", "PLANTUMLFILE_DIRS",
    "DOT_GRAPH_MAX_NODES", "MAX_DOT_GRAPH_DEPTH", "DOT_MULTI_TARGETS",
    "GENERATE_LEGEND", "DOT_CLEANUP", "MSCGEN_TOOL", "MSCFILE_DIRS",
    "DIA_PATH", "DIAFILE_DIRS", "MERMAID_PATH", "MERMAID_CONFIG_FILE",
    "MERMAID_RENDER_MODE", "MERMAID_JS_URL", "MERMAIDFILE_DIRS",
}

EXCLUDED_OPTIONS = {
    "INPUT",
    "SOURCE_BROWSER",
}

OPTION_DOCS_CONTEXT = "OptionDocs"


def parse_config_xml(config_path: str) -> Dict[str, str]:
    """Parse config.xml and extract option IDs and their default descriptions."""
    options = {}
    
    tree = ET.parse(config_path)
    root = tree.getroot()
    
    for option in root.iter('option'):
        option_type = option.get('type', '')
        if option_type == 'obsolete':
            continue
        
        option_id = option.get('id')
        if option_id:
            docs_elem = option.find('docs')
            if docs_elem is not None:
                docs_text = docs_elem.text or ''
                docs_text = docs_text.strip()
                if docs_text.startswith('<![CDATA['):
                    docs_text = docs_text[9:]
                if docs_text.endswith(']]>'):
                    docs_text = docs_text[:-3]
                docs_text = docs_text.strip()
            else:
                docs_text = ''
            options[option_id] = docs_text
    
    return options


def parse_option_translations(cpp_path: str) -> Dict[str, str]:
    """Parse optiontranslations.cpp and extract existing option translations."""
    options = {}
    
    with open(cpp_path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    pattern = r'\{\s*"([^"]+)"\s*,\s*TR_NOOP\s*\(\s*"([^"]+)"\s*\)\s*\}'
    matches = re.findall(pattern, content)
    
    for key, text in matches:
        options[key] = text
    
    return options


def parse_ts_docs_entries(ts_path: str) -> Tuple[Set[str], Dict[str, str]]:
    """Parse a .ts file and extract existing OptionDocs entries.
    
    Returns:
        Tuple of (set of option IDs that exist, dict of option_id -> translation status)
    """
    existing_ids = set()
    translation_status = {}
    
    try:
        tree = ET.parse(ts_path)
        root = tree.getroot()
        
        for context in root.findall('context'):
            name_elem = context.find('name')
            if name_elem is not None and name_elem.text == OPTION_DOCS_CONTEXT:
                for message in context.findall('message'):
                    source = message.find('source')
                    translation = message.find('translation')
                    if source is not None and source.text:
                        opt_id = source.text.strip()
                        existing_ids.add(opt_id)
                        
                        if translation is not None:
                            trans_type = translation.get('type', '')
                            trans_text = translation.text or ''
                            if trans_type == 'unfinished' or not trans_text:
                                translation_status[opt_id] = 'unfinished'
                            else:
                                translation_status[opt_id] = 'translated'
                        else:
                            translation_status[opt_id] = 'missing'
                break
    except ET.ParseError as e:
        print(f"  Warning: Error parsing {ts_path}: {e}")
    except Exception as e:
        print(f"  Warning: Could not read {ts_path}: {e}")
    
    return existing_ids, translation_status


def add_docs_entries_to_ts(ts_path: str, missing_ids: Set[str], config_options: Dict[str, str]) -> int:
    """Add missing documentation entries to a .ts file.
    
    Returns:
        Number of entries added
    """
    try:
        tree = ET.parse(ts_path)
        root = tree.getroot()
        
        option_docs_context = None
        for context in root.findall('context'):
            name_elem = context.find('name')
            if name_elem is not None and name_elem.text == OPTION_DOCS_CONTEXT:
                option_docs_context = context
                break
        
        if option_docs_context is None:
            option_docs_context = ET.SubElement(root, 'context')
            name_elem = ET.SubElement(option_docs_context, 'name')
            name_elem.text = OPTION_DOCS_CONTEXT
        
        added_count = 0
        for opt_id in sorted(missing_ids):
            docs = config_options.get(opt_id, '')
            if docs:
                message = ET.SubElement(option_docs_context, 'message')
                source = ET.SubElement(message, 'source')
                source.text = opt_id
                translation = ET.SubElement(message, 'translation')
                translation.set('type', 'unfinished')
                added_count += 1
        
        indent_ts_file(root)
        
        tree.write(ts_path, encoding='utf-8', xml_declaration=True)
        
        return added_count
        
    except Exception as e:
        print(f"  Error updating {ts_path}: {e}")
        return 0


def indent_ts_file(elem, level=0):
    """Add proper indentation to XML elements for .ts file format."""
    indent = "\n" + "    " * level
    if len(elem):
        if not elem.text or not elem.text.strip():
            elem.text = indent + "    "
        if not elem.tail or not elem.tail.strip():
            elem.tail = indent
        for child in elem:
            indent_ts_file(child, level + 1)
        if not child.tail or not child.tail.strip():
            child.tail = indent
    else:
        if level and (not elem.tail or not elem.tail.strip()):
            elem.tail = indent


def generate_option_description(option_id: str) -> str:
    """Generate a human-readable description from an option ID."""
    words = []
    current_word = ''
    
    for char in option_id:
        if char == '_':
            if current_word:
                words.append(current_word)
                current_word = ''
        elif char.isupper() and current_word and not current_word[-1].isupper():
            words.append(current_word)
            current_word = char
        else:
            current_word += char
    
    if current_word:
        words.append(current_word)
    
    return ' '.join(words).title()


def compare_options(config_options: Dict[str, str], 
                    cpp_options: Dict[str, str]) -> Tuple[Set[str], Set[str], Set[str]]:
    """Compare options from config.xml and optiontranslations.cpp."""
    config_ids = set(config_options.keys())
    cpp_ids = set(cpp_options.keys())
    
    missing_in_cpp = config_ids - cpp_ids
    extra_in_cpp = cpp_ids - config_ids
    common = config_ids & cpp_ids
    
    return missing_in_cpp, extra_in_cpp, common


def generate_cpp_entry(option_id: str, description: str, is_excluded: bool = False) -> str:
    """Generate a C++ entry for optiontranslations.cpp."""
    if is_excluded:
        return f'    // {option_id} is excluded: uses shared message system (DoxygenWizard::msg{option_id.title().replace("_", "")}Topic())'
    return f'    {{ "{option_id}", TR_NOOP("{description}") }},'


def generate_ts_docs_entry(option_id: str, docs: str) -> str:
    """Generate a .ts file entry for documentation translation."""
    escaped_docs = html.escape(docs)
    return f'''    <message>
        <source>{option_id}</source>
        <translation type="unfinished"></translation>
    </message>'''


def main():
    parser = argparse.ArgumentParser(description='Sync config options with optiontranslations.cpp')
    parser.add_argument('--config', default='../../src/config.xml',
                        help='Path to config.xml')
    parser.add_argument('--options', default='optiontranslations.cpp',
                        help='Path to optiontranslations.cpp')
    parser.add_argument('--update', action='store_true',
                        help='Update optiontranslations.cpp instead of just reporting')
    parser.add_argument('--docs', action='store_true',
                        help='Generate documentation translation entries for .ts files')
    parser.add_argument('--apply', action='store_true',
                        help='Actually modify .ts files (use with --docs)')
    parser.add_argument('--ts-dir', default='.',
                        help='Directory containing .ts files')
    parser.add_argument('--ts-file', default=None,
                        help='Specific .ts file to check (optional)')
    parser.add_argument('--verbose', '-v', action='store_true',
                        help='Verbose output')
    parser.add_argument('--limit', type=int, default=10,
                        help='Limit number of docs entries to show (default: 10)')
    
    args = parser.parse_args()
    
    script_dir = os.path.dirname(os.path.abspath(__file__))
    
    config_path = os.path.normpath(os.path.join(script_dir, args.config))
    cpp_path = os.path.normpath(os.path.join(script_dir, args.options))
    ts_dir = os.path.normpath(os.path.join(script_dir, args.ts_dir))
    
    print(f"Config XML: {config_path}")
    print(f"Option Translations: {cpp_path}")
    print()
    
    if not os.path.exists(config_path):
        print(f"Error: config.xml not found at {config_path}")
        return 1
    
    if not os.path.exists(cpp_path):
        print(f"Error: optiontranslations.cpp not found at {cpp_path}")
        return 1
    
    print("Parsing config.xml...")
    config_options = parse_config_xml(config_path)
    print(f"  Found {len(config_options)} options in config.xml")
    
    print("Parsing optiontranslations.cpp...")
    cpp_options = parse_option_translations(cpp_path)
    print(f"  Found {len(cpp_options)} translations in optiontranslations.cpp")
    
    missing_in_cpp, extra_in_cpp, common = compare_options(config_options, cpp_options)
    
    missing_wizard_options = missing_in_cpp & WIZARD_OPTIONS
    missing_non_wizard = missing_in_cpp - WIZARD_OPTIONS
    
    print()
    print("=" * 70)
    print("ANALYSIS RESULTS")
    print("=" * 70)
    
    print()
    print("EXCLUDED OPTIONS (use shared message system):")
    print("-" * 70)
    for opt_id in sorted(EXCLUDED_OPTIONS):
        reason = ""
        if opt_id == "INPUT":
            reason = "Uses DoxygenWizard::msgInputTopic() for translation"
        elif opt_id == "SOURCE_BROWSER":
            reason = "Uses DoxygenWizard::msgSourceBrowserTopic() for translation"
        print(f"  - {opt_id}: {reason}")
    
    if missing_wizard_options:
        print()
        print(f"MISSING IN optiontranslations.cpp (used in wizard): {len(missing_wizard_options)}")
        print("-" * 70)
        for opt_id in sorted(missing_wizard_options):
            if opt_id in EXCLUDED_OPTIONS:
                continue
            desc = generate_option_description(opt_id)
            config_desc = config_options.get(opt_id, '')
            print(f"  - {opt_id}")
            print(f"      Suggested: {desc}")
            if args.verbose and config_desc:
                print(f"      Config docs: {config_desc[:100]}...")
    
    if extra_in_cpp:
        print()
        print(f"EXTRA IN optiontranslations.cpp (not in config.xml): {len(extra_in_cpp)}")
        print("-" * 70)
        for opt_id in sorted(extra_in_cpp):
            print(f"  - {opt_id}: {cpp_options[opt_id]}")
    
    print()
    print(f"Common options: {len(common)}")
    print(f"Options in wizard interface: {len(WIZARD_OPTIONS)}")
    print(f"Excluded options: {len(EXCLUDED_OPTIONS)}")
    
    if args.docs:
        print()
        print("=" * 70)
        print("DOCUMENTATION TRANSLATION ANALYSIS")
        print("=" * 70)
        
        ts_files = []
        if args.ts_file:
            ts_path = os.path.join(ts_dir, args.ts_file)
            if os.path.exists(ts_path):
                ts_files = [args.ts_file]
            else:
                print(f"Error: .ts file not found: {ts_path}")
                return 1
        else:
            ts_files = [f for f in os.listdir(ts_dir) if f.endswith('.ts')]
        
        if not ts_files:
            print("No .ts files found in directory.")
            return 0
        
        all_option_ids = set(config_options.keys()) - EXCLUDED_OPTIONS
        
        for ts_file in ts_files:
            ts_path = os.path.join(ts_dir, ts_file)
            print()
            print(f"Analyzing: {ts_file}")
            print("-" * 70)
            
            existing_ids, translation_status = parse_ts_docs_entries(ts_path)
            
            missing_ids = all_option_ids - existing_ids
            translated_count = sum(1 for s in translation_status.values() if s == 'translated')
            unfinished_count = sum(1 for s in translation_status.values() if s == 'unfinished')
            
            print(f"  Total options with docs: {len(all_option_ids)}")
            print(f"  Existing OptionDocs entries: {len(existing_ids)}")
            print(f"    - Translated: {translated_count}")
            print(f"    - Unfinished: {unfinished_count}")
            print(f"  Missing entries: {len(missing_ids)}")
            
            if args.apply and missing_ids:
                print()
                print(f"  Applying changes to {ts_file}...")
                added = add_docs_entries_to_ts(ts_path, missing_ids, config_options)
                print(f"  Added {added} new entries to OptionDocs context.")
                print(f"  File saved: {ts_path}")
            elif missing_ids:
                print()
                print("  Entries to add to OptionDocs context:")
                print("  ----------------------------------------")
                print("  <context>")
                print(f"      <name>{OPTION_DOCS_CONTEXT}</name>")
                
                count = 0
                for opt_id in sorted(missing_ids):
                    docs = config_options.get(opt_id, '')
                    if docs:
                        count += 1
                        if count <= args.limit:
                            print(f"      {generate_ts_docs_entry(opt_id, docs)}")
                
                if count > args.limit:
                    print(f"      ... and {count - args.limit} more entries")
                
                print("  </context>")
                
                if args.verbose and count > args.limit:
                    print()
                    print("  All missing option IDs:")
                    for opt_id in sorted(missing_ids):
                        if config_options.get(opt_id):
                            print(f"    - {opt_id}")
                
                print()
                print("  TIP: Use --apply flag to automatically add these entries to the .ts file.")
            else:
                print()
                print("  All documentation entries are present. No missing entries.")
            
            print()
            print("  NOTE: Existing translations will NOT be overwritten.")
            print("  Only missing entries are added.")
    
    if args.update and missing_wizard_options:
        print()
        print("=" * 70)
        print("UPDATE MODE - Generating entries for optiontranslations.cpp")
        print("=" * 70)
        print()
        print("Add the following entries to s_optionTranslations[] array:")
        print()
        
        for opt_id in sorted(missing_wizard_options):
            if opt_id in EXCLUDED_OPTIONS:
                print(f"    // {opt_id} is excluded from this list.")
                if opt_id == "INPUT":
                    print(f"    // Reason: Uses shared message system via DoxygenWizard::msgInputTopic()")
                elif opt_id == "SOURCE_BROWSER":
                    print(f"    // Reason: Uses shared message system via DoxygenWizard::msgSourceBrowserTopic()")
                print()
            else:
                desc = generate_option_description(opt_id)
                print(f'    {{ "{opt_id}", TR_NOOP("{desc}") }},')
        
        print()
        print("NOTE: This script does NOT modify .ts translation files.")
        print("After updating optiontranslations.cpp, run lupdate to update .ts files:")
        print("  lupdate optiontranslations.cpp -ts doxywizard_zh_CN.ts ...")
    
    if missing_wizard_options or extra_in_cpp:
        return 1
    return 0


if __name__ == '__main__':
    exit(main())
