/******************************************************************************
 *
 * 
 *
 * Copyright (C) 2000 by Joerg Baumann & Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * All output generated with Doxygen is not covered by this license.
 *
 */

// includes

#include "qtbc.h" 
#include <qtextstream.h>
#include <qfile.h>
#include <qlist.h>
#include <qdict.h>
#include <qstrlist.h>
#include <qdatetime.h>

#include <stdio.h>
#include <stdlib.h>

#define MAX_OPTION_LENGTH 23

static QString spaces="                                                ";

void init(); // defined below

// Iterate over all options calling the spezified method
#define FORALL(x) \
{\
  ConfigOption *co=ConfigOption::optionList.first();\
  while (co)\
  {\
    co->x;\
    co=ConfigOption::optionList.next();\
  }\
}

/*! convert a normal string \a s into a string that can be embedded in C code.
 * The \a prefix string will be placed before each string line and the
 * \a postfix string will be placed after each string.
 */
static void stringToC(QTextStream &t,QCString &s,
                      const char *prefix,const char *postfix)
{
  int i=0,l=s.length();
  bool eol=TRUE;
  for (i=0;i<l;i++)
  {
    char c;
    switch ((c=s[i]))
    {
      case '\n':
        t << postfix << endl; 
        eol=TRUE;
        break;
      case '\\':
      case '"':
        if (eol) t << prefix;
        t << "\\" << (char)c;
        eol=FALSE;
        break;
      default:
        if (eol) t << prefix;
        eol=FALSE;
        t << (char)c; 
        break;
    }
  }
  if (!eol) t << postfix << endl;
}

class ConfigBool;

/*! baseclass for options */
class ConfigOption
{
  public:
    static QList<ConfigOption> optionList;
    static QDict<ConfigOption> optionDict;
    QCString getVarName() const { return varName; }

    virtual ~ConfigOption() {}
    
    virtual void printHeader(QTextStream &t)
    {
      t << "  static " << type << " " << varName 
        << "; // " << shortDesc << endl;
    }
    virtual void printStatic(QTextStream &) {}
    virtual void printConfig(QTextStream &t)
    {
      t << type << " Config::" << varName;
      if (!defaultValue.isEmpty()) t << " = \"" << defaultValue << "\"";
      t << ";" << endl;
    }
    virtual void printRules(QTextStream &)
    {
    }
    virtual void printDump(QTextStream &) 
    {
    }
    virtual void printCheck(QTextStream &t)
    {
      // FIXME
      if (!check.isEmpty()) t << check << endl;
    }
    virtual void printInit(QTextStream &t)
    {
      t << "  Config::" << varName << " = " 
        << defaultValue << ";" << endl;
    }
    virtual void printTemplate(QTextStream &t)
    {
      t << "  if (!sl)" << endl;
      t << "  {" << endl;
      t << "    t << \"\\n\";" << endl;
      stringToC(t,longDesc,"    t << \"# ","\\n\";");
      t << "    t << \"\\n\";" << endl;
      t << "  }" << endl;
      t << "  t << \"" << cfgName;
      t << spaces.left(MAX_OPTION_LENGTH-cfgName.length());
      t << "= \";" << endl;
      t << "  " << writeFunc << "(t,Config::" << varName << ");" << endl;
      t << "  t << \"\\n\";" << endl;
    }
    virtual void printSubstEnv(QTextStream &) {}
    virtual void printWidgets(QTextStream &,int) = 0;
    virtual void addDependency(ConfigOption *)
    {
      fprintf(stderr,"Error: dependency add for non boolean option!\n");
    }
    void setDependsOn(ConfigBool *b) { depends=b; }

  protected:
    ConfigOption(const char * var,const char * cfg,const char * value,
                 const char * short_,const char * long_,const char * type_,
                 const char * type2_,const char * writeFunc_
                ):
      varName(var), cfgName(cfg), defaultValue(value), shortDesc(short_),
      longDesc(long_), type(type_), type2(type2_), writeFunc(writeFunc_)
    {
      if (optionDict.find(var)!=0)
      {
        fprintf(stderr,"Error: configuration variable found more than once!\n");
      }
      else
      {
        optionList.append(this);
        optionDict.insert(var,this);
      }
      depends=0;
    }

    void tclQuote(QTextStream &t,const char *s)
    {
      t << "{";
      for (const char *p=s;*p!=0;p++)
      {
        if (strchr("{}",*p)) t << '\\';
        t << (*p);
      }
      t << "} ";
    }

    QCString varName; // name as in Config::name
    QCString cfgName; // name as in cfgfile
    QCString defaultValue; // default value
    QCString shortDesc; // short description
    QCString longDesc;  // long description
    QCString type,type2; //
    QCString writeFunc; //
    QCString check; //
    ConfigBool *depends;
};

QList<ConfigOption> ConfigOption::optionList;
QDict<ConfigOption> ConfigOption::optionDict;

void addDependency(const char *src,const char *dst)
{
  ConfigOption *sopt = ConfigOption::optionDict.find(src);
  ConfigOption *dopt = ConfigOption::optionDict.find(dst);
  if (!sopt)
  {
    fprintf(stderr,"Error: option %s not defined!\n",src); 
  }
  if (!dopt)
  {
    fprintf(stderr,"Error: option %s not defined!\n",dst);
  }
  dopt->addDependency(sopt);
  sopt->setDependsOn((ConfigBool *)dopt);
}


class ConfigInfo : public ConfigOption
{
  public:
    ConfigInfo(const char *var,const char * short_)
      : ConfigOption(var,"","",short_,"","","","") {}
    static void add(const char *var,const char * short_);
    void printHeader(QTextStream &) {}
    void printConfig(QTextStream &) {}
    void printRules(QTextStream &) {}
    virtual void printTemplate(QTextStream &t)
    {
      t << "  if (!sl)" << endl;
      t << "  {" << endl;
      t << "    t << \"\\n\";" << endl;
      t << "  }" << endl;
      t << "  t << \"#---------------------------------------------------------------------------\\n\";" << endl;
      t << "  t << \"# " << shortDesc << "\\n\";" << endl;
      t << "  t << \"#---------------------------------------------------------------------------\\n\";" << endl;
    }
    virtual void printInit(QTextStream &) {}
    virtual void printDump(QTextStream &t) 
    {
      t << "  printf(\"# " << shortDesc << "\\n\");" << endl;
    }
    virtual void printWidgets(QTextStream &t,int pass)
    {
      switch(pass)
      {
        case 0:
          t << "  PageWidget *" << varName << " = new PageWidget( tab, \"" 
            << varName << "\" );" << endl;
          break;
        case 1:
          t << "  " << varName << "->addStretch(1);" << endl;
          break;
        default:
          break;
      }
    }
    QCString getVarName() { return varName; }
};

class ConfigList : public ConfigOption
{
  public:
    enum WidgetType { String, File, Dir, FileAndDir };
    ConfigList(ConfigInfo *parent,const char * var,const char * cfg,const char * value,
               const char * short_,const char * long_, WidgetType w)
      : ConfigOption(var,cfg,value,short_,long_,"QStrList","List","writeStringList"), 
        m_parent(parent),
        m_widgetType(w)
    {
    }
    static void add(const char * var,const char * cfg,const char * value,
                    const char * short_,const char * long_,WidgetType w=String);
    virtual void printRules(QTextStream &t)
    {
      // rule for assignment
      t << "<Start>\"" << cfgName << "\"[ \\t]*\"=\"";
      t << spaces.left(MAX_OPTION_LENGTH-cfgName.length());
      t << "{ BEGIN(GetStrList); l=&Config::" << varName;
      t << "; l->clear(); elemStr=\"\"; }" << endl;

      // rule for appending
      t << "<Start>\"" << cfgName << "\"[ \\t]*\"+=\"";
      t << spaces.left(MAX_OPTION_LENGTH-cfgName.length()-1);
      t << "{ BEGIN(GetStrList); l=&Config::" << varName;
      t << "; elemStr=\"\"; }" << endl;
    }
    virtual void printInit(QTextStream &t)
    {
      t << "  Config::" << varName;
      if (!defaultValue.isEmpty())
      {
        t << " = \"" << defaultValue << "\"";
      }
      else 
      {
        t << ".clear()";
      }
      t << ";" << endl;
    }
    virtual void printDump(QTextStream &t) 
    {
      t << "  {" << endl;
      t << "    char *is=Config::" << varName << ".first();" << endl;
      t << "    while (is)" << endl;
      t << "    {" << endl;
      t << "      printf(\"" << varName << "=`%s'\\n\",is);" << endl;
      t << "      is=Config::" << varName << ".next();" << endl;
      t << "    }" << endl;
      t << "  }" << endl;
    }
    virtual void printSubstEnv(QTextStream &t)
    {
      t << "  substEnvVarsInStrList( Config::" << varName << " );" << endl;
    }
    virtual void printWidgets(QTextStream &t,int pass)
    {
      switch (pass)
      {
        case 0:
          {
            t << "  " << varName << " = new InputStrList( \"" << cfgName 
              << "\", " << m_parent->getVarName() << ", Config::" << varName;
            if (m_widgetType!=String)
            {
              t << ",";
              switch(m_widgetType)
              {
                case File:       t << "InputStrList::ListFile"; break;
                case Dir:        t << "InputStrList::ListDir"; break;
                case FileAndDir: t << "InputStrList::ListFileDir"; break;
                default: break;
              }
            }
            t << " );" << endl;
            t << "  QWhatsThis::add(" << varName << ", " << endl;
            stringToC(t,longDesc,"    \"","\"");
            t << "  );" << endl;
            t << "  connect(" << varName 
              << ",SIGNAL(changed()),this,SIGNAL(changed()));" << endl;
          }
          break;
        case 2:
          {
            t << "    InputStrList *" << varName << ";" << endl; 
          }
          break;
        case 5:
          {
            t << "  " << varName << "->init();" << endl;
          }
          break;
        default:
          break;
      }
    }
  private:
    ConfigInfo *m_parent;
    WidgetType m_widgetType;
};

