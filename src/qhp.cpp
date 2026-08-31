/*
 * Copyright (C) 1997-2022 Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby
 * granted. No representations are made about the suitability of this software
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * Documents produced by Doxygen are derivative works derived from the
 * input used in their production; they are not affected by this license.
 */

// own header
#include "qhp.h"

// standard includes
#include <algorithm>
#include <memory>
#include <string.h>
#include <vector>
#include <mutex>

// other includes
#include "config.h"
#include "debug.h"
#include "doxygen.h"
#include "groupdef.h"
#include "language.h"
#include "memberdef.h"
#include "message.h"
#include "portable.h"
#include "stringutil.h"
#include "textstream.h"
#include "util.h"
#include "version.h"

static std::once_flag g_blankWritten;


static inline void writeIndent(TextStream &t,int indent)
{
  if (Debug::isFlagSet(Debug::Qhp))
  {
    for (int i=0;i<indent;i++) t << "  ";
  }
}

class QhpSectionTree
{
  private:
    struct Node
    {
      enum class Type { Root, Dir, Section };
      // constructor for root node
      Node() : type(Type::Root), parent(nullptr) {}
      // constructor for dir node
      Node(Node *parent_) : type(Type::Dir), parent(parent_) {}
      // constructor for section node
      Node(Node *parent_, const DString &title_,const DString &ref_)
                          : type(Type::Section), parent(parent_), title(title_), ref(ref_) {}
      Type type;
      Node *parent = nullptr;
      DString title;
      DString ref;
      std::vector<std::unique_ptr<Node>> children;
    };

    Node m_root;
    Node *m_current = &m_root;

    void traverse(const Node &root,TextStream &t,int indent) const
    {
      /* Input:          Output:
       * =================================================
       * Section1        <section title=".." ref="..">
       * Dir1
       *   Section2         <section title=".." ref="..">
       *   Dir2
       *     Section3          <section title=".." ref=".."/>
       *                    </section>
       *                 </section>
       * Section4        <section title=".." ref="..>
       * Dir3
       *   Dir4
       *     Section5       <section title=.." ref=.."/>
       *                 </section>
       * Section6        <section title=".." ref=".."/>
       */
      size_t numChildren = root.children.size();
      size_t i=0;
      while (i<numChildren)
      {
        if (root.children[i]->type==Node::Type::Section)
        {
          i++;
          if (i<numChildren && root.children[i]->type==Node::Type::Dir)
          {
            // we have a dir node
            writeIndent(t,indent);
            t << "<section title=\"" << convertToXML(root.children[i-1]->title) << "\""
              <<         " ref=\""   << convertToXML(root.children[i-1]->ref)   << "\">\n";
            while (i<numChildren && root.children[i]->type==Node::Type::Dir)
            {
              traverse(*root.children[i].get(),t,indent+1);
              i++;
            }
            writeIndent(t,indent);
            t << "</section>\n";
          }
          else // we have a leaf section node
          {
            writeIndent(t,indent);
            t << "<section title=\"" << convertToXML(root.children[i-1]->title) << "\""
              <<           " ref=\"" << convertToXML(root.children[i-1]->ref)   << "\"/>\n";
          }
        }
        else // dir without preceding section (no extra indent)
        {
          traverse(*root.children[i].get(),t,indent);
          i++;
        }
      }
    }

  public:
    void addSection(const DString &title,const DString &ref)
    {
      m_current->children.push_back(std::make_unique<Node>(m_current,title,ref));
    }
    void incLevel()
    {
      auto newNode = new Node(m_current);
      m_current->children.push_back(std::unique_ptr<Node>(newNode));
      m_current = newNode;
    }
    void decLevel()
    {
      ASSERT(m_current->parent!=nullptr);
      if (m_current->parent)
      {
        m_current = m_current->parent;
      }
    }
    void writeToc(TextStream &t) const
    {
      writeIndent(t,2);
      t << "<toc>\n";
      traverse(m_root,t,3);
      writeIndent(t,2);
      t << "</toc>\n";
    }
};

class Qhp::Private
{
  public:
    std::ofstream docFile;
    TextStream doc;
    TextStream index;
    StringSet files;
    QhpSectionTree sectionTree;
};

static DString getFullProjectName()
{
  DString projectName = Config_getString(PROJECT_NAME);
  DString versionText = Config_getString(PROJECT_NUMBER);
  if (projectName.empty()) projectName="Root";
  if (!versionText.empty()) projectName+=" "+versionText;
  return projectName;
}

