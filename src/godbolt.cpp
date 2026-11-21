
// own header
#include "godbolt.h"

#include "containers.h"
#include "fileinfo.h"
#include "config.h"
#include "util.h"

#include <base64.h>
#include <curl/curl.h>
#include <sstream>
#include <nlohmann/json.hpp>
#include <string>
#include <cctype>
#include <cstdio>
#include <iostream>

namespace Godbolt
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
};

const DString sourceTag = "$$godbolt$$";

namespace
{

//std::string toRison(const nlohmann::json& j) {
//    if (j.is_null()) return "!n";
//    if (j.is_boolean()) return j.get<bool>() ? "!t" : "!f";
//    if (j.is_number()) return j.dump();
//    if (j.is_string()) {
//        const std::string& s = j.get_ref<const std::string&>();
//        std::string out = "'";
//        for (char c : s) {
//            if (c == '\'' || c == '!') out += '!';
//            out += c;
//        }
//        out += '\'';
//        return out;
//    }
//    if (j.is_array()) {
//        std::string out = "!(";
//        for (size_t i = 0; i < j.size(); ++i) {
//            if (i > 0) out += ',';
//            out += toRison(j[i]);
//        }
//        return out + ')';
//    }
//    if (j.is_object()) {
//        std::string out = "(";
//        bool first = true;
//        for (auto& [k, v] : j.items()) {
//            if (!first) out += ',';
//            first = false;
//            // Keys ohne Sonderzeichen brauchen in Rison keine Quotes
//            out += k + ":" + toRison(v);
//        }
//        return out + ')';
//    }
//    return "";
//}
//
//std::string toRison(const std::string_view j) {
//  return toRison(nlohmann::json::parse(j));
//}
//
// Callback-Funktion, um die Server-Antwort in einen std::string zu schreiben
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

std::string TranslateClientStateToEmbeddUrl(const std::string& clientStateJson) {
    CURL* curl = curl_easy_init();
    std::string responseString;

    if (curl) {
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "Accept: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, "https://godbolt.org/api/shortener");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, clientStateJson.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

        CURLcode res = curl_easy_perform(curl);
        
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            std::stringstream ss;
            ss << "curl_easy_perform() failed: " << curl_easy_strerror(res) << '\n';
            return ss.str();
        }
    }

    // Format of answer: {"url:https://godbolt.org"}
    // Extract URL and replace "/z/" with "/e/" embedded
    size_t pos = responseString.find("https://");
    if (pos != std::string::npos) {
        size_t endPos = responseString.find('"', pos);
        std::string url = responseString.substr(pos, endPos - pos);
        
        size_t zPos = url.find("/z/");
        if (zPos != std::string::npos) {
            url.replace(zPos, 3, "/e/");
        }
        return url;
    }

    return "Unkown Error!";
}
}

DString Godbolt::ClientStateTemplateFor(const DString& godboltClientStateTemplateName)
{
  StringVector inputList=Config_getList(INPUT);
  std::set<std::string> scannedDirs;
  for (const auto &i : inputList)
  {
    FileInfo fii(i);
    auto dirPath = fii.dirPath();
    if (auto found = scannedDirs.find(dirPath); found != scannedDirs.end()) continue;
    FileInfo fiDir(dirPath);
    if (!fiDir.exists() || !fiDir.isDir()) continue;
    FileInfo fiTemplate(dirPath + "/" + std::string{godboltClientStateTemplateName.view()});
    if (fiTemplate.exists() || fiTemplate.isFile())
    {
       return fileToString(fiTemplate.absFilePath());
    }
  }
  return defaultClientStateTemplate;
}

DString Godbolt::ClientStateFor(const DString& clientStateTemplate, const DString& fragment)
{
  auto leftSize = clientStateTemplate.find(sourceTag);
  auto midStart = leftSize + sourceTag.size();
  return clientStateTemplate.left(leftSize) + fragment + clientStateTemplate.mid(midStart);
}

