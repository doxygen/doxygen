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

#include <algorithm>
#include <iterator>
#include <unordered_map>
#include <string>
#include <optional>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cassert>

#include "anchor.h"
#include "md5.h"
#include "regex.h"
#include "config.h"
#include "definitionimpl.h"
#include "doxygen.h"
#include "language.h"
#include "message.h"
#include "portable.h"
#include "outputlist.h"
#include "code.h"
#include "util.h"
#include "groupdef.h"
#include "pagedef.h"
#include "section.h"
#include "htags.h"
#include "parserintf.h"
#include "debug.h"
#include "vhdldocgen.h"
#include "memberlist.h"
#include "namespacedef.h"
#include "filedef.h"
#include "dirdef.h"
#include "pagedef.h"
#include "reflist.h"
#include "utf8.h"
#include "indexlist.h"
#include "fileinfo.h"

//-----------------------------------------------------------------------------------------

/** Private data associated with a Symbol DefinitionImpl object. */
class DefinitionImpl::Private
{
  public:
    void init(const QCString &df, const QCString &n);
    void setDefFileName(const QCString &df);

    Definition *def = nullptr;

    SectionRefs sectionRefs;

    std::unordered_map<std::string,MemberDef *> sourceRefByDict;
    std::unordered_map<std::string,MemberDef *> sourceRefsDict;
    RefItemVector xrefListItems;
    RequirementRefs requirementRefs;
    GroupList partOfGroups;

    std::optional<DocInfo>   details;    // not exported
    std::optional<DocInfo>   inbodyDocs; // not exported
    std::optional<BriefInfo> brief;      // not exported
    std::optional<BodyInfo>  body;       // not exported

    QCString   briefSignatures;
    QCString   docSignatures;

    QCString localName;      // local (unqualified) name of the definition
                             // in the future m_name should become m_localName
    QCString qualifiedName;
    QCString ref;   // reference to external documentation

    bool hidden = FALSE;
    bool isArtificial = FALSE;
    bool isAnonymous = FALSE;
    bool isExported = FALSE;

    Definition *outerScope = nullptr;  // not owner

    // where the item was defined
    QCString defFileName;
    QCString defFileExt;

    SrcLangExt lang = SrcLangExt::Unknown;

    QCString id; // clang unique id

    QCString name;
    bool isSymbol;
    QCString symbolName;
    int defLine;
    int defColumn;

    MemberVector referencesMembers;    // cache for getReferencesMembers()
    MemberVector referencedByMembers;  // cache for getReferencedByMembers()
};


void DefinitionImpl::Private::setDefFileName(const QCString &df)
{
  defFileName = df;
  FileInfo fi(df.data());
  QCString ext = fi.extension(false);
  if (!ext.isEmpty()) defFileExt = "." + ext;
}

void DefinitionImpl::Private::init(const QCString &df, const QCString &n)
{
  setDefFileName(df);
  if (n!="<globalScope>")
  {
    //extractNamespaceName(m_name,m_localName,ns);
    localName=stripScope(n);
  }
  else
  {
    localName=n;
  }
  //printf("localName=%s\n",qPrint(localName));

  brief.reset();
  details.reset();
  body.reset();
  inbodyDocs.reset();
  sourceRefByDict.clear();
  sourceRefsDict.clear();
  requirementRefs.clear();
  outerScope      = Doxygen::globalScope;
  hidden          = FALSE;
  isArtificial    = FALSE;
  isExported      = FALSE;
  lang            = SrcLangExt::Unknown;
}

void DefinitionImpl::setDefFile(const QCString &df,int defLine,int defCol)
{
  p->setDefFileName(df);
  p->defLine = defLine;
  p->defColumn = defCol;
}

//-----------------------------------------------------------------------------------------

static bool matchExcludedSymbols(const QCString &name)
{
  const StringVector &exclSyms = Config_getList(EXCLUDE_SYMBOLS);
  if (exclSyms.empty()) return FALSE; // nothing specified
  const std::string &symName = name.str();
  for (const auto &pat : exclSyms)
  {
    QCString pattern = pat;
    bool forceStart=FALSE;
    bool forceEnd=FALSE;
    if (pattern.at(0)=='^')
    {
      pattern    = pattern.mid(1);
      forceStart = true;
    }
    if (pattern.at(pattern.length() - 1) == '$')
    {
      pattern  = pattern.left(pattern.length() - 1);
      forceEnd = true;
    }
    if (pattern.find('*')!=-1) // wildcard mode
    {
      const reg::Ex re(substitute(pattern,"*",".*").str());
      reg::Match match;
      if (reg::search(symName,match,re)) // wildcard match
      {
        size_t ui = match.position();
        size_t pl = match.length();
        size_t sl = symName.length();
        if ((ui==0     || pattern.at(0)=='*'                  || (!isId(symName.at(ui-1))  && !forceStart)) &&
            (ui+pl==sl || pattern.at(pattern.length()-1)=='*' || (!isId(symName.at(ui+pl)) && !forceEnd))
           )
        {
          //printf("--> name=%s pattern=%s match at %d\n",qPrint(symName),qPrint(pattern),i);
          return TRUE;
        }
      }
    }
    else if (!pattern.isEmpty()) // match words
    {
      size_t i = symName.find(pattern.str());
      if (i!=std::string::npos) // we have a match!
      {
        size_t ui=i;
        size_t pl=pattern.length();
        size_t sl=symName.length();
        // check if it is a whole word match
        if ((ui==0     || (!isId(symName.at(ui-1))  && !forceStart)) &&
            (ui+pl==sl || (!isId(symName.at(ui+pl)) && !forceEnd))
           )
        {
          //printf("--> name=%s pattern=%s match at %d\n",qPrint(symName),qPrint(pattern),i);
          return TRUE;
        }
      }
    }
  }
  //printf("--> name=%s: no match\n",name);
  return FALSE;
}

static void addToMap(const QCString &name,Definition *d)
{
  bool vhdlOpt = Config_getBool(OPTIMIZE_OUTPUT_VHDL);
  QCString symbolName = name;
  int index=computeQualifiedIndex(symbolName);
  if (!vhdlOpt && index!=-1) symbolName=symbolName.mid(index+2);
  if (!symbolName.isEmpty())
  {
    //printf("adding symbol %s\n",qPrint(symbolName));
    Doxygen::symbolMap->add(symbolName,d);

    d->_setSymbolName(symbolName);
  }
}

static void removeFromMap(const QCString &name,Definition *d)
{
  Doxygen::symbolMap->remove(name,d);
}

DefinitionImpl::DefinitionImpl(Definition *def,
                       const QCString &df,int dl,int dc,
                       const QCString &name,const char *b,
                       const char *d,bool isSymbol)
  : p(std::make_unique<Private>())
{
  setName(name);
  p->def = def;
  p->defLine = dl;
  p->defColumn = dc;
  p->init(df,name);
  p->isSymbol = isSymbol;
  if (isSymbol) addToMap(name,def);
  _setBriefDescription(b,df,dl);
  _setDocumentation(d,df,dl,TRUE,FALSE);
  if (matchExcludedSymbols(name))
  {
    p->hidden = TRUE;
  }
}

DefinitionImpl::DefinitionImpl(const DefinitionImpl &d)
  : p(std::make_unique<Private>(*d.p))
{
  if (p->isSymbol) addToMap(p->name,p->def);
}

DefinitionImpl &DefinitionImpl::operator=(const DefinitionImpl &other)
{
  if (this!=&other)
  {
    p = std::make_unique<Private>(*other.p);
  }
  return *this;
}

DefinitionImpl::~DefinitionImpl()
{
  if (p->isSymbol)
  {
    removeFromMap(p->symbolName,p->def);
  }
}

