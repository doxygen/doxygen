/******************************************************************************
 *
 * Copyright (C) 1997-2019 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby
 * granted. No representations are made about the suitability of this software
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#include "expert.h"
#include "inputbool.h"
#include "inputstring.h"
#include "inputint.h"
#include "inputstring.h"
#include "inputstrlist.h"
#include "inputobsolete.h"
#include "config.h"
#include "doxywizard.h"
#include "version.h"
#include "configdoc.h"
#include "settings.h"
#include "optiontranslations.h"
#include "translationmanager.h"

#include <QTreeWidget>
#include <QStackedWidget>
#include <QTextBrowser>
#include <QSplitter>
#include <QGridLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QFile>
#include <QMessageBox>
#include <QSettings>
#include <QTextStream>
#include <QFileInfo>
#include <QRegularExpression>

#define SA(x) QString::fromLatin1(x)


static QString convertToComment(const QString &s)
{
  if (s.isEmpty())
  {
    return QString();
  }
  else
  {
    return SA("# ")+
           s.trimmed().replace(SA("\n"),SA("\n# ")).replace(SA("# \n"), SA("#\n"))+
           SA("\n");
  }
}

QString convertDoxyCmdToHtml(const QString &s)
{
  QString result = s;
  
  QRegularExpression regexp;
  
  // remove \n at end and replace by a space
  regexp.setPattern(SA("\\n$"));
  result.replace(regexp,SA(" "));
  
  // remove <br> at end
  regexp.setPattern(SA("<br> *$"));
  result.replace(regexp,SA(" "));
  
  // \c word -> <code>word</code>; word ends with ')', ',', '.' or ' '
  regexp.setPattern(SA("\\\\c[ ]+([^ \\)]+)\\)"));
  result.replace(regexp,SA("<code>\\1</code>)"));
  
  regexp.setPattern(SA("\\\\c[ ]+([^ ,]+),"));
  result.replace(regexp,SA("<code>\\1</code>,"));
  
  regexp.setPattern(SA("\\\\c[ ]+([^ \\.]+)\\."));
  result.replace(regexp,SA("<code>\\1</code>."));
  
  regexp.setPattern(SA("\\\\c[ ]+([^ ]+) "));
  result.replace(regexp,SA("<code>\\1</code> "));
  
  result.replace(SA("\\c("), SA("<code>("));
  
  // `word` -> <code>word</code>
  result.replace(SA("``"),SA(""));
  regexp.setPattern(SA("`([^`]+)`"));
  result.replace(regexp,SA("<code>\\1</code>"));
  
  // \p word -> <code>word</code>
  result.replace(SA("\\p "), SA("<code>"));
  result.replace(SA("\\p("), SA("<code>("));
  
  // \b word -> <b>word</b>
  regexp.setPattern(SA("\\\\b[ ]+([^ ]+) "));
  result.replace(regexp,SA("<b>\\1</b> "));
  
  // \e word -> <em>word</em>
  regexp.setPattern(SA("\\\\e[ ]+([^ ]+) "));
  result.replace(regexp,SA("<em>\\1</em> "));
  result.replace(SA("\\e "), SA("<em>"));
  result.replace(SA("\\e("), SA("<em>("));
  
  // \a word -> <em>word</em>
  result.replace(SA("\\a "), SA("<em>"));
  result.replace(SA("\\a("), SA("<em>("));
  
  // \ref key "desc" -> desc
  // First handle special \ref cases that need to preserve the \ref for later
  regexp.setPattern(SA("\\\\ref[ ]+[^ ]+[ ]+\"\\\\\\\\ref\""));
  result.replace(regexp,SA("\\\\REF"));
  regexp.setPattern(SA("\\\\ref[ ]+[^ ]+[ ]+\"([^\"]+)\""));
  result.replace(regexp,SA("<code>\\1</code> "));
  
  // \ref specials
  regexp.setPattern(SA("\\\\ref[ ]+doxygen_usage"));
  result.replace(regexp,SA("\"Doxygen usage\""));
  regexp.setPattern(SA("\\\\ref[ ]+extsearch"));
  result.replace(regexp,SA("\"External Indexing and Searching\""));
  regexp.setPattern(SA("\\\\ref[ ]+external"));
  result.replace(regexp,SA("\"Linking to external documentation\""));
  regexp.setPattern(SA("\\\\ref[ ]+formulas"));
  result.replace(regexp,SA("\"Including formulas\""));
  
  // fallback for not handled \ref
  result.replace(SA("\\\\ref"),SA(""));
  result.replace(SA("\\\\REF"),SA("\\\\ref"));
  
  // \note -> <br>Note:
  result.replace(SA("\\note "),SA("<br>Note: "));
  result.replace(SA("@note "),SA("<br>Note: "));
  
  // \#include -> #include
  result.replace(SA("\\#include"),SA("#include"));
  result.replace(SA("\\#undef"),SA("#undef"));
  result.replace(SA("\\# "),SA("# "));
  
  // -# -> <br>-
  result.replace(SA("-#"),SA("<br>-"));
  result.replace(SA(" - "),SA("<br>-"));
  
  // \sa -> <br>See also:
  result.replace(SA("\\sa "),SA("<br>See also: "));
  
  // \par -> <br>
  result.replace(SA("\\par "),SA("<br>"));
  
  // \verbatim -> <pre>
  result.replace(SA("\\verbatim"),SA("<pre>"));
  result.replace(SA("\\endverbatim"),SA("</pre>"));
  
  // \n -> <br/>
  result.replace(SA("\\n"),SA("<br/>"));
  
  // \& -> &amp;
  result.replace(SA("\\&lt;"), SA("&lt;"));
  result.replace(SA("\\&gt;"), SA("&gt;"));
  result.replace(SA("\\&amp;"), SA("&amp;"));
  result.replace(SA("\\&"),SA("&amp;"));
  
  // \$ -> $
  result.replace(SA("\\$"),SA("$"));
  
  // \< -> &lt;
  result.replace(SA("\\<"),SA("&lt;"));
  
  // \> -> &gt;
  result.replace(SA("\\>"),SA("&gt;"));
  
  // \@ -> @
  result.replace(SA("\\@"),SA("@"));
  
  // \\ -> \
  result.replace(SA("\\\\"),SA("\\"));
  
  // http/https URLs -> links
  // Only match URL-safe characters, exclude trailing punctuation
  regexp.setPattern(SA(" (https?://[a-zA-Z0-9_\\-./~%#+?=&]*[a-zA-Z0-9_\\-/~%#+?=&])"));
  QRegularExpressionMatchIterator urlIt = regexp.globalMatch(result);
  QStringList urlReplacements;
  while (urlIt.hasNext())
  {
    QRegularExpressionMatch match = urlIt.next();
    QString url = match.captured(1);
    urlReplacements.append(match.captured(0));
    urlReplacements.append(SA(" <a href=\"") + url + SA("\">") + url + SA("</a>"));
  }
  for (int i = 0; i < urlReplacements.size(); i += 2)
  {
    result.replace(urlReplacements[i], urlReplacements[i + 1]);
  }
  
  // LaTeX formulas
  regexp.setPattern(SA("\\\\f\\$\\\\mbox\\{\\\\LaTeX\\}\\\\f\\$"));
  result.replace(regexp,SA("LaTeX"));
  regexp.setPattern(SA("\\\\f\\$2\\^\\{\\(16\\+\\\\mbox\\{LOOKUP\\\\_CACHE\\\\_SIZE\\}\\)\\}\\\\f\\$"));
  result.replace(regexp,SA("2^(16+LOOKUP_CACHE_SIZE)"));
  regexp.setPattern(SA("\\\\f\\$2\\^\\{16\\} = 65536\\\\f\\$"));
  result.replace(regexp,SA("2^16=65536"));
  
  // <br> -> <br/>
  result.replace(SA("&lt;br&gt;"), SA("<br/>"));
  result.replace(SA("&lt;br/&gt;"), SA("<br/>"));
  
  return result;
}

void Expert::setHeader(const char *header)
{
  m_header = SA(header);
}

void Expert::add(const char *name,const char *docs)
{
  Input *opt = m_options[SA(name)];
  if (opt)
  {
    opt->setTemplateDocs(SA(docs));
  }
}

//------------------------------------------------------------------------------------

Expert::Expert()
{
  m_treeWidget = new QTreeWidget;
  m_treeWidget->setColumnCount(1);
  m_topicStack = new QStackedWidget;
  m_inShowHelp = false;

  loadConfigXml();
  createTopics(m_rootElement);
  m_helper = new QTextBrowser;
  m_helper->setReadOnly(true);
  m_helper->setOpenExternalLinks(true);
  m_splitter = new QSplitter(Qt::Vertical);
  m_splitter->addWidget(m_treeWidget);
  m_splitter->addWidget(m_helper);

  QWidget *rightSide = new QWidget;
  QGridLayout *grid = new QGridLayout(rightSide);
  m_prev = new QPushButton(DoxygenWizard::msgPreviousButton());
  m_prev->setEnabled(false);
  m_next = new QPushButton(DoxygenWizard::msgNextButton());
  grid->addWidget(m_topicStack,0,0,1,2);
  grid->addWidget(m_prev,1,0,Qt::AlignLeft);
  grid->addWidget(m_next,1,1,Qt::AlignRight);
  grid->setColumnStretch(0,1);
  grid->setRowStretch(0,1);

  addWidget(m_splitter);
  addWidget(rightSide);
  connect(m_next,SIGNAL(clicked()),SLOT(nextTopic()));

  connect(m_prev,SIGNAL(clicked()),SLOT(prevTopic()));

  addConfigDocs(this);
}

void Expert::loadConfigXml()
{
  QString langCode = TranslationManager::instance().currentLanguageCode();
  QString configPath;
  
  if (langCode != QLatin1String("en"))
  {
    configPath = SA(":/config_") + langCode + SA(".xml");
    QFile langFile(configPath);
    if (!langFile.exists())
    {
      configPath = SA(":/config.xml");
    }
  }
  else
  {
    configPath = SA(":/config.xml");
  }
  
  QFile file(configPath);
  QString err;
  int errLine,errCol;
  QDomDocument configXml;
  if (file.open(QIODevice::ReadOnly))
  {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    if (!configXml.setContent(&file,&err,&errLine,&errCol))
#else
    if (!configXml.setContent(&file,false,&err,&errLine,&errCol))
#endif
    {
      QString msg = tr("Error parsing internal config.xml at line %1 column %2.\n%3").
                  arg(errLine).arg(errCol).arg(err);
      QMessageBox::warning(this, tr("Error"), msg);
      exit(1);
    }
  }
  m_rootElement = configXml.documentElement();
}

Expert::~Expert()
{
  QHashIterator<QString,Input*> i(m_options);
  while (i.hasNext())
  {
    i.next();
    delete i.value();
  }
}

void Expert::createTopics(const QDomElement &rootElem)
{
  QList<QTreeWidgetItem*> items;
  QDomElement childElem = rootElem.firstChildElement();
  while (!childElem.isNull())
  {
    if (childElem.tagName()==SA("group"))
    {
      // Remove _ from a group name like: Source_Browser
      QString name = childElem.attribute(SA("name")).replace(SA("_"),SA(" "));
      QString setting = childElem.attribute(SA("setting"));
      if (setting.isEmpty() || IS_SUPPORTED(setting.toLatin1()))
      {
        QString translatedName = DoxygenWizard::translateExpertTopic(name);
        items.append(new QTreeWidgetItem((QTreeWidget*)nullptr,QStringList(translatedName)));
        QWidget *widget = createTopicWidget(childElem);
        m_topics[name] = widget;
        m_topicStack->addWidget(widget);
      }
    }
    childElem = childElem.nextSiblingElement();
  }
  m_treeWidget->setHeaderLabels(QStringList() << DoxygenWizard::msgTopicsHeader());
  m_treeWidget->insertTopLevelItems(0,items);
  connect(m_treeWidget,
          SIGNAL(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)),
          this,
          SLOT(activateTopic(QTreeWidgetItem *,QTreeWidgetItem *)));
}

static QString getDocsForNode(const QDomElement &child)
{
  QString type = child.attribute(SA("type"));
  QString id = child.attribute(SA("id"));
  QString docs = SA("");
  // read documentation text
  QDomElement docsVal = child.firstChildElement();
  while (!docsVal.isNull())
  {
    if (docsVal.tagName()==SA("docs") &&
        docsVal.attribute(SA("doxywizard")) != SA("0"))
    {
      for (QDomNode n = docsVal.firstChild(); !n.isNull(); n = n.nextSibling())
      {
        QDomText t = n.toText();
        if (!t.isNull()) docs+=t.data();
      }
      docs += SA("<br/>");
    }
    docsVal = docsVal.nextSiblingElement();
  }

  // for an enum we list the values
  if (type==SA("enum"))
  {
    if (!docs.endsWith(SA("<br/>"))) docs += SA("<br/>");
    docs += SA("<br/>");
    docs += Expert::tr("Possible values are:");
    docs += SA(" ");
    int numValues=0;
    docsVal = child.firstChildElement();
    while (!docsVal.isNull())
    {
      if (docsVal.tagName()==SA("value"))
      {
        numValues++;
      }
      docsVal = docsVal.nextSiblingElement();
    }
    int i=0;
    docsVal = child.firstChildElement();
    while (!docsVal.isNull())
    {
      if (docsVal.tagName()==SA("value"))
      {
        i++;
        docs += SA("<code>") + docsVal.attribute(SA("name")) + SA("</code>");
        QString desc = docsVal.attribute(SA("desc"));
        if (!desc.isEmpty())
        {
          docs+= SA(" ")+desc;
        }
        if (i==numValues-1)
        {
          docs+=SA(" ") + Expert::tr("and") + SA(" ");
        }
        else if (i==numValues)
        {
          docs+=SA(".");
        }
        else
        {
          docs+=SA(", ");
        }
      }
      docsVal = docsVal.nextSiblingElement();
    }
    if (child.attribute(SA("defval")) != SA(""))
    {
      docs+=SA("<br/>");
      docs+=SA("<br/>");
      QString defval = child.attribute(SA("defval"));
      docs+=SA(" ")+Expert::tr("The default value is: <code>%1</code>.").arg(defval);
    }
    docs+= SA("<br/>");
  }
  else if (type==SA("int"))
  {
    if (!docs.endsWith(SA("<br/>"))) docs += SA("<br/>");
    docs += SA("<br/>");
    QString minval = child.attribute(SA("minval"));
    QString maxval = child.attribute(SA("maxval"));
    QString defval = child.attribute(SA("defval"));
    docs+=Expert::tr("Minimum value: %1, maximum value: %2, default value: %3.").arg(minval).arg(maxval).arg(defval);
    docs+= SA("<br/>");
  }
  else if (type==SA("bool"))
  {
    if (!docs.endsWith(SA("<br/>"))) docs += SA("<br/>");
    docs += SA("<br/>");
    if (child.hasAttribute(SA("altdefval")))
    {
      docs+=SA(" ")+Expert::tr("The default value is: system dependent.");
    }
    else
    {
      QString defval = child.attribute(SA("defval"));
      QString valStr = (defval==SA("1")?SA("YES"):SA("NO"));
      docs+=SA(" ")+Expert::tr("The default value is: <code>%1</code>.").arg(valStr);
    }
    docs+= SA("<br/>");
  }
  else if (type==SA("list"))
  {
    if (child.attribute(SA("format"))==SA("string"))
    {
      int numValues = 0;
      docsVal = child.firstChildElement();
      while (!docsVal.isNull())
      {
        if (docsVal.tagName()==SA("value"))
        {
          QString showDocu = SA("");
          if (docsVal.hasAttribute(SA("show_docu")))
          {
            showDocu = docsVal.attribute(SA("show_docu")).toLower();
          }
          if ((showDocu != SA("no")) && (docsVal.attribute(SA("name"))!=SA(""))) numValues++;
        }
        docsVal = docsVal.nextSiblingElement();
      }
      if (numValues>0)
      {
        if (!docs.endsWith(SA("<br/>"))) docs += SA("<br/>");
        docs += SA("<br/>");
        docs += Expert::tr("Possible values are:");
        docs += SA(" ");
        int i = 0;
        docsVal = child.firstChildElement();
        while (!docsVal.isNull())
        {
          if (docsVal.tagName()==SA("value"))
          {
            QString showDocu = SA("");
            if (docsVal.hasAttribute(SA("show_docu")))
            {
              showDocu = docsVal.attribute(SA("show_docu")).toLower();
            }
            if ((showDocu != SA("no")) && (docsVal.attribute(SA("name"))!=SA("")))
            {
              i++;
              docs += SA("<code>") + docsVal.attribute(SA("name")) + SA("</code>");
              QString desc = docsVal.attribute(SA("desc"));
              if (desc != SA(""))
              {
                docs += SA(" ") + desc;
              }
              if (i==numValues-1)
              {
                docs += SA(" ") + Expert::tr("and") + SA(" ");
              }
              else if (i==numValues)
              {
                docs += SA(".");
              }
              else
              {
                docs += SA(", ");
              }
            }
          }
          docsVal = docsVal.nextSiblingElement();
        }
      }
    }
  }
  else if (type==SA("string"))
  {
    QString defval = child.attribute(SA("defval"));
    if (child.attribute(SA("format")) == SA("dir"))
    {
      if (defval != SA(""))
      {
        if (!docs.endsWith(SA("<br/>"))) docs += SA("<br/>");
        docs += SA("<br/>");
        docs += SA(" ")+Expert::tr("The default directory is: <code>%1</code>.").arg(defval);
        docs += SA("<br/>");
      }
    }
    else if (child.attribute(SA("format")) == SA("file"))
    {
      QString abspath = child.attribute(SA("abspath"));
      if (defval != SA(""))
      {
        if (!docs.endsWith(SA("<br/>"))) docs += SA("<br/>");
        docs += SA("<br/>");
        if (abspath != SA("1"))
        {
          docs += SA(" ")+Expert::tr("The default file is: <code>%1</code>.").arg(defval);
        }
        else
        {
          docs += SA(" ")+Expert::tr("The default file (with absolute path) is: <code>%1</code>.").arg(defval);
        }
        docs += SA("<br/>");
      }
      else
      {
        if (abspath == SA("1"))
        {
          if (!docs.endsWith(SA("<br/>"))) docs += SA("<br/>");
          docs += SA("<br/>");
          docs += SA(" ")+Expert::tr("The file has to be specified with full path.");
          docs += SA("<br/>");
        }
      }
    }
    else if (child.attribute(SA("format")) == SA("image"))
    {
      QString abspath = child.attribute(SA("abspath"));
      if (defval != SA(""))
      {
        if (!docs.endsWith(SA("<br/>"))) docs += SA("<br/>");
        docs += SA("<br/>");
        if (abspath != SA("1"))
        {
          docs += SA(" ")+Expert::tr("The default image is: <code>%1</code>.").arg(defval);
        }
        else
        {
          docs += SA(" ")+Expert::tr("The default image (with absolute path) is: <code>%1</code>.").arg(defval);
        }
        docs += SA("<br/>");
      }
      else
      {
        if (abspath == SA("1"))
        {
          if (!docs.endsWith(SA("<br/>"))) docs += SA("<br/>");
          docs += SA("<br/>");
          docs += SA(" ")+Expert::tr("The image has to be specified with full path.");
          docs += SA("<br/>");
        }
      }
    }
    else // if (child.attribute(SA("format")) == SA("string"))
    {
      if (defval != SA(""))
      {
        if (!docs.endsWith(SA("<br/>"))) docs += SA("<br/>");
        docs += SA("<br/>");
        docs += SA(" ")+Expert::tr("The default value is: <code>%1</code>.").arg(defval);
        docs += SA("<br/>");
      }
    }
  }

  if (child.hasAttribute(SA("depends")))
  {
    QString dependsOn = child.attribute(SA("depends"));
    if (!docs.endsWith(SA("<br/>"))) docs += SA("<br/>");
    docs += SA("<br/>");
    docs+=SA(" ")+Expert::tr("This tag requires that the tag %1 is set to <code>YES</code>.").arg(SA("\\ref cfg_")+dependsOn.toLower()+SA(" \"")+dependsOn.toUpper()+SA("\""));
  }

  // Convert Doxygen markup to HTML
  docs = convertDoxyCmdToHtml(docs);
  
  return docs.trimmed();
}

QWidget *Expert::createTopicWidget(QDomElement &elem)
{
  QScrollArea *area   = new QScrollArea;
  QWidget     *topic  = new QWidget;
  QGridLayout *layout = new QGridLayout(topic);
  QDomElement child   = elem.firstChildElement();
  int row=0;
  while (!child.isNull())
  {
    QString setting = child.attribute(SA("setting"));
    if (setting.isEmpty() || IS_SUPPORTED(setting.toLatin1()))
    {
      QString type = child.attribute(SA("type"));
      QString docs = getDocsForNode(child);
      if (type==SA("bool"))
      {
        InputBool *boolOption =
          new InputBool(
              layout,row,
              child.attribute(SA("id")),
              child.attribute(SA("defval"))==SA("1"),
              docs
              );
        QString id = child.attribute(SA("id"));
        m_options.insert(id, boolOption);
        m_optionElements.insert(id, child);
        connect(boolOption,SIGNAL(showHelp(Input*)),SLOT(showHelp(Input*)));
        connect(boolOption,SIGNAL(changed()),SIGNAL(changed()));
      }
      else if (type==SA("string"))
      {
        InputString::StringMode mode;
        QString format = child.attribute(SA("format"));
        if (format==SA("dir"))
        {
          mode = InputString::StringDir;
        }
        else if (format==SA("file"))
        {
          mode = InputString::StringFile;
        }
        else if (format==SA("filedir"))
        {
          mode = InputString::StringFileDir;
        }
        else if (format==SA("image"))
        {
          mode = InputString::StringImage;
        }
        else // format=="string"
        {
          mode = InputString::StringFree;
        }
        InputString *stringOption =
          new InputString(
              layout,row,
              child.attribute(SA("id")),
              child.attribute(SA("defval")),
              mode,
              docs,
              child.attribute(SA("abspath"))
              );
        QString id = child.attribute(SA("id"));
        m_options.insert(id, stringOption);
        m_optionElements.insert(id, child);
        connect(stringOption,SIGNAL(showHelp(Input*)),SLOT(showHelp(Input*)));
        connect(stringOption,SIGNAL(changed()),SIGNAL(changed()));
      }
      else if (type==SA("enum"))
      {
        InputString *enumList = new InputString(
            layout,row,
            child.attribute(SA("id")),
            child.attribute(SA("defval")),
            InputString::StringFixed,
            docs
            );
        QDomElement enumVal = child.firstChildElement();
        while (!enumVal.isNull())
        {
          if (enumVal.tagName()==SA("value"))
          {
            enumList->addValue(enumVal.attribute(SA("name")));
          }
          enumVal = enumVal.nextSiblingElement();
        }
        enumList->setDefault();

        QString id = child.attribute(SA("id"));
        m_options.insert(id, enumList);
        m_optionElements.insert(id, child);
        connect(enumList,SIGNAL(showHelp(Input*)),SLOT(showHelp(Input*)));
        connect(enumList,SIGNAL(changed()),SIGNAL(changed()));
      }
      else if (type==SA("int"))
      {
        InputInt *intOption =
          new InputInt(
              layout,row,
              child.attribute(SA("id")),
              child.attribute(SA("defval")).toInt(),
              child.attribute(SA("minval")).toInt(),
              child.attribute(SA("maxval")).toInt(),
              docs
              );
        QString id = child.attribute(SA("id"));
        m_options.insert(id, intOption);
        m_optionElements.insert(id, child);
        connect(intOption,SIGNAL(showHelp(Input*)),SLOT(showHelp(Input*)));
        connect(intOption,SIGNAL(changed()),SIGNAL(changed()));
      }
      else if (type==SA("list"))
      {
        InputStrList::ListMode mode;
        QString format = child.attribute(SA("format"));
        if (format==SA("dir"))
        {
          mode = InputStrList::ListDir;
        }
        else if (format==SA("file"))
        {
          mode = InputStrList::ListFile;
        }
        else if (format==SA("filedir"))
        {
          mode = InputStrList::ListFileDir;
        }
        else // format=="string"
        {
          mode = InputStrList::ListString;
        }
        QStringList sl;
        QDomElement listVal = child.firstChildElement();
        while (!listVal.isNull())
        {
          if (listVal.tagName()==SA("value"))
          {
            sl.append(listVal.attribute(SA("name")));
          }
          listVal = listVal.nextSiblingElement();
        }
        InputStrList *listOption =
          new InputStrList(
              layout,row,
              child.attribute(SA("id")),
              sl,
              mode,
              docs
              );
        QString id = child.attribute(SA("id"));
        m_options.insert(id, listOption);
        m_optionElements.insert(id, child);
        connect(listOption,SIGNAL(showHelp(Input*)),SLOT(showHelp(Input*)));
        connect(listOption,SIGNAL(changed()),SIGNAL(changed()));
      }
      else if (type==SA("obsolete"))
      {
        Input::Kind orgKind;
        QString orgKindStr = child.attribute(SA("orgtype"));
        if (orgKindStr==SA("int"))
        {
          orgKind = Input::Int;
        }
        else if (orgKindStr==SA("bool"))
        {
          orgKind = Input::Bool;
        }
        else if (orgKindStr==SA("string") || orgKindStr==SA("enum"))
        {
          orgKind = Input::String;
        }
        else if (orgKindStr==SA("strlist"))
        {
          orgKind = Input::StrList;
        }
        else
        {
          orgKind = Input::Obsolete;
        }
        InputObsolete *obsoleteOption =
          new InputObsolete(child.attribute(SA("id")),orgKind);
        m_options.insert(child.attribute(SA("id")),obsoleteOption);
        // ignore
      }
      else // should not happen
      {
        printf("Unsupported type %s\n",qPrintable(child.attribute(SA("type"))));
      }
    } // IS_SUPPORTED
    child = child.nextSiblingElement();
  }

  // compute dependencies between options
  child = elem.firstChildElement();
  while (!child.isNull())
  {
    QString setting = child.attribute(SA("setting"));
    QString dependsOn = child.attribute(SA("depends"));
    QString id        = child.attribute(SA("id"));
    if (!dependsOn.isEmpty() &&
        (setting.isEmpty() || IS_SUPPORTED(setting.toLatin1())))
    {
       Input *parentOption = m_options[dependsOn];
       if (parentOption==nullptr)
       {
         printf("%s has depends=%s that is not valid\n",
             qPrintable(id),qPrintable(dependsOn));
       }
       Input *thisOption   = m_options[id];
       Q_ASSERT(parentOption);
       Q_ASSERT(thisOption);
       if (parentOption && thisOption)
       {
         //printf("Adding dependency '%s' (%p)->'%s' (%p)\n",
         //  qPrintable(dependsOn),parentOption,
         //  qPrintable(id),thisOption);
         parentOption->addDependency(thisOption);
       }
    }
    child = child.nextSiblingElement();
  }

  // set initial dependencies
  QHashIterator<QString,Input*> i(m_options);
  while (i.hasNext())
  {
    i.next();
    if (i.value())
    {
      i.value()->updateDependencies();
    }
  }

  layout->setRowStretch(row,1);
  layout->setColumnStretch(1,2);
  layout->setSpacing(5);
  topic->setLayout(layout);
  area->setWidget(topic);
  area->setWidgetResizable(true);
  return area;
}

void Expert::activateTopic(QTreeWidgetItem *item,QTreeWidgetItem *)
{
  if (item)
  {
    QString translatedName = item->text(0);
    QWidget *w = nullptr;
    QHashIterator<QString,QWidget*> i(m_topics);
    while (i.hasNext())
    {
      i.next();
      QString originalName = i.key();
      if (DoxygenWizard::translateExpertTopic(originalName) == translatedName)
      {
        w = i.value();
        break;
      }
    }
    if (w)
    {
      m_topicStack->setCurrentWidget(w);
      m_prev->setEnabled(m_topicStack->currentIndex()!=0);
      m_next->setEnabled(true);
    }
  }
}

void Expert::loadSettings(QSettings *s)
{
  QHashIterator<QString,Input*> i(m_options);
  while (i.hasNext())
  {
    i.next();
    QVariant var = s->value(SA("config/")+i.key());
    if (i.value() && var.isValid())
    {
      //printf("Loading key %s: type=%d value='%s'\n",qPrintable(i.key()),var.type(),qPrintable(var.toString()));
      i.value()->value() = var;
      i.value()->update();
    }
  }
}

void Expert::saveSettings(QSettings *s)
{
  QHashIterator<QString,Input*> i(m_options);
  while (i.hasNext())
  {
    i.next();
    //printf("Saving key %s: type=%d value='%s'\n",qPrintable(i.key()),i.value()->value().type(),qPrintable(i.value()->value().toString()));
    if (i.value())
    {
      s->setValue(SA("config/")+i.key(),i.value()->value());
    }
  }
}

void Expert::loadConfig(const QString &fileName)
{
  //printf("Expert::loadConfig(%s)\n",qPrintable(fileName));
  parseConfig(fileName,m_options);
}

void Expert::saveTopic(QTextStream &t,QDomElement &elem,TextCodecAdapter *codec,
                       bool brief,bool condensed,bool convert)
{
  if (!brief)
  {
    t << "\n";
  }
  if (!condensed)
  {
    t << "#---------------------------------------------------------------------------\n";
    t << "# " << elem.attribute(SA("docs")) << "\n";
    t << "#---------------------------------------------------------------------------\n";
  }
  // write options...
  QDomElement childElem = elem.firstChildElement();
  while (!childElem.isNull())
  {
    QString setting = childElem.attribute(SA("setting"));
    QString type = childElem.attribute(SA("type"));
    QString name = childElem.attribute(SA("id"));
    if (setting.isEmpty() || IS_SUPPORTED(setting.toLatin1()))
    {
      QHash<QString,Input*>::const_iterator i = m_options.find(name);
      if (i!=m_options.end())
      {
        Input *option = i.value();
        if (option->kind()!=Input::Obsolete)
        {
          if (option && !brief)
          {
            t << "\n";
            t << convertToComment(option->templateDocs());
            t << "\n";
          }
          bool toPrint = true;
          if (option && condensed) toPrint = !option->isDefault();
          if (toPrint)
          {
            t << name.leftJustified(MAX_OPTION_LENGTH) << "=";
            if (option && !option->isEmpty())
            {
              t << " ";
              option->writeValue(t,codec,convert);
            }
            t << "\n";
          }
        }
      }
    }
    childElem = childElem.nextSiblingElement();
  }
}

bool Expert::writeConfig(QTextStream &t,bool brief, bool condensed, bool convert)
{
  // write global header
  t << "# Doxyfile " << getDoxygenVersion().c_str() << "\n\n";
  if (!brief && !condensed)
  {
    t << convertToComment(m_header);
  }

  Input *option = m_options[QString::fromLatin1("DOXYFILE_ENCODING")];
  TextCodecAdapter codec(option->value().toString().toLatin1());
  QDomElement childElem = m_rootElement.firstChildElement();
  while (!childElem.isNull())
  {
    if (childElem.tagName()==SA("group"))
    {
      saveTopic(t,childElem,&codec,brief,condensed,convert);
    }
    childElem = childElem.nextSiblingElement();
  }
  return true;
}

QByteArray Expert::saveInnerState () const
{
  return m_splitter->saveState();
}

bool Expert::restoreInnerState ( const QByteArray & state )
{
  return m_splitter->restoreState(state);
}

void Expert::showHelp(Input *option)
{
  if (!m_inShowHelp)
  {
    m_inShowHelp = true;
    QString docs = option->docs();
    m_helper->setText(
        QString::fromLatin1("<qt><b>")+option->id()+
        QString::fromLatin1("</b><br>")+
        QString::fromLatin1("<br/>")+
        docs.replace(QChar::fromLatin1('\n'),QChar::fromLatin1(' '))+
        QString::fromLatin1("</qt>")
        );
    m_inShowHelp = false;
  }
}

void Expert::nextTopic()
{
  if (m_topicStack->currentIndex()+1==m_topicStack->count()) // last topic
  {
    done();
  }
  else
  {
    m_topicStack->setCurrentIndex(m_topicStack->currentIndex()+1);
    m_next->setEnabled(m_topicStack->count()!=m_topicStack->currentIndex()+1);
    m_prev->setEnabled(m_topicStack->currentIndex()!=0);
    m_treeWidget->setCurrentItem(m_treeWidget->invisibleRootItem()->child(m_topicStack->currentIndex()));
  }
}

void Expert::prevTopic()
{
  m_topicStack->setCurrentIndex(m_topicStack->currentIndex()-1);
  m_next->setEnabled(m_topicStack->count()!=m_topicStack->currentIndex()+1);
  m_prev->setEnabled(m_topicStack->currentIndex()!=0);
  m_treeWidget->setCurrentItem(m_treeWidget->invisibleRootItem()->child(m_topicStack->currentIndex()));
}

void Expert::resetToDefaults()
{
  //printf("Expert::makeDefaults()\n");
  QHashIterator<QString,Input*> i(m_options);
  while (i.hasNext())
  {
    i.next();
    if (i.value())
    {
      i.value()->reset();
    }
  }
}

static bool stringVariantToBool(const QVariant &v)
{
  QString s = v.toString().toLower();
  return s==QString::fromLatin1("yes") || s==QString::fromLatin1("true") || s==QString::fromLatin1("1");
}

static bool getBoolOption(
    const QHash<QString,Input*>&model,const QString &name)
{
  Input *option = model[name];
  Q_ASSERT(option!=nullptr);
  return stringVariantToBool(option->value());
}

static QString getStringOption(
    const QHash<QString,Input*>&model,const QString &name)
{
  Input *option = model[name];
  Q_ASSERT(option!=nullptr);
  return option->value().toString();
}


bool Expert::htmlOutputPresent(const QString &workingDir) const
{
  bool generateHtml = getBoolOption(m_options,QString::fromLatin1("GENERATE_HTML"));
  if (!generateHtml || workingDir.isEmpty()) return false;
  QString indexFile = getHtmlOutputIndex(workingDir);
  QFileInfo fi(indexFile);
  return fi.exists() && fi.isFile();
}

QString Expert::getHtmlOutputIndex(const QString &workingDir) const
{
  QString outputDir = getStringOption(m_options,QString::fromLatin1("OUTPUT_DIRECTORY"));
  QString htmlOutputDir = getStringOption(m_options,QString::fromLatin1("HTML_OUTPUT"));
  //printf("outputDir=%s\n",qPrintable(outputDir));
  //printf("htmlOutputDir=%s\n",qPrintable(htmlOutputDir));
  QString indexFile = workingDir;
  if (QFileInfo(outputDir).isAbsolute()) // override
  {
    indexFile = outputDir;
  }
  else // append
  {
    indexFile += QString::fromLatin1("/")+outputDir;
  }
  if (QFileInfo(htmlOutputDir).isAbsolute()) // override
  {
    indexFile = htmlOutputDir;
  }
  else // append
  {
    indexFile += QString::fromLatin1("/")+htmlOutputDir;
  }
  indexFile+=QString::fromLatin1("/index.html");
  return indexFile;
}

bool Expert::pdfOutputPresent(const QString &workingDir) const
{
  bool generateLatex = getBoolOption(m_options,QString::fromLatin1("GENERATE_LATEX"));
  bool pdfLatex = getBoolOption(m_options,QString::fromLatin1("USE_PDFLATEX"));
  if (!generateLatex || !pdfLatex) return false;
  QString latexOutput = getStringOption(m_options,QString::fromLatin1("LATEX_OUTPUT"));
  QString indexFile;
  if (QFileInfo(latexOutput).isAbsolute())
  {
    indexFile = latexOutput+QString::fromLatin1("/refman.pdf");
  }
  else
  {
    indexFile = workingDir+QString::fromLatin1("/")+
                latexOutput+QString::fromLatin1("/refman.pdf");
  }
  QFileInfo fi(indexFile);
  return fi.exists() && fi.isFile();
}

void Expert::refresh()
{
  m_treeWidget->setCurrentItem(m_treeWidget->invisibleRootItem()->child(0));
}

void Expert::retranslateUi()
{
  m_treeWidget->setHeaderLabels(QStringList() << DoxygenWizard::msgTopicsHeader());
  m_prev->setText(DoxygenWizard::msgPreviousButton());
  m_next->setText(DoxygenWizard::msgNextButton());
  
  loadConfigXml();
  
  m_topics.clear();
  while (m_topicStack->count() > 0)
  {
    QWidget *w = m_topicStack->widget(0);
    m_topicStack->removeWidget(w);
  }
  m_options.clear();
  m_optionElements.clear();
  
  m_treeWidget->clear();
  
  createTopics(m_rootElement);
  
  m_treeWidget->setCurrentItem(m_treeWidget->invisibleRootItem()->child(0));
}

