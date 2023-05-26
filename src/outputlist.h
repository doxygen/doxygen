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
 */

#ifndef OUTPUTLIST_H
#define OUTPUTLIST_H

#include <utility>
#include <vector>
#include <memory>
#include <variant>
#include <stack>

#include "outputgen.h"
#include "doxygen.h"

#include "dispatcher.h"

#include "htmlgen.h"
#include "latexgen.h"
#include "rtfgen.h"
#include "mangen.h"
#include "docbookgen.h"
#include "xmlgen.h"
#include "devnullgen.h"

//-------------------------------------------------------------------------------------------

/** Namespace containing typed wrappers to refer to member functions for specific code generators called by OutputCodeList.
 *  To be used in combination with dispatch_call()
 */
namespace OutputCodeIntf
{
  template <class T> struct codify            { static constexpr auto method = &T::codify;            };
  template <class T> struct writeCodeLink     { static constexpr auto method = &T::writeCodeLink;     };
  template <class T> struct writeLineNumber   { static constexpr auto method = &T::writeLineNumber;   };
  template <class T> struct writeTooltip      { static constexpr auto method = &T::writeTooltip;      };
  template <class T> struct startCodeLine     { static constexpr auto method = &T::startCodeLine;     };
  template <class T> struct endCodeLine       { static constexpr auto method = &T::endCodeLine;       };
  template <class T> struct startFontClass    { static constexpr auto method = &T::startFontClass;    };
  template <class T> struct endFontClass      { static constexpr auto method = &T::endFontClass;      };
  template <class T> struct writeCodeAnchor   { static constexpr auto method = &T::writeCodeAnchor;   };
  template <class T> struct startCodeFragment { static constexpr auto method = &T::startCodeFragment; };
  template <class T> struct endCodeFragment   { static constexpr auto method = &T::endCodeFragment;   };
}

/** Helper template class which defers all methods of OutputCodeIntf to an existing object of the templated type.
 *  This allows to add the same generator to multiple lists (one that owns the element, and others that refers to it).
 */
template<class OutputCodeGen>
class OutputCodeDefer
{
  public:
    OutputCodeDefer(OutputCodeGen *codeGen) : m_codeGen(codeGen) {}
    OutputType type() const { return m_codeGen->type(); }
    void codify(const QCString &s) { m_codeGen->codify(s); }
    void writeCodeLink(CodeSymbolType type,
                       const QCString &ref,const QCString &file,
                       const QCString &anchor,const QCString &name,
                       const QCString &tooltip)
    { m_codeGen->writeCodeLink(type,ref,file,anchor,name,tooltip); }

    void writeLineNumber(const QCString &ref,const QCString &file,const QCString &anchor,
                         int lineNumber, bool writeLineAnchor)
    { m_codeGen->writeLineNumber(ref,file,anchor,lineNumber,writeLineAnchor); }

    void writeTooltip(const QCString &id, const DocLinkInfo &docInfo, const QCString &decl,
                      const QCString &desc, const SourceLinkInfo &defInfo, const SourceLinkInfo &declInfo)
    { m_codeGen->writeTooltip(id,docInfo,decl,desc,defInfo,declInfo); }

    void startCodeLine(bool hasLineNumbers)
    { m_codeGen->startCodeLine(hasLineNumbers); }

    void endCodeLine()
    { m_codeGen->endCodeLine(); }

    void startFontClass(const QCString &c)
    { m_codeGen->startFontClass(c); }

    void endFontClass()
    { m_codeGen->endFontClass(); }

    void writeCodeAnchor(const QCString &name)
    { m_codeGen->writeCodeAnchor(name); }

    void startCodeFragment(const QCString &style)
    { m_codeGen->startCodeFragment(style); }

    void endCodeFragment(const QCString &style)
    { m_codeGen->endCodeFragment(style); }

  private:
    OutputCodeGen *m_codeGen;
};

/** Base class that allows alternative implementations outside of the fixed set supported by OutputCodeList.
 *  Used by doxyapp and doxyparse for instance.
 */
class OutputCodeExtension
{
  public:
    virtual ~OutputCodeExtension() = default;
    virtual OutputType type() const = 0;
    virtual void codify(const QCString &s) = 0;
    virtual void writeCodeLink(CodeSymbolType type,
                       const QCString &ref,const QCString &file,
                       const QCString &anchor,const QCString &name,
                       const QCString &tooltip) = 0;
    virtual void writeLineNumber(const QCString &ref,const QCString &file,const QCString &anchor,
                         int lineNumber, bool writeLineAnchor) = 0;
    virtual void writeTooltip(const QCString &id, const DocLinkInfo &docInfo, const QCString &decl,
                      const QCString &desc, const SourceLinkInfo &defInfo, const SourceLinkInfo &declInfo) = 0;
    virtual void startCodeLine(bool hasLineNumbers) = 0;
    virtual void endCodeLine() = 0;
    virtual void startFontClass(const QCString &c) = 0;
    virtual void endFontClass() = 0;
    virtual void writeCodeAnchor(const QCString &name) = 0;
    virtual void startCodeFragment(const QCString &style) = 0;
    virtual void endCodeFragment(const QCString &style) = 0;
};

using HtmlCodeGeneratorDefer    = OutputCodeDefer<HtmlCodeGenerator>;
using LatexCodeGeneratorDefer   = OutputCodeDefer<LatexCodeGenerator>;
using RTFCodeGeneratorDefer     = OutputCodeDefer<RTFCodeGenerator>;
using ManCodeGeneratorDefer     = OutputCodeDefer<ManCodeGenerator>;
using DocbookCodeGeneratorDefer = OutputCodeDefer<DocbookCodeGenerator>;
using OutputCodeDeferExtension  = OutputCodeDefer<OutputCodeExtension>;

/** Class representing a list of different code generators.
 *  It offers the same interface as the specific code generators,
 *  and will forward each method to all generators that are enabled.
 */
