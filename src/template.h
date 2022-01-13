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

#include <vector>
#include <memory>
#include <functional>

#include "qcstring.h"
#include "containers.h"
#include "variant.h"

class TemplateListIntf;
class TemplateStructIntf;
class TemplateEngine;
class TextStream;

using TemplateListIntfPtr       = std::shared_ptr<TemplateListIntf>;
using TemplateStructIntfPtr     = std::shared_ptr<TemplateStructIntf>;
using TemplateStructIntfWeakPtr = std::weak_ptr<TemplateStructIntf>;

/** @defgroup template_api Template API
 *
 *  This is the API for a
 *  <a href="https://www.djangoproject.com/">Django</a>
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
    /** Type representing a function call in a template */
    using FunctionDelegate = std::function<TemplateVariant(const std::vector<TemplateVariant>&)>;

    /** Symbolic names for the possible types that this variant can hold. */
    using VariantT = Variant<bool,                      // index==0: Type::Bool
                             int,                       // index==1: Type::Int
                             QCString,                  // index==2: Type::String
                             TemplateStructIntfPtr,     // index==3: Type::Struct
                             TemplateListIntfPtr,       // index==4: Type::List
                             FunctionDelegate,          // index==5: Type::Function
                             TemplateStructIntfWeakPtr  // index==6: Type::WeakStruct
                            >;

    enum class Type : uint8_t
    {
      Bool       = 0,
      Int        = 1,
      String     = 2,
      Struct     = 3,
      List       = 4,
      Function   = 5,
      WeakStruct = 6,
      None       = 255
    };

    /** Constructs an invalid variant. */
    TemplateVariant() {}

    /** Constructs a new variant with a boolean value \a b. */
    explicit TemplateVariant(bool b) { m_variant.set<static_cast<uint8_t>(Type::Bool)>(b); }

    /** Constructs a new variant with a integer value \a v. */
    TemplateVariant(int v) { m_variant.set<static_cast<uint8_t>(Type::Int)>(v); }

    /** Constructs a new variant with a integer value \a v. */
    TemplateVariant(unsigned int v) { m_variant.set<static_cast<uint8_t>(Type::Int)>(static_cast<int>(v)); }

    /** Constructs a new variant with a integer value \a v.
     *  We use SFINAE to avoid a compiler error in case size_t already matches the 'unsigned int' overload.
     */
    template<typename T,
             typename std::enable_if<std::is_same<T,size_t>::value,T>::type* = nullptr
            >
    TemplateVariant(T v) { m_variant.set<static_cast<uint8_t>(Type::Int)>(static_cast<int>(v)); }

    /** Constructs a new variant with a string value \a s. */
    TemplateVariant(const char *s,bool raw=FALSE) : m_raw(raw) { m_variant.set<static_cast<uint8_t>(Type::String)>(s); }

    /** Constructs a new variant with a string value \a s. */
    TemplateVariant(const QCString &s,bool raw=FALSE) : m_raw(raw) { m_variant.set<static_cast<uint8_t>(Type::String)>(s.str()); }

    /** Constructs a new variant with a string value \a s. */
    TemplateVariant(const std::string &s,bool raw=FALSE) : m_raw(raw) { m_variant.set<static_cast<uint8_t>(Type::String)>(s); }

    /** Constructs a new variant with a struct value \a s.
     *  @note. The variant will hold a counting reference to the object.
     */
    TemplateVariant(TemplateStructIntfPtr s) { m_variant.set<static_cast<uint8_t>(Type::Struct)>(s); }

    /** Constructs a new variant with a list value \a l.
     *  @note. The variant will hold a counting reference to the object.
     */
    TemplateVariant(TemplateListIntfPtr l) { m_variant.set<static_cast<uint8_t>(Type::List)>(l); }

    /** Constructs a new variant with a struct value \a s.
     *  @note. The variant will hold a non-counting reference to the object.
     */
    TemplateVariant(TemplateStructIntfWeakPtr s) { m_variant.set<static_cast<uint8_t>(Type::WeakStruct)>(s); }

    /** Constructs a new variant which represents a method call
     *  @param[in] delegate FunctionDelegate object to invoke when
     *             calling call() on this variant.
     *  @note Use TemplateVariant::FunctionDelegate::fromMethod() and
     *  TemplateVariant::FunctionDelegate::fromFunction() to create
     *  FunctionDelegate objects.
     */
    TemplateVariant(FunctionDelegate delegate) { m_variant.set<static_cast<uint8_t>(Type::Function)>(delegate); }

    /** Destroys the Variant object */
    ~TemplateVariant()  = default;

    /** Constructs a copy of the variant, \a v,
     *  passed as the argument to this constructor.
     */
    TemplateVariant(const TemplateVariant &v) = default;

    /** Moves the contents of variant \a v into this variant.
     *  variant \a v will become invalid
     */
    TemplateVariant(TemplateVariant &&v);

    /** Assigns the value of the variant \a v to this variant. */
    TemplateVariant &operator=(const TemplateVariant &v) = default;

    /** Move the value of the variant \a v into this variant.
     *  Variant \a v will become invalid */
    TemplateVariant &operator=(TemplateVariant &&v);

    /** Compares this QVariant with v and returns true if they are equal;
     *  otherwise returns false.
     */
    bool operator==(TemplateVariant &other) const;

    /** Returns the variant as a string. */
    QCString toString() const;

    /** Returns the variant as a boolean. */
    bool toBool() const;

    /** Returns the variant as an integer. */
    int toInt() const;

    /** Returns TRUE if the variant holds a valid value, or FALSE otherwise */
    constexpr bool isValid()      const { return m_variant.valid(); }
    /** Returns TRUE if the variant holds a boolean value */
    constexpr bool isBool()       const { return m_variant.is<static_cast<uint8_t>(Type::Bool)>(); }
    /** Returns TRUE if the variant holds an integer value */
    constexpr bool isInt()        const { return m_variant.is<static_cast<uint8_t>(Type::Int)>(); }
    /** Returns TRUE if the variant holds a string value */
    constexpr bool isString()     const { return m_variant.is<static_cast<uint8_t>(Type::String)>(); }
    /** Returns TRUE if the variant holds a struct value */
    constexpr bool isStruct()     const { return m_variant.is<static_cast<uint8_t>(Type::Struct)>(); }
    /** Returns TRUE if the variant holds a list value */
    constexpr bool isList()       const { return m_variant.is<static_cast<uint8_t>(Type::List)>(); }
    /** Returns TRUE if the variant holds a function value */
    constexpr bool isFunction()   const { return m_variant.is<static_cast<uint8_t>(Type::Function)>(); }
    /** Returns TRUE if the variant holds a struct value */
    constexpr bool isWeakStruct() const { return m_variant.is<static_cast<uint8_t>(Type::WeakStruct)>(); }

    /** Returns the pointer to list referenced by this variant
     *  or 0 if this variant does not have list type.
     */
    TemplateListIntfPtr toList();
    const TemplateListIntfPtr toList() const;

    /** Returns the pointer to struct referenced by this variant
     *  or 0 if this variant does not have struct type.
     */
    TemplateStructIntfPtr toStruct();
    const TemplateStructIntfPtr toStruct() const;

    /** Return the result of apply this function with \a args.
     *  Returns an empty string if the variant type is not a function.
     */
    TemplateVariant call(const std::vector<TemplateVariant> &args = std::vector<TemplateVariant>());

    /** Sets whether or not the value of the Variant should be
     *  escaped or written as-is (raw).
     *  @param[in] b TRUE means write as-is, FALSE means apply escaping.
     */
    void setRaw(bool b) { m_raw = b; }

    /** Returns whether or not the value of the Value is raw.
     *  @see setRaw()
     */
    constexpr bool raw() const { return m_raw; }

    /** Returns the type held by this variant */
    constexpr Type type() const { return static_cast<Type>(m_variant.index()); }

    /** Returns a string representation of this variant's type */
    const char *typeAsString() const;

  private:
    QCString listToString() const;
    QCString structToString() const;

    VariantT              m_variant;
    bool                  m_raw = false;
};

