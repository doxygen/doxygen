/******************************************************************************
 *
 * 
 *
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

#ifndef CONFIGIMPL_H
#define CONFIGIMPL_H

#include <qstrlist.h>
#include <qdict.h>
#include <qlist.h>
#include <qregexp.h>
#include "ftextstream.h"


/** Abstract base class for any configuration option.
 */
class ConfigOption
{
    friend class ConfigImpl;

  public:

    /*! The type of option */
    enum OptionType 
    { 
      O_Info,      //<! A section header
      O_List,      //<! A list of items
      O_Enum,      //<! A fixed set of items
      O_String,    //<! A single item
      O_Int,       //<! An integer value
      O_Bool,      //<! A boolean value
      O_Obsolete,  //<! An obsolete option
      O_Disabled   //<! Disabled compile time option
    };
    enum 
    { 
     /*! Maximum length of an option in the config file. Used for 
      *  alignment purposes.
      */
      MAX_OPTION_LENGTH = 23  
    };
    ConfigOption(OptionType t) : m_kind(t) 
    {
      m_spaces.fill(' ',40);
    }
    virtual ~ConfigOption()
    {
    }

    /*! returns the kind of option this is. */
    OptionType kind() const { return m_kind; }
    QCString name() const { return m_name; }
    QCString docs() const { return m_doc; }

    QCString dependsOn() const { return m_dependency; }
    void addDependency(const char *dep) { m_dependency = dep; }
    void setEncoding(const QCString &e) { m_encoding = e; }
    void setUserComment(const QCString &u) { m_userComment += u; }

  protected:
    virtual void writeTemplate(FTextStream &t,bool sl,bool upd) = 0;
    virtual void convertStrToVal() {}
    virtual void substEnvVars() = 0;
    virtual void init() {}

    void writeBoolValue(FTextStream &t,bool v);
    void writeIntValue(FTextStream &t,int i);
    void writeStringValue(FTextStream &t,QCString &s);
    void writeStringList(FTextStream &t,QStrList &l);

    QCString m_spaces;
    QCString m_name;
    QCString m_doc;
    QCString m_dependency;
    QCString m_encoding;
    QCString m_userComment;
    OptionType m_kind;
};

/** Section marker for grouping the configuration options.
 */
class ConfigInfo : public ConfigOption
{
  public:
    ConfigInfo(const char *name,const char *doc) 
      : ConfigOption(O_Info)
    {
      m_name = name;
      m_doc = doc;
    }
    void writeTemplate(FTextStream &t, bool sl,bool);
    void substEnvVars() {}
};

/** Class respresenting a list type option.
 */
class ConfigList : public ConfigOption
{
  public:
    enum WidgetType { String, File, Dir, FileAndDir };
    ConfigList(const char *name,const char *doc) 
      : ConfigOption(O_List)
    {
      m_name = name;
      m_doc = doc;
      m_widgetType = String;
    }
    void addValue(const char *v) { m_value.append(v); }
    void setWidgetType(WidgetType w) { m_widgetType = w; }
    WidgetType widgetType() const { return m_widgetType; }
    QStrList *valueRef() { return &m_value; }
    void writeTemplate(FTextStream &t,bool sl,bool);
    void substEnvVars();
    void init() { m_value.clear(); }
  private:
    QStrList m_value;
    WidgetType m_widgetType;
};

/** Class representing an enum type option.
 */
class ConfigEnum : public ConfigOption
{
  public:
    ConfigEnum(const char *name,const char *doc,const char *defVal) 
      : ConfigOption(O_Enum)
    {
      m_name = name;
      m_doc = doc;
      m_value = defVal;
      m_defValue = defVal;
    }
    void addValue(const char *v) { m_valueRange.append(v); }
    QStrListIterator iterator() 
    {
      return QStrListIterator(m_valueRange);
    }
    QCString *valueRef() { return &m_value; }
    void substEnvVars();
    void writeTemplate(FTextStream &t,bool sl,bool);
    void init() { m_value = m_defValue.copy(); }