class OutputCodeList
{
  public:
    using OutputCodeVariant = std::variant<HtmlCodeGenerator,
                                           HtmlCodeGeneratorDefer,
                                           LatexCodeGenerator,
                                           LatexCodeGeneratorDefer,
                                           RTFCodeGenerator,
                                           RTFCodeGeneratorDefer,
                                           ManCodeGenerator,
                                           ManCodeGeneratorDefer,
                                           DocbookCodeGenerator,
                                           DocbookCodeGeneratorDefer,
                                           XMLCodeGenerator,
                                           DevNullCodeGenerator,
                                           OutputCodeDeferExtension
                                          >;

    int id() const     { return m_id; }
    void setId(int id) { m_id = id;   }

    /** Add a code generator already wrapped in a variant type. */
    void add(OutputCodeVariant &&v)
    {
      m_outputCodeList.emplace_back(std::move(v));
    }

    /** Add a code generator to the list, using a syntax similar to std::make_unique<T>() */
    template<class T,class... As>
    T* add(As&&... args)
    {
      add(OutputCodeVariant{std::in_place_type<T>,std::forward<As>(args)...});
      return std::get_if<T>(&m_outputCodeList.back().variant);
    }

    /** Returns a pointer to a specific generator in the list */
    template<class T>
    T *get()
    {
      for (auto &e : m_outputCodeList)
      {
        T *t = std::get_if<T>(&e.variant);
        if (t) return t;
      }
      return nullptr;
    }

    /** Enable or disable a specific generator */
    void setEnabledFiltered(OutputType o,bool enabled)
    {
      for (auto &e : m_outputCodeList)
      {
        std::visit([&](auto &&arg) { if (arg.type()==o) e.enabled = enabled; },e.variant);
      }
    }

    void clear()
    {
      m_outputCodeList.clear();
    }

    // ---- OutputCodeIntf forwarding

    void codify(const QCString &s)
    { foreach<OutputCodeIntf::codify>(s); }

    void writeCodeLink(CodeSymbolType type,
                       const QCString &ref,const QCString &file,
                       const QCString &anchor,const QCString &name,
                       const QCString &tooltip)
    { foreach<OutputCodeIntf::writeCodeLink>(type,ref,file,anchor,name,tooltip); }

    void writeLineNumber(const QCString &ref,const QCString &file,const QCString &anchor,
                         int lineNumber, bool writeLineAnchor)
    { foreach<OutputCodeIntf::writeLineNumber>(ref,file,anchor,lineNumber,writeLineAnchor); }

    void writeTooltip(const QCString &id, const DocLinkInfo &docInfo, const QCString &decl,
                      const QCString &desc, const SourceLinkInfo &defInfo, const SourceLinkInfo &declInfo)
    { foreach<OutputCodeIntf::writeTooltip>(id,docInfo,decl,desc,defInfo,declInfo); }

    void startCodeLine(bool hasLineNumbers)
    { foreach<OutputCodeIntf::startCodeLine>(hasLineNumbers); }

    void endCodeLine()
    { foreach<OutputCodeIntf::endCodeLine>(); }

    void startFontClass(const QCString &c)
    { foreach<OutputCodeIntf::startFontClass>(c); }

    void endFontClass()
    { foreach<OutputCodeIntf::endFontClass>(); }

    void writeCodeAnchor(const QCString &name)
    { foreach<OutputCodeIntf::writeCodeAnchor>(name); }

    void startCodeFragment(const QCString &style)
    { foreach<OutputCodeIntf::startCodeFragment>(style); }

    void endCodeFragment(const QCString &style)
    { foreach<OutputCodeIntf::endCodeFragment>(style); }

  private:
    template<template <class> class GeneratorT, class... As>
    void foreach(As&&... args)
    {
      for (auto &e : m_outputCodeList)
      {
        if (e.enabled)
        {
          dispatch_call<GeneratorT>(e.variant,std::forward<As>(args)...);
        }
      }
    }

    struct OutputCodeElem
    {
      explicit OutputCodeElem(OutputCodeVariant &&v) : variant(std::move(v)) {}
      OutputCodeVariant variant;
      bool enabled = true;
    };

    std::vector<OutputCodeElem> m_outputCodeList;
    int m_id = -1;

};

//-------------------------------------------------------------------------------------------

class ClassDiagram;
class DotClassGraph;
class DotDirDeps;
class DotInclDepGraph;
class DotGfxHierarchyTable;
class DotGroupCollaboration;


/** Namespace containing typed wrappers to refer to member functions for specific generators called by OutputList.
 *  To be used in combination with dispatch_call()
 */
