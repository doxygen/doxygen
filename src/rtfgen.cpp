/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2000 by Parker Waechter & Dimitri van Heesch.
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

#include <stdlib.h>

#include "qtbc.h"
#include <qdir.h>
#include <qstack.h>

#include "rtfgen.h"
#include "config.h"
#include "message.h"
#include "doxygen.h"
#include "util.h"
#include "diagram.h"
#include "language.h"
#include "dot.h"

const int indentLevels = 10;

struct ListItemInfo
{
  bool isEnum;
  int number;
};

static ListItemInfo listItemInfo[indentLevels];

// it is undocumented as far as I know, but 
// "."'s in a bookmarkname are converted to "_"'s
// when an RTF file is read in...
static QCString formatBmkStr(const char *name)
{
  QCString result=name;
  int i=0;
  char c;
  while ((c=result.at(i))!=0)
  {
    switch(c)
    {
      case '.': 
      case ':':  
        result.at(i)='_';
        break;
      default:
        break;
    }
    i++;
  }
  return result;
}

static QCString dateToRTFDateString()
{
  const QDateTime &d = QDateTime::currentDateTime();
  QCString result;
  result.sprintf("\\yr%d\\mo%d\\dy%d\\hr%d\\min%d\\sec%d",
      d.date().year(), d.date().month(), d.date().day(), 
      d.time().hour(),d.time().minute(),d.time().second());
  return result;
}

RTFGenerator::RTFGenerator() : OutputGenerator()
{
  dir=Config::rtfOutputDir;
  col=0;
  //insideTabbing=FALSE;
  m_listLevel = 0;
  m_bstartedBody = FALSE;
  m_omitParagraph = FALSE;
}

RTFGenerator::~RTFGenerator()
{
}

void RTFGenerator::append(const OutputGenerator *g)
{
  t << g->getContents();
  col+=((RTFGenerator *)g)->col;
  //insideTabbing=insideTabbing || ((RTFGenerator *)g)->insideTabbing;
  m_listLevel=((RTFGenerator *)g)->m_listLevel;
  m_omitParagraph=((RTFGenerator *)g)->m_omitParagraph;
  //printf("RTFGenerator::append(%s) insideTabbing=%s\n", g->getContents().data(),
  //    insideTabbing ? "TRUE" : "FALSE" );
}

OutputGenerator *RTFGenerator::copy()
{
  RTFGenerator *result = new RTFGenerator;
  //result->insideTabbing=insideTabbing;
  result->m_listLevel=m_listLevel;
  result->m_omitParagraph=m_omitParagraph;
  return result;
}

void RTFGenerator::init()
{
  QCString dir=Config::rtfOutputDir;
  QDir d(dir);
  if (!d.exists() && !d.mkdir(dir))
  {
    err("Could not create output directory %s\n",dir.data());
    exit(1);
  }
}

