#!/usr/bin/python
# python script to correct the linenumbers due to inclusion of comon parts in lex files
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
        sys.exit('Usage: %s <input_cpp_file> <output_cpp_file> <correction_file> <original_lex_file> <genetrated_lex_file>' % sys.argv[0])

    inp_cpp_file = sys.argv[1]
    quoted_inp_cpp_file = '"' + inp_cpp_file + '"'
    out_cpp_file = sys.argv[2]
    corr_cpp_file = sys.argv[3]
    org_lex = sys.argv[4]
    gen_lex = sys.argv[5]
    quoted_gen_lex = '"' + gen_lex + '"'

    corr_list = []
    if (os.path.exists(corr_cpp_file)):
        corr = open(corr_cpp_file,"r")
        # read file
        with open(corr_cpp_file,"r") as corr:
            corr_list = [tuple(map(int, i.split(' '))) for i in corr]
            corr.close()

    if (os.path.exists(inp_cpp_file)):
        out = open(out_cpp_file,"w")
        rule_correction = False
        with open(inp_cpp_file) as f:
            for line in f:
                if re.search(r'^#line ', line):
                    if line.split()[2] == quoted_inp_cpp_file:
                        out.write("%s %s \"%s\"\n"%(line.split()[0],line.split()[1],out_cpp_file))
                    elif line.split()[2] == quoted_gen_lex:
                        line_cnt = int(line.split()[1])
                        # run correction
                        corr_cnt = 0
                        for elem in corr_list:
                            if elem[0] <= line_cnt:
                                corr_cnt = elem[1]
                        line_cnt -= corr_cnt
                        out.write("%s %d \"%s\"\n"%(line.split()[0],line_cnt,org_lex))
                    else:
                        out.write("%s"%(line))
                elif re.search(r'^  /\* #line ', line):
                    out.write("%s %s \"%s\"\n"%(line.split()[1],line.split()[2],line.split()[3]))
                elif re.search(r'static .* yy_rule_linenum', line):
                    out.write("%s"%(line))
                    rule_correction = True
                    # read next lines till } and correct numbers
                    # Assumption structure is like:
                    #   static const flex_int16_t yy_rule_linenum[26] =
                    #      {   0,
                    #        981,  985,  989,  995, 1093, 1151, 1153, 1154, 1179, 1182,
                    #       1195, 1198, 1201, 1207, 1211, 1214, 1217, 1223, 1226, 1228,
                    #       1230, 1235, 1236, 1237, 1238
                    #      } ;
                elif rule_correction:
                    if re.search(r'{', line):
                        out.write("%s"%(line))
                    elif re.search(r'}', line):
                        out.write("%s"%(line))
                        rule_correction = False
                    else:
                        out.write("    ")
                        out_list = []
                        rule_list = line.replace(',','').split()
                        for rule_cnt in rule_list:
                            rule_num = int(rule_cnt)
                            corr_cnt = 0
                            for elem in corr_list:
                                if elem[0] <= rule_num:
                                    corr_cnt = elem[1]
                            out_list.append("%5d"%(rule_num-corr_cnt))
                        out.write(','.join(out_list));
                        if re.search(r',$', line):
                            out.write(",")
                        out.write("\n")
                else:
                    out.write("%s"%(line))
            f.close()
        out.close()

if __name__ == '__main__':
    main()
