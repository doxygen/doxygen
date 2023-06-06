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

def adjust_tm():
    inputFile = open(sys.argv[2], 'r')
    outputFile = open(sys.argv[3], 'w')
    for line in inputFile:
        if "if (curChar < 64)" in line:
          outputFile.write(line.replace("if (curChar < 64)","if (curChar >= 0 && curChar < 64)"))
        elif "else if (curChar < 128)" in line:
          outputFile.write(line.replace("else if (curChar < 128)","else if (curChar >= 0 && curChar < 128)"))
        elif "int hiByte = (curChar >> 8);" in line:
          outputFile.write("         unsigned char curUChar;\n")
          outputFile.write("         curUChar = *((unsigned char *)&curChar);\n")
          outputFile.write(line.replace("int hiByte = (curChar >> 8);","int hiByte = (curUChar >> 8);"))
        elif "int i2 = (curChar & 0xff) >> 6;" in line:
          outputFile.write(line.replace("int i2 = (curChar & 0xff) >> 6;","int i2 = (curUChar & 0xff) >> 6;"))
        elif "unsigned long long l2 = 1ULL << (curChar & 077);" in line:
          outputFile.write(line.replace("unsigned long long l2 = 1ULL << (curChar & 077);","unsigned long long l2 = 1ULL << (curUChar & 077);"))
        else:
          outputFile.write(line)

def adjust_parser():
    inputFile = open(sys.argv[1], 'r')
    outputFile = open(sys.argv[2], 'w')
    for line in inputFile:
        outputFile.write(line.replace("assert(false);","assert(false);return QCString();"))

if __name__ == '__main__':
    if sys.argv[1] == '--tm':
      adjust_tm()
    else:
      adjust_parser()