void DefinitionImpl::setName(const QCString &name)
{
  if (name.isEmpty()) return;
  p->name = name;
  p->isAnonymous = p->name.isEmpty() ||
                        p->name.at(0)=='@' ||
                        p->name.find("::@")!=-1;
}

void DefinitionImpl::setId(const QCString &id)
{
  if (id.isEmpty()) return;
  p->id = id;
  if (Doxygen::clangUsrMap)
  {
    //printf("DefinitionImpl::setId '%s'->'%s'\n",id,qPrint(p->name));
    Doxygen::clangUsrMap->emplace(id.str(),p->def);
  }
}

QCString DefinitionImpl::id() const
{
  return p->id;
}

void DefinitionImpl::addSectionsToDefinition(const std::vector<const SectionInfo*> &anchorList)
{
  //printf("%s: addSectionsToDefinition(%d)\n",qPrint(name()),anchorList->count());
  for (const SectionInfo *si : anchorList)
  {
    //printf("Add section '%s' to definition '%s'\n",
    //    qPrint(si->label()),qPrint(name()));
    SectionManager &sm = SectionManager::instance();
    SectionInfo *gsi=sm.find(si->label());
    //printf("===== label=%s gsi=%p\n",qPrint(si->label()),(void*)gsi);
    if (gsi==nullptr)
    {
      gsi = sm.add(*si);
    }
    if (p->sectionRefs.find(gsi->label())==nullptr)
    {
      p->sectionRefs.add(gsi);
    }
    gsi->setDefinition(p->def);
  }
}

bool DefinitionImpl::hasSections() const
{
  //printf("DefinitionImpl::hasSections(%s) #sections=%zu\n",qPrint(name()), p->sectionRefs.size());
  if (p->sectionRefs.empty()) return FALSE;
  for (const SectionInfo *si : p->sectionRefs)
  {
    if (si->type().isSection())
    {
      return TRUE;
    }
  }
  return FALSE;
}

void DefinitionImpl::writeDocAnchorsToTagFile(TextStream &tagFile) const
{
  if (!p->sectionRefs.empty())
  {
    //printf("%s: writeDocAnchorsToTagFile(%d)\n",qPrint(name()),p->sectionRef.size());
    for (const SectionInfo *si : p->sectionRefs)
    {
      if (!si->generated() && si->ref().isEmpty() && !AnchorGenerator::instance().isGenerated(si->label().str()))
      {
        //printf("write an entry!\n");
        if (p->def->definitionType()==Definition::TypeMember) tagFile << "  ";
        QCString fn = si->fileName();
        addHtmlExtensionIfMissing(fn);
        tagFile << "    <docanchor file=\"" << fn << "\"";
        if (!si->title().isEmpty())
        {
          tagFile << " title=\"" << convertToXML(si->title()) << "\"";
        }
        tagFile << ">" << si->label() << "</docanchor>\n";
      }
    }
  }
}

