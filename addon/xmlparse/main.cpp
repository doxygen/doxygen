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

#include "mainhandler.h"

#include <qstring.h>
#include <qxml.h>
#include <qfile.h>
#include <qdict.h>
#include <qlist.h>

//#define USE_DOM
#define USE_SAX

#ifdef USE_DOM
#include <qdom.h>
#endif

class ErrorHandler : public QXmlErrorHandler
{
    public:
      virtual ~ErrorHandler() {}
      bool warning( const QXmlParseException & )
      {
        return FALSE;
      }
      bool error( const QXmlParseException & )
      {
        return FALSE;
      }
      bool fatalError( const QXmlParseException &exception )
      {
        fprintf(stderr,"Fatal error at line %d column %d: %s\n",
            exception.lineNumber(),exception.columnNumber(),
            exception.message().data());
        return FALSE;
      }
      QString errorString() { return ""; }

    private:
      QString errorMsg;
};


int main(int argc,char **argv)
{
  if (argc==1)
  {
    printf("Usage: %s file.xml\n",argv[0]);
    exit(1);
  }

  QFile xmlFile(argv[1]);
  if (!xmlFile.open( IO_ReadOnly ))
  {
    qFatal("Could not read %s",argv[1] );
  }

#ifdef USE_SAX
  MainHandler handler;
  ErrorHandler errorHandler;
  QXmlInputSource source( xmlFile );
  QXmlSimpleReader reader;
  reader.setContentHandler( &handler );
  reader.setErrorHandler( &errorHandler );
  reader.parse( source );
#endif

#ifdef USE_DOM
  QDomDocument doc;
  doc.setContent( &xmlFile );

  QDomElement de = doc.documentElement();

  printf("docElem=%s\n",de.tagName().data());

  QDomNode n = de.firstChild();
  while( !n.isNull() ) 
  {
    QDomElement e = n.toElement(); // try to convert the node to an element.
    if( !e.isNull() ) 
    { // the node was really an element.
      printf("direct child %s id=%s kind=%s\n",
          e.tagName().data(),
          e.attribute("id").data(),
          e.attribute("kind").data()
          );
    }
    n = n.nextSibling();
  }

#endif

  return 0;
}

