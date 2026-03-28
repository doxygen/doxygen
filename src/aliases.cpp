/******************************************************************************
 *
 * Copyright (C) 1997-2023 by Dimitri van Heesch.
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

#include <unordered_map>
#include <cassert>

#include "message.h"
#include "aliases.h"
#include "containers.h"
#include "config.h"
#include "regex.h"
#include "textstream.h"
#include "util.h"
#include "debug.h"
#include "stringutil.h"

//-----------------------------------------------------------

struct AliasInfo
{
  AliasInfo(const std::string &val,const std::string &sep=std::string())
    : value(val), separator(sep) {}
  std::string value;
  std::string separator;
};

using AliasOverloads = std::unordered_map<int,AliasInfo>;                // key = parameter count
using AliasInfoMap   = std::unordered_map<std::string,AliasOverloads>;   // key = alias name (with parameter part)

//-----------------------------------------------------------

static std::string expandAliasRec(StringUnorderedSet &aliasesProcessed,
                               std::string_view s,bool allowRecursion=FALSE);
static int countAliasArguments(std::string_view args, std::string_view sep);
static std::string extractAliasArgs(std::string_view args);
static std::string expandAlias(std::string_view aliasName,std::string_view aliasValue);

//-----------------------------------------------------------

static AliasInfoMap          g_aliasInfoMap;

//-----------------------------------------------------------

static void addValidAliasToMap(std::string_view alias)
{
  bool        valid = true;
  std::string aliasName;
  std::string aliasValue;
  int         numParams = 0;
  std::string separator;

  static std::string_view separators = "!#$%&,.?|;:'+=~`/";
  auto isValidSeparator = [](char c) -> bool { return separators.find(c)!=std::string::npos; };

  static const reg::Ex re(R"(^(\a[\w-]*)({[^}]*})?\s*=)");
  reg::Match m;
  if (reg::search(alias,m,re)) // valid name= or name{...}= part
  {
    size_t i=m.length();
    assert(i!=std::string::npos); // based on re is always a =
    assert(m.size()==3); // m[0]=full match including '=', m[1]=name, m[2]=optional params
    aliasName  = m[1].str();
    aliasValue = alias.substr(i);
    //printf("Alias: found name='%s' value='%s'\n",qPrint(name),qPrint(aliasValue));
    if (m[2].length()>0) // alias with parameters
    {
      separator=",";
      size_t b = m[2].position();   // index of '{'
      size_t e = b + m[2].length(); // index of '}'
      size_t k=b+1;
      while (k<e-1 && isdigit(alias[k])) k++;
      numParams = atoi(std::string{alias.substr(b+1,k-b-1)}.c_str());
      if (numParams>0)
      {
        if (k<e-1) // we have a separator
        {
          size_t s=k;
          while (s<e && isValidSeparator(alias[s])) s++;
          if (s<e-1)
          {
            err("Invalid alias '{}': invalid separator character '{:c}' (code {:d}), allowed characters: {}. Check your config file.\n",alias,alias[s],alias[s],separators);
            valid=false;
          }
          else
          {
            separator=alias.substr(k,e-k-1);
          }
        }
        if (valid) // valid alias with parameters
        {
          Debug::print(Debug::Alias,0,"Alias definition: name='{}' #param='{}' separator='{}' value='{}'\n",
              aliasName,numParams,separator,aliasValue);
        }
      }
      else
      {
        err("Invalid alias '{}': missing number of parameters. Check your config file.\n",alias);
        valid=false;
      }
    }
    else // valid alias without parameters
    {
      numParams = 0;
      Debug::print(Debug::Alias,0,"Alias definition: name='{}' value='{}'\n",aliasName,aliasValue);
    }
  }
  else
  {
    err("Invalid alias '{}': invalid 'name=' or 'name{{...}}=' part. Check you config file.\n",alias);
    valid=false;
  }

  if (valid) // alias definition passed all checks, so store it.
  {
    auto it = g_aliasInfoMap.find(aliasName);
    if (it==g_aliasInfoMap.end()) // insert new alias
    {
      AliasOverloads overloads { { numParams, AliasInfo(aliasValue, separator) } };
      g_aliasInfoMap.emplace(aliasName,overloads);
    }
    else // replace exiting alias with new definition
    {
      auto it2 = it->second.find(numParams);
      if (it2==it->second.end()) // new alias overload for the given number of parameters
      {
        it->second.emplace(numParams, AliasInfo(aliasValue,separator));
      }
      else // replace alias with new definition
      {
        it2->second = AliasInfo(aliasValue,separator);
      }
    }
  }
}


//----------------------------------------------------------------------------

static std::string escapeAlias(std::string_view value)
{
  std::string newValue = substituteStringView(value,"^^ ","@ilinebr ");
  newValue = substituteStringView(newValue,"^^","@ilinebr ");
  //printf("escapeAlias('%s')='%s'\n",qPrint(std::string{value}),qPrint(newValue));
  return newValue;
}

//----------------------------------------------------------------------------

void readAliases()
{
  // add aliases to a dictionary
  const StringVector &aliasList = Config_getList(ALIASES);
  for (const auto &al : aliasList)
  {
    addValidAliasToMap(al);
  }
  for (auto &[name,overloads] : g_aliasInfoMap)
  {
    for (auto &[numParams,aliasInfo] : overloads)
    {
      aliasInfo.value = expandAlias(name+":"+std::to_string(numParams),aliasInfo.value);
    }
  }
  for (auto &[name,overloads] : g_aliasInfoMap)
  {
    for (auto &[numParams,aliasInfo] : overloads)
    {
      aliasInfo.value = escapeAlias(aliasInfo.value);
    }
  }
}

//--------------------------------------------------------------------------------------

struct Marker
{
  Marker(size_t p, size_t n,size_t s) : pos(p), number(n), size(s) {}
  size_t pos;    // position in the string
  size_t number; // argument number
  size_t size;   // size of the marker
};

/** For a string \a s that starts with a command name, returns the character
 *  offset within that string representing the first character after the
 *  command. For an alias with argument, this is the offset to the
 *  character just after the argument list.
 *
 *  Examples:
 *  - s=="a b"      returns 1
 *  - s=="a{2,3} b" returns 6
 *  = s=="#"        returns 0
 */
