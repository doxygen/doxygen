#!/usr/bin/env python
# python script to generate an overview of the states based on the input lex file.
#
# Copyright (C) 1997-2019 by Dimitri van Heesch.
#
# Permission to use, copy, modify, and distribute this software and its
# documentation under the terms of the GNU General Public License is hereby
# granted. No representations are made about the suitability of this software
# for any purpose. It is provided "as is" without express or implied warranty.
# See the GNU General Public License for more details.
#
# Documents produced by Doxygen are derivative works derived from the
# input used in their production; they are not affected by this license.
#
import sys
import os
import re


def main():
    if len(sys.argv)!=2:
        sys.exit('Usage: %s <lex_file>' % sys.argv[0])

    lex_file = sys.argv[1]
    if (os.path.exists(lex_file)):
        #write preamble
        print("static const char *stateToString(int state)")
        print("{")
        print("  switch(state)")
        print("  {")
        print("    case INITIAL: return \"INITIAL\";")

        with open(lex_file) as f:
            for line in f:
                if re.search(r'^%x', line) or  re.search(r'^%s', line):
                    state = line.split()[1]
                    print("    case %s: return \"%s\";" % (state,state))
                elif re.search(r'^%%', line):
                    break
                else:
                    pass
            f.close()
        #write post
        print("  }")
        print("  return \"Unknown\";")
        print("}")

if __name__ == '__main__':
    main()
