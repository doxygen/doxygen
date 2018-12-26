// objective: test that namespace members are not put to file docs by default
// check: 077__no__xml__namespace__members__in__file__scope_8h.xml

namespace Namespace {

/**
@brief A function

Detailed documentation.
*/
void foo();

/** @brief An enum */
enum class Enum {};

}
