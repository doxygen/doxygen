#include "clangparser.h"
#include "settings.h"
#include <stdio.h>
#include <mutex>

#if USE_LIBCLANG
#include <clang-c/Index.h>
#include "clang/Tooling/CompilationDatabase.h"
#include "clang/Tooling/Tooling.h"
#include <stdlib.h>
#include "message.h"
#include "outputgen.h"
#include "filedef.h"
#include "memberdef.h"
#include "doxygen.h"
#include "util.h"
#include "config.h"
#include "growbuf.h"
#include "membername.h"
#include "filename.h"
#include "tooltip.h"
#include "utf8.h"
#endif

//--------------------------------------------------------------------------

static std::mutex g_clangMutex;
static bool includeCodeFragment = false;

ClangParser *ClangParser::instance()
{
  std::lock_guard<std::mutex> lock(g_clangMutex);
  if (!s_instance) s_instance = new ClangParser;
  return s_instance;
}

ClangParser *ClangParser::s_instance = 0;

//--------------------------------------------------------------------------
#if USE_LIBCLANG

static std::mutex g_docCrossReferenceMutex;

enum class DetectedLang { Cpp, ObjC, ObjCpp };

static QCString detab(const QCString &s)
{
  static int tabSize = Config_getInt(TAB_SIZE);
  GrowBuf out;
  int size = s.length();
  const char *data = s.data();
  int i=0;
  int col=0;
  const int maxIndent=1000000; // value representing infinity
  int minIndent=maxIndent;
  while (i<size)
  {
    char c = data[i++];
    switch(c)
    {
      case '\t': // expand tab
        {
          int stop = tabSize - (col%tabSize);
          //printf("expand at %d stop=%d\n",col,stop);
          col+=stop;
          while (stop--) out.addChar(' ');
        }
        break;
      case '\n': // reset column counter
        out.addChar(c);
        col=0;
        break;
      case ' ': // increment column counter
        out.addChar(c);
        col++;
        break;
      default: // non-whitespace => update minIndent
        {
          int bytes = getUTF8CharNumBytes(c);
          for (int j=0;j<bytes-1 && c!=0; j++)
          {
            out.addChar(c);
            c = data[i++];
          }
          out.addChar(c);
        }
        if (col<minIndent) minIndent=col;
        col++;
    }
  }
  out.addChar(0);
  //printf("detab refIndent=%d\n",refIndent);
  return out.get();
}

static const char * keywordToType(const char *keyword)
{
  static const StringUnorderedSet flowKeywords({
    "break", "case", "catch", "continue", "default", "do",
    "else", "finally", "for", "foreach", "for each", "goto",
    "if", "return", "switch", "throw", "throws", "try",
    "while", "@try", "@catch", "@finally" });
  static const StringUnorderedSet typeKeywords({
    "bool", "char", "double", "float", "int", "long", "object",
    "short", "signed", "unsigned", "void", "wchar_t", "size_t",
    "boolean", "id", "SEL", "string", "nullptr" });
  if (flowKeywords.find(keyword)!=flowKeywords.end()) return "keywordflow";
  if (typeKeywords.find(keyword)!=typeKeywords.end()) return "keywordtype";
  return "keyword";
}


//--------------------------------------------------------------------------

class ClangTUParser::Private
{
  public:
    Private(const ClangParser &p,const FileDef *fd)
      : parser(p), fileDef(fd) {}
    const ClangParser &parser;
    const FileDef *fileDef;
    CXIndex index = 0;
    uint curToken = 0;
    DetectedLang detectedLang = DetectedLang::Cpp;
    uint numFiles = 0;
    std::vector<QCString> sources;
    std::vector<CXUnsavedFile> ufs;
    std::vector<CXCursor> cursors;
    std::unordered_map<std::string,uint> fileMapping;
    CXTranslationUnit tu = 0;
    CXToken *tokens = 0;
    uint numTokens = 0;
    StringVector filesInSameTU;
    TooltipManager tooltipManager;

