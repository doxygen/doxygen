#ifndef CLANGPARSER_H
#define CLANGPARSER_H

#include <qcstring.h>

class OutputList;
class FileDef;

class ClangParser
{
  public:
    static ClangParser *instance();
    void start(const char *fileName);
    void finish();
    QCString lookup(uint line,const char *symbol);
    void writeSources(OutputList &ol,FileDef *fd);

  private:
    void linkIdentifier(OutputList &ol,FileDef *fd,
                        uint &line,uint &column,
                        const char *text,int tokenIndex);
    void linkMacro(OutputList &ol,FileDef *fd,
                   uint &line,uint &column,
                   const char *text);
    void linkInclude(OutputList &ol,FileDef *fd,
                   uint &line,uint &column,
                   const char *text);
    class Private;
    Private *p;
    ClangParser();
    virtual ~ClangParser();
    static ClangParser *s_instance;
};

#endif
