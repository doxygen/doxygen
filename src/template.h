/******************************************************************************
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby
 * granted. No representations are made about the suitability of this software
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * Documents produced by Doxygen are derivative works derived from the
 * input used in their production; they are not affected by this license.
 *
 */

#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <qcstring.h>
#include <qvaluelist.h>

class FTextStream;

class TemplateListIntf;
class TemplateStructIntf;
class TemplateEngine;

/** @defgroup template_api Template API
 *
 *  This is the API for a
 *  <a href="https://docs.djangoproject.com/en/1.6/topics/templates/">Django</a>
 *  compatible template system written in C++.
 *  It is somewhat inspired by Stephen Kelly's
 *  <a href="http://www.gitorious.org/grantlee/pages/Home">Grantlee</a>.
 *
 *  A template is simply a text file.
 *  A template contains \b variables, which get replaced with values when the
 *  template is evaluated, and \b tags, which control the logic of the template.
 *
 *  Variables look like this: `{{ variable }}`
 *  When the template engine encounters a variable, it evaluates that variable and
 *  replaces it with the result. Variable names consist of any combination of
 *  alphanumeric characters and the underscore ("_").
 *  Use a dot (.) to access attributes of a structured variable.
 *
 *  One can modify variables for display by using \b filters, for example:
 *  `{{ value|default:"nothing" }}`
 *
 *  Tags look like this: `{% tag %}`. Tags are more complex than variables:
 *  Some create text in the output, some control flow by performing loops or logic,
 *  and some load external information into the template to be used by later variables.
 *
 *  To comment-out part of a line in a template, use the comment syntax:
 *  `{# comment text #}`.
 *
 *  Supported Django tags:
 *  - `for ... empty ... endfor`
 *  - `if ... else ... endif`
 *  - `block ... endblock`
 *  - `extend`
 *  - `include`
 *  - `with ... endwith`
 *  - `spaceless ... endspaceless`
 *  - `cycle`
 *
 *  Extension tags:
 *  - `create` which instantiates a template and writes the result to a file.
 *     The syntax is `{% create 'filename' from 'template' %}`.
 *  - `recursetree`
 *  - `markers`
 *  - `msg` ... `endmsg`
 *  - `set`
 *
 *  Supported Django filters:
 *  - `default`
 *  - `length`
 *  - `add`
 *  - `divisibleby`
 *
 *  Extension filters:
 *  - `stripPath`
 *  - `nowrap`
 *  - `prepend`
 *  - `append`
 *
 *  @{
 */

/** @brief Variant type which can hold one value of a fixed set of types. */
class TemplateVariant
{
  public:
    /** @brief Helper class to create a delegate that can store a function/method call. */
    class Delegate
    {
      public:
        /** Callback type to use when creating a delegate from a function. */
        typedef TemplateVariant (*StubType)(const void *obj, const QValueList<TemplateVariant> &args);

        Delegate() : m_objectPtr(0) , m_stubPtr(0) {}

        /** Creates a delegate given an object. The method to call is passed as a template parameter */
        template <class T, TemplateVariant (T::*TMethod)(const QValueList<TemplateVariant> &) const>
        static Delegate fromMethod(const T* objectPtr)
        {
          Delegate d;
          d.m_objectPtr = objectPtr;
          d.m_stubPtr   = &methodStub<T, TMethod>;
          return d;
        }
        /** Creates a delegate given an object, and a plain function. */
        static Delegate fromFunction(const void *obj,StubType func)
        {
          Delegate d;
          d.m_objectPtr = obj;
          d.m_stubPtr = func;
          return d;
        }

        /** Invokes the function/method stored in the delegate */
        TemplateVariant operator()(const QValueList<TemplateVariant> &args) const
        {
          return (*m_stubPtr)(m_objectPtr, args);
        }

      private:
        const void* m_objectPtr;
        StubType    m_stubPtr;

        template <class T, TemplateVariant (T::*TMethod)(const QValueList<TemplateVariant> &) const>
        static TemplateVariant methodStub(const void* objectPtr, const QValueList<TemplateVariant> &args)
        {
          T* p = (T*)(objectPtr);
          return (p->*TMethod)(args);
        }
    };

    /** Types of data that can be stored in a TemplateVariant */
    enum Type { None, Bool, Integer, String, Struct, List, Function };

    /** Returns the type of the value stored in the variant */
    Type type() const { return m_type; }

