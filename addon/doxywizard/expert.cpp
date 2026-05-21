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

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QFrame>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QScrollArea>
#include <QScrollBar>
#include <QSettings>
#include <QSplitter>
#include <QTextStream>
#include <QTimer>
#include <QTreeWidget>
#include <QVBoxLayout>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QStringConverter>
#endif

#define SA(x) QString::fromLatin1(x)
#define SMALL_FONT_START SA("<span style=\"font-size: 12px\">")
#define SMALL_FONT_END   SA("</span")


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
  // --- Search bar (top) ---
  m_searchBox = new QLineEdit;
  m_searchBox->setPlaceholderText(tr("Search settings..."));
  m_searchBox->setClearButtonEnabled(true);

  // --- Left: tree widget for group navigation ---
  m_treeWidget = new QTreeWidget;
  m_treeWidget->setColumnCount(1);
  m_treeWidget->setHeaderLabel(tr("Topics"));
  m_treeWidget->setRootIsDecorated(true);
  m_treeWidget->header()->setSectionResizeMode(QHeaderView::Fixed);

  // --- Right: scroll area containing all group sections ---
  m_rightContainer = new QWidget;
  m_rightLayout = new QVBoxLayout(m_rightContainer);
  m_rightLayout->setContentsMargins(0, 0, 0, 0);
  m_rightLayout->setSpacing(0);
  m_rightLayout->setAlignment(Qt::AlignTop);

  m_scrollArea = new QScrollArea;
  m_scrollArea->setWidgetResizable(true);
  m_scrollArea->setWidget(m_rightContainer);
  m_scrollArea->setFrameShape(QFrame::NoFrame);

  // --- Navigation bar (Prev / Next) below the scroll area ---
  m_prevButton = new QPushButton(DoxygenWizard::msgPreviousButton());
  m_nextButton = new QPushButton(DoxygenWizard::msgNextButton());
  m_prevButton->setEnabled(false);
  m_nextButton->setEnabled(false);

  m_navBar = new QWidget;
  QHBoxLayout *navLayout = new QHBoxLayout(m_navBar);
  navLayout->setContentsMargins(4, 4, 4, 4);
  navLayout->addWidget(m_prevButton);
  navLayout->addStretch();
  navLayout->addWidget(m_nextButton);

  // Right panel = scroll area + nav bar
  QWidget *rightPanel = new QWidget;
  QVBoxLayout *rightPanelLayout = new QVBoxLayout(rightPanel);
  rightPanelLayout->setContentsMargins(0, 0, 0, 0);
  rightPanelLayout->setSpacing(0);
  rightPanelLayout->addWidget(m_scrollArea);
  rightPanelLayout->addWidget(m_navBar);

  // --- Horizontal splitter (left: tree, right: options) ---
  m_splitter = new QSplitter(Qt::Horizontal);
  m_splitter->addWidget(m_treeWidget);
  m_splitter->addWidget(rightPanel);
  m_splitter->setStretchFactor(0, 0);
  m_splitter->setStretchFactor(1, 1);
  m_splitter->setSizes({200, 600});

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(4, 4, 4, 4);
  mainLayout->setSpacing(4);
  mainLayout->addWidget(m_searchBox);
  mainLayout->addWidget(m_splitter);

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
  if (!DoxygenWizard::langCode.isEmpty() && DoxygenWizard::langCode!=SA("en"))
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

  // --- Build all group sections and option cards ---
  // createGroups() activates group 0 (lazy), but the Wizard needs all m_options
  // populated before it is constructed, so we create all cards eagerly here.
  createGroups(m_rootElement);
  ensureAllGroupsCreated();

  // --- Wire up signals ---
  m_filterTimer = new QTimer(this);
  m_filterTimer->setSingleShot(true);
  m_filterTimer->setInterval(200);
  connect(m_searchBox,   SIGNAL(textChanged(const QString&)),
          m_filterTimer, SLOT(start()));
  connect(m_filterTimer, &QTimer::timeout, this,
          [this](){ filterChanged(m_searchBox->text()); });
  connect(m_treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
          this,         SLOT(groupSelected(QTreeWidgetItem*,QTreeWidgetItem*)));
  connect(m_prevButton, SIGNAL(clicked()), this, SLOT(prevGroup()));
  connect(m_nextButton, SIGNAL(clicked()), this, SLOT(nextGroup()));

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

