// objective: test strong enum values with the same name in different contexts
// check: class_class.xml
// check: classns_1_1_class.xml
// check: classns2_1_1_class.xml
// check: 058__strong__enum_8cpp.xml
// tagfile: $INPUTDIR/tagfile_058.tag

/**
 * \file 058_strong_enum.cpp
 * \brief Main file
 *
 * Links to local enums:
 *
 * - \ref WeakEnum
 * - \ref WeakEnumFlag
 * - \ref Enum1
 * - \ref Enum1::Flag
 * - \ref ns::WeakEnum
 * - \ref ns::WeakEnumFlag
 * - \ref ns::Enum1
 * - \ref ns::Enum1::Flag
 * - \ref ns::Class::WeakEnum
 * - \ref ns::Class::WeakEnumFlag
 * - \ref ns::Class::Enum2
 * - \ref ns::Class::Enum2::Flag
 * - \ref Class::WeakEnum
 * - \ref Class::WeakEnumFlag
 * - \ref Class::Enum2
 * - \ref Class::Enum2::Flag
 *
 * Links to enums in a tagfile:
 *
 * - \ref ExternalWeakEnum
 * - \ref ExternalWeakEnumFlag
 * - \ref ExternalEnum1
 * - \ref ExternalEnum1::Flag
 * - \ref external::ns::WeakEnum
 * - \ref external::ns::WeakEnumFlag
 * - \ref external::ns::Enum1
 * - \ref external::ns::Enum1::Flag
 * - \ref external::ns::Class::WeakEnum
 * - \ref external::ns::Class::WeakEnumFlag
 * - \ref external::ns::Class::Enum2
 * - \ref external::ns::Class::Enum2::Flag
 * - \ref ExternalClass::WeakEnum
 * - \ref ExternalClass::WeakEnumFlag
 * - \ref ExternalClass::Enum1
 * - \ref ExternalClass::Enum1::Flag
 */

namespace ns {

/**
 * \namespace ns
 * \brief A namespace
 */

enum WeakEnum {
  WeakEnumFlag
};

/**
 * \enum WeakEnum
 * \brief A weak enum in ns
 *
 * \var WeakEnumFlag
 * \brief A flag of WeakEnum in ns
 */

enum class Enum1 {
	Flag
};

/**
 * \enum Enum1
 * \brief A first enum class in ns
 *
 * \var Enum1::Flag
 * \brief A flag of Enum1 in ns
 */

class Class
{
public:
	enum WeakEnum {
	  WeakEnumFlag
	};

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
 * \enum Class::WeakEnum
 * \brief A weak enum in a class in ns
 *
 * \var Class::WeakEnumFlag
 * \brief A flag of WeakEnum in a class in ns
 */

/**
 * \enum Class::Enum1
 * \brief A first enum class in a class in ns
 *
 * \var Class::Enum1::Flag
 * \brief A flag of Enum1 in a class in ns
 */

/**
 * \enum Class::Enum2
 * \brief A second enum class in a class in ns
 *
 * \var Class::Enum2::Flag
 * \brief A flag of Enum2 in a class in ns
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
 * \brief A first enum class in a class in ns2
 *
 * \var Class::Enum1::Flag
 * \brief A flag of Enum1 in a class in ns2
 */

/**
 * \enum Class::Enum2
 * \brief A second enum class in a class in ns2
 *
 * \var Class::Enum2::Flag
 * \brief A flag of Enum2 in a class in ns2
 */

} /* namespace ns2 */

enum WeakEnum {
  WeakEnumFlag
};

/**
 * \enum WeakEnum
 * \brief A weak enum in global scope
 *
 * \var WeakEnumFlag
 * \brief A flag of WeakEnum in global scope
 */

enum class Enum1 {
	Flag
};

/**
 * \enum Enum1
 * \brief A first enum class in global scope
 *
 * \var Enum1::Flag
 * \brief A flag of Enum1 in global scope
 */

class Class
{
public:
	enum WeakEnum {
	  WeakEnumFlag
	};

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
 * \enum Class::WeakEnum
 * \brief A weak enum in a class in global scope
 *
 * \var Class::WeakEnumFlag
 * \brief A flag of WeakEnum in a class in global scope
 */

/**
 * \enum Class::Enum1
 * \brief A first enum class in a class in global scope
 *
 * \var Class::Enum1::Flag
 * \brief A flag of Enum1 in a class in global scope
 */

/**
 * \enum Class::Enum2
 * \brief A second enum class in a class in global scope
 *
 * \var Class::Enum2::Flag
 * \brief A flag of Enum2 in a class in global scope
 */
