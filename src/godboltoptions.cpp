

#include "godboltoptions.h"

#include "stringutil.h"

#include <scn/scan.h>

using namespace Godbolt;


namespace
{
static const std::string iframe = "iframe";
static const std::string link   = "link";
} // namespace

namespace
{
Options interpretOne(const std::string &value)
{
  if (value == iframe)
    return { Bridge::iframe, "godbolt" };
  else if (value == link)
    return { Bridge::link, "godbolt" };
  else
    return { Bridge::link, value };
}
} // namespace

std::optional<Options> Godbolt::scanOptions(const std::string &input)
{
  if (auto result = scn::scan<std::string, std::string>(input, "godbolt=({:[^;]};{:[^)]})"))
  {
    auto [b, cs] = result->values();
    return { { b == iframe ? Bridge::iframe : Bridge::link, cs } };
  }
  if (auto x = scn::scan<std::string>(input, "godbolt=({:[^)]})"))
  {
    return interpretOne(x->value());
  }
  if (auto x = scn::scan<std::string>(input, "godbolt={}"))
  {
    return interpretOne(x->value());
  }
  if (input.find("godbolt") == 0u)
  {
    return { { Bridge::link, "godbolt" } };
  }
  return {};
}

std::optional<Options> Godbolt::scanOptions(const StringVector &input)
{
  for (const auto &opt : input)
    if (auto found = scanOptions(std::string{ stripWhiteSpace(opt) }))
      return found;
  return {};
}