void Expert::createGroups(const QDomElement &rootElem)
{
  // --- Create group structures (header + tree items) — NO option cards yet ---
  QDomElement groupElem = rootElem.firstChildElement();
  while (!groupElem.isNull())
  {
    if (groupElem.tagName() == SA("group"))
    {
      QString setting = groupElem.attribute(SA("setting"));
      if (setting.isEmpty() || IS_SUPPORTED(setting.toLatin1()))
      {
        GroupEntry group;
        group.name = groupElem.attribute(SA("name"));
        group.docs = groupElem.attribute(SA("docs"));
        QString translatedName = groupElem.attribute(SA("trname")).replace(SA("_"), SA(" "));
        if (translatedName.isEmpty()) translatedName = group.name.replace(SA("_"), SA(" "));
        group.translatedName = translatedName;
        group.domElement     = groupElem;
        group.cardsCreated   = false;

        // Group section container (header + separator only; option cards added lazily)
        group.section = new QWidget;
        group.section->setObjectName(SA("grpSection_") + group.name);
        QVBoxLayout *sectionLayout = new QVBoxLayout(group.section);
        sectionLayout->setContentsMargins(0, 12, 0, 0);
        sectionLayout->setSpacing(0);

        // Group header label
        group.header = new QLabel(translatedName + SA(" - ") + group.docs);
        QFont headerFont = group.header->font();
        headerFont.setPointSize(headerFont.pointSize() + 2);
        headerFont.setBold(true);
        group.header->setFont(headerFont);
        group.header->setContentsMargins(8, 4, 8, 4);
        sectionLayout->addWidget(group.header);

        // Thick separator line beneath group header
        QFrame *headerSep = new QFrame;
        headerSep->setFrameShape(QFrame::HLine);
        headerSep->setFrameShadow(QFrame::Sunken);
        sectionLayout->addWidget(headerSep);

        // Tree widget item for this group
        group.treeItem = new QTreeWidgetItem(m_treeWidget, QStringList() << translatedName);

        group.section->hide();
        m_rightLayout->addWidget(group.section);
        m_groups.append(group);
      }
    }
    groupElem = groupElem.nextSiblingElement();
  }
  m_rightLayout->addStretch(1);

  // Activate the first group (creates its cards, shows it, selects tree item)
  if (!m_groups.isEmpty())
  {
    activateGroup(0);
  }
}