bool DefinitionImpl::_docsAlreadyAdded(const QCString &doc,QCString &sigList)
{
  uint8_t md5_sig[16];
  char sigStr[33];
  // to avoid mismatches due to differences in indenting, we first remove
  // double whitespaces...
  QCString docStr = doc.simplifyWhiteSpace();
  MD5Buffer(docStr.data(),static_cast<unsigned int>(docStr.length()),md5_sig);
  MD5SigToString(md5_sig,sigStr);
  //printf("%s:_docsAlreadyAdded doc='%s' sig='%s' docSigs='%s'\n",
  //    qPrint(name()),qPrint(doc),qPrint(sigStr),qPrint(sigList));
  if (sigList.find(sigStr)==-1) // new docs, add signature to prevent re-adding it
  {
    sigList+=QCString(":")+sigStr;
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

void DefinitionImpl::_setDocumentation(const QCString &d,const QCString &docFile,int docLine,
                                   bool stripWhiteSpace,bool atTop)
{
  //printf("%s::setDocumentation(%s,%s,%d,%d)\n",qPrint(name()),d,docFile,docLine,stripWhiteSpace);
  if (d.isEmpty()) return;
  QCString doc = d;
  if (stripWhiteSpace)
  {
    doc = stripLeadingAndTrailingEmptyLines(doc,docLine);
  }
  else // don't strip whitespace
  {
    doc=d;
  }
  if (!_docsAlreadyAdded(doc,p->docSignatures))
  {
    //printf("setting docs for %s: '%s'\n",qPrint(name()),qPrint(m_doc));
    if (!p->details.has_value())
    {
      p->details = std::make_optional<DocInfo>();
    }
    DocInfo &details = p->details.value();
    if (details.doc.isEmpty()) // fresh detailed description
    {
      details.doc = doc;
    }
    else if (atTop) // another detailed description, append it to the start
    {
      details.doc = doc+"\n\n"+details.doc;
    }
    else // another detailed description, append it to the end
    {
      details.doc += "\n\n"+doc;
    }
    if (docLine!=-1) // store location if valid
    {
      details.file = docFile;
      details.line = docLine;
    }
    else
    {
      details.file = docFile;
      details.line = 1;
    }
  }
}

void DefinitionImpl::setDocumentation(const QCString &d,const QCString &docFile,int docLine,bool stripWhiteSpace)
{
  if (d.isEmpty()) return;
  _setDocumentation(d,docFile,docLine,stripWhiteSpace,FALSE);
}

void DefinitionImpl::_setBriefDescription(const QCString &b,const QCString &briefFile,int briefLine)
{
  QCString brief = b;
  brief = brief.stripWhiteSpace();
  brief = stripLeadingAndTrailingEmptyLines(brief,briefLine);
  brief = brief.stripWhiteSpace();
  if (brief.isEmpty()) return;
  size_t bl = brief.length();
  if (bl>0)
  {
    if (!theTranslator || theTranslator->needsPunctuation()) // add punctuation if needed
    {
      int c = brief.at(bl-1);
      switch(c)
      {
        case '.': case '!': case '?': case ':': break;
        default:
          if (isUTF8CharUpperCase(brief.str(),0) && !lastUTF8CharIsMultibyte(brief.str())) brief+='.';
          break;
      }
    }
  }

  if (!_docsAlreadyAdded(brief,p->briefSignatures))
  {
    if (p->brief && !p->brief->doc.isEmpty())
    {
       //printf("adding to details\n");
       _setDocumentation(brief,briefFile,briefLine,FALSE,TRUE);
    }
    else
    {
      //fprintf(stderr,"DefinitionImpl::setBriefDescription(%s,%s,%d)\n",b,briefFile,briefLine);
      if (!p->brief.has_value())
      {
        p->brief = std::make_optional<BriefInfo>();
      }
      BriefInfo &briefInfo = p->brief.value();
      briefInfo.doc=brief;
      if (briefLine!=-1)
      {
        briefInfo.file = briefFile;
        briefInfo.line = briefLine;
      }
      else
      {
        briefInfo.file = briefFile;
        briefInfo.line = 1;
      }
    }
  }
  else
  {
    //printf("do nothing!\n");
  }
}

void DefinitionImpl::setBriefDescription(const QCString &b,const QCString &briefFile,int briefLine)
{
  if (b.isEmpty()) return;
  _setBriefDescription(b,briefFile,briefLine);
}

void DefinitionImpl::_setInbodyDocumentation(const QCString &doc,const QCString &inbodyFile,int inbodyLine)
{
  if (!_docsAlreadyAdded(doc,p->docSignatures))
  {
    if (!p->inbodyDocs.has_value())
    {
      p->inbodyDocs = std::make_optional<DocInfo>();
    }
    DocInfo &inbodyDocs = p->inbodyDocs.value();
    if (inbodyDocs.doc.isEmpty()) // fresh inbody docs
    {
      inbodyDocs.doc  = doc;
      inbodyDocs.file = inbodyFile;
      inbodyDocs.line = inbodyLine;
    }
    else // another inbody documentation fragment, append this to the end
    {
      inbodyDocs.doc += QCString("\n\n")+doc;
    }
  }
}

void DefinitionImpl::setInbodyDocumentation(const QCString &d,const QCString &inbodyFile,int inbodyLine)
{
  if (d.isEmpty()) return;
  _setInbodyDocumentation(d,inbodyFile,inbodyLine);
}

//---------------------------------------

/*! Cache for storing the result of filtering a file */
class FilterCache
{
  private:
    struct FilterCacheItem
    {
      size_t filePos;
      size_t fileSize;
    };
    using LineOffsets = std::vector<size_t>;

  public:
    static FilterCache &instance();

    //! collects the part of file \a fileName starting at \a startLine and ending at \a endLine into
    //! buffer \a str. Applies filtering if FILTER_SOURCE_FILES is enabled and the file extension
    //! matches a filter. Caches file information so that subsequent extraction of blocks from
    //! the same file can be performed efficiently
    bool getFileContents(const QCString &fileName,size_t startLine,size_t endLine, std::string &str)
    {
      bool filterSourceFiles = Config_getBool(FILTER_SOURCE_FILES);
      QCString filter = getFileFilter(fileName,TRUE);
      bool usePipe = !filter.isEmpty() && filterSourceFiles;
      return usePipe ? getFileContentsPipe(fileName,filter,startLine,endLine,str)
                     : getFileContentsDisk(fileName,startLine,endLine,str);
    }
  private:
    bool getFileContentsPipe(const QCString &fileName,const QCString &filter,
                             size_t startLine,size_t endLine,std::string &str)
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      auto it = m_cache.find(fileName.str());
      if (it!=m_cache.end()) // cache hit: reuse stored result
      {
        lock.unlock();
        auto item = it->second;
        //printf("getFileContents(%s): cache hit\n",qPrint(fileName));
        // file already processed, get the results after filtering from the tmp file
        Debug::print(Debug::FilterOutput,0,"Reusing filter result for {} from {} at offset={} size={}\n",
               fileName,Doxygen::filterDBFileName,item.filePos,item.fileSize);

        auto it_off = m_lineOffsets.find(fileName.str());
        assert(it_off!=m_lineOffsets.end());
        auto [ startLineOffset, fragmentSize] = getFragmentLocation(it_off->second,startLine,endLine);
        //printf("%s: existing file [%zu-%zu]->[%zu-%zu] size=%zu\n",
        //    qPrint(fileName),startLine,endLine,startLineOffset,endLineOffset,fragmentSize);
        readFragmentFromFile(str, Doxygen::filterDBFileName.data(),
                             item.filePos+startLineOffset, fragmentSize);
        return true;
      }
      else // cache miss: filter active but file not previously processed
      {
        //printf("getFileContents(%s): cache miss\n",qPrint(fileName));
        // filter file
        QCString cmd=filter+" \""+fileName+"\"";
        Debug::print(Debug::ExtCmd,0,"Executing popen(`{}`)\n",cmd);
        FILE *f = Portable::popen(cmd,"r");
        if (f==nullptr)
        {
          // handle error
          err("Error opening filter pipe command '{}'\n",cmd);
          return false;
        }
        FILE *bf = Portable::fopen(Doxygen::filterDBFileName,"a+b");
        FilterCacheItem item;
        item.filePos = m_endPos;
        if (bf==nullptr)
        {
          // handle error
          err("Error opening filter database file {}\n",Doxygen::filterDBFileName);
          Portable::pclose(f);
          return false;
        }
        // append the filtered output to the database file
        size_t size=0;
        while (!feof(f))
        {
          const int blockSize = 4096;
          char buf[blockSize];
          size_t bytesRead = fread(buf,1,blockSize,f);
          size_t bytesWritten = fwrite(buf,1,bytesRead,bf);
          if (bytesRead!=bytesWritten)
          {
            // handle error
            err("Failed to write to filter database {}. Wrote {} out of {} bytes\n",
                Doxygen::filterDBFileName,bytesWritten,bytesRead);
            Portable::pclose(f);
            fclose(bf);
            return false;
          }
          size+=bytesWritten;
          str+=std::string_view(buf,bytesWritten);
        }
        item.fileSize = size;
        // add location entry to the dictionary
        m_cache.emplace(fileName.str(),item);
        Debug::print(Debug::FilterOutput,0,"Storing new filter result for {} in {} at offset={} size={}\n",
               fileName,Doxygen::filterDBFileName,item.filePos,item.fileSize);
        // update end of file position
        m_endPos += size;
        Portable::pclose(f);
        fclose(bf);

        // shrink buffer to [startLine..endLine] part
        shrinkBuffer(str,fileName,startLine,endLine);
      }
      return true;
    }

    //! reads the fragment start at \a startLine and ending at \a endLine from file \a fileName
    //! into buffer \a str
    bool getFileContentsDisk(const QCString &fileName,size_t startLine,size_t endLine,std::string &str)
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      // normal file
      //printf("getFileContents(%s): no filter\n",qPrint(fileName));
      auto it = m_lineOffsets.find(fileName.str());
      if (it == m_lineOffsets.end()) // new file
      {
        // read file completely into str buffer
        readFragmentFromFile(str,fileName,0);
        // shrink buffer to [startLine..endLine] part
        shrinkBuffer(str,fileName,startLine,endLine);
      }
      else // file already processed before
      {
        lock.unlock();
        auto [ startLineOffset, fragmentSize] = getFragmentLocation(it->second,startLine,endLine);
        //printf("%s: existing file [%zu-%zu] -> start=%zu size=%zu\n",
        //    qPrint(fileName),startLine,endLine,startLineOffset,fragmentSize);
        readFragmentFromFile(str,fileName,startLineOffset,fragmentSize);
      }
      return true;
    }

    //! computes the starting offset for each line for file \a fileName, whose contents should
    //! already be stored in buffer \a str.
    void compileLineOffsets(const QCString &fileName,const std::string &str)
    {
      // line 1 (index 0) is at offset 0
      auto it = m_lineOffsets.emplace(fileName.data(),LineOffsets{0}).first;
      const char *p=str.data();
      while (*p)
      {
        char c=0;
        while ((c=*p)!='\n' && c!=0) p++; // search until end of the line
        if (c!=0) p++;
        it->second.push_back(p-str.data());
      }
    }

    //! Returns the byte offset and size within a file of a fragment given the array of
    //! line offsets and the start and end line of the fragment.
    auto getFragmentLocation(const LineOffsets &lineOffsets,
                             size_t startLine,size_t endLine) -> std::tuple<size_t,size_t>
    {
      assert(startLine > 0);
      assert(startLine <= endLine);
      const size_t startLineOffset = lineOffsets[std::min(startLine-1,lineOffsets.size()-1)];
      const size_t endLineOffset   = lineOffsets[std::min(endLine,    lineOffsets.size()-1)];
      assert(startLineOffset <= endLineOffset);
      const size_t fragmentSize = endLineOffset-startLineOffset;
      return std::tie(startLineOffset,fragmentSize);
    }

    //! Shrinks buffer \a str which should hold the contents of \a fileName to the
    //! fragment starting a line \a startLine and ending at line \a endLine
    void shrinkBuffer(std::string &str,const QCString &fileName,size_t startLine,size_t endLine)
    {
      // compute offsets from start for each line
      compileLineOffsets(fileName,str);
      auto it = m_lineOffsets.find(fileName.str());
      assert(it!=m_lineOffsets.end());
      const LineOffsets &lineOffsets = it->second;
      auto [ startLineOffset, fragmentSize] = getFragmentLocation(lineOffsets,startLine,endLine);
      //printf("%s: new file [%zu-%zu]->[%zu-%zu] size=%zu\n",
      //    qPrint(fileName),startLine,endLine,startLineOffset,endLineOffset,fragmentSize);
      str.erase(0,startLineOffset);
      str.resize(fragmentSize);
    }

    //! Reads the fragment start at byte offset \a startOffset of file \a fileName into buffer \a str.
    //! Result will be a null terminated. If size==0 the whole file will be read and startOffset is ignored.
    //! If size>0, size bytes will be read.
    void readFragmentFromFile(std::string &str,const QCString &fileName,size_t startOffset,size_t size=0)
    {
      std::ifstream ifs = Portable::openInputStream(fileName,true,true);
      if (size==0) { startOffset=0; size = static_cast<size_t>(ifs.tellg()); }
      ifs.seekg(startOffset, std::ios::beg);
      str.resize(size);
      ifs.read(str.data(), size);
    }

    FilterCache() : m_endPos(0) { }
    std::unordered_map<std::string,FilterCacheItem> m_cache;
    std::unordered_map<std::string,LineOffsets> m_lineOffsets;
    std::mutex m_mutex;
    size_t m_endPos;
};

