%option never-interactive
%option prefix="widlscannerYY"
%option reentrant
%option noyywrap
%option extra-type="struct widlscannerYY_state *"


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

struct widlscannerYY_state
{
    WebIDLOutlineParser* thisParser;
    CommentScanner commentScanner;

    const char* fileName;
    int lineNumber;

    const char* inputString;
    int inputPosition;

    std::shared_ptr<Entry> currentRoot;
    std::string lastComment;
};

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
        yyextra->lastComment = "";
        //yyextra->lastComment = yytext;
    }

    module {
        BEGIN(MODULE);
    }
}

<COMMENT>{
    ^[ ]+"*/" {
        BEGIN(INITIAL);
        //yyextra->lastComment += yytext;
    }

    ^[ ]+"*" {
        // ignore
    }

    .|\n {
        yyextra->lastComment += yytext;
    }
}

<MODULE>{
    {IDENTIFIER} {
        auto entry = createNamespaceEntry(yyextra->fileName, yytext);
        Doxygen::docGroup.initGroupInfo(entry.get());
        yyextra->currentRoot->moveToSubEntryAndKeep(entry);
        yyextra->currentRoot = entry;

        auto protection = Public;
        int position = 0;
        bool needsEntry = false;
        yyextra->commentScanner.parseCommentBlock(
            yyextra->thisParser,
            yyextra->currentRoot.get(),
            yyextra->lastComment.c_str(),
            yyextra->fileName,
            yyextra->lineNumber,
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
    yyextra->lineNumber ++;
}

<<EOF>> {
    yyterminate();
}

%%

static int yyread(yyscan_t yyscanner, char* buf, int max_size) {
    struct yyguts_t *yyg = (struct yyguts_t*)yyscanner;
    int c = 0;
    const char *p = yyextra->inputString + yyextra->inputPosition;
    while (c < max_size && *p) {
        *buf++ = *p++;
        c++;
    }
    yyextra->inputPosition += c;
    return c;
}

struct WebIDLOutlineParser::Private
{
    yyscan_t yyscanner;
    widlscannerYY_state state;
};

WebIDLOutlineParser::WebIDLOutlineParser()
    : p(std::make_unique<WebIDLOutlineParser::Private>())
{
    widlscannerYYlex_init_extra(&p->state, &p->yyscanner);
#ifdef FLEX_DEBUG
    widlscannerYYset_debug(1, p->yyscanner);
#endif
}

WebIDLOutlineParser::~WebIDLOutlineParser()
{
    widlscannerYYlex_destroy(p->yyscanner);
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
    struct yyguts_t *yyg = (struct yyguts_t*)p->yyscanner;

    printlex(1, TRUE, __FILE__, fileName);

    fprintf(stderr, "[WIDL] parsing file %s\n", fileName);

    yyextra->thisParser = this;
    yyextra->fileName = fileName;
    yyextra->currentRoot = root;

    yyextra->inputPosition = 0;
    yyextra->inputString = fileBuf;

    Doxygen::docGroup.enterFile(fileName, 0);
    widlscannerYYlex(p->yyscanner);
    root->markAsProcessed();
    root->printDebug();
    Doxygen::docGroup.leaveFile(fileName, yyextra->lineNumber);

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