    /** Return a string representation of the type of the value stored in the variant */
    QCString typeAsString() const
    {
      switch (m_type)
      {
        case None:     return "none";
        case Bool:     return "bool";
        case Integer:  return "integer";
        case String:   return "string";
        case Struct:   return "struct";
        case List:     return "list";
        case Function: return "function";
      }
      return "invalid";
    }

    /** Returns TRUE if the variant holds a valid value, or FALSE otherwise */
    bool isValid() const { return m_type!=None; }

    /** Constructs an invalid variant. */
    TemplateVariant() : m_type(None), m_strukt(0), m_raw(FALSE) {}

    /** Constructs a new variant with a boolean value \a b. */
    explicit TemplateVariant(bool b) : m_type(Bool), m_boolVal(b), m_raw(FALSE) {}

    /** Constructs a new variant with a integer value \a v. */
    TemplateVariant(int v) : m_type(Integer), m_intVal(v), m_raw(FALSE) {}

    /** Constructs a new variant with a string value \a s. */
    TemplateVariant(const char *s,bool raw=FALSE) : m_type(String), m_strVal(s), m_strukt(0), m_raw(raw) {}

    /** Constructs a new variant with a string value \a s. */
    TemplateVariant(const QCString &s,bool raw=FALSE) : m_type(String), m_strVal(s), m_strukt(0), m_raw(raw) {}

    /** Constructs a new variant with a struct value \a s.
     *  @note. The variant will hold a reference to the object.
     */
    TemplateVariant(TemplateStructIntf *s);

    /** Constructs a new variant with a list value \a l.
     *  @note. The variant will hold a reference to the object.
     */
    TemplateVariant(TemplateListIntf *l);

    /** Constructs a new variant which represents a method call
     *  @param[in] delegate Delegate object to invoke when
     *             calling call() on this variant.
     *  @note Use TemplateVariant::Delegate::fromMethod() and
     *  TemplateVariant::Delegate::fromFunction() to create
     *  Delegate objects.
     */
    TemplateVariant(const Delegate &delegate) : m_type(Function), m_strukt(0), m_delegate(delegate), m_raw(FALSE) {}

    /** Destroys the Variant object */
    ~TemplateVariant();

    /** Constructs a copy of the variant, \a v,
     *  passed as the argument to this constructor.
     */
    TemplateVariant(const TemplateVariant &v);

    /** Assigns the value of the variant \a v to this variant. */
    TemplateVariant &operator=(const TemplateVariant &v);

    /** Compares this QVariant with v and returns true if they are equal;
     *  otherwise returns false.
     */
    bool operator==(TemplateVariant &other)
    {
      if (m_type==None)
      {
        return FALSE;
      }
      if (m_type==TemplateVariant::List && other.m_type==TemplateVariant::List)
      {
        return m_list==other.m_list; // TODO: improve me
      }
      else if (m_type==TemplateVariant::Struct && other.m_type==TemplateVariant::Struct)
      {
        return m_strukt==other.m_strukt; // TODO: improve me
      }
      else
      {
        return toString()==other.toString();
      }
    }

    /** Returns the variant as a string. */
    QCString toString() const
    {
      switch (m_type)
      {
        case None:     return QCString();
        case Bool:     return m_boolVal ? "true" : "false";
        case Integer:  return QCString().setNum(m_intVal);
        case String:   return m_strVal;
        case Struct:   return "[struct]";
        case List:     return "[list]";
        case Function: return "[function]";
      }
      return QCString();
    }

    /** Returns the variant as a boolean. */
    bool toBool() const;

    /** Returns the variant as an integer. */
    int toInt() const;

    /** Returns the pointer to list referenced by this variant
     *  or 0 if this variant does not have list type.
     */
    TemplateListIntf   *toList() const
    {
      return m_type==List ? m_list : 0;
    }

    /** Returns the pointer to struct referenced by this variant
     *  or 0 if this variant does not have struct type.
     */
    TemplateStructIntf *toStruct() const
    {
      return m_type==Struct ? m_strukt : 0;
    }

    /** Return the result of apply this function with \a args.
     *  Returns an empty string if the variant type is not a function.
     */
    TemplateVariant call(const QValueList<TemplateVariant> &args)
    {
      if (m_type==Function) return m_delegate(args);
      return TemplateVariant();
    }

    /** Sets whether or not the value of the Variant should be
     *  escaped or written as-is (raw).
     *  @param[in] b TRUE means write as-is, FALSE means apply escaping.
     */
    void setRaw(bool b) { m_raw = b; }