    // state while parsing sources
    const MemberDef  *currentMemberDef=0;
    uint        currentLine=0;
    bool        searchForBody=FALSE;
    bool        insideBody=FALSE;
    uint        bracketCount=0;
};

ClangTUParser::ClangTUParser(const ClangParser &parser,const FileDef *fd)
  : p(std::make_unique<Private>(parser,fd))
{
  //printf("ClangTUParser::ClangTUParser() this=%p\n",this);
}

StringVector ClangTUParser::filesInSameTU() const
{
  return p->filesInSameTU;
}

void ClangTUParser::parse()
{
  //printf("ClangTUParser::parse() this=%p\n",this);
  QCString fileName = p->fileDef->absFilePath();
  p->fileDef->getAllIncludeFilesRecursively(p->filesInSameTU);
  //printf("ClangTUParser::ClangTUParser(fileName=%s,#filesInSameTU=%d)\n",
  //    qPrint(fileName),(int)p->filesInSameTU.size());
  bool clangAssistedParsing = Config_getBool(CLANG_ASSISTED_PARSING);
  bool clangIncludeInputPaths = Config_getBool(CLANG_ADD_INC_PATHS);
  bool filterSourceFiles = Config_getBool(FILTER_SOURCE_FILES);
  const StringVector &includePath = Config_getList(INCLUDE_PATH);
  const StringVector &clangOptions = Config_getList(CLANG_OPTIONS);
  if (!clangAssistedParsing) return;
  //printf("ClangParser::start(%s)\n",fileName);
  assert(p->index==0);
  assert(p->tokens==0);
  assert(p->numTokens==0);
  p->index    = clang_createIndex(0, 0);
  p->curToken = 0;
  p->cursors.clear();
  int argc=0;
  size_t clang_option_len = 0;
  std::vector<clang::tooling::CompileCommand> command;
  if (p->parser.database()!=nullptr)
  {
    // check if the file we are parsing is in the DB
    command = p->parser.database()->getCompileCommands(fileName.data());
    if (!command.empty() )
    {
      // it's possible to have multiple entries for the same file, so use the last entry
      clang_option_len = command[command.size()-1].CommandLine.size();
    }
  }
  char **argv = (char**)malloc(sizeof(char*)*
                               (4+Doxygen::inputPaths.size()+
                                includePath.size()+
                                clangOptions.size()+
                                clang_option_len));
  if (!command.empty() )
  {
    std::vector<std::string> options = command[command.size()-1].CommandLine;
    // copy each compiler option used from the database. Skip the first which is compiler exe.
    for (auto option = options.begin()+1; option != options.end(); option++)
    {
      argv[argc++] = qstrdup(option->c_str());
    }
    // user specified options
    for (size_t i=0;i<clangOptions.size();i++)
    {
      argv[argc++]=qstrdup(clangOptions[i].c_str());
    }
    // this extra addition to argv is accounted for as we are skipping the first entry in
    argv[argc++]=qstrdup("-w"); // finally, turn off warnings.
  }
  else
  {
    // add include paths for input files
    if (clangIncludeInputPaths)
    {
      for (const std::string &path : Doxygen::inputPaths)
      {
        QCString inc = QCString("-I")+path.data();
        argv[argc++]=qstrdup(inc.data());
        //printf("argv[%d]=%s\n",argc,argv[argc]);
      }
    }
    // add external include paths
    for (size_t i=0;i<includePath.size();i++)
    {
      QCString inc = QCString("-I")+includePath[i].c_str();
      argv[argc++]=qstrdup(inc.data());
    }
    // user specified options
    for (size_t i=0;i<clangOptions.size();i++)
    {
      argv[argc++]=qstrdup(clangOptions[i].c_str());
    }
    // extra options
    argv[argc++]=qstrdup("-ferror-limit=0");
    argv[argc++]=qstrdup("-x");

    // Since we can be presented with a .h file that can contain C/C++ or
    // Objective C code and we need to configure the parser before knowing this,
    // we use the source file to detected the language. Detection will fail if you
    // pass a bunch of .h files containing ObjC code, and no sources :-(
    SrcLangExt lang = getLanguageFromFileName(fileName);
    if (lang==SrcLangExt_ObjC || p->detectedLang!=DetectedLang::Cpp)
    {
      QCString fn = fileName;
      if (p->detectedLang!=DetectedLang::Cpp &&
          (fn.right(4).lower()==".cpp" || fn.right(4).lower()==".cxx" ||
           fn.right(3).lower()==".cc" || fn.right(2).lower()==".c"))
      { // fall back to C/C++ once we see an extension that indicates this
        p->detectedLang = DetectedLang::Cpp;
      }
      else if (fn.right(3).lower()==".mm") // switch to Objective C++
      {
        p->detectedLang = DetectedLang::ObjCpp;
      }
      else if (fn.right(2).lower()==".m") // switch to Objective C
      {
        p->detectedLang = DetectedLang::ObjC;
      }
    }
    switch (p->detectedLang)
    {
      case DetectedLang::Cpp:    argv[argc++]=qstrdup("c++");           break;
      case DetectedLang::ObjC:   argv[argc++]=qstrdup("objective-c");   break;
      case DetectedLang::ObjCpp: argv[argc++]=qstrdup("objective-c++"); break;
    }

    // provide the input and and its dependencies as unsaved files so we can
    // pass the filtered versions
    argv[argc++]=qstrdup(fileName.data());
  }
  //printf("source %s ----------\n%s\n-------------\n\n",
  //    fileName,p->source.data());
  int numUnsavedFiles = static_cast<int>(p->filesInSameTU.size()+1);
  p->numFiles = numUnsavedFiles;
  p->sources.resize(numUnsavedFiles);
  p->ufs.resize(numUnsavedFiles);
  p->sources[0]      = detab(fileToString(fileName,filterSourceFiles,TRUE));
  p->ufs[0].Filename = qstrdup(fileName.data());
  p->ufs[0].Contents = p->sources[0].data();
  p->ufs[0].Length   = p->sources[0].length();
  p->fileMapping.insert({fileName.data(),0});
  int i=1;
  for (auto it  = p->filesInSameTU.begin();
            it != p->filesInSameTU.end() && i<numUnsavedFiles;
          ++it, i++)
  {
    p->fileMapping.insert({it->c_str(),static_cast<uint>(i)});
    p->sources[i]      = detab(fileToString(it->c_str(),filterSourceFiles,TRUE));
    p->ufs[i].Filename = qstrdup(it->c_str());
    p->ufs[i].Contents = p->sources[i].data();
    p->ufs[i].Length   = p->sources[i].length();
  }

  // let libclang do the actual parsing
  p->tu = clang_parseTranslationUnit(p->index, 0,
                                     argv, argc, p->ufs.data(), numUnsavedFiles,
                                     CXTranslationUnit_DetailedPreprocessingRecord);
  //printf("  tu=%p\n",p->tu);
  // free arguments
  for (i=0;i<argc;++i)
  {
    delete[](argv[i]);
  }
  free(argv);

  if (p->tu)
  {
    // show any warnings that the compiler produced
    int n=clang_getNumDiagnostics(p->tu);
    for (i=0; i!=n; ++i)
    {
      CXDiagnostic diag = clang_getDiagnostic(p->tu, i);
      CXString string = clang_formatDiagnostic(diag,
          clang_defaultDiagnosticDisplayOptions());
      err("%s [clang]\n",clang_getCString(string));
      clang_disposeString(string);
      clang_disposeDiagnostic(diag);
    }
  }
  else
  {
    err("clang: Failed to parse translation unit %s\n",qPrint(fileName));
  }
}