namespace OutputGenIntf
{
  template<class T> struct codify                      { static constexpr auto method = &T::codify;                      };
  template<class T> struct startFile                   { static constexpr auto method = &T::startFile;                   };
  template<class T> struct writeDoc                    { static constexpr auto method = &T::writeDoc;                    };
  template<class T> struct startIndexSection           { static constexpr auto method = &T::startIndexSection;           };
  template<class T> struct endIndexSection             { static constexpr auto method = &T::endIndexSection;             };
  template<class T> struct writePageLink               { static constexpr auto method = &T::writePageLink;               };
  template<class T> struct startProjectNumber          { static constexpr auto method = &T::startProjectNumber;          };
  template<class T> struct endProjectNumber            { static constexpr auto method = &T::endProjectNumber;            };
  template<class T> struct writeStyleInfo              { static constexpr auto method = &T::writeStyleInfo;              };
  template<class T> struct writeSearchInfo             { static constexpr auto method = &T::writeSearchInfo;             };
  template<class T> struct writeFooter                 { static constexpr auto method = &T::writeFooter;                 };
  template<class T> struct endFile                     { static constexpr auto method = &T::endFile;                     };
  template<class T> struct startTitleHead              { static constexpr auto method = &T::startTitleHead;              };
  template<class T> struct endTitleHead                { static constexpr auto method = &T::endTitleHead;                };
  template<class T> struct startParagraph              { static constexpr auto method = &T::startParagraph;              };
  template<class T> struct endParagraph                { static constexpr auto method = &T::endParagraph;                };
  template<class T> struct writeString                 { static constexpr auto method = &T::writeString;                 };
  template<class T> struct startIndexListItem          { static constexpr auto method = &T::startIndexListItem;          };
  template<class T> struct endIndexListItem            { static constexpr auto method = &T::endIndexListItem;            };
  template<class T> struct startIndexList              { static constexpr auto method = &T::startIndexList;              };
  template<class T> struct endIndexList                { static constexpr auto method = &T::endIndexList;                };
  template<class T> struct startIndexKey               { static constexpr auto method = &T::startIndexKey;               };
  template<class T> struct endIndexKey                 { static constexpr auto method = &T::endIndexKey;                 };
  template<class T> struct startIndexValue             { static constexpr auto method = &T::startIndexValue;             };
  template<class T> struct endIndexValue               { static constexpr auto method = &T::endIndexValue;               };
  template<class T> struct startItemList               { static constexpr auto method = &T::startItemList;               };
  template<class T> struct endItemList                 { static constexpr auto method = &T::endItemList;                 };
  template<class T> struct startIndexItem              { static constexpr auto method = &T::startIndexItem;              };
  template<class T> struct endIndexItem                { static constexpr auto method = &T::endIndexItem;                };
  template<class T> struct docify                      { static constexpr auto method = &T::docify;                      };
  template<class T> struct writeObjectLink             { static constexpr auto method = &T::writeObjectLink;             };
  template<class T> struct startTextLink               { static constexpr auto method = &T::startTextLink;               };
  template<class T> struct endTextLink                 { static constexpr auto method = &T::endTextLink;                 };
  template<class T> struct writeStartAnnoItem          { static constexpr auto method = &T::writeStartAnnoItem;          };
  template<class T> struct startTypewriter             { static constexpr auto method = &T::startTypewriter;             };
  template<class T> struct endTypewriter               { static constexpr auto method = &T::endTypewriter;               };
  template<class T> struct startGroupHeader            { static constexpr auto method = &T::startGroupHeader;            };
  template<class T> struct endGroupHeader              { static constexpr auto method = &T::endGroupHeader;              };
  template<class T> struct startItemListItem           { static constexpr auto method = &T::startItemListItem;           };
  template<class T> struct endItemListItem             { static constexpr auto method = &T::endItemListItem;             };
  template<class T> struct startMemberSections         { static constexpr auto method = &T::startMemberSections;         };
  template<class T> struct endMemberSections           { static constexpr auto method = &T::endMemberSections;           };
  template<class T> struct startHeaderSection          { static constexpr auto method = &T::startHeaderSection;          };
  template<class T> struct endHeaderSection            { static constexpr auto method = &T::endHeaderSection;            };
  template<class T> struct startMemberHeader           { static constexpr auto method = &T::startMemberHeader;           };
  template<class T> struct endMemberHeader             { static constexpr auto method = &T::endMemberHeader;             };
  template<class T> struct startMemberSubtitle         { static constexpr auto method = &T::startMemberSubtitle;         };
  template<class T> struct endMemberSubtitle           { static constexpr auto method = &T::endMemberSubtitle;           };
  template<class T> struct startMemberDocList          { static constexpr auto method = &T::startMemberDocList;          };
  template<class T> struct endMemberDocList            { static constexpr auto method = &T::endMemberDocList;            };
  template<class T> struct startMemberList             { static constexpr auto method = &T::startMemberList;             };
  template<class T> struct endMemberList               { static constexpr auto method = &T::endMemberList;               };
  template<class T> struct startInlineHeader           { static constexpr auto method = &T::startInlineHeader;           };
  template<class T> struct endInlineHeader             { static constexpr auto method = &T::endInlineHeader;             };
  template<class T> struct startAnonTypeScope          { static constexpr auto method = &T::startAnonTypeScope;          };
  template<class T> struct endAnonTypeScope            { static constexpr auto method = &T::endAnonTypeScope;            };
  template<class T> struct startMemberItem             { static constexpr auto method = &T::startMemberItem;             };
  template<class T> struct endMemberItem               { static constexpr auto method = &T::endMemberItem;               };
  template<class T> struct startMemberTemplateParams   { static constexpr auto method = &T::startMemberTemplateParams;   };
  template<class T> struct endMemberTemplateParams     { static constexpr auto method = &T::endMemberTemplateParams;     };
  template<class T> struct startCompoundTemplateParams { static constexpr auto method = &T::startCompoundTemplateParams; };
  template<class T> struct endCompoundTemplateParams   { static constexpr auto method = &T::endCompoundTemplateParams;   };
  template<class T> struct startMemberGroupHeader      { static constexpr auto method = &T::startMemberGroupHeader;      };
  template<class T> struct endMemberGroupHeader        { static constexpr auto method = &T::endMemberGroupHeader;        };
  template<class T> struct startMemberGroupDocs        { static constexpr auto method = &T::startMemberGroupDocs;        };
  template<class T> struct endMemberGroupDocs          { static constexpr auto method = &T::endMemberGroupDocs;          };
  template<class T> struct startMemberGroup            { static constexpr auto method = &T::startMemberGroup;            };
  template<class T> struct endMemberGroup              { static constexpr auto method = &T::endMemberGroup;              };
  template<class T> struct insertMemberAlign           { static constexpr auto method = &T::insertMemberAlign;           };
  template<class T> struct insertMemberAlignLeft       { static constexpr auto method = &T::insertMemberAlignLeft;       };
  template<class T> struct writeRuler                  { static constexpr auto method = &T::writeRuler;                  };
  template<class T> struct writeAnchor                 { static constexpr auto method = &T::writeAnchor;                 };
  template<class T> struct startEmphasis               { static constexpr auto method = &T::startEmphasis;               };
  template<class T> struct endEmphasis                 { static constexpr auto method = &T::endEmphasis;                 };
  template<class T> struct writeChar                   { static constexpr auto method = &T::writeChar;                   };
  template<class T> struct startMemberDoc              { static constexpr auto method = &T::startMemberDoc;              };
  template<class T> struct endMemberDoc                { static constexpr auto method = &T::endMemberDoc;                };
  template<class T> struct startDoxyAnchor             { static constexpr auto method = &T::startDoxyAnchor;             };
  template<class T> struct endDoxyAnchor               { static constexpr auto method = &T::endDoxyAnchor;               };
  template<class T> struct writeLatexSpacing           { static constexpr auto method = &T::writeLatexSpacing;           };
  template<class T> struct startDescForItem            { static constexpr auto method = &T::startDescForItem;            };
  template<class T> struct endDescForItem              { static constexpr auto method = &T::endDescForItem;              };
  template<class T> struct startCenter                 { static constexpr auto method = &T::startCenter;                 };
  template<class T> struct endCenter                   { static constexpr auto method = &T::endCenter;                   };
  template<class T> struct startSmall                  { static constexpr auto method = &T::startSmall;                  };
  template<class T> struct endSmall                    { static constexpr auto method = &T::endSmall;                    };
  template<class T> struct lineBreak                   { static constexpr auto method = &T::lineBreak;                   };
  template<class T> struct startBold                   { static constexpr auto method = &T::startBold;                   };
  template<class T> struct endBold                     { static constexpr auto method = &T::endBold;                     };
  template<class T> struct startMemberDescription      { static constexpr auto method = &T::startMemberDescription;      };
  template<class T> struct endMemberDescription        { static constexpr auto method = &T::endMemberDescription;        };
  template<class T> struct startMemberDeclaration      { static constexpr auto method = &T::startMemberDeclaration;      };
  template<class T> struct endMemberDeclaration        { static constexpr auto method = &T::endMemberDeclaration;        };
  template<class T> struct writeInheritedSectionTitle  { static constexpr auto method = &T::writeInheritedSectionTitle;  };
  template<class T> struct startExamples               { static constexpr auto method = &T::startExamples;               };
  template<class T> struct endExamples                 { static constexpr auto method = &T::endExamples;                 };
  template<class T> struct startIndent                 { static constexpr auto method = &T::startIndent;                 };
  template<class T> struct endIndent                   { static constexpr auto method = &T::endIndent;                   };
  template<class T> struct startSection                { static constexpr auto method = &T::startSection;                };
  template<class T> struct endSection                  { static constexpr auto method = &T::endSection;                  };
  template<class T> struct addIndexItem                { static constexpr auto method = &T::addIndexItem;                };
  template<class T> struct writeSynopsis               { static constexpr auto method = &T::writeSynopsis;               };
  template<class T> struct startClassDiagram           { static constexpr auto method = &T::startClassDiagram;           };
  template<class T> struct endClassDiagram             { static constexpr auto method = &T::endClassDiagram;             };
  template<class T> struct startPageRef                { static constexpr auto method = &T::startPageRef;                };
  template<class T> struct endPageRef                  { static constexpr auto method = &T::endPageRef;                  };
  template<class T> struct startQuickIndices           { static constexpr auto method = &T::startQuickIndices;           };
  template<class T> struct endQuickIndices             { static constexpr auto method = &T::endQuickIndices;             };
  template<class T> struct writeSplitBar               { static constexpr auto method = &T::writeSplitBar;               };
  template<class T> struct writeNavigationPath         { static constexpr auto method = &T::writeNavigationPath;         };
  template<class T> struct writeLogo                   { static constexpr auto method = &T::writeLogo;                   };
  template<class T> struct writeQuickLinks             { static constexpr auto method = &T::writeQuickLinks;             };
  template<class T> struct writeSummaryLink            { static constexpr auto method = &T::writeSummaryLink;            };
  template<class T> struct startContents               { static constexpr auto method = &T::startContents;               };
  template<class T> struct endContents                 { static constexpr auto method = &T::endContents;                 };
  template<class T> struct startPageDoc                { static constexpr auto method = &T::startPageDoc;                };
  template<class T> struct endPageDoc                  { static constexpr auto method = &T::endPageDoc;                  };
  template<class T> struct writeNonBreakableSpace      { static constexpr auto method = &T::writeNonBreakableSpace;      };
  template<class T> struct startDescTable              { static constexpr auto method = &T::startDescTable;              };
  template<class T> struct endDescTable                { static constexpr auto method = &T::endDescTable;                };
  template<class T> struct startDescTableRow           { static constexpr auto method = &T::startDescTableRow;           };
  template<class T> struct endDescTableRow             { static constexpr auto method = &T::endDescTableRow;             };
  template<class T> struct startDescTableTitle         { static constexpr auto method = &T::startDescTableTitle;         };
  template<class T> struct endDescTableTitle           { static constexpr auto method = &T::endDescTableTitle;           };
  template<class T> struct startDescTableData          { static constexpr auto method = &T::startDescTableData;          };
  template<class T> struct endDescTableData            { static constexpr auto method = &T::endDescTableData;            };
  template<class T> struct startDotGraph               { static constexpr auto method = &T::startDotGraph;               };
  template<class T> struct endDotGraph                 { static constexpr auto method = &T::endDotGraph;                 };
  template<class T> struct startInclDepGraph           { static constexpr auto method = &T::startInclDepGraph;           };
  template<class T> struct endInclDepGraph             { static constexpr auto method = &T::endInclDepGraph;             };
  template<class T> struct startCallGraph              { static constexpr auto method = &T::startCallGraph;              };
  template<class T> struct endCallGraph                { static constexpr auto method = &T::endCallGraph;                };
  template<class T> struct startDirDepGraph            { static constexpr auto method = &T::startDirDepGraph;            };
  template<class T> struct endDirDepGraph              { static constexpr auto method = &T::endDirDepGraph;              };
  template<class T> struct startGroupCollaboration     { static constexpr auto method = &T::startGroupCollaboration;     };
  template<class T> struct endGroupCollaboration       { static constexpr auto method = &T::endGroupCollaboration;       };
  template<class T> struct writeGraphicalHierarchy     { static constexpr auto method = &T::writeGraphicalHierarchy;     };
  template<class T> struct startTextBlock              { static constexpr auto method = &T::startTextBlock;              };
  template<class T> struct endTextBlock                { static constexpr auto method = &T::endTextBlock;                };
  template<class T> struct lastIndexPage               { static constexpr auto method = &T::lastIndexPage;               };
  template<class T> struct startMemberDocPrefixItem    { static constexpr auto method = &T::startMemberDocPrefixItem;    };
  template<class T> struct endMemberDocPrefixItem      { static constexpr auto method = &T::endMemberDocPrefixItem;      };
  template<class T> struct startMemberDocName          { static constexpr auto method = &T::startMemberDocName;          };
  template<class T> struct endMemberDocName            { static constexpr auto method = &T::endMemberDocName;            };
  template<class T> struct startParameterType          { static constexpr auto method = &T::startParameterType;          };
  template<class T> struct endParameterType            { static constexpr auto method = &T::endParameterType;            };
  template<class T> struct startParameterName          { static constexpr auto method = &T::startParameterName;          };
  template<class T> struct endParameterName            { static constexpr auto method = &T::endParameterName;            };
  template<class T> struct startParameterList          { static constexpr auto method = &T::startParameterList;          };
  template<class T> struct endParameterList            { static constexpr auto method = &T::endParameterList;            };
  template<class T> struct exceptionEntry              { static constexpr auto method = &T::exceptionEntry;              };
  template<class T> struct startConstraintList         { static constexpr auto method = &T::startConstraintList;         };
  template<class T> struct startConstraintParam        { static constexpr auto method = &T::startConstraintParam;        };
  template<class T> struct endConstraintParam          { static constexpr auto method = &T::endConstraintParam;          };
  template<class T> struct startConstraintType         { static constexpr auto method = &T::startConstraintType;         };
  template<class T> struct endConstraintType           { static constexpr auto method = &T::endConstraintType;           };
  template<class T> struct startConstraintDocs         { static constexpr auto method = &T::startConstraintDocs;         };
  template<class T> struct endConstraintDocs           { static constexpr auto method = &T::endConstraintDocs;           };
  template<class T> struct endConstraintList           { static constexpr auto method = &T::endConstraintList;           };
  template<class T> struct startMemberDocSimple        { static constexpr auto method = &T::startMemberDocSimple;        };
  template<class T> struct endMemberDocSimple          { static constexpr auto method = &T::endMemberDocSimple;          };
  template<class T> struct startInlineMemberType       { static constexpr auto method = &T::startInlineMemberType;       };
  template<class T> struct endInlineMemberType         { static constexpr auto method = &T::endInlineMemberType;         };
  template<class T> struct startInlineMemberName       { static constexpr auto method = &T::startInlineMemberName;       };
  template<class T> struct endInlineMemberName         { static constexpr auto method = &T::endInlineMemberName;         };
  template<class T> struct startInlineMemberDoc        { static constexpr auto method = &T::startInlineMemberDoc;        };
  template<class T> struct endInlineMemberDoc          { static constexpr auto method = &T::endInlineMemberDoc;          };
  template<class T> struct startLabels                 { static constexpr auto method = &T::startLabels;                 };
  template<class T> struct writeLabel                  { static constexpr auto method = &T::writeLabel;                  };
  template<class T> struct endLabels                   { static constexpr auto method = &T::endLabels;                   };
  template<class T> struct writeLocalToc               { static constexpr auto method = &T::writeLocalToc;               };
  template<class T> struct cleanup                     { static constexpr auto method = &T::cleanup;                     };
  template<class T> struct startPlainFile              { static constexpr auto method = &T::startPlainFile;              };
  template<class T> struct endPlainFile                { static constexpr auto method = &T::endPlainFile;                };
}