DString Godbolt::UrlFor(const DString& godboltClientStateTemplateName, const DString& fragmentRaw)
{
  DString request = "https://godbolt.org/clientstate/";
  auto fragmentJSEncoded = convertSourceToJSString(fragmentRaw.view());
  auto clientStateTemplate = ClientStateTemplateFor(godboltClientStateTemplateName);
  auto clientStateDS = ClientStateFor(clientStateTemplate, fragmentJSEncoded);
  auto clientState = std::string{clientStateDS.view()};
  auto clientState64 = base64_encode(clientState, true);
  return request + clientState64;
}
namespace
{
using json = nlohmann::json;

// 1. Prüft, ob ein Rison-String ohne einfache Anführungszeichen ('...') auskommt
bool isSimpleRisonId(const std::string& s) {
    if (s.empty()) return false;
    // Rison-IDs dürfen nicht mit einer Ziffer oder '-' beginnen
    if (std::isdigit(static_cast<unsigned char>(s[0])) || s[0] == '-') return false;
    for (unsigned char c : s) {
        if (!std::isalnum(c) && c != '_' && c != '.' && c != '/' && c != '~' && c != '-') {
            return false;
        }
    }
    return true;
}

// 2. Serialisiert ein nlohmann::json-Objekt in einen Rison-String
std::string toRison(const json& j) {
    if (j.is_null()) return "!n";
    if (j.is_boolean()) return j.get<bool>() ? "!t" : "!f";
    if (j.is_number()) return j.dump();
    if (j.is_string()) {
        const std::string& s = j.get_ref<const std::string&>();
        if (isSimpleRisonId(s)) return s;
        std::string out = "'";
        for (char c : s) {
            if (c == '\'' || c == '!') out += '!';
            out += c;
        }
        out += '\'';
        return out;
    }
    if (j.is_array()) {
        std::string out = "!(";
        for (size_t i = 0; i < j.size(); ++i) {
            if (i > 0) out += ',';
            out += toRison(j[i]);
        }
        return out + ')';
    }
    if (j.is_object()) {
        std::string out = "(";
        bool first = true;
        for (const auto& [k, v] : j.items()) {
            if (!first) out += ',';
            first = false;
            std::string keyStr = isSimpleRisonId(k) ? k : toRison(json(k));
            out += keyStr + ":" + toRison(v);
        }
        return out + ')';
    }
    return "";
}

// 3. URL-Encoding für den Hash-Teil (Rison-Zeichen wie ':', '(', ')', '!', ',' bleiben erhalten)
std::string urlEncodeRison(const std::string& rison) {
    std::string encoded;
    for (unsigned char c : rison) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~' ||
            c == '!' || c == '\'' || c == '(' || c == ')' || c == ',' || c == ':' || c == '/') {
            encoded += static_cast<char>(c);
        } else if (c == ' ') {
            encoded += "%20";
        } else {
            char buf[4];
            std::snprintf(buf, sizeof(buf), "%%%02X", c);
            encoded += buf;
        }
    }
    return encoded;
}

