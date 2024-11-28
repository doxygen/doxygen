#!/usr/bin/env python

from __future__ import print_function
import argparse, glob, itertools, re, shutil, os, sys
import subprocess
import shlex

config_reg = re.compile(r'.*\/\/\s*(?P<name>\S+):\s*(?P<value>.*)$')
bkmk_reg = re.compile(r'.*bkmkstart\s+([A-Z][A-Z][A-Z][A-Z][A-Z][A-Z][A-Z][A-Z][A-Z][A-Z]).*')
hyper_reg = re.compile(r'.*HYPERLINK\s+[\\l]*\s+"([A-Z][A-Z][A-Z][A-Z][A-Z][A-Z][A-Z][A-Z][A-Z][A-Z])".*')
pageref_reg = re.compile(r'.*PAGEREF\s+([A-Z][A-Z][A-Z][A-Z][A-Z][A-Z][A-Z][A-Z][A-Z][A-Z]).*')


def xopen(fname, mode='r', encoding='utf-8'):
    '''Unified file opening for Python 2 an Python 3.

    Python 2 does not have the encoding argument. Python 3 has one.
    '''

    if sys.version_info[0] == 2:
        return open(fname, mode=mode) # Python 2 without encoding
    else:
        return open(fname, mode=mode, encoding=encoding) # Python 3 with encoding

def xpopen(cmd, cmd1="",encoding='utf-8-sig', getStderr=False):
    '''Unified file pipe opening for Python 2 an Python 3.

    Python 2 does not have the encoding argument. Python 3 has one. and
    '''

    if sys.version_info[0] == 2:
        return os.popen(cmd).read() # Python 2 without encoding
    else:
        if (getStderr):
            proc = subprocess.Popen(shlex.split(cmd1),stdout=subprocess.PIPE,stderr=subprocess.PIPE,encoding=encoding) # Python 3 with encoding
            return proc.stderr.read()
        else:
            proc = subprocess.Popen(shlex.split(cmd),stdout=subprocess.PIPE,stderr=subprocess.PIPE,encoding=encoding) # Python 3 with encoding
            return proc.stdout.read()

def clean_header(errmsg):
    # messages (due to the usage of more) have a contents like:
    # ::::::::::::
    # <file name>
    # ::::::::::::
    # we want to skip these
    msg = errmsg.split('\n')
    rtnmsg = ""
    cnt = -1
    for o in msg:
        if (o):
            if (cnt == -1):
                if o.startswith(":::::::"):
                    cnt = 3
            if (cnt > 0):
                cnt-=1
            else:
                rtnmsg+=o
                rtnmsg+="\n"
    return rtnmsg
 