/** Class representing a list of output generators that are written to in parallel.
 */
class OutputList
{
  public:
    using OutputGenVariant = std::variant<HtmlGenerator,
                                          LatexGenerator,
                                          ManGenerator,
                                          RTFGenerator,
                                          DocbookGenerator>;

    OutputList();
    OutputList(const OutputList &ol);
    OutputList &operator=(const OutputList &ol);
    OutputList(OutputList &&) = delete;
    OutputList &operator=(OutputList &&) = delete;

    template<class DocGenerator>
    void add()
    {
      m_outputGenList.emplace_back(OutputGenVariant{DocGenerator()});
      refreshCodeGenerators();
    }

    const OutputCodeList &codeGenerators() const { return m_codeGenList; }
    OutputCodeList &codeGenerators()             { return m_codeGenList; }

    size_t size() const { return m_outputGenList.size(); }

    void enableAll();
    void disableAll();
    void disable(OutputType o);
    void enable(OutputType o);
    bool isEnabled(OutputType o);
    void disableAllBut(OutputType o);
    void pushGeneratorState();
    void popGeneratorState();

    void generateDoc(const QCString &fileName,int startLine,
                     const Definition *ctx,const MemberDef *md,const QCString &docStr,
                     bool indexWords,bool isExample,const QCString &exampleName /*=0*/,
                     bool singleLine /*=FALSE*/,bool linkFromIndex /*=FALSE*/,
                     bool markdownSupport /*=FALSE*/);

