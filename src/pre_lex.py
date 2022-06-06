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
    if len(sys.argv)!=5:
        sys.exit('Usage: %s <input_lex_file> <output_lex_file> <correction_file> <include_path>' % sys.argv[0])

    inp_lex_file = sys.argv[1]
    out_lex_file = sys.argv[2]
    corr_lex_file = sys.argv[3]
    inc_path = sys.argv[4]

    cnt = 0
    rd_cnt = 0
    add_cnt = 0
    if (os.path.exists(inp_lex_file)):
        out = open(out_lex_file,"w")
        corr = open(corr_lex_file,"w")
        first_corr  = True
        with open(inp_lex_file) as f:
            for line in f:
                if re.search(r'^%doxygen', line):
                    inc_file = inc_path + "/" + line.split()[1]
                    first_line = True
                    with open(inc_file) as f_inc:
                        for inc_line in f_inc:
                            if first_line:
                                first_line = False
                                out.write("  /* #line 1 %s */\n"%(inc_file))
                            cnt += 1
                            add_cnt += 1
                            out.write("%s"%(inc_line))
                        f_inc.close()
                    if not first_line:
                        out.write("  /* #line %d %s */\n"%(rd_cnt+2,inp_lex_file))
                        cnt += 2
                        add_cnt += 1
                        if first_corr:
                            corr.write("%d %d\n"%(0,0))
                            first_corr = False
                        corr.write("%d %d\n"%(cnt,add_cnt))
                else:
                    cnt += 1
                    rd_cnt += 1
                    out.write("%s"%(line))
            f.close()
        if not first_corr:
            # sufficient large
            corr.write("%d %d\n"%(cnt*2,add_cnt*2))
        corr.close()
        out.close()

if __name__ == '__main__':
    main()
