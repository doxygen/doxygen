/******************************************************************************
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
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

// STL includes
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <iterator>
#include <regex>
#include <filesystem>
#include <unordered_map>


// Xapian include
#include <xapian.h>

#include "version.h"
#include "xml.h"

#define MAX_TERM_LENGTH 245

#if defined(_WIN32) && !defined(__CYGWIN__)
static char pathSep = '\\';
#else
static char pathSep = '/';
#endif

static void safeAddTerm(const std::string &term,Xapian::Document &doc,int wfd)
{
  if (term.length()<=MAX_TERM_LENGTH) doc.add_term(term,wfd);
}

/** trims \a whitespace characters from the start and end of string \a str. */
static std::string trim(const std::string& str,
                        const std::string& whitespace = " \t")
{
  size_t strBegin = str.find_first_not_of(whitespace);
  if (strBegin == std::string::npos)
    return ""; // no content

  size_t strEnd = str.find_last_not_of(whitespace);
  size_t strRange = strEnd - strBegin + 1;

  return str.substr(strBegin, strRange);
}

/** trims \a whitespace from start and end and replace occurrences of
 *  \a whitespace with \a fill.
 */
static std::string reduce(const std::string& str,
    const std::string& fill = " ",
    const std::string& whitespace = " \t")
{
  // trim first
  std::string result = trim(str, whitespace);

  // replace sub ranges
  size_t beginSpace = result.find_first_of(whitespace);
  while (beginSpace != std::string::npos)
  {
    size_t endSpace = result.find_first_not_of(whitespace, beginSpace);
    size_t range = endSpace - beginSpace;

    result.replace(beginSpace, range, fill);

    size_t newStart = beginSpace + fill.length();
    beginSpace = result.find_first_of(whitespace, newStart);
  }

  return result;
}

/** Adds all words in \a s to document \a doc with weight \a wfd */
static void addWords(const std::string &s,Xapian::Document &doc,int wfd)
{
  std::istringstream iss(s);
  std::istream_iterator<std::string> begin(iss),end,it;
  for (it=begin;it!=end;++it)
  {
    const std::string word = *it;
    const std::string lword = Xapian::Unicode::tolower(word);
    safeAddTerm(word,doc,wfd);
    if (lword!=word)
    {
      safeAddTerm(lword,doc,wfd);
    }
  }
}

/** Adds all identifiers in \a s to document \a doc with weight \a wfd */
static void addIdentifiers(const std::string &s,Xapian::Document &doc,int wfd)
{
  static const std::regex id_re("[A-Z_a-z][A-Z_a-z0-9]*");
  auto id_begin = std::sregex_iterator(s.begin(), s.end(), id_re);
  auto id_end   = std::sregex_iterator();

  for (auto i = id_begin; i!=id_end; ++i)
  {
    std::smatch match = *i;
    safeAddTerm(match.str(),doc,wfd);
  }
}

/** Replaces all occurrences of \a old with \a repl in string \a str */
static void replace_all(std::string& str, const std::string& old, const std::string& repl)
{
  size_t pos = 0;
  while ((pos = str.find(old, pos)) != std::string::npos)
  {
    str.replace(pos, old.length(), repl);
    pos += repl.length();
  }
}

/** Replaces all XML entities in \a s with their unescaped representation */
static std::string unescapeXmlEntities(const std::string &s)
{
  std::string result=s;
  replace_all(result,"&gt;",">");
  replace_all(result,"&lt;","<");
  replace_all(result,"&apos;","'");
  replace_all(result,"&quot;","\"");
  replace_all(result,"&amp;","&");
  return result;
}

/** This class is a wrapper around SAX style XML parser, which
 *  parses the file without first building a DOM tree in memory.
 */
class XMLContentHandler
{
  public:
    /** Handler for parsing XML data */
    XMLContentHandler(const std::string &path)
      : m_db(path+"doxysearch.db",Xapian::DB_CREATE_OR_OVERWRITE),
        m_stemmer("english")
    {
      m_curFieldName = UnknownField;
      m_indexer.set_stemmer(m_stemmer);
      m_indexer.set_document(m_doc);
    }

    /** Free data handler */
   ~XMLContentHandler()
    {
      m_db.commit();
    }

    enum FieldNames
    {
      UnknownField = 0,
      TypeField    = 1,
      NameField    = 2,
      ArgsField    = 3,
      TagField     = 4,
      UrlField     = 5,
      KeywordField = 6,
      TextField    = 7
    };

    /** Handler for a start tag. Called for `<doc>` and `<field>` tags */
    void startElement(const std::string &name, const XMLHandlers::Attributes &attrib)
    {
      m_data="";

      if (name != "field") return;

      static const std::unordered_map<std::string, FieldNames> fieldMap{
        { "type", TypeField },
        { "name", NameField },
        { "args", ArgsField },
        { "tag", TagField },
        { "url", UrlField },
        { "keywords", KeywordField },
        { "text", TextField }
      };
      std::string fieldName = XMLHandlers::value(attrib, "name");
      auto        it        = fieldMap.find(fieldName);
      m_curFieldName        = (it != fieldMap.end()) ? it->second : UnknownField;
    }