ClangTUParser::~ClangTUParser()
{
  //printf("ClangTUParser::~ClangTUParser() this=%p\n",this);
  static bool clangAssistedParsing = Config_getBool(CLANG_ASSISTED_PARSING);
  if (!clangAssistedParsing) return;
  if (p->tu)
  {
    p->cursors.clear();
    clang_disposeTokens(p->tu,p->tokens,p->numTokens);
    clang_disposeTranslationUnit(p->tu);
    clang_disposeIndex(p->index);
    p->fileMapping.clear();
    p->tokens    = 0;
    p->numTokens = 0;
  }
  for (uint i=0;i<p->numFiles;i++)
  {
    delete[] p->ufs[i].Filename;
  }
  p->ufs.clear();
  p->sources.clear();
  p->numFiles  = 0;
  p->tu        = 0;
}

void ClangTUParser::switchToFile(const FileDef *fd)
{
  //printf("ClangTUParser::switchToFile(%s) this=%p\n",qPrint(fd->absFilePath()),this);
  if (p->tu)
  {
    p->cursors.clear();
    clang_disposeTokens(p->tu,p->tokens,p->numTokens);
    p->tokens    = 0;
    p->numTokens = 0;

    CXFile f = clang_getFile(p->tu, fd->absFilePath().data());
    auto it = p->fileMapping.find(fd->absFilePath().data());
    if (it!=p->fileMapping.end() && it->second < p->numFiles)
    {
      uint i = it->second;
      //printf("switchToFile %s: len=%ld\n",fileName,p->ufs[i].Length);
      CXSourceLocation fileBegin = clang_getLocationForOffset(p->tu, f, 0);
      CXSourceLocation fileEnd   = clang_getLocationForOffset(p->tu, f, p->ufs[i].Length);
      CXSourceRange    fileRange = clang_getRange(fileBegin, fileEnd);

      clang_tokenize(p->tu,fileRange,&p->tokens,&p->numTokens);
      p->cursors.resize(p->numTokens);
      clang_annotateTokens(p->tu,p->tokens,p->numTokens,p->cursors.data());
      p->curToken = 0;
    }
    else
    {
      err("clang: Failed to find input file %s in mapping\n",qPrint(fd->absFilePath()));
    }
  }
}