FilterCache &FilterCache::instance()
{
  static FilterCache theInstance;
  return theInstance;
}

//-----------------------------------------


/*! Reads a fragment of code from file \a fileName starting at
 * line \a startLine and ending at line \a endLine (inclusive). The fragment is
 * stored in \a result. If FALSE is returned the code fragment could not be
 * found.
 *
 * The file is scanned for a opening bracket ('{') from \a startLine onward
 * The line actually containing the bracket is returned via startLine.
 * The file is scanned for a closing bracket ('}') from \a endLine backward.
 * The line actually containing the bracket is returned via endLine.
 * Note that for VHDL code the bracket search is not done.
 */
bool readCodeFragment(const QCString &fileName,bool isMacro,
                      int &startLine,int &endLine,QCString &result)
{
  bool filterSourceFiles = Config_getBool(FILTER_SOURCE_FILES);
  QCString filter = getFileFilter(fileName,TRUE);
  bool usePipe = !filter.isEmpty() && filterSourceFiles;
  int tabSize = Config_getInt(TAB_SIZE);
  SrcLangExt lang = getLanguageFromFileName(fileName);
  const int blockSize = 4096;
  std::string str;
  FilterCache::instance().getFileContents(fileName,
                                          static_cast<size_t>(std::max(1,startLine)),
                                          static_cast<size_t>(std::max({1,startLine,endLine})),str);
  //printf("readCodeFragment(%s,startLine=%d,endLine=%d)=\n[[[\n%s]]]\n",qPrint(fileName),startLine,endLine,qPrint(str));

  bool found = lang==SrcLangExt::VHDL   ||
               lang==SrcLangExt::Python ||
               lang==SrcLangExt::Fortran ||
               isMacro;
               // for VHDL, Python, and Fortran no bracket search is possible
  char *p=str.data();
  if (p && *p)
  {
    char c=0;
    int col=0;
    int lineNr=startLine;
    // skip until the opening bracket or lonely : is found
    char cn=0;
    while (*p && !found)
    {
      int pc=0;
      while ((c=*p++)!='{' && c!=':' && c!='=' && c!=0)
      {
        //printf("parsing char '%c'\n",c);
        if (c=='\n')
        {
          lineNr++;
          col = 0;
        }
        else if (c=='\t')
        {
          col+=tabSize - (col%tabSize);
        }
        else if (pc=='/' && c=='/') // skip single line comment
        {
          while ((c=*p++)!='\n' && c!=0);
          if (c == '\n')
          {
            lineNr++;
            col = 0;
          }
        }
        else if (pc=='/' && c=='*') // skip C style comment
        {
          while (((c=*p++)!='/' || pc!='*') && c!=0)
          {
            if (c == '\n')
            {
              lineNr++;
              col = 0;
            }
            pc=c;
          }
        }
        else
        {
          col++;
        }
        pc = c;
      }
      if (c==':')
      {
        cn=*p++;
        if (cn!=':') found=TRUE;
      }
      else if (c=='=')
      {
        cn=*p++;
        if (cn=='>') // C# Expression body
        {
          found=TRUE;
        }
      }
      else if (c=='{')
      {
        found=TRUE;
      }
      else if (c==0)
      {
        break;
      }
    }
    //printf(" -> readCodeFragment(%s,%d,%d) lineNr=%d\n",fileName,startLine,endLine,lineNr);
    if (found)
    {
      // For code with more than one line,
      // fill the line with spaces until we are at the right column
      // so that the opening brace lines up with the closing brace
      if (endLine!=startLine)
      {
        QCString spaces;
        spaces.fill(' ',col);
        result+=spaces;
      }
      // copy until end of line
      if (c) result+=c;
      startLine=lineNr;
      if (c==':' || c=='=')
      {
        result+=cn;
        if (cn=='\n') lineNr++;
      }
      char lineStr[blockSize];
      do
      {
        //printf("reading line %d in range %d-%d\n",lineNr,startLine,endLine);
        int size_read=0;
        do
        {
          // read up to blockSize-1 non-zero characters
          int i=0;
          while ((c=*p) && i<blockSize-1)
          {
            lineStr[i++]=c;
            p++;
            if (c=='\n') break; // stop at end of the line
          }
          lineStr[i]=0;
          size_read=i;
          result+=lineStr; // append line to the output
        } while (size_read == (blockSize-1)); // append more if line does not fit in buffer
        lineNr++;
      } while (*p);

      // strip stuff after closing bracket
      int newLineIndex = result.findRev('\n');
      int braceIndex   = result.findRev('}');
      if (braceIndex > newLineIndex)
      {
        result.resize(static_cast<size_t>(braceIndex+1));
      }
      endLine=lineNr-1;
    }
    if (usePipe)
    {
      Debug::print(Debug::FilterOutput, 0, "Filter output\n");
      Debug::print(Debug::FilterOutput,0,"-------------\n{}\n-------------\n",result);
    }
  }
  QCString encoding = getEncoding(FileInfo(fileName.str()));
  if (encoding!="UTF-8")
  {
    std::string encBuf = result.str();
    bool ok = transcodeCharacterStringToUTF8(encBuf,encoding.data());
    if (ok)
    {
      result = encBuf;
    }
    else
    {
      err("failed to transcode characters in code fragment in file {} lines {} to {}, from input encoding {} to UTF-8\n",
          fileName,startLine,endLine,encoding);

    }
  }
  if (!result.isEmpty() && result.at(result.length()-1)!='\n') result += "\n";
  //printf("readCodeFragment(%d-%d)=%s\n",startLine,endLine,qPrint(result));
  return found;
}

QCString DefinitionImpl::getSourceFileBase() const
{
  ASSERT(p->def->definitionType()!=Definition::TypeFile); // file overloads this method
  QCString fn;
  bool sourceBrowser = Config_getBool(SOURCE_BROWSER);
  if (sourceBrowser &&
      p->body && p->body->startLine!=-1 && p->body->fileDef)
  {
    fn = p->body->fileDef->getSourceFileBase();
  }
  return fn;
}

QCString DefinitionImpl::getSourceAnchor() const
{
  const int maxAnchorStrLen = 20;
  char anchorStr[maxAnchorStrLen];
  anchorStr[0]='\0';
  if (p->body && p->body->startLine!=-1)
  {
    if (Htags::useHtags)
    {
      qsnprintf(anchorStr,maxAnchorStrLen,"L%d",p->body->defLine);
    }
    else
    {
      qsnprintf(anchorStr,maxAnchorStrLen,"l%05d",p->body->defLine);
    }
  }
  return anchorStr;
}

