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

  //for (;;)
  //{
    QFile xmlFile(argv[1]);
    MainHandler handler;
    ErrorHandler errorHandler;
    QXmlInputSource source( xmlFile );
    QXmlSimpleReader reader;
    reader.setContentHandler( &handler );
    reader.setErrorHandler( &errorHandler );
    reader.parse( source );
  //}
  
  return 0;
}