class ConfigString : public ConfigOption
{
  public:
    enum WidgetType { String, File, Dir };
    ConfigString(ConfigInfo *parent, const char * var,const char * cfg,const char * value,
                 const char * short_,const char * long_, WidgetType w
                )
      : ConfigOption(var,cfg,value,short_,long_,
                     "QCString","String","writeStringValue"
                    ), 
        m_parent(parent), 
        m_widgetType(w)
    {
    }
    
    static void add(const char * var,const char * cfg,const char * value,
                    const char * short_,const char * long_,WidgetType w=String);
    static void addFixedValue(const char *var, const char *value);
    
    virtual void printRules(QTextStream &t)
    {
      t << "<Start>\"" << cfgName << "\"[ \\t]*\"=\"";
      t << spaces.left(MAX_OPTION_LENGTH-cfgName.length());
      t << "{ BEGIN(GetString); s=&Config::" << varName;
      t << "; s->resize(0); }" << endl;
    }
    virtual void printInit(QTextStream &t)
    {
      t << "  Config::" << varName;
      if (!defaultValue.isEmpty()) t << " = \"" << defaultValue << "\"";
      else t << ".resize(0)";
      t << ";" << endl;
    }
    virtual void printDump(QTextStream &t) 
    {
      t << "  printf(\"" << varName << "=`%s'\\n\",Config::" << varName << ".data());" << endl;
    }
    virtual void printSubstEnv(QTextStream &t)
    {
      t << "  substEnvVarsInString( Config::" << varName << " );" << endl;
    }
    virtual void printWidgets(QTextStream &t,int pass)
    {
      switch (pass)
      {
        case 0:
          {
            t << "  " << varName << " = new InputString( \"" << cfgName 
              << "\", " << m_parent->getVarName() << ", Config::" << varName;
            if (m_widgetType!=String || m_values.count()>0)
            {
              t << ",";
              switch (m_widgetType)
              {
                case File:
                  t << "InputString::StringFile"; 
                  break;
                case Dir:
                  t << "InputString::StringDir"; 
                  break;
                default:
                  t << "InputString::StringFixed";
                  break;
              }
            }

            t << " );" << endl;
            t << "  QWhatsThis::add(" << varName << ", " << endl;
            stringToC(t,longDesc,"    \"","\"");
            t << "  );" << endl;
            t << "  connect(" << varName 
              << ",SIGNAL(changed()),this,SIGNAL(changed()));" << endl;
            if (m_values.count()>0)
            {
              char *s = m_values.first();
              while (s)
              {
                t << "  " << varName << "->addValue(\"" << s << "\");" << endl;
                s = m_values.next();
              }
            }
          }  
          break;
        case 2:
          {
            t << "    InputString *" << varName << ";" << endl;
          }
          break;
        case 5:
          {
            t << "  " << varName << "->init();" << endl;
          }
          break;
        default:
          break;
      }
    }
    void addValue(const char *val)
    {
      m_values.append(val);
    }
  private:
    ConfigInfo *m_parent;
    WidgetType m_widgetType;
    QStrList m_values;
};

class ConfigInt : public ConfigOption
{
  public:
    ConfigInt(ConfigInfo *parent,const char * var,const char * cfg,const char * value,
            const char * short_,const char * long_,int minVal,int maxVal)
      : ConfigOption(var,cfg,value,short_,long_,"int     ","Int","writeIntValue"), 
        m_parent(parent), m_minVal(minVal), m_maxVal(maxVal) 
    {
    }
    static void add(const char * var,const char * cfg,const char * value,
                    const char * short_,const char * long_,int minVal,int maxVal);
    virtual void printConfig(QTextStream &t)
    {
      t << type << " Config::" << varName;
      if (!defaultValue.isEmpty()) t << " = " << defaultValue;
      t << ";" << endl;
    }
    virtual void printRules(QTextStream &t)
    {
      t << "<Start>\"" << cfgName << "\"[ \\t]*\"=\"";
      t << spaces.left(MAX_OPTION_LENGTH-cfgName.length());
      t << "{ BEGIN(GetString); s=&" << varName;
      t << "String; s->resize(0); }" << endl;
    }
    virtual void printStatic(QTextStream &t)
    {
      t << "static QCString     " << varName << "String;" << endl;
    }
    virtual void printDump(QTextStream &t) 
    {
      t << "  printf(\"" << varName << "=`%d'\\n\",Config::" << varName << ");" << endl;
    }
    virtual void printWidgets(QTextStream &t,int pass)
    {
      switch (pass)
      {
        case 0:
          {
            t << "  " << varName << " = new InputInt( \"" << cfgName 
              << "\", " << m_parent->getVarName() << ", Config::" 
              << varName << "," << m_minVal << "," << m_maxVal << " );" << endl;
            t << "  QWhatsThis::add(" << varName << ", " << endl;
            stringToC(t,longDesc,"    \"","\"");
            t << "  );" << endl;
            t << "  connect(" << varName 
              << ",SIGNAL(changed()),this,SIGNAL(changed()));" << endl;
          }  
          break;
        case 2:
          {
            t << "    InputInt *" << varName << ";" << endl;
          }
          break;
        case 5:
          {
            t << "  " << varName << "->init();" << endl;
          }
          break;
        default:
          break;
      }
    }
  private:
    ConfigInfo *m_parent;
    int m_minVal;
    int m_maxVal;
}; 

class ConfigBool : public ConfigOption
{
  public:
    ConfigBool(ConfigInfo *parent,const char * var,const char * cfg,const char * value,
               const char * short_,const char * long_)
     : ConfigOption(var,cfg,value,short_,long_,"bool    ","Bool","writeBoolValue"),
       m_parent(parent) {}
    static void add(const char * var,const char * cfg,const char * value,
                  const char * short_,const char * long_);
    virtual void printConfig(QTextStream &t)
    {
      t << type << " Config::" << varName;
      if (!defaultValue.isEmpty()) t << " = " << defaultValue;
      t << ";" << endl;
    }
    virtual void printRules(QTextStream &t)
    {
      t << "<Start>\"" << cfgName << "\"[ \\t]*\"=\"";
      t << spaces.left(MAX_OPTION_LENGTH-cfgName.length());
      t << "{ BEGIN(GetBool); b=&Config::" << varName;
      t << "; }" << endl;
    }
    virtual void printDump(QTextStream &t) 
    {
      t << "  printf(\"" << varName << "=`%d'\\n\",Config::" << varName << ");" << endl;
    }
    virtual void printWidgets(QTextStream &t,int pass)
    {
      switch (pass)
      {
        case 0:
          {
            t << "  " << varName << " = new InputBool( \"" << cfgName 
              << "\", " << m_parent->getVarName() << ", Config::" << varName << " );" << endl;
            t << "  QWhatsThis::add(" << varName << ", " << endl;
            stringToC(t,longDesc,"    \"","\"");
            t << "  );" << endl;
            t << "  connect(" << varName 
              << ",SIGNAL(changed()),this,SIGNAL(changed()));" << endl;
            if (dependencies.count()>0)
            {
              t << "  connect(" << varName << ",SIGNAL(toggled(bool)),SLOT(" 
                << varName << "Toggle(bool)));" << endl;
            }
          }
          break;
        case 1:
          {
            if (dependencies.count()>0)
            {
              t << "  " << varName << "Toggle(Config::" << varName << ");" << endl;
            }
          }
          break;
        case 2:
          {
            t << "    InputBool *" << varName << ";" << endl; 
          }
          break;
        case 3:
          {
            if (dependencies.count()>0)
            {
              t << "    void " << varName << "Toggle(bool);" << endl;
            }
          }
          break;
        case 4:
          {
            if (dependencies.count()>0)
            {
              t << "void ConfigFile::" << varName << "Toggle(bool state)" << endl;
              t << "{" << endl;
              ConfigOption *o=dependencies.first();
              while (o)
              {
                t << "  " << o->getVarName() << "->setEnabled(state);" << endl;
                o=dependencies.next();
              }
              t << "}" << endl << endl;
            }
          }
          break;
        case 5:
          {
            t << "  " << varName << "->init();" << endl;
          }
        default:
          break;
      }
    }
    virtual void addDependency(ConfigOption *o)
    {
      dependencies.append(o);
    }
  private:
    ConfigInfo *m_parent;
    QList<ConfigOption> dependencies;
};

