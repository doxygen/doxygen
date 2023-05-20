import sys

version = sys.argv[1]
inputFile = sys.argv[2]
outputFile = sys.argv[3]

with open(outputFile,"w") as out_file:
    with open(inputFile,"r") as in_file:
        for line in in_file:
            out_file.write(line.replace("@VERSION@",version))
