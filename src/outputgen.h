/******************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1997-1999 by Dimitri van Heesch.
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

#ifndef OUTPUTGEN_H
#define OUTPUTGEN_H

#include "qtbc.h"
#include <qtextstream.h>
#include <qbuffer.h>
#include <qfile.h>
#include "index.h"

class ClassDiagram;

class OutputGenerator
{
  public:
    enum OutputType { Html, Latex, Man };

    OutputGenerator();
    virtual ~OutputGenerator();

    virtual OutputGenerator *copy() = 0;
    //virtual OutputGenerator *clone() = 0;
    virtual void append(const OutputGenerator *) = 0;
    virtual void enable() = 0;
    virtual void disable() = 0;
    virtual void enableIf(OutputType o) = 0;
    virtual void disableIf(OutputType o) = 0;
    virtual void disableIfNot(OutputType o) = 0;
    virtual bool isEnabled(OutputType o) = 0;
    virtual OutputGenerator *get(OutputType o) = 0;

    virtual void startFile(const char *name,const char *title,bool ext) = 0;
    virtual void writeFooter(int,bool) = 0;
    virtual void endFile() = 0;
    //virtual void writeIndex() = 0;
    virtual void startIndexSection(IndexSections) = 0;
    virtual void endIndexSection(IndexSections) = 0;
    virtual void startProjectNumber() = 0;
    virtual void endProjectNumber() = 0;
    virtual void writeStyleInfo(int part) = 0;
    virtual void startTitleHead(const char *) = 0;
    virtual void endTitleHead(const char *fileName,const char *name) = 0;
    virtual void startTitle() = 0;
    virtual void endTitle()   = 0;
    virtual void newParagraph()   = 0;
    virtual void writeString(const char *text) = 0;
    virtual void startIndexList() = 0;
    virtual void endIndexList()   = 0;
    virtual void startItemList()  = 0;
    virtual void endItemList()    = 0;
    virtual void startEnumList()  = 0;
    virtual void endEnumList()    = 0;
    virtual void startBold()      = 0;
    virtual void endBold()        = 0;
    virtual void startAlphabeticalIndexList() = 0;
    virtual void endAlphabeticalIndexList() = 0;
    virtual void writeIndexHeading(const char *s) = 0;
    virtual void writeIndexItem(const char *ref,const char *file,
                                const char *text) = 0;
    virtual void docify(const char *s) = 0;
    virtual void codify(const char *s) = 0;
    virtual void writeObjectLink(const char *ref,const char *file,
                                 const char *anchor, const char *text) = 0;
    virtual void writeCodeLink(const char *ref,const char *file,
                               const char *anchor,const char *text) = 0;
    virtual void startTextLink(const char *file,const char *anchor) = 0;
    virtual void endTextLink() = 0;
    virtual void writeHtmlLink(const char *url,const char *text) = 0;
    virtual void startTypewriter() = 0;
    virtual void endTypewriter() = 0;
    virtual void startGroupHeader() = 0;
    virtual void endGroupHeader() = 0;
    virtual void writeListItem() = 0;

    virtual void startMemberSections() = 0;
    virtual void endMemberSections() = 0;
    virtual void startMemberHeader() = 0;
    virtual void endMemberHeader() = 0;
    virtual void startMemberSubtitle() = 0;
    virtual void endMemberSubtitle() = 0;
    virtual void startMemberList()  = 0;
    virtual void endMemberList()    = 0;
    virtual void startMemberItem(bool,int) = 0;
    virtual void endMemberItem(bool,const char *,const char *,bool) = 0;
    virtual void memberGroupSpacing(bool) = 0;
    virtual void memberGroupSeparator() = 0;
    virtual void insertMemberAlign() = 0;

    virtual void writeRuler() = 0;
    virtual void writeAnchor(const char *name) = 0;
    virtual void startCodeFragment() = 0;
    virtual void endCodeFragment() = 0;
    virtual void startCodeLine() = 0;
    virtual void endCodeLine() = 0;
    virtual void writeBoldString(const char *text) = 0;
    virtual void startEmphasis() = 0;
    virtual void endEmphasis() = 0;
    virtual void writeChar(char c) = 0;
    virtual void startMemberDoc(const char *,const char *,
                                const char *,const char *) = 0;
    virtual void endMemberDoc() = 0;
    virtual void startDoxyAnchor(const char *fileName,const char *clName,
                                 const char *anchor,const char *name) = 0;
    virtual void endDoxyAnchor() = 0;
    virtual void startCodeAnchor(const char *label) = 0;
    virtual void endCodeAnchor() = 0;
    virtual void writeLatexSpacing() = 0;
    //virtual void writeLatexLabel(const char *clName,const char *anchor) = 0;
    virtual void writeStartAnnoItem(const char *type,const char *file,
                                    const char *path,const char *name) = 0;
    virtual void writeEndAnnoItem(const char *name) = 0;
    virtual void startDescription() = 0;
    virtual void endDescription() = 0;
    virtual void startDescItem() = 0;
    virtual void endDescItem() = 0;
    virtual void startSubsection() = 0;
    virtual void endSubsection() = 0;
    virtual void startSubsubsection() = 0;
    virtual void endSubsubsection() = 0;
    virtual void startCenter() = 0;
    virtual void endCenter() = 0;
    virtual void startSmall() = 0;
    virtual void endSmall() = 0;
    virtual void startSubscript() = 0;
    virtual void endSubscript() = 0;
    virtual void startSuperscript() = 0;
    virtual void endSuperscript() = 0;
    virtual void startTable(int cols) = 0;
    virtual void endTable() = 0;
    virtual void nextTableRow() = 0;
    virtual void endTableRow() = 0;
    virtual void nextTableColumn() = 0;
    virtual void endTableColumn() = 0;
    virtual void writeQuote() = 0;
    virtual void writeCopyright() = 0;
    virtual void writeUmlaut(char) = 0;
    virtual void writeAcute(char) = 0;
    virtual void writeGrave(char) = 0;
    virtual void writeCirc(char) = 0;
    virtual void writeTilde(char) = 0;
    virtual void writeRing(char) = 0;
    virtual void writeSharpS() = 0;
    virtual void startMemberDescription() = 0;
    virtual void endMemberDescription() = 0;
    virtual void startDescList() = 0;
    virtual void endDescTitle() = 0;
    virtual void writeDescItem() = 0;
    virtual void endDescList() = 0;
    virtual void writeSection(const char *,const char *,bool) = 0;
    virtual void writeSectionRef(const char *,const char *,const char *) = 0;
    virtual void writeSectionRefItem(const char *,const char *,const char *) = 0;
    virtual void writeSectionRefAnchor(const char *,const char *,const char *) = 0;
    virtual void lineBreak() = 0;
    virtual void addToIndex(const char *s1,const char *s2) = 0;
    virtual void startIndent() = 0;
    virtual void endIndent() = 0;
    virtual void writeSynopsis() = 0;
    //virtual void generateExternalIndex() = 0;
    virtual void startClassDiagram() = 0;
    virtual void endClassDiagram(ClassDiagram &,const char *,const char *) = 0;
    virtual void startColorFont(uchar r,uchar g,uchar b) = 0;
    virtual void endColorFont() = 0;
    virtual void startPageRef() = 0;
    virtual void endPageRef(const char *,const char *) = 0;
    virtual void startQuickIndexItem(const char *s,const char *l) = 0;
    virtual void endQuickIndexItem() = 0;
    virtual void writeFormula(const char *,const char *) = 0;
    virtual void writeNonBreakableSpace() = 0;
    virtual void writeImage(const char *,const char *,const char *) = 0;
    
    virtual void startDescTable() = 0;
    virtual void endDescTable() = 0;
    virtual void startDescTableTitle() = 0;
    virtual void endDescTableTitle() = 0;
    virtual void startDescTableData() = 0;
    virtual void endDescTableData() = 0;

    void clear() { b.close(); a.resize(0); b.setBuffer(a); 
                   b.open(IO_WriteOnly); t.setDevice(&b); }
    void startPlainFile(const char *name);
    void endPlainFile();
    QCString getContents() const;
    bool isEnabled() const { return active; }
  protected:
    QTextStream fs;
    QByteArray a;
    QBuffer b;
    QTextStream t;
    QFile *file;
    QCString dir;
    bool active;

  private:
    OutputGenerator(const OutputGenerator &o);
    OutputGenerator &operator=(const OutputGenerator &o);
};

#endif