std::string ClangTUParser::lookup(uint line,const char *symbol)
{
  //printf("ClangParser::lookup(%d,%s)\n",line,symbol);
  std::string result;
  if (symbol==0) return result;
  static bool clangAssistedParsing = Config_getBool(CLANG_ASSISTED_PARSING);
  if (!clangAssistedParsing) return result;

  auto getCurrentTokenLine = [=]() -> uint
  {
    uint l, c;
    if (p->numTokens==0) return 1;
    // guard against filters that reduce the number of lines
    if (p->curToken>=p->numTokens) p->curToken=p->numTokens-1;
    CXSourceLocation start = clang_getTokenLocation(p->tu,p->tokens[p->curToken]);
    clang_getSpellingLocation(start, 0, &l, &c, 0);
    return l;
  };

  int sl = strlen(symbol);
  uint l = getCurrentTokenLine();
  while (l>=line && p->curToken>0)
  {
    if (l==line) // already at the right line
    {
      p->curToken--; // linear search to start of the line
      l = getCurrentTokenLine();
    }
    else
    {
      p->curToken/=2; // binary search backward
      l = getCurrentTokenLine();
    }
  }
  bool found=FALSE;
  while (l<=line && p->curToken<p->numTokens && !found)
  {
    CXString tokenString = clang_getTokenSpelling(p->tu, p->tokens[p->curToken]);
    //if (l==line)
    //{
    //  printf("try to match symbol %s with token %s\n",symbol,clang_getCString(tokenString));
    //}
    const char *ts = clang_getCString(tokenString);
    int tl = strlen(ts);
    int startIndex = p->curToken;
    if (l==line && strncmp(ts,symbol,tl)==0) // found partial match at the correct line
    {
      int offset = tl;
      while (offset<sl) // symbol spans multiple tokens
      {
        //printf("found partial match\n");
        p->curToken++;
        if (p->curToken>=p->numTokens)
        {
          break; // end of token stream
        }
        l = getCurrentTokenLine();
        clang_disposeString(tokenString);
        tokenString = clang_getTokenSpelling(p->tu, p->tokens[p->curToken]);
        ts = clang_getCString(tokenString);
        tl = ts ? strlen(ts) : 0;
        // skip over any spaces in the symbol
        char c;
        while (offset<sl && ((c=symbol[offset])==' ' || c=='\t' || c=='\r' || c=='\n'))
        {
          offset++;
        }
        if (strncmp(ts,symbol+offset,tl)!=0) // next token matches?
        {
          //printf("no match '%s'<->'%s'\n",ts,symbol+offset);
          break; // no match
        }
        //printf("partial match '%s'<->'%s'\n",ts,symbol+offset);
        offset+=tl;
      }
      if (offset==sl) // symbol matches the token(s)
      {
        CXCursor c = p->cursors[p->curToken];
        CXString usr = clang_getCursorUSR(c);
        //printf("found full match %s usr='%s'\n",symbol,clang_getCString(usr));
        result = clang_getCString(usr);
        clang_disposeString(usr);
        found=TRUE;
      }
      else // reset token cursor to start of the search
      {
        p->curToken = startIndex;
      }
    }
    clang_disposeString(tokenString);
    p->curToken++;
    if (p->curToken<p->numTokens)
    {
      l = getCurrentTokenLine();
    }
  }
  //if (!found)
  //{
  //  printf("Did not find symbol %s at line %d :-(\n",symbol,line);
  //}
  //else
  //{
  //  printf("Found symbol %s usr=%s\n",symbol,result.data());
  //}
  return result;
}


