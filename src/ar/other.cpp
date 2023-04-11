#include <cmath>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "ar.hpp"
#include "opts.hpp"

using std::string;

int main(void) {
  std::string token = "pnts_ArrayAvgSumNumHeelObvs";
  token = "get_clockwise";
  std::string res = ar::do_ar(token);
  std::cout << "result: " << res << std::endl;
  return 0;
}