static size_t findEndOfCommand(std::string_view s)
{
  size_t i = 0;
  while (i < s.size() && isId(s[i])) ++i;
  if (i < s.size() && s[i] == '{')
  {
    i += extractAliasArgs(s.substr(i)).length() + 2; // +2 for '{' and '}'
  }
  return i;
}

/** Replaces the markers in an alias definition \a aliasValue
 *  with the corresponding values found in the comma separated argument
 *  list \a argList and the returns the result after recursive alias expansion.
 */
static std::string replaceAliasArguments(StringUnorderedSet &aliasesProcessed,
                                      std::string_view aliasValue,std::string_view argList,
                                      std::string_view sep)
{
  //printf("----- replaceAliasArguments(val=[%s],args=[%s],sep=[%s])\n",qPrint(aliasValue),qPrint(argList),qPrint(sep));

  // first make a list of arguments from the comma separated argument list
  StringViewVector args;
  size_t l=argList.length();
  size_t p=0;
  for (size_t i=0;i<l;i++)
  {
    char c = argList[i];
    if (!sep.empty() &&
        c==sep[0] &&                         // start with separator character
        (i==0 || argList[i-1]!='\\') &&      // is not escaped
        argList.substr(i,sep.length())==sep) // whole separator matches
    {
      args.push_back(argList.substr(p,i-p));
      p = i+sep.length(); // start of next argument
      i = p-1; // compensate with -1 for loop iterator
    }
    else if (c=='@' || c=='\\') // command
    {
      // check if this is the start of another aliased command (see bug704172)
      i+=findEndOfCommand(argList.substr(i+1));
    }
  }
  if (l>p) args.push_back(argList.substr(p));
  //printf("found %zu arguments\n",args.size());

  // next we look for the positions of the markers and add them to a list
  std::vector<Marker> markerList;
  l = aliasValue.length();
  char pc             = '\0';
  bool insideMarkerId = false;
  size_t markerStart  = 0;
  auto isDigit = [](char c) { return c>='0' && c<='9'; };
  for (size_t i=0;i<=l;i++)
  {
    char c = i<l ? aliasValue[i] : '\0';
    if (insideMarkerId && !isDigit(c)) // found end of a markerId
    {
      insideMarkerId = false;
      size_t markerLen = i-markerStart;
      markerList.emplace_back(markerStart-1,
                              static_cast<size_t>(std::stoi(std::string{aliasValue.substr(markerStart,markerLen)})),
                              markerLen+1);
    }
    if (c=='\\' && (pc=='@' || pc=='\\')) // found escaped backslash
    {
      // skip
      pc = '\0';
    }
    else
    {
      if (isDigit(c) && pc=='\\') // found start of a markerId
      {
        insideMarkerId=true;
        markerStart=i;
      }
      pc = c;
    }
  }

  // then we replace the markers with the corresponding arguments in one pass
  std::string result;
  p = 0;
  for (const Marker &m : markerList)
  {
    result+=aliasValue.substr(p,m.pos-p);
    //printf("part before marker: '%s'\n",qPrint(aliasValue.substr(p,m.pos-p)));
    if (m.number>0 && m.number<=args.size()) // valid number
    {
      result+=expandAliasRec(aliasesProcessed,args[m.number-1],true);
      //printf("marker index=%zu pos=%zu number=%zu size=%zu replacement %s\n",i,m.pos,m.number,m.size,
      //    qPrint(args[m.number-1]));
    }
    p=m.pos+m.size; // continue after the marker
  }
  result+=aliasValue.substr(p); // append remainder
  //printf("string after replacement of markers: '%s'\n",qPrint(result));

  // expand the result again
  substituteInplace(result,"\\{","{");
  substituteInplace(result,"\\}","}");
  substituteInplace(result,std::string{"\\"}+std::string{sep},sep);
  result = expandAliasRec(aliasesProcessed,result);

  //printf("final string '%s'\n",qPrint(result));
  return result;
}