/*! Write a reference to the source code defining this definition */
void DefinitionImpl::writeSourceDef(OutputList &ol) const
{
  //printf("DefinitionImpl::writeSourceRef %d %p\n",bodyLine,bodyDef);
  QCString fn = getSourceFileBase();
  if (!fn.isEmpty())
  {
    QCString refText = theTranslator->trDefinedAtLineInSourceFile();
    int lineMarkerPos = refText.find("@0");
    int fileMarkerPos = refText.find("@1");
    if (lineMarkerPos!=-1 && fileMarkerPos!=-1) // should always pass this.
    {
      QCString lineStr;
      lineStr.sprintf("%d",p->body->defLine);
      QCString anchorStr = getSourceAnchor();
      ol.startParagraph("definition");
      if (lineMarkerPos<fileMarkerPos) // line marker before file marker
      {
        // write text left from linePos marker
        ol.parseText(refText.left(lineMarkerPos));
        ol.writeObjectLink(QCString(),fn,anchorStr,lineStr);
        // write text between markers
        ol.parseText(refText.mid(lineMarkerPos+2,fileMarkerPos-lineMarkerPos-2));
        // write file link
        ol.writeObjectLink(QCString(),fn,QCString(),p->body->fileDef->name());
        // write text right from file marker
        ol.parseText(refText.right(refText.length()-static_cast<size_t>(fileMarkerPos)-2));
      }
      else // file marker before line marker
      {
        // write text left from file marker
        ol.parseText(refText.left(fileMarkerPos));
        // write file link
        ol.writeObjectLink(QCString(),fn,QCString(),p->body->fileDef->name());
        // write text between markers
        ol.parseText(refText.mid(fileMarkerPos+2,lineMarkerPos-fileMarkerPos-2));
        // write line link
        ol.writeObjectLink(QCString(),fn,anchorStr,lineStr);
        // write text right from linePos marker
        ol.parseText(refText.right(refText.length()-static_cast<size_t>(lineMarkerPos)-2));
      }
      ol.endParagraph();
    }
    else
    {
      err("translation error: invalid markers in trDefinedAtLineInSourceFile()\n");
    }
  }
}

void DefinitionImpl::setBodySegment(int defLine, int bls,int ble)
{
  if (!p->body.has_value())
  {
    p->body = std::make_optional<BodyInfo>();
  }
  BodyInfo &body = p->body.value();
  body.defLine   = defLine;
  body.startLine = bls;
  body.endLine   = ble;
}

void DefinitionImpl::setBodyDef(const FileDef *fd)
{
  if (!p->body.has_value())
  {
    p->body = std::make_optional<BodyInfo>();
  }
  p->body.value().fileDef=fd;
}

bool DefinitionImpl::hasSources() const
{
  return p->body && p->body->startLine!=-1 &&
         p->body->endLine>=p->body->startLine &&
         p->body->fileDef;
}

/*! Write code of this definition into the documentation */
void DefinitionImpl::writeInlineCode(OutputList &ol,const QCString &scopeName) const
{
  const MemberDef *thisMd = nullptr;
  if (p->def->definitionType()==Definition::TypeMember)
  {
    thisMd = toMemberDef(p->def);
  }
  bool inlineSources = thisMd && thisMd->hasInlineSource();
  //printf("Source Fragment %s: %d-%d\n",qPrint(name()),
  //        p->body->startLine,p->body->endLine);
  if (inlineSources && hasSources())
  {
    ol.pushGeneratorState();
    QCString codeFragment;
    bool isMacro = thisMd && thisMd->memberType()==MemberType::Define;
    int actualStart=p->body->startLine,actualEnd=p->body->endLine;
    if (readCodeFragment(p->body->fileDef->absFilePath(),isMacro,
          actualStart,actualEnd,codeFragment)
       )
    {
      //printf("Adding code fragment '%s' ext='%s' range=%d-%d\n",
      //    qPrint(codeFragment),qPrint(p->defFileExt),actualStart,actualEnd);
      auto intf = Doxygen::parserManager->getCodeParser(p->defFileExt);
      intf->resetCodeParserState();
      //printf("Read:\n'%s'\n\n",qPrint(codeFragment));

      auto &codeOL = ol.codeGenerators();
      codeOL.startCodeFragment("DoxyCode");
      size_t indent = 0;
      intf->parseCode(codeOL,           // codeOutIntf
                      scopeName,        // scope
                      detab(codeFragment,indent), // input
                      p->lang,     // lang
                      Config_getBool(STRIP_CODE_COMMENTS),
                      CodeParserOptions()
                      .setFileDef(p->body->fileDef)
                      .setStartLine(actualStart)
                      .setEndLine(actualEnd)
                      .setInlineFragment(true)
                      .setMemberDef(thisMd)
                     );
      codeOL.endCodeFragment("DoxyCode");
    }
    ol.popGeneratorState();
  }
}

static inline MemberVector refMapToVector(const std::unordered_map<std::string,MemberDef *> &map)
{
  // convert map to a vector of values
  MemberVector result;
  std::transform(map.begin(),map.end(),      // iterate over map
                 std::back_inserter(result), // add results to vector
                 [](const auto &item)
                 { return item.second; }     // extract value to add from map Key,Value pair
                );
  // and sort it
  std::stable_sort(result.begin(),result.end(),
              [](const auto &m1,const auto &m2) { return genericCompareMembers(m1,m2)<0; });
  return result;
}

/*! Write a reference to the source code fragments in which this
 *  definition is used.
 */
void DefinitionImpl::_writeSourceRefList(OutputList &ol,const QCString &scopeName,
    const QCString &text,const std::unordered_map<std::string,MemberDef *> &membersMap,
    bool /*funcOnly*/) const
{
  if (!membersMap.empty())
  {
    auto members = refMapToVector(membersMap);

    auto replaceFunc = [this,&members,scopeName,&ol](size_t entryIndex)
    {
      bool sourceBrowser   = Config_getBool(SOURCE_BROWSER);
      bool refLinkSource   = Config_getBool(REFERENCES_LINK_SOURCE);
      const MemberDef *md=members[entryIndex];
      if (md)
      {
        QCString scope=md->getScopeString();
        QCString name=md->name();
        //printf("class=%p scope=%s scopeName=%s\n",md->getClassDef(),qPrint(scope),scopeName);
        if (!scope.isEmpty() && scope!=scopeName)
        {
          name.prepend(scope+getLanguageSpecificSeparator(p->lang));
        }
        if (!md->isObjCMethod() &&
            (md->isFunction() || md->isSlot() ||
             md->isPrototype() || md->isSignal()
            )
           )
        {
          name+="()";
        }
        if (sourceBrowser &&
            !(md->isLinkable() && !refLinkSource) &&
            md->getStartBodyLine()!=-1 &&
            md->getBodyDef()
           )
        {
          const int maxLineNrStr = 10;
          char anchorStr[maxLineNrStr];
          qsnprintf(anchorStr,maxLineNrStr,"l%05d",md->getStartBodyLine());
          //printf("Write object link to %s\n",qPrint(md->getBodyDef()->getSourceFileBase()));
          ol.writeObjectLink(QCString(),md->getBodyDef()->getSourceFileBase(),anchorStr,name);
        }
        else if (md->isLinkable())
        {
          ol.writeObjectLink(md->getReference(),
              md->getOutputFileBase(),
              md->anchor(),name);
        }
        else
        {
          ol.docify(name);
        }
      }
    };

    ol.startParagraph("reference");
    ol.parseText(text);
    ol.docify(" ");
    writeMarkerList(ol,
                    theTranslator->trWriteList(static_cast<int>(members.size())).str(),
                    members.size(),
                    replaceFunc);
    ol.writeString(".");
    ol.endParagraph();

  }
}

void DefinitionImpl::writeSourceReffedBy(OutputList &ol,const QCString &scopeName) const
{
  _writeSourceRefList(ol,scopeName,theTranslator->trReferencedBy(),p->sourceRefByDict,FALSE);
}

void DefinitionImpl::writeSourceRefs(OutputList &ol,const QCString &scopeName) const
{
  _writeSourceRefList(ol,scopeName,theTranslator->trReferences(),p->sourceRefsDict,TRUE);
}

