/******************************************************************************
 *
 * $Id$
 *
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
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

class IBaseHandler
{
  public:
    virtual void setDelegate(QXmlDefaultHandler *delegate) = 0;
    virtual QXmlDefaultHandler *delegate() const = 0;
};

class IFallBackHandler
{
  public:
    virtual bool handleStartElement(const QString & name, 
                                    const QXmlAttributes & attrib) = 0;
    virtual bool handleEndElement(const QString &name) = 0;
};

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

    typedef StartElementHandler<T> StartElementHandlerT;
    typedef EndElementHandler<T>   EndElementHandlerT;
  
  public:
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

template<class T> class BaseHandler : public IBaseHandler, 
                                      public QXmlDefaultHandler,
                                      public ElementMapper<T>
{
  public:
    BaseHandler() : m_delegateHandler(0), m_fallBackHandler(0)
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
        printf("skipping start tag %s count=%d\n",name.data(),m_skipCount);
        return TRUE; 
      }

      StartElementHandlerT *handler = m_startHandlers[name];
      if (handler)
      {
        (*handler)(attrib);
        //printf("found start tag %s\n",name.data());
      }
      else if (m_fallBackHandler && 
               !m_fallBackHandler->handleStartElement(name,attrib)
              )
      {
        printf("found unexpected tag `%s', skipping until matching end tag\n",name.data());
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
        printf("skipping end tag %s count=%d\n",name.data(),m_skipCount);
        if (m_skipCount==0)
        {
          m_skipUntil="";
        }
        //printf("found end tag %s\n",name.data());
      }
      else if (m_skipUntil.isEmpty())
      {
        EndElementHandlerT *handler = m_endHandlers[name];
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

    virtual bool characters ( const QString & ch ) 
    {
      if (m_delegateHandler) 
      {
        return m_delegateHandler->characters(ch);
      }

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

  protected:
    QString                     m_curString;
    QString                     m_skipUntil;
    int                         m_skipCount;
    QXmlDefaultHandler         *m_delegateHandler;
    IFallBackHandler           *m_fallBackHandler;
};

template<class T> class BaseFallBackHandler : public ElementMapper<T>,
                                              public IFallBackHandler
{
  public:
    BaseFallBackHandler() 
    {
    }
    virtual ~BaseFallBackHandler()
    {
    }
    bool handleStartElement(const QString & name, 
                                    const QXmlAttributes & attrib)
    {
      StartElementHandlerT *handler = m_startHandlers[name];
      if (handler)
      {
        (*handler)(attrib);
        return TRUE;
      }
      return FALSE;
    }
    bool handleEndElement(const QString &name)
    {
      EndElementHandlerT *handler = m_endHandlers[name];
      if (handler)
      {
        (*handler)();
        return TRUE;
      }
      return FALSE;
    }
};


#endif
