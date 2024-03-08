import sys
import argparse
import re
import os

class Solution:
    def __init__(self,inputdir,out,debug):
        self.inputdir = inputdir
        self.out = out
        self.debug = debug
        files = os.listdir(self.inputdir)
        self.p = []
        self.o = {}
        self.o['path define'] = []
        self.o['declare function'] = []
        self.o['file creation code'] = []
        self.o['code in generate function'] = []
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
                for line in lines:
                    line = line.rstrip()
                    line = line.replace('"', '\\"')
                    line = line.replace('\\','\\\\')
                    cpp.append('    "' + line + '\\n"')
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
        self.o['code in generate function'].append('''  bool perlmodPython = Config_getBool(PERLMOD_PYTHON);''')
        self.o['code in generate function'].append('''  if (perlmodPython) {''')
        generateList = []
        for file in files:
            ff = self.createPythonName(file)
            self.o['code in generate function'].append('''    path{ff} = perlModAbsPath + "/{f}";'''.format(f=file,ff=ff))
            generateList.append('generate' + ff + '()')
        print('generateList',generateList)
        self.o['code in generate function'].append('''    if (!(''' + '''\n      && '''.join(generateList) + '''))''')
        self.o['code in generate function'].append('''      return;''')
        self.o['code in generate function'].append('''  }''')
        self.p += self.o['code in generate function']
        self.p.append('''}''')
        self.p.append('')
        self.p.append('''void generatePerlMod()''')
        self.p.append('''{''')
        self.p.append('''}''')
        with open(self.out ,"w") as f:
            print('write:',self.out)
            f.write('\n'.join(self.p))

    def createPythonName(self,file):
        if not file:
            return file
        file2 = file.replace('.py','')
        fileList = list(file2)
        ff = 'Python' + fileList[0].upper() + ''.join(fileList[1:])
        return ff
                
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
        default='input',
        help='input directory')
    parser.add_argument( '--out', 
        metavar="<str>",
        type=str,
        default='out.cpp',
        help='insert python code into cpp (perlmodgen.cpp)')

    args = parser.parse_args()
    debug = args.debug

    print('inputdir:',args.inputdir)
    S = Solution(args.inputdir,args.out,args.debug)