const char *Rtf_Style_Reset     = "\\pard\\plain ";
const char *Rtf_Style_Heading1  = "\\s1\\sb240\\sa60\\keepn\\widctlpar\\adjustright \\b\\f1\\fs36\\kerning36\\cgrid ";
const char *Rtf_Style_Heading2  = "\\s2\\sb240\\sa60\\keepn\\widctlpar\\adjustright \\b\\f1\\fs28\\kerning28\\cgrid ";
const char *Rtf_Style_Heading3  = "\\s3\\sb240\\sa60\\keepn\\widctlpar\\adjustright \\b\\f1\\cgrid ";
const char *Rtf_Style_Heading4  = "\\s4\\sb240\\sa60\\keepn\\widctlpar\\adjustright \\b\\f1\\fs20\\cgrid ";
const char *Rtf_Style_Title     = "\\s15\\qc\\sb240\\sa60\\widctlpar\\outlinelevel0\\adjustright \\b\\f1\\fs32\\kerning28\\cgrid ";
const char *Rtf_Style_SubTitle  = "\\s16\\qc\\sa60\\widctlpar\\outlinelevel1\\adjustright \\f1\\cgrid ";
const char *Rtf_Style_BodyText  = "\\s17\\sa60\\sb30\\widctlpar\\qj \\fs22\\cgrid ";
const char *Rtf_Style_DenseText = "\\s18\\widctlpar\\fs22\\cgrid ";
const char *Rtf_Style_Header    = "\\s28\\widctlpar\\tqc\\tx4320\\tqr\\tx8640\\adjustright \\fs20\\cgrid ";
const char *Rtf_Style_Footer    = "\\s29\\widctlpar\\tqc\\tx4320\\tqr\\tx8640\\qr\\adjustright \\fs20\\cgrid ";
const char *Rtf_Style_CodeExample[]  = 
{
  "\\s40\\li0\\widctlpar\\adjustright \\shading1000\\cbpat8 \\f2\\fs16\\cgrid ",
  "\\s41\\li360\\widctlpar\\adjustright \\shading1000\\cbpat8 \\f2\\fs16\\cgrid ",
  "\\s42\\li720\\widctlpar\\adjustright \\shading1000\\cbpat8 \\f2\\fs16\\cgrid ",
  "\\s43\\li1080\\widctlpar\\adjustright \\shading1000\\cbpat8 \\f2\\fs16\\cgrid ",
  "\\s44\\li1440\\widctlpar\\adjustright \\shading1000\\cbpat8 \\f2\\fs16\\cgrid ",
  "\\s45\\li1800\\widctlpar\\adjustright \\shading1000\\cbpat8 \\f2\\fs16\\cgrid ",
  "\\s46\\li2160\\widctlpar\\adjustright \\shading1000\\cbpat8 \\f2\\fs16\\cgrid ",
  "\\s47\\li2520\\widctlpar\\adjustright \\shading1000\\cbpat8 \\f2\\fs16\\cgrid ",
  "\\s48\\li2880\\widctlpar\\adjustright \\shading1000\\cbpat8 \\f2\\fs16\\cgrid ",
  "\\s49\\li3240\\widctlpar\\adjustright \\shading1000\\cbpat8 \\f2\\fs16\\cgrid "
};
const char *Rtf_Style_ListContinue[] = 
{
  "\\s50\\li0\\sa60\\sb30\\qj\\widctlpar\\qj\\adjustright \\fs20\\cgrid ",
  "\\s51\\li360\\sa60\\sb30\\qj\\widctlpar\\qj\\adjustright \\fs20\\cgrid ",
  "\\s52\\li720\\sa60\\sb30\\qj\\widctlpar\\qj\\adjustright \\fs20\\cgrid ",
  "\\s53\\li1080\\sa60\\sb30\\qj\\widctlpar\\qj\\adjustright \\fs20\\cgrid ",
  "\\s54\\li1440\\sa60\\sb30\\qj\\widctlpar\\qj\\adjustright \\fs20\\cgrid ",
  "\\s55\\li1800\\sa60\\sb30\\qj\\widctlpar\\qj\\adjustright \\fs20\\cgrid ",
  "\\s56\\li2160\\sa60\\sb30\\qj\\widctlpar\\qj\\adjustright \\fs20\\cgrid ",
  "\\s57\\li2520\\sa60\\sb30\\qj\\widctlpar\\qj\\adjustright \\fs20\\cgrid ",
  "\\s58\\li2880\\sa60\\sb30\\qj\\widctlpar\\qj\\adjustright \\fs20\\cgrid ",
  "\\s59\\li3240\\sa60\\sb30\\qj\\widctlpar\\qj\\adjustright \\fs20\\cgrid "
};
const char *Rtf_Style_DescContinue[] = 
{
  "\\s60\\li0\\widctlpar\\qj\\adjustright \\fs20\\cgrid ",
  "\\s61\\li360\\widctlpar\\qj\\adjustright \\fs20\\cgrid ",
  "\\s62\\li720\\widctlpar\\qj\\adjustright \\fs20\\cgrid ",
  "\\s63\\li1080\\widctlpar\\qj\\adjustright \\fs20\\cgrid ",
  "\\s64\\li1440\\widctlpar\\qj\\adjustright \\fs20\\cgrid ",
  "\\s65\\li1800\\widctlpar\\qj\\adjustright \\fs20\\cgrid ",
  "\\s66\\li2160\\widctlpar\\qj\\adjustright \\fs20\\cgrid ",
  "\\s67\\li2520\\widctlpar\\qj\\adjustright \\fs20\\cgrid ",
  "\\s68\\li2880\\widctlpar\\qj\\adjustright \\fs20\\cgrid ",
  "\\s69\\li3240\\widctlpar\\qj\\adjustright \\fs20\\cgrid "
};
const char *Rtf_Style_LatexTOC[] = 
{
  "\\s70\\li0\\sa30\\sb30\\widctlpar\\tqr\\tldot\\tx8640\\adjustright \\fs20\\cgrid ",
  "\\s71\\li360\\sa27\\sb27\\widctlpar\\tqr\\tldot\\tx8640\\adjustright \\fs20\\cgrid ",
  "\\s72\\li720\\sa24\\sb24\\widctlpar\\tqr\\tldot\\tx8640\\adjustright \\fs20\\cgrid ",
  "\\s73\\li1080\\sa21\\sb21\\widctlpar\\tqr\\tldot\\tx8640\\adjustright \\fs20\\cgrid ",
  "\\s74\\li1440\\sa18\\sb18\\widctlpar\\tqr\\tldot\\tx8640\\adjustright \\fs20\\cgrid ",
  "\\s75\\li1800\\sa15\\sb15\\widctlpar\\tqr\\tldot\\tx8640\\adjustright \\fs20\\cgrid ",
  "\\s76\\li2160\\sa12\\sb12\\widctlpar\\tqr\\tldot\\tx8640\\adjustright \\fs20\\cgrid ",
  "\\s77\\li2520\\sa9\\sb9\\widctlpar\\tqr\\tldot\\tx8640\\adjustright \\fs20\\cgrid ",
  "\\s78\\li2880\\sa6\\sb6\\widctlpar\\tqr\\tldot\\tx8640\\adjustright \\fs20\\cgrid ",
  "\\s79\\li3240\\sa3\\sb3\\widctlpar\\tqr\\tldot\\tx8640\\adjustright \\fs20\\cgrid "
};
const char *Rtf_Style_ListBullet[] = 
{
  "\\s80\\fi-360\\li360\\widctlpar\\jclisttab\\tx360{\\*\\pn \\pnlvlbody\\ilvl0\\ls1\\pnrnot0\\pndec }\\ls1\\adjustright \\fs20\\cgrid ",
  "\\s81\\fi-360\\li720\\widctlpar\\jclisttab\\tx720{\\*\\pn \\pnlvlbody\\ilvl0\\ls2\\pnrnot0\\pndec }\\ls2\\adjustright \\fs20\\cgrid ",
  "\\s82\\fi-360\\li1080\\widctlpar\\jclisttab\\tx1080{\\*\\pn \\pnlvlbody\\ilvl0\\ls3\\pnrnot0\\pndec }\\ls3\\adjustright \\fs20\\cgrid ",
  "\\s83\\fi-360\\li1440\\widctlpar\\jclisttab\\tx1440{\\*\\pn \\pnlvlbody\\ilvl0\\ls4\\pnrnot0\\pndec }\\ls4\\adjustright \\fs20\\cgrid ",
  "\\s84\\fi-360\\li1800\\widctlpar\\jclisttab\\tx1800{\\*\\pn \\pnlvlbody\\ilvl0\\ls5\\pnrnot0\\pndec }\\ls5\\adjustright \\fs20\\cgrid ",
  "\\s85\\fi-360\\li2160\\widctlpar\\jclisttab\\tx2160{\\*\\pn \\pnlvlbody\\ilvl0\\ls6\\pnrnot0\\pndec }\\ls6\\adjustright \\fs20\\cgrid ",
  "\\s86\\fi-360\\li2520\\widctlpar\\jclisttab\\tx2520{\\*\\pn \\pnlvlbody\\ilvl0\\ls7\\pnrnot0\\pndec }\\ls7\\adjustright \\fs20\\cgrid ",
  "\\s87\\fi-360\\li2880\\widctlpar\\jclisttab\\tx2880{\\*\\pn \\pnlvlbody\\ilvl0\\ls8\\pnrnot0\\pndec }\\ls8\\adjustright \\fs20\\cgrid ",
  "\\s88\\fi-360\\li3240\\widctlpar\\jclisttab\\tx3240{\\*\\pn \\pnlvlbody\\ilvl0\\ls9\\pnrnot0\\pndec }\\ls9\\adjustright \\fs20\\cgrid ",
  "\\s89\\fi-360\\li3600\\widctlpar\\jclisttab\\tx3600{\\*\\pn \\pnlvlbody\\ilvl0\\ls10\\pnrnot0\\pndec }\\ls10\\adjustright \\fs20\\cgrid "
};
const char *Rtf_Style_ListEnum[] = 
{
  "\\s90\\fi-360\\li360\\widctlpar\\fs20\\cgrid ",
  "\\s91\\fi-360\\li720\\widctlpar\\fs20\\cgrid ",
  "\\s92\\fi-360\\li1080\\widctlpar\\fs20\\cgrid ",
  "\\s93\\fi-360\\li1440\\widctlpar\\fs20\\cgrid ",
  "\\s94\\fi-360\\li1800\\widctlpar\\fs20\\cgrid ",
  "\\s95\\fi-360\\li2160\\widctlpar\\fs20\\cgrid ",
  "\\s96\\fi-360\\li2520\\widctlpar\\fs20\\cgrid ",
  "\\s97\\fi-360\\li2880\\widctlpar\\fs20\\cgrid ",
  "\\s98\\fi-360\\li3240\\widctlpar\\fs20\\cgrid ",
  "\\s99\\fi-360\\li3600\\widctlpar\\fs20\\cgrid "
};