void DefinitionImpl::writeRequirementRefs(OutputList &ol) const
{
  if (!Config_getBool(GENERATE_REQUIREMENTS)) return;
  auto writeRefsForType = [&ol](const RequirementRefs &refs,const char *parType,const QCString &text)
  {
    size_t num = refs.size();
    if (num>0)
    {
      ol.startParagraph(parType);
      ol.parseText(text);
      ol.docify(" ");
      writeMarkerList(ol,
                      theTranslator->trWriteList(static_cast<int>(num)).str(), num,
                      [&refs,&ol](size_t entryIndex) { RequirementManager::instance().writeRef(ol,refs[entryIndex]); }
                     );
      ol.writeString(".");
      ol.endParagraph();
    }
  };

  RequirementRefs satisfiesRefs;
  RequirementRefs verifiesRefs;
  splitRequirementRefs(p->requirementRefs,satisfiesRefs,verifiesRefs);
  writeRefsForType(satisfiesRefs,"satisfies",theTranslator->trSatisfies(satisfiesRefs.size()==1));
  writeRefsForType(verifiesRefs, "verifies", theTranslator->trVerifies(verifiesRefs.size()==1));
}

bool DefinitionImpl::hasSourceReffedBy() const
{
  return !p->sourceRefByDict.empty();
}

bool DefinitionImpl::hasSourceRefs() const
{
  return !p->sourceRefsDict.empty();
}

bool DefinitionImpl::hasRequirementRefs() const
{
  return !p->requirementRefs.empty();
}

bool DefinitionImpl::hasDocumentation() const
{
  bool extractAll    = Config_getBool(EXTRACT_ALL);
  //bool sourceBrowser = Config_getBool(SOURCE_BROWSER);
  bool hasDocs =
         (p->details    && !p->details->doc.isEmpty())    || // has detailed docs
         (p->brief      && !p->brief->doc.isEmpty())      || // has brief description
         (p->inbodyDocs && !p->inbodyDocs->doc.isEmpty()) || // has inbody docs
         extractAll //||                   // extract everything
  //       (sourceBrowser && p->body &&
  //        p->body->startLine!=-1 && p->body->fileDef)
         ; // link to definition
  return hasDocs;
}

bool DefinitionImpl::hasUserDocumentation() const
{
  bool hasDocs =
         (p->details    && !p->details->doc.isEmpty()) ||
         (p->brief      && !p->brief->doc.isEmpty())   ||
         (p->inbodyDocs && !p->inbodyDocs->doc.isEmpty());
  return hasDocs;
}

void DefinitionImpl::addSourceReferencedBy(MemberDef *md,const QCString &sourceRefName)
{
  if (md)
  {
    p->sourceRefByDict.emplace(sourceRefName.str(),md);
  }
}

void DefinitionImpl::addSourceReferences(MemberDef *md,const QCString &sourceRefName)
{
  if (md)
  {
    p->sourceRefsDict.emplace(sourceRefName.str(),md);
  }
}

const Definition *DefinitionImpl::findInnerCompound(const QCString &) const
{
  return nullptr;
}

void DefinitionImpl::addInnerCompound(Definition *)
{
  err("DefinitionImpl::addInnerCompound() called\n");
}

static std::recursive_mutex g_qualifiedNameMutex;

QCString DefinitionImpl::qualifiedName() const
{
  std::lock_guard<std::recursive_mutex> lock(g_qualifiedNameMutex);
  if (!p->qualifiedName.isEmpty())
  {
    return p->qualifiedName;
  }

  //printf("start %s::qualifiedName() localName=%s\n",qPrint(name()),qPrint(p->localName));
  if (p->outerScope==nullptr)
  {
    if (p->localName=="<globalScope>")
    {
      return "";
    }
    else
    {
      return p->localName;
    }
  }

  if (p->outerScope->name()=="<globalScope>")
  {
    p->qualifiedName = p->localName;
  }
  else
  {
    p->qualifiedName = p->outerScope->qualifiedName()+
           getLanguageSpecificSeparator(getLanguage())+
           p->localName;
  }
  //printf("end %s::qualifiedName()=%s\n",qPrint(name()),qPrint(p->qualifiedName));
  //count--;
  return p->qualifiedName;
}

void DefinitionImpl::setOuterScope(Definition *d)
{
  std::lock_guard<std::recursive_mutex> lock(g_qualifiedNameMutex);
  //printf("%s::setOuterScope(%s)\n",qPrint(name()),d?qPrint(d->name()):"<none>");
  Definition *outerScope = p->outerScope;
  bool found=false;
  // make sure that we are not creating a recursive scope relation.
  while (outerScope && !found)
  {
    found = (outerScope==d);
    outerScope = outerScope->getOuterScope();
  }
  if (!found)
  {
    p->qualifiedName.clear(); // flush cached scope name
    p->outerScope = d;
  }
  p->hidden = p->hidden || d->isHidden();
  assert(p->def!=p->outerScope);
}

const QCString &DefinitionImpl::localName() const
{
  return p->localName;
}

void DefinitionImpl::makePartOfGroup(GroupDef *gd)
{
  p->partOfGroups.push_back(gd);
}

void DefinitionImpl::setRefItems(const RefItemVector &sli)
{
  p->xrefListItems.insert(p->xrefListItems.end(), sli.cbegin(), sli.cend());
}

void DefinitionImpl::setRequirementReferences(const RequirementRefs &rqli)
{
  p->requirementRefs.insert(p->requirementRefs.end(), rqli.cbegin(), rqli.cend());
}

void DefinitionImpl::mergeRefItems(Definition *d)
{
  auto otherXrefList = d->xrefListItems();

  // append vectors
  p->xrefListItems.reserve(p->xrefListItems.size()+otherXrefList.size());
  p->xrefListItems.insert (p->xrefListItems.end(),
                                otherXrefList.begin(),otherXrefList.end());

  // sort results on itemId
  std::stable_sort(p->xrefListItems.begin(),p->xrefListItems.end(),
            [](RefItem *left,RefItem *right)
            { return  left->id() <right->id() ||
                     (left->id()==right->id() &&
                      left->list()->listName() < right->list()->listName());
            });

  // filter out duplicates
  auto last = std::unique(p->xrefListItems.begin(),p->xrefListItems.end(),
            [](const RefItem *left,const RefItem *right)
            { return left->id()==right->id() &&
                     left->list()->listName()==right->list()->listName();
            });
  p->xrefListItems.erase(last, p->xrefListItems.end());
}

int DefinitionImpl::_getXRefListId(const QCString &listName) const
{
  for (const RefItem *item : p->xrefListItems)
  {
    if (item->list()->listName()==listName)
    {
      return item->id();
    }
  }
  return -1;
}

const RefItemVector &DefinitionImpl::xrefListItems() const
{
  return p->xrefListItems;
}

const RequirementRefs &DefinitionImpl::requirementReferences() const
{
  return p->requirementRefs;
}

QCString DefinitionImpl::pathFragment() const
{
  QCString result;
  if (p->outerScope && p->outerScope!=Doxygen::globalScope)
  {
    result = p->outerScope->pathFragment();
  }
  if (p->def->isLinkable())
  {
    if (!result.isEmpty()) result+="/";
    if (p->def->definitionType()==Definition::TypeGroup &&
        !toGroupDef(p->def)->groupTitle().isEmpty())
    {
      result+=toGroupDef(p->def)->groupTitle();
    }
    else if (p->def->definitionType()==Definition::TypePage &&
        toPageDef(p->def)->hasTitle())
    {
      result+=toPageDef(p->def)->title();
    }
    else
    {
      result+=p->localName;
    }
  }
  else
  {
    result+=p->localName;
  }
  return result;
}

//----------------------------------------------------------------------------------------

// TODO: move to htmlgen
/*! Returns the string used in the footer for $navpath when
 *  GENERATE_TREEVIEW is enabled
 */
