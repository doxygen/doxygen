// objective: test the \(public|protected|private)section commands
// check: struct_s.xml

/** A struct */
struct S
{
  /** \publicsection */

  /** public field */
  int pub1;
  /** another public field */
  int pub2;

  /** \protectedsection */

  /** protected field */
  int pro1;
  /** another protected field */
  int pro2;

  /** \privatesection */

  /** private field */
  int pri1;
  /** another private field */
  int pri2;
};

