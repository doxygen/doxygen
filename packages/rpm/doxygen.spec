Summary: A documentation system for C/C++.
Name: doxygen
Version: 1.3_rc1
Release: 1
Epoch: 1
Source0: ftp://ftp.stack.nl/pub/users/dimitri/%{name}-%{version}.src.tar.gz
Patch: doxygen-1.2.7-redhat.patch
Patch1: doxygen-1.2.12-qt2.patch
Group: Development/Tools
License: GPL
Url: http://www.stack.nl/~dimitri/doxygen/index.html
Prefix: %{_prefix}
BuildPrereq: libstdc++-devel >= 2.96, /usr/bin/perl
BuildRoot: %{_tmppath}/%{name}-%{version}-root

%description
Doxygen can generate an online class browser (in HTML) and/or a
reference manual (in LaTeX) from a set of documented source files. The
documentation is extracted directly from the sources. Doxygen can
also be configured to extract the code structure from undocumented
source files.

%package doxywizard
Summary: A GUI for creating and editing configuration files.
Group: User Interface/X
Requires: %{name} = %{version}
BuildPrereq: qt-devel => 2.3.0
Requires: qt >= 2.3.0

%description doxywizard
Doxywizard is a GUI for creating and editing configuration files that
are used by doxygen.

%prep
%setup -q
%patch -p1 -b .redhat
%patch1 -p1 -b .qt2

%build
QTDIR="" && . /etc/profile.d/qt.sh
export OLD_PO_FILE_INPUT=yes

./configure --prefix %{_prefix} --shared --release --with-doxywizard
make all docs

%install
rm -rf ${RPM_BUILD_ROOT}

export OLD_PO_FILE_INPUT=yes
make install INSTALL=$RPM_BUILD_ROOT%{_prefix}

%clean
rm -rf ${RPM_BUILD_ROOT}

%files
%defattr(-,root,root)
%doc LANGUAGE.HOWTO README examples html
%{_bindir}/doxygen
%{_bindir}/doxysearch
%{_bindir}/doxytag

%files doxywizard
%defattr(-,root,root)
%{_bindir}/doxywizard

%changelog
* Sun Jan 06 2002 Than Ngo <than@redhat.com> 1.2.13.1-1
- update to 1.2.13.1

* Sun Dec 30 2001 Jeff Johnson <jbj@redhat.com> 1.2.13-1
- update to 1.2.13

* Sun Nov 18 2001 Than Ngo <than@redhat.com> 1.2.12-1
- update to 1.2.12
- s/Copyright/License

* Wed Sep 12 2001 Tim Powers <timp@redhat.com>
- rebuild with new gcc and binutils

* Wed Jun 13 2001 Than Ngo <than@redhat.com>
- update tp 1.2.8.1
- make doxywizard as separat package
- fix to use install as default

* Tue Jun 05 2001 Than Ngo <than@redhat.com>
- update to 1.2.8

* Tue May 01 2001 Than Ngo <than@redhat.com>
- update to 1.2.7
- clean up specfile
- patch to use RPM_OPT_FLAG

* Wed Mar 14 2001 Jeff Johnson <jbj@redhat.com>
- update to 1.2.6

* Wed Feb 28 2001 Trond Eivind Glomsrød <teg@redhat.com>
- rebuild

* Tue Dec 26 2000 Than Ngo <than@redhat.com>
- update to 1.2.4
- remove excludearch ia64
- bzip2 sources

* Mon Dec 11 2000 Than Ngo <than@redhat.com>
- rebuild with the fixed fileutils

* Mon Oct 30 2000 Jeff Johnson <jbj@redhat.com>
- update to 1.2.3.

* Sun Oct  8 2000 Jeff Johnson <jbj@redhat.com>
- update to 1.2.2.
- enable doxywizard.

* Sat Aug 19 2000 Preston Brown <pbrown@redhat.com>
- 1.2.1 is latest stable, so we upgrade before Winston is released.

* Wed Jul 12 2000 Prospector <bugzilla@redhat.com>
- automatic rebuild

* Tue Jul  4 2000 Jakub Jelinek <jakub@redhat.com>
- Rebuild with new C++

* Fri Jun 30 2000 Florian La Roche <laroche@redhat.de>
- fix QTDIR detection

* Fri Jun 09 2000 Preston Brown <pbrown@redhat.com>
- compile on x86 w/o optimization, revert when compiler fixed!!

* Wed Jun 07 2000 Preston Brown <pbrown@redhat.com>
- use newer RPM macros

* Tue Jun  6 2000 Jeff Johnson <jbj@redhat.com>
- add to distro.

* Tue May  9 2000 Tim Powers <timp@redhat.com>
- rebuilt for 7.0

* Wed Feb  2 2000 Bernhard Rosenkraenzer <bero@redhat.com>
- recompile with current Qt (2.1.0/1.45)

* Wed Jan  5 2000 Jeff Johnson <jbj@redhat.com>
- update to 1.0.0.
- recompile with qt-2.0.1 if available.
- relocatable package.

* Mon Nov  8 1999 Tim Powers <timp@redhat.com>
-updated to 0.49-991106

* Tue Jul 13 1999 Tim Powers <timp@redhat.com>
- updated source
- cleaned up some stuff in the spec file

* Thu Apr 22 1999 Jeff Johnson <jbj@redhat.com>
- Create Power Tools 6.0 package.
