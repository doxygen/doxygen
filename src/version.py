# 

# script to read the version information from `../configure`
# relevant lines are starting with:
#   `doxygen_version_major`
#   `doxygen_version_minor`
#   `doxygen_version_revision`
#   `doxygen_version_mmn`
# the collected information is written to: `../VERSION` and `../src/version.cpp`
#
import sys
import os
#
# set 'default' values
#
major = 0
minor = 0
revision = 0
mnt = 'NO'
#
# open input file
#  read file and get relevant information
# close
#
f = open('../configure', 'r')
for line in f:
    # check if line can match (saves 3 comparisons)
    if (line.startswith('doxygen_version')):
        if (line.startswith('doxygen_version_major')):
            major = line.replace('doxygen_version_major=','')
        elif (line.startswith('doxygen_version_minor')):
            minor = line.replace('doxygen_version_minor=','')
        elif (line.startswith('doxygen_version_revision')):
            revision = line.replace('doxygen_version_revision=','')
        elif (line.startswith('doxygen_version_mmn')):
            mnt = line.replace('doxygen_version_mmn=','')
f.close()

# strip superfluous '\n`
major = major.replace('\n','')
minor = minor.replace('\n','')
revision = revision.replace('\n','')
mnt = mnt.replace('\n','')
#
# open output files
# write relevant infomation
# close files
#
f1 = open('../VERSION','w')
f2 = open(os.path.join(sys.argv[1],'version.cpp'),'w')
if (mnt == 'NO'):
    f1.write(major + '.' + minor + '.' + revision)
    f2.write('char versionString[]="' + major + '.' + minor + '.' + revision + '";')
else:
    f1.write(major + '.' + minor + '.' + revision + '-' + mnt)
    f2.write('char versionString[]="' + major + '.' + minor + '.' + revision + '-' + mnt + '";')
f1.close()
f2.close()
