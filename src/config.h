#ifndef CONFIG_H
#define CONFIG_H

#include "qtbc.h"
#include <qstrlist.h>
#include <qfile.h>
#include <qdict.h>
#include <qlist.h>
#include <qtextstream.h>


/*! \brief Abstract base class for any configuration option.
 *
 */
class ConfigOption
{
  public:

    enum OptionType { O_Info, O_List, O_Enum, O_String, O_Int, O_Bool };
    enum { MAX_OPTION_LENGTH = 23 };
    ConfigOption(OptionType t) : m_kind(t) 
    {
      m_spaces.fill(' ',40);
    }
    virtual ~ConfigOption()
    {
    }
    void addDependency(const char *dep)
    {
      m_dependency = dep;
    }
    OptionType kind() const { return m_kind; }
    virtual void writeTemplate(QTextStream &t,bool sl) = 0;
    virtual void convertStrToVal() {}
    virtual void substEnvVars() {}
    virtual void init() {}
    QCString name() const { return m_name; }
    QCString docs() const { return m_doc; }
    QCString dependsOn() const { return m_dependency; }

  protected:

    QCString convertToComment(const QCString &s)
    {
      QCString result;
      if (s.isEmpty()) return result;
      else
      {
        result+="# ";
        QCString tmp=s.stripWhiteSpace();
        char *p=tmp.data();
        char c;
        while ((c=*p++))
        {
          if (c=='\n') result+="\n# ";
          else result+=c;
        }
        result+='\n';
      }
      return result;
    }

    void writeBoolValue(QTextStream &t,bool v)
    {
      if (v) t << "YES"; else t << "NO";
    }

    void writeIntValue(QTextStream &t,int i)
    {
      t << i;
    }

    void writeStringValue(QTextStream &t,QCString &s)
    {
      const char *p=s.data();
      char c;
      bool hasBlanks=FALSE;
      if (p)
      {
        while ((c=*p++)!=0 && !hasBlanks) hasBlanks = (c==' ' || c=='\n' || c=='\t');
        if (hasBlanks) 
          t << "\"" << s << "\"";
        else
          t << s;
      }
    }

    void writeStringList(QTextStream &t,QStrList &l)
    {
      const char *p = l.first();
      bool first=TRUE;
      while (p)
      {
        char c;
        const char *s=p;
        bool hasBlanks=FALSE;
        while ((c=*p++)!=0 && !hasBlanks) hasBlanks = (c==' ' || c=='\n' || c=='\t');
        if (!first) t << "                         ";
        first=FALSE;
        if (hasBlanks) t << "\"" << s << "\""; else t << s;
        p = l.next();
        if (p) t << " \\" << endl;
      }
    }

    QCString m_spaces;
    QCString m_name;
    QCString m_doc;
    QCString m_dependency;
    OptionType m_kind;
};

/*! \brief Section marker for grouping the configuration options
 *
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
    void writeTemplate(QTextStream &t, bool sl)
    {
      if (!sl)
      {
        t << "\n";
      }
      t << "#---------------------------------------------------------------------------\n";
      t << "# " << m_doc << endl;
      t << "#---------------------------------------------------------------------------\n";
    }
};

/*! \brief Option of the list type.
 *
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
    void writeTemplate(QTextStream &t,bool sl)
    {
      if (!sl)
      {
        t << endl;
        t << convertToComment(m_doc);
        t << endl;
      }
      t << m_name << m_spaces.left(MAX_OPTION_LENGTH-m_name.length()) << "= ";
      writeStringList(t,m_value);
      t << "\n";
    }
    void substEnvVars();
  private:
    QStrList m_value;
    WidgetType m_widgetType;
};

/*! \brief Option of the enum type.
 *
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
    void writeTemplate(QTextStream &t,bool sl)
    {
      if (!sl)
      {
        t << endl;
        t << convertToComment(m_doc);
        t << endl;
      }
      t << m_name << m_spaces.left(MAX_OPTION_LENGTH-m_name.length()) << "= ";
      writeStringValue(t,m_value);
      t << "\n";
    }
    void init() { m_value = m_defValue.copy(); }

  private:
    QStrList m_valueRange;
    QCString m_value;
    QCString m_defValue;
};

/*! \brief Option of the string type.
 *
 */