    void startFile(const QCString &name,const QCString &manName,const QCString &title, int hierarchyLevel=0);
    void parseText(const QCString &textStr);

    //////////////////////////////////////////////////
    // OutputDocInterface implementation
    //////////////////////////////////////////////////

    void writeDoc(const IDocNodeAST *ast,const Definition *ctx,const MemberDef *md)
    { foreach<OutputGenIntf::writeDoc>(ast,ctx,md,m_id); }
    void startIndexSection(IndexSection is)
    { foreach<OutputGenIntf::startIndexSection>(is); }
    void endIndexSection(IndexSection is)
    { foreach<OutputGenIntf::endIndexSection>(is); }
    void writePageLink(const QCString &name,bool first)
    { foreach<OutputGenIntf::writePageLink>(name,first); }
    void startProjectNumber()
    { foreach<OutputGenIntf::startProjectNumber>(); }
    void endProjectNumber()
    { foreach<OutputGenIntf::endProjectNumber>(); }
    void writeStyleInfo(int part)
    { foreach<OutputGenIntf::writeStyleInfo>(part); }
    void writeSearchInfo()
    { foreach<OutputGenIntf::writeSearchInfo>(); }
    void writeFooter(const QCString &navPath)
    { foreach<OutputGenIntf::writeFooter>(navPath); }
    void endFile()
    { foreach<OutputGenIntf::endFile>(); }
    void startTitleHead(const QCString &fileName)
    { foreach<OutputGenIntf::startTitleHead>(fileName); }
    void endTitleHead(const QCString &fileName,const QCString &name)
    { foreach<OutputGenIntf::endTitleHead>(fileName,name); }
    void startParagraph(const QCString &classDef=QCString())
    { foreach<OutputGenIntf::startParagraph>(classDef); }
    void endParagraph()
    { foreach<OutputGenIntf::endParagraph>(); }
    void writeString(const QCString &text)
    { foreach<OutputGenIntf::writeString>(text); }
    void startIndexListItem()
    { foreach<OutputGenIntf::startIndexListItem>(); }
    void endIndexListItem()
    { foreach<OutputGenIntf::endIndexListItem>(); }
    void startIndexList()
    { foreach<OutputGenIntf::startIndexList>(); }
    void endIndexList()
    { foreach<OutputGenIntf::endIndexList>(); }
    void startIndexKey()
    { foreach<OutputGenIntf::startIndexKey>(); }
    void endIndexKey()
    { foreach<OutputGenIntf::endIndexKey>(); }
    void startIndexValue(bool b)
    { foreach<OutputGenIntf::startIndexValue>(b); }
    void endIndexValue(const QCString &name,bool b)
    { foreach<OutputGenIntf::endIndexValue>(name,b); }
    void startItemList()
    { foreach<OutputGenIntf::startItemList>(); }
    void endItemList()
    { foreach<OutputGenIntf::endItemList>(); }
    void startIndexItem(const QCString &ref,const QCString &file)
    { foreach<OutputGenIntf::startIndexItem>(ref,file); }
    void endIndexItem(const QCString &ref,const QCString &file)
    { foreach<OutputGenIntf::endIndexItem>(ref,file); }
    void docify(const QCString &s)
    { foreach<OutputGenIntf::docify>(s); }
    void writeObjectLink(const QCString &ref,const QCString &file,
                         const QCString &anchor, const QCString &name)
    { foreach<OutputGenIntf::writeObjectLink>(ref,file,anchor,name); }
    void startTextLink(const QCString &file,const QCString &anchor)
    { foreach<OutputGenIntf::startTextLink>(file,anchor); }
    void endTextLink()
    { foreach<OutputGenIntf::endTextLink>(); }
    void writeStartAnnoItem(const QCString &type,const QCString &file,
                            const QCString &path,const QCString &name)
    { foreach<OutputGenIntf::writeStartAnnoItem>(type,file,path,name); }
    void startTypewriter()
    { foreach<OutputGenIntf::startTypewriter>(); }
    void endTypewriter()
    { foreach<OutputGenIntf::endTypewriter>(); }
    void startGroupHeader(int extraLevels=0)
    { foreach<OutputGenIntf::startGroupHeader>(extraLevels); }
    void endGroupHeader(int extraLevels=0)
    { foreach<OutputGenIntf::endGroupHeader>(extraLevels); }
    void startItemListItem()
    { foreach<OutputGenIntf::startItemListItem>(); }
    void endItemListItem()
    { foreach<OutputGenIntf::endItemListItem>(); }
    void startMemberSections()
    { foreach<OutputGenIntf::startMemberSections>(); }
    void endMemberSections()
    { foreach<OutputGenIntf::endMemberSections>(); }
    void startHeaderSection()
    { foreach<OutputGenIntf::startHeaderSection>(); }
    void endHeaderSection()
    { foreach<OutputGenIntf::endHeaderSection>(); }
    void startMemberHeader(const QCString &anchor, int typ = 2)
    { foreach<OutputGenIntf::startMemberHeader>(anchor,typ); }
    void endMemberHeader()
    { foreach<OutputGenIntf::endMemberHeader>(); }
    void startMemberSubtitle()
    { foreach<OutputGenIntf::startMemberSubtitle>(); }
    void endMemberSubtitle()
    { foreach<OutputGenIntf::endMemberSubtitle>(); }
    void startMemberDocList()
    { foreach<OutputGenIntf::startMemberDocList>(); }
    void endMemberDocList()
    { foreach<OutputGenIntf::endMemberDocList>(); }
    void startMemberList()
    { foreach<OutputGenIntf::startMemberList>(); }
    void endMemberList()
    { foreach<OutputGenIntf::endMemberList>(); }
    void startInlineHeader()
    { foreach<OutputGenIntf::startInlineHeader>(); }
    void endInlineHeader()
    { foreach<OutputGenIntf::endInlineHeader>(); }
    void startAnonTypeScope(int i1)
    { foreach<OutputGenIntf::startAnonTypeScope>(i1); }
    void endAnonTypeScope(int i1)
    { foreach<OutputGenIntf::endAnonTypeScope>(i1); }
    void startMemberItem(const QCString &anchor,OutputGenerator::MemberItemType type,const QCString &id=QCString())
    { foreach<OutputGenIntf::startMemberItem>(anchor,type,id); }
    void endMemberItem(OutputGenerator::MemberItemType type)
    { foreach<OutputGenIntf::endMemberItem>(type); }
    void startMemberTemplateParams()
    { foreach<OutputGenIntf::startMemberTemplateParams>(); }
    void endMemberTemplateParams(const QCString &anchor,const QCString &inheritId)
    { foreach<OutputGenIntf::endMemberTemplateParams>(anchor,inheritId); }
    void startCompoundTemplateParams()
    { foreach<OutputGenIntf::startCompoundTemplateParams>(); }
    void endCompoundTemplateParams()
    { foreach<OutputGenIntf::endCompoundTemplateParams>(); }
    void startMemberGroupHeader(bool b)
    { foreach<OutputGenIntf::startMemberGroupHeader>(b); }
    void endMemberGroupHeader()
    { foreach<OutputGenIntf::endMemberGroupHeader>(); }
    void startMemberGroupDocs()
    { foreach<OutputGenIntf::startMemberGroupDocs>(); }
    void endMemberGroupDocs()
    { foreach<OutputGenIntf::endMemberGroupDocs>(); }
    void startMemberGroup()
    { foreach<OutputGenIntf::startMemberGroup>(); }
    void endMemberGroup(bool last)
    { foreach<OutputGenIntf::endMemberGroup>(last); }
    void insertMemberAlign(bool templ=FALSE)
    { foreach<OutputGenIntf::insertMemberAlign>(templ); }
    void insertMemberAlignLeft(OutputGenerator::MemberItemType typ=OutputGenerator::MemberItemType::Normal, bool templ=FALSE)
    { foreach<OutputGenIntf::insertMemberAlignLeft>(typ,templ); }
    void writeRuler()
    { foreach<OutputGenIntf::writeRuler>(); }
    void writeAnchor(const QCString &fileName,const QCString &name)
    { foreach<OutputGenIntf::writeAnchor>(fileName,name); }
    void startEmphasis()
    { foreach<OutputGenIntf::startEmphasis>(); }
    void endEmphasis()
    { foreach<OutputGenIntf::endEmphasis>(); }
    void writeChar(char c)
    { foreach<OutputGenIntf::writeChar>(c); }
    void startMemberDoc(const QCString &clName,const QCString &memName,
                        const QCString &anchor,const QCString &title,
                        int memCount,int memTotal,bool showInline)
    { foreach<OutputGenIntf::startMemberDoc>(clName,memName,anchor,title,memCount,memTotal,showInline); }
    void endMemberDoc(bool hasArgs)
    { foreach<OutputGenIntf::endMemberDoc>(hasArgs); }
    void startDoxyAnchor(const QCString &fName,const QCString &manName,
                         const QCString &anchor, const QCString &name,
                         const QCString &args)
    { foreach<OutputGenIntf::startDoxyAnchor>(fName,manName,anchor,name,args); }
    void endDoxyAnchor(const QCString &fn,const QCString &anchor)
    { foreach<OutputGenIntf::endDoxyAnchor>(fn,anchor); }
    void writeLatexSpacing()
    { foreach<OutputGenIntf::writeLatexSpacing>(); }
    void startDescForItem()
    { foreach<OutputGenIntf::startDescForItem>(); }
    void endDescForItem()
    { foreach<OutputGenIntf::endDescForItem>(); }
    void startCenter()
    { foreach<OutputGenIntf::startCenter>(); }
    void endCenter()
    { foreach<OutputGenIntf::endCenter>(); }
    void startSmall()
    { foreach<OutputGenIntf::startSmall>(); }
    void endSmall()
    { foreach<OutputGenIntf::endSmall>(); }
    void lineBreak(const QCString &style=QCString())
    { foreach<OutputGenIntf::lineBreak>(style); }
    void startBold()
    { foreach<OutputGenIntf::startBold>(); }
    void endBold()
    { foreach<OutputGenIntf::endBold>(); }
    void startMemberDescription(const QCString &anchor,const QCString &inheritId=QCString(), bool typ = false)
    { foreach<OutputGenIntf::startMemberDescription>(anchor,inheritId, typ); }
    void endMemberDescription()
    { foreach<OutputGenIntf::endMemberDescription>(); }
    void startMemberDeclaration()
    { foreach<OutputGenIntf::startMemberDeclaration>(); }
    void endMemberDeclaration(const QCString &anchor,const QCString &inheritId)
    { foreach<OutputGenIntf::endMemberDeclaration>(anchor,inheritId); }
    void writeInheritedSectionTitle(const QCString &id,   const QCString &ref,
                                    const QCString &file, const QCString &anchor,
                                    const QCString &title,const QCString &name)
    { foreach<OutputGenIntf::writeInheritedSectionTitle>(id,ref,
                                    file,anchor,title,name); }
    void startExamples()
    { foreach<OutputGenIntf::startExamples>(); }
    void endExamples()
    { foreach<OutputGenIntf::endExamples>(); }
    void startIndent()
    { foreach<OutputGenIntf::startIndent>(); }
    void endIndent()
    { foreach<OutputGenIntf::endIndent>(); }
    void startSection(const QCString &lab,const QCString &title,SectionType t)
    { foreach<OutputGenIntf::startSection>(lab,title,t); }
    void endSection(const QCString &lab,SectionType t)
    { foreach<OutputGenIntf::endSection>(lab,t); }
    void addIndexItem(const QCString &s1,const QCString &s2)
    { foreach<OutputGenIntf::addIndexItem>(s1,s2); }
    void writeSynopsis()
    { foreach<OutputGenIntf::writeSynopsis>(); }
    void startClassDiagram()
    { foreach<OutputGenIntf::startClassDiagram>(); }
    void endClassDiagram(const ClassDiagram &d,const QCString &f,const QCString &n)
    { foreach<OutputGenIntf::endClassDiagram>(d,f,n); }
    void startPageRef()
    { foreach<OutputGenIntf::startPageRef>(); }
    void endPageRef(const QCString &c,const QCString &a)
    { foreach<OutputGenIntf::endPageRef>(c,a); }
    void startQuickIndices()
    { foreach<OutputGenIntf::startQuickIndices>(); }
    void endQuickIndices()
    { foreach<OutputGenIntf::endQuickIndices>(); }
    void writeSplitBar(const QCString &name)
    { foreach<OutputGenIntf::writeSplitBar>(name); }
    void writeNavigationPath(const QCString &s)
    { foreach<OutputGenIntf::writeNavigationPath>(s); }
    void writeLogo()
    { foreach<OutputGenIntf::writeLogo>(); }
    void writeQuickLinks(bool compact,HighlightedItem hli,const QCString &file)
    { foreach<OutputGenIntf::writeQuickLinks>(compact,hli,file); }
    void writeSummaryLink(const QCString &file,const QCString &anchor,const QCString &title,bool first)
    { foreach<OutputGenIntf::writeSummaryLink>(file,anchor,title,first); }
    void startContents()
    { foreach<OutputGenIntf::startContents>(); }
    void endContents()
    { foreach<OutputGenIntf::endContents>(); }
    void startPageDoc(const QCString &pageTitle)
    { foreach<OutputGenIntf::startPageDoc>( pageTitle); }
    void endPageDoc()
    { foreach<OutputGenIntf::endPageDoc>(); }
    void writeNonBreakableSpace(int num)
    { foreach<OutputGenIntf::writeNonBreakableSpace>(num); }
    void startDescTable(const QCString &title)
    { foreach<OutputGenIntf::startDescTable>(title); }
    void endDescTable()
    { foreach<OutputGenIntf::endDescTable>(); }
    void startDescTableRow()
    { foreach<OutputGenIntf::startDescTableRow>(); }
    void endDescTableRow()
    { foreach<OutputGenIntf::endDescTableRow>(); }
    void startDescTableTitle()
    { foreach<OutputGenIntf::startDescTableTitle>(); }
    void endDescTableTitle()
    { foreach<OutputGenIntf::endDescTableTitle>(); }
    void startDescTableData()
    { foreach<OutputGenIntf::startDescTableData>(); }
    void endDescTableData()
    { foreach<OutputGenIntf::endDescTableData>(); }
    void startDotGraph()
    { foreach<OutputGenIntf::startDotGraph>(); }
    void endDotGraph(DotClassGraph &g)
    { foreach<OutputGenIntf::endDotGraph>(g); }
    void startInclDepGraph()
    { foreach<OutputGenIntf::startInclDepGraph>(); }
    void endInclDepGraph(DotInclDepGraph &g)
    { foreach<OutputGenIntf::endInclDepGraph>(g); }
    void startCallGraph()
    { foreach<OutputGenIntf::startCallGraph>(); }
    void endCallGraph(DotCallGraph &g)
    { foreach<OutputGenIntf::endCallGraph>(g); }
    void startDirDepGraph()
    { foreach<OutputGenIntf::startDirDepGraph>(); }
    void endDirDepGraph(DotDirDeps &g)
    { foreach<OutputGenIntf::endDirDepGraph>(g); }
    void startGroupCollaboration()
    { foreach<OutputGenIntf::startGroupCollaboration>(); }
    void endGroupCollaboration(DotGroupCollaboration &g)
    { foreach<OutputGenIntf::endGroupCollaboration>(g); }
    void writeGraphicalHierarchy(DotGfxHierarchyTable &g)
    { foreach<OutputGenIntf::writeGraphicalHierarchy>(g); }
    void startTextBlock(bool dense=FALSE)
    { foreach<OutputGenIntf::startTextBlock>(dense); }
    void endTextBlock(bool paraBreak=FALSE)
    { foreach<OutputGenIntf::endTextBlock>(paraBreak); }
    void lastIndexPage()
    { foreach<OutputGenIntf::lastIndexPage>(); }
    void startMemberDocPrefixItem()
    { foreach<OutputGenIntf::startMemberDocPrefixItem>(); }
    void endMemberDocPrefixItem()
    { foreach<OutputGenIntf::endMemberDocPrefixItem>(); }
    void startMemberDocName(bool align)
    { foreach<OutputGenIntf::startMemberDocName>(align); }
    void endMemberDocName()
    { foreach<OutputGenIntf::endMemberDocName>(); }
    void startParameterType(bool first,const QCString &key)
    { foreach<OutputGenIntf::startParameterType>(first,key); }
    void endParameterType()
    { foreach<OutputGenIntf::endParameterType>(); }
    void startParameterName(bool one)
    { foreach<OutputGenIntf::startParameterName>(one); }
    void endParameterName(bool last,bool one,bool bracket)
    { foreach<OutputGenIntf::endParameterName>(last,one,bracket); }
    void startParameterList(bool openBracket)
    { foreach<OutputGenIntf::startParameterList>(openBracket); }
    void endParameterList()
    { foreach<OutputGenIntf::endParameterList>(); }
    void exceptionEntry(const QCString &prefix,bool closeBracket)
    { foreach<OutputGenIntf::exceptionEntry>(prefix,closeBracket); }

