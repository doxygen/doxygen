// objective: test autolink in various environments
// check: 111__autolink_8cpp.xml
/**
 * @file
 */

/**
 * @brief My define
 */
#define MY_DEFINE 1

/**
 * @brief My int
 */
int my_int;

/**
 * @brief My struct
 */
typedef struct
{
    int a; ///< a
} my_struct_t;

/**
 * @brief My enum
 */
typedef enum
{
    ENUM_A ///< A
} my_enum_t;

/**
 * @brief My foo function
 */
void foo();

/**
 * @brief test function
 *
 * Inline code links everything:
 * `foo(), my_enum_t, my_struct_t, MY_DEFINE, ENUM_A, my_int`
 *
 * Normal text only links to functions and types:
 * foo(), my_enum_t, my_struct_t, MY_DEFINE, ENUM_A, my_int
 *
 * ```c
 * // code block links everything:
 * foo(), my_enum_t, my_struct_t, MY_DEFINE, ENUM_A, my_int
 * ```
 *
 * @see Inside see section also everything is linked:
 * foo(), my_enum_t, my_struct_t, MY_DEFINE, ENUM_A, my_int
 */
void test();
