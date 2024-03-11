// objective: test collapse of inline namespace
// check: 102__hide__inline__namespace_8cpp.xml
// config: HIDE_INLINE_NAMESPACE = YES
// config: EXTRACT_ALL = YES

class Size
{
public:
    int width;
    int height;
};

class Point
{
public:
    float x;
    float y;
};

namespace foo {

/// @brief the official API
struct Image { };

inline namespace _v1 {

/// @brief for compatability
struct Image { };

struct Tensor { };

struct Bar { };

namespace Yoho {
struct Yahoo { };
}
} // inline namespace _v1

inline namespace _v2 {
struct Music { };
} // inline namespace _v2
} // namespace foo

namespace rock {
/// @brief entity
struct Entity { };
} // namespace rock