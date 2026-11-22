// objective: resolve member references inside a class with the same name as its C++ module (see issue #11520)
// check: class_log.xml
// input: more_154_module.cppm

export module Log;

/** Logging interface. */
export class Log
{
  public:
    /** Write a message. */
    void write();
    /** Write a message with a severity. */
    void write(int);
    /** See @ref Log::write(int), @ref write(), and the @ref Diagnostics module. */
    bool dirty;
};
