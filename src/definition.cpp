
#include <algorithm>
#include <iterator>
#include <unordered_map>
#include <string>

#include <ctype.h>
#include "md5.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
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
#include "bufstr.h"
#include "reflist.h"
#include "utf8.h"

//-----------------------------------------------------------------------------------------

/** Private data associated with a Symbol DefinitionImpl object. */
class DefinitionImpl::IMPL
{
  public:
   ~IMPL();
    void init(const QCString &df, const QCString &n);
    void setDefFileName(const QCString &df);

    Definition *def = 0;

    SectionRefs sectionRefs;

    std::unordered_map<std::string,const MemberDef *> sourceRefByDict;
    std::unordered_map<std::string,const MemberDef *> sourceRefsDict;
    RefItemVector xrefListItems;
    GroupList partOfGroups;

    DocInfo   *details = 0;    // not exported
    DocInfo   *inbodyDocs = 0; // not exported
    BriefInfo *brief = 0;      // not exported
    BodyInfo  *body = 0;       // not exported
    QCString   briefSignatures;
    QCString   docSignatures;

    QCString localName;      // local (unqualified) name of the definition
                             // in the future m_name should become m_localName
    QCString qualifiedName;
    QCString ref;   // reference to external documentation

    bool hidden = FALSE;
    bool isArtificial = FALSE;
    bool isAnonymous = FALSE;

    Definition *outerScope = 0;  // not owner

    // where the item was defined
    QCString defFileName;
    QCString defFileExt;

    SrcLangExt lang = SrcLangExt_Unknown;

    QCString id; // clang unique id

    QCString name;
    bool isSymbol;
    QCString symbolName;
    int defLine;
    int defColumn;

    mutable MemberVector referencesMembers;
    mutable MemberVector referencedByMembers;
};


DefinitionImpl::IMPL::~IMPL()
{
  delete brief;
  delete details;
  delete body;
  delete inbodyDocs;
}

void DefinitionImpl::IMPL::setDefFileName(const QCString &df)
{
  defFileName = df;
  int lastDot = defFileName.findRev('.');
  if (lastDot!=-1)
  {
    defFileExt = defFileName.mid(lastDot);
  }
}

void DefinitionImpl::IMPL::init(const QCString &df, const QCString &n)
{
  setDefFileName(df);
  QCString lname = n;
  if (lname!="<globalScope>")
  {
    //extractNamespaceName(m_name,m_localName,ns);
    localName=stripScope(n);
  }
  else
  {
    localName=n;
  }
  //printf("localName=%s\n",qPrint(localName));

  brief           = 0;
  details         = 0;
  body            = 0;
  inbodyDocs      = 0;
  sourceRefByDict.clear();
  sourceRefsDict.clear();
  outerScope      = Doxygen::globalScope;
  hidden          = FALSE;
  isArtificial    = FALSE;
  lang            = SrcLangExt_Unknown;
}

void DefinitionImpl::setDefFile(const QCString &df,int defLine,int defCol)
{
  m_impl->setDefFileName(df);
  m_impl->defLine = defLine;
  m_impl->defColumn = defCol;
}

//-----------------------------------------------------------------------------------------

