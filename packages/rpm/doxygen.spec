Name: doxygen
Version: 1.2.10_20010923
Summary: documentation system for C, C++ and IDL
Release: 4
Source: doxygen-%{version}.src.tar.gz

Copyright: GPL
Group: unsorted
URL: http://www.stack.nl/~dimitri/doxygen
Packager: Matthias Andree <ma@dt.e-technik.uni-dortmund.de>
BuildRoot: /var/tmp/doxygen-%{version}.root

# requires Perl for tmake (Troll's make)
BuildPrereq: perl tetex

%description
Doxygen is a documentation system for C and C++. It can generate an
on-line class browser (in HTML) and/or an off-line reference manual
(in LaTeX) from a set of documented source files. The documentation is
extracted directly from the sources. Doxygen is developed on a Linux
platform, but it runs on most other UNIX flavors as well. An
executable for Windows 95/NT is also available.

Author:
--------
    Dimitri van Heesch <dimitri@stack.nl>

%description -l de
Doxygen ist ein Dokumentationssystem für C und C++. Es kann eine
Klassenübersicht (in HTML) und/oder eine Referenz (in LaTeX) aus
dokumentierten Quelldateien erzeugen. Die Dokumentation wird direkt aus
den Quellen extrahiert. Doxygen wird auf einer Linux-Plattform
entwickelt, funktioniert aber genauso auf den meisten andern Unix
Dialekten. Das Programm ist auch für Windows 95/NT erhältlich.

Autor:
--------
    Dimitri van Heesch <dimitri@stack.nl>

%prep
%setup -n doxygen-%{version}

%build
CFLAGS="$RPM_OPT_FLAGS" ./configure --with-doxywizard 
# the next path is Suse specific
QTDIR=/usr/lib/qt2
PATH=${QTDIR}/bin:$PATH
export QTDIR PATH
make

%install
rm -rf $RPM_BUILD_ROOT
make install install_docs INSTALL=$RPM_BUILD_ROOT/usr \
                          DOCDIR=$RPM_BUILD_ROOT%{_docdir}/doxygen
install -m 644 LICENSE LANGUAGE.HOWTO PLATFORMS README VERSION \
        $RPM_BUILD_ROOT%{_docdir}/doxygen

find $RPM_BUILD_ROOT -name CVS -type d -depth -exec rm -r {} \;

%files
%defattr(-,root,root)
%attr(755,root,root) /usr/bin/*
%doc %{_docdir}/doxygen

%clean
rm -rf $RPM_BUILD_ROOT

%changelog
* Sun Jun 10 2001 Matthias Andree <ma@dt.e-technik.uni-dortmund.de>
 - update to 1.2.8.1
* Tue Jun 5 2001 Matthias Andree <ma@dt.e-technik.uni-dortmund.de>
 - update to 1.2.8
 - enable XML-Generator
* Mon Apr 16 2001 Jens Seidel <jensseidel@users.sourceforge.net>
 - new decription (english, german)
 - use of %{_docdir}
 - added README, LICENSE, ... to install section
* Mon Mar 13 2000 Matthias Andree <ma@dt.e-technik.uni-dortmund.de>
 - inital version build with rpmify