void RTFGenerator::beginRTFDocument()
{
  int i;
  /* all the included RTF files should begin with the
   * same header
   */
  t <<"{\\rtf1\\ansi\\ansicpg1252\\uc1 \\deff0\\deflang1033\\deflangfe1033\n";

  t <<"{\\comment Begining font list}\n";
  t <<"{\\fonttbl ";
  t <<"{\\f0\\froman\\fcharset0\\fprq2{\\*\\panose 02020603050405020304}Times New Roman;}\n";
  t <<"{\\f1\\fswiss\\fcharset0\\fprq2{\\*\\panose 020b0604020202020204}Arial;}\n";
  t <<"{\\f2\\fmodern\\fcharset0\\fprq1{\\*\\panose 02070309020205020404}Courier New;}\n";
  t <<"{\\f3\\froman\\fcharset2\\fprq2{\\*\\panose 05050102010706020507}Symbol;}\n";
  t <<"}\n";
  t <<"{\\comment begin colors}\n";
  t <<"{\\colortbl;";
  t <<"\\red0\\green0\\blue0;";
  t <<"\\red0\\green0\\blue255;";
  t <<"\\red0\\green255\\blue255;";
  t <<"\\red0\\green255\\blue0;";
  t <<"\\red255\\green0\\blue255;";
  t <<"\\red255\\green0\\blue0;";
  t <<"\\red255\\green255\\blue0;";
  t <<"\\red255\\green255\\blue255;";
  t <<"\\red0\\green0\\blue128;";
  t <<"\\red0\\green128\\blue128;";
  t <<"\\red0\\green128\\blue0;";
  t <<"\\red128\\green0\\blue128;";
  t <<"\\red128\\green0\\blue0;";
  t <<"\\red128\\green128\\blue0;";
  t <<"\\red128\\green128\\blue128;";
  t <<"\\red192\\green192\\blue192;}" << endl;

  t <<"{\\comment Begining style list}\n";
  t <<"{\\stylesheet\n";
  t <<"{\\widctlpar\\adjustright \\fs20\\cgrid \\snext0 Normal;}\n";
  t <<"{" << Rtf_Style_Heading1 <<"\\sbasedon0 \\snext0 heading 1;}\n";
  t <<"{" << Rtf_Style_Heading2 <<"\\sbasedon0 \\snext0 heading 2;}\n";
  t <<"{" << Rtf_Style_Heading3 <<"\\sbasedon0 \\snext0 heading 3;}\n";
  t <<"{" << Rtf_Style_Heading4 <<"\\sbasedon0 \\snext0 heading 4;}\n";
  t <<"{\\*\\cs10 \\additive Default Paragraph Font;}\n";
  t <<"{" << Rtf_Style_Title << "\\sbasedon0 \\snext15 Title;}\n";
  t <<"{" << Rtf_Style_SubTitle << "\\sbasedon0 \\snext16 Subtitle;}\n";
  t <<"{" << Rtf_Style_BodyText << "\\sbasedon0 \\snext17 BodyText;}\n";
  t <<"{" << Rtf_Style_DenseText << "\\sbasedon0 \\snext18 DenseText;}\n";
  t <<"{" << Rtf_Style_Header << "\\sbasedon0 \\snext28 header;}\n";
  t <<"{" << Rtf_Style_Footer << "\\sbasedon0 \\snext29 footer;}\n";
  for (i=0;i<indentLevels;i++)
  {
    t <<"{" << Rtf_Style_CodeExample[i] <<"\\sbasedon0 \\snext4" 
      << (QMIN(i+1,indentLevels-1)) << " Code Example " << i << ";}\n";
  }
  for (i=0;i<indentLevels;i++)
  {
    t <<"{" << Rtf_Style_ListContinue[i] << "\\sbasedon0 \\snext5" 
      << (QMIN(i+1,indentLevels-1)) << " List Continue " << i << ";}\n";
  }
  for (i=0;i<indentLevels;i++)
  {
    t <<"{" << Rtf_Style_DescContinue[i] << "\\sbasedon0 \\snext6" 
      << (QMIN(i+1,indentLevels-1)) << " DescContinue " << i << ";}\n";
  }
  for (i=0;i<indentLevels;i++)
  {
    t <<"{" << Rtf_Style_LatexTOC[i] << "\\sbasedon50 \\snext7" 
      << (QMIN(i+1,indentLevels-1)) << " LatexTOC " << i << ";}\n";
  }
  for (i=0;i<indentLevels;i++)
  {
    t <<"{" << Rtf_Style_ListBullet[i] << "\\sbasedon0 \\snext8" 
      << (QMIN(i+1,indentLevels-1)) << " \\sautoupd List Bullet " << i << ";}\n";
  }
  for (i=0;i<indentLevels;i++)
  {
    t <<"{" << Rtf_Style_ListEnum[i] << "\\sbasedon0 \\snext9" 
      << (QMIN(i+1,indentLevels-1)) << " \\sautoupd List Enum " << i << ";}\n";
  }
  t <<"}" << endl;
  t <<"{\\comment begin body}" << endl;

}

void RTFGenerator::beginRTFChapter()
{
  t <<"\n{\\comment Begin Chapter}\n" << Rtf_Style_Reset;

  // if we are compact, no extra page breaks...
  if (Config::compactRTFFlag)
  {
    //		t <<"\\sect\\sectd\\sbknone\n";
    t <<"\\sect\\sbknone\n";
    RtfwriteRuler_thick();
  }
  else
    t <<"\\sect\\sbkpage\n";
  //t <<"\\sect\\sectd\\sbkpage\n";

  t << Rtf_Style_Heading1 << "\n";
}

void RTFGenerator::beginRTFSection()
{
  t <<"\n{\\comment Begin Section}\n" << Rtf_Style_Reset;

  // if we are compact, no extra page breaks...
  if (Config::compactRTFFlag)
  {
    //		t <<"\\sect\\sectd\\sbknone\n";
    t <<"\\sect\\sbknone\n";
    RtfwriteRuler_emboss();
  }
  else
    t <<"\\sect\\sbkpage\n";
  //t <<"\\sect\\sectd\\sbkpage\n";

  t << Rtf_Style_Heading2 << "\n";
}

void RTFGenerator::startFile(const char *name,const char *, bool )
{
  QCString fileName=name;

  if (fileName.right(4)!=".rtf" ) fileName+=".rtf";
  startPlainFile(fileName);
  beginRTFDocument();
}

void RTFGenerator::endFile()
{
  t << "}";

  endPlainFile();
}

void RTFGenerator::startProjectNumber()
{
  t << " ";
}

void RTFGenerator::endProjectNumber() 
{
}

void RTFGenerator::startIndexSection(IndexSections is)
{
  QCString paperName;

  m_listLevel = 0;

  switch (is)
  {
    case isTitlePageStart:
      // basic RTFstart
      // get readyfor author etc
      t << "{\\info \n";
      t << "{\\title ";
      break;
    case isTitlePageAuthor:
      t << "}\n{\\author ";
      break;
    case isMainPage:
      //Introduction
      beginRTFChapter();
      break;
    case isModuleIndex:
      //Module Index
      beginRTFChapter();
      break;
    case isNamespaceIndex:
      //Namespace Index
      beginRTFChapter();  
      break;
    case isClassHierarchyIndex:
      //Hierarchical Index
      t << "{\\comment start classhierarchy}\n";
      beginRTFChapter();
      break;
    case isCompoundIndex:
      //Annotated Compound Index
      beginRTFChapter();
      break;
    case isFileIndex:
      //Annotated File Index
      beginRTFChapter();
      break;
    case isPageIndex:
      //Related Page Index
      beginRTFChapter();
      break;
    case isModuleDocumentation:
      {
        //Module Documentation
        GroupDef *gd=groupList.first();
        bool found=FALSE;
        while (gd && !found)
        {
          if (gd->isLinkableInProject() || gd->countMembers()>0)
          {
            beginRTFChapter();
            found=TRUE;
          }
          gd=groupList.next();
        }
      }
      break;
    case isNamespaceDocumentation:
      {
        // Namespace Documentation
        NamespaceDef *nd=namespaceList.first();
        bool found=FALSE;
        while (nd && !found)
        {
          if (nd->isLinkableInProject())
          {
            beginRTFChapter();
            found=TRUE;
          }
          nd=namespaceList.next();
        } 
      }
      break;
    case isClassDocumentation:
      {
        //Compound Documentation
        ClassDef *cd=classList.first();
        bool found=FALSE;
        while (cd && !found)
        {
          if (cd->isLinkableInProject())
          {
            beginRTFChapter();
            found=TRUE;
          }
          cd=classList.next();
        }
      }
      break;
    case isFileDocumentation:
      {
        //File Documentation
        bool isFirst=TRUE;
        FileName *fn=inputNameList.first();
        while (fn)
        {
          FileDef *fd=fn->first();
          while (fd)
          {
            if (fd->isLinkableInProject())
            {
              if (isFirst)
              {
                beginRTFChapter();				  
                isFirst=FALSE;
                break;
              }
            }
            fd=fn->next();
          }
          fn=inputNameList.next();
        }
      }
      break;
    case isExampleDocumentation:
      {
        //Example Documentation
        beginRTFChapter();
      }
      break;
    case isPageDocumentation:
      {
        //Page Documentation
        beginRTFChapter();
      }
      break;
    case isEndIndex:
      break;
  }
}

