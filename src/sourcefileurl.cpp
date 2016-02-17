/******************************************************************************
*
*
*
* Copyright(C) 2016 by Andreas Grimme
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

#include "sourcefileurl.h"
#include "OutputList.h"
#include "config.h"
#include "docparser.h"
#include "filedef.h"
#include <qdir.h>
#include <qregexp.h>

QString urlEncode(const QString& value)
{
  QString escaped;

  for (int i = 0; i < value.length(); ++i)
  {
    // Keep alphanumeric and other accepted characters intact
    QChar c = value.constref(i);
    if (c.isLetterOrNumber() || c == '-' || c == '_' || c == '.' || c == '~')
    {
      escaped.append(c);
      continue;
    }

    escaped.append('%');
    escaped.append(QString().setNum((int)c, 16));
  }

  return escaped;
}

bool writeFileLink(OutputList& ol, const FileDef &fd)
{
  FileLink link;
  if (!resolveFileLink(QDir::current().absPath(), QString(fd.absFilePath()), link))
  {
    return false;
  }

  DocRoot* root = new DocRoot(false, false);
  DocPara* para = new DocPara(root); root->children().append(para);
  DocHRef* href = new DocHRef(para, HtmlAttribList(), link.url.utf8(), QCString()); para->children().append(href);
  DocWord* word = new DocWord(href, link.name.utf8()); href->children().append(word);
  ol.writeDoc(root, NULL, NULL);
  delete root;
  return true;
}

bool writeGeneratedFromFile(OutputList& ol, const QString& path)
{
  FileLink link;
  if (!resolveFileLink(QDir::current().absPath(), path, link))
  {
    return false;
  }

  DocRoot* root = new DocRoot(false, false);
  DocPara* para = new DocPara(root); root->children().append(para);
  para->children().append(new DocWord(para, "Generated from "));
  DocHRef* href = new DocHRef(para, HtmlAttribList(), link.url.utf8(), QCString()); para->children().append(href);
  DocWord* word = new DocWord(href, link.name.utf8()); href->children().append(word);
  para->children().append(new DocWord(para, "."));
  ol.writeDoc(root, NULL, NULL);
  delete root;
  return true;
}


/*! Translates a relative file system path found by doxygen into in URL.
The the config values VERSION_CONTROL_URL and VERSION_CONTROL_ROOT are set,
the function will
- make path relative to VERSION_CONTROL_ROOT
- construct a VERSION_CONTROL_URL and replace %s with the relative path
*/
QString getFileUrl(const QString& path)
{
  QString url = ConfigValues::instance().VERSION_CONTROL_URL;
  if (!url.isEmpty())
  {
    QString absVersionControlRoot = QDir(ConfigValues::instance().VERSION_CONTROL_ROOT).absPath();
    QString absPath = QFileInfo(path).absFilePath();
    if (absPath.lower().startsWith(absVersionControlRoot.lower()))
    {
      QString relativePath = absPath.mid(absVersionControlRoot.length());
      QString urlParameter = urlEncode(relativePath);
      // printf("url encoded path: %s\n", urlParameter.utf8().data());
      url.sprintf(url.utf8().data(), urlParameter.utf8().data());
      return url;
    }
  }

  return QCString();
}

QString getFileName(const QString& path)
{
  QCString url = ConfigValues::instance().VERSION_CONTROL_URL;
  if (!url.isEmpty())
  {
    QString absVersionControlRoot = QDir(ConfigValues::instance().VERSION_CONTROL_ROOT).absPath();
    QString absPath = QFileInfo(path).absFilePath();
    QString relativePath = absPath.mid(absVersionControlRoot.length());
    if (!relativePath.isEmpty())
    {
      return QString("$") + relativePath;
    }
  }

  return QCString();
}

bool resolveFileLink(const QString& basePath, const QString& relativeFileName, FileLink& outLink)
{
  QString canonicalPath = QFileInfo(QFileInfo(basePath).dir().filePath(relativeFileName, true)).absFilePath();
  // printf("canonical: %s\n", canonicalPath.utf8().data());
  if (QFileInfo(canonicalPath).exists())
  {
    outLink.url = getFileUrl(canonicalPath);
    outLink.name = getFileName(canonicalPath);
    return true;
  }
  else
  {
    return false;
  }
}

