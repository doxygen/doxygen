#! python2

from __future__ import print_function

import os
import re
import shutil
import subprocess
import sys
import textwrap


def gitSHA_date_time():
    cmd = 'git rev-parse --short HEAD'
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE)
    output = p.communicate()[0]
    output = output.decode('ASCII')
    p.wait()
    sha = output.strip()

    cmd = 'git show -s --format="%ci" ' + sha
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE)
    output = p.communicate()[0]
    output = output.decode('ASCII')
    p.wait()
    date = output.strip()
    lst = date.split()               # string like '2013-06-21 09:23:47 +0200'
    dstamp = lst[0].replace('-', '') # '20130621' for the date
    tstamp = lst[1].replace(':', '') # '092347' for the time

    return sha, dstamp, tstamp


def getDoxygenVersion():
    # ... from the VERSION file.
    sdir, fname = getThisScriptPathAndName()
    version_fname = os.path.join(sdir, '..', 'VERSION')

    with open(version_fname) as f:
        lst = f.readlines()

    doxver = lst[0].strip()

    m = re.match(r'^(?P<ver>[0-9.]+)(-(?P<date>\d{8}))?', doxver)
    assert m
    ver = m.group('ver')
    return ver


def getThisScriptPathAndName():
    script_fname = os.path.realpath(__file__)
    sdir, fname = os.path.split(script_fname)
    return sdir, fname


def getEmptyDistribDir():
    # Get this script full path, name, and the script subdir name
    # (for checking the location).
    sdir, fname = getThisScriptPathAndName()
    subdir = os.path.basename(sdir)
    assert subdir == 'winbuild'

    # The distribution directory will be a subdirectory of the "../__put"
    # (created if it does not exist, not the part of the git repo).
    target_dir = os.path.normpath(os.path.join(sdir, '..', '__put'))
    if not os.path.exists(target_dir):
        os.mkdir(target_dir)
    assert os.path.isdir(target_dir)

    # The distribution subdir is composed out of 'Doxygen-', version stamp,
    # timestamp, and commit id (partial SHA). Ignore the date from the VERSION
    # file, take the commit date.
    ver = getDoxygenVersion()
    sha, dstamp, tstamp = gitSHA_date_time()
    dist_subdir = 'Doxygen-' + ver + '-' + dstamp + tstamp
    dist_dir = os.path.join(target_dir, dist_subdir)
    print(dist_dir)
    if os.path.isdir(dist_dir):
        print("Removing the existing '{}'".format(dist_dir))
        shutil.rmtree(dist_dir)
    assert not os.path.exists(dist_dir)
    print("Creating the new '{}'".format(dist_dir))
    os.mkdir(dist_dir)
    assert os.path.isdir(dist_dir)

    return dist_dir


def copyBinaries(dist_dir, subdir):
    '''Copy the Windows binaries (doxygen.exe only) to the dist_dir directory.'''

    # Source file should exist.
    sdir, fname = getThisScriptPathAndName()
    src = os.path.normpath(os.path.join(sdir, '..', 'bin', subdir, 'doxygen.exe'))
    assert os.path.isfile(src)

    # Destination directory must not exist. It must be created first.
    dst_dir = os.path.normpath(os.path.join(dist_dir, 'bin', subdir))
    assert not os.path.isdir(dst_dir)
    os.makedirs(dst_dir)

    # Copy the file.
    print("Copying '{}'".format(src))
    shutil.copy2(src, dst_dir)


def getBinariesZipBareName():
    ver = getDoxygenVersion()
    sha, dstamp, tstamp = gitSHA_date_time()
    fname = 'doxygenw{}_{}.zip'.format(dstamp, ver.replace('.', '_'))
    return fname


def getTranslatorReportZipBareName():
    ver = getDoxygenVersion()
    sha, dstamp, tstamp = gitSHA_date_time()
    fname = 'tr{}_{}.zip'.format(dstamp, ver.replace('.', '_'))
    return fname


