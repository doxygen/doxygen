#include <string.h>
#include <version.h>

const char *getFullVersion(void)
{
#define BUF_SIZE 100
  static char fullVersionString[BUF_SIZE];
  static bool init = false;
  if (!init)
  {
    strncpy(fullVersionString,getDoxygenVersion(),BUF_SIZE-1);
    if (strlen(getGitVersion())>0)
    {
      strncat(fullVersionString," (",BUF_SIZE-1);
      strncat(fullVersionString,getGitVersion(),BUF_SIZE-1);
      strncat(fullVersionString,")",BUF_SIZE-1);
    }
    fullVersionString[BUF_SIZE-1]='\0';
    init = true;
  }
  return fullVersionString;
}
