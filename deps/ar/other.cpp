#include <cmath>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "ar.hpp"
#include "opts.hpp"

using std::string;

int main(void) {
  std::string token = "pntarrayavgsumnumheelobvs";
  std::cout << ar::do_ar(token) << std::endl;
  return 0;
}