using TemplateVariantList = std::vector<TemplateVariant>;

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
    using ConstIteratorPtr = std::unique_ptr<ConstIterator>;

    /** Destroys the list */
    virtual ~TemplateListIntf() {}

    /** Returns the number of elements in the list */
    virtual size_t count() const = 0;

    /** Returns the element at index position \a index. */
    virtual TemplateVariant  at(size_t index) const = 0;

    /** Creates a new iterator for this list.
     *  @note the user should call delete on the returned pointer.
     */
    virtual TemplateListIntf::ConstIteratorPtr createIterator() const = 0;

};

/** @brief Default implementation of a immutable context value of type list. */
class TemplateImmutableList : public TemplateListIntf
{
  public:
    // TemplateListIntf methods
    virtual size_t count() const;
    virtual TemplateVariant at(size_t index) const;
    virtual TemplateListIntf::ConstIteratorPtr createIterator() const;

    /** Creates an instance and returns a shared pointer to it */
    static TemplateListIntfPtr alloc(std::initializer_list<TemplateVariant> elements);
    static TemplateListIntfPtr alloc(const std::vector<TemplateVariant> &elements);

    /** Creates a list */
    TemplateImmutableList(std::initializer_list<TemplateVariant> elements);
    TemplateImmutableList(const std::vector<TemplateVariant> &elements);
    /** Destroys the list */
    virtual ~TemplateImmutableList();

