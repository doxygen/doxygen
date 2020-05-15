#include <string.h>
#include <version.h>

const char *getFullVersion(void)
{
#define BUF_SIZE 100
  static char fullVersionString[BUF_SIZE];
  static bool init = false;
  if (!init)
  {
    strlcpy(fullVersionString,getDoxygenVersion(),BUF_SIZE);
    if (strlen(getGitVersion())>0)
    {
      strlcat(fullVersionString," (",BUF_SIZE);
      strlcat(fullVersionString,getGitVersion(),BUF_SIZE);
      strlcat(fullVersionString,")",BUF_SIZE);
    }
    fullVersionString[BUF_SIZE-1]='\0';
    init = true;
  }
  return fullVersionString;
}
