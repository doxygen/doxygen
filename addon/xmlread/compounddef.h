#ifndef COMPOUNDDEF_H
#define COMPOUNDDEF_H

#include "qtbc.h"
#include <qlist.h>
#include <qdict.h>
#include <qstrlist.h>

/*! Abstract interface for a compound as presented to the user */
class CompoundDef 
{
  public:
    /*! Possible compound types */
    enum Type { Unknown_t, Class_t, Struct_t, Union_t, 
                File_t, Namespace_t, Group_t, Interface_t };
    /*! Destructor */
    virtual ~CompoundDef() {}

    /*! Returns the name of this compound */
    virtual QCString name() const = 0;
    /*! Returns the type of this compound */
    virtual Type     type() const = 0;
    /*! Returns the unique identifier for this compound */
    virtual QCString id()   const = 0;
    /*! Returns the list of base compounds from which this compound inherits */
    virtual QList<CompoundDef> *baseCompounds() const = 0;
    /*! Returns the list of compounds that derive from this one */
    virtual QList<CompoundDef> *derivedCompounds() const = 0;
};

/*! Ordered dictionary of abstract compounds. */
class CompoundSDict : public QList<CompoundDef>
{
    friend class CompoundSDictIterator;
    
  public:
    /*! Create an ordered dictionary.
     *  \param size The size of the dictionary. Should be a prime number for
     *              best distribution of elements.
     */
    CompoundSDict(int size) 
    {
      m_list = new QList<CompoundDef>;
      m_list->setAutoDelete(TRUE);
      m_dict = new QDict<CompoundDef>(size);
    }
    /*! Destroys the dictionary */
   ~CompoundSDict() 
    {
      delete m_list;
      delete m_dict;
    }
    /*! Appends a compound to the dictionary. The element is owned by the
     *  dictionary.
     *  \param key The unique key to use to quicky find the item later on.
     *  \param d The compound to add.
     *  \sa find()
     */
    void append(const char *key,CompoundDef *d)
    {
      m_list->append(d);
      m_dict->insert(key,d);
    }
    /*! Looks up a compound given its key. 
     *  \param key The key to identify this element.
     *  \return The requested compound or zero if it cannot be found.
     *  \sa append() 
     */
    CompoundDef *find(const char *key)
    {
      return m_dict->find(key);
    }

  private:
    QList<CompoundDef> *m_list;
    QDict<CompoundDef> *m_dict;
};

/*! Simple iterator for CompoundSDict. It iterates in the order the 
 *  element were appended.
 */
class CompoundSDictIterator
{
  public:
    /*! Create an iterator given the dictionary. */
    CompoundSDictIterator(const CompoundSDict &dict)
    {
      m_li = new QListIterator<CompoundDef>(*dict.m_list);
    }
    /*! Destroys the dictionary */
   ~CompoundSDictIterator()
    {
      delete m_li;
    }
    /*! Set the iterator to the first element in the list. 
     *  \return The first compound, or zero if the list was empty. 
     */
    CompoundDef *toFirst() const
    {
      return m_li->toFirst();
    }
    /*! Returns the current compound */
    CompoundDef *current() const
    {
      return m_li->current();
    }
    /*! Moves the iterator to the next element.
     *  \return the new "current" element, or zero if the iterator was
     *          already pointing at the last element.
     */
    CompoundDef *operator++()
    {
      return m_li->operator++();
    }
    
  private:
    QListIterator<CompoundDef> *m_li;
};

/*! Concrete compound implementation. Used internally by the sax handlers */
class CompoundDef_Impl : public CompoundDef
{
  public:
    /*! Create an compound implementation object */
    CompoundDef_Impl() 
    { 
      m_type = Unknown_t; 
      m_baseCompounds    = new QList<CompoundDef>;
      m_derivedCompounds = new QList<CompoundDef>;
    }
    /*! Destroy the implementation object */
    ~CompoundDef_Impl() 
    {
      delete m_baseCompounds;
      delete m_derivedCompounds;
    }

    /* implementation of the abstract interface */
    Type type() const { return m_type; }
    QCString name() const { return m_name; }
    QCString id() const { return m_id; }
    QList<CompoundDef> *baseCompounds() const { return m_baseCompounds; }
    QList<CompoundDef> *derivedCompounds() const { return m_derivedCompounds; }

    /*! Sets the type of the compound to \a t. */
    void setType(Type t) { m_type = t; } 
    /*! Sets the name of the compound to \a name. */
    void setName(const char *name) { m_name=name; }
    /*! Sets the Id of the compound to \a id. */ 
    void setId(const char *id) { m_id=id; }
    /*! Adds a new reference to the internal base class reference list */
    void addBaseRef(const char *name) { m_baseRefs.append(name); }
    /*! Adds a new reference to the internal derived class reference list */
    void addDerivedRef(const char *name) { m_derivedRefs.append(name); }

    /*! Resolve the base and derived compound references. */
    void resolveReferences(CompoundSDict *dict)
    {
      char *refId;
      refId = m_baseRefs.first();
      while (refId)
      {
        CompoundDef *cd = dict->find(refId);
        m_baseCompounds->append(cd);
        refId = m_baseRefs.next();
      }
      refId = m_derivedRefs.first();
      while (refId)
      {
        CompoundDef *cd = dict->find(refId);
        m_derivedCompounds->append(cd);
        refId = m_derivedRefs.next();
      }
    }
    
  private:
    Type                m_type;
    QCString            m_name;  
    QCString            m_id;
    QStrList            m_derivedRefs;
    QStrList            m_baseRefs;
    QList<CompoundDef> *m_baseCompounds;
    QList<CompoundDef> *m_derivedCompounds;
};

#endif
