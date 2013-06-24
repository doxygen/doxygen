#include "clangparser.h"
#include "settings.h"
#include <stdio.h>

#if USE_LIBCLANG
#include <clang-c/Index.h>
#include <qfileinfo.h>
#include <stdlib.h>
#include "message.h"
#include "sortdict.h"
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

static Definition *g_currentDefinition=0;
static MemberDef  *g_currentMemberDef=0;
static uint        g_currentLine=0;
static bool        g_searchForBody=FALSE;
static bool        g_insideBody=FALSE;
static uint        g_bracketCount=0;
#endif

ClangParser *ClangParser::instance()
{
  if (!s_instance) s_instance = new ClangParser;
  return s_instance;
}

ClangParser *ClangParser::s_instance = 0;

//--------------------------------------------------------------------------
#if USE_LIBCLANG

class ClangParser::Private
{
  public:
    enum DetectedLang { Detected_Cpp, Detected_ObjC, Detected_ObjCpp };
    Private() : tu(0), tokens(0), numTokens(0), cursors(0), 
                ufs(0), sources(0), numFiles(0), fileMapping(257),
                detectedLang(Detected_Cpp)
    { fileMapping.setAutoDelete(TRUE); }
    int getCurrentTokenLine();
    CXIndex index;
    CXTranslationUnit tu;
    QCString fileName;
    CXToken *tokens;
    uint numTokens;
    CXCursor *cursors;
    uint curLine;
    uint curToken;
    CXUnsavedFile *ufs;
    QCString *sources;
    uint numFiles;
    QDict<uint> fileMapping;
    DetectedLang detectedLang;
};

static QCString detab(const QCString &s)
{
  static int tabSize = Config_getInt("TAB_SIZE");
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
      case '\n': // reset colomn counter
        out.addChar(c);
        col=0;
        break;
      case ' ': // increment column counter
        out.addChar(c);
        col++;
        break;
      default: // non-whitespace => update minIndent
        out.addChar(c);
        if (c<0 && i<size) // multibyte sequence
        {
          out.addChar(data[i++]); // >= 2 bytes
          if (((uchar)c&0xE0)==0xE0 && i<size)
          {
            out.addChar(data[i++]); // 3 bytes
          }
          if (((uchar)c&0xF0)==0xF0 && i<size)
          {
            out.addChar(data[i++]); // 4 byres
          }
        }
        if (col<minIndent) minIndent=col;
        col++;
    }
  }
  out.addChar(0);
  //printf("detab refIndent=%d\n",refIndent);
  return out.get();
}

/** Callback function called for each include in a translation unit */
static void inclusionVisitor(CXFile includedFile,
                             CXSourceLocation* /*inclusionStack*/,
                             unsigned /*includeLen*/,
                             CXClientData clientData)
{
  QDict<void> *fileDict = (QDict<void> *)clientData;
  CXString incFileName = clang_getFileName(includedFile);
  //printf("--- file %s includes %s\n",fileName,clang_getCString(incFileName));
  fileDict->insert(clang_getCString(incFileName),(void*)0x8);
  clang_disposeString(incFileName);
}

/** filter the \a files and only keep those that are found as include files
 *  within the current translation unit.
 *  @param[in,out] files The list of files to filter.
 */ 
void ClangParser::determineInputFilesInSameTu(QStrList &files)
{
  // put the files in this translation unit in a dictionary
  QDict<void> incFound(257);
  clang_getInclusions(p->tu,
      inclusionVisitor,
      (CXClientData)&incFound
      );
  // create a new filtered file list
  QStrList resultIncludes;
  QStrListIterator it2(files);
  for (it2.toFirst();it2.current();++it2)
  {
    if (incFound.find(it2.current()))
    {
      resultIncludes.append(it2.current());
    }
  }
  // replace the original list
  files=resultIncludes;
}

