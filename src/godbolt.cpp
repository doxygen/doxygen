
// own header
#include "godbolt.h"

#include "containers.h"
#include "fileinfo.h"
#include "config.h"
#include "util.h"

#include <base64.h>
#include <sstream>
#include <nlohmann/json.hpp>
#include <string>
#include <cctype>
#include <cstdio>
#include <iostream>

namespace
{
const DString defaultClientStateTemplate = R"json({
  "sessions": [
    {
      "id": 1,
      "language": "c++",
      "source": "$$godbolt$$",
      "compilers": [],
      "executors": [
        {
          "compiler": {
            "id": "clang_trunk",
            "libs": [],
            "options": "-std=c++20"
          }
        }
      ]
    }
  ]
}
)json";

const DString sourceTag                  = "$$godbolt$$";
} // namespace

DString Godbolt::ClientStateTemplateFor(const DString &godboltClientStateTemplateName)
{
  StringVector          inputList = Config_getList(INPUT);
  std::set<std::string> scannedDirs;
  for (const auto &i : inputList)
  {
    FileInfo fii(i);
    auto     dirPath = fii.dirPath();
    if (auto found = scannedDirs.find(dirPath); found != scannedDirs.end()) continue;
    FileInfo fiDir(dirPath);
    if (!fiDir.exists() || !fiDir.isDir()) continue;
    FileInfo fiTemplate(dirPath + "/" + std::string{ godboltClientStateTemplateName.view() });
    if (fiTemplate.exists() || fiTemplate.isFile())
    {
      return fileToString(fiTemplate.absFilePath());
    }
  }
  return defaultClientStateTemplate;
}

DString Godbolt::ClientStateFor(const DString &clientStateTemplate, const DString &fragment)
{
  auto leftSize = clientStateTemplate.find(sourceTag);
  auto midStart = leftSize + sourceTag.size();
  return clientStateTemplate.left(leftSize) + fragment + clientStateTemplate.mid(midStart);
}

DString Godbolt::UrlFor(const DString &godboltClientStateTemplateName, const DString &fragmentRaw)
{
  DString request             = "https://godbolt.org/clientstate/";
  auto    fragmentJSEncoded   = convertSourceToJSString(fragmentRaw.view());
  auto    clientStateTemplate = ClientStateTemplateFor(godboltClientStateTemplateName);
  auto    clientStateDS       = ClientStateFor(clientStateTemplate, fragmentJSEncoded);
  auto    clientState         = std::string{ clientStateDS.view() };
  auto    clientState64       = base64_encode(clientState, true);
  return request + clientState64;
}
namespace
{
using json = nlohmann::json;

// 1. Prüft, ob ein Rison-String ohne einfache Anführungszeichen ('...') auskommt
bool isSimpleRisonId(const std::string &s)
{
  if (s.empty()) return false;
  // Rison-IDs dürfen nicht mit einer Ziffer oder '-' beginnen
  if (std::isdigit(static_cast<unsigned char>(s[0])) || s[0] == '-') return false;
  for (unsigned char c : s)
  {
    if (!std::isalnum(c) && c != '_' && c != '.' && c != '/' && c != '~' && c != '-')
    {
      return false;
    }
  }
  return true;
}

// 2. Serialisiert ein nlohmann::json-Objekt in einen Rison-String
std::string toRison(const json &j)
{
  if (j.is_null()) return "!n";
  if (j.is_boolean()) return j.get<bool>() ? "!t" : "!f";
  if (j.is_number()) return j.dump();
  if (j.is_string())
  {
    const std::string &s = j.get_ref<const std::string &>();
    if (isSimpleRisonId(s)) return s;
    std::string out = "'";
    for (char c : s)
    {
      if (c == '\'' || c == '!') out += '!';
      out += c;
    }
    out += '\'';
    return out;
  }
  if (j.is_array())
  {
    std::string out = "!(";
    for (size_t i = 0; i < j.size(); ++i)
    {
      if (i > 0) out += ',';
      out += toRison(j[i]);
    }
    return out + ')';
  }
  if (j.is_object())
  {
    std::string out   = "(";
    bool        first = true;
    for (const auto &[k, v] : j.items())
    {
      if (!first) out += ',';
      first              = false;
      std::string keyStr = isSimpleRisonId(k) ? k : toRison(json(k));
      out += keyStr + ":" + toRison(v);
    }
    return out + ')';
  }
  return "";
}

// 3. URL-Encoding für den Hash-Teil (Rison-Zeichen wie ':', '(', ')', '!', ',' bleiben erhalten)
std::string urlEncodeRison(const std::string &rison)
{
  std::string encoded;
  for (unsigned char c : rison)
  {
    if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~' ||
        c == '!' || c == '\'' || c == '(' || c == ')' || c == ',' || c == ':' || c == '/')
    {
      encoded += static_cast<char>(c);
    }
    else if (c == ' ')
    {
      encoded += "%20";
    }
    else
    {
      char buf[4];
      std::snprintf(buf, sizeof(buf), "%%%02X", c);
      encoded += buf;
    }
  }
  return encoded;
}

