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
#include "version.h"
#include "configdoc.h"
#include "settings.h"

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

  QFile file(SA(":/config.xml"));
  QString err;
  int errLine,errCol;
  QDomDocument configXml;
  if (file.open(QIODevice::ReadOnly))
  {
    if (!configXml.setContent(&file,false,&err,&errLine,&errCol))
    {
      QString msg = tr("Error parsing internal config.xml at line %1 column %2.\n%3").
                  arg(errLine).arg(errCol).arg(err);
      QMessageBox::warning(this, tr("Error"), msg);
      exit(1);
    }
  }
  m_rootElement = configXml.documentElement();

  createTopics(m_rootElement);
  m_helper = new QTextBrowser;
  m_helper->setReadOnly(true);
  m_helper->setOpenExternalLinks(true);
  m_splitter = new QSplitter(Qt::Vertical);
  m_splitter->addWidget(m_treeWidget);
  m_splitter->addWidget(m_helper);

  QWidget *rightSide = new QWidget;
  QGridLayout *grid = new QGridLayout(rightSide);
  m_prev = new QPushButton(tr("Previous"));
  m_prev->setEnabled(false);
  m_next = new QPushButton(tr("Next"));
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
        items.append(new QTreeWidgetItem((QTreeWidget*)nullptr,QStringList(name)));
        QWidget *widget = createTopicWidget(childElem);
        m_topics[name] = widget;
        m_topicStack->addWidget(widget);
      }
    }
    childElem = childElem.nextSiblingElement();
  }
  m_treeWidget->setHeaderLabels(QStringList() << SA("Topics"));
  m_treeWidget->insertTopLevelItems(0,items);
  connect(m_treeWidget,
          SIGNAL(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)),
          this,
          SLOT(activateTopic(QTreeWidgetItem *,QTreeWidgetItem *)));
}