  private:
    QStrList m_valueRange;
    QCString m_value;
    QCString m_defValue;
};

/** Class representing a string type option.
 */
class ConfigString : public ConfigOption
{
  public:
    enum WidgetType { String, File, Dir, Image };
    ConfigString(const char *name,const char *doc) 
      : ConfigOption(O_String)
    {
      m_name = name;
      m_doc = doc;
      m_widgetType = String;
    }
   ~ConfigString()
    {
    }
    void setWidgetType(WidgetType w) { m_widgetType = w; }
    WidgetType widgetType() const { return m_widgetType; }
    void setDefaultValue(const char *v) { m_defValue = v; }
    QCString *valueRef() { return &m_value; }
    void writeTemplate(FTextStream &t,bool sl,bool);
    void substEnvVars();
    void init() { m_value = m_defValue.copy(); }
  
  private:
    QCString m_value;
    QCString m_defValue;
    WidgetType m_widgetType;
};

/** Class representing an integer type option.
 */
class ConfigInt : public ConfigOption
{
  public:
    ConfigInt(const char *name,const char *doc,int minVal,int maxVal,int defVal) 
      : ConfigOption(O_Int)
    {
      m_name = name;
      m_doc = doc;
      m_value = defVal;
      m_defValue = defVal;
      m_minVal = minVal;
      m_maxVal = maxVal;
    }
    QCString *valueStringRef() { return &m_valueString; }
    int *valueRef() { return &m_value; }
    int minVal() const { return m_minVal; }
    int maxVal() const { return m_maxVal; }
    void convertStrToVal();
    void substEnvVars();
    void writeTemplate(FTextStream &t,bool sl,bool upd);
    void init() { m_value = m_defValue; }
  private:
    int m_value;
    int m_defValue;
    int m_minVal;
    int m_maxVal;
    QCString m_valueString;
};

/** Class representing a Boolean type option.
 */
class ConfigBool : public ConfigOption
{
  public:
    ConfigBool(const char *name,const char *doc,bool defVal) 
      : ConfigOption(O_Bool)
    {
      m_name = name;
      m_doc = doc;
      m_value = defVal;
      m_defValue = defVal;
    }
    QCString *valueStringRef() { return &m_valueString; }
    bool *valueRef() { return &m_value; }
    void convertStrToVal();
    void substEnvVars();
    void setValueString(const QCString &v) { m_valueString = v; }
    void writeTemplate(FTextStream &t,bool sl,bool upd);
    void init() { m_value = m_defValue; }
  private:
    bool m_value;
    bool m_defValue;
    QCString m_valueString;
};

/** Section marker for obsolete options
 */
class ConfigObsolete : public ConfigOption
{
  public:
    ConfigObsolete(const char *name) : ConfigOption(O_Obsolete)  
    { m_name = name; }
    void writeTemplate(FTextStream &,bool,bool);
    void substEnvVars() {}
};

/** Section marker for compile time optional options
 */
class ConfigDisabled : public ConfigOption
{
  public:
    ConfigDisabled(const char *name) : ConfigOption(O_Disabled)  
    { m_name = name; }
    void writeTemplate(FTextStream &,bool,bool);
    void substEnvVars() {}
};

// some convenience macros for access the config options
#define ConfigImpl_getString(val)  ConfigImpl::instance()->getString(__FILE__,__LINE__,val)
#define ConfigImpl_getInt(val)     ConfigImpl::instance()->getInt(__FILE__,__LINE__,val)
#define ConfigImpl_getList(val)    ConfigImpl::instance()->getList(__FILE__,__LINE__,val)
#define ConfigImpl_getEnum(val)    ConfigImpl::instance()->getEnum(__FILE__,__LINE__,val)
#define ConfigImpl_getBool(val)    ConfigImpl::instance()->getBool(__FILE__,__LINE__,val)


/** Singleton for configuration variables.
 *
 *  This object holds the global static variables
 *  read from a user-supplied configuration file.
 *  The static member instance() can be used to get
 *  a pointer to the one and only instance.
 *  
 *  Set all variables to their default values by
 *  calling Config::instance()->init()
 *
 */
