// objective: test the \bug, \deprecated, \todo, \test, and \xrefitem commands
// check: class_bug.xml
// check: class_deprecated.xml
// check: class_todo.xml
// check: class_test.xml
// check: class_reminder.xml
// check: bug.xml
// check: deprecated.xml
// check: todo.xml
// check: test.xml
// check: reminders.xml
// config: ALIASES = "reminder=\xrefitem reminders \"Reminder\" \"Reminders\""

/** \bug Class bug. */
class Bug
{
  public:
    /** Description
     *  \bug Function bug
     *  - list item 1 in bug
     *  - list item 2 in bug
     *
     *  More text.
     */
     void foo();
};

/** \deprecated This class is deprecated */
class Deprecated
{
  public:
    /** Do deprecated things.
     *  \deprecated No not use this function anymore. 
     */
    void deprecated();
};

/** \todo This still needs to be done. */
class Todo
{
  public:
    /** \todo more things to do here */
    void todo();
};

/** \test This is part of testing */
class Test
{
  public:
    /** \test more things to test. */
    void test();
};

/** \reminder A reminder */
class Reminder
{
  public:
    /** \reminder Need to rework this before the next release. */
    void reminder();
};


