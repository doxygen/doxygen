import sys
do_print = False
for line in sys.stdin:
    if line.startswith("%--- Begin generated contents ---"):
        do_print = True
    elif line.startswith("%--- End generated contents ---"):
        do_print = False
    elif do_print:
        sys.stdout.write(line)
