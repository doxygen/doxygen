#include "mainhandler.h"

void MainHandler::startCompound(const QXmlAttributes& attrib)
{
  CompoundHandler *compHandler = new CompoundHandler(this);
  compHandler->startCompound(attrib);
  m_compounds.append(compHandler);
}

MainHandler::MainHandler()
{
  m_compounds.setAutoDelete(TRUE);
  addStartHandler("doxygen"); 
  addStartHandler("compounddef",this,&MainHandler::startCompound);
  addEndHandler("doxygen");
  addEndHandler("compounddef");
}

MainHandler::~MainHandler()
{
}


