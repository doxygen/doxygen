# called as `replace_version.py src dest v1`
# replaces @VERSION@ by v1 in file src and writes the result to dest
import sys

def main():
    if len(sys.argv)<4:
        print('replace_version.py src dest version')
        sys.exit(1)

    inputFile  = sys.argv[1]
    outputFile = sys.argv[2]
    version    = sys.argv[3]

    with open(outputFile,"w") as out_file:
        with open(inputFile,"r") as in_file:
            for line in in_file:
                out_file.write(line.replace("@VERSION@",version))

if __name__ == '__main__':
    main()
