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

#ifndef TRACE_H
#define TRACE_H

#ifndef ENABLE_TRACING
#ifndef NDEBUG
#define ENABLE_TRACING 1
#else
#define ENABLE_TRACING 0
#endif
#endif

// Since some modules produce a huge amount of tracing we disable those traces by default.
// Set of or more of the following to 1 to enable the relevant tracing
#define ENABLE_SYMBOLRESOLVER_TRACING 0
#define ENABLE_MARKDOWN_TRACING       0
#define ENABLE_DOCPARSER_TRACING      0


#if ENABLE_TRACING
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE // debug build
#else
#define SPELOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO  // release build (hide trace/debug levels)
#endif

#pragma push_macro("warn")
#undef warn
#include "spdlog/spdlog.h"
#pragma pop_macro("warn")

#include "types.h"
#include "qcstring.h"
#include "construct.h"

extern std::shared_ptr<spdlog::logger> g_tracer;

void initTracing(const QCString &logFile, bool timing);
void exitTracing();

namespace Trace
{
  inline QCString trunc(const QCString &s,size_t numChars=15)
  {
    QCString result;
    size_t i=0;
    for (; i<numChars && i<s.length(); i++)
    {
      char c=s.at(i);
      if      (c=='\n') { result+="\\n"; }
      else if (c=='\t') { result+="\\t"; }
      else if (c=='\r') { result+="\\r"; }
      else if (c=='\\') { result+="\\\\"; }
      else result+=c;
    }
    if (i<s.length()) result+="...";
    return result;
  }
}

#if ENABLE_TRACING
#define TRACE(...) if (g_tracer) SPDLOG_LOGGER_TRACE(g_tracer,__VA_ARGS__)
#else
#define TRACE(...) (void)0
#endif

/** Helper class to trace an entry statement at creation and another one at destruction. */
class AutoTrace
{
  public:
    explicit AutoTrace(spdlog::source_loc loc) : m_loc(loc)
    {
      if (g_tracer)
      {
        g_tracer->log(m_loc,spdlog::level::trace,">");
      }
    }
    template<typename... Args>
    explicit AutoTrace(spdlog::source_loc loc,
                       const std::string &fmt, Args&&...args) : m_loc(loc)
    {
      if (g_tracer)
      {
        if (fmt.empty())
        {
          g_tracer->log(m_loc,spdlog::level::trace,">");
        }
        else
        {
          g_tracer->log(m_loc,spdlog::level::trace,"> "+fmt,std::forward<Args>(args)...);
        }
      }
    }
   ~AutoTrace()
    {
      if (g_tracer)
      {
        if (m_exitMessage.empty())
        {
          g_tracer->log(m_loc,spdlog::level::trace,"<");
        }
        else
        {
          g_tracer->log(m_loc,spdlog::level::trace,"< "+m_exitMessage);
        }
      }
    }
    NON_COPYABLE(AutoTrace)

    template<typename... Args>
    void add(spdlog::source_loc loc,
             const std::string &fmt, Args&&...args)
    {
      if (g_tracer)
      {
        g_tracer->log(loc,spdlog::level::trace,": "+fmt,std::forward<Args>(args)...);
      }
    }
    template<typename... Args>
    void setExit(const std::string &msg,Args&&...args)
    {
      m_exitMessage = fmt::format(msg,std::forward<Args>(args)...);
    }
  private:
   spdlog::source_loc m_loc;
   std::string m_exitMessage;
};

#if ENABLE_TRACING
#define AUTO_TRACE(...)      AutoTrace trace_{spdlog::source_loc{__FILE__,__LINE__,SPDLOG_FUNCTION},__VA_ARGS__}
#define AUTO_TRACE_ADD(...)  trace_.add(spdlog::source_loc{__FILE__,__LINE__,SPDLOG_FUNCTION},__VA_ARGS__)
#define AUTO_TRACE_EXIT(...) trace_.setExit(__VA_ARGS__)
#else
#define AUTO_TRACE(...)      (void)0
#define AUTO_TRACE_ADD(...)  (void)0
#define AUTO_TRACE_EXIT(...) (void)0
#endif

#ifdef DOXYGEN_ONLY
namespace fmt { template<typename T> struct formatter {}; }
#endif

//! adds support for formatting QCString
template<> struct fmt::formatter<QCString> : formatter<std::string>
{
  auto format(const QCString &c, format_context& ctx) const {
    return formatter<std::string>::format(c.str(), ctx);
  }
};

//! adds support for formatting Protected
template<> struct fmt::formatter<Protection> : formatter<std::string>
{
  auto format(Protection prot, format_context& ctx) const {
    std::string result="Unknown";
    switch (prot)
    {
      case Protection::Public:    result="Public";     break;
      case Protection::Protected: result="Protected";  break;
      case Protection::Private:   result="Private";    break;
      case Protection::Package:   result="Package";    break;
    }
    return formatter<std::string>::format(result, ctx);
  }
};

//! adds support for formatting Specifier
template<> struct fmt::formatter<Specifier> : formatter<std::string>
{
  auto format(Specifier spec, format_context& ctx) const {
    std::string result="Unknown";
    switch (spec)
    {
      case Specifier::Normal:   result="Normal";  break;
      case Specifier::Virtual:  result="Virtual"; break;
      case Specifier::Pure:     result="Pure";    break;
    }
    return formatter<std::string>::format(result, ctx);
  }
};

