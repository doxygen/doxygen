#ifndef DOXYGEN_SCANNER_WIDL_H
#define DOXYGEN_SCANNER_WIDL_H

#include "parserintf.h"


class WebIDLOutlineParser : public OutlineParserInterface
{
  public:
    WebIDLOutlineParser();
    ~WebIDLOutlineParser();
    void startTranslationUnit(const char *) {}
    void finishTranslationUnit() {}
    void parseInput(const char *fileName,
                    const char *fileBuf,
                    const std::shared_ptr<Entry> &root,
                    bool sameTranslationUnit,
                    QStrList &filesInSameTranslationUnit);
    bool needsPreprocessing(const QCString &extension) const;
    void parsePrototype(const char *text);

  private:
    struct Private;
    std::unique_ptr<Private> p;

};


#endif