    /** Returns whether or not the value of the Value is raw.
     *  @see setRaw()
     */
    bool raw() const { return m_raw; }

  private:
    Type                  m_type;
    QCString              m_strVal;
    union
    {
      int                 m_intVal;
      bool                m_boolVal;
      TemplateStructIntf *m_strukt;
      TemplateListIntf   *m_list;
    };
    Delegate              m_delegate;
    bool                  m_raw;
};

//------------------------------------------------------------------------

template<class T> class TemplateAutoRef
{
  public:
    TemplateAutoRef(T *obj) : m_obj(obj)
    {
      m_obj->addRef();
    }
   ~TemplateAutoRef()
    {
      m_obj->release();
    }
    T &operator*() const { return *m_obj; }
    T *operator->() const { return m_obj; }
    T *get() const { return m_obj; }

  private:
   T *m_obj;
};

//------------------------------------------------------------------------

/** @brief Abstract read-only interface for a context value of type list.
 *  @note The values of the list are TemplateVariants.
 */
class TemplateListIntf
{
  public:
    /** @brief Abstract interface for a iterator of a list. */
    class ConstIterator
    {
      public:
        /** Destructor for the iterator */
        virtual ~ConstIterator() {}
        /** Moves iterator to the first element in the list */
        virtual void toFirst() = 0;
        /** Moves iterator to the last element in the list */
        virtual void toLast() = 0;
        /** Moves iterator to the next element in the list */
        virtual void toNext() = 0;
        /** Moves iterator to the previous element in the list */
        virtual void toPrev() = 0;
        /* Returns TRUE if the iterator points to a valid element
         * in the list, or FALSE otherwise.
         * If TRUE is returned, the value pointed to be the
         * iterator is assigned to \a v.
         */
        virtual bool current(TemplateVariant &v) const = 0;
    };

    /** Destroys the list */
    virtual ~TemplateListIntf() {}

    /** Returns the number of elements in the list */
    virtual int count() const = 0;

    /** Returns the element at index position \a index. */
    virtual TemplateVariant  at(int index) const = 0;

    /** Creates a new iterator for this list.
     *  @note the user should call delete on the returned pointer.
     */
    virtual TemplateListIntf::ConstIterator *createIterator() const = 0;

    /** Increase object's reference count */
    virtual int addRef() = 0;

    /** Decreases object's referenc count, destroy object if 0 */
    virtual int release() = 0;
};

/** @brief Default implementation of a context value of type list. */
class TemplateList : public TemplateListIntf
{
  public:
    // TemplateListIntf methods
    virtual int  count() const;
    virtual TemplateVariant at(int index) const;
    virtual TemplateListIntf::ConstIterator *createIterator() const;
    virtual int addRef();
    virtual int release();

    /** Creates an instance with ref count set to 0 */
    static TemplateList *alloc();

    /** Appends element \a v to the end of the list */
    virtual void append(const TemplateVariant &v);

  private:
    /** Creates a list */
    TemplateList();
    /** Destroys the list */
   ~TemplateList();

    friend class TemplateListConstIterator;
    class Private;
    Private *p;
};

//------------------------------------------------------------------------

/** @brief Abstract interface for a context value of type struct. */
class TemplateStructIntf
{
  public:
    /** Destroys the struct */
    virtual ~TemplateStructIntf() {}

    /** Gets the value for a field name.
     *  @param[in] name The name of the field.
     */
    virtual TemplateVariant get(const char *name) const = 0;

    /** Increase object's reference count */
    virtual int addRef() = 0;

    /** Decreases object's referenc count, destroy object if 0 */
    virtual int release() = 0;
};


/** @brief Default implementation of a context value of type struct. */
class TemplateStruct : public TemplateStructIntf
{
  public:
    // TemplateStructIntf methods
    virtual TemplateVariant get(const char *name) const;
    virtual int addRef();
    virtual int release();

    /** Creates an instance with ref count set to 0. */
    static TemplateStruct *alloc();

    /** Sets the value the field of a struct
     *  @param[in] name The name of the field.
     *  @param[in] v The value to set.
     */
    virtual void set(const char *name,const TemplateVariant &v);


  private:
    /** Creates a struct */
    TemplateStruct();
    /** Destroys the struct */
    virtual ~TemplateStruct();

    class Private;
    Private *p;
};

//------------------------------------------------------------------------

