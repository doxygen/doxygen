#include "clangparser.h"
#include "settings.h"
#include <stdio.h>

#if USE_LIBCLANG
#include <clang-c/Index.h>
#include <qfileinfo.h>
#include <stdlib.h>
#include "message.h"
#include "sortdict.h"
#include "outputlist.h"
#include "filedef.h"
#include "memberdef.h"
#include "doxygen.h"
#include "util.h"
#include "config.h"
#include "growbuf.h"
#include "membername.h"
#include "filename.h"
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
    int getCurrentTokenLine();
    CXIndex index;
    CXTranslationUnit tu;
    QCString fileName;
    CXToken *tokens;
    uint numTokens;
    CXCursor *cursors;
    uint curLine;
    uint curToken;
    CXUnsavedFile uf;
    QCString source;
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

static QStrList getClangOptions()
{
  static QCString clangOptions = Config_getString("CLANG_OPTIONS");
  int p=0,i;
  QStrList options;
  while ((i=clangOptions.find(' ',p))!=-1)
  {
    QCString opt = clangOptions.mid(p,i-p).stripWhiteSpace();
    if (!opt.isEmpty())
    {
      options.append(opt);
    }
    p=i+1;
  }
  QCString opt = clangOptions.right(clangOptions.length()-p).stripWhiteSpace();
  if (!opt.isEmpty())
  {
    options.append(opt);
  }
  return options;
}

#if 0
static void inclusionVisitor(CXFile includedFile,
                             CXSourceLocation* inclusionStack,
                             unsigned includeLen,
                             CXClientData clientData)
{
  const char *fileName = (const char *)clientData;
  CXString incFileName = clang_getFileName(includedFile);
  printf("--- file %s includes %s\n",fileName,clang_getCString(incFileName));
  clang_disposeString(incFileName);
}
#endif

void ClangParser::start(const char *fileName)
{
  static bool clangAssistedParsing = Config_getBool("CLANG_ASSISTED_PARSING");
  static QStrList &includePath = Config_getList("INCLUDE_PATH");
  static QStrList clangOptions = getClangOptions();
  if (!clangAssistedParsing) return;
  //printf("ClangParser::start(%s)\n",fileName);
  p->fileName = fileName;
  p->index    = clang_createIndex(0, 0);
  p->curLine  = 1;
  p->curToken = 0;
  char *argv[4+Doxygen::inputPaths.count()+includePath.count()+clangOptions.count()];
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
  argv[argc++]=strdup("-x"); // force C++
  argv[argc++]=strdup("c++"); 
  // the file name
  argv[argc++]=strdup(fileName);
  static bool filterSourceFiles = Config_getBool("FILTER_SOURCE_FILES");
  p->source = detab(fileToString(fileName,filterSourceFiles,TRUE));
  //printf("source %s ----------\n%s\n-------------\n\n",
  //    fileName,p->source.data());
  p->uf.Filename = strdup(fileName);
  p->uf.Contents = p->source.data();
  p->uf.Length   = p->source.length();
  p->tu = clang_parseTranslationUnit(p->index, 0,
                                     argv, argc, &p->uf, 1, 
                                     CXTranslationUnit_DetailedPreprocessingRecord);
  for (int i=0;i<argc;++i)
  {
    free(argv[i]);
  }

  if (p->tu)
  {
    //clang_getInclusions(p->tu,
    //                    inclusionVisitor,
    //                    (CXClientData)fileName
    //                   );
    for (uint i=0, n=clang_getNumDiagnostics(p->tu); i!=n; ++i) 
    {
      CXDiagnostic diag = clang_getDiagnostic(p->tu, i); 
      CXString string = clang_formatDiagnostic(diag,
          clang_defaultDiagnosticDisplayOptions()); 
      err("%s\n",clang_getCString(string));
      clang_disposeString(string);
    }
    QFileInfo fi(fileName);
    CXFile f = clang_getFile(p->tu, fileName);
    CXSourceLocation fileBegin = clang_getLocationForOffset(p->tu, f, 0);
    CXSourceLocation fileEnd   = clang_getLocationForOffset(p->tu, f, p->uf.Length);
    CXSourceRange    fileRange = clang_getRange(fileBegin, fileEnd);

    clang_tokenize(p->tu,fileRange,&p->tokens,&p->numTokens);
    p->cursors=new CXCursor[p->numTokens];
    clang_annotateTokens(p->tu,p->tokens,p->numTokens,p->cursors);
  }
  else
  {
    p->tokens    = 0;
    p->numTokens = 0;
    p->cursors   = 0;
    err("Failed to parse translation unit %s\n",fileName);
  }
}

