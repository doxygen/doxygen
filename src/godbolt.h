#ifndef GODBOLT_H
#define GODBOLT_H

#include "dstring.h"
#include <optional>

namespace Godbolt
{
DString ClientStateTemplateFor(const DString& godboltClientStateTemplateName);
DString ClientStateFor(const DString& clientStateTemplate, const DString& fragment);
DString UrlFor(const DString& godboltClientStateTemplateName, const DString& fragmentRaw);
bool CanEmbedd(const DString &godboltClientStateTemplateName);
DString EmbeddUrlFor(const DString& godboltClientStateTemplateName, const DString& fragmentRaw);
DString LinkHtml(const DString& godboltClientStateTemplateName, const DString& fragmentRaw);
DString LinkIFrame(const DString &godboltClientStateTemplateName, const DString &fragmentRaw);
}

#endif