/******************************************************************************
 *
 * $Id$
 *
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#ifndef _BASEHANDLER_H
#define _BASEHANDLER_H

#include <qxml.h>
#include <qdict.h>
#include <qstring.h>

#include "debug.h"

//-----------------------------------------------------------------------------

class IBaseHandler
{
  public:
    virtual void setDelegate(QXmlDefaultHandler *delegate) = 0;
    virtual QXmlDefaultHandler *delegate() const = 0;
    virtual ~IBaseHandler() {}
};

//-----------------------------------------------------------------------------

class IFallBackHandler
{
  public:
    virtual bool handleStartElement(const QString & name, 
                                    const QXmlAttributes & attrib) = 0;
    virtual bool handleEndElement(const QString &name) = 0;
    virtual ~IFallBackHandler() {}
};

//-----------------------------------------------------------------------------

template<class T> class ElementMapper
{
    class StartElementHandler
    {
         typedef void (T::*Handler)(const QXmlAttributes &attrib);
       public:
         StartElementHandler() : m_parent(0) {}
         StartElementHandler(T *parent, Handler h) 
           : m_parent(parent), m_handler(h) {}
         void operator()(const QXmlAttributes &attrib) 
           { if (m_parent) (m_parent->*m_handler)(attrib); }
       private:
         T *m_parent;
         Handler m_handler;
    };

    class EndElementHandler
    {
        typedef void (T::*Handler)();
      public:
        EndElementHandler() : m_parent(0) {}
        EndElementHandler(T *parent, Handler h) 
          : m_parent(parent), m_handler(h) {}
        void operator()() 
          { if (m_parent) (m_parent->*m_handler)(); }
      private:
        T *m_parent;
        Handler m_handler;
    };

  public:
    typedef StartElementHandler StartElementHandlerT;
    typedef EndElementHandler   EndElementHandlerT;
	  
    ElementMapper() : m_startHandlers(67), m_endHandlers(67)
    {
      m_startHandlers.setAutoDelete(TRUE);
      m_endHandlers.setAutoDelete(TRUE);
    }
    virtual ~ElementMapper()
    {
    }

    void addStartHandler(const char *key)
    {
      m_startHandlers.insert(key,new StartElementHandlerT);
    }

    void addStartHandler(const char *key, T *obj, void (T::*handler)(const QXmlAttributes &))
    {
      m_startHandlers.insert(key,new StartElementHandlerT(obj,handler));
    }
    
    void addEndHandler(const char *key)
    {
      m_endHandlers.insert(key,new EndElementHandlerT);
    }

    void addEndHandler(const char *key, T *obj, void (T::*handler)())
    {
      m_endHandlers.insert(key,new EndElementHandlerT(obj,handler));
    }


  protected:
    QDict<StartElementHandlerT> m_startHandlers;
    QDict<EndElementHandlerT>   m_endHandlers;
};

//-----------------------------------------------------------------------------

struct LocatorContainer
{
    static QXmlLocator *s_theLocator;
};

//-----------------------------------------------------------------------------

template<class T> class BaseHandler : public QXmlDefaultHandler,
                                      public ElementMapper<T>,
                                      public LocatorContainer,
                                      public IBaseHandler
{
  public:
    typedef typename ElementMapper<T>::StartElementHandlerT StartElementHandlerT;
    typedef typename ElementMapper<T>::EndElementHandlerT EndElementHandlerT;

    BaseHandler() : m_skipCount(0), m_delegateHandler(0), m_fallBackHandler(0)
    {
    }

    virtual ~BaseHandler() 
    {
      ASSERT(m_delegateHandler==0);
    }

    virtual bool startDocument() 
    {
      return TRUE;
    }

    virtual bool startElement(  const QString & namespaceURI, 
                                const QString & localName, 
                                const QString & name, 
                                const QXmlAttributes & attrib
                             )
    {
      if (m_delegateHandler) 
      {
        return m_delegateHandler->startElement(namespaceURI,localName,name,attrib);
      }
      if (!m_skipUntil.isEmpty()) // skip mode 
      {
        if (m_skipUntil==name) m_skipCount++;
        debug(1,"line %d, col %d: skipping start tag %s count=%d\n",
            s_theLocator->lineNumber(),s_theLocator->columnNumber(),
            name.data(),m_skipCount);
        return TRUE; 
      }

      StartElementHandlerT *handler = ElementMapper<T>::m_startHandlers[name.utf8()];
      if (handler)
      {
        (*handler)(attrib);
        //printf("found start tag %s\n",name.data());
      }
      else if (!m_fallBackHandler ||
               !m_fallBackHandler->handleStartElement(name,attrib)
              )
      {
        debug(1,"line %d, col %d: found unexpected tag `%s', skipping until matching end tag\n",
            s_theLocator->lineNumber(),s_theLocator->columnNumber(),
            name.data());
        m_skipUntil = name;
        m_skipCount=1;
      }
      return TRUE;
    }

    virtual bool endElement( const QString& namespaceURI, const QString& localName, const QString& name )
    {
      if (m_delegateHandler) 
      {
        return m_delegateHandler->endElement(namespaceURI,localName,name);
      }

      if (name==m_skipUntil)
      {
        m_skipCount--;
        debug(1,"line %d, col %d: skipping end tag %s count=%d\n",
            s_theLocator->lineNumber(),s_theLocator->columnNumber(),
            name.data(),m_skipCount);
        if (m_skipCount==0)
        {
          m_skipUntil="";
        }
        //printf("found end tag %s\n",name.data());
      }
      else if (m_skipUntil.isEmpty())
      {
        EndElementHandlerT *handler = ElementMapper<T>::m_endHandlers[name.utf8()];
        if (handler)
        {
          (*handler)();
          //printf("found end tag %s\n",name.data());
        }
        else if (m_fallBackHandler)
        {
          m_fallBackHandler->handleEndElement(name);
        }
      }
      m_curString="";
      return TRUE;
    }
    
    bool skippedEntity ( const QString &s )
    {
      if (m_delegateHandler)
      {
        return m_delegateHandler->skippedEntity(s);
      }
      
      debug(1,"line %d, col %d: Skipped unhandled entity %s\n",
          s_theLocator->lineNumber(),s_theLocator->columnNumber(),
          s.data());
      return TRUE;
    }

    /*! called when a number of characters are received by the parser.
     *  \param ch the characters.
     */
    virtual bool characters ( const QString & ch ) 
    {
      if (m_delegateHandler) 
      {
        return m_delegateHandler->characters(ch);
      }

      //printf("Found characters \"%s\"\n",ch.data());
      m_curString+=ch;
      return TRUE;
    }

    void setDelegate(QXmlDefaultHandler *delegate)
    {
      m_delegateHandler = delegate;
    }

    QXmlDefaultHandler *delegate() const
    {
      return m_delegateHandler;
    }

    void setFallBackHandler(IFallBackHandler *h)
    {
      m_fallBackHandler = h;
    }

    IFallBackHandler *fallBackHandler() const
    {
      return m_fallBackHandler;
    }

    void setDocumentLocator( QXmlLocator * locator )
    {
      debug(2,"setDocumentLocator(%p)\n",locator);
      s_theLocator = locator;
    }

  protected:
    QString                     m_curString;
    QString                     m_skipUntil;
    int                         m_skipCount;
    QXmlDefaultHandler         *m_delegateHandler;
    IFallBackHandler           *m_fallBackHandler;
};

//-----------------------------------------------------------------------------

template<class T> class BaseFallBackHandler : public ElementMapper<T>,
                                              public IFallBackHandler
{
  public:
    typedef typename ElementMapper<T>::StartElementHandlerT StartElementHandlerT;
    typedef typename ElementMapper<T>::EndElementHandlerT EndElementHandlerT;

    BaseFallBackHandler() 
    {
    }
    virtual ~BaseFallBackHandler()
    {
    }
    
    bool handleStartElement(const QString & name, 
                                    const QXmlAttributes & attrib)
    {
      StartElementHandlerT *handler = ElementMapper<T>::m_startHandlers[name.utf8()];
      if (handler)
      {
        (*handler)(attrib);
        return TRUE;
      }
      return FALSE;
    }
    bool handleEndElement(const QString &name)
    {
      EndElementHandlerT *handler = ElementMapper<T>::m_endHandlers[name.utf8()];
      if (handler)
      {
        (*handler)();
        return TRUE;
      }
      return FALSE;
    }
};


#endif