static ConfigInfo *g_lastConfigInfo;

void ConfigList::add(const char * var,const char * cfg,const char * value,
                  const char * short_,const char * long_,WidgetType w)
{
  ConfigOption* o=new ConfigList(g_lastConfigInfo,var,cfg,value,short_,long_,w);
  ASSERT(o!=0);
  ASSERT(g_lastConfigInfo!=0);
  //optionList.append(o);
}

void ConfigString::add(const char * var,const char * cfg,const char * value,
                  const char * short_,const char * long_,WidgetType w)
{
  ConfigOption* o=new ConfigString(g_lastConfigInfo,var,cfg,value,short_,long_,w);
  ASSERT(o!=0);
  ASSERT(g_lastConfigInfo!=0);
  //optionList.append(o);
}

void ConfigInt::add(const char * var,const char * cfg,const char * value,
                  const char * short_,const char * long_,int minVal, int maxVal)
{
  ConfigOption* o=new ConfigInt(g_lastConfigInfo,var,cfg,value,
                                short_,long_,minVal,maxVal);
  ASSERT(o!=0);
  ASSERT(g_lastConfigInfo!=0);
  //optionList.append(o);
}

void ConfigBool::add(const char * var,const char * cfg,const char * value,
                  const char * short_,const char * long_)
{
  ConfigOption* o=new ConfigBool(g_lastConfigInfo,var,cfg,value,short_,long_);
  ASSERT(o!=0);
  ASSERT(g_lastConfigInfo!=0);
  //optionList.append(o);
}

void ConfigInfo::add(const char * var,const char * short_)
{
  g_lastConfigInfo =new ConfigInfo(var,short_);
  ASSERT(g_lastConfigInfo!=0);
  //optionList.append(g_lastConfigInfo);
}

void ConfigString::addFixedValue(const char *var, const char *value)
{
  ConfigString *cs = (ConfigString *)optionDict[var];
  if (cs==0)
  {
    fprintf(stderr,"ConfigString::addFixedValue: unknown variable %s\n",var);
  }
  else
  {
    cs->addValue(value);
  }
}


int main(int argc,char * argv[])
{
  // check arguments
  if (argc!=2) 
  {
    fprintf(stderr,"usage: %s config-template\n",argv[0]);
    exit(1);
  }

  init(); // create options
  QFile ctfile(argv[1]);
  
  // open template file
  if (!ctfile.open(IO_ReadOnly)) 
  {
    fprintf(stderr,"Error: couldn´t open file %s\n",argv[1]);
    exit(1);
  }
  const int maxLineLen=10000;
  char buf[maxLineLen];

  QFile f;
  f.open(IO_WriteOnly,stdout);
  QTextStream t(&f);

  t << "/* This file was generated by configgen on " 
    << QDateTime::currentDateTime().toString()
    << "\n * from " << argv[1] << "\n *\n * DO NOT EDIT!\n */\n\n";
  
  // process template file
  while (!ctfile.atEnd()) 
  {
    ctfile.readLine(buf,maxLineLen-1);
    if      (QCString("#CONFIG Config\n"  )==buf) FORALL(printConfig(t))
    else if (QCString("#CONFIG Static\n"  )==buf) FORALL(printStatic(t))
    else if (QCString("#CONFIG Rules\n"   )==buf) FORALL(printRules(t))
    else if (QCString("#CONFIG Dump\n"    )==buf) FORALL(printDump(t))
    else if (QCString("#CONFIG Init\n"    )==buf) FORALL(printInit(t))
    else if (QCString("#CONFIG Template\n")==buf) FORALL(printTemplate(t))
    else if (QCString("#CONFIG Check\n"   )==buf) FORALL(printCheck(t))
    else if (QCString("#CONFIG Header\n"  )==buf) FORALL(printHeader(t))
    else if (QCString("#CONFIG Substenv\n")==buf) FORALL(printSubstEnv(t))
    else if (QCString("#CONFIG Widgets0\n"       )==buf) FORALL(printWidgets(t,0))
    else if (QCString("#CONFIG Widgets1\n"       )==buf) FORALL(printWidgets(t,1))
    else if (QCString("#CONFIG WidgetVars\n"     )==buf) FORALL(printWidgets(t,2))
    else if (QCString("#CONFIG WidgetSlots\n"    )==buf) FORALL(printWidgets(t,3))
    else if (QCString("#CONFIG WidgetSlotImpl\n" )==buf) FORALL(printWidgets(t,4))
    else if (QCString("#CONFIG WidgetInit\n"     )==buf) FORALL(printWidgets(t,5))
    else t << buf;
  }
  ctfile.close();
  return 0;
}