void ClangParser::start(const char *fileName,QStrList &filesInTranslationUnit)
{
  static bool clangAssistedParsing = Config_getBool("CLANG_ASSISTED_PARSING");
  static QStrList &includePath = Config_getList("INCLUDE_PATH");
  static QStrList clangOptions = Config_getList("CLANG_OPTIONS");
  if (!clangAssistedParsing) return;
  //printf("ClangParser::start(%s)\n",fileName);
  p->fileName = fileName;
  p->index    = clang_createIndex(0, 0);
  p->curLine  = 1;
  p->curToken = 0;
  char **argv = (char**)malloc(sizeof(char*)*(4+Doxygen::inputPaths.count()+includePath.count()+clangOptions.count()));
  QDictIterator<void> di(Doxygen::inputPaths);
  int argc=0;
  // add include paths for input files
  for (di.toFirst();di.current();++di,++argc)
  {
    QCString inc = QCString("-I")+di.currentKey();
    argv[argc]=strdup(inc.data());
    //printf("argv[%d]=%s\n",argc,argv[argc]);
  }
  // add external include paths
  for (uint i=0;i<includePath.count();i++)
  {
    QCString inc = QCString("-I")+includePath.at(i);
    argv[argc++]=strdup(inc.data());
  }
  // user specified options
  for (uint i=0;i<clangOptions.count();i++)
  {
    argv[argc++]=strdup(clangOptions.at(i));
  }
  // extra options
  argv[argc++]=strdup("-ferror-limit=0");
  argv[argc++]=strdup("-x");

  // Since we can be presented with a .h file that can contain C/C++ or
  // Objective C code and we need to configure the parser before knowing this,
  // we use the source file to detected the language. Detection will fail if you
  // pass a bunch of .h files containing ObjC code, and no sources :-(
  SrcLangExt lang = getLanguageFromFileName(fileName);
  if (lang==SrcLangExt_ObjC || p->detectedLang!=ClangParser::Private::Detected_Cpp)
  {
    QCString fn = fileName;
    if (p->detectedLang==ClangParser::Private::Detected_Cpp && 
        (fn.right(4).lower()==".cpp" || fn.right(4).lower()==".cxx" ||
         fn.right(3).lower()==".cc" || fn.right(2).lower()==".c"))
    { // fall back to C/C++ once we see an extension that indicates this
      p->detectedLang = ClangParser::Private::Detected_Cpp;
    }
    else if (fn.right(3).lower()==".mm") // switch to Objective C++
    {
      p->detectedLang = ClangParser::Private::Detected_ObjCpp;
    }
    else if (fn.right(2).lower()==".m") // switch to Objective C
    {
      p->detectedLang = ClangParser::Private::Detected_ObjC;
    }
  }
  switch(p->detectedLang)
  {
    case ClangParser::Private::Detected_Cpp: 
      argv[argc++]=strdup("c++"); 
      break;
    case ClangParser::Private::Detected_ObjC: 
      argv[argc++]=strdup("objective-c"); 
      break;
    case ClangParser::Private::Detected_ObjCpp: 
      argv[argc++]=strdup("objective-c++"); 
      break;
  }

  // provide the input and and its dependencies as unsaved files so we can
  // pass the filtered versions
  argv[argc++]=strdup(fileName);
  static bool filterSourceFiles = Config_getBool("FILTER_SOURCE_FILES");
  //printf("source %s ----------\n%s\n-------------\n\n",
  //    fileName,p->source.data());
  uint numUnsavedFiles = filesInTranslationUnit.count()+1;
  p->numFiles = numUnsavedFiles;
  p->sources = new QCString[numUnsavedFiles];
  p->ufs     = new CXUnsavedFile[numUnsavedFiles];
  p->sources[0]      = detab(fileToString(fileName,filterSourceFiles,TRUE));
  p->ufs[0].Filename = strdup(fileName);
  p->ufs[0].Contents = p->sources[0].data();
  p->ufs[0].Length   = p->sources[0].length();
  QStrListIterator it(filesInTranslationUnit);
  uint i=1;
  for (it.toFirst();it.current() && i<numUnsavedFiles;++it,i++)
  {
    p->fileMapping.insert(it.current(),new uint(i));
    p->sources[i]      = detab(fileToString(it.current(),filterSourceFiles,TRUE));
    p->ufs[i].Filename = strdup(it.current());
    p->ufs[i].Contents = p->sources[i].data();
    p->ufs[i].Length   = p->sources[i].length();
  }

  // let libclang do the actual parsing
  p->tu = clang_parseTranslationUnit(p->index, 0,
                                     argv, argc, p->ufs, numUnsavedFiles, 
                                     CXTranslationUnit_DetailedPreprocessingRecord);
  // free arguments
  for (int i=0;i<argc;++i)
  {
    free(argv[i]);
  }
  free(argv);

  if (p->tu)
  {
    // filter out any includes not found by the clang parser
    determineInputFilesInSameTu(filesInTranslationUnit);

    // show any warnings that the compiler produced
    for (uint i=0, n=clang_getNumDiagnostics(p->tu); i!=n; ++i) 
    {
      CXDiagnostic diag = clang_getDiagnostic(p->tu, i); 
      CXString string = clang_formatDiagnostic(diag,
          clang_defaultDiagnosticDisplayOptions()); 
      err("%s [clang]\n",clang_getCString(string));
      clang_disposeString(string);
      clang_disposeDiagnostic(diag);
    }

    // create a source range for the given file
    QFileInfo fi(fileName);
    CXFile f = clang_getFile(p->tu, fileName);
    CXSourceLocation fileBegin = clang_getLocationForOffset(p->tu, f, 0);
    CXSourceLocation fileEnd   = clang_getLocationForOffset(p->tu, f, p->ufs[0].Length);
    CXSourceRange    fileRange = clang_getRange(fileBegin, fileEnd);

    // produce a token stream for the file
    clang_tokenize(p->tu,fileRange,&p->tokens,&p->numTokens);

    // produce cursors for each token in the stream
    p->cursors=new CXCursor[p->numTokens];
    clang_annotateTokens(p->tu,p->tokens,p->numTokens,p->cursors);
  }
  else
  {
    p->tokens    = 0;
    p->numTokens = 0;
    p->cursors   = 0;
    err("clang: Failed to parse translation unit %s\n",fileName);
  }
}

