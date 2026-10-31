// objective: test that members of an anonymous union in a private section of a class are private, see issue #12395
// check: classcls.xml
// config: EXTRACT_PRIVATE = YES
// config: WARN_IF_UNDOCUMENTED = NO

/** A class. */
class cls
{
  private:
    unsigned long long desc_;
    union
    {
      const int values_;
      const int args_;
    };
};
