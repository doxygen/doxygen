#ifndef GODBOLTOPTIONS_H
#define GODBOLTOPTIONS_H

#include "containers.h"

#include <string>
#include <optional>

namespace Godbolt
{
enum class Bridge
{
  link,
  iframe
};

struct Options
{
    Bridge      bridge;
    std::string clientState;
};

std::optional<Options> scanOptions(const std::string &input);
std::optional<Options> scanOptions(const StringVector &input);

} // namespace Godbolt

#endif