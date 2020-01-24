%option never-interactive
%option prefix="widlscannerYY"
%option reentrant
%option noyywrap


%{

#include <stdio.h>

#include "message.h"
#include "entry.h"
#include "doxygen.h"
#include "commentscan.h"
#include "types.h"

#include "widlscanner.h"
#include "widlcode.h"

#define YY_NO_INPUT 1
#define YY_NO_UNISTD_H 1

static int g_lineNo = 0;
static const char* g_fileName = nullptr;
static const char* g_inputString = nullptr;
static int g_inputPosition = 0;
static Entry* g_currentRoot = nullptr;
static std::string g_lastComment = "";
static CommentScanner g_commentScanner;
static WebIDLOutlineParser* g_widlParser = nullptr;

std::shared_ptr<Entry> createModuleEntry(const char*, const char*);
std::shared_ptr<Entry> createNamespaceEntry(const char*, const char*);

static int yyread(yyscan_t yyscanner, char *buf, int max_size);
#undef YY_INPUT
#define YY_INPUT(buf, result, max_size) result = yyread(yyscanner, buf, max_size);

%}

%x COMMENT
%x MODULE

IDENTIFIER [a-zA-Z_][a-zA-Z0-9_]+

%%

<INITIAL>{
    "/**" {
        BEGIN(COMMENT);
        g_lastComment = "";
        //g_lastComment = yytext;
    }

    module {
        BEGIN(MODULE);
    }
}

<COMMENT>{
    ^[ ]+"*/" {
        BEGIN(INITIAL);
        //g_lastComment += yytext;
    }

    ^[ ]+"*" {
        // ignore
    }

    .|\n {
        g_lastComment += yytext;
    }
}

<MODULE>{
    {IDENTIFIER} {
        auto entry = createNamespaceEntry(g_fileName, yytext);
        Doxygen::docGroup.initGroupInfo(entry.get());
        g_currentRoot->moveToSubEntryAndKeep(entry);
        g_currentRoot = entry.get();

        auto protection = Public;
        int position = 0;
        bool needsEntry = false;
        g_commentScanner.parseCommentBlock(
            g_widlParser,
            g_currentRoot,
            g_lastComment.c_str(),
            g_fileName,
            g_lineNo,
            false,
            false,
            false,
            protection,
            position,
            needsEntry);

        BEGIN(INITIAL);
    }

    .|\n {
        // ignore
    }
}

. {
    // ignore
}

\n {
    g_lineNo ++;
}

<<EOF>> {
    yyterminate();
}

%%

static int yyread(yyscan_t yyscanner, char* buf, int max_size) {
    struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
    int c = 0;
    const char *p = g_inputString + g_inputPosition;
    while (c < max_size && *p) {
        *buf++ = *p++;
        c++;
    }
    g_inputPosition += c;
    return c;
}


WebIDLOutlineParser::WebIDLOutlineParser()
{
}

WebIDLOutlineParser::~WebIDLOutlineParser()
{
}


std::shared_ptr<Entry> createNamespaceEntry(
    const char* fileName,
    const char* name) {
    std::shared_ptr<Entry> entry = std::make_shared<Entry>();
    entry->type = "namespace";
    entry->fileName = fileName;
    entry->name = name;
    entry->section = Entry::NAMESPACE_SEC;
    entry->lang = SrcLangExt_WebIDL;
    return entry;
}


void WebIDLOutlineParser::parseInput(
    const char *fileName,
    const char *fileBuf,
    const std::shared_ptr<Entry> &root,
    bool,
    QStrList&)
{
    yyscan_t scanner;
    widlscannerYYlex_init(&scanner);
    struct yyguts_t *yyg = (struct yyguts_t*)scanner;

    printlex(1, TRUE, __FILE__, fileName);

    fprintf(stderr, "[WIDL] parsing file %s\n", fileName);

    g_widlParser = this;
    g_fileName = fileName;
    g_currentRoot = root.get();

    g_inputPosition = 0;
    g_inputString = fileBuf;

    Doxygen::docGroup.enterFile(fileName, 0);
    widlscannerYYlex(scanner);
    root->markAsProcessed();
    root->printDebug();
    Doxygen::docGroup.leaveFile(fileName, g_lineNo);

    widlscannerYYlex_destroy(scanner);

    printlex(yy_flex_debug, FALSE, __FILE__, fileName);
}

bool WebIDLOutlineParser::needsPreprocessing(const QCString &) const
{
  return FALSE;
}

void WebIDLOutlineParser::parsePrototype(const char *text)
{
}


#include "widlscanner.l.h"