void ClangTUParser::writeLineNumber(CodeOutputInterface &ol,const FileDef *fd,uint line,bool includeCodeFragment)
{
  const Definition *d = fd ? fd->getSourceDefinition(line) : 0;
  if (d && d->isLinkable())
  {
    p->currentLine=line;
    const MemberDef *md = fd->getSourceMember(line);
    if (md && md->isLinkable())  // link to member
    {
      if (p->currentMemberDef!=md) // new member, start search for body
      {
        p->searchForBody=TRUE;
        p->insideBody=FALSE;
        p->bracketCount=0;
      }
      p->currentMemberDef=md;
      ol.writeLineNumber(md->getReference(),
                         md->getOutputFileBase(),
                         md->anchor(),
                         line,includeCodeFragment);
    }
    else // link to compound
    {
      p->currentMemberDef=0;
      ol.writeLineNumber(d->getReference(),
                         d->getOutputFileBase(),
                         d->anchor(),
                         line,includeCodeFragment);
    }
  }
  else // no link
  {
    ol.writeLineNumber(QCString(),QCString(),QCString(),line,includeCodeFragment);
  }

  // set search page target
  if (Doxygen::searchIndex)
  {
    QCString lineAnchor;
    lineAnchor.sprintf("l%05d",line);
    ol.setCurrentDoc(fd,lineAnchor,TRUE);
  }

  //printf("writeLineNumber(%d) g_searchForBody=%d\n",line,g_searchForBody);
}

void ClangTUParser::codifyLines(CodeOutputInterface &ol,const FileDef *fd,const char *text,
                        uint &line,uint &column,const char *fontClass)
{
  if (fontClass) ol.startFontClass(fontClass);
  const char *p=text,*sp=p;
  char c;
  bool done=FALSE;
  while (!done)
  {
    sp=p;
    while ((c=*p++) && c!='\n') { column++; }
    if (c=='\n')
    {
      line++;
      int l = (int)(p-sp-1);
      column=l+1;
      char *tmp = (char*)malloc(l+1);
      memcpy(tmp,sp,l);
      tmp[l]='\0';
      ol.codify(tmp);
      free(tmp);
      if (fontClass) ol.endFontClass();
      ol.endCodeLine();
      ol.startCodeLine(TRUE);
      writeLineNumber(ol,fd,line,includeCodeFragment);
      if (fontClass) ol.startFontClass(fontClass);
    }
    else
    {
      ol.codify(sp);
      done=TRUE;
    }
  }
  if (fontClass) ol.endFontClass();
}

