#include "clangparser.h"
#include "settings.h"
#include <cstdio>
#include <cstdint>
#include <vector>
#include <mutex>

#if USE_LIBCLANG
#include <clang-c/Index.h>
#include "clang/Tooling/CompilationDatabase.h"
#include "clang/Tooling/Tooling.h"
#include <stdlib.h>
#include "message.h"
#include "outputgen.h"
#include "outputlist.h"
#include "filedef.h"
#include "memberdef.h"
#include "doxygen.h"
#include "util.h"
#include "config.h"
#include "membername.h"
#include "filename.h"
#include "tooltip.h"
#include "utf8.h"
#include "searchindex.h"
#include "trace.h"
#endif

//--------------------------------------------------------------------------

static std::mutex g_clangMutex;

ClangParser *ClangParser::instance()
{
  std::lock_guard<std::mutex> lock(g_clangMutex);
  if (s_instance==nullptr) s_instance = new ClangParser;
  return s_instance;
}

ClangParser *ClangParser::s_instance = nullptr;

//--------------------------------------------------------------------------
#if USE_LIBCLANG

static std::mutex g_docCrossReferenceMutex;

enum class DetectedLang { Cpp, ObjC, ObjCpp };

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
    CXIndex index = nullptr;
    uint32_t curToken = 0;
    DetectedLang detectedLang = DetectedLang::Cpp;
    size_t numFiles = 0;
    std::vector<QCString> sources;
    std::vector<CXUnsavedFile> ufs;
    std::vector<CXCursor> cursors;
    std::unordered_map<std::string,uint32_t> fileMapping;
    CXTranslationUnit tu = nullptr;
    CXToken *tokens = nullptr;
    uint32_t numTokens = 0;
    StringVector filesInSameTU;
    TooltipManager tooltipManager;
    std::vector<const Definition *> foldStack;

    // state while parsing sources
    const MemberDef  *currentMemberDef=nullptr;
    uint32_t          currentLine=0;
    bool              searchForBody=FALSE;
    bool              insideBody=FALSE;
    uint32_t          bracketCount=0;
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
  assert(p->index==nullptr);
  assert(p->tokens==nullptr);
  assert(p->numTokens==0);
  p->index    = clang_createIndex(0, 0);
  p->curToken = 0;
  p->cursors.clear();
  std::vector<clang::tooling::CompileCommand> command;
  if (p->parser.database()!=nullptr)
  {
    // check if the file we are parsing is in the DB
    command = p->parser.database()->getCompileCommands(fileName.data());
  }
  std::vector<char *> argv;
  if (!command.empty() )
  {
    std::vector<std::string> options = command[command.size()-1].CommandLine;
    // copy each compiler option used from the database. Skip the first which is compiler exe.
    for (auto option = options.begin()+1; option != options.end(); option++)
    {
      argv.push_back(qstrdup(option->c_str()));
    }
    // The last compile command (last entry of argv) should be the filename of the source
    // file to parse. It does not matter to clang_parseTranslationUnit below if we pass the file name
    // separately in its second argument or if we just pass it a nullptr as the second
    // argument and pass the file name with the other compile commands.
    // However, in some cases (e.g., starting from Clang 14, if we are parsing a header file, see
    // https://github.com/doxygen/doxygen/issues/10733), the compile commands returned by
    // getCompileCommands include a "--" as second to last argument (which is supposed to make it
    // easier to parse the argument list). If we pass this "--" to clang_parseTranslationUnit below,
    // it returns an error. To avoid this, we remove the file name argument (and the "--" if present)
    // from argv and pass the file name separately.
    argv.pop_back(); // remove file name
    if (std::string(argv[argv.size() - 1]) == "--") {
      // remove '--' from argv
      argv.pop_back();
    }

    // user specified options
    for (size_t i=0;i<clangOptions.size();i++)
    {
      argv.push_back(qstrdup(clangOptions[i].c_str()));
    }
    // this extra addition to argv is accounted for as we are skipping the first entry in
    argv.push_back(qstrdup("-w")); // finally, turn off warnings.
  }
  else
  {
    // add include paths for input files
    if (clangIncludeInputPaths)
    {
      for (const std::string &path : Doxygen::inputPaths)
      {
        QCString inc = QCString("-I")+path.data();
        argv.push_back(qstrdup(inc.data()));
        //printf("argv[%d]=%s\n",argc,argv[argc]);
      }
    }
    // add external include paths
    for (size_t i=0;i<includePath.size();i++)
    {
      QCString inc = "-I"+includePath[i];
      argv.push_back(qstrdup(inc.data()));
    }
    // user specified options
    for (size_t i=0;i<clangOptions.size();i++)
    {
      argv.push_back(qstrdup(clangOptions[i].c_str()));
    }
    // extra options
    argv.push_back(qstrdup("-ferror-limit=0"));
    argv.push_back(qstrdup("-x"));

    // Since we can be presented with a .h file that can contain C/C++ or
    // Objective C code and we need to configure the parser before knowing this,
    // we use the source file to detected the language. Detection will fail if you
    // pass a bunch of .h files containing ObjC code, and no sources :-(
    SrcLangExt lang = getLanguageFromFileName(fileName);
    QCString fn = fileName.lower();
    if (lang==SrcLangExt::ObjC || p->detectedLang!=DetectedLang::Cpp)
    {
      if (p->detectedLang!=DetectedLang::Cpp &&
          (fn.endsWith(".cpp") || fn.endsWith(".cxx") ||
           fn.endsWith(".cc")  || fn.endsWith(".c")))
      { // fall back to C/C++ once we see an extension that indicates this
        p->detectedLang = DetectedLang::Cpp;
      }
      else if (fn.endsWith(".mm")) // switch to Objective C++
      {
        p->detectedLang = DetectedLang::ObjCpp;
      }
      else if (fn.endsWith(".m")) // switch to Objective C
      {
        p->detectedLang = DetectedLang::ObjC;
      }
    }
    switch (p->detectedLang)
    {
      case DetectedLang::Cpp:
        if (fn.endsWith(".hpp") || fn.endsWith(".hxx") ||
            fn.endsWith(".hh")  || fn.endsWith(".h"))
          argv.push_back(qstrdup("c++-header"));
        else
          argv.push_back(qstrdup("c++"));
        break;
      case DetectedLang::ObjC:   argv.push_back(qstrdup("objective-c"));   break;
      case DetectedLang::ObjCpp: argv.push_back(qstrdup("objective-c++")); break;
    }
  }
  //printf("source %s ----------\n%s\n-------------\n\n",
  //    fileName,p->source.data());
  size_t numUnsavedFiles = p->filesInSameTU.size()+1;
  p->numFiles = numUnsavedFiles;
  p->sources.resize(numUnsavedFiles);
  p->ufs.resize(numUnsavedFiles);
  size_t refIndent = 0;
  p->sources[0]      = detab(fileToString(fileName,filterSourceFiles,TRUE),refIndent);
  p->ufs[0].Filename = qstrdup(fileName.data());
  p->ufs[0].Contents = p->sources[0].data();
  p->ufs[0].Length   = p->sources[0].length();
  p->fileMapping.emplace(fileName.data(),0);
  size_t i=1;
  for (auto it  = p->filesInSameTU.begin();
            it != p->filesInSameTU.end() && i<numUnsavedFiles;
          ++it, i++)
  {
    p->fileMapping.emplace(std::make_pair(*it,static_cast<uint32_t>(i)));
    p->sources[i]      = detab(fileToString(QCString(*it),filterSourceFiles,TRUE),refIndent);
    p->ufs[i].Filename = qstrdup(it->c_str());
    p->ufs[i].Contents = p->sources[i].data();
    p->ufs[i].Length   = p->sources[i].length();
  }

  // let libclang do the actual parsing
  //for (i=0;i<argv.size();i++) printf("Argument %d: %s\n",i,argv[i]);
  p->tu = clang_parseTranslationUnit(p->index, fileName.data(),
                                     argv.data(), static_cast<int>(argv.size()), p->ufs.data(), numUnsavedFiles,
                                     CXTranslationUnit_DetailedPreprocessingRecord);
  //printf("  tu=%p\n",p->tu);
  // free arguments
  for (i=0;i<argv.size();++i)
  {
    qstrfree(argv[i]);
  }

  if (p->tu)
  {
    // show any warnings that the compiler produced
    size_t n=clang_getNumDiagnostics(p->tu);
    for (i=0; i!=n; ++i)
    {
      CXDiagnostic diag = clang_getDiagnostic(p->tu, static_cast<unsigned>(i));
      CXString string = clang_formatDiagnostic(diag,
          clang_defaultDiagnosticDisplayOptions());
      err("{} [clang]\n",clang_getCString(string));
      clang_disposeString(string);
      clang_disposeDiagnostic(diag);
    }
  }
  else
  {
    err("clang: Failed to parse translation unit {}\n",fileName);
  }
}