def zipBinaries(distr_dir):
    # Build the zip filename. It is to be located at the same level as distr_dir.
    zip_bare_name = getBinariesZipBareName()
    dst, distr_subdir = os.path.split(distr_dir)
    zip_full_name = os.path.join(dst, zip_bare_name)

    if os.path.isfile(zip_full_name):
        print("Removing the existing '{}'".format(zip_full_name))
        os.remove(zip_full_name)

    # Change the working directory to destination directory and zip from
    # there using the bare names so that the full path is not zipped inside.
    wd = os.getcwd()
    os.chdir(dst)
    print("Zipping new '{}'".format(zip_full_name))
    subprocess.call('zip -r {} {}'.format(zip_bare_name, distr_subdir), shell=True)
    os.chdir(wd)  # back to the original working directory


def buildAndZipTranslatorReport(distr_dir):
    # Build the translator report zip filename. It is to be located at the same
    # level as distr_dir.
    zip_bare_name = getTranslatorReportZipBareName()
    dst, subdir = os.path.split(distr_dir)
    zip_full_name = os.path.join(dst, zip_bare_name)

    if os.path.isfile(zip_full_name):
        print("Removing the existing '{}'".format(zip_full_name))
        os.remove(zip_full_name)
    print("Zipping new '{}'".format(zip_full_name))

    # Change the working directory to the doc one and generate
    # the translator report.
    sdir, fname = getThisScriptPathAndName()
    docdir = os.path.join(sdir, '..', 'doc')
    assert os.path.isdir(docdir)
    wd = os.getcwd()
    os.chdir(docdir)
    subprocess.call('python translator.py', shell=True)

    # Zip the generated translator_report.txt.
    subprocess.call('zip -r {} {}'.format(zip_full_name,
                                          'translator_report.txt'), shell=True)

    os.chdir(wd)  # back to the original working directory


def mailto():

    # Information for the letter.
    ver = getDoxygenVersion()
    sha, dstamp, tstamp = gitSHA_date_time()
    doxzipname = getBinariesZipBareName()
    trzipname = getTranslatorReportZipBareName()

    subject = 'Windows binaries available for {}-{} at SourceForge'.format(ver, dstamp)
    subject = subject.replace(' ', '%20')

    body = textwrap.dedent('''\
        Hi,

        If interested, you can download the doxygen binaries
        compiled for MS Windows from

          http://sourceforge.net/projects/doxygen/files/snapshots/doxygen-1.8-svn/windows

        This is the place where you should find also the next
        releases.  Name of the archive file is

          {}

        The related translator report can be found inside the directory

          http://sourceforge.net/projects/doxygen/files/snapshots/doxygen-1.8-svn/translator_reports/

        Name of the archive file is

          {}

        The binaries are NOT created automatically, so it may
        happen that some newer sources were not compiled
        because I am not present to do that or I forgot... ;)

        Regards,
            Petr

        --
        Petr Prikryl (prikryl at atlas dot cz)''').format(doxzipname, trzipname)
    body = body.replace('\n', '%0d')

    # Make the mailto URI and launch the mailer.
    to_addr = 'doxygen-users@lists.sourceforge.net'
    mailtoURI = 'mailto:%s?subject=%s&body=%s' % (to_addr, subject, body)
    os.startfile(mailtoURI)


if __name__ == '__main__':
    # Create the empty directory for the distribution files.
    dist_dir = getEmptyDistribDir()

    # Copy the compiled binaries to the distribution directory and zip them.
    copyBinaries(dist_dir, 'Debug')
    copyBinaries(dist_dir, 'Debug64')
    copyBinaries(dist_dir, 'Release')
    copyBinaries(dist_dir, 'Release64')
    zipBinaries(dist_dir)

    # The translator report...
    buildAndZipTranslatorReport(dist_dir)

    # Launch the mailer with the generated message body.
    mailto()