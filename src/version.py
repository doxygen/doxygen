#!/usr/bin/python
# python script to generate version.cpp from first argument
#
# Copyright (C) 1997-2018 by Dimitri van Heesch.
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

def main():
    if len(sys.argv)<2:
		sys.exit('Usage: %s <version>' % sys.argv[0])
    print('char versionString[]="%s";' % sys.argv[1])

if __name__ == '__main__':
	main()