static QString getDocsForNode(const QDomElement &child)
{
  QString type = child.attribute(SA("type"));
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
    docs += SA("<br/>");
    docs += SA("Possible values are: ");
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
          docs+=SA(" and ");
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
      docs+=SA(" The default value is: <code>")+
            child.attribute(SA("defval"))+
            SA("</code>.");
    }
    docs+= SA("<br/>");
  }
  else if (type==SA("int"))
  {
    docs+=SA("<br/>");
    docs+=SA("Minimum value: ")+child.attribute(SA("minval"))+SA(", ");
    docs+=SA("maximum value: ")+child.attribute(SA("maxval"))+SA(", ");
    docs+=SA("default value: ")+child.attribute(SA("defval"))+SA(".");
    docs+= SA("<br/>");
  }
  else if (type==SA("bool"))
  {
    docs+=SA("<br/>");
    if (child.hasAttribute(SA("altdefval")))
    {
      docs+=SA(" The default value is: system dependent.");
    }
    else
    {
      QString defval = child.attribute(SA("defval"));
      docs+=SA(" The default value is: <code>")+
            (defval==SA("1")?SA("YES"):SA("NO"))+
            SA("</code>.");
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
                docs += SA(" and ");
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
      // docs+= SA("<br/>");
    }
  }
  else if (type==SA("string"))
  {
    QString defval = child.attribute(SA("defval"));
    if (child.attribute(SA("format")) == SA("dir"))
    {
      if (defval != SA(""))
      {
        docs+=SA("<br/>");
        docs += SA(" The default directory is: <code>") + defval + SA("</code>.");
        docs += SA("<br/>");
      }
    }
    else if (child.attribute(SA("format")) == SA("file"))
    {
      QString abspath = child.attribute(SA("abspath"));
      if (defval != SA(""))
      {
        docs+=SA("<br/>");
        if (abspath != SA("1"))
        {
          docs += SA(" The default file is: <code>") + defval + SA("</code>.");
        }
        else
        {
          docs += SA(" The default file (with absolute path) is: <code>") + defval + SA("</code>.");
        }
        docs += SA("<br/>");
      }
      else
      {
        if (abspath == SA("1"))
        {
          docs+=SA("<br/>");
          docs += SA(" The file has to be specified with full path.");
          docs += SA("<br/>");
        }
      }
    }
    else if (child.attribute(SA("format")) == SA("image"))
    {
      QString abspath = child.attribute(SA("abspath"));
      if (defval != SA(""))
      {
        docs+=SA("<br/>");
        if (abspath != SA("1"))
        {
          docs += SA(" The default image is: <code>") + defval + SA("</code>.");
        }
        else
        {
          docs += SA(" The default image (with absolute path) is: <code>") + defval + SA("</code>.");
        }
        docs += SA("<br/>");
      }
      else
      {
        if (abspath == SA("1"))
        {
          docs+=SA("<br/>");
          docs += SA(" The image has to be specified with full path.");
          docs += SA("<br/>");
        }
      }
    }
    else // if (child.attribute(SA("format")) == SA("string"))
    {
      if (defval != SA(""))
      {
        docs+=SA("<br/>");
        docs += SA(" The default value is: <code>") + defval + SA("</code>.");
        docs += SA("<br/>");
      }
    }
  }

  if (child.hasAttribute(SA("depends")))
  {
    QString dependsOn = child.attribute(SA("depends"));
    docs+=SA("<br/>");
    docs+=  SA(" This tag requires that the tag \\ref cfg_");
    docs+=  dependsOn.toLower();
    docs+=  SA(" \"");
    docs+=  dependsOn.toUpper();
    docs+=  SA("\" is set to <code>YES</code>.");
  }

  // Remove / replace doxygen markup strings
  // the regular expressions are hard to read so the intention will be given
  // Note: see also configgen.py in the src directory for other doxygen parts
  QRegularExpression regexp;
  // remove \n at end and replace by a space
  regexp.setPattern(SA("\\n$"));
  docs.replace(regexp,SA(" "));
  // remove <br> at end
  regexp.setPattern(SA("<br> *$"));
  docs.replace(regexp,SA(" "));
  // \c word -> <code>word</code>; word ends with ')', ',', '.' or ' '
  regexp.setPattern(SA("\\\\c[ ]+([^ \\)]+)\\)"));
  docs.replace(regexp,SA("<code>\\1</code>)"));

  regexp.setPattern(SA("\\\\c[ ]+([^ ,]+),"));
  docs.replace(regexp,SA("<code>\\1</code>,"));

  regexp.setPattern(SA("\\\\c[ ]+([^ \\.]+)\\."));
  docs.replace(regexp,SA("<code>\\1</code>."));

  regexp.setPattern(SA("\\\\c[ ]+([^ ]+) "));
  docs.replace(regexp,SA("<code>\\1</code> "));
  // `word` -> <code>word</code>
  docs.replace(SA("``"),SA(""));
  regexp.setPattern(SA("`([^`]+)`"));
  docs.replace(regexp,SA("<code>\\1</code>"));
  // \ref key "desc" -> <code>desc</code>
  regexp.setPattern(SA("\\\\ref[ ]+[^ ]+[ ]+\"([^\"]+)\""));
  docs.replace(regexp,SA("<code>\\1</code> "));
  //\ref specials
  // \ref <key> -> description
  regexp.setPattern(SA("\\\\ref[ ]+doxygen_usage"));
  docs.replace(regexp,SA("\"Doxygen usage\""));
  regexp.setPattern(SA("\\\\ref[ ]+extsearch"));
  docs.replace(regexp,SA("\"External Indexing and Searching\""));
  regexp.setPattern(SA("\\\\ref[ ]+external"));
  docs.replace(regexp,SA("\"Linking to external documentation\""));
  regexp.setPattern(SA("\\\\ref[ ]+formulas"));
  docs.replace(regexp,SA("\"Including formulas\""));
  // fallback for not handled
  docs.replace(SA("\\\\ref"),SA(""));
  // \b word -> <b>word<\b>
  regexp.setPattern(SA("\\\\b[ ]+([^ ]+) "));
  docs.replace(regexp,SA("<b>\\1</b> "));
  // \e word -> <em>word<\em>
  regexp.setPattern(SA("\\\\e[ ]+([^ ]+) "));
  docs.replace(regexp,SA("<em>\\1</em> "));
  // \note -> <br>Note:
  // @note -> <br>Note:
  docs.replace(SA("\\note"),SA("<br>Note:"));
  docs.replace(SA("@note"),SA("<br>Note:"));
  // \#include -> #include
  // \#undef -> #undef
  docs.replace(SA("\\#include"),SA("#include"));
  docs.replace(SA("\\#undef"),SA("#undef"));
  // -# -> <br>-
  // " - " -> <br>-
  docs.replace(SA("-#"),SA("<br>-"));
  docs.replace(SA(" - "),SA("<br>-"));
  // \verbatim -> <pre>
  // \endverbatim -> </pre>
  docs.replace(SA("\\verbatim"),SA("<pre>"));
  docs.replace(SA("\\endverbatim"),SA("</pre>"));
  // \sa -> <br>See also:
  // \par -> <br>
  docs.replace(SA("\\sa"),SA("<br>See also:"));
  docs.replace(SA("\\par"),SA("<br>"));
  // 2xbackslash -> backslash
  // \@ -> @
  docs.replace(SA("\\\\"),SA("\\"));
  docs.replace(SA("\\@"),SA("@"));
  // \& -> &
  // \$ -> $
  docs.replace(SA("\\&"),SA("&"));
  docs.replace(SA("\\$"),SA("$"));
  // \< -> &lt;
  // \> -> &gt;
  docs.replace(SA("\\<"),SA("&lt;"));
  docs.replace(SA("\\>"),SA("&gt;"));
  regexp.setPattern(SA(" (http:[^ \\)]*)([ \\)])"));
  docs.replace(regexp,SA(" <a href=\"\\1\">\\1</a>\\2"));
  regexp.setPattern(SA(" (https:[^ \\)]*)([ \\)])"));
  docs.replace(regexp,SA(" <a href=\"\\1\">\\1</a>\\2"));
  // LaTeX name as formula -> LaTeX
  regexp.setPattern(SA("\\\\f\\$\\\\mbox\\{\\\\LaTeX\\}\\\\f\\$"));
  docs.replace(regexp,SA("LaTeX"));
  // Other formula's (now just 2) so explicitly mentioned.
  regexp.setPattern(SA("\\\\f\\$2\\^\\{\\(16\\+\\\\mbox\\{LOOKUP\\\\_CACHE\\\\_SIZE\\}\\)\\}\\\\f\\$"));
  docs.replace(regexp,SA("2^(16+LOOKUP_CACHE_SIZE)"));
  regexp.setPattern(SA("\\\\f\\$2\\^\\{16\\} = 65536\\\\f\\$"));
  docs.replace(regexp,SA("2^16=65536"));

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
        m_options.insert(
            child.attribute(SA("id")),
            boolOption
            );
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
        m_options.insert(
            child.attribute(SA("id")),
            stringOption
            );
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

        m_options.insert(child.attribute(SA("id")),enumList);
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
        m_options.insert(
            child.attribute(SA("id")),
            intOption
            );
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
        m_options.insert(
            child.attribute(SA("id")),
            listOption
            );
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
    QWidget *w = m_topics[item->text(0)];
    m_topicStack->setCurrentWidget(w);
    m_prev->setEnabled(m_topicStack->currentIndex()!=0);
    m_next->setEnabled(true);
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
    m_helper->setText(
        QString::fromLatin1("<qt><b>")+option->id()+
        QString::fromLatin1("</b><br>")+
        QString::fromLatin1("<br/>")+
        option->docs().
        replace(QChar::fromLatin1('\n'),QChar::fromLatin1(' '))+
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

