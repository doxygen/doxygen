#include "paramhandler.h"
#include "memberhandler.h"

ParamHandler::ParamHandler(IBaseHandler *parent) : m_parent(parent)
{
  addEndHandler("param",this,&ParamHandler::endParam);

  addStartHandler("type");
  addEndHandler("type",this,&ParamHandler::endType);

  addStartHandler("declname");
  addEndHandler("declname",this,&ParamHandler::endDeclName);

  addStartHandler("defname");
  addEndHandler("defname",this,&ParamHandler::endDefName);

  addStartHandler("array");
  addEndHandler("array",this,&ParamHandler::endArray);

  addStartHandler("attrib");
  addEndHandler("attrib",this,&ParamHandler::endAttrib);

  addStartHandler("defval");
  addEndHandler("defval",this,&ParamHandler::endDefVal);
}

ParamHandler::~ParamHandler()
{
}

void ParamHandler::startParam(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
  printf("param\n");
}

void ParamHandler::endParam()
{
  m_parent->setDelegate(0);
}

void ParamHandler::endType()
{
  m_type = m_curString.stripWhiteSpace();
  printf("param type=`%s'\n",m_type.data());
}

void ParamHandler::endDeclName()
{
  m_declName = m_curString.stripWhiteSpace();
  printf("member declName=`%s'\n",m_declName.data());
}

void ParamHandler::endDefName()
{
  m_defName = m_curString.stripWhiteSpace();
  printf("member defName=`%s'\n",m_defName.data());
}

void ParamHandler::endAttrib()
{
  m_attrib = m_curString.stripWhiteSpace();
  printf("member attrib=`%s'\n",m_attrib.data());
}

void ParamHandler::endArray()
{
  m_array = m_curString.stripWhiteSpace();
  printf("member array=`%s'\n",m_array.data());
}

void ParamHandler::endDefVal()
{
  m_defVal = m_curString.stripWhiteSpace();
  printf("member defVal=`%s'\n",m_defVal.data());
}



