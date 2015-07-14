# set CPack properties
# This file has been inspired from:
# - http://sourceforge.net/p/klusters/klusters/ci/5a65c7732890d371ca6c74a6167b13c995c54bc3/tree/CMakeLists.txt
# - http://www.cmake.org/Wiki/CMake:CPackPackageGenerators
# - http://www.cmake.org/cmake/help/v3.3/module/CPack.html
# TODO: add doxywizard in package

set(CPACK_PACKAGE_NAME ${PROJECT_NAME} )
set(CPACK_PACKAGE_VERSION ${VERSION})
set(CPACK_PACKAGE_CONTACT "Dimitri van Heesch")
set(CPACK_PACKAGE_VENDOR "Dimitri van Heesch")
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
set(CPACK_RESOURCE_FILE_README  ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Generate documentation from C, C++, Java, Python and other languages")
set(CPACK_PACKAGE_DESCRIPTION "Doxygen is the de facto standard tool for generating documentation from annotated C++ sources.
But it also supports other popular programming languages such as C, Objective-C, C#, PHP, Java, Python, Fortran, VHDL, Tcl, D (some extent) and IDL (Corba, Microsoft, and UNO/OpenOffice flavors).

Doxygen can be used in three ways:

  1. Generate documentation from annotated source files in various format: on-line documentation (HTML), off-line reference manual (LaTeX), and also: RTF (MS-Word), PostScript, hyperlinked PDF, compressed HTML, and Unix man pages.
  2. Extract the code structure from undocumented source files. This is very useful to quickly find your way in large source distributions. Doxygen can also visualize the relations between the various elements by means of include dependency graphs, inheritance diagrams, and collaboration diagrams, which are all generated automatically.
  3. Create normal documentation (as the doxygen user manual and web-site http://doxygen.org/).

Install the doxygen-latex package to build LaTeX based documents.
Install the libclang1 package to use the 'clang assisted parsing'.")
set(CPACK_RPM_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION})
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Matthias Klose <doko@debian.org>")
set(CPACK_RPM_PACKAGE_LICENSE       "GPLv2")
set(CPACK_DEBIAN_PACKAGE_SECTION    "devel")
set(CPACK_RPM_PACKAGE_GROUP         "Development/Tools")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE   "http://doxygen.org/")
set(CPACK_RPM_PACKAGE_URL           "http://doxygen.org/")
set(CPACK_DEBIAN_PACKAGE_DEPENDS    "libc0.1 (>= 2.17-91) [kfreebsd-amd64, kfreebsd-i386], libc0.3 (>= 2.12) [hurd-i386], libc6 (>= 2.11) [hppa], libc6 (>= 2.14) [amd64], libc6 (>= 2.16) [x32], libc6 (>= 2.17) [arm64, ppc64el], libc6 (>= 2.19) [sh4], 	libc6 (>= 2.4) [armel, armhf, i386, m68k, mips, mipsel, s390x, sparc64], libc6 (>= 2.6) [powerpc, powerpcspe, ppc64, sparc], libc6.1 (>= 2.4) [alpha], 	libclang1-3.5 (>= 3.2) [not hppa, hurd-i386, m68k, ppc64, sh4, sparc, sparc64, x32], libgcc1 [x32], libgcc1 (>= 1:4.1.1) [not arm64, armel, armhf, hppa, hurd-i386, m68k, x32], libgcc1 (>= 1:4.4.0) [armel, armhf], libgcc1 (>= 1:4.7) [arm64], libgcc1 (>= 4.2.1) [hurd-i386], libgcc2 (>= 4.2.1) [m68k], libgcc4 (>= 4.1.1) [hppa], libsqlite3-0 (>= 3.6.0) [not ppc64], libstdc++6 [x32], libstdc++6 (>= 4.1.1) [ppc64], libstdc++6 (>= 4.9) [not ppc64, x32], libxapian22 [not ppc64]")
set(CPACK_RPM_PACKAGE_REQUIRES      "/sbin/chkconfig, /bin/mktemp, /bin/rm, /bin/mv, libstdc++ >= 2.96")
set(CPACK_DEBIAN_PACKAGE_SUGGESTS   "doxygen-latex, doxygen-doc, doxygen-gui, graphviz, libclang1")
set(CPACK_RPM_PACKAGE_SUGGESTS      "doxygen-latex, doxygen-doc, doxygen-gui, graphviz, libclang1")
set(CPACK_DEBIAN_PACKAGE_CONFLICTS  "graphviz (<< 1.12)")

if(WIN32)
  set(CPACK_GENERATOR "NSIS")

elseif(APPLE)
    set( CPACK_GENERATOR "ZIP,PackageMaker,Bundle" )
    set( CPACK_DMG_VOLUME_NAME ${PROJECT_NAME} )
    set( CPACK_SYSTEM_NAME "OSX" )

elseif(UNIX)
    # Determine distribution and release
    execute_process(COMMAND lsb_release -si OUTPUT_VARIABLE distribution                   OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND lsb_release -sc OUTPUT_VARIABLE release                        OUTPUT_STRIP_TRAILING_WHITESPACE)
    #xecute_process(COMMAND uname -i        OUTPUT_VARIABLE CPACK_RPM_PACKAGE_ARCHITECTURE OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND uname -m        OUTPUT_VARIABLE CPACK_RPM_PACKAGE_ARCHITECTURE OUTPUT_STRIP_TRAILING_WHITESPACE)

    if(distribution STREQUAL "Debian" OR distribution STREQUAL "Ubuntu")
      set(CPACK_GENERATOR "DEB")
      execute_process(COMMAND dpkg --print-architecture OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE OUTPUT_STRIP_TRAILING_WHITESPACE)
      set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}.${CPACK_DEBIAN_PACKAGE_ARCHITECTURE})

    elseif(distribution MATCHES "openSUSE.*")
      set(CPACK_GENERATOR "RPM")
      set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${release}.${CPACK_RPM_PACKAGE_ARCHITECTURE})

    elseif(distribution STREQUAL "Fedora")
      set(CPACK_GENERATOR "RPM")
      set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}.fc${release}.${CPACK_RPM_PACKAGE_ARCHITECTURE})

    elseif(distribution STREQUAL "Scientific")
      set(CPACK_GENERATOR "RPM")
      set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${release}.${CPACK_RPM_PACKAGE_ARCHITECTURE})

    else()
      set(CPACK_GENERATOR "RPM;TGZ;STGZ")
      set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${release}.${CPACK_RPM_PACKAGE_ARCHITECTURE})
    endif()

else()
    # other operating system (not Windows/Apple/Unix)
endif()
