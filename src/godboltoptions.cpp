
// own header
#include "godboltoptions.h"

#include <scn/scan.h>

using namespace Godbolt;
namespace
{
namespace tag
{
} // namespace tag

} // namespace
std::optional<Options> Godbolt::ScanOptions(const std::string &input)
{
  static const std::string iframe = "iframe";
  static const std::string link   = "link";
  if (auto result = scn::scan<std::string, std::string>(input, "godbolt={:[^;]};{:[^)]}"))
  {
    auto [b, cs] = result->values();
    return { { b == iframe ? Bridge::iframe : Bridge::link, cs } };
  }
  else if (auto x = scn::scan<std::string>(input, "godbolt={:[^,]}"))
  {
    if (x->value() == iframe)
      return { { Bridge::iframe, "godbolt" } };
    else if (x->value() == link)
      return { { Bridge::link, "godbolt" } };
    else
      return { { Bridge::link, x->value() } };
  }
  else if (input.find("godbolt") == 0u)
  {
    return { { Bridge::link, "godbolt" } };
  }
  return {};
}
