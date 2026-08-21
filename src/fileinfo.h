/******************************************************************************
 *
 * Copyright (C) 1997-2021 by Dimitri van Heesch.
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

#ifndef FILEINFO_H
#define FILEINFO_H

#include <string>

#include "regex.h"
#include "utf8.h"

/** @brief Minimal replacement for QFileInfo. */
class FileInfo
{
  public:
    explicit FileInfo(const std::string &name) : m_name(name) {}
    bool exists() const;
    size_t size() const;
    bool isWritable() const;
    bool isReadable() const;
    bool isExecutable() const;
    bool isRelative() const;
    bool isFile() const;
    bool isDir() const;
    bool isSymLink() const;
    std::string readLink() const;
    std::string filePath() const;
    std::string absFilePath() const;
    std::string fileName() const;
    std::string baseName() const;
    std::string extension(bool complete) const;
    std::string dirPath(bool absPath = true) const;

    /*!
     * @brief Match the file name against a list of patterns.
     *
     * The pattern list can be a list of strings or a list of objects that can be converted to strings.
     * The pattern is matched against the file name, the file path and the absolute file path.
     * If a match is found, the matching pattern is returned in elem (if not nullptr).
     *
     * @param patList List of patterns to match against.
     * @param caseSenseNames Whether to match case-sensitively or not.
     * @param elem Pointer to store the matching pattern (optional).
     * @param getter Function to convert pattern elements to strings (optional).
     * @return True if a match is found, false otherwise.
     */
    template<class PatternList, class PatternElem = std::string, typename PatternGet = std::string(*)(const PatternElem &)>
    bool match(const PatternList &patList,
               bool caseSenseNames,
               PatternElem *elem = nullptr,
               PatternGet getter = [](const std::string &s){ return s; }
              ) const
    {
      bool found = false;

      if (!patList.empty())
      {
        std::string fn = fileName();
        std::string fp = filePath();
        std::string afp= absFilePath();

        for (const auto &li : patList)
        {
          std::string pattern = getter(li);
          if (!pattern.empty())
          {
            size_t i=pattern.find('=');
            if (i!=std::string::npos) pattern=pattern.substr(0,i); // strip of the extension specific filter name

            if (!caseSenseNames)
            {
              pattern = convertUTF8ToLower(pattern);
              fn      = convertUTF8ToLower(fn);
              fp      = convertUTF8ToLower(fp);
              afp     = convertUTF8ToLower(afp);
            }
            reg::Ex re(pattern,reg::Ex::Mode::Wildcard);
            found = re.isValid() && (reg::match(fn,re) ||
                                     (fn!=fp && reg::match(fp,re)) ||
                                     (fn!=afp && fp!=afp && reg::match(afp,re)));
            if (found)
            {
              if (elem) *elem = li;
              break;
            }
            //printf("Matching '%s' against pattern '%s' found=%d\n",
            //    qPrint(fi->fileName()),qPrint(pattern),found);
          }
        }
      }
      return found;
    }

  private:
    static void correctPath(std::string &s);
    std::string m_name;
};

#endif