static DString makeFileName(const DString & withoutExtension)
{
  DString result=withoutExtension;
  if (!result.empty())
  {
    if (result.at(0)=='!') // relative URL -> strip marker
    {
      result=result.mid(1);
    }
    else // add specified HTML extension
    {
      addHtmlExtensionIfMissing(result);
    }
  }
  return result;
}

static DString makeRef(const DString & withoutExtension, const DString & anchor)
{
  //printf("QHP::makeRef(%s,%s)\n",withoutExtension,anchor);
  if (withoutExtension.empty()) return DString();
  DString result = makeFileName(withoutExtension);
  if (anchor.empty()) return result;
  return result+"#"+anchor;
}

Qhp::Qhp() : p(std::make_unique<Private>()) {}
Qhp::~Qhp() = default;

void Qhp::initialize()
{
  /*
  <QtHelpProject version="1.0">
    <namespace>mycompany.com.myapplication.1_0</namespace>
    <virtualFolder>doc</virtualFolder>
    <customFilter name="My Application 1.0">
      <filterAttribute>myapp</filterAttribute>
      <filterAttribute>1.0</filterAttribute>
    </customFilter>
    <filterSection>
      <filterAttribute>myapp</filterAttribute>
      <filterAttribute>1.0</filterAttribute>
  ..
  */
  DString fileName = Config_getString(HTML_OUTPUT) + "/" + qhpFileName;
  p->docFile = Portable::openOutputStream(fileName);
  if (!p->docFile.is_open())
  {
    term("Could not open file {} for writing\n",fileName);
  }
  p->doc.setStream(&p->docFile);

  p->doc << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  p->doc << "<QtHelpProject version=\"1.0\">\n";
  writeIndent(p->doc,1);
  p->doc << "<namespace>" << convertToXML(Config_getString(QHP_NAMESPACE)) << "</namespace>\n";
  writeIndent(p->doc,1);
  p->doc << "<virtualFolder>" << convertToXML(Config_getString(QHP_VIRTUAL_FOLDER)) << "</virtualFolder>\n";

  // Add custom filter
  DString filterName = Config_getString(QHP_CUST_FILTER_NAME);
  if (!filterName.empty())
  {
    writeIndent(p->doc,1);
    p->doc << "<customFilter name=\"" << convertToXML(filterName) << "\">\n";

    StringVector customFilterAttributes =
        split(Config_getString(QHP_CUST_FILTER_ATTRS).str(), " ");
    for (const auto &attr : customFilterAttributes)
    {
      writeIndent(p->doc,2);
      p->doc << "<filterAttribute>" << convertToXML(attr) << "</filterAttribute>\n";
    }
    writeIndent(p->doc,1);
    p->doc << "</customFilter>\n";
  }

  writeIndent(p->doc,1);
  p->doc << "<filterSection>\n";

  // Add section attributes
  StringVector sectionFilterAttributes = split(Config_getString(QHP_SECT_FILTER_ATTRS).str(), " ");
  // always add doxygen as filter attribute
  if (std::find(sectionFilterAttributes.begin(), sectionFilterAttributes.end(), "doxygen") ==
      sectionFilterAttributes.end())
  {
    sectionFilterAttributes.emplace_back("doxygen");
  }
  for (const auto &attr : sectionFilterAttributes)
  {
    writeIndent(p->doc,2);
    p->doc << "<filterAttribute>" << convertToXML(attr) << "</filterAttribute>\n";
  }

  // Add extra root node to the TOC
  p->sectionTree.addSection(getFullProjectName(),"index"+Doxygen::htmlFileExtension);
  p->sectionTree.incLevel();

  writeIndent(p->index,2);
  p->index << "<keywords>\n";
}

void Qhp::finalize()
{
  // close root node
  p->sectionTree.decLevel();

  // Finish TOC
  p->sectionTree.writeToc(p->doc);

  // Finish index
  writeIndent(p->index,2);
  p->index << "</keywords>\n";
  p->doc << p->index.str();

  // Finish files
  writeIndent(p->doc,2);
  p->doc << "<files>\n";
  for (auto &s : p->files)
  {
    writeIndent(p->doc,3);
    p->doc << s << "\n";
  }
  writeIndent(p->doc,2);
  p->doc << "</files>\n";

  writeIndent(p->doc,1);
  p->doc << "</filterSection>\n";
  p->doc << "</QtHelpProject>\n";

  p->doc.flush();
  p->docFile.close();
}

void Qhp::incContentsDepth()
{
  p->sectionTree.incLevel();
}

void Qhp::decContentsDepth()
{
  p->sectionTree.decLevel();
}

