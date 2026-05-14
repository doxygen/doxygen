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
#include "doxywizard.h"

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
#include <QDebug>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QStringConverter>
#endif

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

static void translateEnumDescription(QDomElement &valueElem,const QDomElement &translationRoot)
{
  QDomElement trDocsVal = translationRoot.firstChildElement();
  while (!trDocsVal.isNull())
  {
    if (trDocsVal.tagName()==SA("value") && trDocsVal.attribute(SA("name"))==valueElem.attribute(SA("name")) && trDocsVal.hasAttribute(SA("desc")))
    {
      QString trDesc = trDocsVal.attribute(SA("desc"));
      valueElem.setAttribute(SA("desc"),trDesc);
    }
    trDocsVal = trDocsVal.nextSiblingElement();
  }
}

static void translateOption(QDomElement &configRoot,const QDomElement &translationRoot)
{
  QDomElement docsVal   = configRoot.firstChildElement();
  QDomElement trDocsVal = translationRoot.firstChildElement();
  bool first=true;
  if (!docsVal.isNull()   && docsVal.tagName()==SA("docs") &&
      !trDocsVal.isNull() && trDocsVal.tagName()==SA("docs"))
  {
    //qDebug() << "id=" << configRoot.attribute(SA("id")) << "trId=" << translationRoot.attribute(SA("id"));
    docsVal.parentNode().replaceChild(trDocsVal,docsVal);
  }
  docsVal = configRoot.firstChildElement().nextSiblingElement();
  // disable options docs (already part of the translation)
  while (!docsVal.isNull())
  {
    //qDebug() << "tagName" << docsVal.tagName();
    if (docsVal.tagName()==SA("docs") && docsVal.attribute(SA("doxywizard"))!=SA("0"))
    {
      docsVal.setAttribute(SA("doxywizard"),SA("0"));
    }
    else if (docsVal.tagName()==SA("value") && docsVal.hasAttribute(SA("desc")))
    {
      //qDebug() << "attribute" << docsVal.attribute(SA("desc"));
      translateEnumDescription(docsVal,translationRoot);
    }
    docsVal = docsVal.nextSiblingElement();
  }
}

static void translateTopics(QDomElement &configRoot,const QDomElement &translationRoot)
{
  struct GroupInfo
  {
    QDomElement elem;
    QMap<QString, QDomElement> options;
  };
  QMap<QString,GroupInfo> groupMap;

  // collect the elements in a nested map
  QDomElement groupElem = configRoot.firstChildElement();
  while (!groupElem.isNull())
  {
    // store group itself
    if (groupElem.tagName()==SA("group"))
    {
      QString name = groupElem.attribute(SA("name"));
      groupMap[name].elem = groupElem;
      // store options inside the group
      QDomElement optionElem = groupElem.firstChildElement();
      while (!optionElem.isNull())
      {
        if (optionElem.tagName()==SA("option"))
        {
          QString id = optionElem.attribute(SA("id"));
          groupMap[name].options[id] = optionElem;
        }
        optionElem = optionElem.nextSiblingElement();
      }
    }
    groupElem = groupElem.nextSiblingElement();
  }

  groupElem = translationRoot.firstChildElement();
  while (!groupElem.isNull())
  {
    if (groupElem.tagName()==SA("group"))
    {
      // translate the group docs
      QString name   = groupElem.attribute(SA("name"));
      if (groupMap.contains(name))
      {
        QString trDocs = groupElem.attribute(SA("docs"));
        QString trName = groupElem.attribute(SA("trname"));
        groupMap[name].elem.setAttribute(SA("docs"),trDocs);
        groupMap[name].elem.setAttribute(SA("trname"),trName);
      }
      else
      {
        qDebug() << "group does not have translation" << name;
      }
      // translate the option docs
      QDomElement optionElem = groupElem.firstChildElement();
      while (!optionElem.isNull())
      {
        if (optionElem.tagName()==SA("option"))
        {
          QString id = optionElem.attribute(SA("id"));
          if (groupMap[name].options.contains(id))
          {
            translateOption(groupMap[name].options[id],optionElem);
          }
          else
          {
            qDebug() << "group " << name << "does not have option" << id;
          }
        }
        optionElem = optionElem.nextSiblingElement();
      }
    }
    groupElem = groupElem.nextSiblingElement();
  }
}

