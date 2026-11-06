/******************************************************************************
 *
 * Copyright (C) 1997-2024 by Dimitri van Heesch.
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

#include "plantumlsvgpatcher.h"
#include "portable.h"
#include "message.h"
#include "debug.h"
#include "docparser.h"
#include "docnode.h"
#include "util.h"
#include "dir.h"
#include "regex.h"

PlantumlSvgPatcher::PlantumlSvgPatcher(const QCString &svgFile, const QCString &relPath, const QCString &context)
  : m_svgFile(svgFile), m_relPath(relPath), m_context(context)
{
}

bool PlantumlSvgPatcher::run()
{
  Debug::print(Debug::Plantuml, 0, "PlantumlSvgPatcher::run() patching file: {}\n", m_svgFile);

  // Read the SVG file
  std::ifstream fi = Portable::openInputStream(m_svgFile);
  if (!fi.is_open())
  {
    err("problem opening file {} for patching!\n", m_svgFile);
    return false;
  }

  // Read entire file content
  std::string content((std::istreambuf_iterator<char>(fi)),
                       std::istreambuf_iterator<char>());
  fi.close();

  QCString svgContent(content);

  // Check if there are any bare refs to patch
  if (svgContent.find("href=\"\\ref\"") == -1 && svgContent.find("xlink:href=\"\\ref\"") == -1)
  {
    Debug::print(Debug::Plantuml, 0, "PlantumlSvgPatcher: No bare refs found in {}\n", m_svgFile);
    return true; // Nothing to patch
  }

  // Patch the content
  QCString patchedContent = patchBareRefs(svgContent);

  // Write back the patched content
  std::ofstream fo = Portable::openOutputStream(m_svgFile);
  if (!fo.is_open())
  {
    err("problem opening file {} for writing!\n", m_svgFile);
    return false;
  }

  fo.write(patchedContent.data(), patchedContent.length());
  fo.close();

  Debug::print(Debug::Plantuml, 0, "PlantumlSvgPatcher: Successfully patched {}\n", m_svgFile);
  return true;
}

QCString PlantumlSvgPatcher::patchBareRefs(const QCString &content) const
{
  QCString result = content;

  // Process all bare refs iteratively
  // Pattern: <a ... href="\ref" ...> ... <text>REFNAME</text> ... </a>
  // We find each occurrence and replace it

  int searchStart = 0;
  while (true)
  {
    // Find the next bare ref (either href="\ref" or xlink:href="\ref")
    int hrefPos = result.find("href=\"\\ref\"", searchStart);
    int xlinkPos = result.find("xlink:href=\"\\ref\"", searchStart);

    // Determine which one comes first
    int refPos = -1;
    if (hrefPos != -1 && (xlinkPos == -1 || hrefPos < xlinkPos))
    {
      refPos = hrefPos;
    }
    else if (xlinkPos != -1)
    {
      refPos = xlinkPos;
    }

    if (refPos == -1)
    {
      break; // No more bare refs
    }

    // Find the containing <a> tag
    int aTagStart = result.findRev("<a", refPos);
    if (aTagStart == -1)
    {
      searchStart = refPos + 1;
      continue;
    }

    // Find the closing </a> tag
    int aTagClose = result.find("</a>", refPos);
    if (aTagClose == -1)
    {
      searchStart = refPos + 1;
      continue;
    }

    // Find the end of the opening <a ...> tag
    int aTagOpenEnd = result.find('>', aTagStart);
    if (aTagOpenEnd == -1 || aTagOpenEnd > aTagClose)
    {
      searchStart = refPos + 1;
      continue;
    }

    // Extract the opening tag
    QCString openingTag = result.mid(aTagStart, aTagOpenEnd - aTagStart + 1);

    // Extract the content between <a ...> and </a>
    QCString anchorContent = result.mid(aTagOpenEnd + 1, aTagClose - aTagOpenEnd - 1);

    // Extract the reference name from the text content
    QCString refName = extractRefNameFromAnchorContent(anchorContent);

    if (refName.isEmpty())
    {
      Debug::print(Debug::Plantuml, 0, "PlantumlSvgPatcher: Could not extract ref name from anchor content\n");
      searchStart = refPos + 1;
      continue;
    }

    // Resolve the reference to a URL
    QCString url = resolveRefToUrl(refName);

    // Build the new opening tag with all replacements done at once
    QCString newOpeningTag = openingTag;

    // Check which attributes exist in this tag
    bool hasHref = openingTag.find("href=\"\\ref\"") != -1;
    bool hasXlinkHref = openingTag.find("xlink:href=\"\\ref\"") != -1;

    if (url.isEmpty())
    {
      // Unresolved reference - use onclick with postMessage for JavaScript handling
      // Escape single quotes and backslashes in refName for JavaScript string
      QCString escapedRefName = substitute(refName, "\\", "\\\\");
      escapedRefName = substitute(escapedRefName, "'", "\\'");

      QCString onclickHandler = "window.parent.postMessage({type:'unresolved-ref',name:'" + escapedRefName + "'},'*');return false;";

      // Replace href attributes with # and add onclick only once
      if (hasHref)
      {
        newOpeningTag = substitute(newOpeningTag, "href=\"\\ref\"", "href=\"#\"");
      }
      if (hasXlinkHref)
      {
        newOpeningTag = substitute(newOpeningTag, "xlink:href=\"\\ref\"", "xlink:href=\"#\"");
      }

      // Add onclick handler before the closing >
      // Find the position of the closing > in the new opening tag
      int closingBracket = newOpeningTag.findRev('>');
      if (closingBracket != -1)
      {
        newOpeningTag = newOpeningTag.left(closingBracket) + " onclick=\"" + onclickHandler + "\"" + newOpeningTag.mid(closingBracket);
      }

      Debug::print(Debug::Plantuml, 0, "PlantumlSvgPatcher: Ref '{}' unresolved, added onclick handler\n", refName);
    }
    else
    {
      // Resolved reference - replace both href attributes with the URL
      if (hasHref)
      {
        newOpeningTag = substitute(newOpeningTag, "href=\"\\ref\"", "href=\"" + url + "\"");
      }
      if (hasXlinkHref)
      {
        newOpeningTag = substitute(newOpeningTag, "xlink:href=\"\\ref\"", "xlink:href=\"" + url + "\"");
      }

      Debug::print(Debug::Plantuml, 0, "PlantumlSvgPatcher: Replaced ref '{}' with URL '{}'\n", refName, url);
    }

    // Replace the entire opening tag
    result = result.left(aTagStart) + newOpeningTag + result.mid(aTagOpenEnd + 1);

    // Move search position forward (accounting for possible length change)
    searchStart = aTagStart + newOpeningTag.length();
  }

  return result;
}

QCString PlantumlSvgPatcher::extractRefNameFromAnchorContent(const QCString &anchorContent)
{
  // Look for <text ...>REFNAME</text> pattern within the anchor content
  static const reg::Ex textPattern(R"(<text[^>]*>([^<]+)</text>)");
  reg::Match match;

  if (reg::search(anchorContent.str(), match, textPattern))
  {
    QCString refName = match[1].str();
    return refName.stripWhiteSpace();
  }

  return QCString();
}

QCString PlantumlSvgPatcher::resolveRefToUrl(const QCString &refName) const
{
  QCString url;
  bool resolved = false;

  auto parser { createDocParser() };
  auto dfAst  { createRef(*parser.get(), refName, m_context) };
  auto dfAstImpl = dynamic_cast<const DocNodeAST*>(dfAst.get());

  if (dfAstImpl)
  {
    const DocRef *df = std::get_if<DocRef>(&dfAstImpl->root);
    if (df)
    {
      // Check if reference was actually resolved (has file or anchor)
      if (!df->file().isEmpty() || !df->anchor().isEmpty())
      {
        url = externalRef(m_relPath, df->ref(), TRUE);
        if (!df->file().isEmpty())
        {
          QCString fn = df->file();
          addHtmlExtensionIfMissing(fn);
          url += fn;
        }
        if (!df->anchor().isEmpty())
        {
          url += "#" + df->anchor();
        }
        resolved = true;
      }
    }
  }

  if (!resolved)
  {
    Debug::print(Debug::Plantuml, 0, "PlantumlSvgPatcher: Ref '{}' unresolved\n", refName);
    // Return empty string to signal unresolved - caller will add onclick handler
    return QCString();
  }

  return url;
}