ClangTUParser::~ClangTUParser()
{
  //printf("ClangTUParser::~ClangTUParser() this=%p\n",this);
  bool clangAssistedParsing = Config_getBool(CLANG_ASSISTED_PARSING);
  if (!clangAssistedParsing) return;
  if (p->tu)
  {
    p->cursors.clear();
    clang_disposeTokens(p->tu,p->tokens,p->numTokens);
    clang_disposeTranslationUnit(p->tu);
    clang_disposeIndex(p->index);
    p->fileMapping.clear();
    p->tokens    = nullptr;
    p->numTokens = 0;
  }
  for (size_t i=0;i<p->numFiles;i++)
  {
    delete[] p->ufs[i].Filename;
  }
  p->ufs.clear();
  p->sources.clear();
  p->numFiles  = 0;
  p->tu        = nullptr;
}

void ClangTUParser::switchToFile(const FileDef *fd)
{
  //printf("ClangTUParser::switchToFile(%s) this=%p\n",qPrint(fd->absFilePath()),this);
  if (p->tu)
  {
    p->cursors.clear();
    clang_disposeTokens(p->tu,p->tokens,p->numTokens);
    p->tokens    = nullptr;
    p->numTokens = 0;

    CXFile f = clang_getFile(p->tu, fd->absFilePath().data());
    auto it = p->fileMapping.find(fd->absFilePath().data());
    if (it!=p->fileMapping.end() && it->second < p->numFiles)
    {
      uint32_t i = it->second;
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
      err("clang: Failed to find input file {} in mapping\n",fd->absFilePath());
    }
  }
}