// 4. Überführt deinen ClientState in die vollständige GoldenLayout-Struktur für /e#
std::string buildEmbedUrlFromClientState(const json &clientState, bool hideToolbars = true)
{
  const auto &session    = clientState["sessions"][0];
  int         sessionId  = session.value("id", 1);
  std::string lang       = session.value("language", "c++");
  std::string source     = session.value("source", "");

  const auto &execObj    = session["executors"][0];

  const auto &compiler   = execObj["compiler"];
  std::string compilerId = compiler.value("id", "clang_trunk");
  std::string options    = compiler.value("options", "");
  json        libs       = compiler.value("libs", json::array());

  json        glState    = {
    //
    { "g", json::array({
             //
             {
               //
               { "g", json::array({
                        //
                        {
                          //
                          { "g", json::array({
                                   //
                                   {
                                     //
                                     { "h", "codeEditor" }, //
                                     { "i", {
                                              //
                                              { "filename", std::to_string(sessionId) },  //
                                              { "fontScale", 14 },                        //
                                              { "id", sessionId },                        //
                                              { "lang", lang },                           //
                                              { "source", source }                        //
                                            } },                                          //
                                     { "l", "5" },                                        //
                                     { "n", "0" },                                        //
                                     { "o", "C++ source #" + std::to_string(sessionId) }, //
                                     { "t", "0" }                                         //
                                   } //
                                 }) },  //
                          { "k", 50 },  //
                          { "l", "4" }, //
                          { "n", "0" }, //
                          { "o", "" },  //
                          { "s", 0 },   //
                          { "t", "0" }  //
                        },              //
                        {
                          //
                          { "g", json::array({
                                   //
                                   {
                                     //
                                     { "h", "executor" }, //
                                     { "i", {
                                              //
                                              { "argsPanelShown", false },                    //
                                              { "compilationPanelShown", false },             //
                                              { "compiler", compilerId },                     //
                                              { "compilerOutShown", false },                  //
                                              { "execArgs", execObj.value("arguments", "") }, //
                                              { "execStdin", execObj.value("stdin", "") },    //
                                              { "fontScale", 14 },                            //
                                              { "lang", lang },                               //
                                              { "libs", libs },                               //
                                              { "options", options },                         //
                                              { "overrides", json::array() },                 //
                                              { "runtimeTools", json::array() },              //
                                              { "source", sessionId },                        //
                                              { "stdinPanelShown", false },                   //
                                              { "wrap", false }                               //
                                            } },                                              //
                                     { "l", "5" },                                            //
                                     { "n", "0" },                                            //
                                     { "o", "Executor " + compilerId },                       //
                                     { "t", "0" }                                             //
                                   } //
                                 }) },  //
                          { "k", 50 },  //
                          { "l", "4" }, //
                          { "m", 100 }, //
                          { "n", "0" }, //
                          { "o", "" },  //
                          { "s", 0 },   //
                          { "t", "0" }  //
                        } //
                      }) },  //
               { "l", "2" }, //
               { "n", "0" }, //
               { "o", "" },  //
               { "t", "0" }  //
             } //
           }) },     //
    { "version", 4 } //
  }; //

  if (hideToolbars)
  {
    glState["hideEditorToolbars"] = true;
  }

  // Äußere Rison-Klammern '(' und ')' für das Root-Objekt entfernen
  std::string risonBody = toRison(glState);
  if (risonBody.size() >= 2 && risonBody.front() == '(' && risonBody.back() == ')')
  {
    risonBody = risonBody.substr(1, risonBody.size() - 2);
  }

  std::string baseUrl = "https://godbolt.org";
  baseUrl += "/e#";
  return baseUrl + urlEncodeRison(risonBody);
}

} // namespace

std::optional<DString> Godbolt::EmbeddUrlFor(const DString &godboltClientStateTemplateName, const DString &fragmentRaw)
{
  DString clientStateTemplateRaw = ClientStateTemplateFor(godboltClientStateTemplateName);
  auto    clientState            = json::parse(clientStateTemplateRaw.c_str());
  if (clientState["sessions"].size() > 1u)
    return {};
  auto &const session0 = clientState["sessions"][0];
  if (!session0.contains("executors") || session0["executors"].size() != 1u)
    return {};
  clientState["sessions"][0]["source"] = fragmentRaw.c_str();
  return buildEmbedUrlFromClientState(clientState, true);
}

DString Godbolt::Html(const DString &godboltClientStateTemplateName, const DString &fragment)
{
  if (auto embeddUrl = Godbolt::EmbeddUrlFor(godboltClientStateTemplateName, fragment))
  {
    return "<iframe width=\"100%\" height=\"350px\" src=\"" + *embeddUrl + "\"></iframe>";
  }
  else
  {
    return "<a href=\"" + UrlFor(godboltClientStateTemplateName, fragment) + "\">Godbolt</a>";
  }
}
