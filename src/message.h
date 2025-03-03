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

#ifndef MESSAGE_H
#define MESSAGE_H

#include <fmt/core.h>
#include <fmt/compile.h>

#include "types.h"
#include "qcstring.h"

enum class WarningType
{
  Generic,
  Undocumented,
  IncompleteDoc,
  DocError,
  Layout
};

void msg_(fmt::string_view fmt, fmt::format_args args);
void warn_(WarningType type, const QCString &file, int line, fmt::string_view fmt, fmt::format_args args);
void warn_uncond_(fmt::string_view fmt, fmt::format_args args);
void err_(fmt::string_view fmt, fmt::format_args args);
void err_full_(const QCString &file, int line, fmt::string_view fmt, fmt::format_args args);
void term_(fmt::string_view fmt, fmt::format_args args);
QCString warn_line(const QCString &file, int line);
void initWarningFormat();
void warn_flush();
void finishWarnExit();

template<typename ...Args>
void err_fmt(fmt::format_string<Args...> fmt, Args&&... args)
{
  err_(fmt,fmt::make_format_args(args...));
}

template<typename ...Args>
void err_full_fmt(const QCString &file, int line, fmt::format_string<Args...> fmt,Args&&... args)
{
  err_full_(file,line,fmt,fmt::make_format_args(args...));
}

template<typename ...Args>
void term_fmt(fmt::format_string<Args...> fmt, Args&&... args)
{
  term_(fmt,fmt::make_format_args(args...));
}

template<typename ...Args>
void msg_fmt(fmt::format_string<Args...> fmt, Args&&... args)
{
  msg_(fmt,fmt::make_format_args(args...));
}

template<typename ...Args>
void warn_fmt(WarningType type, const QCString &file, int line, fmt::format_string<Args...> fmt,Args&&... args)
{
  warn_(type,file,line,fmt,fmt::make_format_args(args...));
}

template<typename ...Args>
void warn_uncond_fmt(fmt::format_string<Args...> fmt,Args&&... args)
{
  warn_uncond_(fmt,fmt::make_format_args(args...));
}

// N is size including 0-terminal
template<std::size_t N>
constexpr bool has_newline_at_end(const char (&str)[N])
{
  return str[N-2]=='\n';
}

#define msg_no_newline_allowed(x) \
   static_assert(!has_newline_at_end(x),"text: \"" x "\" should not have \\n at end");

#define msg_newline_required(x) \
   static_assert(has_newline_at_end(x),"text: \"" x "\" should have \\n at end");

#define msg(fmt,...)                      \
  msg_fmt(FMT_STRING(fmt),##__VA_ARGS__)

#define warn(file,line,fmt,...) do {                                        \
    msg_no_newline_allowed(fmt);                                            \
    warn_fmt(WarningType::Generic,file,line,FMT_STRING(fmt),##__VA_ARGS__); \
  } while (0)

#define warn_undoc(file,line,fmt,...) do {                                       \
    msg_no_newline_allowed(fmt);                                                 \
    warn_fmt(WarningType::Undocumented,file,line,FMT_STRING(fmt),##__VA_ARGS__); \
  } while (0)

#define warn_incomplete_doc(file,line,fmt,...) do {                               \
    msg_no_newline_allowed(fmt);                                                  \
    warn_fmt(WarningType::IncompleteDoc,file,line,FMT_STRING(fmt),##__VA_ARGS__); \
  } while (0)

#define warn_doc_error(file,line,fmt,...) do {                               \
    msg_no_newline_allowed(fmt);                                             \
    warn_fmt(WarningType::DocError,file,line,FMT_STRING(fmt),##__VA_ARGS__); \
  } while (0)

#define warn_layout(file,line,fmt,...) do {                               \
    msg_no_newline_allowed(fmt);                                             \
    warn_fmt(WarningType::Layout,file,line,FMT_STRING(fmt),##__VA_ARGS__); \
  } while (0)

#define warn_uncond(fmt,...) do {                   \
    msg_newline_required(fmt);                      \
    warn_uncond_fmt(FMT_STRING(fmt),##__VA_ARGS__); \
  } while (0)

#define err(fmt,...) do {                   \
    msg_newline_required(fmt);              \
    err_fmt(FMT_STRING(fmt),##__VA_ARGS__); \
  } while (0)

#define err_full(file,line,fmt,...) do {                   \
    msg_no_newline_allowed(fmt);                             \
    err_full_fmt(file,line,FMT_STRING(fmt),##__VA_ARGS__); \
  } while (0)

#define term(fmt,...) do {                   \
    msg_newline_required(fmt);               \
    term_fmt(FMT_STRING(fmt),##__VA_ARGS__); \
  } while (0)


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
    return formatter<std::string>::format(to_string(prot), ctx);
  }
};

//! adds support for formatting Specifier
template<> struct fmt::formatter<Specifier> : formatter<std::string>
{
  auto format(Specifier spec, format_context& ctx) const {
    return formatter<std::string>::format(to_string(spec), ctx);
  }
};

//! adds support for formatting MethodTypes
template<> struct fmt::formatter<MethodTypes> : formatter<std::string>
{
  auto format(MethodTypes mtype, format_context& ctx) const {
    return formatter<std::string>::format(to_string(mtype), ctx);
  }
};

//! adds support for formatting RelatesType
template<> struct fmt::formatter<RelatesType> : formatter<std::string>
{
  auto format(RelatesType type, format_context& ctx) const {
    return formatter<std::string>::format(to_string(type), ctx);
  }
};

//! adds support for formatting RelationShip
template<> struct fmt::formatter<Relationship> : formatter<std::string>
{
  auto format(Relationship relation, format_context& ctx) const {
    return formatter<std::string>::format(to_string(relation), ctx);
  }
};

//! adds support for formatting SrcLangExt
template<> struct fmt::formatter<SrcLangExt> : formatter<std::string>
{
  auto format(SrcLangExt lang, format_context& ctx) const {
    return formatter<std::string>::format(to_string(lang), ctx);
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

#endif // MESSAGE_H

