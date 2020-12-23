# place " at begin of each line
# escape existing '\' and '"'
# remove \n at the end of the line (sometimes the last line does not have a \n
#   so we cannot do a replacement with some other text)
# place an escaped \n and " at the end of each line
import sys
for line in sys.stdin:
    sys.stdout.write('"' + line.replace('\\','\\\\').replace('"','\\"').replace('\n','').replace('\r','') + '\\n"\n')