void RTFGenerator::endIndexSection(IndexSections is)
{
  switch (is)
  {
    case isTitlePageStart:
      break;
    case isTitlePageAuthor:
      {
        t << " Doxygen}\n"
          "{\\creatim " << dateToRTFDateString() << "}\n"
          "}{\\comment end of infoblock}\n";

        // setup for this section
        t << Rtf_Style_Reset <<"\n";
        t <<"\\sectd\\pgnlcrm\n";
        t <<"{\\footer "<<Rtf_Style_Footer << "{\\chpgn}}\n";
        // the title entry
        t << "{\\comment begin title page}\n";
        t << Rtf_Style_Reset << Rtf_Style_Title << endl; // set to title style
        t << "{\\field\\fldedit {\\*\\fldinst TITLE \\\\*MERGEFORMAT}{\\fldrslt TITLE}}\\par" << endl;
        t << Rtf_Style_Reset << Rtf_Style_SubTitle << endl; // set to subtitle style
        t << "{\\field\\fldedit {\\*\\fldinst AUTHOR \\\\*MERGEFORMAT}{\\fldrslt AUTHOR}}\\par" << endl;
        t << "{\\field\\fldedit {\\*\\fldinst CREATEDATE \\\\*MERGEFORMAT}"
          "{\\fldrslt CREATEDATE}}\\par"<<endl;
        t << "{\\comment End title page}" << endl;

        // table of contents section
        t << "{\\comment Table of contents}\n";
        t << Rtf_Style_Reset << endl;
        t << "{\\field\\fldedit {\\*\\fldinst TOC \\\\f \\\\*MERGEFORMAT}{\\fldrslt Table of contents}}\\par\n";
        t << Rtf_Style_Reset << endl;

      }
      break;
    case isMainPage:
      t << "\\par " << Rtf_Style_Reset  << endl;
      t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"index.rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
      break;
    case isModuleIndex:
      t << "\\par " << Rtf_Style_Reset  << endl;
      t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"modules.rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
      break;
    case isNamespaceIndex:
      t << "\\par " << Rtf_Style_Reset  << endl;
      t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"namespaces.rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
      break;
    case isClassHierarchyIndex:
      t << "\\par " << Rtf_Style_Reset  << endl;
      t << "{\\tc \\v Hierarchical Index}"<< endl;;
      t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"hierarchy.rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
      break;
    case isCompoundIndex:
      t << "\\par " << Rtf_Style_Reset  << endl;
      t << "{\\tc \\v Compound Index}"<< endl;;
      t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"annotated.rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
      break;
    case isFileIndex:
      t << "\\par " << Rtf_Style_Reset  << endl;
      t << "{\\tc \\v File Index}"<< endl;;
      t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"files.rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
      break;
    case isPageIndex:
      t << "\\par " << Rtf_Style_Reset  << endl;
      t << "{\\tc \\v Page Index}"<< endl;;
      t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"pages.rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
      break;
    case isModuleDocumentation:
      {
        GroupDef *gd=groupList.first();
        bool found=FALSE;
        t << "{\\tc \\v Module Documentation}"<< endl;;
        while (gd && !found)
        {
          if (gd->isLinkableInProject() || gd->countMembers()>0)
          {
            t << "\\par " << Rtf_Style_Reset  << endl;
            t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"";
            t << gd->getOutputFileBase();
            t << ".rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";

            found=TRUE;
          }
          gd=groupList.next();
        }
        while (gd)
        {
          if (gd->isLinkableInProject() || gd->countMembers()>0)
          {
            t << "\\par " << Rtf_Style_Reset  << endl;
            beginRTFSection();
            t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"";
            t << gd->getOutputFileBase();
            t << ".rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
          }
          gd=groupList.next();
        }
      }
      break;
    case isNamespaceDocumentation:
      {
        NamespaceDef *nd=namespaceList.first();
        bool found=FALSE;
        while (nd && !found)
        {
          if (nd->isLinkableInProject() || nd->countMembers()>0)
          {
            t << "\\par " << Rtf_Style_Reset  << endl;
            t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"";
            t << nd->getOutputFileBase();
            t << ".rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
            found=TRUE;
          }
          nd=namespaceList.next();
        }
        while (nd)
        {
          if (nd->isLinkableInProject() || nd->countMembers()>0)
          {
            t << "\\par " << Rtf_Style_Reset  << endl;
            beginRTFSection();
            t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"";
            t << nd->getOutputFileBase();
            t << ".rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
          }
          nd=namespaceList.next();
        }
      }
      break;
    case isClassDocumentation:
      {
        ClassDef *cd=classList.first();
        bool found=FALSE;

        t << "{\\tc \\v Class Documentation}"<< endl;;
        while (cd && !found)
        {
          if (cd->isLinkableInProject())
          {
            t << "\\par " << Rtf_Style_Reset  << endl;
            t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"";
            t << cd->getOutputFileBase();
            t << ".rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
            found=TRUE;
          }
          cd=classList.next();
        }
        while (cd)
        {
          if (cd->isLinkableInProject())
          {
            t << "\\par " << Rtf_Style_Reset  << endl;
            beginRTFSection();
            t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"";
            t << cd->getOutputFileBase();
            t << ".rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
          } 
          cd=classList.next();
        }
      }
      break;
    case isFileDocumentation:
      {
        bool isFirst=TRUE;
        FileName *fn=inputNameList.first();

        t << "{\\tc \\v File Documentation}"<< endl;
        while (fn)
        {
          FileDef *fd=fn->first();
          while (fd)
          {
            if (fd->isLinkableInProject())
            {
              if (isFirst)
              {
                t << "\\par " << Rtf_Style_Reset  << endl;
                t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"";
                t << fd->getOutputFileBase();
                t << ".rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
                isFirst=FALSE;
              }
              else
              {
                t << "\\par " << Rtf_Style_Reset  << endl;
                beginRTFSection();
                t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"";
                t << fd->getOutputFileBase();
                t << ".rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
              }
            }
            fd=fn->next();
          }
          fn=inputNameList.next();
        }
      }
      break;
    case isExampleDocumentation:
      {
        //t << "}\n";
        PageInfo *pi=exampleList.first();
        if (pi)
        {
          t << "\\par " << Rtf_Style_Reset  << endl;
          t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"";
          t << convertSlashes(pi->name,TRUE);
          t << "-example.rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
          pi=exampleList.next();
        }
        while (pi)
        {
          t << "\\par " << Rtf_Style_Reset  << endl;
          beginRTFSection();
          t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"";
          t << convertSlashes(pi->name,TRUE);
          t << "-example.rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
          pi=exampleList.next();
        }
      }
      break;
    case isPageDocumentation:
      {
        t << "{\\tc \\v Page Documentation}"<< endl;;
        PageInfo *pi=pageList.first();
        if (pi)
        {
          QCString pageName;
          if (Config::caseSensitiveNames)
            pageName=pi->name.copy();
          else
            pageName=pi->name.lower();
          t << "\\par " << Rtf_Style_Reset  << endl;
          t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"";
          t << pageName;
          t << ".rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
          pi=pageList.next();
        }
        while (pi)
        {
          QCString pageName;
          if (Config::caseSensitiveNames)
            pageName=pi->name.copy();
          else
            pageName=pi->name.lower();
          //t << "\\par " << Rtf_Style_Reset  << endl;
          //beginRTFSection();
          t << "{\\field\\fldedit{\\*\\fldinst INCLUDETEXT \"";
          t << pageName;
          t << ".rtf\" \\\\*MERGEFORMAT}{\\fldrslt includedstuff}}\n";
          pi=pageList.next();
        }
      }
      break;
    case isEndIndex:
      beginRTFChapter();				  
      t << Rtf_Style_Heading1;
      t << "Index\\par "<< endl;
      t << Rtf_Style_Reset<< endl;
      t << "{\\tc \\v Index}" << endl;
      t << "{\\field\\fldedit {\\*\\fldinst INDEX \\\\c2 \\\\*MERGEFORMAT}{\\fldrslt INDEX}}\n";

      break;
   }
}