/** @brief Interface used to escape characters in a string */
class TemplateEscapeIntf
{
  public:
    /** Returns the \a input after escaping certain characters */
    virtual QCString escape(const QCString &input) = 0;
    /** Setting tabbing mode on or off (for LaTeX) */
    virtual void enableTabbing(bool b) = 0;
};

//------------------------------------------------------------------------

/** @brief Interface used to remove redundant spaces inside a spaceless block */
class TemplateSpacelessIntf
{
  public:
    /** Returns the \a input after removing redundant whitespace */
    virtual QCString remove(const QCString &input) = 0;
    /** Reset filter state */
    virtual void reset() = 0;
};

//------------------------------------------------------------------------

/** @brief Abstract interface for a template context.
 *
 *  A Context consists of a stack of dictionaries.
 *  A dictionary consists of a mapping of string keys onto TemplateVariant values.
 *  A key is searched starting with the dictionary at the top of the stack
 *  and searching downwards until it is found. The stack is used to create
 *  local scopes.
 *  @note This object must be created by TemplateEngine::createContext()
 */
class TemplateContext
{
  public:
    virtual ~TemplateContext() {}

    /** Push a new scope on the stack. */
    virtual void push() = 0;

    /** Pop the current scope from the stack. */
    virtual void pop() = 0;

    /** Sets a value in the current scope.
     *  @param[in] name The name of the value; the key in the dictionary.
     *  @param[in] v The value associated with the key.
     *  @note When a given key is already present,
     *  its value will be replaced by \a v
     */
    virtual void set(const char *name,const TemplateVariant &v) = 0;

    /** Gets the value for a given key
     *  @param[in] name The name of key.
     *  @returns The value, which can be an invalid variant in case the
     *  key was not found.
     */
    virtual TemplateVariant get(const QCString &name) const = 0;

    /** Returns a pointer to the value corresponding to a given key.
     *  @param[in] name The name of key.
     *  @returns A pointer to the value, or 0 in case the key was not found.
     */
    virtual const TemplateVariant *getRef(const QCString &name) const = 0;

    /** When files are created (i.e. by {% create ... %}) they written
     *  to the directory \a dir.
     */
    virtual void setOutputDirectory(const QCString &dir) = 0;

    /** Sets the interface that will be used for escaping the result
     *  of variable expansion before writing it to the output.
     */
    virtual void setEscapeIntf(const QCString &extension, TemplateEscapeIntf *intf) = 0;

    /** Sets the interface that will be used inside a spaceless block
     *  to remove any redundant whitespace.
     */
    virtual void setSpacelessIntf(TemplateSpacelessIntf *intf) = 0;
};

//------------------------------------------------------------------------

/** @brief Abstract interface for a template.
 *  @note Must be created and is deleted by the TemplateEngine
 */
class Template
{
  public:
    /** Destructor */
    virtual ~Template() {}

    /** Renders a template instance to a stream.
     *  @param[in] ts The text stream to write the results to.
     *  @param[in] c The context containing data that can be used
     *  when instantiating the template.
     */
    virtual void render(FTextStream &ts,TemplateContext *c) = 0;
};

//------------------------------------------------------------------------

/** @brief Engine to create templates and template contexts. */
class TemplateEngine
{
  public:
    /** Create a template engine. */
    TemplateEngine();

    /** Destroys the template engine. */
   ~TemplateEngine();

    /** Creates a new context that can be using to render a template.
     *  @see Template::render()
     */
    TemplateContext *createContext() const;

    /** Destroys a context created via createContext().
     *  @param[in] ctx The context.
     */
    void destroyContext(TemplateContext *ctx);

    /** Creates a new template whose contents are in a file.
     *  @param[in] fileName The name of the file containing the template data
     *  @param[in] fromLine The line number of the statement that triggered the load
     *  @return the new template, the engine will keep ownership of the object.
     */
    Template *loadByName(const QCString &fileName,int fromLine);

    /** Indicates that template \a t is no longer needed. The engine
     *  may decide to delete it.
     */
    void unload(Template *t);

    /** Prints the current template file include stack */
    void printIncludeContext(const char *fileName,int line) const;

  private:
    friend class TemplateNodeBlock;
    friend class TemplateNodeCreate;

    void enterBlock(const QCString &fileName,const QCString &blockName,int line);
    void leaveBlock();

    /** Sets the extension of the output file. This is used to control the
     *  format of 'special' tags in the template
     */
    void setOutputExtension(const char *extension);

    /** Returns the output extension, set via setOutputExtension() */
    QCString outputExtension() const;

    class Private;
    Private *p;
};

/** @} */

#endif
