// objective: test template parameters for a type alias
// check: 072__using_8cpp.xml

/** \file */
#include <vector>
/** @brief A vector */
template<class T> using Vec = std::vector<T>;