    void startConstraintList(const QCString &header)
    { foreach<OutputGenIntf::startConstraintList>(header); }
    void startConstraintParam()
    { foreach<OutputGenIntf::startConstraintParam>(); }
    void endConstraintParam()
    { foreach<OutputGenIntf::endConstraintParam>(); }
    void startConstraintType()
    { foreach<OutputGenIntf::startConstraintType>(); }
    void endConstraintType()
    { foreach<OutputGenIntf::endConstraintType>(); }
    void startConstraintDocs()
    { foreach<OutputGenIntf::startConstraintDocs>(); }
    void endConstraintDocs()
    { foreach<OutputGenIntf::endConstraintDocs>(); }
    void endConstraintList()
    { foreach<OutputGenIntf::endConstraintList>(); }

    void startMemberDocSimple(bool b)
    { foreach<OutputGenIntf::startMemberDocSimple>(b); }
    void endMemberDocSimple(bool b)
    { foreach<OutputGenIntf::endMemberDocSimple>(b); }
    void startInlineMemberType()
    { foreach<OutputGenIntf::startInlineMemberType>(); }
    void endInlineMemberType()
    { foreach<OutputGenIntf::endInlineMemberType>(); }
    void startInlineMemberName()
    { foreach<OutputGenIntf::startInlineMemberName>(); }
    void endInlineMemberName()
    { foreach<OutputGenIntf::endInlineMemberName>(); }
    void startInlineMemberDoc()
    { foreach<OutputGenIntf::startInlineMemberDoc>(); }
    void endInlineMemberDoc()
    { foreach<OutputGenIntf::endInlineMemberDoc>(); }

