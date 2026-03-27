/******************************************************************************
 *
 * Copyright (C) 1997-2020 by Dimitri van Heesch.
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
 * The code is this file is largely based on a contribution from
 * Harm van der Heijden <H.v.d.Heijden@phys.tue.nl>
 * Please send thanks to him and bug reports to me :-)
 */

#ifndef HTMLHELP_H
#define HTMLHELP_H

#include <memory>
#include "qcstring.h"
#include "construct.h"
#include "indexlist.h"

class Definition;
class MemberDef;

/** A class that generated the HTML Help specific files.
 *
 *  These files can be used with the Microsoft HTML Help workshop
 *  to generate compressed HTML files (.chm).
 */
class HtmlHelp : public IndexIntf
{
    /*! used in imageNumber param of HTMLHelp::addContentsItem() function
        to specify document icon in tree view.
        Writes \<param name="ImageNumber" value="xx"\> in .HHC file. */
    enum ImageNumber {
      BOOK_CLOSED=1,    BOOK_OPEN,
      BOOK_CLOSED_NEW,  BOOK_OPEN_NEW,
      FOLDER_CLOSED,    FOLDER_OPEN,
      FOLDER_CLOSED_NEW,FOLDER_OPEN_NEW,
      QUERY,            QUERY_NEW,
      TEXT,             TEXT_NEW,
      WEB_DOC,          WEB_DOC_NEW,
      WEB_LINK,         WEB_LINK_NEW,
      INFO,             INFO_NEW,
      LINK,             LINK_NEW,
      BOOKLET,          BOOKLET_NEW,
      EMAIL,            EMAIL_NEW,
      EMAIL2,           EMAIL2_NEW,
      IMAGE,            IMAGE_NEW,
      AUDIO,            AUDIO_NEW,
      MUSIC,            MUSIC_NEW,
      VIDEO,            VIDEO_NEW,
      INDEX,            INDEX_NEW,
      IDEA,             IDEA_NEW,
      NOTE,             NOTE_NEW,
      TOOL,             TOOL_NEW
    };
  public:
    HtmlHelp();
    ~HtmlHelp();
    NON_COPYABLE(HtmlHelp)

    void initialize();
    void finalize();
    void incContentsDepth();
    void decContentsDepth();
    void addContentsItem(bool isDir,
                         const QCString &name,
                         const QCString &ref,
                         const QCString &file,
                         const QCString &anchor,
                         bool separateIndex,
                         bool addToNavIndex,
                         const Definition *def,
                         const QCString &nameAsHtml);
    void addIndexItem(const Definition *context,const MemberDef *md,
                      const QCString &sectionAnchor, const QCString &title);
    void addIndexFile(const QCString &name);
    void addImageFile(const QCString &);
    void addStyleSheetFile(const QCString &);

    static inline const QCString hhcFileName = "index.hhc";
    static inline const QCString hhkFileName = "index.hhk";
    static inline const QCString hhpFileName = "index.hhp";
  private:
    class Private;
    std::unique_ptr<Private> p;
    QCString recode(const QCString &s);

};

#endif /* HTMLHELP_H */