void ClangTUParser::writeMultiLineCodeLink(CodeOutputInterface &ol,
                  const FileDef *fd,uint &line,uint &column,
                  const Definition *d,
                  const char *text)
{
  static bool sourceTooltips = Config_getBool(SOURCE_TOOLTIPS);
  p->tooltipManager.addTooltip(ol,d);
  QCString ref  = d->getReference();
  QCString file = d->getOutputFileBase();
  QCString anchor = d->anchor();
  QCString tooltip;
  if (!sourceTooltips) // fall back to simple "title" tooltips
  {
   tooltip = d->briefDescriptionAsTooltip();
  }
  bool done=FALSE;
  char *p=(char *)text;
  while (!done)
  {
    char *sp=p;
    char c;
    while ((c=*p++) && c!='\n') { column++; }
    if (c=='\n')
    {
      line++;
      *(p-1)='\0';
      //printf("writeCodeLink(%s,%s,%s,%s)\n",ref,file,anchor,sp);
      ol.writeCodeLink(d->codeSymbolType(),ref,file,anchor,sp,tooltip);
      ol.endCodeLine();
      ol.startCodeLine(TRUE);
      writeLineNumber(ol,fd,line,includeCodeFragment);
    }
    else
    {
      //printf("writeCodeLink(%s,%s,%s,%s)\n",ref,file,anchor,sp);
      ol.writeCodeLink(d->codeSymbolType(),ref,file,anchor,sp,tooltip);
      done=TRUE;
    }
  }
}

void ClangTUParser::linkInclude(CodeOutputInterface &ol,const FileDef *fd,
    uint &line,uint &column,const char *text)
{
  QCString incName = text;
  incName = incName.mid(1,incName.length()-2); // strip ".." or  <..>
  FileDef *ifd=0;
  if (!incName.isEmpty())
  {
    FileName *fn = Doxygen::inputNameLinkedMap->find(incName);
    if (fn)
    {
      // see if this source file actually includes the file
      auto it = std::find_if(fn->begin(),
                             fn->end(),
                             [&fd](const auto &ifd)
                             { return fd->isIncluded(ifd->absFilePath()); });
      bool found = it!=fn->end();
      if (found)
      {
        //printf("      include file %s found=%d\n",(*it)->absFilePath().data(),found);
        ifd = it->get();
      }
    }
  }
  if (ifd)
  {
    ol.writeCodeLink(ifd->codeSymbolType(),
                     ifd->getReference(),
                     ifd->getOutputFileBase(),
                     QCString(),
                     text,
                     ifd->briefDescriptionAsTooltip());
  }
  else
  {
    codifyLines(ol,ifd,text,line,column,"preprocessor");
  }
}

void ClangTUParser::linkMacro(CodeOutputInterface &ol,const FileDef *fd,
    uint &line,uint &column,const char *text)
{
  MemberName *mn=Doxygen::functionNameLinkedMap->find(text);
  if (mn)
  {
    for (const auto &md : *mn)
    {
      if (md->isDefine())
      {
        writeMultiLineCodeLink(ol,fd,line,column,md.get(),text);
        return;
      }
    }
  }
  codifyLines(ol,fd,text,line,column);
}