void ClangParser::switchToFile(const char *fileName)
{
  if (p->tu)
  {
    delete[] p->cursors;
    clang_disposeTokens(p->tu,p->tokens,p->numTokens);
    p->tokens    = 0;
    p->numTokens = 0;
    p->cursors   = 0;

    QFileInfo fi(fileName);
    CXFile f = clang_getFile(p->tu, fileName);
    uint *pIndex=p->fileMapping.find(fileName);
    if (pIndex && *pIndex<p->numFiles)
    {
      uint i=*pIndex;
      //printf("switchToFile %s: len=%ld\n",fileName,p->ufs[i].Length);
      CXSourceLocation fileBegin = clang_getLocationForOffset(p->tu, f, 0);
      CXSourceLocation fileEnd   = clang_getLocationForOffset(p->tu, f, p->ufs[i].Length);
      CXSourceRange    fileRange = clang_getRange(fileBegin, fileEnd);

      clang_tokenize(p->tu,fileRange,&p->tokens,&p->numTokens);
      p->cursors=new CXCursor[p->numTokens];
      clang_annotateTokens(p->tu,p->tokens,p->numTokens,p->cursors);

      p->curLine  = 1;
      p->curToken = 0;
    }
    else
    {
      err("clang: Failed to find input file %s in mapping\n",fileName);
    }
  }
}

void ClangParser::finish()
{
  static bool clangAssistedParsing = Config_getBool("CLANG_ASSISTED_PARSING");
  if (!clangAssistedParsing) return;
  if (p->tu)
  {
    //printf("ClangParser::finish()\n");
    delete[] p->cursors;
    clang_disposeTokens(p->tu,p->tokens,p->numTokens);
    clang_disposeTranslationUnit(p->tu);
    clang_disposeIndex(p->index);
    p->fileMapping.clear();
    p->tokens    = 0;
    p->numTokens = 0;
    p->cursors   = 0;
  }
  for (uint i=0;i<p->numFiles;i++)
  {
    free((void *)p->ufs[i].Filename);
  }
  delete[] p->ufs;
  delete[] p->sources;
  p->ufs       = 0;
  p->sources   = 0;
  p->numFiles  = 0;
  p->tu        = 0;
}

int ClangParser::Private::getCurrentTokenLine()
{
  uint l, c;
  if (numTokens==0) return 1;
  // guard against filters that reduce the number of lines
  if (curToken>=numTokens) curToken=numTokens-1;
  CXSourceLocation start = clang_getTokenLocation(tu,tokens[curToken]);
  clang_getSpellingLocation(start, 0, &l, &c, 0);
  return l;
}

