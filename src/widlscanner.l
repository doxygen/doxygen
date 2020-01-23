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

%}



%%

. {
    // ignore
}

\n {
    g_lineNo ++;
}

%%


WebIDLOutlineParser::WebIDLOutlineParser()
{
}

WebIDLOutlineParser::~WebIDLOutlineParser()
{
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

    // test entry
    std::shared_ptr<Entry> entry = std::make_shared<Entry>();
    entry->type = "namespace";
    entry->fileName = fileName;
    entry->name = "HelloWorld";
    entry->section = Entry::NAMESPACE_SEC;
    entry->lang = SrcLangExt_WebIDL;

    Doxygen::docGroup.initGroupInfo(entry.get());

    root->moveToSubEntryAndKeep(entry);
    root->markAsProcessed();

    root->printDebug();
    Doxygen::docGroup.enterFile(fileName, 0);
    widlscannerYYlex(scanner);
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
  fprintf(stderr, "[WIDL] WebIDLOutlineParser::parsePrototype()\n");
  fprintf(stderr, "[WIDL8] %s\n", text);
}


#include "widlscanner.l.h"