class Tester:
    def __init__(self,args,test):
        self.args      = args
        self.test      = test
        self.update    = args.updateref
        self.config    = self.get_config()
        self.test_name = '[%s]: %s' % (self.test,self.config['objective'][0])
        self.test_id   = self.test.split('_')[0]
        if self.update:
            self.test_out = self.args.inputdir+'/'+self.test_id
        else:
            self.test_out = self.args.outputdir+'/test_output_'+self.test_id
        self.prepare_test()

    def compare_ok(self,got_file,expected_file,name):
        if not os.path.isfile(got_file):
            return (True,'%s absent' % got_file)
        elif not os.path.isfile(expected_file):
            return (True,'%s absent' % expected_file)
        else:
            diff = xpopen('diff -b -w -u %s %s' % (got_file,expected_file))
            if diff and not diff.startswith("No differences"):
                return (True,'Difference between generated output and reference:\n%s' % diff)
        return (False,'')

    def cleanup_xmllint(self,errmsg):
        msg = errmsg.split('\n')
        rtnmsg = ""
        for o in msg:
            if (o):
                if (o.startswith("I/O error : Attempt")):
                    pass
                else:
                    if (rtnmsg):
                        rtnmsg += '\n'
                    rtnmsg += o
        return rtnmsg

    def cleanup_xmllint_docbook(self,errmsg):
        # For future work, first get everything valid XML
        msg = self.cleanup_xmllint(errmsg).split('\n')
        rtnmsg = ""
        cnt = 0
        for o in msg:
            if (o):
                if (cnt):
                    cnt -= 1
                    pass
                elif (o.endswith("does not validate")):
                    pass
                elif (o.find("no DTD found!")!=-1):
                    pass
                elif (o.find("is not an NCName")!=-1):
                    cnt = 2
                else:
                    if (rtnmsg):
                        rtnmsg += '\n'
                    rtnmsg += o
        return rtnmsg

    def get_config(self):
        config = {}
        with xopen(self.args.inputdir+'/'+self.test,'r') as f:
            for line in f.readlines():
                m = config_reg.match(line)
                if m:
                    key   = m.group('name')
                    value = m.group('value')
                    if (key=='config'):
                        value = value.replace('$INPUTDIR',self.args.inputdir)
                    # print('key=%s value=%s' % (key,value))
                    config.setdefault(key, []).append(value)
        return config

    def prepare_test(self):
        # prepare test environment
        shutil.rmtree(self.test_out,ignore_errors=True)
        os.mkdir(self.test_out)
        shutil.copy(self.args.inputdir+'/Doxyfile',self.test_out)
        inputs = '%s/%s' % (self.args.inputdir,self.test)
        for i in self.config.get('input', []):
            inputs += ' %s/%s' % (self.args.inputdir,i)
        with xopen(self.test_out+'/Doxyfile','a') as f:
            print('INPUT=%s' % inputs, file=f)
            print('STRIP_FROM_PATH=%s' % self.args.inputdir, file=f)
            print('EXAMPLE_PATH=%s' % self.args.inputdir, file=f)
            print('WARN_LOGFILE=%s/warnings.log' % self.test_out, file=f)
            if 'config' in self.config:
                for option in self.config['config']:
                    print(option, file=f)
            if (self.args.xml or self.args.xmlxsd):
                print('GENERATE_XML=YES', file=f)
                print('XML_OUTPUT=%s/out' % self.test_out, file=f)
            else:
                print('GENERATE_XML=NO', file=f)
            if (self.args.rtf):
                print('GENERATE_RTF=YES', file=f)
                print('RTF_HYPERLINKS=YES', file=f)
                print('RTF_OUTPUT=%s/rtf' % self.test_out, file=f)
            else:
                print('GENERATE_RTF=NO', file=f)
            if (self.args.docbook):
                print('GENERATE_DOCBOOK=YES', file=f)
                print('DOCBOOK_OUTPUT=%s/docbook' % self.test_out, file=f)
            else:
                print('GENERATE_DOCBOOK=NO', file=f)
            if (self.args.qhp):
                print('GENERATE_QHP=YES', file=f)
            if (self.args.xhtml or self.args.qhp):
                print('GENERATE_HTML=YES', file=f)
            # HTML_OUTPUT can also have been set locally
            print('HTML_OUTPUT=%s/html' % self.test_out, file=f)
            print('HTML_FILE_EXTENSION=.xhtml', file=f)
            if (self.args.pdf):
                print('GENERATE_LATEX=YES', file=f)
                print('LATEX_BATCHMODE=YES', file=f)
                print('LATEX_OUTPUT=%s/latex' % self.test_out, file=f)
            if self.args.subdirs:
                print('CREATE_SUBDIRS=YES', file=f)
            if (self.args.clang):
                print('CLANG_ASSISTED_PARSING=YES', file=f)
            if (self.args.cfgs):
                for cfg in self.args.cfgs:
                    if cfg[0].find('=') == -1:
                        print("Not a doxygen configuration item, missing '=' sign: '%s'."%cfg)
                        sys.exit(1)
                    print(cfg[0], file=f)

        if 'check' not in self.config or not self.config['check']:
            print('Test doesn\'t specify any files to check')
            sys.exit(1)

        # run doxygen
        if (sys.platform == 'win32'):
            redir=' > nul: 2>&1'
        else:
            redir=' 2> /dev/null > /dev/null'

        if (self.args.noredir):
            redir=''

        if os.system('%s %s %s/Doxyfile %s' % (self.args.doxygen,self.args.doxygen_dbg,self.test_out,redir))!=0:
            print('Error: failed to run %s on %s/Doxyfile' % (self.args.doxygen,self.test_out))
            sys.exit(1)


    def check_link_rtf_file(self,fil):
        bkmk_res = []
        hyper_res = []
        pageref_res = []
        with xopen(fil,'r') as f:
            for line in f.readlines():
                if ("bkmkstart" in line) or ("HYPERLINK" in line) or ("PAGEREF" in line):
                    msg = line.split('}')
                    for m in msg:
                        if bkmk_reg.match(m):
                            m1 = re.sub(bkmk_reg, '\\1', m)
                            bkmk_res.append(m1)
                        elif hyper_reg.match(m):
                            m1 = re.sub(hyper_reg, '\\1', m)
                            hyper_res.append(m1)
                        elif pageref_reg.match(m):
                            m1 = re.sub(pageref_reg, '\\1', m)
                            pageref_res.append(m1)
        # Has been commented out as in the test 57, inline namespace, there is still a small problem.
        #if sorted(bkmk_res) != sorted(set(bkmk_res)):
        #   return (False, "RTF: one (or more) bookmark(s) has(have) been defined multiple times")
        hyper_res = sorted(set(hyper_res))
        for h in hyper_res:
            if h not in bkmk_res:
                #print(bkmk_res)
                #print(hyper_res)
                return (False, "RTF: Not all used hyperlinks have been defined")
        pageref_res = sorted(set(pageref_res))
        for p in pageref_res:
            if p not in bkmk_res:
                #print(bkmk_res)
                #print(pageref_res)
                return (False, "RTF: Not all used page reference bookmarks have been defined")
        return (True,"")


    # update the reference data for this test
    def update_test(self,testmgr):
        print('Updating reference for %s' % self.test_name)

        if 'check' in self.config:
            for check in self.config['check']:
                check_file='%s/out/%s' % (self.test_out,check)
                # check if the file we need to check is actually generated
                if not os.path.isfile(check_file):
                    print('Non-existing file %s after \'check:\' statement' % check_file)
                    return False
                # convert output to canonical form
                data = xpopen('%s --format --noblanks --nowarning %s' % (self.args.xmllint,check_file))
                if data:
                    # strip version
                    data = re.sub(r'xsd" version="[0-9.-]+"','xsd" version=""',data).rstrip('\n')
                else:
                    print('Failed to run %s on the doxygen output file %s' % (self.args.xmllint,self.test_out))
                    return False
                out_file='%s/%s' % (self.test_out,check)
                with xopen(out_file,'w') as f:
                    print(data,file=f)
        shutil.rmtree(self.test_out+'/out',ignore_errors=True)
        os.remove(self.test_out+'/Doxyfile')
        os.remove(self.test_out+'/warnings.log')
        return True

    # check the relevant files of a doxygen run with the reference material
    def perform_test(self,testmgr):
        if (sys.platform == 'win32'):
            redir=' > nul:'
            separ='&'
        else:
            redir=' 2> /dev/null'
            separ=';'

        if (self.args.noredir):
            redir=''

        failed_xml=False
        failed_html=False
        failed_qhp=False
        failed_latex=False
        failed_docbook=False
        failed_rtf=False
        failed_xmlxsd=False
        msg = ()
        # look for files to check against the reference
        if self.args.xml or self.args.xmlxsd:
            failed_xml=False
            if 'check' in self.config and self.args.xml:
                failed_xml=True
                for check in self.config['check']:
                    check_file='%s/out/%s' % (self.test_out,check)
                    # check if the file we need to check is actually generated
                    if not os.path.isfile(check_file):
                        # try with sub dirs
                        check_file = glob.glob('%s/out/*/*/%s' % (self.test_out,check))
                        if not check_file:
                            check_file='%s/out/%s' % (self.test_out,check)
                            msg += ('Non-existing file %s after \'check:\' statement' % check_file,)
                            break
                        else:
                            check_file = check_file[0]
                    # convert output to canonical form
                    check_file = check_file.replace('\\','/')
                    data = xpopen('%s --format --noblanks --nowarning %s' % (self.args.xmllint,check_file))
                    if data:
                        # strip version
                        data = re.sub(r'xsd" version="[0-9.-]+"','xsd" version=""',data).rstrip('\n')
                    else:
                        msg += ('Failed to run %s on the doxygen output file %s' % (self.args.xmllint,self.test_out),)
                        break
                    if self.args.subdirs:
                        data = re.sub('d[0-9a-f]/d[0-9a-f][0-9a-f]/','',data)
                    out_file='%s/%s' % (self.test_out,check)
                    with xopen(out_file,'w') as f:
                        print(data,file=f)
                    ref_file='%s/%s/%s' % (self.args.inputdir,self.test_id,check)
                    (failed_xml,xml_msg) = self.compare_ok(out_file,ref_file,self.test_name)
                    if failed_xml:
                        msg+= (xml_msg,)
                        break
            failed_xmlxsd=False
            if self.args.xmlxsd:
                xmlxsd_output='%s/out' % self.test_out
                if (sys.platform == 'win32'):
                    redirx=' 2> %s/temp >nul:'%xmlxsd_output
                else:
                    redirx='2>%s/temp >/dev/null'%xmlxsd_output
                #
                index_xml = []
                index_xml.append(glob.glob('%s/index.xml' % (xmlxsd_output)))
                index_xml.append(glob.glob('%s/*/*/index.xml' % (xmlxsd_output)))
                index_xml = ' '.join(list(itertools.chain.from_iterable(index_xml))).replace(self.args.outputdir +'/','').replace('\\','/')
                index_xsd = []
                index_xsd.append(glob.glob('%s/index.xsd' % (xmlxsd_output)))
                index_xsd.append(glob.glob('%s/*/*/index.xsd' % (xmlxsd_output)))
                index_xsd = ' '.join(list(itertools.chain.from_iterable(index_xsd))).replace(self.args.outputdir +'/','').replace('\\','/')
                exe_string = '%s --noout --schema %s %s' % (self.args.xmllint,index_xsd,index_xml)
                exe_string1 = exe_string
                exe_string += ' %s' % (redirx)
                exe_string += ' %s more "%s/temp"' % (separ,xmlxsd_output)

                xmllint_out = xpopen(exe_string,exe_string1,getStderr=True)
                if xmllint_out:
                    xmllint_out = re.sub(r'.*validates','',xmllint_out).rstrip('\n')
                else:
                    msg += ('Failed to run %s with schema %s for files: %s' % (self.args.xmllint,index_xsd,index_xml),)
                    failed_xmlxsd=True
                if xmllint_out:
                    xmllint_out  = clean_header(xmllint_out)
                if xmllint_out:
                    msg += (xmllint_out,)
                    failed_xmlxsd=True
                #
                doxyfile_xml = []
                doxyfile_xml.append(glob.glob('%s/Doxyfile.xml' % (xmlxsd_output)))
                doxyfile_xml.append(glob.glob('%s/*/*/Doxyfile.xml' % (xmlxsd_output)))
                doxyfile_xml = ' '.join(list(itertools.chain.from_iterable(doxyfile_xml))).replace(self.args.outputdir +'/','').replace('\\','/')
                doxyfile_xsd = []
                doxyfile_xsd.append(glob.glob('%s/doxyfile.xsd' % (xmlxsd_output)))
                doxyfile_xsd.append(glob.glob('%s/*/*/doxyfile.xsd' % (xmlxsd_output)))
                doxyfile_xsd = ' '.join(list(itertools.chain.from_iterable(doxyfile_xsd))).replace(self.args.outputdir +'/','').replace('\\','/')
                exe_string = '%s --noout --schema %s %s' % (self.args.xmllint,doxyfile_xsd,doxyfile_xml)
                exe_string1 = exe_string
                exe_string += ' %s' % (redirx)
                exe_string += ' %s more "%s/temp"' % (separ,xmlxsd_output)

                xmllint_out = xpopen(exe_string,exe_string1,getStderr=True)
                if xmllint_out:
                    xmllint_out = re.sub(r'.*validates','',xmllint_out).rstrip('\n')
                else:
                    msg += ('Failed to run %s with schema %s for files: %s' % (self.args.xmllint,doxyfile_xsd,doxyfile_xml),)
                    failed_xmlxsd=True
                if xmllint_out:
                    xmllint_out  = clean_header(xmllint_out)
                if xmllint_out:
                    msg += (xmllint_out,)
                    failed_xmlxsd=True
                #
                compound_xml = []
                compound_xml.append(glob.glob('%s/*.xml' % (xmlxsd_output)))
                compound_xml.append(glob.glob('%s/*/*/*.xml' % (xmlxsd_output)))
                compound_xml = ' '.join(list(itertools.chain.from_iterable(compound_xml))).replace(self.args.outputdir +'/','').replace('\\','/')
                compound_xml = re.sub(r' [^ ]*/index.xml','',compound_xml)
                compound_xml = re.sub(r'[^ ]*/index.xml ','',compound_xml)
                compound_xml = re.sub(r' [^ ]*/Doxyfile.xml','',compound_xml)
                compound_xml = re.sub(r'[^ ]*/Doxyfile.xml ','',compound_xml)

                compound_xsd = []
                compound_xsd.append(glob.glob('%s/compound.xsd' % (xmlxsd_output)))
                compound_xsd.append(glob.glob('%s/*/*/compound.xsd' % (xmlxsd_output)))
                compound_xsd = ' '.join(list(itertools.chain.from_iterable(compound_xsd))).replace(self.args.outputdir +'/','').replace('\\','/')
                exe_string = '%s --noout --schema %s %s' % (self.args.xmllint,compound_xsd,compound_xml)
                exe_string1 = exe_string
                exe_string += ' %s' % (redirx)
                exe_string += ' %s more "%s/temp"' % (separ,xmlxsd_output)

                xmllint_out = xpopen(exe_string,exe_string1,getStderr=True)
                if xmllint_out:
                    xmllint_out = re.sub(r'.*validates','',xmllint_out).rstrip('\n')
                else:
                    msg += ('Failed to run %s with schema %s for files: %s' % (self.args.xmllint,compound_xsd,compound_xml),)
                    failed_xmlxsd=True
                if xmllint_out:
                    xmllint_out  = clean_header(xmllint_out)
                if xmllint_out:
                    msg += (xmllint_out,)
                    failed_xmlxsd=True

            if not failed_xml and not failed_xmlxsd and not self.args.keep:
                xml_output='%s/out' % self.test_out
                shutil.rmtree(xml_output,ignore_errors=True)

        if (self.args.rtf):
            (res, msg1) = self.check_link_rtf_file("%s/rtf/refman.rtf" % self.test_out)
            if not res:
                #msg += ("RTF: Not all used hyperlinks have been defined",)
                msg += (msg1,)
                failed_rtf=True

        if (self.args.docbook):
            docbook_output='%s/docbook' % self.test_out
            if (sys.platform == 'win32'):
                redirx=' 2> %s/temp >nul:'%docbook_output
            else:
                redirx='2>%s/temp >/dev/null'%docbook_output
            # For future work, first get everything valid XML
            # exe_string = '%s --relaxng db/docbook.rng --nonet --postvalid %s/*xml %s  % (self.args.xmllint,docbook_output,redirx)
            tests = []
            tests.append(glob.glob('%s/*.xml' % (docbook_output)))
            tests.append(glob.glob('%s/*/*/*.xml' % (docbook_output)))
            tests = ' '.join(list(itertools.chain.from_iterable(tests))).replace(self.args.outputdir +'/','').replace('\\','/')
            exe_string = '%s --noout --nonet --postvalid %s' % (self.args.xmllint,tests)
            exe_string1 = exe_string
            exe_string += ' %s' % (redirx)
            exe_string += ' %s more "%s/temp"' % (separ,docbook_output)

            failed_docbook=False
            xmllint_out = xpopen(exe_string,exe_string1,getStderr=True)
            xmllint_out = self.cleanup_xmllint_docbook(xmllint_out)
            if xmllint_out:
                xmllint_out  = clean_header(xmllint_out)
            if xmllint_out:
                msg += (xmllint_out,)
                failed_docbook=True
            elif not self.args.keep:
                shutil.rmtree(docbook_output,ignore_errors=True)

        if (self.args.xhtml or self.args.qhp):
            html_output='%s/html' % self.test_out
            if (sys.platform == 'win32'):
                redirx=' 2> %s/temp >nul:'%html_output
            else:
                redirx='2>%s/temp >/dev/null'%html_output
            check_file = []
            check_file.append(glob.glob('%s/*.xhtml' % (html_output)))
            check_file.append(glob.glob('%s/*/*/*.xhtml' % (html_output)))
            check_file = ' '.join(list(itertools.chain.from_iterable(check_file))).replace(self.args.outputdir +'/','').replace('\\','/')
            exe_string = '%s --noout --path dtd --nonet --postvalid %s' % (self.args.xmllint,check_file)
            exe_string1 = exe_string
            exe_string += ' %s' % (redirx)
            exe_string += ' %s more "%s/temp"' % (separ,html_output)
            failed_html=False
            xmllint_out = xpopen(exe_string,exe_string1,getStderr=True)
            xmllint_out = self.cleanup_xmllint(xmllint_out)
            if xmllint_out:
                xmllint_out  = clean_header(xmllint_out)
                if xmllint_out:
                    msg += (xmllint_out,)
                    failed_html=True

            failed_qhp=False
            if not failed_html and self.args.qhp:
                check_file = "%s/index.qhp"%(html_output)
                exe_string = '%s --noout %s' % (self.args.xmllint,check_file)
                exe_string1 = exe_string
                exe_string += ' %s' % (redirx)
                exe_string += ' %s more "%s/temp"' % (separ,html_output)
                xmllint_out = xpopen(exe_string,exe_string1,getStderr=True)
                xmllint_out = self.cleanup_xmllint(xmllint_out)
                if xmllint_out:
                    msg += (xmllint_out,)
                    failed_qhp=True
            if not failed_html and not failed_qhp and not self.args.keep:
                shutil.rmtree(html_output,ignore_errors=True)
        if (self.args.pdf):
            failed_latex=False
            latex_output='%s/latex' % self.test_out
            # with languages like Hungarian we had problems with some tests on windows when stderr was used.
            if (sys.platform == 'win32'):
                outType=False
                redirl='>nul: 2>temp'
                mk='make.bat'
            else:
                outType=True
                redirl='>/dev/null 2>temp'
                mk='make'
            cur_directory = os.getcwd()
            os.chdir(latex_output)
            exe_string = mk
            exe_string1 = exe_string
            exe_string += ' %s' % (redirl)
            if outType:
                exe_string += ' %s more temp' % (separ)
            latex_out = xpopen(exe_string,exe_string1,getStderr=outType)
            os.chdir(cur_directory);
            if (outType and latex_out.find("Error")!=-1):
                msg += ("PDF generation failed\n  For a description of the problem see 'refman.log' in the latex directory of this test",)
                failed_latex=True
            elif (not outType and xopen(latex_output + "/temp",'r').read().find("Error")!= -1):
                msg += ("PDF generation failed\n  For a description of the problem see 'refman.log' in the latex directory of this test",)
                failed_latex=True
            elif xopen(latex_output + "/refman.log",'r',encoding='ISO-8859-1').read().find("Error")!= -1:
                msg += ("PDF generation failed\n  For a description of the problem see 'refman.log' in the latex directory of this test",)
                failed_latex=True
            elif xopen(latex_output + "/refman.log",'r',encoding='ISO-8859-1').read().find("Emergency stop")!= -1:
                msg += ("PDF generation failed\n  For a description of the problem see 'refman.log' in the latex directory of this test",)
                failed_latex=True
            elif not self.args.keep:
                shutil.rmtree(latex_output,ignore_errors=True)

        warnings = xopen(self.test_out + "/warnings.log",'r',encoding='ISO-8859-1').read()
        failed_warn =  len(warnings)!=0
        if failed_warn:
            msg += (warnings,)

        if failed_warn or failed_xml or failed_html or failed_qhp or failed_latex or failed_docbook or failed_rtf or failed_xmlxsd:
            testmgr.ok(False,self.test_name,msg)
            return False

        testmgr.ok(True,self.test_name)
        if not self.args.keep:
            shutil.rmtree(self.test_out,ignore_errors=True)
        return True

    def run(self,testmgr):
        if self.update:
            return self.update_test(testmgr)
        else:
            return self.perform_test(testmgr)