QCString DefinitionImpl::navigationPathAsString() const
{
  QCString result;
  Definition *outerScope = getOuterScope();
  QCString locName = localName();
  if (outerScope && outerScope!=Doxygen::globalScope)
  {
    result+=outerScope->navigationPathAsString();
  }
  else if (p->def->definitionType()==Definition::TypeFile &&
           toFileDef(p->def)->getDirDef())
  {
    result+=(toFileDef(p->def))->getDirDef()->navigationPathAsString();
  }
  result+="<li class=\"navelem\">";
  if (p->def->isLinkableInProject())
  {
    QCString fn = p->def->getOutputFileBase();
    addHtmlExtensionIfMissing(fn);
    if (p->def->definitionType()==Definition::TypeGroup &&
        !toGroupDef(p->def)->groupTitle().isEmpty())
    {
      QCString title = parseCommentAsHtml(p->def,nullptr,toGroupDef(p->def)->groupTitle(),
                                          p->def->getDefFileName(),p->def->getDefLine());
      result+="<a href=\"$relpath^"+fn+"\">"+title+"</a>";
    }
    else if (p->def->definitionType()==Definition::TypePage &&
             toPageDef(p->def)->hasTitle())
    {
      QCString title = parseCommentAsHtml(p->def,nullptr,toPageDef(p->def)->title(),
                                          p->def->getDefFileName(),p->def->getDefLine());
      result+="<a href=\"$relpath^"+fn+"\">"+title+"</a>";
    }
    else if (p->def->definitionType()==Definition::TypeClass)
    {
      QCString name = toClassDef(p->def)->className();
      if (name.endsWith("-p"))
      {
        name = name.left(name.length()-2);
      }
      result+="<a href=\"$relpath^"+fn;
      if (!p->def->anchor().isEmpty()) result+="#"+p->def->anchor();
      result+="\">"+convertToHtml(name)+"</a>";
    }
    else
    {
      result+="<a href=\"$relpath^"+fn+"\">"+
              convertToHtml(locName)+"</a>";
    }
  }
  else
  {
    result+="<b>"+convertToHtml(locName)+"</b>";
  }
  result+="</li>";
  return result;
}

// TODO: move to htmlgen
void DefinitionImpl::writeNavigationPath(OutputList &ol) const
{
  ol.pushGeneratorState();
  ol.disableAllBut(OutputType::Html);

  QCString navPath;
  navPath += "<div id=\"nav-path\" class=\"navpath\">\n"
             "  <ul>\n";
  navPath += navigationPathAsString();
  navPath += "  </ul>\n"
             "</div>\n";
  ol.writeNavigationPath(navPath);

  ol.popGeneratorState();
}

void DefinitionImpl::writeToc(OutputList &ol, const LocalToc &localToc) const
{
  // first check if we have anything to show or if the outline is already shown on the outline panel
  if (p->sectionRefs.empty() || (Config_getBool(GENERATE_TREEVIEW) && Config_getBool(PAGE_OUTLINE_PANEL))) return;
  // generate the embedded toc
  //ol.writeLocalToc(p->sectionRefs,localToc);

  auto generateTocEntries = [this,&ol]()
  {
    for (const SectionInfo *si : p->sectionRefs)
    {
      if (si->type().isSection())
      {
        ol.startTocEntry(si);
        const MemberDef *md     = p->def->definitionType()==Definition::TypeMember ? toMemberDef(p->def) : nullptr;
        const Definition *scope = p->def->definitionType()==Definition::TypeMember ? p->def->getOuterScope() : p->def;
        QCString docTitle = si->title();
        if (docTitle.isEmpty()) docTitle = si->label();
        ol.generateDoc(docFile(),
                       getStartBodyLine(),
                       scope,
                       md,
                       docTitle,
                       DocOptions()
                       .setIndexWords(true)
                       .setSingleLine(true)
                       .setSectionLevel(si->type().level())
                      );
        ol.endTocEntry(si);
      }
    }
  };

  if (localToc.isHtmlEnabled())
  {
    ol.pushGeneratorState();
    ol.disableAllBut(OutputType::Html);
    ol.startLocalToc(localToc.htmlLevel());
    generateTocEntries();
    ol.endLocalToc();
    ol.popGeneratorState();
  }
  if (localToc.isDocbookEnabled())
  {
    ol.pushGeneratorState();
    ol.disableAllBut(OutputType::Docbook);
    ol.startLocalToc(localToc.docbookLevel());
    generateTocEntries();
    ol.endLocalToc();
    ol.popGeneratorState();
  }
  if (localToc.isLatexEnabled())
  {
    ol.pushGeneratorState();
    ol.disableAllBut(OutputType::Latex);
    ol.startLocalToc(localToc.latexLevel());
    // no gneerateTocEntries() needed for LaTeX
    ol.endLocalToc();
    ol.popGeneratorState();
  }
}

//----------------------------------------------------------------------------------------

const SectionRefs &DefinitionImpl::getSectionRefs() const
{
  return p->sectionRefs;
}

QCString DefinitionImpl::symbolName() const
{
  return p->symbolName;
}

//----------------------

QCString DefinitionImpl::documentation() const
{
  return p->details ? p->details->doc : QCString("");
}

int DefinitionImpl::docLine() const
{
  return p->details ? p->details->line : p->brief ? p->brief->line : 1;
}

QCString DefinitionImpl::docFile() const
{
  if (p->details && !p->details->file.isEmpty()) return p->details->file;
  else if (p->brief && !p->brief->file.isEmpty()) return p->brief->file;
  else  return "<" + p->name + ">";
}

//----------------------------------------------------------------------------
// strips w from s iff s starts with w
static bool stripWord(QCString &s,QCString w)
{
  bool success=FALSE;
  if (s.left(w.length())==w)
  {
    success=TRUE;
    s=s.right(s.length()-w.length());
  }
  return success;
}

//----------------------------------------------------------------------------
// some quasi intelligent brief description abbreviator :^)
static QCString abbreviate(const QCString &s,const QCString &name)
{
  QCString scopelessName=name;
  int i=scopelessName.findRev("::");
  if (i!=-1) scopelessName=scopelessName.mid(i+2);
  QCString result=s;
  result=result.stripWhiteSpace();
  // strip trailing .
  if (!result.isEmpty() && result.at(result.length()-1)=='.')
    result=result.left(result.length()-1);

  // strip any predefined prefix
  const StringVector &briefDescAbbrev = Config_getList(ABBREVIATE_BRIEF);
  for (const auto &p : briefDescAbbrev)
  {
    QCString str = substitute(p,"$name",scopelessName); // replace $name with entity name
    str += " ";
    stripWord(result,str);
  }

  // capitalize first character
  if (!result.isEmpty())
  {
    char c = result[0];
    if (c >= 'a' && c <= 'z') result[0] += 'A' - 'a';
  }

  return result;
}


//----------------------

QCString DefinitionImpl::briefDescription(bool abbr) const
{
  //printf("%s::briefDescription(%d)='%s'\n",qPrint(name()),abbr,p->brief?qPrint(p->brief->doc):"<none>");
  return p->brief ?
         (abbr ? abbreviate(p->brief->doc,p->def->displayName()) : p->brief->doc) :
         QCString("");
}

void DefinitionImpl::computeTooltip()
{
  if (p->brief && p->brief->tooltip.isEmpty() && !p->brief->doc.isEmpty())
  {
    const MemberDef *md = p->def->definitionType()==Definition::TypeMember ? toMemberDef(p->def) : nullptr;
    const Definition *scope = p->def->definitionType()==Definition::TypeMember ? p->def->getOuterScope() : p->def;
    p->brief->tooltip = parseCommentAsText(scope,md,
                                p->brief->doc, p->brief->file, p->brief->line);
  }
}

