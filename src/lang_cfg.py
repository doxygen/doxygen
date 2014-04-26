import sys

if (len(sys.argv) > 1):
    if (sys.argv[1] == "ENONLY"):
        print "#define ENGLISH_ONLY"
    else:
        for x in xrange(1, len(sys.argv)):
            print "#define LANG_%s"%(sys.argv[x])