Expert::Expert()
{
  m_treeWidget = new QTreeWidget;
  m_treeWidget->setColumnCount(2);
  m_topicStack = new QStackedWidget;
  m_inShowHelp = false;

  QFile file(SA(":/config.xml"));
  QString err = tr("Error");
  int errLine=0,errCol=0;
  QDomDocument configXml;
  if (!file.open(QIODevice::ReadOnly) || !configXml.setContent(&file,false,&err,&errLine,&errCol))
  {
    QString msg = tr("Error parsing internal config.xml at line %1 column %2.\n%3").
                arg(errLine).arg(errCol).arg(err);
    QMessageBox::warning(this, tr("Error"), msg);
    exit(1);
  }
  m_rootElement = configXml.documentElement();
  if (!DoxygenWizard::langCode.isEmpty())
  {
    QFile trFile(SA(":/i18n/config_%1.xml").arg(DoxygenWizard::langCode));
    if (trFile.open(QIODevice::ReadOnly))
    {
      QDomDocument trConfigXml;
      if (!trConfigXml.setContent(&trFile,false,&err,&errLine,&errCol))
      {
        QString msg = tr("Error parsing internal config_%1.xml at line %2 column %3.\n%4").
                    arg(DoxygenWizard::langCode).arg(errLine).arg(errCol).arg(err);
        QMessageBox::warning(this, tr("Error"), msg);
      }
      // overrule english text with translations
      translateTopics(m_rootElement,trConfigXml.documentElement());
    }
    else
    {
      qDebug() << SA("config_%1.xml not found").arg(DoxygenWizard::langCode);
    }
  }

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
  m_treeWidget->resizeColumnToContents(0);

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
      QString docs = childElem.attribute(SA("docs")).replace(SA("_"),SA(" "));
      QString setting = childElem.attribute(SA("setting"));
      if (setting.isEmpty() || IS_SUPPORTED(setting.toLatin1()))
      {
        QString translatedName = childElem.attribute(SA("trname")).replace(SA("_"),SA(" "));
        if (translatedName.isEmpty()) translatedName = name;
        items.append(new QTreeWidgetItem((QTreeWidget*)nullptr,QStringList() << translatedName << docs));
        QWidget *widget = createTopicWidget(childElem);
        m_topics[translatedName] = widget;
        m_topicStack->addWidget(widget);
      }
    }
    childElem = childElem.nextSiblingElement();
  }
  m_treeWidget->setHeaderLabels(QStringList() << DoxygenWizard::msgTopicsHeader() << tr("Description"));
  m_treeWidget->insertTopLevelItems(0,items);
  connect(m_treeWidget,
          SIGNAL(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)),
          this,
          SLOT(activateTopic(QTreeWidgetItem *,QTreeWidgetItem *)));
}