void ClangTUParser::linkIdentifier(CodeOutputInterface &ol,const FileDef *fd,
    uint &line,uint &column,const char *text,int tokenIndex)
{
  CXCursor c = p->cursors[tokenIndex];
  CXCursor r = clang_getCursorReferenced(c);
  if (!clang_equalCursors(r, c))
  {
    c=r; // link to referenced location
  }
  CXCursor t = clang_getSpecializedCursorTemplate(c);
  if (!clang_Cursor_isNull(t) && !clang_equalCursors(t,c))
  {
    c=t; // link to template
  }
  CXString usr = clang_getCursorUSR(c);
  const char *usrStr = clang_getCString(usr);

  const Definition *d = 0;
  auto kv = Doxygen::clangUsrMap->find(usrStr);
  if (kv!=Doxygen::clangUsrMap->end())
  {
    d = kv->second;
  }
  //CXCursorKind kind = clang_getCursorKind(c);
  //if (d==0)
  //{
  //  printf("didn't find definition for '%s' usr='%s' kind=%d\n",
  //      text,usrStr,kind);
  //}
  //else
  //{
  //  printf("found definition for '%s' usr='%s' name='%s'\n",
  //      text,usrStr,d->name().data());
  //}
  if (d && d->isLinkable())
  {
    if (p->insideBody &&
        p->currentMemberDef && d->definitionType()==Definition::TypeMember &&
        (p->currentMemberDef!=d || p->currentLine<line)) // avoid self-reference
    {
      std::lock_guard<std::mutex> lock(g_docCrossReferenceMutex);
      addDocCrossReference(toMemberDefMutable(p->currentMemberDef),toMemberDefMutable(d));
    }
    writeMultiLineCodeLink(ol,fd,line,column,d,text);
  }
  else
  {
    codifyLines(ol,fd,text,line,column);
  }
  clang_disposeString(usr);
}

void ClangTUParser::detectFunctionBody(const char *s)
{
  //printf("punct=%s g_searchForBody=%d g_insideBody=%d g_bracketCount=%d\n",
  //  s,g_searchForBody,g_insideBody,g_bracketCount);

  if (p->searchForBody && (qstrcmp(s,":")==0 || qstrcmp(s,"{")==0)) // start of 'body' (: is for constructor)
  {
    p->searchForBody=FALSE;
    p->insideBody=TRUE;
  }
  else if (p->searchForBody && qstrcmp(s,";")==0) // declaration only
  {
    p->searchForBody=FALSE;
    p->insideBody=FALSE;
  }
  if (p->insideBody && qstrcmp(s,"{")==0) // increase scoping level
  {
    p->bracketCount++;
  }
  if (p->insideBody && qstrcmp(s,"}")==0) // decrease scoping level
  {
    p->bracketCount--;
    if (p->bracketCount<=0) // got outside of function body
    {
      p->insideBody=FALSE;
      p->bracketCount=0;
    }
  }
}