// 4. Überführt deinen ClientState in die vollständige GoldenLayout-Struktur für /e#
std::string buildEmbedUrlFromClientState(const json& clientState, bool hideToolbars = true) {
    const auto& session = clientState["sessions"][0];
    const auto& execObj = session["executors"][0];
    const auto& compiler = execObj["compiler"];

    int sessionId = session.value("id", 1);
    std::string lang = session.value("language", "c++");
    std::string source = session.value("source", "");
    std::string compilerId = compiler.value("id", "clang_trunk");
    std::string options = compiler.value("options", "");
    json libs = compiler.value("libs", json::array());

    json glState = {
        {"g", json::array({
            {
                {"g", json::array({
                    {
                        {"g", json::array({
                            {
                                {"h", "codeEditor"},
                                {"i", {
                                    {"filename", std::to_string(sessionId)},
                                    {"fontScale", 14},
                                    {"id", sessionId},
                                    {"lang", lang},
                                    {"source", source}
                                }},
                                {"l", "5"},
                                {"n", "0"},
                                {"o", "C++ source #" + std::to_string(sessionId)},
                                {"t", "0"}
                            }
                        })},
                        {"k", 50},
                        {"l", "4"},
                        {"n", "0"},
                        {"o", ""},
                        {"s", 0},
                        {"t", "0"}
                    },
                    {
                        {"g", json::array({
                            {
                                {"h", "executor"},
                                {"i", {
                                    {"argsPanelShown", false},
                                    {"compilationPanelShown", false},
                                    {"compiler", compilerId},
                                    {"compilerOutShown", false},
                                    {"execArgs", execObj.value("arguments", "")},
                                    {"execStdin", execObj.value("stdin", "")},
                                    {"fontScale", 14},
                                    {"lang", lang},
                                    {"libs", libs},
                                    {"options", options},
                                    {"overrides", json::array()},
                                    {"runtimeTools", json::array()},
                                    {"source", sessionId},
                                    {"stdinPanelShown", false},
                                    {"wrap", false}
                                }},
                                {"l", "5"},
                                {"n", "0"},
                                {"o", "Executor " + compilerId},
                                {"t", "0"}
                            }
                        })},
                        {"k", 50},
                        {"l", "4"},
                        {"m", 100},
                        {"n", "0"},
                        {"o", ""},
                        {"s", 0},
                        {"t", "0"}
                    }
                })},
                {"l", "2"},
                {"n", "0"},
                {"o", ""},
                {"t", "0"}
            }
        })},
        {"version", 4}
    };

    if (hideToolbars) {
        glState["hideEditorToolbars"] = true;
    }

    // Äußere Rison-Klammern '(' und ')' für das Root-Objekt entfernen
    std::string risonBody = toRison(glState);
    if (risonBody.size() >= 2 && risonBody.front() == '(' && risonBody.back() == ')') {
        risonBody = risonBody.substr(1, risonBody.size() - 2);
    }

    std::string baseUrl = "https://godbolt.org";
    baseUrl += "/e#";
    return baseUrl + urlEncodeRison(risonBody);
}

}

DString Godbolt::EmbeddUrlFor(const DString& godboltClientStateTemplateName, const DString& fragmentRaw)
{
   // Raw-String-Literal verwenden, damit keine doppelten Backslashes (\\n) entstehen
    std::string cppSource = R"(#include <stdio.h>
int main() {
  printf("Hello World\n");
}
)";

    json state = {
        {"sessions", {{
            {"id", 1},
            {"language", "c++"},
            {"source", cppSource},
            {"executors", {{
                {"compiler", {
                    {"id", "clang_trunk"},
                    {"libs", json::array()},
                    {"options", "-std=c++20"}
                }}
            }}}
        }}}
    };

  std::cout << '\n';
  std::cout << buildEmbedUrlFromClientState(state, true) << '\n';
  auto s = "https://godbolt.org/e#g:!((g:!((g:!((h:codeEditor,i:(filename:'1',fontScale:14,id:1,lang:'c%2B%2B',source:'%23include%20%3Cstdio.h%3E%0Aint%20main()%20%7B%0A%20%20printf(%22Hello%20World%5Cn%22)%3B%0A%7D%0A'),l:'5',n:'0',o:'C%2B%2B%20source%20%231',t:'0')),k:50,l:'4',n:'0',o:'',s:0,t:'0'),(g:!((h:executor,i:(argsPanelShown:!f,compilationPanelShown:!f,compiler:clang_trunk,compilerOutShown:!f,execArgs:'',execStdin:'',fontScale:14,lang:'c%2B%2B',libs:!(),options:'-std%3Dc%2B%2B20',overrides:!(),runtimeTools:!(),source:1,stdinPanelShown:!f,wrap:!f),l:'5',n:'0',o:'Executor%20clang_trunk',t:'0')),k:50,l:'4',m:100,n:'0',o:'',s:0,t:'0')),l:'2',n:'0',o:'',t:'0')),hideEditorToolbars:!t,version:4";
  std::cout << s << "\n";

  return s;
}