void Expert::createOptionCard(GroupEntry &group, const QDomElement &child)
{
  QString setting = child.attribute(SA("setting"));
  if (!setting.isEmpty() && !IS_SUPPORTED(setting.toLatin1())) return;

  QString type = child.attribute(SA("type"));
  QString id   = child.attribute(SA("id"));
  QString docs = getDocsForNode(child);

  // Each option gets its own control layout
  QWidget     *ctrlWidget = new QWidget;
  QGridLayout *ctrlLayout = new QGridLayout(ctrlWidget);
  ctrlLayout->setContentsMargins(8, 4, 8, 0);
  ctrlLayout->setSpacing(5);
  ctrlLayout->setColumnStretch(1, 2);

  int row = 0;
  Input *input = nullptr;

  if (type == SA("bool"))
  {
    InputBool *boolOption = new InputBool(ctrlLayout, row,
                                         id,
                                         child.attribute(SA("defval")) == SA("1"),
                                         docs);
    input = boolOption;
    connect(boolOption, SIGNAL(changed()), SIGNAL(changed()));
  }
  else if (type == SA("string"))
  {
    InputString::StringMode mode;
    QString format = child.attribute(SA("format"));
    if      (format == SA("dir"))     mode = InputString::StringDir;
    else if (format == SA("file"))    mode = InputString::StringFile;
    else if (format == SA("filedir")) mode = InputString::StringFileDir;
    else if (format == SA("image"))   mode = InputString::StringImage;
    else                              mode = InputString::StringFree;
    InputString *stringOption = new InputString(ctrlLayout, row, id,
                                                child.attribute(SA("defval")),
                                                mode,
                                                docs,
                                                child.attribute(SA("abspath")));
    input = stringOption;
    connect(stringOption, SIGNAL(changed()), SIGNAL(changed()));
  }
  else if (type == SA("enum"))
  {
    InputString *enumList = new InputString(ctrlLayout, row, id,
                                            child.attribute(SA("defval")),
                                            InputString::StringFixed,
                                            docs);
    QDomElement enumVal = child.firstChildElement();
    while (!enumVal.isNull())
    {
      if (enumVal.tagName() == SA("value"))
      {
        enumList->addValue(enumVal.attribute(SA("name")));
      }
      enumVal = enumVal.nextSiblingElement();
    }
    enumList->setDefault();
    input = enumList;
    connect(enumList, SIGNAL(changed()), SIGNAL(changed()));
  }
  else if (type == SA("int"))
  {
    InputInt *intOption = new InputInt(ctrlLayout, row, id,
                   child.attribute(SA("defval")).toInt(),
                   child.attribute(SA("minval")).toInt(),
                   child.attribute(SA("maxval")).toInt(),
                   docs);
    input = intOption;
    connect(intOption, SIGNAL(changed()), SIGNAL(changed()));
  }
  else if (type == SA("list"))
  {
    InputStrList::ListMode mode;
    QString format = child.attribute(SA("format"));
    if      (format == SA("dir"))     mode = InputStrList::ListDir;
    else if (format == SA("file"))    mode = InputStrList::ListFile;
    else if (format == SA("filedir")) mode = InputStrList::ListFileDir;
    else                              mode = InputStrList::ListString;
    QStringList sl;
    QDomElement listVal = child.firstChildElement();
    while (!listVal.isNull())
    {
      if (listVal.tagName() == SA("value"))
      {
        sl.append(listVal.attribute(SA("name")));
      }
      listVal = listVal.nextSiblingElement();
    }
    InputStrList *listOption =
      new InputStrList(ctrlLayout, row,
                       id, sl, mode, docs);
    input = listOption;
    connect(listOption, SIGNAL(changed()), SIGNAL(changed()));
  }
  else if (type == SA("obsolete"))
  {
    Input::Kind orgKind;
    QString orgKindStr = child.attribute(SA("orgtype"));
    if      (orgKindStr == SA("int"))                    orgKind = Input::Int;
    else if (orgKindStr == SA("bool"))                   orgKind = Input::Bool;
    else if (orgKindStr == SA("string") ||
             orgKindStr == SA("enum"))                   orgKind = Input::String;
    else if (orgKindStr == SA("strlist"))                orgKind = Input::StrList;
    else                                                 orgKind = Input::Obsolete;
    input = new InputObsolete(id, orgKind);
    m_options.insert(id, input);
    // Obsolete options are not shown
    return;
  }
  else
  {
    printf("Unsupported type %s\n", qPrintable(type));
  }

  if (input)
    m_options.insert(id, input);

  // Inline documentation label (spans all 3 columns, shown below the controls)
  QLabel *docsLabel = nullptr;
  if (!docs.isEmpty())
  {
    docsLabel = new QLabel;
    docsLabel->setTextFormat(Qt::RichText);
    docsLabel->setWordWrap(true);
    docsLabel->setOpenExternalLinks(true);
    docsLabel->setContentsMargins(0, 2, 0, 4);
    // Use a smaller, muted style for the description text
    QString display = SMALL_FONT_START + SA("<b>") + id + SA("</b>");
    if (!docs.isEmpty())
    {
      display += SA(" &mdash; ") + docs;
    }
    display += SMALL_FONT_END;
    docsLabel->setText(display);
    // Dim the label using PlaceholderText, which is calibrated for readable
    // secondary text in both light and dark modes (available since Qt 5.12).
    QColor textColor = docsLabel->palette().color(QPalette::Text);
    QColor bgColor = docsLabel->palette().color(QPalette::Window);

    double mixAmount = 0.5;
    int r = textColor.red()   * mixAmount + bgColor.red()   * (1.0-mixAmount);
    int g = textColor.green() * mixAmount + bgColor.green() * (1.0-mixAmount);
    int b = textColor.blue()  * mixAmount + bgColor.blue()  * (1.0-mixAmount);

    QPalette pal = docsLabel->palette();
    pal.setColor(QPalette::WindowText, QColor(r,g,b));
    docsLabel->setPalette(pal);
    ctrlLayout->addWidget(docsLabel, row, 0, 1, 3);
    row++;
  }
  ctrlLayout->setRowStretch(row, 0);

  // Thin separator at the bottom of each option card
  QFrame *sep = new QFrame;
  sep->setFrameShape(QFrame::HLine);
  sep->setFrameShadow(QFrame::Plain);

  // Wrap the control widget + separator into a single container so we can
  // show/hide both together during filtering
  QWidget *container = new QWidget;
  QVBoxLayout *containerLayout = new QVBoxLayout(container);
  containerLayout->setContentsMargins(0, 0, 0, 0);
  containerLayout->setSpacing(0);
  containerLayout->addWidget(ctrlWidget);
  containerLayout->addWidget(sep);

  // Add to the group section
  QVBoxLayout *sectionLayout = static_cast<QVBoxLayout *>(group.section->layout());
  sectionLayout->addWidget(container);

  // Child tree item for this option (hidden by default – only visible when filtering)
  QTreeWidgetItem *optTreeItem = new QTreeWidgetItem(group.treeItem, QStringList() << id);
  optTreeItem->setHidden(true);

  // Record the entry for filtering
  OptionEntry entry;
  entry.groupName = group.name;
  entry.id        = id;
  entry.docs      = docs;
  entry.card      = container;
  entry.docsLabel = docsLabel;
  entry.sep       = sep;
  entry.treeItem  = optTreeItem;
  entry.input     = input;
  group.options.append(entry);
}