QCString ClangParser::lookup(uint line,const char *symbol)
{
  //printf("ClangParser::lookup(%d,%s)\n",line,symbol);
  QCString result;
  if (symbol==0) return result;
  static bool clangAssistedParsing = Config_getBool("CLANG_ASSISTED_PARSING");
  if (!clangAssistedParsing) return result;

  int sl = strlen(symbol);
  uint l = p->getCurrentTokenLine();
  while (l>=line && p->curToken>0)
  {
    if (l==line) // already at the right line
    {
      p->curToken--; // linear search to start of the line
      l = p->getCurrentTokenLine();
    }
    else 
    {
      p->curToken/=2; // binary search backward
      l = p->getCurrentTokenLine();
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
        l = p->getCurrentTokenLine();
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
      l = p->getCurrentTokenLine();
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

static QCString keywordToType(const char *keyword)
{
  static bool init=TRUE;
  static QDict<void> flowKeywords(47);
  static QDict<void> typeKeywords(47);
  if (init)
  {
    flowKeywords.insert("break",(void*)0x8);
    flowKeywords.insert("case",(void*)0x8);
    flowKeywords.insert("catch",(void*)0x8);
    flowKeywords.insert("continue",(void*)0x8);
    flowKeywords.insert("default",(void*)0x8);
    flowKeywords.insert("do",(void*)0x8);
    flowKeywords.insert("else",(void*)0x8);
    flowKeywords.insert("finally",(void*)0x8);
    flowKeywords.insert("for",(void*)0x8);
    flowKeywords.insert("foreach",(void*)0x8);
    flowKeywords.insert("for each",(void*)0x8);
    flowKeywords.insert("goto",(void*)0x8);
    flowKeywords.insert("if",(void*)0x8);
    flowKeywords.insert("return",(void*)0x8);
    flowKeywords.insert("switch",(void*)0x8);
    flowKeywords.insert("throw",(void*)0x8);
    flowKeywords.insert("throws",(void*)0x8);
    flowKeywords.insert("try",(void*)0x8);
    flowKeywords.insert("while",(void*)0x8);
    flowKeywords.insert("@try",(void*)0x8);
    flowKeywords.insert("@catch",(void*)0x8);
    flowKeywords.insert("@finally",(void*)0x8);

    typeKeywords.insert("bool",(void*)0x8);
    typeKeywords.insert("char",(void*)0x8);
    typeKeywords.insert("double",(void*)0x8);
    typeKeywords.insert("float",(void*)0x8);
    typeKeywords.insert("int",(void*)0x8);
    typeKeywords.insert("long",(void*)0x8);
    typeKeywords.insert("object",(void*)0x8);
    typeKeywords.insert("short",(void*)0x8);
    typeKeywords.insert("signed",(void*)0x8);
    typeKeywords.insert("unsigned",(void*)0x8);
    typeKeywords.insert("void",(void*)0x8);
    typeKeywords.insert("wchar_t",(void*)0x8);
    typeKeywords.insert("size_t",(void*)0x8);
    typeKeywords.insert("boolean",(void*)0x8);
    typeKeywords.insert("id",(void*)0x8);
    typeKeywords.insert("SEL",(void*)0x8);
    typeKeywords.insert("string",(void*)0x8);
    typeKeywords.insert("nullptr",(void*)0x8);
    init=FALSE;
  }
  if (flowKeywords[keyword]) return "keywordflow";
  if (typeKeywords[keyword]) return "keywordtype";
  return "keyword";
}

static void writeLineNumber(CodeOutputInterface &ol,FileDef *fd,uint line)
{
  Definition *d = fd ? fd->getSourceDefinition(line) : 0;
  if (d && d->isLinkable())
  {
    g_currentDefinition=d;
    g_currentLine=line;
    MemberDef *md = fd->getSourceMember(line);
    if (md && md->isLinkable())  // link to member
    {
      if (g_currentMemberDef!=md) // new member, start search for body
      {
        g_searchForBody=TRUE;
        g_insideBody=FALSE;
        g_bracketCount=0;
      }
      g_currentMemberDef=md;
      ol.writeLineNumber(md->getReference(),
                         md->getOutputFileBase(),
                         md->anchor(),
                         line);
    }
    else // link to compound
    {
      g_currentMemberDef=0;
      ol.writeLineNumber(d->getReference(),
                         d->getOutputFileBase(),
                         d->anchor(),
                         line);
    }
  }
  else // no link
  {
    ol.writeLineNumber(0,0,0,line);
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

static void codifyLines(CodeOutputInterface &ol,FileDef *fd,const char *text,
                        uint &line,uint &column,const char *fontClass=0)
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
      writeLineNumber(ol,fd,line);
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

static void writeMultiLineCodeLink(CodeOutputInterface &ol,
                  FileDef *fd,uint &line,uint &column,
                  Definition *d,
                  const char *text)
{
  static bool sourceTooltips = Config_getBool("SOURCE_TOOLTIPS");
  TooltipManager::instance()->addTooltip(d);
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
      ol.writeCodeLink(ref,file,anchor,sp,tooltip);
      ol.endCodeLine();
      ol.startCodeLine(TRUE);
      writeLineNumber(ol,fd,line);
    }
    else
    {
      //printf("writeCodeLink(%s,%s,%s,%s)\n",ref,file,anchor,sp);
      ol.writeCodeLink(ref,file,anchor,sp,tooltip);
      done=TRUE;
    }
  }
}

void ClangParser::linkInclude(CodeOutputInterface &ol,FileDef *fd,
    uint &line,uint &column,const char *text)
{
  QCString incName = text;
  incName = incName.mid(1,incName.length()-2); // strip ".." or  <..>
  FileDef *ifd=0;
  if (!incName.isEmpty())
  {
    FileName *fn = Doxygen::inputNameDict->find(incName);
    if (fn)
    {
      bool found=false;
      FileNameIterator fni(*fn);
      // for each include name
      for (fni.toFirst();!found && (ifd=fni.current());++fni)
      {
        // see if this source file actually includes the file
        found = fd->isIncluded(ifd->absFilePath());
        //printf("      include file %s found=%d\n",ifd->absFilePath().data(),found);
      }
    }
  }
  if (ifd)
  {
    ol.writeCodeLink(ifd->getReference(),ifd->getOutputFileBase(),0,text,ifd->briefDescriptionAsTooltip());
  }
  else
  {
    codifyLines(ol,ifd,text,line,column,"preprocessor");
  }
}

void ClangParser::linkMacro(CodeOutputInterface &ol,FileDef *fd,
    uint &line,uint &column,const char *text)
{
  MemberName *mn=Doxygen::functionNameSDict->find(text);
  if (mn)
  {
    MemberNameIterator mni(*mn);
    MemberDef *md;
    for (mni.toFirst();(md=mni.current());++mni)
    {
      if (md->isDefine())
      {
        writeMultiLineCodeLink(ol,fd,line,column,md,text);
        return;
      }
    }
  }
  codifyLines(ol,fd,text,line,column);
}


void ClangParser::linkIdentifier(CodeOutputInterface &ol,FileDef *fd,
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

  Definition *d = usrStr ? Doxygen::clangUsrMap->find(usrStr) : 0;
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
    if (g_insideBody &&
        g_currentMemberDef && d->definitionType()==Definition::TypeMember && 
        (g_currentMemberDef!=d || g_currentLine<line)) // avoid self-reference
    {
      addDocCrossReference(g_currentMemberDef,(MemberDef*)d);
    }
    writeMultiLineCodeLink(ol,fd,line,column,d,text);
  }
  else
  {
    codifyLines(ol,fd,text,line,column);
  }
  clang_disposeString(usr);
}

static void detectFunctionBody(const char *s)
{
  //printf("punct=%s g_searchForBody=%d g_insideBody=%d g_bracketCount=%d\n",
  //  s,g_searchForBody,g_insideBody,g_bracketCount);

  if (g_searchForBody && (qstrcmp(s,":")==0 || qstrcmp(s,"{")==0)) // start of 'body' (: is for constructor)
  {
    g_searchForBody=FALSE;
    g_insideBody=TRUE;
  }
  else if (g_searchForBody && qstrcmp(s,";")==0) // declaration only
  {
    g_searchForBody=FALSE;
    g_insideBody=FALSE;
  }
  if (g_insideBody && qstrcmp(s,"{")==0) // increase scoping level
  {
    g_bracketCount++;
  }
  if (g_insideBody && qstrcmp(s,"}")==0) // decrease scoping level
  {
    g_bracketCount--;
    if (g_bracketCount<=0) // got outside of function body
    {
      g_insideBody=FALSE;
      g_bracketCount=0;
    }
  }
}

void ClangParser::writeSources(CodeOutputInterface &ol,FileDef *fd)
{
  TooltipManager::instance()->clearTooltips();
  // (re)set global parser state
  g_currentDefinition=0;
  g_currentMemberDef=0;
  g_currentLine=0;
  g_searchForBody=FALSE;
  g_insideBody=FALSE;
  g_bracketCount=0;

  unsigned int line=1,column=1;
  QCString lineNumber,lineAnchor;
  ol.startCodeLine(TRUE);
  writeLineNumber(ol,fd,line);
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
      writeLineNumber(ol,fd,line);
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
  TooltipManager::instance()->writeTooltips(ol);
}

ClangParser::ClangParser()
{
  p = new Private;
}

ClangParser::~ClangParser()
{
  delete p;
}

//--------------------------------------------------------------------------
#else // use stubbed functionality in case libclang support is disabled.

void ClangParser::start(const char *,QStrList &)
{
}

void ClangParser::switchToFile(const char *)
{
}

void ClangParser::finish()
{
}

QCString ClangParser::lookup(uint,const char *)
{
  return "";
}

void ClangParser::writeSources(CodeOutputInterface &,FileDef *)
{
}

ClangParser::ClangParser()
{
}

ClangParser::~ClangParser()
{
}


#endif
//--------------------------------------------------------------------------