QString Expert::getDocsForNode(const QDomElement &child) const
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
    if (!docs.endsWith(SA("<br/>"))) docs += SA("<br/>");
    docs += SA("<br/>");
    docs += tr("Possible values are:");
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
          docs+=SA(" ") + tr("and") + SA(" ");
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
      docs+=SA(" ")+tr("The default value is: <code>%1</code>.").arg(defval);
    }
    docs+= SA("<br/>");
  }
  else if (type==SA("int"))
  {
    if (!docs.endsWith(SA("<br/>"))) docs += SA("<br/>");
    docs+=SA("<br/>");
    QString minval = child.attribute(SA("minval"));
    QString maxval = child.attribute(SA("maxval"));
    QString defval = child.attribute(SA("defval"));
    docs+=tr("Minimum value: %1, maximum value: %2, default value: %3.").arg(minval).arg(maxval).arg(defval);
    docs+= SA("<br/>");
  }
  else if (type==SA("bool"))
  {
    if (!docs.endsWith(SA("<br/>"))) docs += SA("<br/>");
    docs+=SA("<br/>");
    if (child.hasAttribute(SA("altdefval")))
    {
      docs+=SA(" ")+tr("The default value is: system dependent.");
    }
    else
    {
      QString defval = child.attribute(SA("defval"));
      QString valStr = (defval==SA("1")?SA("YES"):SA("NO"));
      docs+=SA(" ")+tr("The default value is: <code>%1</code>.").arg(valStr);
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
        docs += tr("Possible values are:");
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
                docs += SA(" ") + tr("and") + SA(" ");
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
        if (!docs.endsWith(SA("<br/>"))) docs += SA("<br/>");
        docs+=SA("<br/>");
        docs += SA(" ")+tr("The default directory is: <code>%1</code>.").arg(defval);
        docs += SA("<br/>");
      }
    }
    else if (child.attribute(SA("format")) == SA("file"))
    {
      QString abspath = child.attribute(SA("abspath"));
      if (defval != SA(""))
      {
        if (!docs.endsWith(SA("<br/>"))) docs += SA("<br/>");
        docs+=SA("<br/>");
        if (abspath != SA("1"))
        {
          docs += SA(" ")+tr("The default file is: <code>%1</code>.").arg(defval);
        }
        else
        {
          docs += SA(" ")+tr("The default file (with absolute path) is: <code>%1</code>.").arg(defval);
        }
        docs += SA("<br/>");
      }
      else
      {
        if (abspath == SA("1"))
        {
          if (!docs.endsWith(SA("<br/>"))) docs += SA("<br/>");
          docs+=SA("<br/>");
          docs += SA(" ")+tr("The file has to be specified with full path.");
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
        docs+=SA("<br/>");
        if (abspath != SA("1"))
        {
          docs += SA(" ")+tr("The default image is: <code>%1</code>.").arg(defval);
        }
        else
        {
          docs += SA(" ")+tr("The default image (with absolute path) is: <code>%1</code>.").arg(defval);
        }
        docs += SA("<br/>");
      }
      else
      {
        if (abspath == SA("1"))
        {
          if (!docs.endsWith(SA("<br/>"))) docs += SA("<br/>");
          docs+=SA("<br/>");
          docs += SA(" ")+tr("The image has to be specified with full path.");
          docs += SA("<br/>");
        }
      }
    }
    else // if (child.attribute(SA("format")) == SA("string"))
    {
      if (defval != SA(""))
      {
        if (!docs.endsWith(SA("<br/>"))) docs += SA("<br/>");
        docs+=SA("<br/>");
        docs += SA(" ")+tr("The default value is: <code>%1</code>.").arg(defval);
        docs += SA("<br/>");
      }
    }
  }

  if (child.hasAttribute(SA("depends")))
  {
    QString dependsOn = child.attribute(SA("depends"));
    if (!docs.endsWith(SA("<br/>"))) docs += SA("<br/>");
    docs+=SA("<br/>");
    docs += SA(" ")+tr("This tag requires that the tag %1 is set to <code>YES</code>.").arg(SA("\\ref cfg_")+dependsOn.toLower()+SA(" \"")+dependsOn.toUpper()+SA("\""));
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
  regexp.setPattern(SA("\\\\ref[ ]+[^ ]+[ ]+\"\\\\\\\\ref\""));
  docs.replace(regexp,SA("\\\\REF"));
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
  docs.replace(SA("\\\\REF"),SA("\\\\ref"));
  // \b word -> <b>word<\b>
  regexp.setPattern(SA("\\\\b[ ]+([^ ]+) "));
  docs.replace(regexp,SA("<b>\\1</b> "));
  // \e word -> <em>word<\em>
  regexp.setPattern(SA("\\\\e[ ]+([^ ]+) "));
  docs.replace(regexp,SA("<em>\\1</em> "));
  // \note -> <br>Note:
  // @note -> <br>Note:
  docs.replace(SA("\\note "),SA("<br>Note: "));
  docs.replace(SA("@note "),SA("<br>Note: "));
  // \#include -> #include
  // \#undef -> #undef
  docs.replace(SA("\\#include"),SA("#include"));
  docs.replace(SA("\\#undef"),SA("#undef"));
  // \verbatim -> <pre>
  // \endverbatim -> </pre>
  docs.replace(SA("\\verbatim"),SA("<pre>"));
  docs.replace(SA("\\endverbatim"),SA("</pre>"));
  // \sa -> <br>See also:
  // \par -> <br>
  docs.replace(SA("\\sa "),SA("<br>See also: "));
  docs.replace(SA("\\par "),SA("<br>"));
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
  // -# -> <br>-
  // " - " -> <br>-
  docs.replace(SA("-#"),SA("<br>-"));
  docs.replace(SA("\\# "),SA("# "));
  docs.replace(SA(" - "),SA("<br>-"));

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
         printf("option '%s' has depends on '%s' that is not valid\n",
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

  Input *option = m_options[SA("DOXYFILE_ENCODING")];
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
    m_helper->setText(SA("<qt><b>")+option->id()+
        SA("</b><br/><br/>")+
        option->docs().
        replace(QChar::fromLatin1('\n'),QChar::fromLatin1(' '))+
        SA("</qt>")
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
  return s==SA("yes") || s==SA("true") || s==SA("1");
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
  bool generateHtml = getBoolOption(m_options,SA("GENERATE_HTML"));
  if (!generateHtml || workingDir.isEmpty()) return false;
  QString indexFile = getHtmlOutputIndex(workingDir);
  QFileInfo fi(indexFile);
  return fi.exists() && fi.isFile();
}

QString Expert::getHtmlOutputIndex(const QString &workingDir) const
{
  QString outputDir = getStringOption(m_options,SA("OUTPUT_DIRECTORY"));
  QString htmlOutputDir = getStringOption(m_options,SA("HTML_OUTPUT"));
  //printf("outputDir=%s\n",qPrintable(outputDir));
  //printf("htmlOutputDir=%s\n",qPrintable(htmlOutputDir));
  QString indexFile = workingDir;
  if (QFileInfo(outputDir).isAbsolute()) // override
  {
    indexFile = outputDir;
  }
  else // append
  {
    indexFile += SA("/")+outputDir;
  }
  if (QFileInfo(htmlOutputDir).isAbsolute()) // override
  {
    indexFile = htmlOutputDir;
  }
  else // append
  {
    indexFile += SA("/")+htmlOutputDir;
  }
  indexFile+=SA("/index.html");
  return indexFile;
}

bool Expert::pdfOutputPresent(const QString &workingDir) const
{
  bool generateLatex = getBoolOption(m_options,SA("GENERATE_LATEX"));
  bool pdfLatex = getBoolOption(m_options,SA("USE_PDFLATEX"));
  if (!generateLatex || !pdfLatex) return false;
  QString latexOutput = getStringOption(m_options,SA("LATEX_OUTPUT"));
  QString indexFile;
  if (QFileInfo(latexOutput).isAbsolute())
  {
    indexFile = latexOutput+SA("/refman.pdf");
  }
  else
  {
    indexFile = workingDir+SA("/")+latexOutput+SA("/refman.pdf");
  }
  QFileInfo fi(indexFile);
  return fi.exists() && fi.isFile();
}

void Expert::refresh()
{
  m_treeWidget->setCurrentItem(m_treeWidget->invisibleRootItem()->child(0));
}

bool compareFunction (QString a, QString b) {return a<b;}

void Expert::dump()
{
  QFile fileOut(SA("dump_%1.txt").arg(DoxygenWizard::langCode));
  if (fileOut.open(QFile::WriteOnly|QFile::Text))
  {
    QTextStream out(&fileOut);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    out.setCodec(QTextCodec::codecForName("UTF-8"));
#else
    out.setEncoding(QStringConverter::Utf8);
#endif
    QHashIterator<QString, Input*> i(m_options);
    std::vector<QString> v;
    while (i.hasNext())
    {
      i.next();
      if (!i.value())
      {
        // no value  present, like for not compiled in settings (CLANG_...)
      }
      else if (i.value()->docs().isEmpty())
      {
        // no documentation present, like for Obsolete items
      }
      else
      {
        v.push_back(i.key());
      }
    }
    std::sort(v.begin(),v.end(),compareFunction);
    for (const auto & n : v)
    {
      out << n << ": " <<  m_options[n]->docs() << "\n";
      out << SA("=================================\n");
    }

    fileOut.flush();
    fileOut.close();
  }
}

