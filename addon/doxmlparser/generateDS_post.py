#!/usr/bin/env python
#
# Copyright (C) 1997-2022 by Dimitri van Heesch.
#
# Permission to use, copy, modify, and distribute this software and its
# documentation under the terms of the GNU General Public License is hereby
# granted. No representations are made about the suitability of this software
# for any purpose. It is provided "as is" without express or implied warranty.
# See the GNU General Public License for more details.
#
# Documents produced by Doxygen are derivative works derived from the
# input used in their production; they are not affected by this license.

import sys
import re

def main():
    inputFile = open(sys.argv[1], 'r')
    outputFile = open(sys.argv[2], 'wb')
    for line in inputFile:
        line = line.rstrip()
        line = re.sub(r'##','# #',line)
        line = re.sub(r'# Python.*','#',line)
        line = re.sub(r"('-o', ').*(/addon/doxmlparser/doxmlparser)","\\1...\\2",line)
        # python 2 slips the u in ...
        line = re.sub(r"u'","'",line)

        if line.find("generateDS") == -1:
            line = re.sub(r'(#  ).*(/templates/xml/)','\\1...\\2',line)
        else:
            line = re.sub(r'(#  ).*generateDS(.* -o ").*(/addon/doxmlparser/doxmlparser/.* ).*(/templates/xml/)',
                           '\\1.../generateDS\\2...\\3...\\4',line)
        if line.find("              self") == 0:
            line = "  " + line
        outputFile.write(str.encode(line))
        outputFile.write(str.encode('\n'))


if __name__ == '__main__':
    main()