class ConfigImpl
{
  public:
    /////////////////////////////
    // public API
    /////////////////////////////

    /*! Returns the one and only instance of this class */
    static ConfigImpl *instance()
    {
      if (m_instance==0) m_instance = new ConfigImpl;
      return m_instance;
    }
    /*! Delete the instance */
    static void deleteInstance()
    {
      delete m_instance;
      m_instance=0;
    }
    
    /*! Returns an iterator that can by used to iterate over the 
     *  configuration options.
     */
    QListIterator<ConfigOption> iterator()
    {
      return QListIterator<ConfigOption>(*m_options);
    }

    /*! 
     *  @name Getting configuration values.
     *  @{
     */

    /*! Returns the value of the string option with name \a fileName. 
     *  The arguments \a num and \a name are for debugging purposes only.
     *  There is a convenience function Config_getString() for this.
     */
    QCString &getString(const char *fileName,int num,const char *name) const;

    /*! Returns the value of the list option with name \a fileName. 
     *  The arguments \a num and \a name are for debugging purposes only.
     *  There is a convenience function Config_getList() for this.
     */
    QStrList &getList(const char *fileName,int num,const char *name) const;

    /*! Returns the value of the enum option with name \a fileName. 
     *  The arguments \a num and \a name are for debugging purposes only.
     *  There is a convenience function Config_getEnum() for this.
     */
    QCString &getEnum(const char *fileName,int num,const char *name) const;

    /*! Returns the value of the integer option with name \a fileName. 
     *  The arguments \a num and \a name are for debugging purposes only.
     *  There is a convenience function Config_getInt() for this.
     */
    int      &getInt(const char *fileName,int num,const char *name) const;

    /*! Returns the value of the boolean option with name \a fileName. 
     *  The arguments \a num and \a name are for debugging purposes only.
     *  There is a convenience function Config_getBool() for this.
     */
    bool     &getBool(const char *fileName,int num,const char *name) const;

    /*! Returns the ConfigOption corresponding with \a name or 0 if
     *  the option is not supported.
     */
    ConfigOption *get(const char *name) const
    {
      return m_dict->find(name); 
    }
    /* @} */

    /*! 
     *  @name Adding configuration options. 
     *  @{
     */

    /*! Starts a new configuration section with \a name and description \a doc.
     *  \returns An object representing the option.
     */
    ConfigInfo   *addInfo(const char *name,const char *doc)
    {
      ConfigInfo *result = new ConfigInfo(name,doc);
      m_options->append(result);
      return result;
    }

    /*! Adds a new string option with \a name and documentation \a doc.
     *  \returns An object representing the option.
     */
    ConfigString *addString(const char *name,
                            const char *doc)
    {
      ConfigString *result = new ConfigString(name,doc);
      m_options->append(result);
      m_dict->insert(name,result);
      return result;
    }

    /*! Adds a new enumeration option with \a name and documentation \a doc
     *  and initial value \a defVal. 
     *  \returns An object representing the option.
     */
    ConfigEnum   *addEnum(const char *name,
                          const char *doc,
                          const char *defVal)
    {
      ConfigEnum *result = new ConfigEnum(name,doc,defVal);
      m_options->append(result);
      m_dict->insert(name,result);
      return result;
    }

    /*! Adds a new string option with \a name and documentation \a doc.
     *  \returns An object representing the option.
     */
    ConfigList   *addList(const char *name,
                          const char *doc)
    {
      ConfigList *result = new ConfigList(name,doc);
      m_options->append(result);
      m_dict->insert(name,result);
      return result;
    }

    /*! Adds a new integer option with \a name and documentation \a doc.
     *  The integer has a range between \a minVal and \a maxVal and a
     *  default value of \a defVal.
     *  \returns An object representing the option.
     */
    ConfigInt    *addInt(const char *name,
                         const char *doc,
                         int minVal,int maxVal,int defVal)
    {
      ConfigInt *result = new ConfigInt(name,doc,minVal,maxVal,defVal);
      m_options->append(result);
      m_dict->insert(name,result);
      return result;
    }

