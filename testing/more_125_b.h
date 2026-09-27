/** @file */

/** @brief A class declared in a different file than MY_STATIC_CONST. */
class cls
{
  public:
    /** @brief Refers to @ref MY_STATIC_CONST from another file, which does not resolve. */
    void clsFunction();
    void sameFileFunction();
};