void RTFGenerator::lastIndexPage()
{
  t <<"{\\comment Begining Body of RTF Document}\n";
  // end page and setup for rest of document
  t <<"\\sect \\sbkpage \\pgnrestart\n";
  t <<"\\sect \\sectd \\sbknone \\pgndec\n";

  // set footer
  t <<"{\\footer "<< Rtf_Style_Footer << "{\\chpgn}}\n";
  //t << Rtf_Style_Heading1 << "\n";

}

void RTFGenerator::writeStyleInfo(int)
{
}

void RTFGenerator::lineBreak()
{
  //t << "\\par" << endl;
  //newParagraph();
  t << "\\line" << endl;
}

void RTFGenerator::writeString(const char *text)
{
  t << text;
}

void RTFGenerator::startIndexList() 
{ 
  t << "{\\comment (startIndexList)}" << endl;
  t << "{" << endl;
  incrementIndentLevel();
  t << Rtf_Style_Reset << Rtf_LCList_DepthStyle() << endl;
  newParagraph();
  m_omitParagraph=TRUE;
}

void RTFGenerator::endIndexList()
{ 
  t << "{\\comment (endIndexList)}" << endl; 
  newParagraph();
  t << "}";
  decrementIndentLevel();
  m_omitParagraph=TRUE;
}

/*! start bullet list */
void RTFGenerator::startItemList()  
{ 
  t << "{\\comment (startItemList level=" << m_listLevel << ") }" << endl; 
  t << "{";
  incrementIndentLevel();
  listItemInfo[m_listLevel].isEnum = FALSE;
  //t << Rtf_Style_Reset << Rtf_BList_DepthStyle();
}

/*! end bullet list */
void RTFGenerator::endItemList()    
{ 
  newParagraph();
  t << "{\\comment (endItemList level=" << m_listLevel << ")}" << endl; 
  t << "}";
  decrementIndentLevel();
  m_omitParagraph=TRUE;
  //t << Rtf_Style_Reset << styleStack.top() << endl;
  //printf("RTFGenerator::endItemList() `%s'\n",styleStack.top());
  //newParagraph();
}

/*! start enumeration list */
void RTFGenerator::startEnumList()  // starts an enumeration list
{ 
  t << "{\\comment (startEnumList)}" << endl; 
  t << "{" << endl;
  incrementIndentLevel();
  listItemInfo[m_listLevel].isEnum = TRUE;
  listItemInfo[m_listLevel].number = 1;
  //t << Rtf_Style_Reset << Rtf_EList_DepthStyle() << endl;
  //newParagraph();
  //m_omitParagraph=TRUE;

}

/*! end enumeration list */
void RTFGenerator::endEnumList()
{ 
  newParagraph();
  t << "{\\comment (endEnumList)}" << endl; 
  t << "}";
  decrementIndentLevel();
  m_omitParagraph=TRUE;
}

/*! write bullet or enum item */
void RTFGenerator::writeListItem()
{
  t << "{\\comment (writeListItem)}" << endl;
  newParagraph();
  t << Rtf_Style_Reset;
  if (listItemInfo[m_listLevel].isEnum)
  {
    t << Rtf_EList_DepthStyle() << endl;
    t << listItemInfo[m_listLevel].number << ".\\tab ";
    listItemInfo[m_listLevel].number++;
  }
  else
  {
    t << Rtf_BList_DepthStyle() << endl;
  }
  m_omitParagraph=TRUE;
}

void RTFGenerator::writeIndexItem(const char *ref,const char *fn,
    const char *name)
{
  t << "{\\comment (writeIndexItem)}" << endl;
  //t << Rtf_LCList_DepthStyle() << endl;
  docify(name);
  if (!ref && fn)
  {
    t << "\\tab ";
    WriteRTFReference(name);
    t << endl;
  }
  else
  {
    t << endl;	
  }
  newParagraph();
  m_omitParagraph=TRUE;
}

//void RTFGenerator::writeIndexFileItem(const char *,const char *text)
//{
//  t << "\\item\\contentsline{section}{";
//  docify(text);
//  t << "}{\\pageref{" << text << "}}" << endl;
//}

void RTFGenerator::writeHtmlLink(const char *url,const char *text)
{

  if (url && Config::rtfHyperFlag)
  {
    t << "{\\field {\\*\\fldinst { HYPERLINK  \\\\l \"";
    t << url;
    t << "\" }{}";
    t << "}{\\fldrslt {\\cs37\\ul\\cf2 "; 

    if (text)
      docify(text);
    else
      docify(url);

    t << "}}}" << endl;
  }
  else
  {
    startTypewriter();
    docify(text);
    endTypewriter();
  }
}

void RTFGenerator::writeMailLink(const char *url)
{
  startTypewriter();
  docify(url);
  endTypewriter();
}

void RTFGenerator::writeStartAnnoItem(const char *,const char *f,
    const char *path,const char *name)
{
  t << "{\\comment (writeStartAnnoItem)}" << endl;
  t << "{\\b ";
  if (path) docify(path);
  if (f && Config::rtfHyperFlag)
  {
    t << "{\\field {\\*\\fldinst { HYPERLINK  \\\\l \"";
    t << formatBmkStr(f);
    t << "\" }{}";
    t << "}{\\fldrslt {\\cs37\\ul\\cf2 "; 

    docify(name);

    t << "}}}" << endl;
  }
  else
  {
    docify(name); 
  }
  t << "} ";
}

void RTFGenerator::writeEndAnnoItem(const char *name)
{
  t << "{\\comment (writeEndAnnoItem)}" << endl;
  if (name)
  {
    t << "\\tab ";
    WriteRTFReference(name);
    t << endl;
  }
  else
  {
    t << endl;	
  }
  newParagraph();
}

void RTFGenerator::startSubsection() 
{ 
  //beginRTFSubSection();
  t <<"\n{\\comment Begin SubSection}\n" << Rtf_Style_Reset;
  t << Rtf_Style_Heading3 << "\n";
}

void RTFGenerator::endSubsection() 
{
  newParagraph();
  t << Rtf_Style_Reset << endl;
}

void RTFGenerator::startSubsubsection() 
{
  //beginRTFSubSubSection();
  t << "\n{\\comment Begin SubSubSection}\n";
  t << "{" << endl;
  t << Rtf_Style_Reset << Rtf_Style_Heading4 << "\n";
}

void RTFGenerator::endSubsubsection() 
{ 
  newParagraph();
  t << "}" << endl;
} 


//void RTFGenerator::writeClassLink(const char *,const char *,
//                                    const char *,const char *name)
//{
//  t << "{\\bf ";
//  docify(name);
//  t << "}"; 
//}

void RTFGenerator::startTextLink(const char *f,const char *anchor)
{
  if (Config::rtfHyperFlag)
  {
    QCString ref;
    if (f)
    {
      ref+=f;
    }
    if (anchor)
    {
      ref+='_';
      ref+=anchor;
    }

    t << "{\\field {\\*\\fldinst { HYPERLINK  \\\\l \"";
    t << formatBmkStr(ref);
    t << "\" }{}";
    t << "}{\\fldrslt {\\cs37\\ul\\cf2 "; 
  }
}