void ClangTUParser::writeSources(CodeOutputInterface &ol,const FileDef *fd)
{
  // (re)set global parser state
  p->currentMemberDef=0;
  p->currentLine=0;
  p->searchForBody=FALSE;
  p->insideBody=FALSE;
  p->bracketCount=0;

  unsigned int line=1,column=1;
  QCString lineNumber,lineAnchor;
  ol.startCodeLine(TRUE);
  writeLineNumber(ol,fd,line,includeCodeFragment);
  for (unsigned int i=0;i<p->numTokens;i++)
  {
    CXSourceLocation start = clang_getTokenLocation(p->tu, p->tokens[i]);
    unsigned int l, c;
    clang_getSpellingLocation(start, 0, &l, &c, 0);
    if (l > line) column = 1;
    while (line<l)
    {
      line++;
      ol.endCodeLine();
      ol.startCodeLine(TRUE);
      writeLineNumber(ol,fd,line,includeCodeFragment);
    }
    while (column<c) { ol.codify(" "); column++; }
    CXString tokenString = clang_getTokenSpelling(p->tu, p->tokens[i]);
    char const *s = clang_getCString(tokenString);
    CXCursorKind cursorKind  = clang_getCursorKind(p->cursors[i]);
    CXTokenKind tokenKind = clang_getTokenKind(p->tokens[i]);
    //printf("%d:%d %s cursorKind=%d tokenKind=%d\n",line,column,s,cursorKind,tokenKind);
    switch (tokenKind)
    {
      case CXToken_Keyword:
        if (strcmp(s,"operator")==0)
        {
          linkIdentifier(ol,fd,line,column,s,i);
        }
        else
        {
          codifyLines(ol,fd,s,line,column,
              cursorKind==CXCursor_PreprocessingDirective ? "preprocessor" :
              keywordToType(s));
        }
        break;
      case CXToken_Literal:
        if (cursorKind==CXCursor_InclusionDirective)
        {
          linkInclude(ol,fd,line,column,s);
        }
        else if (s[0]=='"' || s[0]=='\'')
        {
          codifyLines(ol,fd,s,line,column,"stringliteral");
        }
        else
        {
          codifyLines(ol,fd,s,line,column);
        }
        break;
      case CXToken_Comment:
        codifyLines(ol,fd,s,line,column,"comment");
        break;
      default:  // CXToken_Punctuation or CXToken_Identifier
        if (tokenKind==CXToken_Punctuation)
        {
          detectFunctionBody(s);
          //printf("punct %s: %d\n",s,cursorKind);
        }
        switch (cursorKind)
        {
          case CXCursor_PreprocessingDirective:
            codifyLines(ol,fd,s,line,column,"preprocessor");
            break;
          case CXCursor_MacroDefinition:
            codifyLines(ol,fd,s,line,column,"preprocessor");
            break;
          case CXCursor_InclusionDirective:
            linkInclude(ol,fd,line,column,s);
            break;
          case CXCursor_MacroExpansion:
            linkMacro(ol,fd,line,column,s);
            break;
          default:
            if (tokenKind==CXToken_Identifier ||
                (tokenKind==CXToken_Punctuation && // for operators
                 (cursorKind==CXCursor_DeclRefExpr ||
                  cursorKind==CXCursor_MemberRefExpr ||
                  cursorKind==CXCursor_CallExpr ||
                  cursorKind==CXCursor_ObjCMessageExpr)
                 )
               )
            {
              linkIdentifier(ol,fd,line,column,s,i);
              if (Doxygen::searchIndex)
              {
                ol.addWord(s,FALSE);
              }
            }
            else
            {
              codifyLines(ol,fd,s,line,column);
            }
            break;
        }
    }
    clang_disposeString(tokenString);
  }
  ol.endCodeLine();
  p->tooltipManager.writeTooltips(ol);
}

//--------------------------------------------------------------------------

class ClangParser::Private
{
  public:
    Private()
    {
      std::string error;
      QCString clangCompileDatabase = Config_getString(CLANG_DATABASE_PATH);
      // load a clang compilation database (https://clang.llvm.org/docs/JSONCompilationDatabase.html)
      db = clang::tooling::CompilationDatabase::loadFromDirectory(clangCompileDatabase.data(), error);
      if (!clangCompileDatabase.isEmpty() && clangCompileDatabase!="0" && db==nullptr)
      {
          // user specified a path, but DB file was not found
          err("%s using clang compilation database path of: \"%s\"\n", error.c_str(),
              clangCompileDatabase.data());
      }
    }

    std::unique_ptr<clang::tooling::CompilationDatabase> db;
};

const clang::tooling::CompilationDatabase *ClangParser::database() const
{
  return p->db.get();
}

ClangParser::ClangParser() : p(std::make_unique<Private>())
{
}

ClangParser::~ClangParser()
{
}

std::unique_ptr<ClangTUParser> ClangParser::createTUParser(const FileDef *fd) const
{
  //printf("ClangParser::createTUParser()\n");
  return std::make_unique<ClangTUParser>(*this,fd);
}


//--------------------------------------------------------------------------
#else // use stubbed functionality in case libclang support is disabled.

void ClangTUParser::switchToFile(const FileDef *fd)
{
}

void ClangTUParser::parse()
{
}

std::string ClangTUParser::lookup(uint,const char *)
{
  return std::string();
}

class ClangParser::Private
{
};

ClangParser::ClangParser() : p(std::make_unique<Private>())
{
}

ClangParser::~ClangParser()
{
}

std::unique_ptr<ClangTUParser> ClangParser::createTUParser(const FileDef *) const
{
  return nullptr;
}

#endif
//--------------------------------------------------------------------------

