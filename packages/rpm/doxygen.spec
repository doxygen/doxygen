Name: doxygen
Version: 1.1.4-20000625
Summary: documentation system for C, C++ and IDL
Release: 1
Source0: doxygen-%{version}.src.tar.gz

Copyright: GPL
Group: unsorted
URL: http://www.stack.nl/~dimitri/doxygen
Packager: Matthias Andree <ma@dt.e-technik.uni-dortmund.de>
BuildRoot: /var/tmp/doxygen-%{version}.root

# requires Perl for tmake (Troll's make)
BuildPrereq: perl tetex

%define prefix /usr

%description


%prep
%setup -n doxygen-%{version}



%build
CFLAGS="$RPM_OPT_FLAGS" ./configure --prefix %{prefix} --with-doxywizard
make

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT%{prefix}/usr

# install
make INSTALL=$RPM_BUILD_ROOT%{prefix} install
( cd $RPM_BUILD_ROOT
  mkdir -p ./usr/doc/packages
  mv .%{prefix}/doc/doxygen ./usr/doc/packages/doxygen
  mv .%{prefix}/doc/doxysearch ./usr/doc/packages/doxysearch
  mv .%{prefix}/doc/doxytag ./usr/doc/packages/doxytag
  mv .%{prefix}/doc/doxywizard ./usr/doc/packages/doxywizard
)

%files
%defattr(-,root,root)
%{prefix}/bin
# %doc LANGUAGE.HOWTO LICENSE PLATFORMS VERSION README 
%doc %{prefix}/doc/packages/doxygen

%clean
rm -rf $RPM_BUILD_ROOT

%changelog
* Mon Mar 13 2000 Matthias Andree <ma@dt.e-technik.uni-dortmund.de>
 - inital version build with rpmify
