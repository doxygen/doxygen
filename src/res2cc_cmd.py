#!/usr/bin/env python
# Script that compiles a set of resources into a single C++ source file. The C++ file
# offers an initResources() function, which registers the resources with the resource
# manager (class ResourceMgr)
#
# Copyright (C) 1997-2015 by Dimitri van Heesch.
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
from __future__ import print_function
from os         import stat, walk
from os.path    import join, splitext
import sys

class File(object):
    def __init__(self,directory,subdir,fileName,mode):
        self.directory = directory
        self.subdir    = subdir
        self.fileName  = fileName
        filePath  = join(directory,subdir,fileName)
        self.fileSize  = stat(filePath).st_size
        self.bareName  = fileName.replace('.','_')
        self.inputFile = open(filePath,mode)

    def formatByte(self,byte):
        if isinstance(byte,int):
            return "%02x" % byte
        else:
            return format(ord(byte),'02x')

    def writeBytes(self,data,outputFile):
        bytes_per_line=16
        print("static const unsigned char %s_data[] = " % self.bareName,file=outputFile)
        print("{",file=outputFile)
        lines = [data[x:x+bytes_per_line] for x in range(0,len(data),bytes_per_line)]
        linesAsString = ',\n  '.join([', '.join(['0x'+self.formatByte(byte) for byte in line]) for line in lines])
        print('  %s' % linesAsString,file=outputFile)
        print("};",file=outputFile)
        print("const int %s_len = %d;\n" % (self.bareName,len(data)),file=outputFile)

    def convertToBytes(self,outputFile):
        lines = [x for x in self.inputFile.readlines() if not x.startswith('#')]
        w,h = (int(x) for x in lines[0].split())
        data = "".join(map(chr,[int(w>>8),int(w&0xFF),int(h>>8),int(h&0xFF)]+
                               [int(x) for line in lines[1:] for x in line.split()]))
        self.writeBytes(data,outputFile)

    @staticmethod
    def factory(directory,subdir,fname):
        ext = splitext(fname)[1]
        if ext=='.svg':
            return SVGFile(directory,subdir,fname)
        return VerbatimFile(directory,subdir,fname)

class VerbatimFile(File):
    def __init__(self,directory,subdir,fileName):
        File.__init__(self,directory,subdir,fileName,"rb")
    def writeContents(self,outputFile):
        self.writeBytes(self.inputFile.read(),outputFile)
    def writeDirEntry(self,outputFile):
        print("  { \"%s\", \"%s\", %s_data, %s_len, Resource::Verbatim }," % (self.subdir,self.fileName,self.bareName,self.bareName), file=outputFile)

class SVGFile(File):
    def __init__(self,directory,subdir,fileName):
        File.__init__(self,directory,subdir,fileName,"r")
    def writeContents(self,outputFile):
        self.writeBytes(self.inputFile.read(),outputFile)
    def writeDirEntry(self,outputFile):
        print("  { \"%s\", \"%s\", %s_data, %s_len, Resource::SVG }," % (self.subdir,self.fileName,self.bareName,self.bareName), file=outputFile)

def main():
    if len(sys.argv)<3:
        sys.exit('Usage: %s directory output_file.cpp' % sys.argv[0])
    directory = sys.argv[1]
    files = []
    for dirName, subdirList, fileList in walk(directory):
        for fname in fileList:
            subdir = dirName[len(directory)+1:] if dirName.startswith(directory) else dirName
            if subdir:
                files.append(File.factory(directory,subdir,fname))
    files.sort(key=lambda f: f.subdir + "/" + f.fileName)
    outputFile = open(sys.argv[2],"w")
    print("#include \"resourcemgr.h\"\n",file=outputFile)
    for f in files:
        f.writeContents(outputFile)
    print("void initResources() { ResourceMgr::instance().registerResources({",file=outputFile)
    for f in files:
        f.writeDirEntry(outputFile)
    print("});}",file=outputFile)

if __name__ == '__main__':
    main()