    void startLabels()
    { foreach<OutputGenIntf::startLabels>(); }
    void writeLabel(const QCString &l,bool isLast)
    { foreach<OutputGenIntf::writeLabel>(l,isLast); }
    void endLabels()
    { foreach<OutputGenIntf::endLabels>(); }

    void writeLocalToc(const SectionRefs &refs,const LocalToc &lt)
    { foreach<OutputGenIntf::writeLocalToc>(refs,lt); }

    void cleanup()
    { foreach<OutputGenIntf::cleanup>(); }

    void startPlainFile(const QCString &name)
    { foreach<OutputGenIntf::startPlainFile>(name); }
    void endPlainFile()
    { foreach<OutputGenIntf::endPlainFile>(); }

  private:
    void newId();
    void syncEnabled();
    void refreshCodeGenerators();

    template<template <class> class GeneratorT, class... As>
    void foreach(As&&... args)
    {
      for (auto &e : m_outputGenList)
      {
        if (e.enabled)
        {
          dispatch_call<GeneratorT>(e.variant,std::forward<As>(args)...);
        }
      }
    }

    struct OutputGenElem
    {
      explicit OutputGenElem(OutputGenVariant &&v) : variant(std::move(v)) {}
      OutputGenVariant variant;
      void setEnabled(bool e) { enabled = e && !enabledStack.empty() ? enabledStack.top() : e; }
      bool enabled = true;
      std::stack<bool> enabledStack;
    };

    std::vector<OutputGenElem> m_outputGenList;
    OutputCodeList m_codeGenList;
    int m_id;

};

#endif
