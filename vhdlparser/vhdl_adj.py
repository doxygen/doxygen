#!/usr/bin/env python
# python script to adjust generated VhdlParser.cc
#
# Copyright (C) 1997-2021 by Dimitri van Heesch.
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

message_re = re.compile(r'message\s*\+=\s*("[^"]*")')

def main():
    inputFile = open(sys.argv[1], 'r')
    outputFile = open(sys.argv[2], 'w')
    for line in inputFile:
        # fix literal strings
        line = re.sub(message_re,'message += reinterpret_cast<const JJChar*>(\\1)',line)
        # fix missing return statements
        outputFile.write(line.replace("assert(false);","assert(false);return QCString();"))

if __name__ == '__main__':
    main()

