// objective: test strong enum values with the same name in different contexts
// check: class_class.xml
// check: classns_1_1_class.xml
// check: classns2_1_1_class.xml

/**
 * \file 058_strong_enum.cpp
 * \brief Main file
 */

namespace ns {

/**
 * \namespace ns
 * \brief A namespace
 */

class Class
{
public:
	enum class Enum1 {
		Flag,
	};

	enum class Enum2 {
		Flag,
	};
};

/**
 * \class Class
 * \brief A class
 */

/**
 * \enum Class::Enum1
 * \brief A first enum class in ns
 *
 * \var Class::Enum1::Flag
 * \brief A flag of Enum1 in ns
 */

/**
 * \enum Class::Enum2
 * \brief A second enum class in ns
 *
 * \var Class::Enum2::Flag
 * \brief A flag of Enum2 in ns
 */

} /* namespace ns */

namespace ns2 {

/**
 * \namespace ns2
 * \brief A namespace
 */
class Class
{
public:
	enum class Enum1 {
		Flag,
	};

	enum class Enum2 {
		Flag,
	};
};

/**
 * \class Class
 * \brief A class
 */

/**
 * \enum Class::Enum1
 * \brief A first enum class in ns2
 *
 * \var Class::Enum1::Flag
 * \brief A flag of Enum1 in ns2
 */

/**
 * \enum Class::Enum2
 * \brief A second enum class in ns2
 *
 * \var Class::Enum2::Flag
 * \brief A flag of Enum2 in ns2
 */

} /* namespace ns2 */


class Class
{
public:
	enum class Enum1 {
		Flag,
	};

	enum class Enum2 {
		Flag,
	};
};

/**
 * \class Class
 * \brief A class
 */

/**
 * \enum Class::Enum1
 * \brief A first enum class in global scope
 *
 * \var Class::Enum1::Flag
 * \brief A flag of Enum1 in global scope
 */

/**
 * \enum Class::Enum2
 * \brief A second enum class in global scope
 *
 * \var Class::Enum2::Flag
 * \brief A flag of Enum2 in global scope
 */