  private:
    class Private;
    std::unique_ptr<Private> p;
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
    virtual TemplateVariant get(const QCString &name) const = 0;

    /** Return the list of fields. */
    virtual StringVector fields() const = 0;
};

//------------------------------------------------------------------------

/** @brief Default implementation of an immutable context value of type struct. */
class TemplateImmutableStruct : public TemplateStructIntf
{
  public:
    // TemplateStructIntf methods
    virtual TemplateVariant get(const QCString &name) const;
    virtual StringVector fields() const;

    using StructField = std::pair<const std::string,TemplateVariant>;

    /** Creates an instance and returns a shared pointer to it
     *  @param fields the fields of the struct as key/value pairs.
     */
    static TemplateStructIntfPtr alloc(std::initializer_list<StructField> fields);

    /** Creates a struct */
    TemplateImmutableStruct(std::initializer_list<StructField> fields);
    /** Destroys the struct */
    virtual ~TemplateImmutableStruct();

  private:

    class Private;
    std::unique_ptr<Private> p;
};


//------------------------------------------------------------------------

/** @brief Interface used to escape characters in a string */
class TemplateEscapeIntf
{
  public:
    virtual ~TemplateEscapeIntf() {}
    /** Create a copy of the escape filter */
    virtual std::unique_ptr<TemplateEscapeIntf> clone() = 0;
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
    virtual ~TemplateSpacelessIntf() {}
    /** Create a copy of the spaceless filter */
    virtual std::unique_ptr<TemplateSpacelessIntf> clone() = 0;
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
    virtual void set(const QCString &name,const TemplateVariant &v) = 0;

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
    virtual void setEscapeIntf(const QCString &extension, std::unique_ptr<TemplateEscapeIntf> intf) = 0;

    /** Sets the interface that will be used inside a spaceless block
     *  to remove any redundant whitespace.
     */
    virtual void setSpacelessIntf(std::unique_ptr<TemplateSpacelessIntf> intf) = 0;
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
    virtual void render(TextStream &ts,TemplateContext *c) = 0;
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
    std::unique_ptr<TemplateContext> createContext() const;

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
    void printIncludeContext(const QCString &fileName,int line) const;

    /** Sets the search directory where to look for template files */
    void setTemplateDir(const QCString &dirName);

  private:
    friend class TemplateNodeBlock;
    friend class TemplateNodeCreate;

    void enterBlock(const QCString &fileName,const QCString &blockName,int line);
    void leaveBlock();

    /** Sets the extension of the output file. This is used to control the
     *  format of 'special' tags in the template
     */
    void setOutputExtension(const QCString &extension);

    /** Returns the output extension, set via setOutputExtension() */
    QCString outputExtension() const;

    class Private;
    std::unique_ptr<Private> p;
};

/** @} */

#endif
