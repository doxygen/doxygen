#ifndef _DOXMLINTF_H
#define _DOXMLINTF_H

#include <qlist.h>
#include <qstring.h>

class IParam
{
  public:
    virtual QString type() const = 0;
    virtual QString declarationName() const = 0;
    virtual QString definitionName() const = 0;
    virtual QString attrib() const = 0;
    virtual QString arraySpecifier() const = 0;
    virtual QString defaultValue() const = 0;
};

class IMember
{
  public:
    virtual QString kind() const = 0;
    virtual QString id() const = 0;
    virtual QString protection() const = 0;
    virtual QString virtualness() const = 0;
    virtual QString type() const = 0;
    virtual QString name() const = 0;
    virtual QListIterator<IParam> getParamIterator() const = 0;
};

class ISection
{
  public:
    virtual QString kind() const = 0;
    virtual QListIterator<IMember> getMemberIterator() const = 0;
};

class ICompound
{
  public:
    virtual QString name() const = 0;
    virtual QString id()   const = 0;
    virtual QString kind() const = 0;
    virtual QListIterator<ISection> getSectionIterator() const = 0;
};

/*! Root node of the object model. */
class IDoxygen
{
  public:
    /*! Returns an iterator that can be used to iterate over the list
     *  of compounds found in the project.
     */
    virtual QListIterator<ICompound> getCompoundIterator() const = 0;
};

/*! Factory method that creates an object model given an XML file generated
 *  by doxygen.
 *  @param xmlFileName The name of the XML to parse.
 *  @returns An iterface to the object model.
 */
IDoxygen *createObjectModelFromXML(const char *xmlFileName);

#endif