// ---------------------------------------------------------------------------
// Lazy-creation helpers
// ---------------------------------------------------------------------------

// Wire any dependencies from the root DOM that can be resolved with the
// currently available m_options entries. Uses m_wiredDeps to avoid double
// wiring when called multiple times.
void Expert::wireDependencies()
{
  QDomElement groupElem = m_rootElement.firstChildElement();
  while (!groupElem.isNull())
  {
    if (groupElem.tagName() == SA("group"))
    {
      QDomElement optElem = groupElem.firstChildElement();
      while (!optElem.isNull())
      {
        QString setting   = optElem.attribute(SA("setting"));
        QString dependsOn = optElem.attribute(SA("depends"));
        QString id        = optElem.attribute(SA("id"));
        if (!dependsOn.isEmpty() &&
            (setting.isEmpty() || IS_SUPPORTED(setting.toLatin1())))
        {
          Input *parentOption = m_options.value(dependsOn);
          Input *thisOption   = m_options.value(id);
          if (parentOption && thisOption)
          {
            parentOption->addDependency(thisOption);
          }
        }
        optElem = optElem.nextSiblingElement();
      }
    }
    groupElem = groupElem.nextSiblingElement();
  }
}

// Create option cards for 'group' if they haven't been created yet.
void Expert::ensureGroupCardsCreated(GroupEntry &group)
{
  if (group.cardsCreated) return;
  group.cardsCreated = true;

  QDomElement optElem = group.domElement.firstChildElement();
  while (!optElem.isNull())
  {
    if (optElem.tagName() == SA("option"))
    {
      createOptionCard(group, optElem);
    }
    optElem = optElem.nextSiblingElement();
  }

  // Bottom spacing for the section (mirrors what was in the old createGroups loop)

  // Wire what can be wired now (intra-group + any cross-group where both sides exist)
  wireDependencies();

  // Apply dependency states for this group's options
  for (const OptionEntry &opt : group.options)
  {
    if (opt.input)
    {
      opt.input->updateDependencies();
    }
  }
}

void Expert::setDocumentationVisibility(bool hidden)
{
  for (GroupEntry &group : m_groups)
  {
    for (const OptionEntry &opt : group.options)
    {
      if (opt.sep)
      {
        opt.sep->setHidden(hidden);
      }
      if (opt.docsLabel)
      {
        opt.docsLabel->setHidden(hidden);
      }
    }
  }
}

