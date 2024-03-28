#include <string.h>
#include <version.h>

std::string getFullVersion()
{
  static std::string fullVersion;
  static bool init = false;
  if (!init)
  {
    fullVersion = getDoxygenVersion();
    std::string gitVersion  = getGitVersion();
    if (!gitVersion.empty()) fullVersion+=" ("+gitVersion+")";
    init = true;
  }
  return fullVersion;
}