    /** Handler for an end tag. Called for `</doc>` and `</field>` tags */
    void endElement(const std::string &name)
    {
      if (name=="doc") // </doc>
      {
        std::string term = m_doc.get_value(NameField);
        std::string partTerm;
        size_t pos = term.rfind("::");
        if (pos!=std::string::npos)
        {
          partTerm = term.substr(pos+2);
        }
        if (m_doc.get_value(TypeField)=="class" ||
            m_doc.get_value(TypeField)=="file" ||
            m_doc.get_value(TypeField)=="namespace") // containers get highest prio
        {
          safeAddTerm(term,m_doc,1000);
          if (!partTerm.empty())
          {
            safeAddTerm(partTerm,m_doc,500);
          }
        }
        else // members and others get lower prio
        {
          safeAddTerm(m_doc.get_value(NameField),m_doc,100);
          if (!partTerm.empty())
          {
            safeAddTerm(partTerm,m_doc,50);
          }
        }
        m_db.add_document(m_doc);
        m_doc.clear_values();
        m_doc.clear_terms();
      }
      else if (name=="field" && m_curFieldName!=UnknownField) // </field>
      {
        // strip whitespace from m_data
        m_data = reduce(m_data);
        // replace XML entities
        m_data = unescapeXmlEntities(m_data);
        // add data to the document
        m_doc.add_value(m_curFieldName,m_data);
        switch (m_curFieldName)
        {
          case TypeField:
          case NameField:
          case TagField:
          case UrlField:
            // meta data that is not searchable
            break;
          case KeywordField:
            addWords(m_data,m_doc,50);
            break;
          case ArgsField:
            addIdentifiers(m_data,m_doc,10);
            break;
          case TextField:
            addWords(m_data,m_doc,2);
            break;
          default:
            break;
        }
        m_data="";
        m_curFieldName=UnknownField;
      }
      // reset m_data
    }

    /** Handler for inline text */
    void characters(const std::string& ch)
    {
      m_data += ch;
    }

    void error(const std::string &fileName,int lineNr,const std::string &msg)
    {
      std::cerr << "Fatal error at " << fileName << ":" << lineNr << ": " << msg << std::endl;
    }

  private:

    // internal state
    Xapian::WritableDatabase m_db;
    Xapian::Document m_doc;
    Xapian::TermGenerator m_indexer;
    Xapian::Stem m_stemmer;
    std::string m_data;
    FieldNames m_curFieldName;
};

static void usage(const char *name, int exitVal = 1)
{
  std::cerr << "Usage: " << name << " [-o output_dir] searchdata.xml [searchdata2.xml ...]" << std::endl;
  exit(exitVal);
}

// return the contents of a file as a string
inline std::string fileToString(const std::string &fileName)
{
  std::ifstream t(fileName);
  std::string result;
  t.seekg(0, std::ios::end);
  auto size = t.tellg();
  if (size < 0) size = 0;
  result.reserve(static_cast<std::size_t>(size));
  t.seekg(0, std::ios::beg);
  result.assign(std::istreambuf_iterator<char>(t),
                std::istreambuf_iterator<char>());
  return result;
}

bool dirExists(const char *path)
{
  std::error_code ec;
  return std::filesystem::is_directory(path, ec);
}

/** main function to index data */
int main(int argc,const char **argv)
{
  if (argc<2)
  {
    usage(argv[0]);
  }
  std::string outputDir;
  for (int i=1;i<argc;i++)
  {
    const std::string arg{ argv[i] };
    if (arg == "-o")
    {
      if (i>=argc-1)
      {
        std::cerr << "Error: missing parameter for -o option" << std::endl;
        usage(argv[0]);
      }
      else
      {
        i++;
        outputDir=argv[i];
        if (!dirExists(outputDir.c_str()))
        {
          std::cerr << "Error: specified output directory does not exist!" << std::endl;
          usage(argv[0]);
        }
      }
    }
    else if (arg == "-h" || arg == "--help")
    {
      usage(argv[0],0);
    }
    else if (arg == "-v" || arg == "--version")
    {
      std::cerr << argv[0] << " version: " << getFullVersion() << std::endl;
      exit(0);
    }
  }

  try
  {
    if (!outputDir.empty() && outputDir.at(outputDir.length()-1)!=pathSep)
    {
      outputDir+=pathSep;
    }
    XMLContentHandler contentHandler(outputDir);
    XMLHandlers handlers;
    handlers.startElement = [&contentHandler](const std::string &name,const XMLHandlers::Attributes &attrs)  { contentHandler.startElement(name,attrs);   };
    handlers.endElement   = [&contentHandler](const std::string &name)                                       { contentHandler.endElement(name);           };
    handlers.characters   = [&contentHandler](const std::string &chars)                                      { contentHandler.characters(chars);          };
    handlers.error        = [&contentHandler](const std::string &fileName,int lineNr,const std::string &msg) { contentHandler.error(fileName,lineNr,msg); };
    for (int i=1;i<argc;i++)
    {
      if (std::string(argv[i])=="-o")
      {
        i++;
      }
      else
      {
        std::cout << "Processing " << argv[i] << "..." << std::endl;
        std::string inputStr = fileToString(argv[i]);
        XMLParser parser(handlers);
        parser.parse(argv[i],inputStr.c_str(),false,[](){},[](){});
      }
    }
  }
  catch(const Xapian::Error &e)
  {
    std::cerr << "Caught exception: " << e.get_description() << std::endl;
  }
  catch(...)
  {
    std::cerr << "Caught an unknown exception" << std::endl;
  }

  return 0;
}
