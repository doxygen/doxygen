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

