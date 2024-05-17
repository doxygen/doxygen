# Doxyparse

This directory contains an "source parsing engine" based on doxyapp code.

Doxyparse modifies the default output of Doxygen and dumps the dependencies
among code elements in a YAML format, instead of output it in a human-readable
format, as Doxygen does Doxyparse's output is intended to produce a
machine-readable output.

Doxyparse has been used in many software engineering research (as a source-code
static analysis tool) regards on software metrics, quality metrics and so on,
Doxyparse was first used by the [Analizo](https://www.analizo.org) toolkit, a suite
of source code analysis tools, aimed at being language-independent and
extensible, able to extract and calculate a fair number of source code metrics,
generate dependency graphs, and other software evolution analysis.

Academic publications citing Doxyparse:
* https://scholar.google.com.br/scholar?q=doxyparse

## build dependencies

    apt-get install flex bison cmake build-essential python

## build

    cmake -G "Unix Makefiles" -Dbuild_parse=ON
    make

## install

    sudo make install

## release

* ensure analizo testsuite passing on newer doxyparse version
* update debian/changelog, commit, push
* create git tag, push to github analizo/doxyparse
* build on amd64 and i386 archs, upload tar.gz to github
  * `tar -zcf doxyparse_<VERSION>_amd64.tar.gz -C bin/ doxyparse`
  * `tar -zcf doxyparse_<VERSION>_i386.tar.gz -C bin/ doxyparse`
* build debian packages for amd64 and i386, update analizo.org repository
    * (see analizo.github.io/README.md file for updating repository instructions)
    * upload the deb files to github release tag also
* check if a alien-doxyparse release is necessary and do it on cpan

## Authors

* Antonio Terceiro <terceiro@softwarelivre.org>
* João M. Miranda <joaomm88@gmail.com>
* Joenio Costa <joenio@joenio.me>
* Paulo Meirelles <paulo@softwarelivre.org>
* Vinicius Daros <vkdaros@mercurio.eclipse.ime.usp.br>