std::string ClangTUParser::lookup(uint32_t line,const char *symbol)
{
  AUTO_TRACE("line={},symbol={}",line,symbol);
  std::string result;
  if (symbol==nullptr) return result;
  bool clangAssistedParsing = Config_getBool(CLANG_ASSISTED_PARSING);
  if (!clangAssistedParsing) return result;

  auto getCurrentTokenLine = [this]() -> uint32_t
  {
    uint32_t l=0, c=0;
    if (p->numTokens==0) return 1;
    // guard against filters that reduce the number of lines
    if (p->curToken>=p->numTokens) p->curToken=p->numTokens-1;
    CXSourceLocation start = clang_getTokenLocation(p->tu,p->tokens[p->curToken]);
    clang_getSpellingLocation(start, nullptr, &l, &c, nullptr);
    return l;
  };

  int sl = strlen(symbol);
  uint32_t l = getCurrentTokenLine();
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
    if (l==line)
    {
      AUTO_TRACE_ADD("try to match symbol {} with token {}",symbol,clang_getCString(tokenString));
    }
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
        char c = 0;
        while (offset<sl && ((c=symbol[offset])==' ' || c=='\t' || c=='\r' || c=='\n'))
        {
          offset++;
        }
        if (strncmp(ts,symbol+offset,tl)!=0) // next token matches?
        {
          //printf("no match '%s'<->'%s'\n",ts,symbol+offset);
          break; // no match
        }
        AUTO_TRACE_ADD("partial match '{}'<->'{}'",ts,symbol+offset);
        offset+=tl;
      }
      if (offset==sl) // symbol matches the token(s)
      {
        CXCursor c = p->cursors[p->curToken];
        CXString usr = clang_getCursorUSR(c);
        AUTO_TRACE_ADD("found full match {} usr='{}'",symbol,clang_getCString(usr));
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
  if (!found)
  {
    AUTO_TRACE_EXIT("Did not find symbol {} at line {} :-(",symbol,line);
  }
  else
  {
    AUTO_TRACE_EXIT("Found symbol {} usr={}",symbol,result);
  }
  return result;
}