void RTFGenerator::endTextLink()
{
  if (Config::rtfHyperFlag)
  {
    t << "}}}" << endl;
  }
}

void RTFGenerator::writeObjectLink(const char *ref, const char *f,
    const char *anchor, const char *text)
{
  if (!ref && Config::rtfHyperFlag)
  {
    QCString refName;
    if (f)
    {
      refName+=f;
    }
    if (anchor)
    {
      refName+='_';
      refName+=anchor;
    }

    t << "{\\field {\\*\\fldinst { HYPERLINK  \\\\l \"";
    t << formatBmkStr(refName);
    t << "\" }{}";
    t << "}{\\fldrslt {\\cs37\\ul\\cf2 "; 

    docify(text);

    t << "}}}" << endl;
  }
  else
  {
    startBold();
    docify(text);
    endBold();
  } 
}

void RTFGenerator::startPageRef()
{
  t << " (";
  startEmphasis();
}

void RTFGenerator::endPageRef(const char *clname, const char *anchor)
{
  QCString ref;
  if (clname)
  {
    ref+=clname;
  }
  if (anchor)
  {
    ref+='_';
    ref+=anchor;
  }
  WriteRTFReference(ref);
  endEmphasis();
  t << ")";
}

void RTFGenerator::writeCodeLink(const char *ref,const char *f,
    const char *anchor,const char *name)
{
  if (!ref && Config::rtfHyperFlag)
  {
    QCString refName;
    if (f)
    {
      refName+=f;
    }
    if (anchor)
    {
      refName+='_';
      refName+=anchor;
    }

    t << "{\\field {\\*\\fldinst { HYPERLINK  \\\\l \"";
    t << formatBmkStr(refName);
    t << "\" }{}";
    t << "}{\\fldrslt {\\cs37\\ul\\cf2 "; 

    codify(name);

    t << "}}}" << endl;
  }
  else
  {
    codify(name);
  }
}

void RTFGenerator::startTitleHead(const char *)
{
  t <<"{\\comment startTitleHead}" << endl;

  //	beginRTFSection();
  t << Rtf_Style_Reset << Rtf_Style_Heading2 << endl;
}

void RTFGenerator::endTitleHead(const char *fileName,const char *name)
{
  t <<"{\\comment endTitleHead}" << endl;
  t << "\\par " << Rtf_Style_Reset << endl;
  if (name)
  {
    // make table of contents entry
    t << "{\\tc\\tcl2 \\v ";
    docify(name);
    t << "}" << endl;

    // make an index entry
    addToIndex(name,NULL);

    // make a bookmark for referencing
    writeAnchor(name);

    if (fileName)
    {
      // doxygen expects different anchors for pdf and if "FULL PATHS"
      if (strcmp(name,fileName) != 0)
        writeAnchor(fileName);
    }
  }
}

void RTFGenerator::startTitle()
{
  t <<"{\\comment startTitle}" << endl;
  if (Config::compactRTFFlag)
    beginRTFSection();
  else
    beginRTFChapter();
}

void RTFGenerator::startGroupHeader()
{
  t <<"{\\comment startGroupHeader}" << endl;
  t << Rtf_Style_Reset;
  t << Rtf_Style_Heading3; 
  t << endl;
}

void RTFGenerator::endGroupHeader()
{
  t <<"{\\comment endGroupHeader}" << endl;
  newParagraph();
  t << Rtf_Style_Reset << endl;
}

void RTFGenerator::startMemberDoc(const char *clname,
    const char *memname,
    const char *,
    const char *)
{
  t << "{\\comment startMemberDoc}" << endl;
  addToIndex(memname,clname);
  addToIndex(clname,memname);
  //t << Rtf_Style_Reset << Rtf_Style_ListBullet1;
  t << Rtf_Style_Reset << Rtf_Style_Heading4;
  //styleStack.push(Rtf_Style_Heading4);
  t << "{" << endl;
  //printf("RTFGenerator::startMemberDoc() `%s'\n",Rtf_Style_Heading4);
  startBold();
  t << endl;
}

void RTFGenerator::endMemberDoc() 
{
  t << "{\\comment endMemberDoc}" << endl;
  t << "}" << endl;
  //const char *style = styleStack.pop();
  //printf("RTFGenerator::endMemberDoc() `%s'\n",style);
  //ASSERT(style==Rtf_Style_Heading4);
  endBold();
  newParagraph();
}

void RTFGenerator::startDoxyAnchor(const char *fName,const char *clname,
    const char *anchor,const char *)
{
  QCString ref;
  if (clname)
  {
    ref+=clname;
  }
  if (anchor)
  {
    ref+='_';
    ref+=anchor;
  }
  
  t << "{\\bkmkstart ";
  t << formatBmkStr(ref);
  t << "}" << endl;
  t << "{\\bkmkend ";
  t << formatBmkStr(ref);
  t << "}" << endl;

  if (Config::rtfHyperFlag)
  { // doxygen expects a bookmark based on fName for the hyper target so we make a second bookmark
    ref="";

    if (fName)
    {
      ref+=fName;
    }
    if (anchor)
    {
      ref+='_';
      ref+=anchor;
    }

    t << "{\\bkmkstart ";
    t << formatBmkStr(ref);
    t << "}" << endl;
    t << "{\\bkmkend ";
    t << formatBmkStr(ref);
    t << "}" << endl;
  }
}

void RTFGenerator::endDoxyAnchor()
{

}


//void RTFGenerator::writeLatexLabel(const char *clName,const char *anchor)
//{
//  writeDoxyAnchor(0,clName,anchor,0);
//}

void RTFGenerator::addToIndex(const char *s1,const char *s2)
{
  if (s1)
  {
    t << "{\\xe \\v ";
    docify(s1);
    if (s2)
    {
      t << "\\:";
      docify(s2);
    }
    t << "}" << endl;
  }
}

void RTFGenerator::startIndent()
{
  incrementIndentLevel();
  t << "{\\comment (startIndent) }" << endl;
  t << "{" << endl;
  t << Rtf_Style_Reset << Rtf_CList_DepthStyle() << endl;
  //styleStack.push(style);
}

void RTFGenerator::endIndent()
{
  //const char *style = /* Rtf_CList_DepthStyle(); */
  t << "}" << endl;
  decrementIndentLevel();
}


void RTFGenerator::startDescription() 
{ 
  t << "{\\comment (startDescription)}"    << endl; 
  t << "{" << endl;
  t << Rtf_Style_Reset << Rtf_CList_DepthStyle();
}

void RTFGenerator::endDescription()   
{ 
  t << "{\\comment (endDescription)}"    << endl; 
  t << "}";
  newParagraph();
}

void RTFGenerator::startDescItem()
{ 
  newParagraph();
  t << "{\\comment (startDescItem)}"    << endl; 
  t << "{\\b ";
}

void RTFGenerator::endDescItem()
{ 
  t << "{\\comment (endDescItem)}"    << endl;
  t << "}" << endl;
  //newParagraph();
}

void RTFGenerator::startMemberDescription() 
{ 
  t << "{\\comment (startMemberDescription)}"    << endl;
  t << Rtf_Style_Reset << Rtf_CList_DepthStyle();
  startEmphasis();
}

void RTFGenerator::endMemberDescription()
{ 
  t << "{\\comment (endMemberDescription)}"    << endl;
  endEmphasis();
}

void RTFGenerator::startDescList()     
{ 
  t << "{\\comment (startDescList)}"    << endl; 
  t << "{";
  /*if (!m_omitParagraph)*/ newParagraph();
}