static std::string escapeSeparators(const std::string &s, const std::string &sep)
{
  if (s.empty() || sep.empty()) return s;
  std::string result;
  result.reserve(s.length()+10);
  size_t i, p=0, l=sep.length();
  while ((i=s.find(sep,p))!=std::string::npos)
  {
    result += s.substr(p,i-p);
    if (i>0 && s[i-1]!='\\') // escape the separator
    {
      result += '\\';
    }
    result += s.substr(i,l);
    p = i+l;
  }
  result += s.substr(p);
  //printf("escapeSeparators(%s,sep='%s')=%s\n",qPrint(s),qPrint(sep),qPrint(result));
  return result;
}

static std::string expandAliasRec(StringUnorderedSet &aliasesProcessed,std::string_view s,bool allowRecursion)
{
  std::string result;
  static const reg::Ex re(R"([\\@](\a[\w-]*))");
  std::string str{s};
  reg::Match match;
  size_t p = 0;
  while (reg::search(str,match,re,p))
  {
    size_t i = match.position();
    size_t l = match.length();
    if (i>p) result+=s.substr(p,i-p);

    std::string args    = extractAliasArgs(s.substr(i+l));
    bool hasArgs        = !args.empty();            // found directly after command
    size_t argsLen      = args.length();
    std::string cmd     = match[1].str();
    int selectedNumArgs = -1;
    //printf("looking for alias '%s' with params '%s'\n",qPrint(cmd),qPrint(args));
    auto it = g_aliasInfoMap.find(cmd);
    if (it == g_aliasInfoMap.end())
    {
      // if command has a - then also try part in without it
      size_t minusPos = cmd.find('-');
      if (minusPos!=std::string::npos)
      {
        it = g_aliasInfoMap.find(cmd.substr(0,minusPos));
        if (it!=g_aliasInfoMap.end()) // found part before - as alias
        {
          cmd     = cmd.substr(0,minusPos);
          args    = "";
          hasArgs = false;
          argsLen = 0;
          l       = cmd.length()+1; // +1 for the minus sign
        }
      }
    }
    if (it != g_aliasInfoMap.end()) // cmd is an alias
    {
      //printf("found an alias, hasArgs=%d\n",hasArgs);
      if (hasArgs)
      {
        // Find the an alias that matches the number of arguments.
        // If there are multiple candidates, take the one that matches the most parameters
        for (const auto &[numParams,aliasInfo] : it->second)
        {
          int numArgs = countAliasArguments(args,aliasInfo.separator);
          if (numParams==numArgs && numArgs>selectedNumArgs)
          {
            selectedNumArgs = numArgs;
          }
        }
        if (selectedNumArgs==-1) // no match found, check if there is an alias with one argument
        {
          auto it2 = it->second.find(1);
          if (it2 != it->second.end())
          {
            args = escapeSeparators(args,it2->second.separator); // escape separator so that everything is seen as one argument
            selectedNumArgs = 1;
          }
        }
      }
      else
      {
        selectedNumArgs = 0;
      }
    }
    else
    {
      //printf("Alias %s not found\n",qPrint(cmd));
    }
    //printf("Found command s='%s' cmd='%s' numArgs=%d args='%s'\n", qPrint(s),qPrint(cmd),selectedNumArgs,qPrint(args));
    std::string qualifiedName = cmd+":"+std::to_string(selectedNumArgs);
    if ((allowRecursion || aliasesProcessed.find(qualifiedName)==aliasesProcessed.end()) &&
        it!=g_aliasInfoMap.end() && selectedNumArgs!=-1 &&
        it->second.find(selectedNumArgs)!=it->second.end()) // expand the alias
    {
      const auto &aliasInfo = it->second.find(selectedNumArgs)->second;
      //printf("is an alias with separator='%s' selectedNumArgs=%d hasArgs=%d!\n",qPrint(aliasInfo.separator),selectedNumArgs,hasArgs);
      if (!allowRecursion) aliasesProcessed.insert(qualifiedName);
      std::string val = aliasInfo.value;
      if (hasArgs)
      {
        //printf("before replaceAliasArguments(val='%s')\n",qPrint(val));
        val = replaceAliasArguments(aliasesProcessed,val,args,aliasInfo.separator);
        //printf("after replaceAliasArguments sep='%s' val='%s' args='%s'\n",
        //       qPrint(aliasInfo.separator),qPrint(val),qPrint(args));
      }
      result += expandAliasRec(aliasesProcessed,val);
      if (!allowRecursion) aliasesProcessed.erase(qualifiedName);
      p = i+l;
      if (hasArgs) p += argsLen+2;
    }
    else // command is not an alias
    {
      //printf("not an alias!\n");
      result += match.str();
      p = i+l;
    }
  }
  result += s.substr(p);
  //printf("expandAliases \"%s\"->\"%s\"\n",qPrint(s),qPrint(result));
  return result;
}