//! adds support for formatting MethodTypes
template<> struct fmt::formatter<MethodTypes> : formatter<std::string>
{
  auto format(MethodTypes mtype, format_context& ctx) const {
    std::string result="Unknown";
    switch (mtype)
    {
      case MethodTypes::Method:   result="Method";   break;
      case MethodTypes::Signal:   result="Signal";   break;
      case MethodTypes::Slot:     result="Slot";     break;
      case MethodTypes::DCOP:     result="DCOP";     break;
      case MethodTypes::Property: result="Property"; break;
      case MethodTypes::Event:    result="Event";    break;
    }
    return formatter<std::string>::format(result, ctx);
  }
};

//! adds support for formatting RelatesType
template<> struct fmt::formatter<RelatesType> : formatter<std::string>
{
  auto format(RelatesType type, format_context& ctx) const {
    std::string result="Unknown";
    switch (type)
    {
      case RelatesType::Simple:    result="Simple";    break;
      case RelatesType::Duplicate: result="Duplicate"; break;
      case RelatesType::MemberOf:  result="MemberOf";  break;
    }
    return formatter<std::string>::format(result, ctx);
  }
};

//! adds support for formatting RelationShip
template<> struct fmt::formatter<Relationship> : formatter<std::string>
{
  auto format(Relationship relation, format_context& ctx) const {
    std::string result="Unknown";
    switch (relation)
    {
      case Relationship::Member:  result="Member";  break;
      case Relationship::Related: result="Related"; break;
      case Relationship::Foreign: result="Foreign"; break;
    }
    return formatter<std::string>::format(result, ctx);
  }
};

//! adds support for formatting SrcLangExt
template<> struct fmt::formatter<SrcLangExt> : formatter<std::string>
{
  auto format(SrcLangExt lang, format_context& ctx) const {
    std::string result="Unknown";
    switch (lang)
    {
      case SrcLangExt::Unknown:  result="Unknown";     break;
      case SrcLangExt::IDL:      result="IDL";         break;
      case SrcLangExt::Java:     result="Java";        break;
      case SrcLangExt::CSharp:   result="C#";          break;
      case SrcLangExt::D:        result="D";           break;
      case SrcLangExt::PHP:      result="PHP";         break;
      case SrcLangExt::ObjC:     result="Objective-C"; break;
      case SrcLangExt::Cpp:      result="C++";         break;
      case SrcLangExt::JS:       result="Javascript";  break;
      case SrcLangExt::Python:   result="Python";      break;
      case SrcLangExt::Fortran:  result="Fortran";     break;
      case SrcLangExt::VHDL:     result="VHDL";        break;
      case SrcLangExt::XML:      result="XML";         break;
      //case SrcLangExt::Tcl:    result="Tcl";         break;
      case SrcLangExt::Markdown: result="Markdown";    break;
      case SrcLangExt::SQL:      result="SQL";         break;
      case SrcLangExt::Slice:    result="Slice";       break;
      case SrcLangExt::Lex:      result="Lex";         break;
    }
    return formatter<std::string>::format(result, ctx);
  }
};

//! adds support for formatting MemberType
template<> struct fmt::formatter<MemberType> : formatter<std::string>
{
  auto format(MemberType mtype, format_context& ctx) const {
    std::string result="Unknown";
    switch (mtype)
    {
      case MemberType::Define:      result="Define";      break;
      case MemberType::Function:    result="Function";    break;
      case MemberType::Variable:    result="Variable";    break;
      case MemberType::Typedef:     result="Typedef";     break;
      case MemberType::Enumeration: result="Enumeration"; break;
      case MemberType::EnumValue:   result="EnumValue";   break;
      case MemberType::Signal:      result="Signal";      break;
      case MemberType::Slot:        result="Slot";        break;
      case MemberType::Friend:      result="Friend";      break;
      case MemberType::DCOP:        result="DCOP";        break;
      case MemberType::Property:    result="Property";    break;
      case MemberType::Event:       result="Event";       break;
      case MemberType::Interface:   result="Interface";   break;
      case MemberType::Service:     result="Service";     break;
      case MemberType::Sequence:    result="Sequence";    break;
      case MemberType::Dictionary:  result="Dictionary";  break;
    }
    return formatter<std::string>::format(result, ctx);
  }
};

//! adds support for formatting TypeSpecifier
template<> struct fmt::formatter<TypeSpecifier> : formatter<std::string>
{
  auto format(TypeSpecifier type, format_context& ctx) const {
    return formatter<std::string>::format(type.to_string(),ctx);
  }
};

//! adds support for formatting EntryType
template<> struct fmt::formatter<EntryType> : formatter<std::string>
{
  auto format(EntryType type, format_context& ctx) const {
    return formatter<std::string>::format(type.to_string(),ctx);
  }
};

//! adds support for formatting MemberListType
template<> struct fmt::formatter<MemberListType> : formatter<std::string>
{
  auto format(MemberListType type, format_context& ctx) const {
    return formatter<std::string>::format(type.to_string(),ctx);
  }
};


#endif // TRACE_H