class ConfigString : public ConfigOption
{
  public:
    enum WidgetType { String, File, Dir };
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
    void writeTemplate(QTextStream &t,bool sl)
    {
      if (!sl)
      {
        t << endl;
        t << convertToComment(m_doc);
        t << endl;
      }
      t << m_name << m_spaces.left(MAX_OPTION_LENGTH-m_name.length()) << "= ";
      writeStringValue(t,m_value);
      t << "\n";
    }
    void substEnvVars();
    void init() { m_value = m_defValue.copy(); }
  
  private:
    QCString m_value;
    QCString m_defValue;
    WidgetType m_widgetType;
};

/*! \brief Option of the integer type.
 *
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
    void writeTemplate(QTextStream &t,bool sl)
    {
      if (!sl)
      {
        t << endl;
        t << convertToComment(m_doc);
        t << endl;
      }
      t << m_name << m_spaces.left(MAX_OPTION_LENGTH-m_name.length()) << "= ";
      writeIntValue(t,m_value);
      t << "\n";
    }
    void init() { m_value = m_defValue; }
  private:
    int m_value;
    int m_defValue;
    int m_minVal;
    int m_maxVal;
    QCString m_valueString;
};

/*! \brief Option of the boolean type.
 *
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
    bool *valueRef() { return &m_value; }
    void writeTemplate(QTextStream &t,bool sl)
    {
      if (!sl)
      {
        t << endl;
        t << convertToComment(m_doc);
        t << endl;
      }
      t << m_name << m_spaces.left(MAX_OPTION_LENGTH-m_name.length()) << "= ";
      writeBoolValue(t,m_value);
      t << "\n";
    }
    void init() { m_value = m_defValue; }
  private:
    bool m_value;
    bool m_defValue;
};

/*! \brief Singleton for configuration variables.
 *
 *  This object holds the global static variables
 *  read from a user-supplied configuration file.
 *  The static member instance() can be used to get
 *  a pointer to the one and only instance.
 */
struct Config
{
  public:
    static Config *instance()
    {
      if (m_instance==0) m_instance = new Config;
      return m_instance;
    }
    
    void writeTemplate(QFile *f,bool shortIndex);
    void convertStrToVal();
    void substituteEnvironmentVars();
    void check();
    void init();
    void parse(const QCString &config,const char *fn);
    QListIterator<ConfigOption> iterator()
    {
      return QListIterator<ConfigOption>(*m_options);
    }

    ////////////////////////
    // get functions
    ////////////////////////
    QCString &getString(const char *name) const;
    QStrList &getList(const char *name) const;
    QCString &getEnum(const char *name) const;
    int      &getInt(const char *name) const;
    bool     &getBool(const char *name) const;
    ConfigOption *get(const char *name) const
    {
      return m_dict->find(name); 
    }

    ////////////////////////
    // add functions
    ////////////////////////
    ConfigInfo   *addInfo(const char *name,const char *doc)
    {
      ConfigInfo *result = new ConfigInfo(name,doc);
      m_options->append(result);
      return result;
    }
    ConfigString *addString(const char *name,
                            const char *doc)
    {
      ConfigString *result = new ConfigString(name,doc);
      m_options->append(result);
      m_dict->insert(name,result);
      return result;
    }
    ConfigEnum   *addEnum(const char *name,
                          const char *doc,
                          const char *defVal)
    {
      ConfigEnum *result = new ConfigEnum(name,doc,defVal);
      m_options->append(result);
      m_dict->insert(name,result);
      return result;
    }
    ConfigList   *addList(const char *name,
                          const char *doc)
    {
      ConfigList *result = new ConfigList(name,doc);
      m_options->append(result);
      m_dict->insert(name,result);
      return result;
    }
    ConfigInt    *addInt(const char *name,
                         const char *doc,
                         int minVal,int maxVal,int defVal)
    {
      ConfigInt *result = new ConfigInt(name,doc,minVal,maxVal,defVal);
      m_options->append(result);
      m_dict->insert(name,result);
      return result;
    }
    ConfigBool   *addBool(const char *name,
                          const char *doc,
                          bool defVal)
    {
      ConfigBool *result = new ConfigBool(name,doc,defVal);
      m_options->append(result);
      m_dict->insert(name,result);
      return result;
    }
  protected:
    Config()
    { 
      m_options = new QList<ConfigOption>;
      m_dict = new QDict<ConfigOption>(257);
      m_options->setAutoDelete(TRUE);
      m_initialized = FALSE;
      create();
    }
   ~Config()
    {
      delete m_options;
      delete m_dict;
    }
    void create();

  private:
    QList<ConfigOption> *m_options;
    QDict<ConfigOption> *m_dict;
    static Config *m_instance;
    bool m_initialized;
};

#endif