void init()
{
  // option definitions
  //-----------------------------------------------------------------------------------------------
  ConfigInfo::add("General","General configuration options");
  //-----------------------------------------------------------------------------------------------
  
  ConfigString::add("projectName",
                    "PROJECT_NAME",
                    "",
                    "the name of the project",
                    "The PROJECT_NAME tag is a single word (or a sequence of words surrounded \n"
                    "by quotes) that should identify the project. "
                   );
  ConfigString::add("projectNumber",
                    "PROJECT_NUMBER",
                    "",
                    "the number of the project",
                    "The PROJECT_NUMBER tag can be used to enter a project or revision number. \n"
                    "This could be handy for archiving the generated documentation or \n"
                    "if some version control system is used.\n"
                   );
  ConfigString::add("outputDir",
                    "OUTPUT_DIRECTORY",
                    "",
                    "the global output directory",
                    "The OUTPUT_DIRECTORY tag is used to specify the (relative or absolute) \n"
                    "base path where the generated documentation will be put. \n"
                    "If a relative path is entered, it will be relative to the location \n"
                    "where doxygen was started. If left blank the current directory will be used.\n",
                    ConfigString::Dir
                   );
  ConfigString::add("outputLanguage",
                    "OUTPUT_LANGUAGE",
                    "English",
                    "the output language",
                    "The OUTPUT_LANGUAGE tag is used to specify the language in which all \n"
                    "documentation generated by doxygen is written. Doxygen will use this \n"
                    "information to generate all constant output in the proper language. \n"
                    "The default language is English, other supported languages are: \n"
                    "Dutch, French, Italian, Czech, Swedish, German, Finnish, Japanese, \n"
                    "Spanish, Russian, Croatian, Polish, and Portuguese.\n"
                   );
  ConfigString::addFixedValue("outputLanguage","English");
  ConfigString::addFixedValue("outputLanguage","Dutch");
  ConfigString::addFixedValue("outputLanguage","French");
  ConfigString::addFixedValue("outputLanguage","Italian");
  ConfigString::addFixedValue("outputLanguage","Czech");
  ConfigString::addFixedValue("outputLanguage","Swedish");
  ConfigString::addFixedValue("outputLanguage","German");
  ConfigString::addFixedValue("outputLanguage","Finnish");
  ConfigString::addFixedValue("outputLanguage","Japanese");
  ConfigString::addFixedValue("outputLanguage","Spanish");
  ConfigString::addFixedValue("outputLanguage","Russian");
  ConfigString::addFixedValue("outputLanguage","Croatian");
  ConfigString::addFixedValue("outputLanguage","Polish");
  ConfigString::addFixedValue("outputLanguage","Portuguese");
  ConfigBool::add(  "extractAllFlag",
                    "EXTRACT_ALL",
                    "FALSE",
                    "generate docs for all classes flag",
                    "If the EXTRACT_ALL tag is set to YES doxygen will assume all entities in \n"
                    "documentation are documented, even if no documentation was available. \n"
                    "Private class members and static file members will be hidden unless \n"
                    "the EXTRACT_PRIVATE and EXTRACT_STATIC tags are set to YES \n"
                 );
  ConfigBool::add(  "extractPrivateFlag",
                    "EXTRACT_PRIVATE",
                    "FALSE",
                    "generate docs for private members flag",
                    "If the EXTRACT_PRIVATE tag is set to YES all private members of a class \n"
                    "will be included in the documentation. \n"
                 );
  ConfigBool::add(  "extractStaticFlag",
                    "EXTRACT_STATIC",
                    "FALSE",
                    "generate docs for static members flag",
                    "If the EXTRACT_STATIC tag is set to YES all static members of a file \n"
                    "will be included in the documentation. \n"
                 );
  ConfigBool::add(  "hideMemberFlag",
                    "HIDE_UNDOC_MEMBERS",
                    "FALSE",
                    "hide undocumented members.",
                    "If the HIDE_UNDOC_MEMBERS tag is set to YES, Doxygen will hide all \n"
                    "undocumented members of documented classes, files or namespaces. \n"
                    "If set to NO (the default) these members will be included in the \n"
                    "various overviews, but no documentation section is generated. \n"
                    "This option has no effect if EXTRACT_ALL is enabled. \n"
                 );
  ConfigBool::add(  "hideClassFlag",
                    "HIDE_UNDOC_CLASSES",
                    "FALSE",
                    "hide undocumented members.",
                    "If the HIDE_UNDOC_CLASSES tag is set to YES, Doxygen will hide all \n"
                    "undocumented classes that are normally visible in the class hierarchy. \n"
                    "If set to NO (the default) these class will be included in the various \n"
                    "overviews. This option has no effect if EXTRACT_ALL is enabled. \n"
                 );
  ConfigBool::add(  "briefMemDescFlag",
                    "BRIEF_MEMBER_DESC",
                    "TRUE",
                    "enable `inline' brief member descr.",
                    "If the BRIEF_MEMBER_DESC tag is set to YES (the default) Doxygen will \n"
                    "include brief member descriptions after the members that are listed in \n"
                    "the file and class documentation (similar to JavaDoc). \n"
                    "Set to NO to disable this. \n"
                 );
  ConfigBool::add(  "repeatBriefFlag",
                    "REPEAT_BRIEF",
                    "TRUE",
                    "repeat brief descriptions.",
                    "If the REPEAT_BRIEF tag is set to YES (the default) Doxygen will prepend \n"
                    "the brief description of a member or function before the detailed description. \n"
                    "Note: if both HIDE_UNDOC_MEMBERS and BRIEF_MEMBER_DESC are set to NO, the \n"
                    "brief descriptions will be completely suppressed. \n"
                 );
  ConfigBool::add(  "alwaysDetailsFlag",
                    "ALWAYS_DETAILED_SEC",
                    "FALSE",
                    "show details description even if there is only a brief description?",
                    "If the ALWAYS_DETAILED_SEC and REPEAT_BRIEF tags are both set to YES then \n"
                    "Doxygen will generate a detailed section even if there is only a brief \n"
                    "description. \n"
                 );
  ConfigBool::add(  "fullPathNameFlag",
                    "FULL_PATH_NAMES",
                    "FALSE",
                    "using full path name in output",
                    "If the FULL_PATH_NAMES tag is set to YES then Doxygen will prepend the full \n"
                    "path before files name in the file list and in the header files. If set \n"
                    "to NO the shortest path that makes the file name unique will be used. \n"
                 );
  ConfigList::add(  "stripFromPath",
                    "STRIP_FROM_PATH",
                    "",
                    "list of candidates to strip from the file path",
                    "If the FULL_PATH_NAMES tag is set to YES then the STRIP_FROM_PATH tag \n"
                    "can be used to strip a user defined part of the path. Stripping is \n"
                    "only done if one of the specified strings matches the left-hand part of \n"
                    "the path. It is allowed to use relative paths in the argument list.\n"
                 );
  addDependency("stripFromPath","fullPathNameFlag");
  ConfigBool::add(  "internalDocsFlag",
                    "INTERNAL_DOCS",
                    "FALSE",
                    "determines what happens to internal docs.",
                    "The INTERNAL_DOCS tag determines if documentation \n"
                    "that is typed after a \\internal command is included. If the tag is set \n"
                    "to NO (the default) then the documentation will be excluded. \n"
                    "Set it to YES to include the internal documentation. \n"
                 );
  ConfigBool::add(  "classDiagramFlag",
                    "CLASS_DIAGRAMS",
                    "TRUE",
                    "enable the generation of class diagrams.",
                    "If the CLASS_DIAGRAMS tag is set to YES (the default) Doxygen will \n"
                    "generate a class diagram (in Html and LaTeX) for classes with base or \n"
                    "super classes. Setting the tag to NO turns the diagrams off. \n"
                 );
  ConfigBool::add(  "sourceBrowseFlag",
                    "SOURCE_BROWSER",
                    "FALSE",
                    "include source code in documentation.",
                    "If the SOURCE_BROWSER tag is set to YES then a list of source files will \n"
                    "be generated. Documented entities will be cross-referenced with these sources. \n"
                 );
  ConfigBool::add(  "inlineSourceFlag",
                    "INLINE_SOURCES",
                    "FALSE",
                    "inline the definition bodies in the docs?",
                    "Setting the INLINE_SOURCES tag to YES will include the body \n"
                    "of functions and classes directly in the documentation. \n"
                 );
  ConfigBool::add(  "stripCommentsFlag",
                    "STRIP_CODE_COMMENTS",
                    "TRUE",
                    "strip special comments from code fragments?",
                    "Setting the STRIP_CODE_COMMENTS tag to YES (the default) will instruct \n"
                    "doxygen to hide any special comment blocks from generated source code \n"
                    "fragments. Normal C and C++ comments will always remain visible. \n"
                 );
  ConfigBool::add(  "caseSensitiveNames",
                    "CASE_SENSE_NAMES",
                    "TRUE",
                    "determines if output can be mixed case.",
                    "If the CASE_SENSE_NAMES tag is set to NO (the default) then Doxygen \n"
                    "will only generate file names in lower case letters. If set to \n"
                    "YES upper case letters are also allowed. This is useful if you have \n"
                    "classes or files whose names only differ in case and if your file system \n"
                    "supports case sensitive file names. \n"
                 );
  ConfigBool::add(  "hideScopeNames",
                    "HIDE_SCOPE_NAMES",
                    "FALSE",
                    "hide the name of the scope.",
                    "If the HIDE_SCOPE_NAMES tag is set to NO (the default) then Doxygen \n"
                    "will show members with their full class and namespace scopes in the \n"
                    "documentation. If set to YES the scope will be hidden. \n"
                 );
  ConfigBool::add(  "verbatimHeaderFlag",
                    "VERBATIM_HEADERS",
                    "TRUE",
                    "enable/disable generation of verb headers.",
                    "If the VERBATIM_HEADERS tag is set to YES (the default) then Doxygen \n"
                    "will generate a verbatim copy of the header file for each class for \n"
                    "which an include is specified. Set to NO to disable this. \n"
                 );
  ConfigBool::add(  "showIncFileFlag",
                    "SHOW_INCLUDE_FILES",
                    "TRUE",
                    "show include file in file documentation?",
                    "If the SHOW_INCLUDE_FILES tag is set to YES (the default) then Doxygen \n"
                    "will put list of the files that are included by a file in the documentation \n"
                    "of that file. \n"
                 );
  ConfigBool::add(  "autoBriefFlag",
                    "JAVADOC_AUTOBRIEF",
                    "TRUE",
                    "javadoc comments behaves as Qt comments.",
                    "If the JAVADOC_AUTOBRIEF tag is set to YES (the default) then Doxygen \n"
                    "will interpret the first line (until the first dot) of a JavaDoc-style \n"
                    "comment as the brief description. If set to NO, the Javadoc-style will \n"
                    "behave just like the Qt-style comments. \n"
                 );
  ConfigBool::add(  "inheritDocsFlag",
                    "INHERIT_DOCS",
                    "TRUE",
                    "inheritance of documentation enabled?",
                    "If the INHERIT_DOCS tag is set to YES (the default) then an undocumented \n"
                    "member inherits the documentation from any documented member that it \n"
                    "reimplements. \n"
                 );
  ConfigBool::add(  "inlineInfoFlag",
                    "INLINE_INFO",
                    "TRUE",
                    "show info about inline members?",
                    "If the INLINE_INFO tag is set to YES (the default) then a tag [inline] \n"
                    "is inserted in the documentation for inline members. \n"
                 );
  ConfigBool::add(  "sortMembersFlag",
                    "SORT_MEMBER_DOCS",
                    "TRUE",
                    "sort members alphabetically?",
                    "If the SORT_MEMBER_DOCS tag is set to YES (the default) then doxygen \n"
                    "will sort the (detailed) documentation of file and class members \n"
                    "alphabetically by member name. If set to NO the members will appear in \n"
                    "declaration order. \n"
                 );
  ConfigInt::add(   "tabSize",
                    "TAB_SIZE",
                    "8",
                    "number of spaces in a tab",
                    "The TAB_SIZE tag can be used to set the number of spaces in a tab. \n"
                    "Doxygen uses this value to replace tabs by spaces in code fragments. \n",
                    1,16
                );
  ConfigList::add(  "sectionFilterList",
                    "ENABLED_SECTIONS",
                    "",
                    "list of section filters that are enabled",
                    "The ENABLE_SECTIONS tag can be used to enable conditional \n"
                    "documentation sections, marked by \\if sectionname ... \\endif. \n"
                 );
  ConfigBool::add(  "generateTodoList",
                    "GENERATE_TODOLIST",
                    "TRUE",
                    "do we want a todo list?",
                    "The GENERATE_TODOLIST tag can be used to enable (YES) or \n"
                    "disable (NO) the todo list. This list is created by putting \\todo \n"
                    "commands in the documentation.\n"
                 );
  ConfigBool::add(  "generateTestList",
                    "GENERATE_TESTLIST",
                    "TRUE",
                    "do we want a test list?",
                    "The GENERATE_TESTLIST tag can be used to enable (YES) or \n"
                    "disable (NO) the test list. This list is created by putting \\test \n"
                    "commands in the documentation.\n"
                 );
  //-----------------------------------------------------------------------------------------------
  ConfigInfo::add(  "Messages","configuration options related to warning and progress messages");
  //-----------------------------------------------------------------------------------------------
  ConfigBool::add(  "quietFlag",
                    "QUIET",
                    "FALSE",
                    "generate progress messages flag",
                    "The QUIET tag can be used to turn on/off the messages that are generated \n"
                    "by doxygen. Possible values are YES and NO. If left blank NO is used. \n"
                 );
  ConfigBool::add(  "warningFlag",
                    "WARNINGS",
                    "TRUE",
                    "generate warnings flag",
                    "The WARNINGS tag can be used to turn on/off the warning messages that are \n"
                    "generated by doxygen. Possible values are YES and NO. If left blank \n"
                    "NO is used. \n"
                 );
  ConfigBool::add(  "warningUndocFlag",
                    "WARN_IF_UNDOCUMENTED",
                    "TRUE",
                    "generate undocumented warnings",
                    "If WARN_IF_UNDOCUMENTED is set to YES, then doxygen will generate warnings \n"
                    "for undocumented members. If EXTRACT_ALL is set to YES then this flag will \n"
                    "automatically be disabled. \n"
                 );
  ConfigString::add("warnFormat",
                    "WARN_FORMAT",
                    "$file:$line: $text",
                    "format of the warning messages",
                    "The WARN_FORMAT tag determines the format of the warning messages that \n"
                    "doxygen can produce. The string should contain the $file, $line, and $text \n"
                    "tags, which will be replaced by the file and line number from which the \n"
                    "warning originated and the warning text. \n"
                   ); 
  //-----------------------------------------------------------------------------------------------
  ConfigInfo::add(  "Input","configuration options related to the input files");
  //-----------------------------------------------------------------------------------------------
  ConfigList::add(  "inputSources",
                    "INPUT",
                    "",
                    "list of input files",
                    "The INPUT tag can be used to specify the files and/or directories that contain \n"
                    "documented source files. You may enter file names like \"myfile.cpp\" or \n"
                    "directories like \"/usr/src/myproject\". Separate the files or directories \n"
                    "with spaces. \n",
                    ConfigList::FileAndDir
                 );
  ConfigList::add(  "filePatternList",
                    "FILE_PATTERNS",
                    "",
                    "list of file patterns",
                    "If the value of the INPUT tag contains directories, you can use the \n"
                    "FILE_PATTERNS tag to specify one or more wildcard pattern (like *.cpp \n"
                    "and *.h) to filter out the source-files in the directories. If left \n"
                    "blank all files are included. \n"
                 );
  ConfigBool::add(  "recursiveFlag",
                    "RECURSIVE",
                    "FALSE",
                    "scan directories recursively",
                    "The RECURSIVE tag can be used to turn specify whether or not subdirectories \n"
                    "should be searched for input files as well. Possible values are YES and NO. \n"
                    "If left blank NO is used. \n"
                 );
  ConfigList::add(  "excludeSources",
                    "EXCLUDE",
                    "",
                    "list of files to exclude from the input",
                    "The EXCLUDE tag can be used to specify files and/or directories that should \n"
                    "excluded from the INPUT source files. This way you can easily exclude a \n"
                    "subdirectory from a directory tree whose root is specified with the INPUT tag. \n",
                    ConfigList::FileAndDir
                 );
  ConfigList::add(  "excludePatternList",
                    "EXCLUDE_PATTERNS",
                    "",
                    "list of patterns to exclude from input",
                    "If the value of the INPUT tag contains directories, you can use the \n"
                    "EXCLUDE_PATTERNS tag to specify one or more wildcard patterns to exclude \n"
                    "certain files from those directories. \n"
                 );
  ConfigList::add(  "examplePath",
                    "EXAMPLE_PATH",
                    "",
                    "list of example paths",
                    "The EXAMPLE_PATH tag can be used to specify one or more files or \n"
                    "directories that contain example code fragments that are included (see \n"
                    "the \\include command). \n",
                    ConfigList::Dir
                 );
  ConfigList::add(  "examplePatternList",
                    "EXAMPLE_PATTERNS",
                    "",
                    "list of example patterns",
                    "If the value of the EXAMPLE_PATH tag contains directories, you can use the \n"
                    "EXAMPLE_PATTERNS tag to specify one or more wildcard pattern (like *.cpp \n"
                    "and *.h) to filter out the source-files in the directories. If left \n"
                    "blank all files are included. \n"
                 );
  ConfigList::add(  "imagePath",
                    "IMAGE_PATH",
                    "",
                    "list of image paths",
                    "The IMAGE_PATH tag can be used to specify one or more files or \n"
                    "directories that contain image that are included in the documentation (see \n"
                    "the \\image command). \n",
                    ConfigList::Dir
                 );
  ConfigString::add("inputFilter",
                    "INPUT_FILTER",
                    "",
                    "a filter command that is applied to input files",
                    "The INPUT_FILTER tag can be used to specify a program that doxygen should \n"
                    "invoke to filter for each input file. Doxygen will invoke the filter program \n"
                    "by executing (via popen()) the command <filter> <input-file>, where <filter> \n"
                    "is the value of the INPUT_FILTER tag, and <input-file> is the name of an \n"
                    "input file. Doxygen will then use the output that the filter program writes \n"
                    "to standard output. \n",
                    ConfigString::File
                   );
  
  //-----------------------------------------------------------------------------------------------
  ConfigInfo::add(  "Index","configuration options related to the alphabetical class index");
  //-----------------------------------------------------------------------------------------------
  
  ConfigBool::add(  "alphaIndexFlag",
                    "ALPHABETICAL_INDEX",
                    "FALSE",
                    "should an alphabetical index be generated?",
                    "If the ALPHABETICAL_INDEX tag is set to YES, an alphabetical index \n"
                    "of all compounds will be generated. Enable this if the project \n"
                    "contains a lot of classes, structs, unions or interfaces. \n"
                 );
  ConfigInt::add(   "colsInAlphaIndex",
                    "COLS_IN_ALPHA_INDEX",
                    "5",
                    "number of columns in the alphabetical index",
                    "If the alphabetical index is enabled (see ALPHABETICAL_INDEX) then \n"
                    "the COLS_IN_ALPHA_INDEX tag can be used to specify the number of columns \n"
                    "in which this list will be split (can be a number in the range [1..20]) \n",
                    1,20
                );
  ConfigList::add(  "ignorePrefixList",
                    "IGNORE_PREFIX",
                    "",
                    "list of prefixes to ignore for the alphabetical index",
                    "In case all classes in a project start with a common prefix, all \n"
                    "classes will be put under the same header in the alphabetical index. \n"
                    "The IGNORE_PREFIX tag can be used to specify one or more prefixes that \n"
                    "should be ignored while generating the index headers. \n"
                 );
  //-----------------------------------------------------------------------------------------------
  ConfigInfo::add(  "HTML","configuration options related to the HTML output");
  //-----------------------------------------------------------------------------------------------
  ConfigBool::add(  "generateHtml",
                    "GENERATE_HTML",
                    "TRUE",
                    "generate HTML output",
                    "If the GENERATE_HTML tag is set to YES (the default) Doxygen will \n"
                    "generate HTML output. \n"
                 );
  ConfigString::add("htmlOutputDir",
                    "HTML_OUTPUT",
                    "html",
                    "the directory to put the HTML files",
                    "The HTML_OUTPUT tag is used to specify where the HTML docs will be put. \n"
                    "If a relative path is entered the value of OUTPUT_DIRECTORY will be \n"
                    "put in front of it. If left blank `html' will be used as the default path. \n",
                    ConfigString::Dir
                   );
  addDependency("htmlOutputDir","generateHtml");
  ConfigString::add("headerFile",
                    "HTML_HEADER",
                    "",
                    "the name of the personal HTML header",
                    "The HTML_HEADER tag can be used to specify a personal HTML header for \n"
                    "each generated HTML page. If it is left blank doxygen will generate a \n"
                    "standard header.\n",
                    ConfigString::File
                   );
  addDependency("headerFile","generateHtml");
  ConfigString::add("footerFile",
                    "HTML_FOOTER",
                    "",
                    "the name of the personal HTML footer",
                    "The HTML_FOOTER tag can be used to specify a personal HTML footer for \n"
                    "each generated HTML page. If it is left blank doxygen will generate a \n"
                    "standard footer.\n",
                    ConfigString::File
                   );
  addDependency("footerFile","generateHtml");
  ConfigString::add("htmlStyleSheet",
                    "HTML_STYLESHEET",
                    "",
                    "user defined cascading style sheet",
                    "The HTML_STYLESHEET tag can be used to specify a user defined cascading \n"
                    "style sheet that is used by each HTML page. It can be used to \n"
                    "fine-tune the look of the HTML output. If the tag is left blank doxygen \n"
                    "will generate a default style sheet \n",
                    ConfigString::File
                   );
  addDependency("htmlStyleSheet","generateHtml");
  ConfigBool::add(  "htmlAlignMemberFlag",
                    "HTML_ALIGN_MEMBERS",
                    "TRUE",
                    "align members in HTML using tables.",
                    "If the HTML_ALIGN_MEMBERS tag is set to YES, the members of classes, \n"
                    "files or namespaces will be aligned in HTML using tables. If set to \n"
                    "NO a bullet list will be used. \n"
                 );
  addDependency("htmlAlignMemberFlag","generateHtml");
  ConfigBool::add(  "htmlHelpFlag",
                    "GENERATE_HTMLHELP",
                    "FALSE",
                    "should html help files be generated?",
                    "If the GENERATE_HTMLHELP tag is set to YES, additional index files \n"
                    "will be generated that can be used as input for tools like the \n"
                    "Microsoft HTML help workshop to generate a compressed HTML help file (.chm) \n"
                    "of the generated HTML documentation. \n"
                 );
  addDependency("htmlHelpFlag","generateHtml");
  ConfigBool::add(  "noIndexFlag",
                    "DISABLE_INDEX",
                    "FALSE",
                    "generate condensed index flag",
                    "The DISABLE_INDEX tag can be used to turn on/off the condensed index at \n"
                    "top of each HTML page. The value NO (the default) enables the index and \n"
                    "the value YES disables it. \n"
                 );
  addDependency("noIndexFlag","generateHtml");
  //-----------------------------------------------------------------------------------------------
  ConfigInfo::add(  "LaTeX","configuration options related to the LaTeX output");
  //-----------------------------------------------------------------------------------------------
  ConfigBool::add(  "generateLatex",
                    "GENERATE_LATEX",
                    "TRUE",
                    "generate Latex output",
                    "If the GENERATE_LATEX tag is set to YES (the default) Doxygen will \n"
                    "generate Latex output. \n"
                 );
  ConfigString::add("latexOutputDir",
                    "LATEX_OUTPUT",
                    "latex",
                    "the directory to put the Latex files",
                    "The LATEX_OUTPUT tag is used to specify where the LaTeX docs will be put. \n"
                    "If a relative path is entered the value of OUTPUT_DIRECTORY will be \n"
                    "put in front of it. If left blank `latex' will be used as the default path. \n",
                    ConfigString::Dir
                   );
  addDependency("latexOutputDir","generateLatex");
  ConfigBool::add(  "compactLatexFlag",
                    "COMPACT_LATEX",
                    "FALSE",
                    "generate compact LaTeX documentation.",
                    "If the COMPACT_LATEX tag is set to YES Doxygen generates more compact \n"
                    "LaTeX documents. This may be useful for small projects and may help to \n"
                    "save some trees in general. \n"
                 );
  addDependency("compactLatexFlag","generateLatex");
  ConfigString::add("paperType",
                    "PAPER_TYPE",
                    "a4wide",
                    "the page type to generate docs for",
                    "The PAPER_TYPE tag can be used to set the paper type that is used \n"
                    "by the printer. Possible values are: a4, a4wide, letter, legal and \n"
                    "executive. If left blank a4wide will be used. \n"
                   );
  ConfigString::addFixedValue("paperType","a4");
  ConfigString::addFixedValue("paperType","a4wide");
  ConfigString::addFixedValue("paperType","letter");
  ConfigString::addFixedValue("paperType","legal");
  ConfigString::addFixedValue("paperType","executive");
  addDependency("paperType","generateLatex");
  ConfigList::add(  "extraPackageList",
                    "EXTRA_PACKAGES",
                    "",
                    "list of extra LaTeX packages.",
                    "The EXTRA_PACKAGES tag can be to specify one or more names of LaTeX \n"
                    "packages that should be included in the LaTeX output. \n"
                 );
  addDependency("extraPackageList","generateLatex");
  ConfigString::add("latexHeaderFile",
                    "LATEX_HEADER",
                    "",
                    "the name of the personal LaTeX header",
                    "The LATEX_HEADER tag can be used to specify a personal LaTeX header for \n"
                    "the generated latex document. The header should contain everything until \n"
                    "the first chapter. If it is left blank doxygen will generate a \n"
                    "standard header. Notice: only use this tag if you know what you are doing! \n",
                    ConfigString::File
                 );
  addDependency("latexHeaderFile","generateLatex");
  ConfigBool::add(  "pdfHyperFlag",
                    "PDF_HYPERLINKS",
                    "FALSE",
                    "generate latex prepared creating hyperlinked pdfs.",
                    "If the PDF_HYPERLINKS tag is set to YES, the LaTeX that is generated \n"
                    "is prepared for conversion to pdf (using ps2pdf). The pdf file will \n"
                    "contain links (just like the HTML output) instead of page references \n"
                    "This makes the output suitable for online browsing using a pdf viewer. \n"
                 );
  addDependency("pdfHyperFlag","generateLatex");
  ConfigBool::add(  "usePDFLatexFlag",
                    "USE_PDFLATEX",
                    "FALSE",
                    "use pdflatex instead of plain latex",
                    "If the USE_PDFLATEX tag is set to YES, pdflatex will be used instead of \n"
                    "plain latex in the generated Makefile. Set this option to YES to get a \n"
                    "higher quality PDF documentation. \n"
                 );   
  addDependency("usePDFLatexFlag","generateLatex");
  ConfigBool::add(  "latexBatchModeFlag",
                    "LATEX_BATCHMODE",
                    "FALSE",
                    "continue after latex errors?",
                    "If the LATEX_BATCHMODE tag is set to YES, doxygen will add the \\\\batchmode. \n"
                    "command to the generated LaTeX files. This will instruct LaTeX to keep \n"
                    "running if errors occur, instead of asking the user for help. \n"
                    "This option is also used when generating formulas in HTML. \n"
                 );
  addDependency("latexBatchModeFlag","generateLatex");
  //-----------------------------------------------------------------------------------------------
  ConfigInfo::add(  "RTF","configuration options related to the RTF output");
  //-----------------------------------------------------------------------------------------------
  ConfigBool::add(  "generateRTF",
                    "GENERATE_RTF",
                    "TRUE",
                    "generate RTF flag",
                    "If the GENERATE_RTF tag is set to YES Doxygen will generate RTF output \n"
                    "The RTF output is optimised for Word 97 and may not look very pretty with \n"
                    "other RTF readers or editors.\n"
                 );
  ConfigString::add("rtfOutputDir",
                    "RTF_OUTPUT",
                    "rtf",
                    "the directory to put the RTF files",
                    "The RTF_OUTPUT tag is used to specify where the RTF docs will be put. \n"
                    "If a relative path is entered the value of OUTPUT_DIRECTORY will be \n"
                    "put in front of it. If left blank `rtf' will be used as the default path. \n",
                    ConfigString::Dir
                 );
  addDependency("rtfOutputDir","generateRTF");
  ConfigBool::add(  "compactRTFFlag",
                    "COMPACT_RTF",
                    "FALSE",
                    "generate more compact RTF",
                    "If the COMPACT_RTF tag is set to YES Doxygen generates more compact \n"
                    "RTF documents. This may be useful for small projects and may help to \n"
                    "save some trees in general. \n"
                 );
  addDependency("compactRTFFlag","generateRTF");
  ConfigBool::add(  "rtfHyperFlag",
                    "RTF_HYPERLINKS",
                    "FALSE",
                    "generate hyper links in RTF",
                    "If the RTF_HYPERLINKS tag is set to YES, the RTF that is generated \n"
                    "will contain hyperlink fields. The RTF file will \n"
                    "contain links (just like the HTML output) instead of page references. \n"
                    "This makes the output suitable for online browsing using a WORD or other. \n"
                    "programs which support those fields. \n"
                    "Note: wordpad (write) and others do not support links. \n"
                 );
  addDependency("rtfHyperFlag","generateRTF");
  ConfigString::add("rtfStylesheetFile",
                    "RTF_STYLESHEET_FILE",
                    "",
                    "file to load stylesheet definitions from",
                    "Load stylesheet definitions from file. Syntax is similar to doxygen's \n"
                    "config file, i.e. a series of assigments. You only have to provide \n"
                    "replacements, missing definitions are set to their default value. \n",
                    ConfigString::File
                   );
  addDependency("rtfStylesheetFile","generateRTF");                  
  //-----------------------------------------------------------------------------------------------
  ConfigInfo::add(  "Man","configuration options related to the man page output");
  //-----------------------------------------------------------------------------------------------
  ConfigBool::add(  "generateMan",
                    "GENERATE_MAN",
                    "TRUE",
                    "generate Man pages",
                    "If the GENERATE_MAN tag is set to YES (the default) Doxygen will \n"
                    "generate man pages \n"
                   );
  ConfigString::add("manOutputDir",
                    "MAN_OUTPUT",
                    "man",
                    "the directory to put the man pages",
                    "The MAN_OUTPUT tag is used to specify where the man pages will be put. \n"
                    "If a relative path is entered the value of OUTPUT_DIRECTORY will be \n"
                    "put in front of it. If left blank `man' will be used as the default path. \n",
                    ConfigString::Dir
                   );
  addDependency("manOutputDir","generateMan");
  ConfigString::add("manExtension",
                    "MAN_EXTENSION",
                    ".3",
                    "extension the man page files",
                    "The MAN_EXTENSION tag determines the extension that is added to \n"
                    "the generated man pages (default is the subroutine's section .3) \n"
                   );
  addDependency("manExtension","generateMan");
  //-----------------------------------------------------------------------------------------------
  ConfigInfo::add(  "XML","configuration options related to the XML output");
  //-----------------------------------------------------------------------------------------------
  ConfigBool::add(  "generateXML",
                    "GENERATE_XML",
                    "FALSE",
                    "generate XML output",
                    "If the GENERATE_XML tag is set to YES Doxygen will \n"
                    "generate an XML file that captures the structure of \n"
                    "the code including all documentation. Warning: This feature \n"
                    "is still experimental and very incomplete.\n"
                 );
  //-----------------------------------------------------------------------------------------------
  ConfigInfo::add(  "Preprocessor","Configuration options related to the preprocessor   ");
  //-----------------------------------------------------------------------------------------------
  ConfigBool::add(  "preprocessingFlag",
                    "ENABLE_PREPROCESSING",
                    "TRUE",
                    "enable preprocessing",
                    "If the ENABLE_PREPROCESSING tag is set to YES (the default) Doxygen will \n"
                    "evaluate all C-preprocessor directives found in the sources and include \n"
                    "files. \n"
                 );
  ConfigBool::add(  "macroExpansionFlag",
                    "MACRO_EXPANSION",
                    "FALSE",
                    "expand macros in the source.",
                    "If the MACRO_EXPANSION tag is set to YES Doxygen will expand all macro \n"
                    "names in the source code. If set to NO (the default) only conditional \n"
                    "compilation will be performed. Macro expansion can be done in a controlled \n"
                    "way by setting EXPAND_ONLY_PREDEF to YES. \n"
                 );
  addDependency("macroExpansionFlag","preprocessingFlag");
  ConfigBool::add(  "onlyPredefinedFlag",
                    "EXPAND_ONLY_PREDEF",
                    "FALSE",
                    "expand only predefined macros",
                    "If the EXPAND_ONLY_PREDEF and MACRO_EXPANSION tags are both set to YES \n"
                    "then the macro expansion is limited to the macros specified with the \n"
                    "PREDEFINED and EXPAND_AS_PREDEFINED tags. \n"
                 );
  addDependency("onlyPredefinedFlag","preprocessingFlag");
  ConfigBool::add(  "searchIncludeFlag",
                    "SEARCH_INCLUDES",
                    "TRUE",
                    "search for included files",
                    "If the SEARCH_INCLUDES tag is set to YES (the default) the includes files \n"
                    "in the INCLUDE_PATH (see below) will be search if a #include is found. \n"
                 );
  addDependency("searchIncludeFlag","preprocessingFlag");
  ConfigList::add(  "includePath",
                    "INCLUDE_PATH",
                    "",
                    "list of include paths",
                    "The INCLUDE_PATH tag can be used to specify one or more directories that \n"
                    "contain include files that are not input files but should be processed by \n"
                    "the preprocessor. \n",
                    ConfigList::Dir
                 );
  addDependency("includePath","preprocessingFlag");
  ConfigList::add(  "includeFilePatternList",
                    "INCLUDE_FILE_PATTERNS",
                    "",
                    "list of include file patterns",
                    "You can use the INCLUDE_FILE_PATTERNS tag to specify one or more wildcard \n"
                    "patterns (like *.h and *.hpp) to filter out the header-files in the \n"
                    "directories. If left blank, the patterns specified with FILE_PATTERNS will \n"
                    "be used. \n"
                 );
  addDependency("includeFilePatternList","preprocessingFlag");
  ConfigList::add(  "predefined",
                    "PREDEFINED",
                    "",
                    "list of predefined macro names.",
                    "The PREDEFINED tag can be used to specify one or more macro names that \n"
                    "are defined before the preprocessor is started (similar to the -D option of \n"
                    "gcc). The argument of the tag is a list of macros of the form: name \n"
                    "or name=definition (no spaces). If the definition and the = are \n"
                    "omitted =1 is assumed. \n"
                 );
  addDependency("predefined","preprocessingFlag");
  ConfigList::add(  "expandAsDefinedList",
                    "EXPAND_AS_DEFINED",
                    "",
                    "list of defines to expand",
                    "If the MACRO_EXPANSION and EXPAND_PREDEF_ONLY tags are set to YES then \n"
                    "this tag can be used to specify a list of macro names that should be expanded. \n"
                    "The macro definition that is found in the sources will be used. \n"
                    "Use the PREDEFINED tag if you want to use a different macro definition. \n" 
                 );
  //-----------------------------------------------------------------------------------------------
  ConfigInfo::add(  "External","Configuration::addtions related to external references   ");
  //-----------------------------------------------------------------------------------------------
  ConfigList::add(  "tagFileList",
                    "TAGFILES",
                    "",
                    "list of tag files",
                    "The TAGFILES tag can be used to specify one or more tagfiles. \n",
                    ConfigList::File
                 );
  ConfigString::add("genTagFile",
                    "GENERATE_TAGFILE",
                    "",
                    "the tag file to generate",
                    "When a file name is specified after GENERATE_TAGFILE, doxygen will create \n"
                    "a tag file that is based on the input files it reads. \n",
                    ConfigString::File
                   );
  ConfigBool::add(  "allExtFlag",
                    "ALLEXTERNALS",
                    "FALSE",
                    "include all external classes flag",
                    "If the ALLEXTERNALS tag is set to YES all external classes will be listed \n"
                    "in the class index. If set to NO only the inherited external classes \n"
                    "will be listed. \n"
                 );
  ConfigString::add("perlPath",
                    "PERL_PATH",
                    "/usr/bin/perl",
                    "the absolute path to perl",
                    "The PERL_PATH should be the absolute path and name of the perl script \n"
                    "interpreter (i.e. the result of `which perl'). \n",
                    ConfigString::Dir
                   );
  //-----------------------------------------------------------------------------------------------
  ConfigInfo::add(  "Dot","Configuration options related to the dot tool   ");
  //-----------------------------------------------------------------------------------------------
  ConfigBool::add(  "haveDotFlag",
                    "HAVE_DOT",
                    "FALSE",
                    "indicates wether or not dot is present",
                    "If you set the HAVE_DOT tag to YES then doxygen will assume the dot tool is \n"
                    "available from the path. This tool is part of Graphviz, a graph visualization \n"
                    "toolkit from AT&T and Lucent Bell Labs. The other options in this section \n"
                    "have no effect if this option is set to NO (the default) \n"
                 );
  ConfigBool::add(  "classGraphFlag",
                    "CLASS_GRAPH",
                    "TRUE",
                    "class graph",
                    "If the CLASS_GRAPH and HAVE_DOT tags are set to YES then doxygen \n"
                    "will generate a graph for each documented class showing the direct and \n"
                    "indirect inheritance relations. Setting this tag to YES will force the \n"
                    "the CLASS_DIAGRAMS tag to NO.\n"
                 );
  addDependency("classGraphFlag","haveDotFlag");
  ConfigBool::add(  "collGraphFlag",
                    "COLLABORATION_GRAPH",
                    "TRUE",
                    "collaboration graph",
                    "If the COLLABORATION_GRAPH and HAVE_DOT tags are set to YES then doxygen \n"
                    "will generate a graph for each documented class showing the direct and \n"
                    "indirect implementation dependencies (inheritance, containment, and \n"
                    "class references variables) of the class with other documented classes. \n"
                 );
  addDependency("collGraphFlag","haveDotFlag");
  ConfigBool::add(  "includeGraphFlag",
                    "INCLUDE_GRAPH",
                    "TRUE",
                    "include graph",
                    "If the ENABLE_PREPROCESSING, INCLUDE_GRAPH, and HAVE_DOT tags are set to \n"
                    "YES then doxygen will generate a graph for each documented file showing \n"
                    "the direct and indirect include dependencies of the file with other \n"
                    "documented files. \n"
                 );
  addDependency("includeGraphFlag","haveDotFlag");
  ConfigBool::add(  "includedByGraphFlag",
                    "INCLUDED_BY_GRAPH",
                    "TRUE",
                    "depends on include graph",
                    "If the ENABLE_PREPROCESSING, INCLUDED_BY_GRAPH, and HAVE_DOT tags are set to \n"
                    "YES then doxygen will generate a graph for each documented header file showing \n"
                    "the documented files that directly or indirectly include this file \n"
                 );
  addDependency("includedByGraphFlag","haveDotFlag");
  ConfigBool::add(  "gfxHierarchyFlag",
                    "GRAPHICAL_HIERARCHY",
                    "TRUE",
                    "flag to enable graphical hierarchy",
                    "If the GRAPHICAL_HIERARCHY and HAVE_DOT tags are set to YES then doxygen \n"
                    "will graphical hierarchy of all classes instead of a textual one. \n"
                 );
  addDependency("gfxHierarchyFlag","haveDotFlag");
  ConfigString::add("dotPath",
                    "DOT_PATH",
                    "",
                    "path to the dot tool",
                    "The tag DOT_PATH can be used to specify the path where the dot tool can be \n"
                    "found. If left blank, it is assumed the dot tool can be found on the path. \n",
                    ConfigString::Dir
                   );
  addDependency("dotPath","haveDotFlag");
  ConfigInt::add(   "maxDotGraphWidth",
                    "MAX_DOT_GRAPH_WIDTH",
                    "1024",
                    "max dot graph width",
                    "The MAX_DOT_GRAPH_WIDTH tag can be used to set the maximum allowed width \n"
                    "(in pixels) of the graphs generated by dot. If a graph becomes larger than \n"
                    "this value, doxygen will try to truncate the graph, so that it fits within \n"
                    "the specified constraint. Beware that most browsers cannot cope with very \n"
                    "large images. \n",
                    100,30000
                );
  addDependency("maxDotGraphWidth","haveDotFlag");
  ConfigInt::add(   "maxDotGraphHeight",
                    "MAX_DOT_GRAPH_HEIGHT",
                    "1024",
                    "max dot graph height",
                    "The MAX_DOT_GRAPH_HEIGHT tag can be used to set the maximum allows height \n"
                    "(in pixels) of the graphs generated by dot. If a graph becomes larger than \n"
                    "this value, doxygen will try to truncate the graph, so that it fits within \n"
                    "the specified constraint. Beware that most browsers cannot cope with very \n"
                    "large images. \n",
                    100,30000
                );
  addDependency("maxDotGraphHeight","haveDotFlag");
 

  //-----------------------------------------------------------------------------------------------
  ConfigInfo::add(  "Search","Configuration::addtions related to the search engine   ");
  //-----------------------------------------------------------------------------------------------
  ConfigBool::add(  "searchEngineFlag",
                    "SEARCHENGINE",
                    "FALSE",
                    "generate search engine flag",
                    "The SEARCHENGINE tag specifies whether or not a search engine should be \n"
                    "used. If set to NO the values of all tags below this one will be ignored. \n"
                 );
  ConfigString::add("cgiName",
                    "CGI_NAME",
                    "search.cgi",
                    "the name of the CGI binary",
                    "The CGI_NAME tag should be the name of the CGI script that \n"
                    "starts the search engine (doxysearch) with the correct parameters. \n"
                    "A script with this name will be generated by doxygen. \n"
                   );
  addDependency("cgiName","searchEngineFlag");
  ConfigString::add("cgiURL",
                    "CGI_URL",
                    "",
                    "the absolute URL to the CGI binary",
                    "The CGI_URL tag should be the absolute URL to the directory where the \n"
                    "cgi binaries are located. See the documentation of your http daemon for \n"
                    "details. \n"
                   );
  addDependency("cgiURL","searchEngineFlag");
  ConfigString::add("docURL",
                    "DOC_URL",
                    "",
                    "the absolute URL to the documentation",
                    "The DOC_URL tag should be the absolute URL to the directory where the \n"
                    "documentation is located. If left blank the absolute path to the \n"
                    "documentation, with file:// prepended to it, will be used. \n"
                   );
  addDependency("docURL","searchEngineFlag");
  ConfigString::add("docAbsPath",
                    "DOC_ABSPATH",
                    "",
                    "the absolute path to the documentation",
                    "The DOC_ABSPATH tag should be the absolute path to the directory where the \n"
                    "documentation is located. If left blank the directory on the local machine \n"
                    "will be used. \n",
                    ConfigString::Dir
                   );
  addDependency("docAbsPath","searchEngineFlag");
  ConfigString::add("binAbsPath",
                    "BIN_ABSPATH",
                    "/usr/local/bin/",
                    "the absolute path to the doxysearch",
                    "The BIN_ABSPATH tag must point to the directory where the doxysearch binary \n"
                    "is installed. \n",
                    ConfigString::Dir
                   );
  addDependency("binAbsPath","searchEngineFlag");
  ConfigList::add(  "extDocPathList",
                    "EXT_DOC_PATHS",
                    "",
                    "list of external doc. directories.",
                    "The EXT_DOC_PATHS tag can be used to specify one or more paths to \n"
                    "documentation generated for other projects. This allows doxysearch to search \n"
                    "the documentation for these projects as well. \n",
                    ConfigList::Dir
                 );
  addDependency("extDocPathList","searchEngineFlag");

  // The IMAGE_PATTERNS tag is now officially obsolete.
}
