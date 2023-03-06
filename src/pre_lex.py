#!/usr/bin/python
# python script to include common parts in lex file
#
# Copyright (C) 1997-2022 by Dimitri van Heesch.
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
import os
import re

def main():
    if len(sys.argv)!=6:
        sys.exit('Usage: {0} <input_lex_file> <output_lex_file> <correction_file> <dependency_lex_file> <include_path>'.format(sys.argv[0]))

    inp_lex_file, out_lex_file, corr_lex_file, dep_lex_file, inc_path = sys.argv[1:]

    out_cnt = rd_cnt = add_cnt = 0
    if (os.path.exists(inp_lex_file)):
        with open(out_lex_file,"w") as out_file:
            with open(corr_lex_file,"w") as corr_file:
                first_corr  = True
                with open(dep_lex_file,"w") as dep_file:
                    dep_file.write("{0}:".format(out_lex_file))
                    with open(inp_lex_file) as in_file:
                        for line in in_file:
                            if re.search(r'^%doxygen', line): # special include file marker
                                inc_file = inc_path + "/" + line.split()[1]
                                dep_file.write(" {0}".format(inc_file))
                                first_line = True
                                with open(inc_file) as f_inc:
                                    for inc_line in f_inc:
                                        if first_line:
                                            first_line = False
                                            out_file.write("  /* #line 1 {0} */\n".format(inc_file))
                                        out_cnt += 1
                                        add_cnt += 1
                                        out_file.write(inc_line)
                                    f_inc.close()
                                if not first_line:
                                    out_file.write("  /* #line {0} {1} */\n".format(rd_cnt+2,inp_lex_file))
                                    out_cnt += 2
                                    add_cnt += 1
                                    if first_corr:
                                        corr_file.write("{0} {1}\n".format(0,0))
                                        first_corr = False
                                    corr_file.write("{0} {1}\n".format(out_cnt,add_cnt))
                            else:
                                out_cnt += 1
                                rd_cnt  += 1
                                out_file.write(line)
                    dep_file.write("\n")
    else: # input file does not exist
        sys.exit("Input lex file '{0}' does not exist.".format(sys.argv[1]))

if __name__ == '__main__':
    main()
