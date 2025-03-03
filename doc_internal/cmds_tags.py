#!/usr/bin/env python

from __future__ import print_function
import re
import os
import sys
import subprocess
import shlex


cmds_reg = re.compile(r'.refitem *cmd[a-z]* *')
tags_obsolete_reg = re.compile(r"^.*option *type=.obsolete.*id='([^']*)'.*")
tags_active_reg = re.compile(r"^.*option .*id='([^']*)'.*")


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


def get_versions():
    raw_versions = xpopen('git tag')
    versions = []
    strip_versions = []
    versions.append("Release_1_8_20")

    pre = "Release_"
    for o in shlex.split(raw_versions):
        if o.startswith(pre):
          ver = o.split('_')
          v = int(ver[1])*1000+int(ver[2])
          if v >= 1009:
              strip_versions.append(v*1000+int(ver[3]))
    for i in sorted(set(strip_versions)):
        versions.append(pre + str(int((i/1000)/1000)) + "_" + str(int((i/1000) % 1000)) + "_" + str(i%1000))

    versions.append("master")
    print(versions)
    return versions


def get_commands(version):
    lst = xpopen('git show %s:doc/commands.dox'%(version))
    if lst.count('\n') == 0:
        lst = xpopen('git show %s:doc/commands.doc'%(version))
    lst_list = []
    for line in lst.split('\n'):
        if cmds_reg.match(line):
            lst_list.append(re.sub(cmds_reg,'',line).replace("\\\\","\\"))
        elif line.startswith("\\endsecreflist"):
            break
    return lst_list


def process_commands(old_version,new_version):
    ver = xpopen('git show %s:VERSION'%(new_version)).rstrip()

    old_list = get_commands(old_version)
    new_list = get_commands(new_version)
    hits = []
    for n in new_list:
        toadd = True
        for o in old_list:
            if n == o:
                toadd = False
        if toadd:
            hits.append(n)

    cmds = ""
    first = True
    for o in hits:
        if first:
            cmds += "<dt>New in %s</dt><dd>\\%s</dd>"%(ver,o)
        else:
            cmds += "\n<dt></dt>             <dd>\\%s</dd>"%(o)
        first = False
    if not first:
        cmds += "\n"
    return cmds


def get_tags(version):
    lst = xpopen('git show %s:src/config.xml'%(version))
    lst_act = []
    lst_obs = []
    for line in lst.split('\n'):
        if tags_obsolete_reg.match(line):
            lst_obs.append(re.sub(tags_obsolete_reg,'\\1',line))
        elif tags_active_reg.match(line):
            lst_act.append(re.sub(tags_active_reg,'\\1',line))

    return (sorted(lst_act),sorted(lst_obs))


def process_tags(old_version,new_version):
    ver = xpopen('git show %s:VERSION'%(new_version)).rstrip()

    (old_active,old_obsolete) = get_tags(old_version)
    (new_active,new_obsolete) = get_tags(new_version)
    hits_active = []
    hits_obsolete = []
    for n in new_active:
        toadd = True
        for o in old_active:
            if n == o:
                toadd = False
        if toadd:
            hits_active.append(n)

    for n in new_obsolete:
        toadd = True
        for o in old_obsolete:
            if n == o:
                toadd = False
        if toadd:
            hits_obsolete.append(n)

    tags= ""
    if len(hits_active) != 0:
        tags+= "<dt>New in %s</dt>\n"%(ver)
        for o in hits_active:
            tags+= "<dt></dt>              <dd>%s</dd>\n"%(o)
    if len(hits_obsolete) != 0:
        tags+= "<dt>Obsolete in %s</dt>\n"%(ver)
        for o in hits_obsolete:
            tags+= "<dt></dt>              <dd>%s</dd>\n"%(o)
    return tags


def replace_placeholder(cmds,inp_file,outp_file):
    if (os.path.exists(inp_file)):
        with xopen(outp_file,"w") as out_file:
            with xopen(inp_file) as in_file:
                for line in in_file:
                    if line.startswith("<!-- PLACEHOLDER -->"):
                        out_file.write(cmds)
                    else:
                        out_file.write(line)


def main():
    git_working_dir = sys.argv[2]
    inp_file = sys.argv[3]
    outp_file = sys.argv[4]

    # set the working directory (to get git information)
    old_working_dir = os.getcwd()
    os.chdir(git_working_dir)

    versions = get_versions()
    if sys.argv[1] == "-cmds":
        cmds = ""
        for i in range(1,len(versions)):
            cmds += process_commands(versions[i-1],versions[i])
        replace_placeholder(cmds,inp_file,outp_file)

    elif sys.argv[1] == "-tags":
        tags = ""
        for i in range(1,len(versions)):
            tags += process_tags(versions[i-1],versions[i])
            pass
        replace_placeholder(tags,inp_file,outp_file)

    os.chdir(old_working_dir)

if __name__ == '__main__':
    main()
