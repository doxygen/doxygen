#include "doxygen.h"

/*! \file
 *  \brief main entry point for doxygen
 *
 *  This file contains main()
 */

/*! Default main. The idea of separating this from the rest of doxygen,
 *  is to make it possible to write your own main, with a different 
 *  generateOutput() function for instance.
 */
int main(int argc,char **argv)
{
  initDoxygen();
  readConfiguration(argc,argv);
  parseInput();
  generateOutput();
  return 0;
}
