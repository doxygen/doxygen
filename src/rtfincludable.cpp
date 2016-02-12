// Copyright(C) 2016 by Andreas Grimme

#include "rtfincludable.h"
#include "OutputList.h"
#include "config.h"
#include "docparser.h"
#include <qdir.h>

bool isIncludableRtf()
{
	return Config_getBool(INCLUDABLE_RTF);
}

bool isTableOfContentEntriesEnabled()
{
	return !isIncludableRtf();
}

bool writeFileLink(OutputList& ol, const FileDef &fd)
{
	return false;
}

QCString getFileUrl(const QString& path)
{
	QCString url = ConfigValues::instance().VERSION_CONTROL_URL;
	if (!url.isEmpty())
	{
		QString absVersionControlRoot = QDir(ConfigValues::instance().VERSION_CONTROL_ROOT).absPath();
		QString absPath = QFileInfo(path).absFilePath();
		QString relativePath = absPath.mid(absVersionControlRoot.length());
		if (!relativePath.isEmpty())
		{
			printf("%s\n", path.data());
			printf("%s\n", relativePath.utf8().data());
			url.sprintf(url, relativePath.utf8().data());
			return url;
		}
	}

	return QCString();
}

QCString getFileName(const QString& path)
{
	QCString url = ConfigValues::instance().VERSION_CONTROL_URL;
	if (!url.isEmpty())
	{
		QString absVersionControlRoot = QDir(ConfigValues::instance().VERSION_CONTROL_ROOT).absPath();
		QString absPath = QFileInfo(path).absFilePath();
		QString relativePath = absPath.mid(absVersionControlRoot.length());
		if (!relativePath.isEmpty())
		{
			return relativePath.utf8();
		}
	}

	return QCString();
}

bool writeFileLink(OutputList& ol, const QCString &path)
{
	QCString url = ConfigValues::instance().VERSION_CONTROL_URL;
	if (url.isEmpty())
	{
		return false;
	}

	
	QDir versionControlRoot = QDir(ConfigValues::instance().VERSION_CONTROL_ROOT);
	versionControlRoot.convertToAbs();
	QFileInfo file(path);
	QString rel = file.absFilePath().mid(versionControlRoot.absFilePath(false).length());
	url.sprintf(url, rel.utf8().data());

	DocRoot* root = new DocRoot(false, false);
	DocPara* para = new DocPara(root); root->children().append(para);
	para->children().append(new DocWord(para, "Generated from "));
	DocHRef* href = new DocHRef(para, HtmlAttribList(), url, QCString()); para->children().append(href);
	DocWord* word = new DocWord(href, rel.utf8().data()); href->children().append(word);
	ol.writeDoc(root, NULL, NULL);
	delete root;
	return true;
}

bool resolveFileLink(const QCString& currentFileName, const QCString& link, FileLink& outLink)
{
	QFileInfo file(currentFileName);
	QFileInfo f(QDir(file.dirPath(true) + QString("/") + QString(link)).canonicalPath());
	if (f.exists())
	{
		outLink.url = getFileUrl(f.absFilePath());
		outLink.name = getFileName(f.absFilePath());
		return true;
	}
	else
	{
		return false;
	}
}

