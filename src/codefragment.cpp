/******************************************************************************
 *
 * Copyright (C) 1997-2023 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby
 * granted. No representations are made about the suitability of this software
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * Documents produced by Doxygen are derivative works derived from the
 * input used in their production; they are not affected by this license.
 *
 */

#include <mutex>
#include <unordered_map>
#include <map>

#include "codefragment.h"
#include "util.h"
#include "doxygen.h"
#include "parserintf.h"
#include "outputlist.h"
#include "clangparser.h"
#include "trace.h"
#include "fileinfo.h"
#include "filedef.h"
#include "portable.h"
#include "message.h"

struct CodeFragmentManager::Private
{
  struct BlockMarker
  {
    int indent=0;
    std::string key;
    std::vector<int> lines;
  };

  struct FragmentInfo
  {
    QCString fileContents;
    QCString fileContentsTrimLeft;
    FragmentInfo() { recorderCodeList.add<OutputCodeRecorder>(); }
    void findBlockMarkers();
    OutputCodeList recorderCodeList;
    OutputCodeList recorderCodeListTrimLeft;
    std::map<int,BlockMarker> blocks;
    std::map<std::string,const BlockMarker*> blocksById;
    std::mutex mutex;
  };

  std::unordered_map<std::string,std::unique_ptr<FragmentInfo> > fragments;
  std::mutex mutex;
};

void CodeFragmentManager::Private::FragmentInfo::findBlockMarkers()
{
  AUTO_TRACE("findBlockMarkers() size={}",fileContents.size());
  // give fileContents and a list of candidate [XYZ] labels with/without trim left flag (from commentscan?)
  if (fileContents.length()==0) return;

  // find the potential snippet blocks (can also be other array like stuff in the file)
  const char *s=fileContents.data();
  int lineNr=1;
  char c=0;
  const char *foundOpen=nullptr;
  std::unordered_map<std::string,BlockMarker> candidates;
  while ((c=*s))
  {
    if (c=='[')
    {
      foundOpen=s;
    }
    else if (foundOpen && c==']' && foundOpen+1<s) // non-empty [...] section
    {
      std::string key(foundOpen+1,s-foundOpen-1);
      candidates[key].lines.push_back(lineNr);
    }
    else if (c=='\n')
    {
      foundOpen=nullptr;
      lineNr++;
    }
    s++;
  }

  // Sort the valid snippet blocks by line number, Look for blocks that appears twice,
  // where candidate has block id as key and the start and end line as value.
  // Store the key in marker.key
  for (auto &kv : candidates)
  {
    auto &marker = kv.second;
    if (marker.lines.size()==2 && marker.lines[0]+1<=marker.lines[1]-1)
    {
      marker.key = kv.first;
      int startLine = marker.lines[0];
      blocks[startLine] = marker;
      blocksById["["+kv.first+"]"] = &blocks[startLine];
    }
  }

  // determine the shared indentation for each line in each block, and store it in marker.indent
  s=fileContents.data();
  static auto gotoLine = [](const char *startBuf, const char *startPos, int startLine, int targetLine) -> const char *
  {
    char cc=0;
    if (targetLine<startLine)
    {
      //printf("gotoLine(pos=%p,start=%d,target=%d) backward\n",(void*)startPos,startLine,targetLine);
      while (startLine>=targetLine && startPos>=startBuf && (cc=*startPos--)) { if (cc=='\n') startLine--; }
      if (startPos>startBuf)
      {
        // given fragment:
        //         line1\n
        //         line2\n
        //         line3
        // and targetLine==2 then startPos ends at character '1' of line 1 before we detect that startLine<targetLine,
        // so we need to advance startPos with 2 to be at the start of line2, unless we are already at the first line.
        startPos+=2;
      }
      //printf("result=[%s]\n",qPrint(QCString(startPos).left(20)));
    }
    else
    {
      //printf("gotoLine(pos=%p,start=%d,target=%d) forward\n",(void*)startPos,startLine,targetLine);
      while (startLine<targetLine && (cc=*startPos++)) { if (cc=='\n') startLine++; }
      //printf("result=[%s]\n",qPrint(QCString(startPos).left(20)));
    }
    return startPos;
  };
  static auto lineIndent = [](const char *&ss) -> int
  {
    int tabSize=Config_getInt(TAB_SIZE);
    int col = 0;
    char cc = 0;
    while ((cc=*ss++))
    {
      if (cc==' ') col++;
      else if (cc=='\t') col+=tabSize-(col%tabSize);
      else
      {
        // goto end of the line
        while ((cc=*ss++) && cc!='\n');
        break;
      }
    }
    return col;
  };
  bool needsTrimLeft=false;
  lineNr=1;
  const char *startBuf = s;
  for (auto &kv : blocks)
  {
    auto &marker = kv.second;
    s = gotoLine(startBuf,s,lineNr,marker.lines[0]+1);
    lineNr=marker.lines[1];
    const char *e = gotoLine(startBuf,s,marker.lines[0]+1,lineNr);

    const char *ss = s;
    int minIndent=100000;
    while (ss<e)
    {
      int indent = lineIndent(ss);
      if (indent<minIndent)
      {
        minIndent=indent;
        if (minIndent==0) break;
      }
    }
    marker.indent = minIndent;
    if (minIndent>0) needsTrimLeft=true;

    AUTO_TRACE_ADD("found snippet key='{}' range=[{}..{}] indent={}",
        marker.key,
        marker.lines[0]+1,
        marker.lines[1]-1,
        marker.indent);
    s=e;
  }

  AUTO_TRACE_ADD("needsTrimLeft={}",needsTrimLeft);
  // create trimmed version of fileContents (if needed)
  if (needsTrimLeft)
  {
    fileContentsTrimLeft = fileContents;
    char *d = fileContentsTrimLeft.rawData();          // destination pointer
    startBuf = s = d;                                  // source pointer
    const char *e = s + fileContentsTrimLeft.length(); // end of string pointer
    lineNr=1;
    int tabSize=Config_getInt(TAB_SIZE);
    for (const auto &kv : blocks)
    {
      auto &marker = kv.second;
      if (marker.indent>0)
      {
        const char *ss = gotoLine(startBuf,s,lineNr,marker.lines[0]+1);
        while (s < ss) *d++=*s++; // copy until start of indented section

        // find position of the end of the marker
        const char *ee = gotoLine(startBuf,ss,marker.lines[0]+1,marker.lines[1]);
        lineNr=marker.lines[1];

        // process lines until the end of the indented block
        while (s < ee)
        {
          int col = 0;
          char cc = 0;
          // skip over indentation
          while (col < marker.indent && (cc=*s++))
          {
            if (cc==' ') col++;
            else if (cc=='\t') col+=tabSize-(col%tabSize);
          }
          // copy until the end of the line
          while ((cc=*s++) && cc!='\n') *d++=cc;
          if (cc) *d++=cc; // copy newline
        }
      }
    }
    // copy part after the last indented block
    while (s < e) *d++=*s++;
    // shrink the string for the indentation that was removed
    fileContentsTrimLeft.resize(d-fileContentsTrimLeft.data());
    //printf("result after trimming:\n=====%s=====\n",qPrint(fileContentsTrimLeft));
    recorderCodeListTrimLeft.add<OutputCodeRecorder>();
  }
}

