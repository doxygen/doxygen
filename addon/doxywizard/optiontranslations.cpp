#include "optiontranslations.h"
#include "doxywizard.h"
#include <QCoreApplication>

#define TR_NOOP(x) QT_TR_NOOP(x)

static const struct {
    const char *key;
    const char *text;
} s_optionTranslations[] = {
    { "DOXYFILE_ENCODING", TR_NOOP("Doxyfile Encoding") },
    { "PROJECT_NAME", TR_NOOP("Project Name") },
    { "PROJECT_NUMBER", TR_NOOP("Project Number") },
    { "PROJECT_BRIEF", TR_NOOP("Project Brief") },
    { "PROJECT_LOGO", TR_NOOP("Project Logo") },
    { "PROJECT_ICON", TR_NOOP("Project Icon") },
    { "OUTPUT_DIRECTORY", TR_NOOP("Output Directory") },
    { "CREATE_SUBDIRS", TR_NOOP("Create Subdirectories") },
    { "CREATE_SUBDIRS_LEVEL", TR_NOOP("Create Subdirectories Level") },
    { "ALLOW_UNICODE_NAMES", TR_NOOP("Allow Unicode Names") },
    { "OUTPUT_LANGUAGE", TR_NOOP("Output Language") },
    
    { "BRIEF_MEMBER_DESC", TR_NOOP("Brief Member Description") },
    { "REPEAT_BRIEF", TR_NOOP("Repeat Brief") },
    { "ABBREVIATE_BRIEF", TR_NOOP("Abbreviate Brief") },
    { "ALWAYS_DETAILED_SEC", TR_NOOP("Always Detailed Section") },
    { "INLINE_INHERITED_MEMB", TR_NOOP("Inline Inherited Members") },
    { "FULL_PATH_NAMES", TR_NOOP("Full Path Names") },
    { "STRIP_FROM_PATH", TR_NOOP("Strip From Path") },
    { "STRIP_FROM_INC_PATH", TR_NOOP("Strip From Include Path") },
    { "SHORT_NAMES", TR_NOOP("Short Names") },
    { "JAVADOC_AUTOBRIEF", TR_NOOP("Javadoc Auto Brief") },
    { "JAVADOC_BANNER", TR_NOOP("Javadoc Banner") },
    { "QT_AUTOBRIEF", TR_NOOP("Qt Auto Brief") },
    { "MULTILINE_CPP_IS_BRIEF", TR_NOOP("Multiline CPP Is Brief") },
    { "PYTHON_DOCSTRING", TR_NOOP("Python Docstring") },
    { "INHERIT_DOCS", TR_NOOP("Inherit Docs") },
    { "SEPARATE_MEMBER_PAGES", TR_NOOP("Separate Member Pages") },
    { "TAB_SIZE", TR_NOOP("Tab Size") },
    { "ALIASES", TR_NOOP("Aliases") },
    
    { "OPTIMIZE_OUTPUT_FOR_C", TR_NOOP("Optimize Output For C") },
    { "OPTIMIZE_OUTPUT_JAVA", TR_NOOP("Optimize Output For Java") },
    { "OPTIMIZE_FOR_FORTRAN", TR_NOOP("Optimize For Fortran") },
    { "OPTIMIZE_OUTPUT_VHDL", TR_NOOP("Optimize Output For VHDL") },
    { "OPTIMIZE_OUTPUT_SLICE", TR_NOOP("Optimize Output For Slice") },
    { "EXTENSION_MAPPING", TR_NOOP("Extension Mapping") },
    
    { "MARKDOWN_SUPPORT", TR_NOOP("Markdown Support") },
    { "MARKDOWN_STRICT", TR_NOOP("Markdown Strict") },
    { "TOC_INCLUDE_HEADINGS", TR_NOOP("TOC Include Headings") },
    { "MARKDOWN_ID_STYLE", TR_NOOP("Markdown ID Style") },
    
    { "AUTOLINK_SUPPORT", TR_NOOP("Auto Link Support") },
    { "AUTOLINK_IGNORE_WORDS", TR_NOOP("Auto Link Ignore Words") },
    { "BUILTIN_STL_SUPPORT", TR_NOOP("Built-in STL Support") },
    { "CPP_CLI_SUPPORT", TR_NOOP("C++/CLI Support") },
    { "SIP_SUPPORT", TR_NOOP("SIP Support") },
    { "IDL_PROPERTY_SUPPORT", TR_NOOP("IDL Property Support") },
    { "DISTRIBUTE_GROUP_DOC", TR_NOOP("Distribute Group Doc") },
    { "GROUP_NESTED_COMPOUNDS", TR_NOOP("Group Nested Compounds") },
    { "SUBGROUPING", TR_NOOP("Subgrouping") },
    { "INLINE_GROUPED_CLASSES", TR_NOOP("Inline Grouped Classes") },
    { "INLINE_SIMPLE_STRUCTS", TR_NOOP("Inline Simple Structs") },
    { "TYPEDEF_HIDES_STRUCT", TR_NOOP("Typedef Hides Struct") },
    { "LOOKUP_CACHE_SIZE", TR_NOOP("Lookup Cache Size") },
    { "NUM_PROC_THREADS", TR_NOOP("Number of Processing Threads") },
    { "TIMESTAMP", TR_NOOP("Timestamp") },
    
    { "EXTRACT_ALL", TR_NOOP("Extract All") },
    { "EXTRACT_PRIVATE", TR_NOOP("Extract Private") },
    { "EXTRACT_PRIV_VIRTUAL", TR_NOOP("Extract Private Virtual") },
    { "EXTRACT_PACKAGE", TR_NOOP("Extract Package") },
    { "EXTRACT_STATIC", TR_NOOP("Extract Static") },
    { "EXTRACT_LOCAL_CLASSES", TR_NOOP("Extract Local Classes") },
    { "EXTRACT_LOCAL_METHODS", TR_NOOP("Extract Local Methods") },
    { "EXTRACT_ANON_NSPACES", TR_NOOP("Extract Anonymous Namespaces") },
    { "RESOLVE_UNNAMED_PARAMS", TR_NOOP("Resolve Unnamed Params") },
    
    { "HIDE_UNDOC_MEMBERS", TR_NOOP("Hide Undocumented Members") },
    { "HIDE_UNDOC_CLASSES", TR_NOOP("Hide Undocumented Classes") },
    { "HIDE_UNDOC_NAMESPACES", TR_NOOP("Hide Undocumented Namespaces") },
    { "HIDE_FRIEND_COMPOUNDS", TR_NOOP("Hide Friend Compounds") },
    { "HIDE_IN_BODY_DOCS", TR_NOOP("Hide In Body Docs") },
    { "INTERNAL_DOCS", TR_NOOP("Internal Docs") },
    { "CASE_SENSE_NAMES", TR_NOOP("Case Sensitive Names") },
    { "HIDE_SCOPE_NAMES", TR_NOOP("Hide Scope Names") },
    { "HIDE_COMPOUND_REFERENCE", TR_NOOP("Hide Compound Reference") },
    { "SHOW_HEADERFILE", TR_NOOP("Show Header File") },
    { "SHOW_INCLUDE_FILES", TR_NOOP("Show Include Files") },
    { "SHOW_GROUPED_MEMB_INC", TR_NOOP("Show Grouped Member Include") },
    { "FORCE_LOCAL_INCLUDES", TR_NOOP("Force Local Includes") },
    { "INLINE_INFO", TR_NOOP("Inline Info") },
    
    { "SORT_MEMBER_DOCS", TR_NOOP("Sort Member Docs") },
    { "SORT_BRIEF_DOCS", TR_NOOP("Sort Brief Docs") },
    { "SORT_MEMBERS_CTORS_1ST", TR_NOOP("Sort Members Ctors 1st") },
    { "SORT_GROUP_NAMES", TR_NOOP("Sort Group Names") },
    { "SORT_BY_SCOPE_NAME", TR_NOOP("Sort By Scope Name") },
    { "STRICT_PROTO_MATCHING", TR_NOOP("Strict Proto Matching") },
    
    { "GENERATE_TODOLIST", TR_NOOP("Generate Todo List") },
    { "GENERATE_TESTLIST", TR_NOOP("Generate Test List") },
    { "GENERATE_BUGLIST", TR_NOOP("Generate Bug List") },
    { "GENERATE_DEPRECATEDLIST", TR_NOOP("Generate Deprecated List") },
    { "GENERATE_REQUIREMENTS", TR_NOOP("Generate Requirements") },
    { "REQ_TRACEABILITY_INFO", TR_NOOP("Requirements Traceability Info") },
    { "ENABLED_SECTIONS", TR_NOOP("Enabled Sections") },
    { "MAX_INITIALIZER_LINES", TR_NOOP("Max Initializer Lines") },
    { "SHOW_USED_FILES", TR_NOOP("Show Used Files") },
    { "SHOW_FILES", TR_NOOP("Show Files") },
    { "SHOW_NAMESPACES", TR_NOOP("Show Namespaces") },
    { "FILE_VERSION_FILTER", TR_NOOP("File Version Filter") },
    { "LAYOUT_FILE", TR_NOOP("Layout File") },
    { "CITE_BIB_FILES", TR_NOOP("Cite Bib Files") },
    { "EXTERNAL_TOOL_PATH", TR_NOOP("External Tool Path") },
    
    { "QUIET", TR_NOOP("Quiet") },
    { "WARNINGS", TR_NOOP("Warnings") },
    { "WARN_IF_UNDOCUMENTED", TR_NOOP("Warn If Undocumented") },
    { "WARN_IF_DOC_ERROR", TR_NOOP("Warn If Doc Error") },
    { "WARN_IF_INCOMPLETE_DOC", TR_NOOP("Warn If Incomplete Doc") },
    { "WARN_NO_PARAMDOC", TR_NOOP("Warn No Param Doc") },
    { "WARN_IF_UNDOC_ENUM_VAL", TR_NOOP("Warn If Undoc Enum Val") },
    { "WARN_LAYOUT_FILE", TR_NOOP("Warn Layout File") },
    { "WARN_AS_ERROR", TR_NOOP("Warn As Error") },
    { "WARN_FORMAT", TR_NOOP("Warn Format") },
    { "WARN_LINE_FORMAT", TR_NOOP("Warn Line Format") },
    { "WARN_LOGFILE", TR_NOOP("Warn Logfile") },
    
    { "INPUT", TR_NOOP("Input") },
    { "INPUT_ENCODING", TR_NOOP("Input Encoding") },
    { "INPUT_FILE_ENCODING", TR_NOOP("Input File Encoding") },
    { "FILE_PATTERNS", TR_NOOP("File Patterns") },
    { "RECURSIVE", TR_NOOP("Recursive") },
    { "EXCLUDE", TR_NOOP("Exclude") },
    { "EXCLUDE_SYMLINKS", TR_NOOP("Exclude Symlinks") },
    { "EXCLUDE_PATTERNS", TR_NOOP("Exclude Patterns") },
    { "EXCLUDE_SYMBOLS", TR_NOOP("Exclude Symbols") },
    { "EXAMPLE_PATH", TR_NOOP("Example Path") },
    { "EXAMPLE_PATTERNS", TR_NOOP("Example Patterns") },
    { "EXAMPLE_RECURSIVE", TR_NOOP("Example Recursive") },
    { "IMAGE_PATH", TR_NOOP("Image Path") },
    { "INPUT_FILTER", TR_NOOP("Input Filter") },
    { "FILTER_PATTERNS", TR_NOOP("Filter Patterns") },
    { "FILTER_SOURCE_FILES", TR_NOOP("Filter Source Files") },
    { "FILTER_SOURCE_PATTERNS", TR_NOOP("Filter Source Patterns") },
    { "USE_MDFILE_AS_MAINPAGE", TR_NOOP("Use MD File As Mainpage") },
    { "IMPLICIT_DIR_DOCS", TR_NOOP("Implicit Dir Docs") },
    { "FORTRAN_COMMENT_AFTER", TR_NOOP("Fortran Comment After") },
    
    { "SOURCE_BROWSER", TR_NOOP("Source Browser") },
    { "INLINE_SOURCES", TR_NOOP("Inline Sources") },
    { "STRIP_CODE_COMMENTS", TR_NOOP("Strip Code Comments") },
    { "REFERENCED_BY_RELATION", TR_NOOP("Referenced By Relation") },
    { "REFERENCES_RELATION", TR_NOOP("References Relation") },
    { "REFERENCES_LINK_SOURCE", TR_NOOP("References Link Source") },
    { "SOURCE_TOOLTIPS", TR_NOOP("Source Tooltips") },
    { "USE_HTAGS", TR_NOOP("Use HTAGS") },
    { "VERBATIM_HEADERS", TR_NOOP("Verbatim Headers") },
    
    { "CLANG_ASSISTED_PARSING", TR_NOOP("Clang Assisted Parsing") },
    { "CLANG_ADD_INC_PATHS", TR_NOOP("Clang Add Include Paths") },
    { "CLANG_OPTIONS", TR_NOOP("Clang Options") },
    { "CLANG_DATABASE_PATH", TR_NOOP("Clang Database Path") },
    
    { "ALPHABETICAL_INDEX", TR_NOOP("Alphabetical Index") },
    { "IGNORE_PREFIX", TR_NOOP("Ignore Prefix") },
    
    { "GENERATE_HTML", TR_NOOP("Generate HTML") },
    { "HTML_OUTPUT", TR_NOOP("HTML Output") },
    { "HTML_FILE_EXTENSION", TR_NOOP("HTML File Extension") },
    { "HTML_HEADER", TR_NOOP("HTML Header") },
    { "HTML_FOOTER", TR_NOOP("HTML Footer") },
    { "HTML_STYLESHEET", TR_NOOP("HTML Stylesheet") },
    { "HTML_EXTRA_STYLESHEET", TR_NOOP("HTML Extra Stylesheet") },
    { "HTML_EXTRA_FILES", TR_NOOP("HTML Extra Files") },
    { "HTML_COLORSTYLE", TR_NOOP("HTML Color Style") },
    { "HTML_COLORSTYLE_HUE", TR_NOOP("HTML Color Style Hue") },
    { "HTML_COLORSTYLE_SAT", TR_NOOP("HTML Color Style Saturation") },
    { "HTML_COLORSTYLE_GAMMA", TR_NOOP("HTML Color Style Gamma") },
    { "HTML_DYNAMIC_MENUS", TR_NOOP("HTML Dynamic Menus") },
    { "HTML_DYNAMIC_SECTIONS", TR_NOOP("HTML Dynamic Sections") },
    { "HTML_CODE_FOLDING", TR_NOOP("HTML Code Folding") },
    { "HTML_COPY_CLIPBOARD", TR_NOOP("HTML Copy Clipboard") },
    { "HTML_PROJECT_COOKIE", TR_NOOP("HTML Project Cookie") },
    { "HTML_INDEX_NUM_ENTRIES", TR_NOOP("HTML Index Num Entries") },
    { "GENERATE_DOCSET", TR_NOOP("Generate DocSet") },
    { "DOCSET_FEEDNAME", TR_NOOP("DocSet Feed Name") },
    { "DOCSET_FEEDURL", TR_NOOP("DocSet Feed URL") },
    { "DOCSET_BUNDLE_ID", TR_NOOP("DocSet Bundle ID") },
    { "DOCSET_PUBLISHER_ID", TR_NOOP("DocSet Publisher ID") },
    { "DOCSET_PUBLISHER_NAME", TR_NOOP("DocSet Publisher Name") },
    { "GENERATE_HTMLHELP", TR_NOOP("Generate HTML Help") },
    { "CHM_FILE", TR_NOOP("CHM File") },
    { "HHC_LOCATION", TR_NOOP("HHC Location") },
    { "GENERATE_CHI", TR_NOOP("Generate CHI") },
    { "CHM_INDEX_ENCODING", TR_NOOP("CHM Index Encoding") },
    { "BINARY_TOC", TR_NOOP("Binary TOC") },
    { "TOC_EXPAND", TR_NOOP("TOC Expand") },
    { "SITEMAP_URL", TR_NOOP("Sitemap URL") },
    { "GENERATE_QHP", TR_NOOP("Generate QHP") },
    { "QCH_FILE", TR_NOOP("QCH File") },
    { "QHP_NAMESPACE", TR_NOOP("QHP Namespace") },
    { "QHP_VIRTUAL_FOLDER", TR_NOOP("QHP Virtual Folder") },
    { "QHP_CUST_FILTER_NAME", TR_NOOP("QHP Custom Filter Name") },
    { "QHP_CUST_FILTER_ATTRS", TR_NOOP("QHP Custom Filter Attrs") },
    { "QHP_SECT_FILTER_ATTRS", TR_NOOP("QHP Section Filter Attrs") },
    { "QHG_LOCATION", TR_NOOP("QHG Location") },
    { "GENERATE_ECLIPSEHELP", TR_NOOP("Generate Eclipse Help") },
    { "ECLIPSE_DOC_ID", TR_NOOP("Eclipse Doc ID") },
    { "DISABLE_INDEX", TR_NOOP("Disable Index") },
    { "GENERATE_TREEVIEW", TR_NOOP("Generate Treeview") },
    { "PAGE_OUTLINE_PANEL", TR_NOOP("Page Outline Panel") },
    { "FULL_SIDEBAR", TR_NOOP("Full Sidebar") },
    { "ENUM_VALUES_PER_LINE", TR_NOOP("Enum Values Per Line") },
    { "SHOW_ENUM_VALUES", TR_NOOP("Show Enum Values") },
    { "TREEVIEW_WIDTH", TR_NOOP("Treeview Width") },
    { "EXT_LINKS_IN_WINDOW", TR_NOOP("Ext Links In Window") },
    { "OBFUSCATE_EMAILS", TR_NOOP("Obfuscate Emails") },
    { "HTML_FORMULA_FORMAT", TR_NOOP("HTML Formula Format") },
    { "FORMULA_FONTSIZE", TR_NOOP("Formula Font Size") },
    { "FORMULA_MACROFILE", TR_NOOP("Formula Macro File") },
    { "USE_MATHJAX", TR_NOOP("Use MathJax") },
    { "MATHJAX_VERSION", TR_NOOP("MathJax Version") },
    { "MATHJAX_FORMAT", TR_NOOP("MathJax Format") },
    { "MATHJAX_RELPATH", TR_NOOP("MathJax Rel Path") },
    { "MATHJAX_EXTENSIONS", TR_NOOP("MathJax Extensions") },
    { "MATHJAX_CODEFILE", TR_NOOP("MathJax Code File") },
    { "SEARCHENGINE", TR_NOOP("Search Engine") },
    { "SERVER_BASED_SEARCH", TR_NOOP("Server Based Search") },
    { "EXTERNAL_SEARCH", TR_NOOP("External Search") },
    { "SEARCHENGINE_URL", TR_NOOP("Search Engine URL") },
    { "SEARCHDATA_FILE", TR_NOOP("Search Data File") },
    { "EXTERNAL_SEARCH_ID", TR_NOOP("External Search ID") },
    { "EXTRA_SEARCH_MAPPINGS", TR_NOOP("Extra Search Mappings") },
    
    { "GENERATE_LATEX", TR_NOOP("Generate LaTeX") },
    { "LATEX_OUTPUT", TR_NOOP("LaTeX Output") },
    { "LATEX_CMD_NAME", TR_NOOP("LaTeX Command Name") },
    { "MAKEINDEX_CMD_NAME", TR_NOOP("MakeIndex Command Name") },
    { "LATEX_MAKEINDEX_CMD", TR_NOOP("LaTeX MakeIndex Command") },
    { "COMPACT_LATEX", TR_NOOP("Compact LaTeX") },
    { "PAPER_TYPE", TR_NOOP("Paper Type") },
    { "EXTRA_PACKAGES", TR_NOOP("Extra Packages") },
    { "LATEX_HEADER", TR_NOOP("LaTeX Header") },
    { "LATEX_FOOTER", TR_NOOP("LaTeX Footer") },
    { "LATEX_EXTRA_STYLESHEET", TR_NOOP("LaTeX Extra Stylesheet") },
    { "LATEX_EXTRA_FILES", TR_NOOP("LaTeX Extra Files") },
    { "PDF_HYPERLINKS", TR_NOOP("PDF Hyperlinks") },
    { "USE_PDFLATEX", TR_NOOP("Use PDFLaTeX") },
    { "LATEX_BATCHMODE", TR_NOOP("LaTeX Batch Mode") },
    { "LATEX_HIDE_INDICES", TR_NOOP("LaTeX Hide Indices") },
    { "LATEX_BIB_STYLE", TR_NOOP("LaTeX Bib Style") },
    { "LATEX_EMOJI_DIRECTORY", TR_NOOP("LaTeX Emoji Directory") },
    
    { "GENERATE_RTF", TR_NOOP("Generate RTF") },
    { "RTF_OUTPUT", TR_NOOP("RTF Output") },
    { "COMPACT_RTF", TR_NOOP("Compact RTF") },
    { "RTF_HYPERLINKS", TR_NOOP("RTF Hyperlinks") },
    { "RTF_STYLESHEET_FILE", TR_NOOP("RTF Stylesheet File") },
    { "RTF_EXTENSIONS_FILE", TR_NOOP("RTF Extensions File") },
    { "RTF_EXTRA_FILES", TR_NOOP("RTF Extra Files") },
    
    { "GENERATE_MAN", TR_NOOP("Generate Man") },
    { "MAN_OUTPUT", TR_NOOP("Man Output") },
    { "MAN_EXTENSION", TR_NOOP("Man Extension") },
    { "MAN_SUBDIR", TR_NOOP("Man Subdir") },
    { "MAN_LINKS", TR_NOOP("Man Links") },
    
    { "GENERATE_XML", TR_NOOP("Generate XML") },
    { "XML_OUTPUT", TR_NOOP("XML Output") },
    { "XML_PROGRAMLISTING", TR_NOOP("XML Program Listing") },
    { "XML_NS_MEMB_FILE_SCOPE", TR_NOOP("XML NS Memb File Scope") },
    
    { "GENERATE_DOCBOOK", TR_NOOP("Generate DocBook") },
    { "DOCBOOK_OUTPUT", TR_NOOP("DocBook Output") },
    
    { "GENERATE_AUTOGEN_DEF", TR_NOOP("Generate Autogen Def") },
    { "GENERATE_SQLITE3", TR_NOOP("Generate SQLite3") },
    { "SQLITE3_OUTPUT", TR_NOOP("SQLite3 Output") },
    { "SQLITE3_RECREATE_DB", TR_NOOP("SQLite3 Recreate DB") },
    
    { "GENERATE_PERLMOD", TR_NOOP("Generate Perl Mod") },
    { "PERLMOD_LATEX", TR_NOOP("Perl Mod LaTeX") },
    { "PERLMOD_PRETTY", TR_NOOP("Perl Mod Pretty") },
    { "PERLMOD_MAKEVAR_PREFIX", TR_NOOP("Perl Mod Makevar Prefix") },
    
    { "ENABLE_PREPROCESSING", TR_NOOP("Enable Preprocessing") },
    { "MACRO_EXPANSION", TR_NOOP("Macro Expansion") },
    { "EXPAND_ONLY_PREDEF", TR_NOOP("Expand Only Predef") },
    { "SEARCH_INCLUDES", TR_NOOP("Search Includes") },
    { "INCLUDE_PATH", TR_NOOP("Include Path") },
    { "INCLUDE_FILE_PATTERNS", TR_NOOP("Include File Patterns") },
    { "PREDEFINED", TR_NOOP("Predefined") },
    { "EXPAND_AS_DEFINED", TR_NOOP("Expand As Defined") },
    { "SKIP_FUNCTION_MACROS", TR_NOOP("Skip Function Macros") },
    
    { "TAGFILES", TR_NOOP("Tag Files") },
    { "GENERATE_TAGFILE", TR_NOOP("Generate Tag File") },
    { "ALLEXTERNALS", TR_NOOP("All Externals") },
    { "EXTERNAL_GROUPS", TR_NOOP("External Groups") },
    { "EXTERNAL_PAGES", TR_NOOP("External Pages") },
    
    { "HIDE_UNDOC_RELATIONS", TR_NOOP("Hide Undoc Relations") },
    { "HAVE_DOT", TR_NOOP("Have Dot") },
    { "DOT_NUM_THREADS", TR_NOOP("Dot Num Threads") },
    { "DOT_COMMON_ATTR", TR_NOOP("Dot Common Attr") },
    { "DOT_EDGE_ATTR", TR_NOOP("Dot Edge Attr") },
    { "DOT_NODE_ATTR", TR_NOOP("Dot Node Attr") },
    { "DOT_FONTPATH", TR_NOOP("Dot Font Path") },
    { "CLASS_GRAPH", TR_NOOP("Class Graph") },
    { "COLLABORATION_GRAPH", TR_NOOP("Collaboration Graph") },
    
    { "UML_LOOK", TR_NOOP("UML Look") },
    { "UML_LIMIT_NUM_FIELDS", TR_NOOP("UML Limit Num Fields") },
    { "UML_MAX_EDGE_LABELS", TR_NOOP("UML Max Edge Labels") },
    
    { "PLANTUML_JAR_PATH", TR_NOOP("PlantUML Jar Path") },
    { "PLANTUML_CFG_FILE", TR_NOOP("PlantUML Config File") },
    { "PLANTUML_INCLUDE_PATH", TR_NOOP("PlantUML Include Path") },
    
    { "DOT_UML_DETAILS", TR_NOOP("Dot UML Details") },
    { "DOT_WRAP_THRESHOLD", TR_NOOP("Dot Wrap Threshold") },
    { "TEMPLATE_RELATIONS", TR_NOOP("Template Relations") },
    { "INCLUDE_GRAPH", TR_NOOP("Include Graph") },
    { "INCLUDED_BY_GRAPH", TR_NOOP("Included By Graph") },
    { "CALL_GRAPH", TR_NOOP("Call Graph") },
    { "CALLER_GRAPH", TR_NOOP("Caller Graph") },
    { "GRAPHICAL_HIERARCHY", TR_NOOP("Graphical Hierarchy") },
    { "DIRECTORY_GRAPH", TR_NOOP("Directory Graph") },
    { "DIR_GRAPH_MAX_DEPTH", TR_NOOP("Dir Graph Max Depth") },
    { "DOT_IMAGE_FORMAT", TR_NOOP("Dot Image Format") },
    { "INTERACTIVE_SVG", TR_NOOP("Interactive SVG") },
    { "DOT_PATH", TR_NOOP("Dot Path") },
    { "DOTFILE_DIRS", TR_NOOP("Dot File Dirs") },
    { "PLANTUMLFILE_DIRS", TR_NOOP("PlantUML File Dirs") },
    { "DOT_GRAPH_MAX_NODES", TR_NOOP("Dot Graph Max Nodes") },
    { "MAX_DOT_GRAPH_DEPTH", TR_NOOP("Max Dot Graph Depth") },
    { "DOT_MULTI_TARGETS", TR_NOOP("Dot Multi Targets") },
    { "GENERATE_LEGEND", TR_NOOP("Generate Legend") },
    { "DOT_CLEANUP", TR_NOOP("Dot Cleanup") },
    { "MSCGEN_TOOL", TR_NOOP("MSCGEN Tool") },
    { "MSCFILE_DIRS", TR_NOOP("MSC File Dirs") },
    { "DIA_PATH", TR_NOOP("DIA Path") },
    { "DIAFILE_DIRS", TR_NOOP("DIA File Dirs") },
    
    { "MERMAID_PATH", TR_NOOP("Mermaid Path") },
    { "MERMAID_CONFIG_FILE", TR_NOOP("Mermaid Config File") },
    { "MERMAID_RENDER_MODE", TR_NOOP("Mermaid Render Mode") },
    { "MERMAID_JS_URL", TR_NOOP("Mermaid JS URL") },
    { "MERMAIDFILE_DIRS", TR_NOOP("Mermaid File Dirs") },
    
    { nullptr, nullptr }
};