// Create cards for every group that hasn't been created yet, then do a full
// dependency update (needed when cross-group deps span newly-created groups).
void Expert::ensureAllGroupsCreated()
{
  for (GroupEntry &group : m_groups)
  {
    ensureGroupCardsCreated(group);
  }

  // Re-run update so cross-group dependencies are fully applied
  QHashIterator<QString,Input*> i(m_options);
  while (i.hasNext())
  {
    i.next();
    if (i.value()) i.value()->updateDependencies();
  }
}

// Show only group[index] in showAll mode, update tree selection and nav buttons.
void Expert::activateGroup(int index)
{
  if (index < 0 || index >= m_groups.size()) return;
  m_currentGroupIndex = index;

  GroupEntry &group = m_groups[index];
  //ensureGroupCardsCreated(group);

  // Show only the active group's section
  for (int i = 0; i < m_groups.size(); i++)
    m_groups[i].section->setVisible(i == index);

  // Scroll to top
  m_scrollArea->verticalScrollBar()->setValue(0);

  // Update tree selection without re-triggering groupSelected
  m_treeWidget->blockSignals(true);
  m_treeWidget->setCurrentItem(group.treeItem);
  m_treeWidget->blockSignals(false);

  // Enable/disable navigation buttons
  m_prevButton->setEnabled(index > 0);
  m_nextButton->setEnabled(index < m_groups.size() - 1);
}

void Expert::prevGroup()
{
  activateGroup(m_currentGroupIndex - 1);
}

void Expert::nextGroup()
{
  activateGroup(m_currentGroupIndex + 1);
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
  docs.replace(SA("(\\c \\\\)"),SA("(\\c JUST_WIZARD_BACKSLASH)"));
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
  docs.replace(SA("JUST_WIZARD_BACKSLASH"),SA("\\"));

  return docs.trimmed();
}

// Highlight all occurrences of 'filter' inside an HTML string without touching tag content.
static QString highlightInHtml(const QString &html, const QString &filter)
{
  if (filter.isEmpty()) return html;

  const QString markOpen  = SA("<span style='background-color:#ffff00;color:#000000'>");
  const QString markClose = SA("</span>");
  const int filterLen = filter.length();

  QString result;
  result.reserve(html.size() + 64);
  int i = 0;
  int len = html.length();

  while (i < len)
  {
    // Pass HTML tags through unchanged
    if (html[i] == QLatin1Char('<'))
    {
      int end = html.indexOf(QLatin1Char('>'), i);
      if (end == -1) end = len - 1;
      result += html.mid(i, end - i + 1);
      i = end + 1;
      continue;
    }
    // In text content: search for next match using indexOf
    int matchPos = html.indexOf(filter, i, Qt::CaseInsensitive);
    if (matchPos == -1)
    {
      result += html.mid(i);
      break;
    }
    // Copy text up to the match, respecting embedded tags
    while (i < matchPos)
    {
      if (html[i] == QLatin1Char('<'))
      {
        int end = html.indexOf(QLatin1Char('>'), i);
        if (end == -1) end = len - 1;
        if (end >= matchPos)
        {
          // tag straddles the match position; emit tag and re-search
          result += html.mid(i, end - i + 1);
          i = end + 1;
          matchPos = html.indexOf(filter, i, Qt::CaseInsensitive);
          if (matchPos == -1) { result += html.mid(i); i = len; }
          continue;
        }
        result += html.mid(i, end - i + 1);
        i = end + 1;
      }
      else
      {
        result += html[i++];
      }
    }
    if (i >= len) break;
    result += markOpen + html.mid(matchPos, filterLen) + markClose;
    i = matchPos + filterLen;
  }
  return result;
}