void Qhp::addContentsItem(bool /* isDir */, const DString & name, const DString & /*ref*/,
                          const DString & file, const DString &anchor, bool /* separateIndex */,
                          bool /* addToNavIndex */, const Definition * /*def*/, const DString & /*nameAsHtml*/)
{
  /*
  <toc>
    <section title="My Application Manual" ref="index.html">
      <section title="Chapter 1" ref="doc.html#chapter1"/>
      <section title="Chapter 2" ref="doc.html#chapter2"/>
      <section title="Chapter 3" ref="doc.html#chapter3"/>
    </section>
  </toc>
  */

  DString f = file;
  if (!f.empty() && f.at(0)=='^') return; // absolute URL not supported

  if (f.empty())
  {
    f = "doxygen_blank";
    addHtmlExtensionIfMissing(f);
    std::call_once(g_blankWritten,[this,&f]()
    {
      DString fileName = Config_getString(HTML_OUTPUT) + "/" + f;
      std::ofstream blankFile = Portable::openOutputStream(fileName); // we just need an empty file
      if (!blankFile.is_open())
      {
        term("Could not open file {} for writing\n",fileName);
      }
      TextStream blank;
      blank.setStream(&blankFile);
      blank << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"https://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n";
      blank << "<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\"" + theTranslator->trISOLang() + "\">\n";
      blank << "<head>\n";
      blank << "<title>Validator / crawler helper</title>\n";
      blank << "<meta http-equiv=\"Content-Type\" content=\"text/xhtml;charset=UTF-8\"/>\n";
      blank << "<meta http-equiv=\"X-UA-Compatible\" content=\"IE=11\"/>\n";

      blank << "<meta name=\"generator\" content=\"Doxygen " + getDoxygenVersion() + "\"/>\n";
      blank << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/>\n";
      blank << "</head>\n";
      blank << "<body>\n";
      blank << "</body>\n";
      blank << "</html>\n";
      blank.flush();
      blankFile.close();
      addFile(f);
    });
  }
  DString finalRef = makeRef(f, anchor);
  p->sectionTree.addSection(name,finalRef);
}

void Qhp::addIndexItem(const Definition *context,const MemberDef *md,
                       const DString &sectionAnchor,const DString &word)
{
  //printf("addIndexItem(%s %s %s\n",
  //       context?context->name().data():"<none>",
  //       md?md->name().data():"<none>",
  //       qPrint(word));

  if (context && md) // member
  {
    if (sectionAnchor.empty() && !md->hasDocumentation()) return;
    DString cfname  = md->getOutputFileBase();
    DString argStr  = md->argsString();
    DString level1  = context->name();
    DString level2  = !word.empty() ? word : md->name();
    DString anchor  = !sectionAnchor.empty() ? sectionAnchor : md->anchor();
    DString ref;

    // <keyword name="foo" id="MyApplication::foo" ref="doc.html#foo"/>
    ref = makeRef(cfname, anchor);
    DString id = level1+"::"+level2;
    writeIndent(p->index,3);
    p->index << "<keyword name=\"" << convertToXML(level2 + argStr) << "\""
                          " id=\"" << convertToXML(id + "_" + anchor) << "\""
                         " ref=\"" << convertToXML(ref) << "\"/>\n";
  }
  else if (context) // container
  {
    // <keyword name="Foo" id="Foo" ref="doc.html#Foo"/>
    DString contRef = context->getOutputFileBase();
    DString level1  = !word.empty() ? word : context->name();
    DString ref = makeRef(contRef,sectionAnchor);
    writeIndent(p->index,3);
    p->index << "<keyword name=\"" << convertToXML(level1) << "\""
             <<           " id=\"" << convertToXML(level1 +"_" + sectionAnchor) << "\""
             <<          " ref=\"" << convertToXML(ref) << "\"/>\n";
  }
}

void Qhp::addFile(const DString & fileName)
{
  p->files.insert(("<file>" + convertToXML(fileName) + "</file>").str());
}

void Qhp::addIndexFile(const DString & fileName)
{
  addFile(fileName);
}

void Qhp::addImageFile(const DString &fileName)
{
  addFile(fileName);
}

void Qhp::addStyleSheetFile(const DString &fileName)
{
  addFile(fileName);
}

DString Qhp::getQchFileName()
{
  DString qchFile = Config_getString(QCH_FILE);
  if (!qchFile.empty())
  {
    return qchFile;
  }

  DString projectName = Config_getString(PROJECT_NAME);
  DString versionText = Config_getString(PROJECT_NUMBER);

  return DString("../qch/")
      + (projectName.empty() ? DString("index") : projectName)
      + (versionText.empty() ? DString("") : DString("-") + versionText)
      + DString(".qch");
}
