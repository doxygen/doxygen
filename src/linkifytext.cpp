/*****************************************************************************
 *
 * Copyright (C) 1997-2026 by Dimitri van Heesch.
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

#include "linkifytext.h"
#include "trace.h"
#include "regex.h"
#include "symbolresolver.h"
#include "filedef.h"
#include "conceptdef.h"
#include "memberdef.h"
#include "util.h"

void linkifyText(const TextGeneratorIntf &out, const DString &text,
    const LinkifyTextOptions &options)
{
  const Definition *scope = options.scope();
  const FileDef *fileScope = options.fileScope();
  const Definition *self = options.self();
  AUTO_TRACE("scope={} fileScope={} text={} autoBreak={} external={} keepSpaces={} indentLevel={}",
      scope?scope->name():"",fileScope?fileScope->name():"",
      text,options.autoBreak(),options.external(),options.keepSpaces(),options.indentLevel());
  if (text.empty()) return;

  //printf("linkify='%s'\n",qPrint(text));
  std::string_view txtStr=text.view();
  size_t strLen = txtStr.length();
  if (strLen==0) return;

  static const reg::Ex regExp(R"((::)?\a[\w~!\\.:$"]*)");
  reg::Iterator it(txtStr,regExp);
  reg::Iterator end;

  //printf("linkifyText scope=%s fileScope=%s strtxt=%s strlen=%zu external=%d\n",
  //    scope ? qPrint(scope->name()):"<none>",
  //    fileScope ? qPrint(fileScope->name()) : "<none>",
  //    qPrint(txtStr),strLen,options.external());
  size_t index=0;
  size_t skipIndex=0;
  size_t floatingIndex=0;
  for (; it!=end ; ++it) // for each word from the text string
  {
    const auto &match = *it;
    size_t newIndex = match.position();
    size_t matchLen = match.length();
    floatingIndex+=newIndex-skipIndex+matchLen;
    if (newIndex>0 && txtStr.at(newIndex-1)=='0') // ignore hex numbers (match x00 in 0x00)
    {
      std::string_view part = txtStr.substr(skipIndex,newIndex+matchLen-skipIndex);
      out.writeString(part,options.keepSpaces());
      skipIndex=index=newIndex+matchLen;
      continue;
    }

    // add non-word part to the result
    bool insideString=false;
    for (size_t i=index;i<newIndex;i++)
    {
      if (txtStr.at(i)=='"') insideString=!insideString;
      if (txtStr.at(i)=='\\') i++; // skip next character it is escaped
    }

    //printf("floatingIndex=%d strlen=%d autoBreak=%d\n",floatingIndex,strLen,options.autoBreak());
    if (strLen>options.breakThreshold()+5 && floatingIndex>options.breakThreshold() &&
        options.autoBreak()) // try to insert a split point
    {
      std::string_view splitText = txtStr.substr(skipIndex,newIndex-skipIndex);
      size_t splitLength = splitText.length();
      size_t offset=1;
      size_t i = splitText.find(',');
      if (i==std::string::npos) { i=splitText.find('<'); if (i!=std::string::npos) offset=0; }
      if (i==std::string::npos) { i=splitText.find("||"); if (i!=std::string::npos) offset=2; }
      if (i==std::string::npos) { i=splitText.find("&&"); if (i!=std::string::npos) offset=2; }
      if (i==std::string::npos) { i=splitText.find(">>"); if (i!=std::string::npos) offset=2; }
      if (i==std::string::npos) i=splitText.find('>');
      if (i==std::string::npos) i=splitText.find(' ');
      //printf("splitText=[%s] len=%d i=%d offset=%d\n",qPrint(splitText),splitLength,i,offset);
      if (i!=std::string::npos) // add a link-break at i in case of Html output
      {
        std::string_view part1 = splitText.substr(0,i+offset);
        out.writeString(part1,options.keepSpaces());
        out.writeBreak(options.indentLevel()==0 ? 0 : options.indentLevel()+1);
        std::string_view part2 = splitText.substr(i+offset);
        out.writeString(part2,options.keepSpaces());
        floatingIndex=splitLength-i-offset+matchLen;
      }
      else
      {
        out.writeString(splitText,options.keepSpaces());
      }
    }
    else
    {
      //ol.docify(txtStr.mid(skipIndex,newIndex-skipIndex));
      std::string_view part = txtStr.substr(skipIndex,newIndex-skipIndex);
      out.writeString(part,options.keepSpaces());
    }
    // get word from string
    std::string_view word=txtStr.substr(newIndex,matchLen);
    DString matchWord = substitute(substitute(word,"\\","::"),".","::");
    bool found=false;
    // check for argument name
    if (options.argumentList())
    {
      for (auto it1 = options.argumentList()->begin(); it1!=options.argumentList()->end(); ++it1)
      {
        if (it1->name == matchWord)
        {
          out.writeString(matchWord.data(),options.keepSpaces());
          found = true;
          break;
        }
      }
    }
    //printf("linkifyText word=%s matchWord=%s scope=%s\n",
    //    qPrint(word),qPrint(matchWord),scope ? qPrint(scope->name()) : "<none>");
    if (!insideString)
    {
      const ClassDef     *cd=nullptr;
      const ConceptDef   *cnd=nullptr;
      const Definition   *d=nullptr;
      //printf("** Match word '%s'\n",qPrint(matchWord));

      SymbolResolver resolver(fileScope);
      cd=resolver.resolveClass(scope,matchWord);
      const MemberDef *typeDef = resolver.getTypedef();
      if (typeDef) // First look at typedef then class, see bug 584184.
      {
        if (options.external() ? typeDef->isLinkable() : typeDef->isLinkableInProject())
        {
          if (typeDef->getOuterScope()!=self)
          {
            //printf("Found typedef %s word='%s'\n",qPrint(typeDef->name()),qPrint(word));
            out.writeLink(typeDef->getReference(),
                typeDef->getOutputFileBase(),
                typeDef->anchor(),
                word);
            found=true;
          }
        }
      }
      auto writeCompoundName = [&](const auto *cd_) {
        if (options.external() ? cd_->isLinkable() : cd_->isLinkableInProject())
        {
          if (self==nullptr || cd_->qualifiedName()!=self->qualifiedName())
          {
            //printf("Found compound %s word='%s'\n",qPrint(cd->name()),qPrint(word));
            out.writeLink(cd_->getReference(),cd_->getOutputFileBase(),cd_->anchor(),word);
            found=true;
          }
        }
      };

      if (found)
      {
        //printf("   -> skip\n");
      }
      else if ((cd=getClass(matchWord)))
      {
        writeCompoundName(cd);
      }
      else if ((cd=getClass(matchWord+"-p"))) // search for Obj-C protocols as well
      {
        writeCompoundName(cd);
      }
      else if ((cnd=getConcept(matchWord))) // search for concepts
      {
        writeCompoundName(cnd);
      }
      else if ((d=resolver.resolveSymbol(scope,matchWord)))
      {
        writeCompoundName(d);
      }
      else
      {
        //printf("   -> nothing\n");
      }

      size_t m = matchWord.rfind("::");
      DString scopeName;
      if (scope &&
          (scope->definitionType()==Definition::TypeClass ||
           scope->definitionType()==Definition::TypeNamespace
          )
         )
      {
        scopeName=scope->name();
      }
      else if (m!=DString::npos)
      {
        scopeName = matchWord.left(m);
        matchWord = matchWord.mid(m+2);
      }

      //printf("ScopeName=%s\n",qPrint(scopeName));
      //if (!found) printf("Trying to link '%s' in '%s'\n",qPrint(word),qPrint(scopeName));
      if (!found)
      {
        GetDefInput input(scopeName,matchWord,DString());
        GetDefResult result = getDefs(input);
        if (result.found && result.md &&
            (options.external() ? result.md->isLinkable() : result.md->isLinkableInProject())
           )
        {
          //printf("Found ref scope=%s\n",d ? qPrint(d->name()) : "<global>");
          //ol.writeObjectLink(d->getReference(),d->getOutputFileBase(),
          //                       md->anchor(),word);
          if (result.md!=self && (self==nullptr || result.md->name()!=self->name()))
            // name check is needed for overloaded members, where getDefs just returns one
          {
            /* in case of Fortran scope and the variable is a non Fortran variable: don't link,
             * see also getLink in fortrancode.l
             */
            if (!(scope &&
                 (scope->getLanguage() == SrcLangExt::Fortran) &&
                 result.md->isVariable() &&
                 (result.md->getLanguage() != SrcLangExt::Fortran)
                )
               )
            {
              //printf("found symbol %s word='%s'\n",qPrint(result.md->name()),qPrint(word));
              out.writeLink(result.md->getReference(),result.md->getOutputFileBase(),
                  result.md->anchor(),word);
              found=true;
            }
          }
        }
      }
    }

    if (!found) // add word to the result
    {
      out.writeString(word,options.keepSpaces());
    }
    // set next start point in the string
    //printf("index=%d/%d\n",index,txtStr.length());
    skipIndex=index=newIndex+matchLen;
  }
  // add last part of the string to the result.
  std::string_view lastPart = txtStr.substr(skipIndex);
  out.writeString(lastPart,options.keepSpaces());
}
