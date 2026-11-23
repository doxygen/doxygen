#ifndef GODBOLTOPTIONS_H
#define GODBOLTOPTIONS_H

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

std::optional<Options> ScanOptions(const std::string &input);

} // namespace Godbolt

#endif