CodeFragmentManager::CodeFragmentManager() : p(std::make_unique<Private>())
{
}

CodeFragmentManager::~CodeFragmentManager() = default;

CodeFragmentManager &CodeFragmentManager::instance()
{
  static CodeFragmentManager m;
  return m;
}

static QCString readTextFileByName(const QCString &file)
{
  AUTO_TRACE("file={}",file);
  if (Portable::isAbsolutePath(file))
  {
    FileInfo fi(file.str());
    if (fi.exists())
    {
      return fileToString(file,Config_getBool(FILTER_SOURCE_FILES));
    }
  }
  const StringVector &examplePathList = Config_getList(EXAMPLE_PATH);
  for (const auto &s : examplePathList)
  {
    std::string absFileName = s+(Portable::pathSeparator()+file).str();
    FileInfo fi(absFileName);
    if (fi.exists())
    {
      return fileToString(QCString(absFileName),Config_getBool(FILTER_SOURCE_FILES));
    }
  }

  // as a fallback we also look in the exampleNameDict
  bool ambig=false;
  FileDef *fd = findFileDef(Doxygen::exampleNameLinkedMap,file,ambig);
  if (fd)
  {
    if (ambig)
    {
      err("included file name '%s' is ambiguous.\nPossible candidates:\n%s\n",qPrint(file),
           qPrint(showFileDefMatches(Doxygen::exampleNameLinkedMap,file))
          );
    }
    return fileToString(fd->absFilePath(),Config_getBool(FILTER_SOURCE_FILES));
  }
  else
  {
    err("included file %s is not found. Check your EXAMPLE_PATH\n",qPrint(file));
  }
  return QCString();
}


