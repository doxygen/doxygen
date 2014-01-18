# 

import sys
import os

f_sqlite3 = sys.argv[1]
f_libclang = sys.argv[2]

f1 = open(os.path.join(sys.argv[3],'settings.h'),'w')
f1.write("#ifndef SETTINGS_H\n")
f1.write("#define SETTINGS_H\n")
f1.write("\n")
if (f_sqlite3 != "NO"):
  f1.write("#define USE_SQLITE3  1\n")
else:
  f1.write("#define USE_SQLITE3  0\n")

if (f_libclang != "NO"):
  f1.write("#define USE_LIBCLANG 1\n")
else:
  f1.write("#define USE_LIBCLANG 0\n")

f1.write("\n")
f1.write("#define IS_SUPPORTED(x) \\\n")
f1.write("  ((USE_SQLITE3  && strcmp(\"USE_SQLITE3\",(x))==0)  || \\\n")
f1.write("   (USE_LIBCLANG && strcmp(\"USE_LIBCLANG\",(x))==0) || \\\n")
f1.write("  0)\n")
f1.write("\n")
f1.write("#endif\n")