void ClangTUParser::codeFolding(OutputCodeList &ol,const Definition *d,uint32_t line)
{
  if (Config_getBool(HTML_CODE_FOLDING))
  {
    endCodeFold(ol,line);
    if (d)
    {
      int startLine = d->getStartDefLine();
      int endLine   = d->getEndBodyLine();
      if (endLine!=-1 && startLine!=endLine &&
          // since the end of a section is closed after the last line, we need to avoid starting a
          // new section if the previous section ends at the same line, i.e. something like
          // struct X {
          // ...
          // }; struct S {  <- start of S and end of X at the same line
          // ...
          // };
          (p->foldStack.empty() || p->foldStack.back()->getEndBodyLine()!=startLine))
      {
        if (d->definitionType()==Definition::TypeMember)
        {
          const MemberDef *md = toMemberDef(d);
          if (md && md->isDefine())
          {
            ol.startFold(line,"",""); // #define X ...
          }
          else if (md && md->isCallable())
          {
            ol.startFold(line,"{","}"); // func() { ... }
          }
          else
          {
            ol.startFold(line,"{","};"); // enum X { ... }
          }
        }
        else if (d->definitionType()==Definition::TypeClass)
        {
          ol.startFold(line,"{","};"); // class X { ... };
        }
        else
        {
          ol.startFold(line,"{","}"); // namespace X {...}
        }
        p->foldStack.push_back(d);
      }
    }
  }
}

void ClangTUParser::endCodeFold(OutputCodeList &ol,uint32_t line)
{
  while (!p->foldStack.empty())
  {
    const Definition *dd = p->foldStack.back();
    if (dd->getEndBodyLine()+1==static_cast<int>(line))
    {
      ol.endFold();
      p->foldStack.pop_back();
    }
    else
    {
      break;
    }
  }
}

void ClangTUParser::writeLineNumber(OutputCodeList &ol,const FileDef *fd,uint32_t line,bool writeLineAnchor)
{
  const Definition *d = fd ? fd->getSourceDefinition(line) : nullptr;
  if (d)
  {
    p->currentLine=line;
    const MemberDef *md = fd->getSourceMember(line);
    //printf("writeLineNumber(%p,line=%d)\n",(void*)md,line);
    if (md && md->isLinkable())  // link to member
    {
      if (p->currentMemberDef!=md) // new member, start search for body
      {
        p->searchForBody=TRUE;
        p->insideBody=FALSE;
        p->bracketCount=0;
      }
      p->currentMemberDef=md;
      codeFolding(ol,md,line);
      ol.writeLineNumber(md->getReference(),
                         md->getOutputFileBase(),
                         md->anchor(),
                         line,writeLineAnchor);
    }
    else if (d->isLinkable()) // link to compound
    {
      p->currentMemberDef=nullptr;
      codeFolding(ol,d,line);
      ol.writeLineNumber(d->getReference(),
                         d->getOutputFileBase(),
                         d->anchor(),
                         line,writeLineAnchor);
    }
    else // no link
    {
      codeFolding(ol,nullptr,line);
      ol.writeLineNumber(QCString(),QCString(),QCString(),line,writeLineAnchor);
    }
  }
  else // no link
  {
    codeFolding(ol,nullptr,line);
    ol.writeLineNumber(QCString(),QCString(),QCString(),line,writeLineAnchor);
  }

  // set search page target
  if (Doxygen::searchIndex.enabled())
  {
    QCString lineAnchor;
    lineAnchor.sprintf("l%05d",line);
    Doxygen::searchIndex.setCurrentDoc(fd,lineAnchor,TRUE);
  }

  //printf("writeLineNumber(%d) g_searchForBody=%d\n",line,g_searchForBody);
}

