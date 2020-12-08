#ifndef XML_H
#define XML_H

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
    virtual ~XMLLocator() {}
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
   ~XMLParser();

    /*! Parses a file gives the contents of the file as a string.
     *  @param fileName the name of the file, used for error reporting.
     *  @param inputString the contents of the file as a zero terminated UTF-8 string.
     */
    void parse(const char *fileName,const char *inputString);

  private:
   virtual int lineNr() const override;
   virtual std::string fileName() const override;
   struct Private;
   std::unique_ptr<Private> p;
};

#endif
