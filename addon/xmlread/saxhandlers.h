#ifndef _SAXHANDLERS_H
#define _SAXHANDLERS_H

#include "compounddef.h"
#include <sax/HandlerBase.hpp>

/*! SAX Handlers used for pass 1. */
class SaxPass1Handlers : public HandlerBase
{
  public:
    // -----------------------------------------------------------------------
    //! @name Constructors
    // -----------------------------------------------------------------------
    //@{
    /*! Constructor */
    SaxPass1Handlers(CompoundSDict *comp);
    /*! Destructor */
    ~SaxPass1Handlers();
    //@}

    // -----------------------------------------------------------------------
    //! @name Implementations of the SAX DocumentHandler interface
    // -----------------------------------------------------------------------
    //@{
    /*! Handler called at the start of the XML document */
    void startDocument() {}
    /*! Handler called at the end of the XML document */
    void endDocument();
    /*! Handler called when an element is found in the XML file 
     *  \param name The element's name.
     *  \param attributes The list of attributes for the element.
     */
    void startElement(const XMLCh* const name, AttributeList& attributes);
    /*! Handler called when the element of an element is found in the XML file
     *  \param name The element's name.
     */
    void endElement(const XMLCh* const name);
    /*! Handler called when a character string is found in the XML file.
     *  \param chars Zero terminated string of characters.
     *  \param length The number of characters in the string.
     */
    void characters(const XMLCh* const chars, const unsigned int length);

    /*! Handler called when an amount of whitespace is found, that is not
     *  Part of the text.
     */
    void ignorableWhitespace(const XMLCh* const /*chars*/, 
                             const unsigned int /*length*/
                            ) {}

    /*! Handler called when a preprocessing intruction is found.
     */
    void processingInstruction(const XMLCh* const /*target*/, 
                               const XMLCh* const /*data*/
                              ) {}
    //@}

    // -----------------------------------------------------------------------
    //! @name Implementations of the SAX ErrorHandler interface
    // -----------------------------------------------------------------------
    //@{
    /*! Handler called for a parser warning. */ 
    void warning(const SAXParseException& exception);
    /*! Handler called for a parser error. */ 
    void error(const SAXParseException& exception);
    /*! Handler called for a parser fatal error. */ 
    void fatalError(const SAXParseException& exception);
    //@}

    // -----------------------------------------------------------------------
    //! @name Implementation of the SAX DTDHandler interface
    // -----------------------------------------------------------------------
    //@{
    /*! Handler for DTD notation declaration. */
    void notationDecl(const XMLCh* const /*name*/, 
                      const XMLCh* const /*publicId*/, 
                      const XMLCh* const /*systemId*/
                     ) {}

    /*! Handler for DTD unparsed entity declaration. */
    void unparsedEntityDecl(const XMLCh* const /*name*/, 
                            const XMLCh* const /*publicId*/, 
                            const XMLCh* const /*systemId*/, 
                            const XMLCh* const /*notationName*/
                           ) {}
    //@}

  private:
    // -----------------------------------------------------------------------
    //  Private data members
    //
    // -----------------------------------------------------------------------

    enum CharActions { Done, ReadCompoundName };
    CompoundSDict *m_compounds;
    CompoundDef_Impl *m_currentCompound;
    QCString m_compoundName;
    CharActions m_charAction;
};

#endif
