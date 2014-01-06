# 

import sys
import os

f1 = open(os.path.join(sys.argv[1],'unistd.h'),'w')

f1.write("#ifndef UNISTD_H\n")
f1.write("#define UNISTD_H\n")
f1.write("/* this is a dummy file, that is needed for compiling files that are\n")
f1.write(" * generated with flex under Windows 95/NT.\n")
f1.write(" */\n")
f1.write("#if defined(_MSC_VER)\n")
f1.write("  #include <io.h>\n")
f1.write("#endif\n")
f1.write("#endif\n")