OptionTranslations& OptionTranslations::instance()
{
    static OptionTranslations inst;
    return inst;
}

void OptionTranslations::initTranslations()
{
    if (m_initialized) return;
    
    for (int i = 0; s_optionTranslations[i].key != nullptr; ++i)
    {
        QString key = QString::fromLatin1(s_optionTranslations[i].key);
        QString translation;
        if (key == QLatin1String("INPUT"))
        {
            translation = DoxygenWizard::msgInputTopic();
        }
        else if (key == QLatin1String("SOURCE_BROWSER"))
        {
            translation = DoxygenWizard::msgSourceBrowserTopic();
        }
        else
        {
            translation = tr(s_optionTranslations[i].text);
        }
        m_translations.insert(key, translation);
    }
    m_initialized = true;
}

QString OptionTranslations::translate(const QString &optionName)
{
    if (!m_initialized)
    {
        initTranslations();
    }
    
    if (m_translations.contains(optionName))
    {
        return m_translations[optionName];
    }
    return optionName;
}

QString OptionTranslations::trStatic(const QString &optionName)
{
    return instance().translate(optionName);
}

void OptionTranslations::retranslate()
{
    m_translations.clear();
    m_initialized = false;
    initTranslations();
    m_docsTranslations.clear();
    m_docsInitialized = false;
}

void OptionTranslations::initDocsTranslations()
{
    if (m_docsInitialized) return;
    m_docsInitialized = true;
}

QString OptionTranslations::translateDocs(const QString &optionName, const QString &defaultDocs)
{
    Q_UNUSED(defaultDocs);
    if (!m_docsInitialized)
    {
        initDocsTranslations();
    }
    
    QString translated = QCoreApplication::translate("OptionDocs", optionName.toUtf8().constData());
    if (!translated.isEmpty() && translated != optionName)
    {
        return translated;
    }
    
    return defaultDocs;
}

QString OptionTranslations::trDocsStatic(const QString &optionName, const QString &defaultDocs)
{
    return instance().translateDocs(optionName, defaultDocs);
}