def do_generation_work(test):
    tester = Tester(test[0].args,test[1])
    return tester.run(test[0])

class TestManager:
    def __init__(self,args,tests):
        self.args  = args
        self.tests = tests
        self.num_tests = len(tests)
        self.count=1
        self.passed=0
        if (self.args.xhtml or self.args.qhp):
            self.prepare_dtd()
        print('1..%d' % self.num_tests)

    def ok(self,result,test_name,msg='Ok'):
        if result:
            print('ok - %s' % (test_name))
            self.passed = self.passed + 1
        else:
            print('not ok - %s' % (test_name))
            print('-------------------------------------')
            for o in msg:
                print(o)
                print('-------------------------------------')
        self.count = self.count + 1

    def result(self):
        if self.passed==self.num_tests:
            print('All tests passed!')
        else:
            print('%d out of %s tests failed' % (self.num_tests-self.passed,self.num_tests))
        return 0 if self.passed==self.num_tests else 1

    def perform_tests(self):
        if (self.args.pool == 1):
            passed = 0
            for test in self.tests:
                tester = Tester(self.args,test)
                passed += tester.run(self)
            self.passed = passed
        else:
            dl = []
            for test in self.tests:
                dl += [(self, test)]
            import multiprocessing as mp
            p = mp.Pool(processes=self.args.pool)
            passed = p.map(do_generation_work, dl)
            self.passed = sum(passed)
        res=self.result()
        if (self.args.xhtml or self.args.qhp) and self.args.inputdir!='.' and not res and not self.args.keep:
            shutil.rmtree("dtd",ignore_errors=True)
        return 0 if self.args.updateref else res

    def prepare_dtd(self):
        if self.args.inputdir!='.':
            shutil.rmtree("dtd",ignore_errors=True)
            shutil.copytree(self.args.inputdir+"/dtd", "dtd")