void ClangTUParser::codifyLines(OutputCodeList &ol,const FileDef *fd,const char *text,
                        uint32_t &line,uint32_t &column,const char *fontClass)
{
  if (fontClass) ol.startFontClass(fontClass);
  const char *p=text,*sp=p;
  char c = 0;
  bool inlineCodeFragment = false;
  bool done=FALSE;
  while (!done)
  {
    sp=p;
    while ((c=*p++) && c!='\n') { column++; }
    if (c=='\n')
    {
      line++;
      size_t l = static_cast<size_t>(p-sp-1);
      column=l+1;
      ol.codify(QCString(sp,l));
      if (fontClass) ol.endFontClass();
      ol.endCodeLine();
      writeLineNumber(ol,fd,line,inlineCodeFragment);
      ol.startCodeLine(line);
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

void ClangTUParser::writeMultiLineCodeLink(OutputCodeList &ol,
                  const FileDef *fd,uint32_t &line,uint32_t &column,
                  const Definition *d,
                  const char *text)
{
  bool sourceTooltips = Config_getBool(SOURCE_TOOLTIPS);
  p->tooltipManager.addTooltip(d);
  QCString ref  = d->getReference();
  QCString file = d->getOutputFileBase();
  QCString anchor = d->anchor();
  QCString tooltip;
  if (!sourceTooltips) // fall back to simple "title" tooltips
  {
   tooltip = d->briefDescriptionAsTooltip();
  }
  bool inlineCodeFragment = false;
  bool done=FALSE;
  const char *p=text;
  while (!done)
  {
    const char *sp=p;
    char c = 0;
    while ((c=*p++) && c!='\n') { column++; }
    if (c=='\n')
    {
      line++;
      //printf("writeCodeLink(%s,%s,%s,%s)\n",ref,file,anchor,sp);
      ol.writeCodeLink(d->codeSymbolType(),ref,file,anchor,QCString(sp,p-sp-1),tooltip);
      ol.endCodeLine();
      writeLineNumber(ol,fd,line,inlineCodeFragment);
      ol.startCodeLine(line);
    }
    else
    {
      //printf("writeCodeLink(%s,%s,%s,%s)\n",ref,file,anchor,sp);
      ol.writeCodeLink(d->codeSymbolType(),ref,file,anchor,sp,tooltip);
      done=TRUE;
    }
  }
}

void ClangTUParser::linkInclude(OutputCodeList &ol,const FileDef *fd,
    uint32_t &line,uint32_t &column,const char *text)
{
  QCString incName = text;
  incName = incName.mid(1,incName.length()-2); // strip ".." or  <..>
  FileDef *ifd=nullptr;
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

void ClangTUParser::linkMacro(OutputCodeList &ol,const FileDef *fd,
    uint32_t &line,uint32_t &column,const char *text)
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


void ClangTUParser::linkIdentifier(OutputCodeList &ol,const FileDef *fd,
    uint32_t &line,uint32_t &column,const char *text,int tokenIndex)
{
  AUTO_TRACE("line={} column={} text={}",line,column,text);
  CXCursor c = p->cursors[tokenIndex];
  CXCursorKind cKind = clang_getCursorKind(c);
  AUTO_TRACE_ADD("cursor kind={}",(int)cKind);
  CXCursor r = clang_getCursorReferenced(c);
  AUTO_TRACE_ADD("cursor reference kind={}",(int)clang_getCursorKind(r));
  if (!clang_equalCursors(r, c))
  {
    AUTO_TRACE_ADD("link to referenced location");
    c=r; // link to referenced location
  }
  if (!clang_isDeclaration(cKind))
  {
    CXCursor t = clang_getSpecializedCursorTemplate(c);
    AUTO_TRACE_ADD("cursor template kind={}",(int)clang_getCursorKind(t));
    if (!clang_Cursor_isNull(t) && !clang_equalCursors(t,c))
    {
      c=t; // link to template
    }
  }
  CXString usr = clang_getCursorUSR(c);
  const char *usrStr = clang_getCString(usr);
  AUTO_TRACE_ADD("usr={}",usrStr);

  const Definition *d = nullptr;
  auto kv = Doxygen::clangUsrMap->find(usrStr);
  if (kv!=Doxygen::clangUsrMap->end())
  {
    d = kv->second;
  }
  if (d==0)
  {
    AUTO_TRACE_ADD("didn't find definition for '{}' usr='{}' kind={}",
        text,usrStr,(int)clang_getCursorKind(c));
  }
  else
  {
    AUTO_TRACE_ADD("found definition for '{}' usr='{}' name='{}'",
        text,usrStr,d->name().data());
  }

  if (d && d->isLinkable())
  {
    //printf("linkIdentifier(%s) p->insideBody=%d p->currentMemberDef=%p\n",text,p->insideBody,(void*)p->currentMemberDef);
    if (p->insideBody &&
        p->currentMemberDef && d->definitionType()==Definition::TypeMember &&
        (p->currentMemberDef!=d || p->currentLine<line)) // avoid self-reference
    {
      std::lock_guard<std::mutex> lock(g_docCrossReferenceMutex);
      addDocCrossReference(p->currentMemberDef,toMemberDef(d));
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

void ClangTUParser::writeSources(OutputCodeList &ol,const FileDef *fd)
{
  AUTO_TRACE("file={}",fd->name());
  // (re)set global parser state
  p->currentMemberDef=nullptr;
  p->currentLine=0;
  p->searchForBody=FALSE;
  p->insideBody=FALSE;
  p->bracketCount=0;
  p->foldStack.clear();

  unsigned int line=1,column=1;
  QCString lineNumber,lineAnchor;
  bool inlineCodeFragment = false;
  writeLineNumber(ol,fd,line,!inlineCodeFragment);
  ol.startCodeLine(line);
  for (unsigned int i=0;i<p->numTokens;i++)
  {
    CXSourceLocation start = clang_getTokenLocation(p->tu, p->tokens[i]);
    unsigned int l=0, c=0;
    clang_getSpellingLocation(start, nullptr, &l, &c, nullptr);
    if (l > line) column = 1;
    while (line<l)
    {
      line++;
      ol.endCodeLine();
      writeLineNumber(ol,fd,line,!inlineCodeFragment);
      ol.startCodeLine(line);
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
              if (Doxygen::searchIndex.enabled())
              {
                Doxygen::searchIndex.addWord(s,FALSE);
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
  if (Config_getBool(HTML_CODE_FOLDING))
  {
    while (!p->foldStack.empty())
    {
      ol.endFold();
      p->foldStack.pop_back();
    }
  }
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
          err("{} using clang compilation database path of: \"{}\"\n", error, clangCompileDatabase);
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

class ClangTUParser::Private
{
};

void ClangTUParser::switchToFile(const FileDef *)
{
}

void ClangTUParser::parse()
{
}

ClangTUParser::ClangTUParser(const ClangParser &,const FileDef *) : p(std::make_unique<Private>())
{
}

ClangTUParser::~ClangTUParser()
{
}

std::string ClangTUParser::lookup(uint32_t,const char *)
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