void Expert::filterChanged(const QString &text)
{
  QString filter = text.trimmed().toLower();
  bool showAll = filter.length()<=2;

  m_rightContainer->setUpdatesEnabled(false);
  m_treeWidget->setUpdatesEnabled(false);

  if (showAll)
  {
    // Show all mode

    // Restore single-group view and show the nav bar
    m_navBar->setVisible(true);
    // Restore each created group's option cards and collapse tree children
    for (GroupEntry &group : m_groups)
    {
      for (OptionEntry &opt : group.options)
      {
        opt.card->setVisible(true);
        if (opt.treeItem)
        {
          opt.treeItem->setHidden(true);
        }
        if (opt.docsLabel && opt.labelHighlighted)
        {
          QString display = SMALL_FONT_START + SA("<b>") + opt.id + SA("</b>");
          if (!opt.docs.isEmpty())
          {
            display += SA(" &mdash; ") + opt.docs;
          }
          display += SMALL_FONT_END;
          opt.docsLabel->setText(display);
          opt.input->setText(opt.id);
          opt.labelHighlighted = false;
        }
      }
      if (group.treeItem)
      {
        group.treeItem->setHidden(false);
        group.treeItem->setExpanded(false);
      }
    }
    m_rightContainer->setUpdatesEnabled(true);
    m_treeWidget->setUpdatesEnabled(true);
    // Re-activate the current group (shows its section, hides the rest)
    activateGroup(m_currentGroupIndex >= 0 ? m_currentGroupIndex : 0);
  }
  else
  {
    // Filter mode
    m_navBar->setVisible(false);

    for (GroupEntry &group : m_groups)
    {
      bool anyVisible = false;
      for (OptionEntry &opt : group.options)
      {
        bool matchesId   = opt.id.contains(filter, Qt::CaseInsensitive);
        bool matchesDocs = opt.docs.contains(filter, Qt::CaseInsensitive);
        bool visible     = matchesId || matchesDocs;
        opt.card->setVisible(visible);

        if (opt.treeItem)
        {
          opt.treeItem->setHidden(!visible);
        }

        if (opt.docsLabel)
        {
          QString hiDocs = matchesDocs ? highlightInHtml(opt.docs, filter) : opt.docs;
          QString hiId   = matchesId   ? highlightInHtml(opt.id,   filter) : opt.id;
          opt.input->setText(hiId);
          QString display = SMALL_FONT_START + SA("<b>") + hiId + SA("</b>");
          if (!opt.docs.isEmpty())
          {
            display += SA(" &mdash; ") + hiDocs;
          }
          display += SMALL_FONT_END;
          opt.docsLabel->setText(display);
          opt.labelHighlighted = true;
        }

        if (visible) anyVisible = true;
      }
      group.section->setVisible(anyVisible);
      if (group.treeItem)
      {
        group.treeItem->setHidden(!anyVisible);
        group.treeItem->setExpanded(anyVisible);
      }
    }
    m_rightContainer->setUpdatesEnabled(true);
    m_treeWidget->setUpdatesEnabled(true);
  }
}

void Expert::groupSelected(QTreeWidgetItem *item, QTreeWidgetItem *)
{
  if (!item) return;

  bool filterActive = m_searchBox->text().trimmed().length() > 2;

  if (!filterActive)
  {
    // showAll mode: clicking a group item activates it
    for (int i = 0; i < m_groups.size(); i++)
    {
      if (m_groups[i].treeItem == item)
      {
        activateGroup(i);
        return;
      }
    }
  }
  else
  {
    // Filter mode: scroll the right panel to the clicked group or option
    m_rightContainer->updateGeometry();
    for (const GroupEntry &group : m_groups)
    {
      if (group.treeItem == item)
      {
        int y = group.section->mapTo(m_rightContainer, QPoint(0, 0)).y();
        m_scrollArea->verticalScrollBar()->setValue(y);
        return;
      }
      for (const OptionEntry &opt : group.options)
      {
        if (opt.treeItem == item)
        {
          int y = opt.card->mapTo(m_rightContainer, QPoint(0, 0)).y();
          m_scrollArea->verticalScrollBar()->setValue(y);
          return;
        }
      }
    }
  }
}


void Expert::loadSettings(QSettings *s)
{
  //ensureAllGroupsCreated();
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
  //ensureAllGroupsCreated();
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
  //ensureAllGroupsCreated();
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
  //ensureAllGroupsCreated();
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

void Expert::resetToDefaults()
{
  //ensureAllGroupsCreated();
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
  m_searchBox->clear();
  activateGroup(0);
}

bool compareFunction (QString a, QString b) {return a<b;}

void Expert::dump()
{
  //ensureAllGroupsCreated();
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

