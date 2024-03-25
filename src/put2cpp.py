import sys
import argparse
import re
import os

class Solution:
    '''
    we replace several points.
    - path define
    - declare function
    - file creation code
    - code in generate function
    - Makefile
    '''
    def __init__(self,inputdir,out,perlmodgen,debug):
        '''
            This is  init funciton """ test """
        '''
        self.inputdir = inputdir
        self.out = out
        self.perlmodgen = perlmodgen
        self.debug = debug
        if self.perlmodgen and not os.path.exists(self.perlmodgen):
            print('error:  not exist file :', self.perlmodgen)
            print(' !! set --perlmodgen option with the location of perlmodgen.cpp')
            quit(4)
        files = os.listdir(self.inputdir)
        self.p = []
        self.o = {}
        self.o['path define'] = []
        self.o['declare function'] = []
        self.o['file creation code'] = []
        self.o['code in generate function'] = []
        self.o['Makefile'] = []
        self.p.append('''class PerlModGenerator''')
        self.p.append('''{''')
        self.p.append('''  QCString pathDoxyRules;''')
        self.p.append('''  // perlmodPython code''')
        for file in files:
            self.o['path define'].append('''  QCString path{f};'''.format(f=self.createPythonName(file)))
        self.p += self.o['path define']
        self.p.append('''  QCString pathMakefile;''')
        self.p.append('')
        self.p.append('''  bool generateDoxyRules();''')
        self.p.append('''  // perlmodPython code''')
        for file in files:
            self.o['declare function'].append('''  bool generate{f}();'''.format(f=self.createPythonName(file)))
        self.p += self.o['declare function']
        self.p.append('''  bool generateMakefile();''')
        self.p.append('''}''')
        self.p.append('')

        self.p.append('''bool PerlModGenerator::generateDoxyRules()''')
        self.p.append('''{''')
        self.p.append('''}''')
        self.p.append('')
        self.p.append('''// perlmodPython code''')
        self.p.append('')
        for file in files:
            filepath = self.inputdir+'/'+file
            ff = self.createPythonName(file)
            cpp = []
            with open(filepath,'r', encoding='utf-8', errors='ignore') as f:
                lines = f.readlines()
                 #bs = ' \ '
                 #bs = bs.replace(' ','')
                for line in lines:
                    cpp.append(self.line2cpp(4,line))
            self.o['file creation code'].append('''bool PerlModGenerator::generate{ff}()'''.format(ff=ff))
            self.o['file creation code'].append('''{''')
            self.o['file creation code'].append('''  std::ofstream pythonStream;''')
            self.o['file creation code'].append('''  if (!createOutputFile(pythonStream, path{ff}))'''.format(ff=ff))
            self.o['file creation code'].append('''    return false;''')
            self.o['file creation code'].append('')
            self.o['file creation code'].append('''  pythonStream <<''')
            self.o['file creation code'].append('\n'.join(cpp) + ';')
            self.o['file creation code'].append('')
            self.o['file creation code'].append('''  return true;''')
            self.o['file creation code'].append('''}''')
            self.o['file creation code'].append('')
        self.p += self.o['file creation code']
        self.p.append('''bool PerlModGenerator::generateMakefile()''')
        self.p.append('''{''')
        self.p.append('''}''')
        self.p.append('')

        self.p.append('''void PerlModGenerator::generate()''')
        self.p.append('''{''')
        self.p.append('''  bool perlmodLatex = Config_getBool(PERLMOD_LATEX);''')
        self.p.append('')
        self.p.append('''  if (perlmodLatex) {''')
        self.p.append('''  }''')
        self.p.append('')
        self.p.append('  // perlmodPython code')
        self.p.append('')
        generateList = []
        for file in files:
            ff = self.createPythonName(file)
            self.o['code in generate function'].append('''    path{ff} = perlModAbsPath + "/{f}";'''.format(f=file,ff=ff))
            generateList.append('generate' + ff + '()')
        print('generateList',generateList)
        self.o['code in generate function'].append('''    if (!(''' + '''\n      && '''.join(generateList) + '''))''')
        self.o['code in generate function'].append('''      return;''')
        self.p += self.o['code in generate function']
        self.p.append('''}''')
        self.p.append('')
        self.p.append('''void generatePerlMod()''')
        self.p.append('''{''')
        self.p.append('''}''')
        self.p.append('')
        self.p.append('''  bool perlmodPython = Config_getBool(PERLMOD_PYTHON);''')
        self.p.append('''  if (perlmodPython) {''')
        self.o['Makefile'].append('''    makefileStream <<''')
        mk = []
        mk.append('')
        mk.append('''# code2cpp''')
        mk.append('''code2cpp:''')
        mk.append('''\trm -rf input_perlmodgen_files''')
        mk.append('''\tmkdir -p input_perlmodgen_files''')
        for file in files:
            mk.append('''\tcd input_perlmodgen_files; ln -s ../{f} {f}'''.format(f=file))
        mk.append('''\t# if you want to add python file into perlmodgen.cpp , you can use it.''')
        mk.append('''\t# first of all, you add your py in input_perlmodgen_files directory''')
        mk.append('''\t# please change ../../../../src/perlmodgen.cpp to location of src/perlmodgen.cpp''')
        mk.append('''\t# output filename : perlmodgen.cpp.mod''')
        mk.append('''\tpython3 put2cpp.py --inputdir=input_perlmodgen_files --perlmodgen=../../../../src/perlmodgen.cpp''')
        mk.append('')
        mk.append('''# run python code with DoxyDocs''')
        mk.append('''python:''')
        mk.append('''\tperl doxy2py.pl''')
        mk.append('''\tpython3 sample.py''')
        mk.append('')
        self.o['Makefile'].append('\n'.join([self.line2cpp(6,item) for item in mk]) + ';')
        self.p += self.o['Makefile']
        self.p.append('''  }''')
        with open(self.out ,"w") as f:
            print('write:',self.out)
            f.write('\n'.join(self.p))
        startRe = re.compile('^\s*//\s+perlmodPython\s+PerlModGenerator\s+(?P<key>[^<>]+)<start>\s*')
        endRe = re.compile('^\s*//\s+perlmodPython\s+PerlModGenerator\s+(?P<key>[^<>]+)<end>\s*')
        if self.perlmodgen:
            pmg = []
            lines = []
            with open(self.perlmodgen,'r', encoding='utf-8', errors='ignore') as f:
                lines = f.readlines()
            flag = 0
            startKey = ''
            endKey = '-'
            for line in lines:
                line = line.rstrip()
                grpStart = startRe.search(line)
                grpEnd = endRe.search(line)
                if grpStart:
                    if flag == 1:
                        print('error: already you were in  perlmodpython pair')
                        print('  !! please chek <start> ~ <end> pair')
                        quit(4)
                    print('start [',grpStart.group('key').strip(),']',line)
                    startKey = grpStart.group('key').strip()
                    flag = 1
                    pmg.append(line)
                elif grpEnd:
                    if flag == 0:
                        print('error: already you were out of perlmodpython pair')
                        print('  !! please chek <start> ~ <end> pair')
                        quit(4)
                    print('end [',grpEnd.group('key').strip(),']',line)
                    endKey = grpEnd.group('key').strip()
                    flag = 0
                    pmg += self.o[endKey]
                    pmg.append(line)
                else:
                    if flag == 0:
                        pmg.append(line)
            with open(self.perlmodgen + '.mod','w') as f:
                print('! output perlmodgen file:',self.perlmodgen + '.mod')
                s = "\n".join(pmg)
                f.write(s)

    def createPythonName(self,file):
        '''
        create new path name
        '''
        if not file:
            return file
        file2 = file.replace('.','_')
        fileList = list(file2)
        ff = 'Python' + fileList[0].upper() + ''.join(fileList[1:])
        return ff

    def line2cpp(self,indent,line):
        '''
        change characters for C++
        '''
        line = line.rstrip()
        line = line.replace('\\','\\\\')
        line = line.replace("'", "\\'")
        line = line.replace('"', '\\"')
        return ' '*indent + '"' + line + str('\\n') + '"'

if (__name__ == "__main__"):
    parser = argparse.ArgumentParser(
        prog=sys.argv[0],
        description=
        'put python code into cpp code and edit perlmodgen.cpp to add python code'
    )
    parser.add_argument( '--debug', '-d' , action='store_const' , const=1 , help='debug on')
    parser.add_argument( '--inputdir',
        metavar="<str>",
        type=str,
        default='input_perlmodgen_files',
        help='input directory with python code. files of this directory will be inserted in cpp')
    parser.add_argument( '--perlmodgen',
        metavar="<str>",
        type=str,
        default='./perlmodgen.cpp',
        help='location of perlmodgen.cpp.  python codes in inputdir will be updated.')
    parser.add_argument( '--out',
        metavar="<str>",
        type=str,
        default='out.mod',
        help='insert python code into cpp (perlmodgen.cpp)')

    args = parser.parse_args()
    debug = args.debug

    print('inputdir:',args.inputdir)
    print('input location of perlmodegen.cpp:',args.perlmodgen)
    S = Solution(args.inputdir,args.out,args.perlmodgen,args.debug)

