#ifndef GODBOLT_H
#define GODBOLT_H

#include "dstring.h"
#include <optional>

namespace Godbolt
{
DString ClientStateTemplateFor(const DString& godboltClientStateTemplateName);
DString ClientStateFor(const DString& clientStateTemplate, const DString& fragment);
DString UrlFor(const DString& godboltClientStateTemplateName, const DString& fragmentRaw);
std::optional<DString> EmbeddUrlFor(const DString& godboltClientStateTemplateName, const DString& fragmentRaw);
DString Html(const DString& godboltClientStateTemplateName, const DString& fragmentRaw);
}

#endif