void RTFGenerator::endDescTitle()      
{ 
  t << "{\\comment (endDescTitle) }"    << endl; 
  newParagraph();
  //t << Rtf_Style_Reset << styleStack.top();
  incrementIndentLevel();
  t << Rtf_Style_Reset << Rtf_DList_DepthStyle();
}

void RTFGenerator::writeDescItem()
{
  t << "{\\comment (writeDescItem) }"    << endl;
  //	incrementIndentLevel();
  //t << Rtf_Style_Reset << Rtf_CList_DepthStyle(); 
}

void RTFGenerator::endDescList()       
{
  t << "{\\comment (endDescList)}"    << endl; 
  newParagraph();
  t << "}";
  decrementIndentLevel();
  m_omitParagraph = TRUE;
  //t << Rtf_Style_Reset << styleStack.top() << endl;
}

void RTFGenerator::writeSection(const char *lab,const char *title,bool sub)
{
  t << "{\\comment (writeSection)}"    << endl; 
  t<< Rtf_Style_Reset;
  if (sub)
  {
    // set style
    t << Rtf_Style_Heading2; 
    // make table of contents entry
    t << "{\\tc\\tcl2 \\v ";
    docify(title);
    t << "}" << endl;
  }
  else
  {
    // set style
    t << Rtf_Style_Heading3; 
    // make table of contents entry
    t << "{\\tc\\tcl3  \\v ";
    docify(title);
    t << "}" << endl;
  }

  // write out actual title
  docify(title);

  newParagraph();
  // make bookmark
  writeAnchor(lab);
}

void RTFGenerator::writeSectionRef(const char *,const char *lab,
    const char *)
{
  WriteRTFReference(lab);
}

void RTFGenerator::writeSectionRefItem(const char *,const char *lab,
    const char *title)
{
  docify(title);
  t << "\\tab";
  WriteRTFReference(lab);
  t << endl;
}

void RTFGenerator::writeSectionRefAnchor(const char *,const char *lab,
    const char *title)
{
  startBold();
  docify(title);
  endBold();
  t << " (p.~\\pageref{" << lab << "})" << endl;
}

void RTFGenerator::docify(const char *str)
{
  if (str)
  {
    const unsigned char *p=(const unsigned char *)str;
    unsigned char c;
    unsigned char pc='\0';
    while (*p)
    {
      c=*p++;
      switch(c)
      {
        case '{':  t << "\\{";            break;
        case '}':  t << "\\}";            break;
        case '\\': t << "\\\\";	          break;
        default:   
          { 
            // see if we can insert an hyphenation hint
            //if (isupper(c) && islower(pc) && !insideTabbing) t << "\\-";
            t << (char)c;    
          }
      }
      pc = c;
      m_omitParagraph = FALSE;
    }
  }
}

void RTFGenerator::codify(const char *str)
{
  // note that RTF does not have a "verbatim", so "\n" means
  // nothing... add a "newParagraph()";
  //static char spaces[]="        ";
  if (str)
  { 
    const char *p=str;
    char c;
    int spacesToNextTabStop;
    while (*p)
    {
      c=*p++;
      switch(c)
      {
        case '\t':  spacesToNextTabStop = Config::tabSize - (col%Config::tabSize);
                    t << spaces.left(spacesToNextTabStop); 
                    col+=spacesToNextTabStop;
                    break; 
        case '\n':  newParagraph();
                    t << '\n'; col=0;
                    break;
        case '{':   t << "\\{"; col++;          break;
        case '}':   t << "\\}"; col++;          break;
        case '\\':  t << "\\\\"; col++;         break;
        default:    t << c;    col++;           break;
      }
    }
  }
}

void RTFGenerator::writeChar(char c)
{
  char cs[2];
  cs[0]=c;
  cs[1]=0;
  docify(cs);
}

void RTFGenerator::startClassDiagram()
{
}

void RTFGenerator::endClassDiagram(ClassDiagram &d,
    const char *fileName,const char *)
{
  newParagraph();
  t <<"{\\comment This would be an image map..." << endl;

  // create a gif file
  d.writeImageMap(t,dir,fileName);

  t << "}" << endl;

  // display the file
  t << "{" << endl;
  t << Rtf_Style_Reset << endl;
  t << "\\par\\pard \\qc {\\field\\flddirty {\\*\\fldinst INCLUDEPICTURE ";
  t << fileName << ".gif";
  t << " \\\\*MERGEFORMAT}{\\fldrslt IMAGE}}\\par" << endl;
  t << "}" << endl;
}

void RTFGenerator::writeFormula(const char *,const char *text)
{
  t << text;
}

void RTFGenerator::startMemberItem(int) 
{ 
  t << Rtf_Style_Reset << Rtf_BList_DepthStyle() << endl; // set style to apropriate depth
}

void RTFGenerator::endMemberItem(bool) 
{
  newParagraph();
}

void RTFGenerator::writeAnchor(const char *name) 
{ 
  t << "{\\bkmkstart " << formatBmkStr(name) << "}" << endl;
  t << "{\\bkmkend " << formatBmkStr(name) << "}" << endl;
}

void RTFGenerator::WriteRTFReference(const char *label)
{
  t << "{\\field\\fldedit {\\*\\fldinst PAGEREF ";
  t << formatBmkStr(label);
  t << " \\\\*MERGEFORMAT}{\\fldrslt pagenum}}";
}

void RTFGenerator::startCodeFragment() 
{ 
  t << "{" << endl;
  newParagraph();
  t << Rtf_Style_Reset << Rtf_Code_DepthStyle();
  //styleStack.push(Rtf_Style_CodeExample);
}

void RTFGenerator::endCodeFragment()
{ 
  newParagraph();
  //styleStack.pop();
  //printf("RTFGenerator::endCodeFrament() top=%s\n",styleStack.top());
  //t << Rtf_Style_Reset << styleStack.top() << endl; 
  t << "}" << endl;
  m_omitParagraph = TRUE;
}

void RTFGenerator::writeNonBreakableSpace() 
{
  t << "\\~ ";
}


void RTFGenerator::startMemberList()  
{
  t << endl;
#ifdef DELETEDCODE
  if (!insideTabbing)
    t << "\\begin{CompactItemize}" << endl; 
#endif
}

void RTFGenerator::endMemberList()    
{
#ifdef DELETEDCODE
  if (!insideTabbing)
    t << "\\end{CompactItemize}"   << endl; 
#endif
}

void RTFGenerator::writeImage(const char *,const char *,const char *)
{
#ifdef DELETEDCODE
  t << "\\mbox{\\epsfig{file=" << name;
  if (w)
    t << "," << w; 
  else if (h)
    t << "," << h;
  t << "}}" << endl;
#endif
}

void RTFGenerator::startDescTable()  
{ 
  t << "{\\comment (startDescTable) }"    << endl; 
  t << "{" << endl;
  //incrementIndentLevel();
  //t << Rtf_Style_Reset << Rtf_CList_DepthStyle();
}

void RTFGenerator::endDescTable()    
{ 
  //decrementIndentLevel();
  t << "{\\comment (endDescTable)}"      << endl; 
  t << "}" << endl;
  //t << Rtf_Style_Reset << styleStack.top();
}

void RTFGenerator::startDescTableTitle()
{
  //t << Rtf_BList_DepthStyle() << endl;
  t << "{\\comment (startDescTableTitle) }"    << endl; 
  startBold();
}

void RTFGenerator::endDescTableTitle()
{
  t << "{\\comment (endDescTableTitle) }"    << endl; 
  endBold();
  t << "  ";
}

void RTFGenerator::startDescTableData() 
{
  t << "{\\comment (startDescTableData) }"    << endl; 
}