static int countAliasArguments(std::string_view args, std::string_view sep)
{
  int count = 1;
  size_t l = args.length();
  for (size_t i=0;i<l;i++)
  {
    char c = args[i];
    if (!sep.empty() &&
        c==sep[0] && // start with separator character
        (i==0 || args[i-1]!='\\') && // is not escaped
        args.substr(i,sep.length())==sep) // whole separator matches
    {
      count++;
    }
    else if (c=='@' || c=='\\')
    {
      // check if this is the start of another aliased command (see bug704172)
      i += findEndOfCommand(args.substr(i+1));
    }
  }
  //printf("countAliasArguments(%s,sep=%s)=%d\n",qPrint(args),qPrint(sep),count);
  return count;
}

static std::string extractAliasArgs(std::string_view args)
{
  int bc = 0;
  char prevChar = 0;
  if (!args.empty() && args[0]=='{') // alias has argument
  {
    for (size_t i=0;i<args.length();i++)
    {
      char c = args[i];
      if (prevChar!='\\') // not escaped
      {
        if (c=='{') bc++;
        if (c=='}') bc--;
        prevChar=c;
      }
      else
      {
        prevChar=0;
      }

      if (bc==0)
      {
        //printf("extractAliasArgs('%s')->'%s'\n",qPrint(args),qPrint(args.substr(1,i-1)));
        return std::string{args.substr(1,i-1)};
      }
    }
  }
  return std::string{};
}

std::string resolveAliasCmd(std::string_view aliasCmd)
{
  StringUnorderedSet aliasesProcessed;
  //printf("Expanding: '%s'\n",qPrint(aliasCmd));
  std::string result = expandAliasRec(aliasesProcessed,aliasCmd);
  //printf("Expanding result: '%s'->'%s'\n",qPrint(aliasCmd),qPrint(result));
  Debug::print(Debug::Alias,0,"Resolving alias: cmd='{}' result='{}'\n",std::string{aliasCmd},result);
  return result;
}

static std::string expandAlias(std::string_view aliasName,std::string_view aliasValue)
{
  std::string result;
  StringUnorderedSet aliasesProcessed;
  // avoid expanding this command recursively
  aliasesProcessed.insert(std::string{aliasName});
  // expand embedded commands
  //printf("Expanding: '%s'->'%s'\n",qPrint(aliasName),qPrint(aliasValue));
  result = expandAliasRec(aliasesProcessed,aliasValue);
  //printf("Expanding result: '%s'->'%s'\n",qPrint(aliasName),qPrint(result));
  Debug::print(Debug::Alias,0,"Expanding alias: input='{}' result='{}'\n",std::string{aliasValue},result);
  return result;
}

bool isAliasCmd(std::string_view aliasCmd)
{
  return g_aliasInfoMap.find(std::string{aliasCmd}) != g_aliasInfoMap.end();
}
