#!/usr/bin/python
# python script to correct the linenumbers due to inclusion of common parts in lex files
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
        sys.exit('Usage: {0} <input_cpp_file> <output_cpp_file> <correction_file> <original_lex_file> <generated_lex_file>'.format(sys.argv[0]))

    inp_cpp_file, out_cpp_file, corr_cpp_file, org_lex, gen_lex = sys.argv[1:]

    quoted_gen_lex      = '"' + gen_lex      + '"'

    corr_list = []
    if (os.path.exists(corr_cpp_file)):
        # read correction file as list of tuples
        with open(corr_cpp_file,"r") as corr_file:
            corr_list = [tuple(map(int, line.split(' '))) for line in corr_file]

    if (os.path.exists(inp_cpp_file)):
        with open(out_cpp_file,"w") as out_file:
            rule_correction = False
            with open(inp_cpp_file) as in_file:
                for line in in_file:
                    # helper function to find the correction needed for the given line number
                    def get_line_correction(line_no):
                        corr_cnt = 0
                        for elem in corr_list:
                            if elem[0] <= line_no:
                                corr_cnt = elem[1]
                        return corr_cnt

                    if re.search(r'^#line ', line): # statement added by C-preprocessing
                        inc_line, inc_file = line.split()[1:3]
                        if inc_file == quoted_gen_lex:
                            line_cnt = int(inc_line)
                            line_cnt -= get_line_correction(line_cnt) # adjust line number
                            out_file.write("#line {0} \"{1}\"\n".format(line_cnt,org_lex))
                        else:
                            out_file.write(line)
                    elif re.search(r'^  /\* #line ', line): # statement added by lex-preprocessing
                        inc_line, inc_file = line.split()[2:4] # first token is `/*` part
                        out_file.write("#line {0} \"{1}\"\n".format(inc_line,inc_file))
                    elif re.search(r'static .* yy_rule_linenum', line): # linenum table generated in debug mode (flex -d)
                        out_file.write(line)
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
                        if re.search(r'{', line): # start of yy_rule_linenum array
                            out_file.write(line)
                        elif re.search(r'}', line): # end of yy_rule_linenum array
                            out_file.write(line)
                            rule_correction = False
                        else: # inside the table
                            out_file.write("    ")
                            out_list = []
                            rule_list = line.replace(',',' ').split()
                            for rule_cnt in rule_list:
                                rule_num = int(rule_cnt)
                                rule_num -= get_line_correction(rule_num)
                                out_list.append("{0:5d}".format(rule_num))
                            out_file.write(','.join(out_list))
                            if re.search(r',$', line): # re-add trailing comma
                                out_file.write(",")
                            out_file.write("\n")
                    else: # normal line -> just copy
                        out_file.write(line)
    else: # input file does not exist
        sys.exit("Input cpp file '{0}' does not exist.".format(sys.argv[1]))
        exit(1)

if __name__ == '__main__':
    main()