void CodeFragmentManager::parseCodeFragment(OutputCodeList & codeOutList,
                                            const QCString & fileName,
                                            const QCString & blockId,
                                            const QCString & scopeName,
                                            bool             showLineNumbers,
                                            bool             trimLeft,
                                            bool             stripCodeComments
                                           )
{
  AUTO_TRACE("CodeFragmentManager::parseCodeFragment({},blockId={},scopeName={},showLineNumber={},trimLeft={}",
      fileName, blockId, scopeName, showLineNumbers, trimLeft);
  std::string fragmentKey=fileName.str()+":"+scopeName.str();
  std::unordered_map< std::string,std::unique_ptr<Private::FragmentInfo> >::iterator it;
  bool inserted = false;
  {
    // create new entry if it is not yet in the map
    std::lock_guard lock(p->mutex);
    it = p->fragments.find(fragmentKey);
    if (it == p->fragments.end())
    {
      it = p->fragments.emplace(fragmentKey, std::make_unique<Private::FragmentInfo>()).first;
      inserted = true;
      AUTO_TRACE_ADD("new fragment");
    }
  }
  // only lock the one item we are working with
  auto &codeFragment = it->second;
  std::lock_guard lock(codeFragment->mutex);
  if (inserted) // new entry, need to parse the file and record the output and cache it
  {


    SrcLangExt langExt = getLanguageFromFileName(fileName);
    FileInfo cfi( fileName.str() );
    auto fd = createFileDef( cfi.dirPath(), cfi.fileName() );
    auto intf = Doxygen::parserManager->getCodeParser(getFileNameExtension(fileName));
    intf->resetCodeParserState();
    bool filterSourceFiles = Config_getBool(FILTER_SOURCE_FILES);
    bool needs2PassParsing =
        Doxygen::parseSourcesNeeded &&                // we need to parse (filtered) sources for cross-references
        !filterSourceFiles &&                         // but user wants to show sources as-is
        !getFileFilter(fileName,TRUE).isEmpty();     // and there is a filter used while parsing
    codeFragment->fileContents = readTextFileByName(fileName);
    //printf("fileContents=[%s]\n",qPrint(codeFragment->fileContents));
    if (needs2PassParsing)
    {
      OutputCodeList devNullList;
      devNullList.add<DevNullCodeGenerator>();
      intf->parseCode(devNullList,
                      scopeName,
                      codeFragment->fileContents,
                      langExt,
                      stripCodeComments, // actually not important here
                      false,
                      QCString()
                      );
    }
    codeFragment->findBlockMarkers();
    if (codeFragment->fileContents.length()>0) // parse the normal version
    {
      intf->parseCode(codeFragment->recorderCodeList,
          scopeName,
          codeFragment->fileContents,
          langExt,            // lang
          false,              // strip code comments (overruled before replaying)
          false,              // isExampleBlock
          QCString(),         // exampleName
          fd.get(),           // fileDef
          -1,                 // startLine
          -1,                 // endLine
          true,               // inlineFragment
          nullptr,            // memberDef
          true,               // showLineNumbers
          nullptr,            // searchCtx
          false               // collectXRefs
          );
    }
    if (codeFragment->fileContentsTrimLeft.length()>0) // also need to parse a version with trimmed indentation
    {
      intf->parseCode(codeFragment->recorderCodeListTrimLeft,
          scopeName,
          codeFragment->fileContentsTrimLeft,
          langExt,            // lang
          false,              // strip code comments (overruled before replaying)
          false,              // isExampleBlock
          QCString(),         // exampleName
          fd.get(),           // fileDef
          -1,                 // startLine
          -1,                 // endLine
          true,               // inlineFragment
          nullptr,            // memberDef
          true,               // showLineNumbers
          nullptr,            // searchCtx
          false               // collectXRefs
          );
    }
  }
  // use the recorded OutputCodeList from the cache to output a pre-recorded fragment
  auto blockKv = codeFragment->blocksById.find(blockId.str());
  if (blockKv != codeFragment->blocksById.end())
  {
    const auto &marker = blockKv->second;
    int startLine = marker->lines[0];
    int endLine   = marker->lines[1];
    AUTO_TRACE_ADD("replay(start={},end={}) fileContentsTrimLeft.empty()={}",startLine,endLine,codeFragment->fileContentsTrimLeft.isEmpty());
    if (!trimLeft || codeFragment->fileContentsTrimLeft.isEmpty()) // replay the normal version
    {
      auto recorder = codeFragment->recorderCodeList.get<OutputCodeRecorder>(OutputType::Recorder);
      recorder->stripCodeComments(stripCodeComments);
      recorder->replay(codeOutList,startLine+1,endLine,showLineNumbers);
    }
    else // replay the trimmed version
    {
      auto recorder = codeFragment->recorderCodeListTrimLeft.get<OutputCodeRecorder>(OutputType::Recorder);
      recorder->stripCodeComments(stripCodeComments);
      recorder->replay(codeOutList,startLine+1,endLine,showLineNumbers);
    }
  }
  else
  {
    AUTO_TRACE_ADD("block not found!");
  }
}