void ClangParser::finish()
{
  static bool clangAssistedParsing = Config_getBool("CLANG_ASSISTED_PARSING");
  if (!clangAssistedParsing) return;
  //printf("ClangParser::finish()\n");
  delete[] p->cursors;
  clang_disposeTokens(p->tu,p->tokens,p->numTokens);
  clang_disposeTranslationUnit(p->tu);
  clang_disposeIndex(p->index);
  free((void *)p->uf.Filename);
  p->source.resize(0);
  p->uf.Contents = 0;
  p->uf.Filename = 0;
  p->uf.Contents = 0;
  p->tokens    = 0;
  p->numTokens = 0;
  p->cursors   = 0;
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

/** Looks for \a symbol which should be found at \a line and returns
 *  a Clang unique identifier for the symbol. 
 */
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

static void writeLineNumber(OutputList &ol,FileDef *fd,uint line)
{
  Definition *d = fd ? fd->getSourceDefinition(line) : 0;
  if (d && d->isLinkable())
  {
    MemberDef *md = fd->getSourceMember(line);
    if (md && md->isLinkable())  // link to member
    {
      ol.writeLineNumber(md->getReference(),
                         md->getOutputFileBase(),
                         md->anchor(),
                         line);
    }
    else // link to compound
    {
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
}

static void codifyLines(OutputList &ol,FileDef *fd,const char *text,
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

static void writeMultiLineCodeLink(OutputList &ol,
                  FileDef *fd,uint &line,uint &column,
                  const char *ref,const char *file,
                  const char *anchor,const char *text,
		  const char *tooltip)
{
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

void ClangParser::linkInclude(OutputList &ol,FileDef *fd,
    uint &line,uint &column,const char *text)
{
  QCString incName = text;
  incName = incName.mid(1,incName.length()-2); // strip ".." or  <..>
  FileDef *ifd=0;
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
  if (ifd)
  {
    ol.writeCodeLink(ifd->getReference(),ifd->getOutputFileBase(),0,text,ifd->briefDescriptionAsTooltip());
  }
  else
  {
    codifyLines(ol,ifd,text,line,column,"preprocessor");
  }
}

void ClangParser::linkMacro(OutputList &ol,FileDef *fd,
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
        writeMultiLineCodeLink(ol,
            fd,line,column,
            md->getReference(),
            md->getOutputFileBase(),
            md->anchor(),
            text,
            md->briefDescriptionAsTooltip()
            );
        return;
      }
    }
  }
  codifyLines(ol,fd,text,line,column);
}

void ClangParser::linkIdentifier(OutputList &ol,FileDef *fd,
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
    writeMultiLineCodeLink(ol,
        fd,line,column,
        d->getReference(),
        d->getOutputFileBase(),
        d->anchor(),
        text,
        d->briefDescriptionAsTooltip()
        );
  }
  else
  {
    codifyLines(ol,fd,text,line,column);
  }
  clang_disposeString(usr);
}

void ClangParser::writeSources(OutputList &ol,FileDef *fd)
{
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
        //case CXToken_Punctuation: return "CXToken_Punctation";
        //case CXToken_Identifier: return "CXToken_Indentifier";
      default: 
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
            if (tokenKind==CXToken_Identifier)
            {
              linkIdentifier(ol,fd,line,column,s,i);
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

void ClangParser::start(const char *)
{
}

void ClangParser::finish()
{
}

QCString ClangParser::lookup(uint,const char *)
{
  return "";
}

void ClangParser::writeSources(OutputList &,FileDef *)
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