def split_and_keep(s,sep):
    s = s.replace('"','')             # add token separator
    s = s.replace(sep,'\0'+sep)             # add token separator
    s = s.split('\0')                       # split by null delimiter
    s = [x.strip() for x in filter(None,s)] # strip and remove empty elements
    s = [z.split(' ',1) for z in s]         # split by first space
    s = [i for ss in s for i in ss]         # flatten the list
    return s

def main():
    # argument handling
    parser = argparse.ArgumentParser(description='run doxygen tests')
    parser.add_argument('--updateref',help=
        'update the reference files. Should be used in combination with --id to '
        'update the reference file(s) for the given test',action="store_true")
    parser.add_argument('--doxygen',nargs='?',default='doxygen',help=
        'path/name of the doxygen executable')
    parser.add_argument('--doxygen_dbg',nargs='?',default='',help=
        'the doxygen debugging arguments')
    parser.add_argument('--xmllint',nargs='?',default='xmllint',help=
        'path/name of the xmllint executable')
    parser.add_argument('--id',nargs='+',dest='ids',action='append',type=int,help=
        'run test number n (the option can be specified multiple times)')
    parser.add_argument('--start_id',dest='start_id',type=int,help=
        'run tests starting with number n')
    parser.add_argument('--end_id',dest='end_id',type=int,help=
        'run tests ending with number n')
    parser.add_argument('--exclude_id',nargs='+',dest='exclude_ids',action='append',type=int,help=
        'run without test number n (the option can be specified multiple times)')
    parser.add_argument('--all',help=
        'can be used in combination with -updateref to update the reference files '
        'for all tests.',action="store_true")
    parser.add_argument('--inputdir',nargs='?',default='.',help=
        'input directory containing the tests')
    parser.add_argument('--outputdir',nargs='?',default='.',help=
        'output directory to write the doxygen output to')
    parser.add_argument('--noredir',help=
        'disable redirection of doxygen warnings',action="store_true")
    parser.add_argument('--pool',nargs='?',default='1',type=int,help=
        'pool size of multiprocess tests')
    parser.add_argument('--xml',help='create xml output and check',
        action="store_true")
    parser.add_argument('--rtf',help=
        'create rtf output',action="store_true")
    parser.add_argument('--docbook',help=
        'create docbook output and check with xmllint',action="store_true")
    parser.add_argument('--xhtml',help=
        'create xhtml output and check with xmllint',action="store_true")
    parser.add_argument('--qhp',help=
        'create qhp output and check with xmllint',action="store_true")
    parser.add_argument('--xmlxsd',help=
        'create xml output and check with xmllint against xsd',action="store_true")
    parser.add_argument('--pdf',help='create LaTeX output and create pdf from it',
        action="store_true")
    parser.add_argument('--subdirs',help='use the configuration parameter CREATE_SUBDIRS=YES',
        action="store_true")
    parser.add_argument('--clang',help='use CLANG_ASSISTED_PARSING, works only when '
                'doxygen has been compiled with "use_libclang"',
        action="store_true")
    parser.add_argument('--keep',help='keep result directories',
        action="store_true")
    parser.add_argument('--cfg',nargs='+',dest='cfgs',action='append',help=
        'run test with extra doxygen configuration settings '
        '(the option may be specified multiple times)')

    test_flags = split_and_keep(os.getenv('TEST_FLAGS', default=''), '--')

    args = parser.parse_args(test_flags + sys.argv[1:])

    # sanity check
    if (not args.xml) and (not args.pdf) and (not args.xhtml) and (not args.qhp) and (not args.docbook and (not args.rtf) and (not args.xmlxsd)):
        args.xml=True
    if (not args.updateref is None) and (args.ids is None) and (args.all is None):
        parser.error('--updateref requires either --id or --all')

    starting_directory = os.getcwd()
    os.chdir(args.inputdir)
    # find the tests to run
    tests = []
    if args.start_id:
        if args.end_id:
            for id in range(args.start_id, args.end_id + 1):
                tests.append(glob.glob('%03d_*'%int(id)))
        else:
            parser.error('--start_id requires --end_id')
    elif args.end_id:
        parser.error('--end_id requires --start_id')
    if args.ids:  # test ids are given by user
        for id in list(itertools.chain.from_iterable(args.ids)):
            tests.append(glob.glob('%03d_*'%int(id)))
    if (not args.ids and not args.start_id):  # find all tests
        tests = sorted(glob.glob('[0-9][0-9][0-9]_*'))
    else:
        tests = list(itertools.chain.from_iterable(tests))

    if args.exclude_ids:  # test ids are given by user
        for id in list(itertools.chain.from_iterable(args.exclude_ids)):
            x=glob.glob('%03d_*'%int(id))
            if len(x):
              tests.remove(x[0])
    os.chdir(starting_directory)

    # create test manager to run the tests
    testManager = TestManager(args,tests)
    sys.exit(testManager.perform_tests())

if __name__ == '__main__':
    main()
