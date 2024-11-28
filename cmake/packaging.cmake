##### set CPack properties #####
#
# Good doc/tutorial/example:
# - https://gitlab.kitware.com/cmake/community/-/wikis/doc/cpack/PackageGenerators
# - https://www.cmake.org/cmake/help/v3.3/module/CPack.html
# - https://sourceforge.net/p/klusters/klusters/ci/master/tree/CMakeLists.txt
#
# This cmake script should generate same packages (deb,rpm) as:
# - https://mirror.debian.ikoula.com/debian/pool/main/d/doxygen
# - http://archive.ubuntu.com/ubuntu/pool/main/d/doxygen (http://old-releases.ubuntu.com/ubuntu/pool/main/d/doxygen)
# - https://rpmfind.net/linux/rpm2html/search.php?query=doxygen

set(CPACK_STRIP_FILES      ON)
set(CPACK_PACKAGE_NAME     ${PROJECT_NAME} )
set(CPACK_PACKAGE_VERSION  ${VERSION})
set(CPACK_PACKAGE_CONTACT  "Dimitri van Heesch")
set(CPACK_PACKAGE_VENDOR   "Dimitri van Heesch")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Generate documentation from C, C++, Java, Python and other languages")
set(CPACK_PACKAGE_DESCRIPTION "Doxygen is the de facto standard tool for generating documentation from annotated C++ sources.
 But many other popular programming languages are supported:
 C, Objective-C, C#, PHP, Java, Python, Fortran, D (some extent), and IDL (Corba, Microsoft, and UNO/OpenOffice flavors).
 Doxygen also supports the hardware description language VHDL.
 .
 Three usages:
 .
    1. Generate documentation from annotated source files to various format:
       - On-line documentation (HTML)
       - Off-line reference manual (LaTeX, RTF, PostScript, hyperlinked PDF, compressed HTML, Unix man pages)
 .
    2. Extract the code structure from undocumented source files.
       Also generate include dependency graphs, inheritance diagrams, and collaboration diagrams.
       Useful to quickly understand code organization in large source distributions.
 .
    3. Create normal documentation (as the doxygen user manual and web-site http://doxygen.org/)
 .
 Install the doxygen-latex package to build LaTeX based documents.
 Install the libclang1 package to use the 'clang assisted parsing'.")

set(CPACK_RESOURCE_FILE_LICENSE     ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
set(CPACK_RESOURCE_FILE_README      ${CMAKE_CURRENT_SOURCE_DIR}/README.md)

# Variables specific to CPack RPM generator
set(CPACK_RPM_PACKAGE_DESCRIPTION   ${CPACK_PACKAGE_DESCRIPTION})
set(CPACK_RPM_PACKAGE_LICENSE       "GPLv2")
set(CPACK_RPM_PACKAGE_GROUP         "Development/Tools")
set(CPACK_RPM_PACKAGE_URL           "https://doxygen.org/")
set(CPACK_RPM_PACKAGE_REQUIRES      "/sbin/chkconfig, /bin/mktemp, /bin/rm, /bin/mv, libstdc++ >= 2.96")
set(CPACK_RPM_PACKAGE_SUGGESTS      "doxygen-latex, doxygen-doc, doxygen-gui, graphviz, libclang1")

# Variables specific to CPack DEB generator
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION})
set(CPACK_DEBIAN_PACKAGE_SECTION     "devel")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE    "https://doxygen.org/")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS   YES) #set(CPACK_DEBIAN_PACKAGE_DEPENDS    "libc6, libclang1-3.6, libgcc1, libstdc++6, libxapian22")
set(CPACK_DEBIAN_PACKAGE_SUGGESTS    "doxygen-latex, doxygen-doc, doxygen-gui, graphviz, libclang1")
set(CPACK_DEBIAN_PACKAGE_CONFLICTS   "graphviz (<< 1.12)")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER  "Matthias Klose <doko@debian.org>")  # Ubuntu Developers <ubuntu-devel-discuss@lists.ubuntu.com>

# Variables specific to CPack NSIS generator
set(CPACK_NSIS_MUI_ICON         ${CMAKE_CURRENT_SOURCE_DIR}/addon/doxywizard/doxywizard.ico)
set(CPACK_NSIS_URL_INFO_ABOUT   "https://doxygen.org/")
set(CPACK_NSIS_PACKAGE_NAME     ${PROJECT_NAME})

# Variables specific to CPack DragNDrop generator
set(CPACK_DMG_FORMAT            "UDBZ")         # UDRO=UDIF-Read-Only, UDZO=zlib, UDBZ=bzip2 -- See hdiutil
set(CPACK_DMG_VOLUME_NAME       ${PROJECT_NAME})
set(CPACK_DMG_BACKGROUND_IMAGE  ${CMAKE_CURRENT_SOURCE_DIR}/doc/doxygen_logo.eps)

if(WIN32)
    set(CPACK_GENERATOR "ZIP;NSIS")

elseif(APPLE)
    set(CPACK_GENERATOR       "ZIP;DragNDrop;PackageMaker;Bundle" )
    set(CPACK_SYSTEM_NAME     "OSX" )

elseif(UNIX)
    # Determine distribution and release
    execute_process(COMMAND lsb_release -si OUTPUT_VARIABLE distribution                   OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND lsb_release -sc OUTPUT_VARIABLE release                        OUTPUT_STRIP_TRAILING_WHITESPACE)
    #xecute_process(COMMAND uname -i        OUTPUT_VARIABLE CPACK_RPM_PACKAGE_ARCHITECTURE OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND uname -m        OUTPUT_VARIABLE CPACK_RPM_PACKAGE_ARCHITECTURE OUTPUT_STRIP_TRAILING_WHITESPACE)

    if(distribution STREQUAL "Debian" OR distribution STREQUAL "Ubuntu")
      set(CPACK_GENERATOR "DEB")
      execute_process(COMMAND dpkg --print-architecture OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE OUTPUT_STRIP_TRAILING_WHITESPACE)
      set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}_${CPACK_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE})

    elseif(distribution MATCHES "RedHat.*")
      # extract the major version from RedHat full version (e.g. 6.7 --> 6)
      execute_process(COMMAND lsb_release -sr COMMAND sed s/[.].*//  OUTPUT_VARIABLE redhat_version_major OUTPUT_STRIP_TRAILING_WHITESPACE)
      set(CPACK_GENERATOR "RPM")
      set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_RPM_PACKAGE_RELEASE}.el${redhat_version_major}.${CPACK_RPM_PACKAGE_ARCHITECTURE})

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