void RTFGenerator::endDescTableData() 
{
  t << "{\\comment (endDescTableData) }"    << endl; 
  newParagraph();
}

// a style for list formatted as a "bulleted list"

void RTFGenerator::incrementIndentLevel()
{
  m_listLevel++;
  if (m_listLevel>indentLevels-1) 
  {
    warn("Warning: Maximum indent level (%d) exceeded while generating RTF output!\n",indentLevels); 
    m_listLevel=indentLevels-1;
  }
}

void RTFGenerator::decrementIndentLevel()
{
  m_listLevel--;
  if (m_listLevel<0) 
  {
    warn("Warning: Negative indent level while generating RTF output!\n");
    m_listLevel=0;
  }
}

// a style for list formatted with "list continue" style
const char * RTFGenerator::Rtf_CList_DepthStyle()
{
  return Rtf_Style_ListContinue[m_listLevel];
}

// a style for list formatted as a "latext style" table of contents
const char * RTFGenerator::Rtf_LCList_DepthStyle()
{
  return Rtf_Style_LatexTOC[m_listLevel];
}

// a style for list formatted as a "bullet" style 
const char * RTFGenerator::Rtf_BList_DepthStyle()
{
  return Rtf_Style_ListBullet[m_listLevel];
}

// a style for list formatted as a "enumeration" style 
const char * RTFGenerator::Rtf_EList_DepthStyle()
{
  return Rtf_Style_ListEnum[m_listLevel];
}

const char * RTFGenerator::Rtf_DList_DepthStyle()
{
  return Rtf_Style_DescContinue[m_listLevel];
}

const char * RTFGenerator::Rtf_Code_DepthStyle()
{
  return Rtf_Style_CodeExample[m_listLevel];
}

void RTFGenerator::startTextBlock(bool dense)
{
  t << "{" << endl;
  t << Rtf_Style_Reset;
  if (dense) // no spacing between "paragraphs"
  {
    t << Rtf_Style_DenseText;
  }
  else // some spacing
  {
    t << Rtf_Style_BodyText;
  }
}

void RTFGenerator::endTextBlock()
{
  newParagraph();
  t << "}" << endl;
  m_omitParagraph = TRUE;
}

void RTFGenerator::newParagraph()
{
  if (!m_omitParagraph) t << "\\par" << endl;
  m_omitParagraph = FALSE;
}

void RTFGenerator::startMemberSubtitle()
{
  t << "{" << endl;
  t << Rtf_Style_Reset << Rtf_CList_DepthStyle() << endl;
}

void RTFGenerator::endMemberSubtitle()
{
  newParagraph();
  t << "}" << endl;
}




/**
 * VERY brittle routine inline RTF's included by other RTF's.
 * it is recursive and ugly.
 */
static bool PreProcessFile(QDir &d,QCString &infName, QTextStream &t, bool bIncludeHeader=TRUE)
{
  QFile f(infName);
  if (!f.open(IO_ReadOnly))
  {
    err("Error opening rtf file %s for reading\n",infName.data());
    return FALSE;
  }

  const int maxLineLength = 10240; 
  static QCString lineBuf(maxLineLength);

  // scan until find end of header
  // this is EXTREEEEEEEMLY brittle.  It works on OUR rtf
  // files because the first line before the body
  // ALWAYS contains "{\comment begin body}"
  do
  {
    if (f.readLine(lineBuf.data(),maxLineLength)==-1)
    {
      err("ERROR - read error in %s before end of RTF header!\n",infName.data());
      return FALSE;
    }
    if (bIncludeHeader) t << lineBuf;
  } while (lineBuf.find("\\comment begin body")==-1);


  //while (fgets(buffer,sizeof(buffer),infp) != NULL)
  while (f.readLine(lineBuf.data(),maxLineLength)!=-1)
  {
    int pos;
    if ((pos=lineBuf.find("INCLUDETEXT"))!=-1)
    {
      int startNamePos  = lineBuf.find('"',pos)+1;
      int endNamePos    = lineBuf.find('"',startNamePos);
      QCString fileName = lineBuf.mid(startNamePos,endNamePos-startNamePos);
      t << "{\\comment begin include " << fileName << "}" << endl;
      if (!PreProcessFile(d,fileName,t,FALSE)) return FALSE;
      t << "{\\comment end include " << fileName << "}" << endl;
    }
    else
    {
      // elaborate hoopla to skip  the final "}" if we didn't include the
      // headers
      if (!f.atEnd() || bIncludeHeader)
      {
        t << lineBuf;
      }
      else
      {
        // null terminate at the last '}'
        //char *str = strrchr(buffer,'}');
        int pos = lineBuf.findRev('}');

        if (pos != -1)
          lineBuf.at(pos) = '\0';
        else
          err("Strange, the last char was not a '}'\n");
        t << lineBuf;
      }
    }
  }
  f.close();
  // remove temporary file
  d.remove(infName);
  return TRUE;
}

void RTFGenerator::startCollaborationDiagram() 
{
}

void RTFGenerator::endCollaborationDiagram(DotGfxUsageGraph &g) 
{
  newParagraph();
  t <<"{\\comment This would be an image map..." << endl;

  g.writeGraph(t,Config::rtfOutputDir);

  t << "}" << endl;

  // display the file
  t << "{" << endl;
  t << Rtf_Style_Reset << endl;
  t << "\\par\\pard \\qc {\\field\\flddirty {\\*\\fldinst INCLUDEPICTURE ";
  t << g.diskName() << ".gif";
  t << " \\\\*MERGEFORMAT}{\\fldrslt IMAGE}}\\par" << endl;
  t << "}" << endl;
}

void RTFGenerator::startInclDepGraph() 
{
}

void RTFGenerator::endInclDepGraph(DotInclDepGraph &g) 
{
  newParagraph();
  t <<"{\\comment This would be an image map..." << endl;

  g.writeGraph(t,Config::rtfOutputDir);

  t << "}" << endl;

  QCString diskName = g.diskName();

  // display the file
  t << "{" << endl;
  t << Rtf_Style_Reset << endl;
  t << "\\par\\pard \\qc {\\field\\flddirty {\\*\\fldinst INCLUDEPICTURE ";
  t << g.diskName() << ".gif";
  t << " \\\\*MERGEFORMAT}{\\fldrslt IMAGE}}\\par" << endl;
  t << "}" << endl;
}

/**
 * This is an API to a VERY brittle RTF preprocessor that combines nested
 * RTF files.  This version replaces the infile with the new file 
 */
bool RTFGenerator::preProcessFileInplace(const char *path,const char *name)
{
  QDir d(path);
  // store the original directory
  if (!d.exists()) 
  { 
    err("Error: Output dir %s does not exist!\n",path); 
    return FALSE;
  }
  QCString oldDir = convertToQCString(QDir::currentDirPath());

  // goto the html output directory (i.e. path)
  QDir::setCurrent(d.absPath());
  QDir thisDir;

  QCString combinedName = (QCString)path+"/combined.rtf";
  QCString mainRTFName  = (QCString)path+"/"+name;

  QFile outf(combinedName);
  if (!outf.open(IO_WriteOnly))
  {
    err("Failed to open %s for writing!\n",combinedName.data());
    return FALSE;
  }
  QTextStream outt(&outf);

  if (!PreProcessFile(thisDir,mainRTFName,outt))
  {
    // it failed, remove the temp file
    outf.close();
    thisDir.remove(combinedName);
    QDir::setCurrent(oldDir);
    return FALSE;
  }

  // everything worked, move the files
  outf.close();
  thisDir.remove(mainRTFName);
  thisDir.rename(combinedName,mainRTFName);

  QDir::setCurrent(oldDir);
  return TRUE;
}