    /*! Adds a new boolean option with \a name and documentation \a doc.
     *  The boolean has a default value of \a defVal.
     *  \returns An object representing the option.
     */
    ConfigBool   *addBool(const char *name,
                          const char *doc,
                          bool defVal)
    {
      ConfigBool *result = new ConfigBool(name,doc,defVal);
      m_options->append(result);
      m_dict->insert(name,result);
      return result;
    }
    /*! Adds an option that has become obsolete. */
    ConfigOption *addObsolete(const char *name)
    {
      ConfigObsolete *option = new ConfigObsolete(name);
      m_dict->insert(name,option);
      m_obsolete->append(option);
      return option;
    }
    /*! Adds an option that has been disabled at compile time. */
    ConfigOption *addDisabled(const char *name)
    {
      ConfigDisabled *option = new ConfigDisabled(name);
      m_dict->insert(name,option);
      m_disabled->append(option);
      return option;
    }
    /*! @} */

    /*! Writes a template configuration to stream \a t. If \a shortIndex
     *  is \c TRUE the description of each configuration option will
     *  be omitted.
     */
    void writeTemplate(FTextStream &t,bool shortIndex,bool updateOnly);

    void setHeader(const char *header) { m_header = header; }

    /////////////////////////////
    // internal API
    /////////////////////////////

    /*! Converts the string values read from the configuration file
     *  to real values for non-string type options (like int, and bools)
     */
    void convertStrToVal();

    /*! Replaces references to environment variable by the actual value
     *  of the environment variable.
     */
    void substituteEnvironmentVars();

    /*! Initialize config variables to their default value */
    void init();

    /*! Parse a configuration data in string \a str.
     *  \returns TRUE if successful, or FALSE if the string could not be
     *  parsed.
     */ 
    //bool parseString(const char *fn,const char *str);
    bool parseString(const char *fn,const char *str,bool upd = FALSE);

    /*! Parse a configuration file with name \a fn.
     *  \returns TRUE if successful, FALSE if the file could not be 
     *  opened or read.
     */ 
    bool parse(const char *fn,bool upd = FALSE);

    /*! Called from the constructor, will add doxygen's default options
     *  to the configuration object 
     */
    void create();

    /*! Append user start comment
     */
    void appendStartComment(const QCString &u)
    {
      m_startComment += u;
    }
    /*! Append user comment
     */
    void appendUserComment(const QCString &u)
    {
      m_userComment += u;
    }
    /*! Take the user start comment and reset it internally
     *  \returns user start comment
     */
    QCString takeStartComment()
    {
      QCString result=m_startComment;
      m_startComment.resize(0);
      return result.replace(QRegExp("\r"),"");
    }
    /*! Take the user comment and reset it internally
     *  \returns user comment
     */
    QCString takeUserComment()
    {
      QCString result=m_userComment;
      m_userComment.resize(0);
      return result.replace(QRegExp("\r"),"");
    }

  protected:

    ConfigImpl()
    {
      m_options  = new QList<ConfigOption>;
      m_obsolete = new QList<ConfigOption>;
      m_disabled = new QList<ConfigOption>;
      m_dict     = new QDict<ConfigOption>(257);
      m_options->setAutoDelete(TRUE);
      m_obsolete->setAutoDelete(TRUE);
      m_disabled->setAutoDelete(TRUE);
      m_initialized = FALSE;
      create();
    }
   ~ConfigImpl()
    {
      delete m_options;
      delete m_obsolete;
      delete m_disabled;
      delete m_dict;
    }

  private:
    QList<ConfigOption> *m_options;
    QList<ConfigOption> *m_obsolete;
    QList<ConfigOption> *m_disabled;
    QDict<ConfigOption> *m_dict;
    static ConfigImpl *m_instance;
    QCString m_startComment;
    QCString m_userComment;
    bool m_initialized;
    QCString m_header;
};

#endif