QCString DefinitionImpl::briefDescriptionAsTooltip() const
{
  return p->brief ? p->brief->tooltip : QCString();
}

int DefinitionImpl::briefLine() const
{
  return p->brief ? p->brief->line : 1;
}

QCString DefinitionImpl::briefFile() const
{
  return p->brief && !p->brief->file.isEmpty() ? p->brief->file : QCString("<"+p->name+">");
}

//----------------------

QCString DefinitionImpl::inbodyDocumentation() const
{
  return p->inbodyDocs ? p->inbodyDocs->doc : QCString("");
}

int DefinitionImpl::inbodyLine() const
{
  return p->inbodyDocs ? p->inbodyDocs->line : 1;
}

QCString DefinitionImpl::inbodyFile() const
{
  return p->inbodyDocs && !p->inbodyDocs->file.isEmpty() ? p->inbodyDocs->file : QCString("<"+p->name+">");
}


//----------------------

QCString DefinitionImpl::getDefFileName() const
{
  return p->defFileName;
}

QCString DefinitionImpl::getDefFileExtension() const
{
  return p->defFileExt;
}

bool DefinitionImpl::isHidden() const
{
  return p->hidden;
}

bool DefinitionImpl::isVisibleInProject() const
{
  return p->def->isLinkableInProject() && !p->hidden;
}

bool DefinitionImpl::isVisible() const
{
  return p->def->isLinkable() && !p->hidden;
}

bool DefinitionImpl::isArtificial() const
{
  return p->isArtificial;
}

bool DefinitionImpl::isExported() const
{
  return p->isExported;
}

QCString DefinitionImpl::getReference() const
{
  return p->ref;
}

bool DefinitionImpl::isReference() const
{
  return !p->ref.isEmpty();
}

int DefinitionImpl::getStartDefLine() const
{
  return p->body ? p->body->defLine : -1;
}

int DefinitionImpl::getStartBodyLine() const
{
  return p->body ? p->body->startLine : -1;
}

int DefinitionImpl::getEndBodyLine() const
{
  return p->body ? p->body->endLine : -1;
}

const FileDef *DefinitionImpl::getBodyDef() const
{
  return p->body ? p->body->fileDef : nullptr;
}

const GroupList &DefinitionImpl::partOfGroups() const
{
  return p->partOfGroups;
}

bool DefinitionImpl::isLinkableViaGroup() const
{
  for (const auto &gd : partOfGroups())
  {
    if (gd->isLinkable()) return true;
  }
  return false;
}

Definition *DefinitionImpl::getOuterScope() const
{
  return p->outerScope;
}

static std::mutex g_memberReferenceMutex;

const MemberVector &DefinitionImpl::getReferencesMembers() const
{
  std::lock_guard<std::mutex> lock(g_memberReferenceMutex);
  if (p->referencesMembers.empty() && !p->sourceRefsDict.empty())
  {
    p->referencesMembers = refMapToVector(p->sourceRefsDict);
  }
  return p->referencesMembers;
}

const MemberVector &DefinitionImpl::getReferencedByMembers() const
{
  std::lock_guard<std::mutex> lock(g_memberReferenceMutex);
  if (p->referencedByMembers.empty() && !p->sourceRefByDict.empty())
  {
    p->referencedByMembers = refMapToVector(p->sourceRefByDict);
  }
  return p->referencedByMembers;
}

void DefinitionImpl::mergeReferences(const Definition *other)
{
  const DefinitionImpl *defImpl = other->toDefinitionImpl_();
  if (defImpl)
  {
    for (const auto &kv : defImpl->p->sourceRefsDict)
    {
      auto it = p->sourceRefsDict.find(kv.first);
      if (it != p->sourceRefsDict.end())
      {
        p->sourceRefsDict.insert(kv);
      }
    }
  }
}

void DefinitionImpl::mergeReferencedBy(const Definition *other)
{
  const DefinitionImpl *defImpl = other->toDefinitionImpl_();
  if (defImpl)
  {
    for (const auto &kv : defImpl->p->sourceRefByDict)
    {
      auto it = p->sourceRefByDict.find(kv.first);
      if (it != p->sourceRefByDict.end())
      {
        p->sourceRefByDict.emplace(kv.first,kv.second);
      }
    }
  }
}


void DefinitionImpl::setReference(const QCString &r)
{
  p->ref=r;
}

SrcLangExt DefinitionImpl::getLanguage() const
{
  return p->lang;
}

void DefinitionImpl::setHidden(bool b)
{
  p->hidden = p->hidden || b;
}

void DefinitionImpl::setArtificial(bool b)
{
  p->isArtificial = b;
}

void DefinitionImpl::setExported(bool b)
{
  p->isExported = b;
}

void DefinitionImpl::setLocalName(const QCString &name)
{
  p->localName=name;
}

void DefinitionImpl::setLanguage(SrcLangExt lang)
{
  p->lang=lang;
}


void DefinitionImpl::_setSymbolName(const QCString &name)
{
  p->symbolName=name;
}

QCString DefinitionImpl::_symbolName() const
{
  return p->symbolName;
}

bool DefinitionImpl::hasBriefDescription() const
{
  bool briefMemberDesc = Config_getBool(BRIEF_MEMBER_DESC);
  return !briefDescription().isEmpty() && briefMemberDesc;
}

QCString DefinitionImpl::externalReference(const QCString &relPath) const
{
  QCString ref = getReference();
  if (!ref.isEmpty())
  {
    auto it = Doxygen::tagDestinationMap.find(ref.str());
    if (it!=Doxygen::tagDestinationMap.end())
    {
      QCString result(it->second);
      size_t l = result.length();
      if (!relPath.isEmpty() && l>0 && result.at(0)=='.')
      { // relative path -> prepend relPath.
        result.prepend(relPath);
        l+=relPath.length();
      }
      if (l>0 && result.at(l-1)!='/') result+='/';
      return result;
    }
  }
  return relPath;
}

const QCString &DefinitionImpl::name() const
{
  return p->name;
}

bool DefinitionImpl::isAnonymous() const
{
  return p->isAnonymous;
}

int DefinitionImpl::getDefLine() const
{
  return p->defLine;
}

int DefinitionImpl::getDefColumn() const
{
  return p->defColumn;
}

void DefinitionImpl::writeQuickMemberLinks(OutputList &,const MemberDef *) const
{
}

void DefinitionImpl::writeSummaryLinks(OutputList &) const
{
}

void DefinitionImpl::writePageNavigation(OutputList &) const
{
}

//---------------------------------------------------------------------------------

DefinitionAliasImpl::DefinitionAliasImpl(Definition *def,const Definition *scope, const Definition *alias)
      : m_def(def), m_scope(scope), m_symbolName(alias->_symbolName())
{
}

DefinitionAliasImpl::~DefinitionAliasImpl()
{
}

void DefinitionAliasImpl::init()
{
  //printf("%s::addToMap(%s)\n",qPrint(name()),qPrint(alias->name()));
  addToMap(m_symbolName,m_def);
  if (m_scope==nullptr)
  {
    m_qualifiedName = m_def->localName();
  }
  else
  {
    m_qualifiedName = m_scope->qualifiedName()+
      getLanguageSpecificSeparator(m_scope->getLanguage())+
      m_def->localName();
  }
}

void DefinitionAliasImpl::deinit()
{
  removeFromMap(m_symbolName,m_def);
}

QCString DefinitionAliasImpl::qualifiedName() const
{
  return m_qualifiedName;
}

const QCString &DefinitionAliasImpl::name() const
{
  return m_qualifiedName;
}

//---------------------------------------------------------------------------------

Definition *toDefinition(DefinitionMutable *dm)
{
  return dm ? dm->toDefinition_() : nullptr;
}

DefinitionMutable *toDefinitionMutable(Definition *d)
{
  return d ? d->toDefinitionMutable_() : nullptr;
}

