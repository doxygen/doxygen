
// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include <util/XMLUni.hpp>
#include <sax/AttributeList.hpp>
#include "saxhandlers.h"
#include "strx.h"
#include <iostream.h>

static void error(const SAXParseException& e)
{
  cerr << "\nError at (file " << StrX(e.getSystemId())
       << ", line " << e.getLineNumber()
       << ", char " << e.getColumnNumber()
       << "): " << StrX(e.getMessage()) << endl;
  exit(1);
}

static void fatalError(const SAXParseException& e)
{
  cerr << "\nFatal Error at (file " << StrX(e.getSystemId())
       << ", line " << e.getLineNumber()
       << ", char " << e.getColumnNumber()
       << "): " << StrX(e.getMessage()) << endl;
  exit(1);
}

static void warning(const SAXParseException& e)
{
  cerr << "\nWarning at (file " << StrX(e.getSystemId())
       << ", line " << e.getLineNumber()
       << ", char " << e.getColumnNumber()
       << "): " << StrX(e.getMessage()) << endl;
  exit(1);
}

// ---------------------------------------------------------------------------
//  SaxPass1Handlers: Constructors and Destructor
// ---------------------------------------------------------------------------
SaxPass1Handlers::SaxPass1Handlers(CompoundSDict *comp) : m_compounds(comp) 
{
  m_currentCompound = 0;
}

SaxPass1Handlers::~SaxPass1Handlers()
{
}

// ---------------------------------------------------------------------------
//  SaxPass1Handlers: Overrides of the SAX ErrorHandler interface
// ---------------------------------------------------------------------------
void SaxPass1Handlers::error(const SAXParseException& e)
{
  ::error(e);
}

void SaxPass1Handlers::fatalError(const SAXParseException& e)
{
  ::fatalError(e);
}

void SaxPass1Handlers::warning(const SAXParseException& e)
{
  ::warning(e);
}

// ---------------------------------------------------------------------------
//  SaxPass1Handlers: Overrides of the SAX DocumentHandler interface
// ---------------------------------------------------------------------------
void SaxPass1Handlers::characters(const XMLCh* const chars,
                                  const unsigned int /*length*/
                                 )
{
  if (m_charAction==ReadCompoundName)
  {
    m_compoundName=StrX(chars);
    m_charAction=Done;
  }
}

void SaxPass1Handlers::endDocument()
{
  // resolve all references
  CompoundSDictIterator cdi(*m_compounds);
  CompoundDef *cd;
  for (cdi.toFirst();(cd=cdi.current());++cdi)
  {
    CompoundDef_Impl *impl = dynamic_cast<CompoundDef_Impl*>(cd);
    impl->resolveReferences(m_compounds);
  }
}

void SaxPass1Handlers::startElement(const XMLCh* const name,
                                    AttributeList& attributes
                                   )
{
  QCString sname = StrX(name);
  if (sname=="compounddef")
  {
    m_currentCompound = new CompoundDef_Impl;
    unsigned int len = attributes.getLength(),index;
    for (index = 0; index < len; index++)
    {
      QCString attName  = StrX(attributes.getName(index));
      QCString attValue = StrX(attributes.getValue(index));
      if (attName == "id")
      {
        m_currentCompound->setId(attValue); 
      }
      else if (attName=="type")
      {
        if (attValue=="class")
        {
          m_currentCompound->setType(CompoundDef::Class_t);
        }
        else if (attValue=="struct")
        {
          m_currentCompound->setType(CompoundDef::Struct_t);
        }
        else if (attValue=="union")
        {
          m_currentCompound->setType(CompoundDef::Union_t);
        }
        else if (attValue=="interface")
        {
          m_currentCompound->setType(CompoundDef::Interface_t);
        }
        else if (attValue=="file")
        {
          m_currentCompound->setType(CompoundDef::File_t);
        }
        else
        {
          cerr << "Unsupported compound type: " << attValue << "\n";
        }
      }
    }
  }
  else if (sname=="compoundname")
  {
    m_charAction = ReadCompoundName;
  }
  else if (sname=="basecompoundref")
  {
    unsigned int len = attributes.getLength(),index;
    for (index = 0; index < len; index++)
    {
      QCString attName  = StrX(attributes.getName(index));
      QCString attValue = StrX(attributes.getValue(index));
      if (attName=="idref")
      {
        m_currentCompound->addBaseRef(attValue);
      }
    }
  }
  else if (sname=="derivedcompoundref")
  {
    unsigned int len = attributes.getLength(),index;
    for (index = 0; index < len; index++)
    {
      QCString attName  = StrX(attributes.getName(index));
      QCString attValue = StrX(attributes.getValue(index));
      if (attName=="idref")
      {
        m_currentCompound->addDerivedRef(attValue);
      }
    }
  }
}

void SaxPass1Handlers::endElement(const XMLCh* const name)
{
  QCString sname = StrX(name);
  if (sname=="compounddef")
  {
    m_compounds->append(m_currentCompound->id(),m_currentCompound);
    m_currentCompound=0;
  }
  else if (sname=="compoundname")
  {
    m_currentCompound->setName(m_compoundName);
  }
}