static bool matchExcludedSymbols(const QCString &name)
{
  const StringVector &exclSyms = Config_getList(EXCLUDE_SYMBOLS);
  if (exclSyms.empty()) return FALSE; // nothing specified
  std::string symName = name.str();
  for (const auto &pat : exclSyms)
  {
    QCString pattern = pat.c_str();
    bool forceStart=FALSE;
    bool forceEnd=FALSE;
    if (pattern.at(0)=='^')
      pattern=pattern.mid(1),forceStart=TRUE;
    if (pattern.at(pattern.length()-1)=='$')
      pattern=pattern.left(pattern.length()-1),forceEnd=TRUE;
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
{
  m_impl = new DefinitionImpl::IMPL;
  setName(name);
  m_impl->def = def;
  m_impl->defLine = dl;
  m_impl->defColumn = dc;
  m_impl->init(df,name);
  m_impl->isSymbol = isSymbol;
  if (isSymbol) addToMap(name,def);
  _setBriefDescription(b,df,dl);
  _setDocumentation(d,df,dl,TRUE,FALSE);
  if (matchExcludedSymbols(name))
  {
    m_impl->hidden = TRUE;
  }
}

DefinitionImpl::DefinitionImpl(const DefinitionImpl &d)
{
  m_impl = new DefinitionImpl::IMPL;
  *m_impl = *d.m_impl;
  m_impl->brief = 0;
  m_impl->details = 0;
  m_impl->body = 0;
  m_impl->inbodyDocs = 0;
  if (d.m_impl->brief)
  {
    m_impl->brief = new BriefInfo(*d.m_impl->brief);
  }
  if (d.m_impl->details)
  {
    m_impl->details = new DocInfo(*d.m_impl->details);
  }
  if (d.m_impl->body)
  {
    m_impl->body = new BodyInfo(*d.m_impl->body);
  }
  if (d.m_impl->inbodyDocs)
  {
    m_impl->inbodyDocs = new DocInfo(*d.m_impl->inbodyDocs);
  }

  if (m_impl->isSymbol) addToMap(m_impl->name,m_impl->def);
}

DefinitionImpl::~DefinitionImpl()
{
  if (m_impl->isSymbol)
  {
    removeFromMap(m_impl->symbolName,m_impl->def);
  }
  delete m_impl;
  m_impl=0;
}

void DefinitionImpl::setName(const QCString &name)
{
  if (name.isEmpty()) return;
  m_impl->name = name;
  m_impl->isAnonymous = m_impl->name.isEmpty() ||
                        m_impl->name.at(0)=='@' ||
                        m_impl->name.find("::@")!=-1;
}

void DefinitionImpl::setId(const QCString &id)
{
  if (id.isEmpty()) return;
  m_impl->id = id;
  if (Doxygen::clangUsrMap)
  {
    //printf("DefinitionImpl::setId '%s'->'%s'\n",id,qPrint(m_impl->name));
    Doxygen::clangUsrMap->insert(std::make_pair(id.str(),m_impl->def));
  }
}

QCString DefinitionImpl::id() const
{
  return m_impl->id;
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
    //printf("===== label=%s gsi=%p\n",qPrint(si->label),gsi);
    if (gsi==0)
    {
      gsi = sm.add(*si);
    }
    if (m_impl->sectionRefs.find(gsi->label())==0)
    {
      m_impl->sectionRefs.add(gsi);
      gsi->setDefinition(m_impl->def);
    }
  }
}

bool DefinitionImpl::hasSections() const
{
  //printf("DefinitionImpl::hasSections(%s) #sections=%d\n",qPrint(name()),
  //    m_impl->sectionRefs.size());
  if (m_impl->sectionRefs.empty()) return FALSE;
  for (const SectionInfo *si : m_impl->sectionRefs)
  {
    if (isSection(si->type()))
    {
      return TRUE;
    }
  }
  return FALSE;
}

void DefinitionImpl::addSectionsToIndex()
{
  if (m_impl->sectionRefs.empty()) return;
  //printf("DefinitionImpl::addSectionsToIndex()\n");
  int level=1;
  for (auto it = m_impl->sectionRefs.begin(); it!=m_impl->sectionRefs.end(); ++it)
  {
    const SectionInfo *si = *it;
    SectionType type = si->type();
    if (isSection(type))
    {
      //printf("  level=%d title=%s\n",level,qPrint(si->title));
      int nextLevel = static_cast<int>(type);
      int i;
      if (nextLevel>level)
      {
        for (i=level;i<nextLevel;i++)
        {
          Doxygen::indexList->incContentsDepth();
        }
      }
      else if (nextLevel<level)
      {
        for (i=nextLevel;i<level;i++)
        {
          Doxygen::indexList->decContentsDepth();
        }
      }
      QCString title = si->title();
      if (title.isEmpty()) title = si->label();
      // determine if there is a next level inside this item
      auto it_next = std::next(it);
      bool isDir = (it_next!=m_impl->sectionRefs.end()) ?
                       (static_cast<int>((*it_next)->type()) > nextLevel) : FALSE;
      Doxygen::indexList->addContentsItem(isDir,title,
                                         getReference(),
                                         m_impl->def->getOutputFileBase(),
                                         si->label(),
                                         FALSE,
                                         TRUE);
      level = nextLevel;
    }
  }
  while (level>1)
  {
    Doxygen::indexList->decContentsDepth();
    level--;
  }
}

void DefinitionImpl::writeDocAnchorsToTagFile(TextStream &tagFile) const
{
  if (!m_impl->sectionRefs.empty())
  {
    //printf("%s: writeDocAnchorsToTagFile(%d)\n",qPrint(name()),m_impl->sectionRef.size());
    for (const SectionInfo *si : m_impl->sectionRefs)
    {
      if (!si->generated() && si->ref().isEmpty() && !si->label().startsWith("autotoc_md"))
      {
        //printf("write an entry!\n");
        if (m_impl->def->definitionType()==Definition::TypeMember) tagFile << "  ";
        tagFile << "    <docanchor file=\"" << addHtmlExtensionIfMissing(si->fileName()) << "\"";
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
  uchar md5_sig[16];
  char sigStr[33];
  // to avoid mismatches due to differences in indenting, we first remove
  // double whitespaces...
  QCString docStr = doc.simplifyWhiteSpace();
  MD5Buffer(docStr.data(),docStr.length(),md5_sig);
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
  if (!_docsAlreadyAdded(doc,m_impl->docSignatures))
  {
    //printf("setting docs for %s: '%s'\n",qPrint(name()),qPrint(m_doc));
    if (m_impl->details==0)
    {
      m_impl->details = new DocInfo;
    }
    if (m_impl->details->doc.isEmpty()) // fresh detailed description
    {
      m_impl->details->doc = doc;
    }
    else if (atTop) // another detailed description, append it to the start
    {
      m_impl->details->doc = doc+"\n\n"+m_impl->details->doc;
    }
    else // another detailed description, append it to the end
    {
      m_impl->details->doc += "\n\n"+doc;
    }
    if (docLine!=-1) // store location if valid
    {
      m_impl->details->file = docFile;
      m_impl->details->line = docLine;
    }
    else
    {
      m_impl->details->file = docFile;
      m_impl->details->line = 1;
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
  uint bl = brief.length();
  if (bl>0)
  {
    if (!theTranslator || theTranslator->needsPunctuation()) // add punctuation if needed
    {
      int c = brief.at(bl-1);
      switch(c)
      {
        case '.': case '!': case '?': case '>': case ':': case ')': break;
        default:
          if (isUTF8CharUpperCase(brief.str(),0) && !lastUTF8CharIsMultibyte(brief.str())) brief+='.';
          break;
      }
    }
  }

  if (!_docsAlreadyAdded(brief,m_impl->briefSignatures))
  {
    if (m_impl->brief && !m_impl->brief->doc.isEmpty())
    {
       //printf("adding to details\n");
       _setDocumentation(brief,briefFile,briefLine,FALSE,TRUE);
    }
    else
    {
      //fprintf(stderr,"DefinitionImpl::setBriefDescription(%s,%s,%d)\n",b,briefFile,briefLine);
      if (m_impl->brief==0)
      {
        m_impl->brief = new BriefInfo;
      }
      m_impl->brief->doc=brief;
      if (briefLine!=-1)
      {
        m_impl->brief->file = briefFile;
        m_impl->brief->line = briefLine;
      }
      else
      {
        m_impl->brief->file = briefFile;
        m_impl->brief->line = 1;
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
  if (m_impl->inbodyDocs==0)
  {
    m_impl->inbodyDocs = new DocInfo;
  }
  if (m_impl->inbodyDocs->doc.isEmpty()) // fresh inbody docs
  {
    m_impl->inbodyDocs->doc  = doc;
    m_impl->inbodyDocs->file = inbodyFile;
    m_impl->inbodyDocs->line = inbodyLine;
  }
  else // another inbody documentation fragment, append this to the end
  {
    m_impl->inbodyDocs->doc += QCString("\n\n")+doc;
  }
}

void DefinitionImpl::setInbodyDocumentation(const QCString &d,const QCString &inbodyFile,int inbodyLine)
{
  if (d.isEmpty()) return;
  _setInbodyDocumentation(d,inbodyFile,inbodyLine);
}

//---------------------------------------

struct FilterCacheItem
{
  portable_off_t filePos;
  size_t fileSize;
};

/*! Cache for storing the result of filtering a file */
class FilterCache
{
  public:
    FilterCache() : m_endPos(0) { }
    bool getFileContents(const QCString &fileName,BufStr &str)
    {
      bool filterSourceFiles = Config_getBool(FILTER_SOURCE_FILES);
      QCString filter = getFileFilter(fileName,TRUE);
      bool usePipe = !filter.isEmpty() && filterSourceFiles;
      FILE *f=0;
      const int blockSize = 4096;
      char buf[blockSize];
      auto it = m_cache.find(fileName.str());
      if (usePipe && it!=m_cache.end()) // cache hit: reuse stored result
      {
        auto item = it->second;
        //printf("getFileContents(%s): cache hit\n",qPrint(fileName));
        // file already processed, get the results after filtering from the tmp file
        Debug::print(Debug::FilterOutput,0,"Reusing filter result for %s from %s at offset=%lld size=%zu\n",
               qPrint(fileName),qPrint(Doxygen::filterDBFileName),item.filePos,item.fileSize);
        f = Portable::fopen(Doxygen::filterDBFileName,"rb");
        if (f)
        {
          bool success=TRUE;
          str.resize(static_cast<uint>(item.fileSize+1));
          if (Portable::fseek(f,item.filePos,SEEK_SET)==-1)
          {
            err("Failed to seek to position %d in filter database file %s\n",static_cast<int>(item.filePos),qPrint(Doxygen::filterDBFileName));
            success=FALSE;
          }
          if (success)
          {
            size_t numBytes = fread(str.data(),1,item.fileSize,f);
            if (numBytes!=item.fileSize)
            {
              err("Failed to read %zu bytes from position %d in filter database file %s: got %zu bytes\n",
                 item.fileSize,static_cast<int>(item.filePos),qPrint(Doxygen::filterDBFileName),numBytes);
              success=FALSE;
            }
          }
          str.addChar('\0');
          fclose(f);
          return success;
        }
        else
        {
          err("Failed to open filter database file %s\n",qPrint(Doxygen::filterDBFileName));
          return FALSE;
        }
      }
      else if (usePipe) // cache miss: filter active but file not previously processed
      {
        //printf("getFileContents(%s): cache miss\n",qPrint(fileName));
        // filter file
        QCString cmd=filter+" \""+fileName+"\"";
        Debug::print(Debug::ExtCmd,0,"Executing popen(`%s`)\n",qPrint(cmd));
        f = Portable::popen(cmd,"r");
        FILE *bf = Portable::fopen(Doxygen::filterDBFileName,"a+b");
        FilterCacheItem item;
        item.filePos = m_endPos;
        if (bf==0)
        {
          // handle error
          err("Error opening filter database file %s\n",qPrint(Doxygen::filterDBFileName));
          str.addChar('\0');
          Portable::pclose(f);
          return FALSE;
        }
        // append the filtered output to the database file
        size_t size=0;
        while (!feof(f))
        {
          size_t bytesRead = fread(buf,1,blockSize,f);
          size_t bytesWritten = fwrite(buf,1,bytesRead,bf);
          if (bytesRead!=bytesWritten)
          {
            // handle error
            err("Failed to write to filter database %s. Wrote %zu out of %zu bytes\n",
                qPrint(Doxygen::filterDBFileName),bytesWritten,bytesRead);
            str.addChar('\0');
            Portable::pclose(f);
            fclose(bf);
            return FALSE;
          }
          size+=bytesWritten;
          str.addArray(buf,static_cast<uint>(bytesWritten));
        }
        str.addChar('\0');
        item.fileSize = size;
        // add location entry to the dictionary
        m_cache.insert(std::make_pair(fileName.str(),item));
        Debug::print(Debug::FilterOutput,0,"Storing new filter result for %s in %s at offset=%lld size=%zu\n",
               qPrint(fileName),qPrint(Doxygen::filterDBFileName),item.filePos,item.fileSize);
        // update end of file position
        m_endPos += size;
        Portable::pclose(f);
        fclose(bf);
      }
      else // no filtering
      {
        // normal file
        //printf("getFileContents(%s): no filter\n",qPrint(fileName));
        f = Portable::fopen(fileName,"r");
        while (!feof(f))
        {
          size_t bytesRead = fread(buf,1,blockSize,f);
          str.addArray(buf,static_cast<uint>(bytesRead));
        }
        str.addChar('\0');
        fclose(f);
      }
      return TRUE;
    }
  private:
    std::unordered_map<std::string,FilterCacheItem> m_cache;
    portable_off_t m_endPos;
};

static FilterCache g_filterCache;

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
bool readCodeFragment(const QCString &fileName,
                      int &startLine,int &endLine,QCString &result)
{
  //printf("readCodeFragment(%s,startLine=%d,endLine=%d)\n",fileName,startLine,endLine);
  bool filterSourceFiles = Config_getBool(FILTER_SOURCE_FILES);
  QCString filter = getFileFilter(fileName,TRUE);
  bool usePipe = !filter.isEmpty() && filterSourceFiles;
  int tabSize = Config_getInt(TAB_SIZE);
  SrcLangExt lang = getLanguageFromFileName(fileName);
  const int blockSize = 4096;
  BufStr str(blockSize);
  g_filterCache.getFileContents(fileName,str);

  bool found = lang==SrcLangExt_VHDL   ||
               lang==SrcLangExt_Python ||
               lang==SrcLangExt_Fortran;
               // for VHDL, Python, and Fortran no bracket search is possible
  char *p=str.data();
  if (p)
  {
    char c=0;
    int col=0;
    int lineNr=1;
    // skip until the startLine has reached
    while (lineNr<startLine && *p)
    {
      while ((c=*p++)!='\n' && c!=0) /* skip */;
      lineNr++;
      if (found && c == '\n') c = '\0';
    }
    if (*p)
    {
      // skip until the opening bracket or lonely : is found
      char cn=0;
      while (lineNr<=endLine && *p && !found)
      {
        int pc=0;
        while ((c=*p++)!='{' && c!=':' && c!=0)
        {
          //printf("parsing char '%c'\n",c);
          if (c=='\n')
          {
            lineNr++,col=0;
          }
          else if (c=='\t')
          {
            col+=tabSize - (col%tabSize);
          }
          else if (pc=='/' && c=='/') // skip single line comment
          {
            while ((c=*p++)!='\n' && c!=0);
            if (c=='\n') lineNr++,col=0;
          }
          else if (pc=='/' && c=='*') // skip C style comment
          {
            while (((c=*p++)!='/' || pc!='*') && c!=0)
            {
              if (c=='\n') lineNr++,col=0;
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
        else if (c=='{')
        {
          found=TRUE;
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
        if (c==':')
        {
          result+=cn;
          if (cn=='\n') lineNr++;
        }
        char lineStr[blockSize];
        do
        {
          //printf("reading line %d in range %d-%d\n",lineNr,startLine,endLine);
          int size_read;
          do
          {
            // read up to maxLineLength-1 bytes, the last byte being zero
            int i=0;
            while ((c=*p++) && i<blockSize-1)
            {
              lineStr[i++]=c;
              if (c=='\n') break; // stop at end of the line
            }
            lineStr[i]=0;
            size_read=i;
            result+=lineStr; // append line to the output
          } while (size_read == (blockSize-1)); // append more if line does not fit in buffer
          lineNr++;
        } while (lineNr<=endLine && *p);

        // strip stuff after closing bracket
        int newLineIndex = result.findRev('\n');
        int braceIndex   = result.findRev('}');
        if (braceIndex > newLineIndex)
        {
          result.truncate(static_cast<size_t>(braceIndex+1));
        }
        endLine=lineNr-1;
      }
    }
    if (usePipe)
    {
      Debug::print(Debug::FilterOutput, 0, "Filter output\n");
      Debug::print(Debug::FilterOutput,0,"-------------\n%s\n-------------\n",qPrint(result));
    }
  }
  result = transcodeCharacterStringToUTF8(result);
  if (!result.isEmpty() && result.at(result.length()-1)!='\n') result += "\n";
  //printf("readCodeFragment(%d-%d)=%s\n",startLine,endLine,qPrint(result));
  return found;
}

QCString DefinitionImpl::getSourceFileBase() const
{
  ASSERT(m_impl->def->definitionType()!=Definition::TypeFile); // file overloads this method
  QCString fn;
  bool sourceBrowser = Config_getBool(SOURCE_BROWSER);
  if (sourceBrowser &&
      m_impl->body && m_impl->body->startLine!=-1 && m_impl->body->fileDef)
  {
    fn = m_impl->body->fileDef->getSourceFileBase();
  }
  return fn;
}

QCString DefinitionImpl::getSourceAnchor() const
{
  const int maxAnchorStrLen = 20;
  char anchorStr[maxAnchorStrLen];
  anchorStr[0]='\0';
  if (m_impl->body && m_impl->body->startLine!=-1)
  {
    if (Htags::useHtags)
    {
      qsnprintf(anchorStr,maxAnchorStrLen,"L%d",m_impl->body->defLine);
    }
    else
    {
      qsnprintf(anchorStr,maxAnchorStrLen,"l%05d",m_impl->body->defLine);
    }
  }
  return anchorStr;
}

/*! Write a reference to the source code defining this definition */
void DefinitionImpl::writeSourceDef(OutputList &ol,const QCString &) const
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
      lineStr.sprintf("%d",m_impl->body->defLine);
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
        ol.writeObjectLink(QCString(),fn,QCString(),m_impl->body->fileDef->name());
        // write text right from file marker
        ol.parseText(refText.right(refText.length()-static_cast<size_t>(fileMarkerPos)-2));
      }
      else // file marker before line marker
      {
        // write text left from file marker
        ol.parseText(refText.left(fileMarkerPos));
        // write file link
        ol.writeObjectLink(QCString(),fn,QCString(),m_impl->body->fileDef->name());
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
  //printf("setBodySegment(%d,%d) for %s\n",bls,ble,qPrint(name()));
  if (m_impl->body==0) m_impl->body = new BodyInfo;
  m_impl->body->defLine   = defLine;
  m_impl->body->startLine = bls;
  m_impl->body->endLine   = ble;
}

void DefinitionImpl::setBodyDef(const FileDef *fd)
{
  if (m_impl->body==0) m_impl->body = new BodyInfo;
  m_impl->body->fileDef=fd;
}

bool DefinitionImpl::hasSources() const
{
  return m_impl->body && m_impl->body->startLine!=-1 &&
         m_impl->body->endLine>=m_impl->body->startLine &&
         m_impl->body->fileDef;
}

/*! Write code of this definition into the documentation */
void DefinitionImpl::writeInlineCode(OutputList &ol,const QCString &scopeName) const
{
  bool inlineSources = Config_getBool(INLINE_SOURCES);
  ol.pushGeneratorState();
  //printf("Source Fragment %s: %d-%d bodyDef=%p\n",qPrint(name()),
  //        m_startBodyLine,m_endBodyLine,m_bodyDef);
  if (inlineSources && hasSources())
  {
    QCString codeFragment;
    int actualStart=m_impl->body->startLine,actualEnd=m_impl->body->endLine;
    if (readCodeFragment(m_impl->body->fileDef->absFilePath(),
          actualStart,actualEnd,codeFragment)
       )
    {
      //printf("Adding code fragment '%s' ext='%s'\n",
      //    qPrint(codeFragment),qPrint(m_impl->defFileExt));
      auto intf = Doxygen::parserManager->getCodeParser(m_impl->defFileExt);
      intf->resetCodeParserState();
      //printf("Read:\n'%s'\n\n",qPrint(codeFragment));
      const MemberDef *thisMd = 0;
      if (m_impl->def->definitionType()==Definition::TypeMember)
      {
        thisMd = toMemberDef(m_impl->def);
      }

      ol.startCodeFragment("DoxyCode");
      intf->parseCode(ol,               // codeOutIntf
                      scopeName,        // scope
                      codeFragment,     // input
                      m_impl->lang,     // lang
                      FALSE,            // isExample
                      QCString(),       // exampleName
                      m_impl->body->fileDef,  // fileDef
                      actualStart,      // startLine
                      actualEnd,        // endLine
                      TRUE,             // inlineFragment
                      thisMd,           // memberDef
                      TRUE              // show line numbers
                     );
      ol.endCodeFragment("DoxyCode");
    }
  }
  ol.popGeneratorState();
}

static inline MemberVector refMapToVector(const std::unordered_map<std::string,const MemberDef *> &map)
{
  // convert map to a vector of values
  MemberVector result;
  std::transform(map.begin(),map.end(),      // iterate over map
                 std::back_inserter(result), // add results to vector
                 [](const auto &item)
                 { return item.second; }     // extract value to add from map Key,Value pair
                );
  // and sort it
  std::sort(result.begin(),result.end(),
              [](const auto &m1,const auto &m2) { return genericCompareMembers(m1,m2)<0; });
  return result;
}

/*! Write a reference to the source code fragments in which this
 *  definition is used.
 */
void DefinitionImpl::_writeSourceRefList(OutputList &ol,const QCString &scopeName,
    const QCString &text,const std::unordered_map<std::string,const MemberDef *> &membersMap,
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
          name.prepend(scope+getLanguageSpecificSeparator(m_impl->lang));
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
  _writeSourceRefList(ol,scopeName,theTranslator->trReferencedBy(),m_impl->sourceRefByDict,FALSE);
}

void DefinitionImpl::writeSourceRefs(OutputList &ol,const QCString &scopeName) const
{
  _writeSourceRefList(ol,scopeName,theTranslator->trReferences(),m_impl->sourceRefsDict,TRUE);
}

bool DefinitionImpl::hasSourceReffedBy() const
{
  return !m_impl->sourceRefByDict.empty();
}

bool DefinitionImpl::hasSourceRefs() const
{
  return !m_impl->sourceRefsDict.empty();
}

bool DefinitionImpl::hasDocumentation() const
{
  bool extractAll    = Config_getBool(EXTRACT_ALL);
  //bool sourceBrowser = Config_getBool(SOURCE_BROWSER);
  bool hasDocs =
         (m_impl->details    && !m_impl->details->doc.isEmpty())    || // has detailed docs
         (m_impl->brief      && !m_impl->brief->doc.isEmpty())      || // has brief description
         (m_impl->inbodyDocs && !m_impl->inbodyDocs->doc.isEmpty()) || // has inbody docs
         extractAll //||                   // extract everything
  //       (sourceBrowser && m_impl->body &&
  //        m_impl->body->startLine!=-1 && m_impl->body->fileDef)
         ; // link to definition
  return hasDocs;
}

bool DefinitionImpl::hasUserDocumentation() const
{
  bool hasDocs =
         (m_impl->details    && !m_impl->details->doc.isEmpty()) ||
         (m_impl->brief      && !m_impl->brief->doc.isEmpty())   ||
         (m_impl->inbodyDocs && !m_impl->inbodyDocs->doc.isEmpty());
  return hasDocs;
}


void DefinitionImpl::addSourceReferencedBy(const MemberDef *md)
{
  if (md)
  {
    QCString name  = md->name();
    QCString scope = md->getScopeString();

    if (!scope.isEmpty())
    {
      name.prepend(scope+"::");
    }

    m_impl->sourceRefByDict.insert({name.str(),md});
  }
}

void DefinitionImpl::addSourceReferences(const MemberDef *md)
{
  if (md)
  {
    QCString name  = md->name();
    QCString scope = md->getScopeString();

    if (!scope.isEmpty())
    {
      name.prepend(scope+"::");
    }

    m_impl->sourceRefsDict.insert({name.str(),md});
  }
}

const Definition *DefinitionImpl::findInnerCompound(const QCString &) const
{
  return 0;
}

void DefinitionImpl::addInnerCompound(const Definition *)
{
  err("DefinitionImpl::addInnerCompound() called\n");
}

QCString DefinitionImpl::qualifiedName() const
{
  //static int count=0;
  //count++;
  if (!m_impl->qualifiedName.isEmpty())
  {
    //count--;
    return m_impl->qualifiedName;
  }

  //printf("start %s::qualifiedName() localName=%s\n",qPrint(name()),qPrint(m_impl->localName));
  if (m_impl->outerScope==0)
  {
    if (m_impl->localName=="<globalScope>")
    {
      //count--;
      return "";
    }
    else
    {
      //count--;
      return m_impl->localName;
    }
  }

  if (m_impl->outerScope->name()=="<globalScope>")
  {
    m_impl->qualifiedName = m_impl->localName;
  }
  else
  {
    m_impl->qualifiedName = m_impl->outerScope->qualifiedName()+
           getLanguageSpecificSeparator(getLanguage())+
           m_impl->localName;
  }
  //printf("end %s::qualifiedName()=%s\n",qPrint(name()),qPrint(m_impl->qualifiedName));
  //count--;
  return m_impl->qualifiedName;
}

void DefinitionImpl::setOuterScope(Definition *d)
{
  //printf("%s::setOuterScope(%s)\n",qPrint(name()),d?qPrint(d->name()):"<none>");
  Definition *p = m_impl->outerScope;
  bool found=false;
  // make sure that we are not creating a recursive scope relation.
  while (p && !found)
  {
    found = (p==d);
    p = p->getOuterScope();
  }
  if (!found)
  {
    m_impl->qualifiedName.resize(0); // flush cached scope name
    m_impl->outerScope = d;
  }
  m_impl->hidden = m_impl->hidden || d->isHidden();
}

QCString DefinitionImpl::localName() const
{
  return m_impl->localName;
}

void DefinitionImpl::makePartOfGroup(const GroupDef *gd)
{
  m_impl->partOfGroups.push_back(gd);
}

void DefinitionImpl::setRefItems(const RefItemVector &sli)
{
  m_impl->xrefListItems.insert(m_impl->xrefListItems.end(), sli.cbegin(), sli.cend());
}

void DefinitionImpl::mergeRefItems(Definition *d)
{
  auto otherXrefList = d->xrefListItems();

  // append vectors
  m_impl->xrefListItems.reserve(m_impl->xrefListItems.size()+otherXrefList.size());
  m_impl->xrefListItems.insert (m_impl->xrefListItems.end(),
                                otherXrefList.begin(),otherXrefList.end());

  // sort results on itemId
  std::sort(m_impl->xrefListItems.begin(),m_impl->xrefListItems.end(),
            [](RefItem *left,RefItem *right)
            { return  left->id() <right->id() ||
                     (left->id()==right->id() &&
                      left->list()->listName() < right->list()->listName());
            });

  // filter out duplicates
  auto last = std::unique(m_impl->xrefListItems.begin(),m_impl->xrefListItems.end(),
            [](const RefItem *left,const RefItem *right)
            { return left->id()==right->id() &&
                     left->list()->listName()==right->list()->listName();
            });
  m_impl->xrefListItems.erase(last, m_impl->xrefListItems.end());
}

int DefinitionImpl::_getXRefListId(const QCString &listName) const
{
  for (const RefItem *item : m_impl->xrefListItems)
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
  return m_impl->xrefListItems;
}

QCString DefinitionImpl::pathFragment() const
{
  QCString result;
  if (m_impl->outerScope && m_impl->outerScope!=Doxygen::globalScope)
  {
    result = m_impl->outerScope->pathFragment();
  }
  if (m_impl->def->isLinkable())
  {
    if (!result.isEmpty()) result+="/";
    if (m_impl->def->definitionType()==Definition::TypeGroup &&
        !toGroupDef(m_impl->def)->groupTitle().isEmpty())
    {
      result+=toGroupDef(m_impl->def)->groupTitle();
    }
    else if (m_impl->def->definitionType()==Definition::TypePage &&
        toPageDef(m_impl->def)->hasTitle())
    {
      result+=toPageDef(m_impl->def)->title();
    }
    else
    {
      result+=m_impl->localName;
    }
  }
  else
  {
    result+=m_impl->localName;
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
  else if (m_impl->def->definitionType()==Definition::TypeFile &&
           toFileDef(m_impl->def)->getDirDef())
  {
    result+=(toFileDef(m_impl->def))->getDirDef()->navigationPathAsString();
  }
  result+="<li class=\"navelem\">";
  if (m_impl->def->isLinkable())
  {
    if (m_impl->def->definitionType()==Definition::TypeGroup &&
        !toGroupDef(m_impl->def)->groupTitle().isEmpty())
    {
      result+="<a class=\"el\" href=\"$relpath^"+addHtmlExtensionIfMissing(m_impl->def->getOutputFileBase())+"\">"+
              convertToHtml(toGroupDef(m_impl->def)->groupTitle())+"</a>";
    }
    else if (m_impl->def->definitionType()==Definition::TypePage &&
             toPageDef(m_impl->def)->hasTitle())
    {
      result+="<a class=\"el\" href=\"$relpath^"+addHtmlExtensionIfMissing(m_impl->def->getOutputFileBase())+"\">"+
            convertToHtml((toPageDef(m_impl->def))->title())+"</a>";
    }
    else if (m_impl->def->definitionType()==Definition::TypeClass)
    {
      QCString name = locName;
      if (name.right(2)=="-p" /*|| name.right(2)=="-g"*/)
      {
        name = name.left(name.length()-2);
      }
      result+="<a class=\"el\" href=\"$relpath^"+addHtmlExtensionIfMissing(m_impl->def->getOutputFileBase());
      if (!m_impl->def->anchor().isEmpty()) result+="#"+m_impl->def->anchor();
      result+="\">"+convertToHtml(name)+"</a>";
    }
    else
    {
      result+="<a class=\"el\" href=\"$relpath^"+addHtmlExtensionIfMissing(m_impl->def->getOutputFileBase())+"\">"+
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
  ol.disableAllBut(OutputGenerator::Html);

  QCString navPath;
  navPath += "<div id=\"nav-path\" class=\"navpath\">\n"
             "  <ul>\n";
  navPath += navigationPathAsString();
  navPath += "  </ul>\n"
             "</div>\n";
  ol.writeNavigationPath(navPath);

  ol.popGeneratorState();
}

// TODO: move to htmlgen
void DefinitionImpl::writeToc(OutputList &ol, const LocalToc &localToc) const
{
  if (m_impl->sectionRefs.empty()) return;
  if (localToc.isHtmlEnabled())
  {
    int maxLevel = localToc.htmlLevel();
    ol.pushGeneratorState();
    ol.disableAllBut(OutputGenerator::Html);
    ol.writeString("<div class=\"toc\">");
    ol.writeString("<h3>");
    ol.writeString(theTranslator->trRTFTableOfContents());
    ol.writeString("</h3>\n");
    ol.writeString("<ul>");
    int level=1,l;
    char cs[2];
    cs[1]='\0';
    BoolVector inLi(maxLevel+1,false);
    for (const SectionInfo *si : m_impl->sectionRefs)
    {
      SectionType type = si->type();
      if (isSection(type))
      {
        //printf("  level=%d title=%s\n",level,qPrint(si->title));
        int nextLevel = static_cast<int>(type);
        if (nextLevel>level)
        {
          for (l=level;l<nextLevel;l++)
          {
            if (l < maxLevel) ol.writeString("<ul>");
          }
        }
        else if (nextLevel<level)
        {
          for (l=level;l>nextLevel;l--)
          {
            if (l <= maxLevel && inLi[l]) ol.writeString("</li>\n");
            inLi[l]=false;
            if (l <= maxLevel) ol.writeString("</ul>\n");
          }
        }
        cs[0]=static_cast<char>('0'+nextLevel);
        if (nextLevel <= maxLevel && inLi[nextLevel])
        {
          ol.writeString("</li>\n");
        }
        QCString titleDoc = convertToHtml(si->title());
        if (nextLevel <= maxLevel)
        {
          ol.writeString("<li class=\"level"+QCString(cs)+"\">"
                         "<a href=\"#"+si->label()+"\">"+
                         (si->title().isEmpty()?si->label():titleDoc)+"</a>");
        }
        inLi[nextLevel]=true;
        level = nextLevel;
      }
    }
    if (level > maxLevel) level = maxLevel;
    while (level>1 && level <= maxLevel)
    {
      if (inLi[level])
      {
        ol.writeString("</li>\n");
      }
      inLi[level]=FALSE;
      ol.writeString("</ul>\n");
      level--;
    }
    if (level <= maxLevel && inLi[level]) ol.writeString("</li>\n");
    ol.writeString("</ul>\n");
    ol.writeString("</div>\n");
    ol.popGeneratorState();
  }

  if (localToc.isDocbookEnabled())
  {
    ol.pushGeneratorState();
    ol.disableAllBut(OutputGenerator::Docbook);
    ol.writeString("    <toc>\n");
    ol.writeString("    <title>" + theTranslator->trRTFTableOfContents() + "</title>\n");
    int level=1,l;
    int maxLevel = localToc.docbookLevel();
    BoolVector inLi(maxLevel+1,false);
    for (const SectionInfo *si : m_impl->sectionRefs)
    {
      SectionType type = si->type();
      if (isSection(type))
      {
        //printf("  level=%d title=%s\n",level,qPrint(si->title));
        int nextLevel = static_cast<int>(type);
        if (nextLevel>level)
        {
          for (l=level;l<nextLevel;l++)
          {
            if (l < maxLevel) ol.writeString("    <tocdiv>\n");
          }
        }
        else if (nextLevel<level)
        {
          for (l=level;l>nextLevel;l--)
          {
            inLi[l]=FALSE;
            if (l <= maxLevel) ol.writeString("    </tocdiv>\n");
          }
        }
        if (nextLevel <= maxLevel)
        {
          QCString titleDoc = convertToDocBook(si->title());
          ol.writeString("      <tocentry>" +
              (si->title().isEmpty()?si->label():titleDoc) +
              "</tocentry>\n");
        }
        inLi[nextLevel]=TRUE;
        level = nextLevel;
      }
    }
    if (level > maxLevel) level = maxLevel;
    while (level>1 && level <= maxLevel)
    {
      inLi[level]=FALSE;
      ol.writeString("</tocdiv>\n");
      level--;
    }
    ol.writeString("    </toc>\n");
    ol.popGeneratorState();
  }

  if (localToc.isLatexEnabled())
  {
    ol.pushGeneratorState();
    ol.disableAllBut(OutputGenerator::Latex);
    int maxLevel = localToc.latexLevel();

    ol.writeString("\\etocsetnexttocdepth{"+QCString().setNum(maxLevel)+"}\n");

    ol.writeString("\\localtableofcontents\n");
    ol.popGeneratorState();
  }
}

//----------------------------------------------------------------------------------------

const SectionRefs &DefinitionImpl::getSectionRefs() const
{
  return m_impl->sectionRefs;
}

QCString DefinitionImpl::symbolName() const
{
  return m_impl->symbolName;
}

//----------------------

QCString DefinitionImpl::documentation() const
{
  return m_impl->details ? m_impl->details->doc : QCString("");
}

int DefinitionImpl::docLine() const
{
  return m_impl->details ? m_impl->details->line : 1;
}

QCString DefinitionImpl::docFile() const
{
  return m_impl->details ? m_impl->details->file : QCString("<"+m_impl->name+">");
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
    QCString str = substitute(p.c_str(),"$name",scopelessName); // replace $name with entity name
    str += " ";
    stripWord(result,str);
  }

  // capitalize first word
  if (!result.isEmpty())
  {
    char c=result[0];
    if (c>='a' && c<='z') c+='A'-'a';
    result[0]=c;
  }
  return result;
}


//----------------------

QCString DefinitionImpl::briefDescription(bool abbr) const
{
  //printf("%s::briefDescription(%d)='%s'\n",qPrint(name()),abbr,m_impl->brief?qPrint(m_impl->brief->doc):"<none>");
  return m_impl->brief ?
         (abbr ? abbreviate(m_impl->brief->doc,m_impl->def->displayName()) : m_impl->brief->doc) :
         QCString("");
}

void DefinitionImpl::computeTooltip()
{
  if (m_impl->brief && m_impl->brief->tooltip.isEmpty() && !m_impl->brief->doc.isEmpty())
  {
    const MemberDef *md = m_impl->def->definitionType()==Definition::TypeMember ? toMemberDef(m_impl->def) : 0;
    const Definition *scope = m_impl->def->definitionType()==Definition::TypeMember ? m_impl->def->getOuterScope() : m_impl->def;
    m_impl->brief->tooltip = parseCommentAsText(scope,md,
                                m_impl->brief->doc, m_impl->brief->file, m_impl->brief->line);
  }
}

QCString DefinitionImpl::briefDescriptionAsTooltip() const
{
  return m_impl->brief ? m_impl->brief->tooltip : QCString();
}

int DefinitionImpl::briefLine() const
{
  return m_impl->brief ? m_impl->brief->line : 1;
}

QCString DefinitionImpl::briefFile() const
{
  return m_impl->brief ? m_impl->brief->file : QCString("<"+m_impl->name+">");
}

//----------------------

QCString DefinitionImpl::inbodyDocumentation() const
{
  return m_impl->inbodyDocs ? m_impl->inbodyDocs->doc : QCString("");
}

int DefinitionImpl::inbodyLine() const
{
  return m_impl->inbodyDocs ? m_impl->inbodyDocs->line : 1;
}

QCString DefinitionImpl::inbodyFile() const
{
  return m_impl->inbodyDocs ? m_impl->inbodyDocs->file : QCString("<"+m_impl->name+">");
}


//----------------------

QCString DefinitionImpl::getDefFileName() const
{
  return m_impl->defFileName;
}

QCString DefinitionImpl::getDefFileExtension() const
{
  return m_impl->defFileExt;
}

bool DefinitionImpl::isHidden() const
{
  return m_impl->hidden;
}

bool DefinitionImpl::isVisibleInProject() const
{
  return m_impl->def->isLinkableInProject() && !m_impl->hidden;
}

bool DefinitionImpl::isVisible() const
{
  return m_impl->def->isLinkable() && !m_impl->hidden;
}

bool DefinitionImpl::isArtificial() const
{
  return m_impl->isArtificial;
}

QCString DefinitionImpl::getReference() const
{
  return m_impl->ref;
}

bool DefinitionImpl::isReference() const
{
  return !m_impl->ref.isEmpty();
}

int DefinitionImpl::getStartDefLine() const
{
  return m_impl->body ? m_impl->body->defLine : -1;
}

int DefinitionImpl::getStartBodyLine() const
{
  return m_impl->body ? m_impl->body->startLine : -1;
}

int DefinitionImpl::getEndBodyLine() const
{
  return m_impl->body ? m_impl->body->endLine : -1;
}

const FileDef *DefinitionImpl::getBodyDef() const
{
  return m_impl->body ? m_impl->body->fileDef : 0;
}

const GroupList &DefinitionImpl::partOfGroups() const
{
  return m_impl->partOfGroups;
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
  return m_impl->outerScope;
}

const MemberVector &DefinitionImpl::getReferencesMembers() const
{
  if (m_impl->referencesMembers.empty() && !m_impl->sourceRefsDict.empty())
  {
    m_impl->referencesMembers = refMapToVector(m_impl->sourceRefsDict);
  }
  return m_impl->referencesMembers;
}

const MemberVector &DefinitionImpl::getReferencedByMembers() const
{
  if (m_impl->referencedByMembers.empty() && !m_impl->sourceRefByDict.empty())
  {
    m_impl->referencedByMembers = refMapToVector(m_impl->sourceRefByDict);
  }
  return m_impl->referencedByMembers;
}

void DefinitionImpl::mergeReferences(const Definition *other)
{
  const DefinitionImpl *defImpl = other->toDefinitionImpl_();
  if (defImpl)
  {
    for (const auto &kv : defImpl->m_impl->sourceRefsDict)
    {
      auto it = m_impl->sourceRefsDict.find(kv.first);
      if (it != m_impl->sourceRefsDict.end())
      {
        m_impl->sourceRefsDict.insert(kv);
      }
    }
  }
}

void DefinitionImpl::mergeReferencedBy(const Definition *other)
{
  const DefinitionImpl *defImpl = other->toDefinitionImpl_();
  if (defImpl)
  {
    for (const auto &kv : defImpl->m_impl->sourceRefByDict)
    {
      auto it = m_impl->sourceRefByDict.find(kv.first);
      if (it != m_impl->sourceRefByDict.end())
      {
        m_impl->sourceRefByDict.insert({kv.first,kv.second});
      }
    }
  }
}


void DefinitionImpl::setReference(const QCString &r)
{
  m_impl->ref=r;
}

SrcLangExt DefinitionImpl::getLanguage() const
{
  return m_impl->lang;
}

void DefinitionImpl::setHidden(bool b)
{
  m_impl->hidden = m_impl->hidden || b;
}

void DefinitionImpl::setArtificial(bool b)
{
  m_impl->isArtificial = b;
}

void DefinitionImpl::setLocalName(const QCString &name)
{
  m_impl->localName=name;
}

void DefinitionImpl::setLanguage(SrcLangExt lang)
{
  m_impl->lang=lang;
}


void DefinitionImpl::_setSymbolName(const QCString &name)
{
  m_impl->symbolName=name;
}

QCString DefinitionImpl::_symbolName() const
{
  return m_impl->symbolName;
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
      uint l = result.length();
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

QCString DefinitionImpl::name() const
{
  return m_impl->name;
}

bool DefinitionImpl::isAnonymous() const
{
  return m_impl->isAnonymous;
}

int DefinitionImpl::getDefLine() const
{
  return m_impl->defLine;
}

int DefinitionImpl::getDefColumn() const
{
  return m_impl->defColumn;
}

void DefinitionImpl::writeQuickMemberLinks(OutputList &,const MemberDef *) const
{
}

void DefinitionImpl::writeSummaryLinks(OutputList &) const
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
}

void DefinitionAliasImpl::deinit()
{
  removeFromMap(m_symbolName,m_def);
}

QCString DefinitionAliasImpl::qualifiedName() const
{
  //printf("start %s::qualifiedName() localName=%s\n",qPrint(name()),qPrint(m_impl->localName));
  if (m_scope==0)
  {
    return m_def->localName();
  }
  else
  {
    return m_scope->qualifiedName()+
           getLanguageSpecificSeparator(m_scope->getLanguage())+
           m_def->localName();
  }
}

QCString DefinitionAliasImpl::name() const
{
  return qualifiedName();
}

//---------------------------------------------------------------------------------

Definition *toDefinition(DefinitionMutable *dm)
{
  if (dm==0) return 0;
  return dm->toDefinition_();
}

DefinitionMutable *toDefinitionMutable(Definition *d)
{
  if (d==0) return 0;
  return d->toDefinitionMutable_();
}

DefinitionMutable *toDefinitionMutable(const Definition *d)
{
  return toDefinitionMutable(const_cast<Definition*>(d));
}

