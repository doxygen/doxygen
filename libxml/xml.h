#ifndef XML_H
#define XML_H

/******************************************************************************
 *
 * Copyright (C) 1997-2021 by Dimitri van Heesch.
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

#include <memory>
#include <functional>
#include <string>
#include <unordered_map>

/*! @brief Event handlers that can installed by the client and called while parsing a XML document.
 */
class XMLHandlers
{
  public:
    using Attributes       = std::unordered_map<std::string,std::string>;
    using StartDocType     = void();
    using EndDocType       = void();
    using StartElementType = void(const std::string &,const Attributes &);
    using EndElementType   = void(const std::string &);
    using ErrorType        = void(const std::string,int,const std::string &);
    using CharsType        = void(const std::string &);

    std::function<StartDocType>      startDocument; /**< handler invoked at the start of the document */
    std::function<EndDocType>        endDocument;   /**< handler invoked at the end of the document */
    std::function<StartElementType>  startElement;  /**< handler invoked when an opening tag has been found */
    std::function<EndElementType>    endElement;    /**< handler invoked when a closing tag has been found */
    std::function<CharsType>         characters;    /**< handler invoked when content between tags has been found */
    std::function<ErrorType>         error;         /**< handler invoked when the parser encounters an error */

    static std::string value(const Attributes &attrib,const std::string &key)
    {
      auto it = attrib.find(key);
      if (it!=attrib.end())
      {
        return it->second;
      }
      return "";
    }
};

class XMLLocator
{
  public:
    XMLLocator() = default;
    XMLLocator(const XMLLocator &) = delete;
    XMLLocator &operator=(const XMLLocator &) = delete;
    XMLLocator(XMLLocator &&) = delete;
    XMLLocator &operator=(XMLLocator &&) = delete;
    virtual ~XMLLocator() = default;

    virtual int lineNr() const = 0;
    virtual std::string fileName() const = 0;
};

/*! Very basic SAX style parser to parse XML documents. */
class XMLParser : public XMLLocator
{
  public:
    /*! Creates an instance of the parser object. Different instances can run on different
     *  threads without interference.
     *
     *  @param handlers The event handlers passed by the client.
     */
    XMLParser(const XMLHandlers &handlers);
    /*! Destructor */
   ~XMLParser() override;
    XMLParser(const XMLParser &) = delete;
    XMLParser &operator=(const XMLParser &) = delete;
    XMLParser(XMLParser &&) = delete;
    XMLParser &operator=(XMLParser &&) = delete;

   using Transcode = bool(std::string &,const char *);

    /*! Parses a file gives the contents of the file as a string.
     *  @param fileName the name of the file, used for error reporting.
     *  @param inputString the contents of the file as a zero terminated UTF-8 string.
     *  @param debugEnabled indicates if debugging via -d lex is enabled or not.
     *  @param debugStart hook that is to be called before starting with parsing
     *  @param debugEnd   hook that is to be called after finishing with parsing
     *  @param transcoder hook that is to be called when transcoding text to UTF-8
     */
    void parse(const char *fileName,
               const char *inputString,
               bool debugEnabled,
               std::function<void()> debugStart,
               std::function<void()> debugEnd,
               std::function<Transcode> transcoder =
                   [](std::string&s,const char *){ return true; }
              );

  private:
   int lineNr() const override;
   std::string fileName() const override;
   struct Private;
   std::unique_ptr<Private> p;
};

#endif
