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

#include <qxml.h>
#include "mainhandler.h"

void MainHandler::startCompound(const QXmlAttributes& attrib)
{
  CompoundHandler *compHandler = new CompoundHandler(this);
  compHandler->startCompound(attrib);
  m_compounds.append(compHandler);
  m_compoundDict.insert(compHandler->id(),compHandler);
}

MainHandler::MainHandler() : m_compoundDict(10007)
{
  m_compounds.setAutoDelete(TRUE);
  addStartHandler("doxygen"); 
  addStartHandler("compounddef",this,&MainHandler::startCompound);
  addEndHandler("doxygen");
  addEndHandler("compounddef");
}

MainHandler::~MainHandler()
{
  printf("MainHandler::~MainHandler()\n");
}


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

IDoxygen *createObjectModelFromXML(const char * xmlFileName)
{
  QFile xmlFile(xmlFileName);
  MainHandler * handler = new MainHandler;
  ErrorHandler errorHandler;
  QXmlInputSource source( xmlFile );
  QXmlSimpleReader reader;
  reader.setContentHandler( handler );
  reader.setErrorHandler( &errorHandler );
  reader.parse( source );
  return handler;
}

